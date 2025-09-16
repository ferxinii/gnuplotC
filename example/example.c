
#include "plot.h"
#include <omp.h>
#include <stdlib.h>


double parabola(double x) { return x*x; }

int main() {
    int figsize[2] = {1080, 1080};
    int fontsize = 18;
    int framerate = 0;
    
    /* -------------------------- */
    /* ----- BASIC EXAMPLES ----- */

    // GNUPLOT_start:
    t_gnuplot *ifc = gnuplot_start(PNG_2D, "1.png", figsize, fontsize, framerate);
    draw_segment_2d(ifc, 0, 0, 1, 1, NULL); 
    draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2"); 
    gnuplot_fini(ifc);

    
    // GNUPLOT_start with configurations:
    ifc = gnuplot_start(PNG_2D, "2.png", figsize, fontsize, framerate, 
                        "set title 'Config and legends'", 
                        "set xlabel 'x'", 
                        "set xrange [0:1]", 
                        "set yrange [0:1]");
    draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2 title 'legend'"); 
    gnuplot_fini(ifc);

    
    // GNUPLOT_ARRAY (array of commands):
    // When passing an array of commands, it MUST be NULL terminated
    char *cmd_array[] = {"set xlabel 'x'", 
                         "set ylabel 'y'", 
                         "set xrange [0:1]", 
                         "set yrange [0:1]", 
                         "set grid lw 2",
                         NULL};

    ifc = gnuplot_start(PNG_2D, "3.png", figsize, fontsize, framerate, 
                        "set title 'Configuring'", 
                        GNUPLOT_ARRAY(cmd_array), 
                        "set xtics 0,0.1,1");
    draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2 title 'legend'"); 
    gnuplot_fini(ifc);


    // GNUPLOT_CONFIG:
    ifc = gnuplot_start(PNG_2D, "4.png", figsize, fontsize, framerate);
    gnuplot_config(ifc, "set title 'Configuring'", GNUPLOT_ARRAY(cmd_array));
    draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2 title 'legend'"); 
    gnuplot_fini(ifc);


    
    /* ----------------------- */
    /* ----- 2D ELEMENTS ----- */
    
    // DRAW_POINT_2D:
    ifc = gnuplot_start(PNG_2D, "5.png", figsize, fontsize, framerate,
                        "set title 'draw\\_point'", GNUPLOT_ARRAY(cmd_array));
    draw_point_2d(ifc, 0.5, 0.3, "ps 3 pt 15");
    draw_point_2d(ifc, 0.2, 0.8, "ps 6 pt 9");
    gnuplot_fini(ifc);


    // DRAW_2D:
    double x[4] = {0, 0.4, 0.8, 1};
    double y[4] = {0.7, 0.8, 0.2, 0.1};

    ifc = gnuplot_start(PNG_2D, "6.png", figsize, fontsize, framerate,
                        "set title 'draw\\_2d'", GNUPLOT_ARRAY(cmd_array));
    draw_2d(ifc, x, y, 4, "w linespoints lw 2 ps 2 pt 7 title 'w linespoints'");
    gnuplot_fini(ifc);
    
    
    // DRAW_ARRAY_2D:
    double **coords = malloc(sizeof(double*) * 3);
    coords[0] = malloc(sizeof(double) * 2); 
    coords[1] = malloc(sizeof(double) * 2); 
    coords[2] = malloc(sizeof(double) * 2);

    coords[0][0] = 0.1;  coords[0][1] = 0.7; 
    coords[1][0] = 0.3;  coords[1][1] = 0.8; 
    coords[2][0] = 0.9;  coords[2][1] = 0.7; 

    ifc = gnuplot_start(PNG_2D, "7.png", figsize, fontsize, framerate,
                        "set title 'draw\\_array\\_2d'", GNUPLOT_ARRAY(cmd_array));
    draw_array_2d(ifc, coords, 3, "w lines dt '_ ' lw 4 title 'w lines'");
    gnuplot_fini(ifc);


    // DRAW_ARROWS_FROM_ARRAY_2D
    ifc = gnuplot_start(PNG_2D, "8.png", figsize, fontsize, framerate,
                        "set title 'draw\\_arrows\\_from\\_array\\_2d'", GNUPLOT_ARRAY(cmd_array));
    draw_arrows_from_array_2d(ifc, coords, 3, 1, 0, "lw 3 size 0.05,20 fixed");  // Recall size command for head size (and fixed keyword), ex: size 0.2,20 fixed
    gnuplot_fini(ifc);
    
    
    // DRAW_ERRORBARS_2D:
    x[0] = 0.2; x[1] = 0.4; x[2] = 0.6; x[3] = 0.8; 
    y[0] = 0.2; y[1] = 0.4; y[2] = 0.45; y[3] = 0.5; 
    double errors[4] = {0.1, 0.15, 0.2, 0.25};
    ifc = gnuplot_start(PNG_2D, "9.png", figsize, fontsize, framerate,
                        "set title 'draw\\_errorbars'", GNUPLOT_ARRAY(cmd_array));
    draw_errorbars_2d(ifc, x, y, errors, 4, "lw 3 lc 6 pt 5 ps 2");
    gnuplot_fini(ifc);


    // DRAW_FUNCTION_2D
    ifc = gnuplot_start(PNG_2D, "10.png", figsize, fontsize, framerate,
                        "set title 'draw\\_function\\_2d'", GNUPLOT_ARRAY(cmd_array));
    draw_function_2d(ifc, 0, 1, 100, parabola, "w lines lw 2");
    gnuplot_fini(ifc);


    
    /* ----------------------- */
    /* ----- 3D ELEMENTS ----- */

    // DRAW_SPHERE_3D
    ifc = gnuplot_start(PNG_3D, "11.png", figsize, fontsize, framerate,
                        "set title 'draw\\_sphere\\_3d'", GNUPLOT_ARRAY(cmd_array),
                        "set pm3d depthorder");
    draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, "lines", "title 'lines'");
    draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, "pm3d", "title 'pm3d'");
    gnuplot_fini(ifc);


    // UNSET COLORBOX, SET ISOSAMPLES
    ifc = gnuplot_start(PNG_3D, "12.png", figsize, fontsize, framerate,
                        "set title 'draw\\_sphere\\_3d'", GNUPLOT_ARRAY(cmd_array),
                        "set pm3d depthorder", "unset colorbox", "set isosamples 100");
    draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, "lines", "title 'lines'");
    draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, "pm3d", "title 'pm3d'");
    gnuplot_fini(ifc);


    // DRAW_SOLID_TRIANGLE_3D
    double v0[3] = {0.1, 0.1, 0.1};
    double v1[3] = {0.8, 0.3, 0.1};
    double v2[3] = {0.2, 0.2, 0.7};

    ifc = gnuplot_start(PNG_3D, "13.png", figsize, fontsize, framerate,
                        "set title 'draw\\_triangle\\_3d'", GNUPLOT_ARRAY(cmd_array),
                        "set pm3d depthorder");
    draw_solid_triangle_3d(ifc, v0, v1, v2, NULL);
    gnuplot_fini(ifc);


    
    free(coords[0]); free(coords[1]); free(coords[2]); free(coords);


    /* ----------------------- */
    /* -------- VIDEO -------- */

    activate_parallel_video_processing(8);

    char *video_config[] = {"set title 'video'", "set pm3d depthorder", "unset colorbox", 
                            "unset border", "unset tics", "set isosamples 100", "set view equal xyz", NULL};
    framerate = 24;;
    ifc = gnuplot_start(VIDEO_3D, "14.mp4", figsize, fontsize, framerate, GNUPLOT_ARRAY(video_config));
    draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, "lines", NULL);
    draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, "pm3d", NULL);
    for (int ii=0; ii<72; ii++) {
        char buff[256]; 
        snprintf(buff, 256, "set view 70, %f", ii*360.0/72.0);

        next_frame(ifc, GNUPLOT_ARRAY(video_config), buff);
        draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, "lines", NULL);
        draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, "pm3d", NULL);
    }
    gnuplot_fini(ifc);



    

}
