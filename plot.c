// TODO: Make and remove temporal directories for video processing
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <omp.h>
#include "plot.h"


int VIDEO_PARALLEL_PROCESSES = 0;  // Controls omp threads for plotting video frames


FILE *popen_gnuplot(void)
{
    FILE *pipe = popen("gnuplot -persistent 2>&1", "w");
    if (!pipe) {
        puts("popen_gnuplot: Error.");
        exit(1);
    }
    return pipe;
}


void pipe_terminal_output(t_gnuplot *interface, const char *terminal, char *outfile)
{
    int NMAX = 1024;
    char buff[NMAX];
    char template[] = "set terminal %s enhanced font 'Arial,%d' size %d,%d enhanced\n set output '%s'\n";
    // char template[] = "set terminal %s color font 'Arial,%d' size %d,%d enhanced\n set output '%s'\n";
    int N = -1;

    N = snprintf(buff, NMAX, template, terminal, interface->font_size, 
                 interface->size[0], interface->size[1], outfile);

    if (N < 0 || N >= NMAX) {
        puts("gnuplot_plot: Error with snprintf.");
        exit(1);
    }

    fputs(buff, interface->pipe);
}


void pipe_command(t_gnuplot *interface, char *cmd)
{   // Adds newline \n by default
    if (interface->state != 0) {
        puts("Error! Cannot execute command when plotting is active. Execute all commands before drawing any element.");
        return;
    }
    fputs(cmd, interface->pipe);
    fputs("\n", interface->pipe);
}


void pipe_config_vargs(t_gnuplot *interface, va_list ap)
{   // Pipes configuration variable arguments which may be literal strings or an 
    // array of literal strings (NULL terminated, and marked with a sentinel 
    // GNUPLOT_ARRAY_MARKER). These can be in any order and as many as needed, but 
    // the last argument must always be NULL.
    char *cmd = va_arg(ap, char *);
    while(cmd != NULL) {
        if (cmd == GNUPLOT_ARRAY_MARKER) {
            char **cmd_array = va_arg(ap, char **);
            for (int ii=0; cmd_array[ii] != NULL; ii++) {
                pipe_command(interface, cmd_array[ii]);
            }
        } else {
            pipe_command(interface, cmd);
        }
        cmd = va_arg(ap, char *);
    }
}


void gnuplot_config_impl(t_gnuplot *interface, ...)
{
    va_list ap;
    va_start(ap, interface);
    pipe_config_vargs(interface, ap);
    va_end(ap);
}


void pipe_default_commands(t_gnuplot *interface)
{
    pipe_command(interface, "set parametric");
}


void activate_parallel_video_processing(int num_threads)
{
    if (num_threads <= 0) {
        puts("Error: num_threads must be > 0");
        exit(1);
    }
    VIDEO_PARALLEL_PROCESSES = num_threads;

    mkdir(TEMPLATES_TMP_DIR, 0755);
    mkdir(FRAMES_TMP_DIR, 0755);
}


void setup_video_output(t_gnuplot *interface)
{
    if (VIDEO_PARALLEL_PROCESSES == 0) {
        // We pipe each frame from gnuplot directly into ffmpeg as a png
        interface->pipe = popen_gnuplot();

        char buff[256];
        snprintf(buff, 256, "| ffmpeg -y -loglevel error -f png_pipe -r %d -s:v %dx%d -i pipe: -pix_fmt yuv420p -c:v libx264 -crf 18 %s", interface->framerate, interface->size[0], interface->size[1], interface->file_name);
        pipe_terminal_output(interface, "pngcairo", buff);

    } else if (VIDEO_PARALLEL_PROCESSES > 0) {
        // We write each frame into a gnuplot script file
        // In the end these templates will be executed in parallel to produce each frame as a png
        // ffmpeg will then make a video from them
        char buff[256];
        snprintf(buff, 256, "%s/frame_%04d.plt", TEMPLATES_TMP_DIR, interface->frame);
        interface->pipe = fopen(buff, "w");

        snprintf(buff, 256, "%s/frame_%04d.png", FRAMES_TMP_DIR, interface->frame);
        pipe_terminal_output(interface, "pngcairo", buff);
    } else {
        puts("Error: VIDEO_PARALLEL_PROCESSES < 0");
        exit(1);
    }
}


t_gnuplot *gnuplot_start_impl(enum gnuplot_type type, char *file_name, int size[2], int font_size, int framerate, ...) 
{
    t_gnuplot *interface = malloc(sizeof(t_gnuplot));
    interface->pipe = NULL;  // safeguard 
    interface->type = type;
    interface->dim = -1;  // safeguard 
    interface->file_name = strdup(file_name);
    interface->size[0] = size[0];
    interface->size[1] = size[1];
    interface->font_size = font_size;
    interface->framerate = framerate;
    interface->state = 0;
    interface->N_OMP = VIDEO_PARALLEL_PROCESSES;
    interface->frame = 1;

    switch (type) {
        case PNG_2D: {
            interface->dim = 2;
            interface->pipe = popen_gnuplot();
            pipe_terminal_output(interface, "pngcairo", file_name);
            break;
        }
        case PDF_2D: {
            interface->dim = 2;
            interface->pipe = popen_gnuplot();
            pipe_terminal_output(interface, "pdfcairo", file_name);
            break;
        }
        case EPS_2D: {
            interface->dim = 2;
            interface->pipe = popen_gnuplot();
            puts("TODO EPS");
            exit(1);
            break;
        }
       case PNG_3D: {
            interface->dim = 3;
            interface->pipe = popen_gnuplot();
            pipe_terminal_output(interface, "pngcairo", file_name);
            break;
        }
        case PDF_3D: {
            interface->dim = 3;
            interface->pipe = popen_gnuplot();
            pipe_terminal_output(interface, "pdfcairo", file_name);
            break;
        }
        case VIDEO_2D: {
            interface->dim = 2;
            setup_video_output(interface);
            break;
        }
        case VIDEO_3D: {
            interface->dim = 3;
            setup_video_output(interface);
            break;
        }
        default: {
            puts("UNIMPLEMENTED TYPE");
            exit(1);
            break;
        }
    }
    
    // Config:
    pipe_default_commands(interface);

    va_list ap;
    va_start(ap, framerate);
    pipe_config_vargs(interface, ap);
    va_end(ap);

    return interface;
}


void next_frame_impl(t_gnuplot *interface, ...)
{
    interface->state = 0;
    interface->frame++;
    fputs("\n", interface->pipe);
    fflush(interface->pipe);

    if (VIDEO_PARALLEL_PROCESSES > 0) {
        // Open new file to write template of frame to:
        fclose(interface->pipe);
        char buff[256];
        snprintf(buff, 256, "%s/frame_%04d.plt", TEMPLATES_TMP_DIR, interface->frame);
        interface->pipe = fopen(buff, "w");

        // Setup config
        snprintf(buff, 256, "%s/frame_%04d.png", FRAMES_TMP_DIR, interface->frame);
        pipe_terminal_output(interface, "pngcairo", buff);
        pipe_default_commands(interface);
    }
    
    va_list ap;
    va_start(ap, interface);
    pipe_config_vargs(interface, ap);
    va_end(ap);
}


void process_video_parallel(t_gnuplot *interface)
{
    omp_set_num_threads(8);
    #pragma omp parallel for schedule(dynamic)
    for (int ii=1; ii<73; ii++) {
        char scriptfile[256];
        snprintf(scriptfile, sizeof(scriptfile), "%s/frame_%04d.plt", TEMPLATES_TMP_DIR, ii);

        char cmd[512];
        snprintf(cmd, sizeof(cmd), "gnuplot %s", scriptfile);
        FILE *gnuplot_instance = popen(cmd, "r");
        pclose(gnuplot_instance);
    }

    char buff[1024];
    snprintf(buff, 1024, "ffmpeg -y -loglevel error -framerate %d -s:v %dx%d -i GNUPLOT_FRAMES_TMP/frame_%%04d.png -pix_fmt yuv420p -c:v libx264 -crf 18 %s", interface->framerate, interface->size[0], interface->size[1], interface->file_name);
    system(buff);
}


void gnuplot_fini(t_gnuplot *interface)
{   
    fputs("\n", interface->pipe);

    if (VIDEO_PARALLEL_PROCESSES == 0) {
        pclose(interface->pipe);
    } else if (VIDEO_PARALLEL_PROCESSES > 0) {
        fclose(interface->pipe);
        process_video_parallel(interface);
    } else {
        puts("Error: VIDEO_PARALLEL_PROCESSES < 0.");
    }

    free(interface->file_name);
}


void video_to_gif(const char *name_video, const char *name_gif)
{
      sleep(1);  //Delay necessary to ensure mp4 exists?
      char command_mp4_to_gif[1024];
      char cwd[256];
      getcwd(cwd, 256);
      snprintf(command_mp4_to_gif, 1024, "ffmpeg -loglevel error -i %s/%s -vf \"fps=12,scale=640:-1:flags=lanczos\" -gifflags +transdiff -y %s 2>&1", cwd, name_video, name_gif);
      system(command_mp4_to_gif);
}


int guard_is2d(t_gnuplot *interface)
{
    if (interface->dim == 2) return 1;
    puts("Error: Cannot use 2d method!");
    return 0;
}


int guard_is3d(t_gnuplot *interface)
{
    if (interface->dim == 3) return 1;
    puts("Error: Cannot use 3d method!");
    return 0;
}


void guard_active_plotting(t_gnuplot *interface)
{
    if (interface->state == 0) {
        if (interface->dim == 2) {
            fputs( "plot ", interface->pipe);
        } else if (interface->dim == 3) {
            fputs( "splot ", interface->pipe);
        } else {
            puts("Error. dim not valid."); 
            exit(1);
        }
        interface->state = 1;
    } else if (interface-> state == 1) {
        fputs(" , ", interface->pipe);
    } else {
        puts("Error. State is not valid.");
    }
}


void add_datablock_from_file(t_gnuplot *interface, const char *filename, int header_lines_skip, const char *datablock_name)
{
    if (interface->state != 0) {
        puts("Cannot add datablock when plotting is active. Do so before drawing any element."); 
        return;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {puts("Error opening file for data block in plot"); return;}

    fprintf(interface->pipe, "$%s << EOD\n", datablock_name);
    char buffer[1024];

    for (int ii=0; ii<header_lines_skip; ii++) {
        fgets(buffer, sizeof(buffer), file);
    }
    while (fgets(buffer, sizeof(buffer), file)) {
        fputs(buffer, interface->pipe);
    }
    fputs("EOD\n", interface->pipe);
    fclose(file);
}


int config_specifies_title(const char *config)
{
    return strstr(config, "title") != NULL || strstr(config, "notitle") != NULL;
}


void pipe_element_config(t_gnuplot *interface, const char *config)
{
    if (!config) {
        fprintf(interface->pipe, " notitle");
        return;
    }

    if (config_specifies_title(config)) fprintf(interface->pipe, " %s", config);
    else fprintf(interface->pipe, " %s notitle", config);
}


void draw_datablock(t_gnuplot *interface, const char *datablock_name, const char *config)
{
    guard_active_plotting(interface);
    fprintf(interface->pipe, "$%s", datablock_name);
    pipe_element_config(interface, config);
}


void draw_file(t_gnuplot *interface, const char *file_name, const char *config)
{
    guard_active_plotting(interface);
    fprintf(interface->pipe, "'%s'", file_name);
    pipe_element_config(interface, config);
}


void draw_command(t_gnuplot *interface, const char *command)
{
    guard_active_plotting(interface);
    fprintf(interface->pipe, "%s ", command);
}


void draw_point_2d(t_gnuplot *interface, double x, double y, const char *config)
{
    if (!guard_is2d(interface)) return;
    guard_active_plotting(interface);

    fprintf(interface->pipe, "\"<echo \'%f %f\'\" w p", x, y);
    pipe_element_config(interface, config);
} 


void draw_2d(t_gnuplot *interface, double *x, double *y, int N, const char *config)
{
    if (!guard_is2d(interface)) return;
    guard_active_plotting(interface);

    fprintf(interface->pipe, "\"<echo \'");
    for (int ii=0; ii<N; ii++) {
        fprintf(interface->pipe, "%f %f", x[ii], y[ii]);
        if (ii < N-1) {
            fprintf(interface->pipe, "\\n");
        }
    }
    fprintf(interface->pipe, "\'\"");
    pipe_element_config(interface, config);
}


void draw_array_2d(t_gnuplot *interface, double **coords, int N, const char *config)
{ 
    if (!guard_is2d(interface)) return;
    guard_active_plotting(interface);

    fprintf(interface->pipe, "\"<echo \'");
    for (int ii=0; ii<N; ii++) {
        fprintf(interface->pipe, "%f %f", coords[ii][0], coords[ii][1]);
        if (ii < N-1) {
            fprintf(interface->pipe, "\\n");
        }
    }
    fprintf(interface->pipe, "\'\"");
    pipe_element_config(interface, config);
}


void draw_errorbars_2d(t_gnuplot *interface, double *x, double *mean, double *err, int N, const char *config)
{
    if (!guard_is2d(interface)) return;
    guard_active_plotting(interface);

    fprintf(interface->pipe, "\"<echo \'");
    for (int ii=0; ii<N; ii++) {
        fprintf(interface->pipe, "%f %f %f %f", x[ii], mean[ii], mean[ii] + err[ii], mean[ii] - err[ii]);
        if (ii < N-1) {
            fprintf(interface->pipe, "\\n");
        }
    }
    fprintf(interface->pipe, "\'\" w errorbars");
    pipe_element_config(interface, config);
}


void draw_arrows_from_array_2d(t_gnuplot *interface, double **coords, int N, int spacing, int offset, const char *config)
{   
    if (!guard_is2d(interface)) return;
    guard_active_plotting(interface);

    fprintf(interface->pipe, "\"<echo \'");
    for (int ii=0; ii<N; ii++) {
        if ((offset +ii)%spacing == 0 && ii+1 < N) {
            fprintf(interface->pipe, "%f %f %f %f", coords[ii][0], coords[ii][1], coords[ii+1][0]-coords[ii][0], coords[ii+1][1]-coords[ii][1]);
        }
        if (ii < N-1) {
            fprintf(interface->pipe, "\\n");
        }
    }
    fprintf(interface->pipe, "\'\" w vectors");
    pipe_element_config(interface, config);
}


void draw_segment_2d(t_gnuplot *interface, double x0, double y0, double xf, double yf, const char *config)
{
    if (!guard_is2d(interface)) return;
    guard_active_plotting(interface);

    fprintf(interface->pipe, "\"<echo \'%f %f %f %f\' \" w vectors nohead", x0, y0, xf-x0, yf-y0);
    pipe_element_config(interface, config);
}


void draw_function_2d(t_gnuplot *interface, double x0, double xf, int N, double (*fun)(double), const char *config)
{
    if (!guard_is2d(interface)) return;
    guard_active_plotting(interface);

    fprintf(interface->pipe, "\"<echo \'");
    for (int ii=0; ii<N; ii++) {
        double x = x0 + ii * (xf - x0) / (N - 1);
        fprintf(interface->pipe, "%f %f", x, fun(x));
        if (ii < N-1) {
            fprintf(interface->pipe, "\\n");
        }
    }
    fprintf(interface->pipe, "\'\"");
    pipe_element_config(interface, config);
}


void draw_sphere_3d(t_gnuplot *interface, double x, double y, double z, double r, const char *type, const char *config)
{   // type must be: pm3d, lines, points, ... (polygons? Should produce nothing if haven't "set pm3d", else, same as pm3d)
    if (!guard_is3d(interface)) return;
    guard_active_plotting(interface);
    fprintf(interface->pipe, "[u=-pi/2:pi/2] [v=0:2*pi] %f+%f*cos(u)*cos(v),%f+%f*cos(u)*sin(v),%f+%f*sin(u) w %s", 
            x, r, y, r, z, r, type);
    pipe_element_config(interface, config);
}


void draw_solid_triangle_3d(t_gnuplot *interface, double v0[3], double v1[3], double v2[3], const char *config)
{
    if (!guard_is3d(interface)) return;
    guard_active_plotting(interface);
    fprintf(interface->pipe, "\"<echo \'");
    fprintf(interface->pipe, "%f %f %f\\n", v0[0], v0[1], v0[2]);
    fprintf(interface->pipe, "%f %f %f\\n", v1[0], v1[1], v1[2]);
    fprintf(interface->pipe, "%f %f %f\\n\'\"", v2[0], v2[1], v2[2]);
    fprintf(interface->pipe, "w polygons");
    pipe_element_config(interface, config);
}


void draw_polytope_3d(t_gnuplot *interface, double **coords, int N, const char *config)
{
    if (!guard_is3d(interface)) return;
    guard_active_plotting(interface);

    fprintf(interface->pipe, "\"<echo \'");
    for (int ii=0; ii<N-1; ii++) {
        fprintf(interface->pipe, "%f %f %f\\n", coords[ii][0], coords[ii][1], coords[ii][2]);
    }
    fprintf(interface->pipe, "%f %f %f\'\"", coords[N-1][0], coords[N-1][1], coords[N-1][2]);
    fprintf(interface->pipe, "w polygons");
    pipe_element_config(interface, config);
}


// void plot_array_2D(double **array, int N, const char *title, char *xlabel, char *ylabel, const char *file_name, 
//                    double *IC, double *plotDimensions_x0_xf_y0_yf, char *config, double *arrows_size_freq_offset)
// {
//     FILE *pipe = popen_gnuplot();
//     char buffer[256];
//     if (plotDimensions_x0_xf_y0_yf) {
//         snprintf(buffer, 256, "set xrange[%f:%f]", plotDimensions_x0_xf_y0_yf[0], plotDimensions_x0_xf_y0_yf[1]);
//         set_config(pipe, buffer);
//         snprintf(buffer, 256, "set yrange[%f:%f]", plotDimensions_x0_xf_y0_yf[2], plotDimensions_x0_xf_y0_yf[3]);
//         set_config(pipe, buffer);
//     }
//     if (xlabel) {
//         snprintf(buffer, 256, "set xlabel '%s'", xlabel);
//         set_config(pipe, buffer);
//     }
//     if (ylabel) {
//         snprintf(buffer, 256, "set ylabel '%s'", ylabel);
//         set_config(pipe, buffer);
//     }
//     set_config(pipe, "set grid");
//     set_config(pipe, "set key box opaque");
//     snprintf(buffer, 256, "set title \"%s\" font ',24'", title);
//     set_config(pipe, buffer);
//
//     start_plot(pipe, file_name);
//     if (IC) {
//         add_point(pipe, IC[0], IC[1], "ps 2 pt 7 lc 8 notitle");
//     }
//     add_array_points(pipe, array, N, config);
//     if (arrows_size_freq_offset) {
//         snprintf(buffer, 256, "lc 2 lw 3 size %f,20 fixed notitle", arrows_size_freq_offset[0]);
//         add_arrows_from_array_points(pipe, array, N, arrows_size_freq_offset[1], arrows_size_freq_offset[2],
//                                      buffer);
//     }
//     end_plot(pipe);
//     pclose(pipe); 
// }


