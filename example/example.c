
#include "gnuplotc.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


double parabola(double x) { return x*x; }

int main() {
    int figsize[2] = {1080, 1080};
    int fontsize = 18;
    
    /* -------------------------- */
    /* ----- BASIC EXAMPLES ----- */

    // GNUPLOTC_start:
    t_gnuplot *ifc = gnuplot_start(PNG_2D, "1.png", figsize, fontsize);
    draw_segment_2d(ifc, 0, 0, 1, 1, NULL); 
    draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2"); 
    gnuplot_end(ifc);

    
    // GNUPLOTC_start with configurations:
    ifc = gnuplot_start(PNG_2D, "2.png", figsize, fontsize, 
                        "set title 'Config and legends'", 
                        "set xlabel 'x'", 
                        "set xrange [0:1]", 
                        "set yrange [0:1]");
    draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2 title 'legend'"); 
    gnuplot_end(ifc);

    
    // GNUPLOTC_ARRAY (array of commands):
    // When passing an array of commands, it MUST be NULL terminated
    char *cmd_array[] = {"set xlabel 'x'", 
                         "set ylabel 'y'", 
                         "set xrange [0:1]", 
                         "set yrange [0:1]", 
                         "set grid lw 2",
                         NULL};

    ifc = gnuplot_start(PNG_2D, "3.png", figsize, fontsize, 
                        "set title 'Configuring'", 
                        GNUPLOTC_ARRAY(cmd_array), 
                        "set xtics 0,0.1,1");
    draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2 title 'legend'"); 
    gnuplot_end(ifc);


    // GNUPLOTC_CONFIG:
    ifc = gnuplot_start(PNG_2D, "4.png", figsize, fontsize);
    gnuplot_config(ifc, "set title 'Configuring'", GNUPLOTC_ARRAY(cmd_array));
    draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2 title 'legend'"); 
    gnuplot_end(ifc);


    
    /* ----------------------- */
    /* ----- 2D ELEMENTS ----- */
    
    // DRAW_POINT_2D:
    ifc = gnuplot_start(PNG_2D, "5.png", figsize, fontsize,
                        "set title 'draw\\_point'", GNUPLOTC_ARRAY(cmd_array));
    draw_point_2d(ifc, 0.5, 0.3, "ps 3 pt 15");
    draw_point_2d(ifc, 0.2, 0.8, "ps 6 pt 9");
    gnuplot_end(ifc);


    // DRAW_2D:
    double x[4] = {0, 0.4, 0.8, 1};
    double y[4] = {0.7, 0.8, 0.2, 0.1};

    ifc = gnuplot_start(PNG_2D, "6.png", figsize, fontsize,
                        "set title 'draw\\_2d'", GNUPLOTC_ARRAY(cmd_array));
    draw_2d(ifc, x, y, 4, LINESPOINTS, "lw 2 ps 2 pt 7 title 'w linespoints'");
    gnuplot_end(ifc);
    
    
    // DRAW_ARRAY_2D:
    double **coords = malloc(sizeof(double*) * 3);
    coords[0] = malloc(sizeof(double) * 2); 
    coords[1] = malloc(sizeof(double) * 2); 
    coords[2] = malloc(sizeof(double) * 2);

    coords[0][0] = 0.1;  coords[0][1] = 0.7; 
    coords[1][0] = 0.3;  coords[1][1] = 0.8; 
    coords[2][0] = 0.9;  coords[2][1] = 0.7; 

    ifc = gnuplot_start(PNG_2D, "7.png", figsize, fontsize,
                        "set title 'draw\\_array\\_2d'", GNUPLOTC_ARRAY(cmd_array));
    draw_array_2d(ifc, coords, 3, LINES, "dt '_ ' lw 4 title 'w lines'");
    gnuplot_end(ifc);


    // DRAW_ARROWS_FROM_ARRAY_2D
    ifc = gnuplot_start(PNG_2D, "8.png", figsize, fontsize,
                        "set title 'draw\\_arrows\\_from\\_array\\_2d'", GNUPLOTC_ARRAY(cmd_array));
    draw_arrows_from_array_2d(ifc, coords, 3, 1, 0, "lw 3 size 0.05,20 fixed");  // Recall size command for head size (and fixed keyword), ex: size 0.2,20 fixed
    gnuplot_end(ifc);
    
    
    // DRAW_ERRORBARS_2D:
    x[0] = 0.2; x[1] = 0.4; x[2] = 0.6; x[3] = 0.8; 
    y[0] = 0.2; y[1] = 0.4; y[2] = 0.45; y[3] = 0.5; 
    double errors[4] = {0.1, 0.15, 0.2, 0.25};
    ifc = gnuplot_start(PNG_2D, "9.png", figsize, fontsize,
                        "set title 'draw\\_errorbars'", GNUPLOTC_ARRAY(cmd_array));
    draw_errorbars_2d(ifc, x, y, errors, 4, "lw 3 lc 6 pt 5 ps 2");
    gnuplot_end(ifc);


    // DRAW_FUNCTION_2D
    ifc = gnuplot_start(PNG_2D, "10.png", figsize, fontsize,
                        "set title 'draw\\_function\\_2d'", GNUPLOTC_ARRAY(cmd_array));
    draw_function_2d(ifc, 0, 1, 100, parabola, LINES, "lw 2");
    gnuplot_end(ifc);


    
    /* ----------------------- */
    /* ----- 3D ELEMENTS ----- */

    // DRAW_SPHERE_3D
    ifc = gnuplot_start(PNG_3D, "11.png", figsize, fontsize,
                        "set title 'draw\\_sphere\\_3d'", GNUPLOTC_ARRAY(cmd_array),
                        "set pm3d depthorder");
    draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, LINES, "title 'lines'");
    draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, PM3D, "title 'pm3d'");
    gnuplot_end(ifc);


    // UNSET COLORBOX, SET ISOSAMPLES
    ifc = gnuplot_start(PNG_3D, "12.png", figsize, fontsize,
                        "set title 'draw\\_sphere\\_3d'", GNUPLOTC_ARRAY(cmd_array),
                        "set pm3d depthorder", "unset colorbox", "set isosamples 100");
    draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, LINES, "title 'lines'");
    draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, PM3D, "title 'pm3d'");
    gnuplot_end(ifc);


    // DRAW_SOLID_TRIANGLE_3D
    double v0[3] = {0.1, 0.1, 0.1};
    double v1[3] = {0.8, 0.3, 0.1};
    double v2[3] = {0.2, 0.2, 0.7};

    ifc = gnuplot_start(PNG_3D, "13.png", figsize, fontsize,
                        "set title 'draw\\_triangle\\_3d'", GNUPLOTC_ARRAY(cmd_array),
                        "set pm3d depthorder");
    draw_solid_triangle_3d(ifc, v0, v1, v2, NULL);
    gnuplot_end(ifc);


    
    free(coords[0]); free(coords[1]); free(coords[2]); free(coords);


    /* ----------------------- */
    /* -------- VIDEO -------- */

    char *video_config[] = {"set title 'video'", "set pm3d depthorder", "unset colorbox", "set view 70, 0",
                            "unset border", "unset tics", "set isosamples 100", "set view equal xyz", NULL};
    ifc = gnuplot_start(VIDEO_3D, "14.mp4", figsize, fontsize, GNUPLOTC_ARRAY(video_config));
    draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, LINES, NULL);
    draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, PM3D, NULL);
    for (int ii=1; ii<GNUPLOTC_FRAMERATE*3+1; ii++) {
        char buff[256]; 
        snprintf(buff, 256, "set view 70, %f", fmod(ii*360.0/(GNUPLOTC_FRAMERATE*3), 360.0));

        next_frame(ifc, GNUPLOTC_ARRAY(video_config), buff);
        draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, LINES, NULL);
        draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, PM3D, NULL);
    }
    gnuplot_end(ifc);


    // PARALLEL VIDEO PROCESSING
    GNUPLOTC_FRAMERATE = 60;
    activate_parallel_video_processing(8);

    ifc = gnuplot_start(VIDEO_3D, "15.mp4", figsize, fontsize, GNUPLOTC_ARRAY(video_config));
    draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, LINES, NULL);
    draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, PM3D, NULL);
    for (int ii=1; ii<GNUPLOTC_FRAMERATE*3+1; ii++) {
        char buff[256]; 
        snprintf(buff, 256, "set view 70, %f", fmod(ii*360.0/(GNUPLOTC_FRAMERATE*3), 360.0));

        next_frame(ifc, GNUPLOTC_ARRAY(video_config), buff);
        draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, LINES, NULL);
        draw_sphere_3d(ifc, 0.8, 0.8, 0.8, 0.1, PM3D, NULL);
    }
    gnuplot_end(ifc);

    
    // VIDEO_TO_GIF
    video_to_gif("14.mp4", "16.gif", figsize, 24);

    

}
