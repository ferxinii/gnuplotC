# gnuplotC

Simple and easy C interface to *gnuplot*. Allows for video creation.

### Modes
Currently, **gnuplotC** supports the following modes:


|    | **png** | **pdf** | **eps** | **video (mp4)** |
| ---: | :---: | :---: | :---: | :---: |
| **2D** | PNG_2D | PDF_2D | EPS_2D | VIDEO_2D |
| **3D** | PNG_3D | PDF_3D |  | VIDEO_3D |




---
### Creating a plot

1. **Start *gnuplot* interface.** Select mode, file name, figure size, ...
2. *(Optional)* **Configure the interface.** Before adding elements!
3. **Add elements.**
4. **End.**

```{C}
t_gnuplot *ifc = gnuplot_start(PNG_2D, "segment.png", figsize, fontsize);
draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2"); 
gnuplot_end(ifc);
```



---
### Creating a video
Videos are built using *ffmpeg*, which must be installed and available. Treat each frame as its own independent figure. By default, each frame is piped directly to *ffmpeg* without the need to store it.

1. **Start *gnuplot* interface.** Select framerate, ...
2. *(Optional)* **Configure the interface.** Before adding elements!
3. **Add elements.**
4. **Start a new frame.**
5. **Repeat from step 2 as needed.**
6. **End.**

Optionally, the default framerate (24 fps) can be changed by modifying the global variable *GNUPLOTC_FRAMERATE* **before** starting the interface.

```{C}
GNUPLOTC_FRAMERATE = 12;

char *video_config[] = {"set view 70, 0", "unset border", "unset tics", 
                        "set isosamples 100", "set view equal xyz", NULL};
ifc = gnuplot_start(VIDEO_3D, "14.mp4", figsize, fontsize, GNUPLOTC_ARRAY(video_config));
draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, "lines", NULL);
for (int ii=1; ii<GNUPLOTC_FRAMERATE*3+1; ii++) {
    char buff[256]; 
    snprintf(buff, 256, "set view 70, %f", fmod(ii*360.0/(GNUPLOTC_FRAMERATE*3), 360.0));
    next_frame(ifc, GNUPLOTC_ARRAY(video_config), buff);
    draw_sphere_3d(ifc, 0.5, 0.5, 0.5, 0.2, "lines", NULL);
}
gnuplot_end(ifc);
```


### Faster parallel video processing
Parallel processing of the frames is possible, with each thread running *gnuplot* in parallel resulting in much faster processing times. The downside is that all the frames must be saved to disc beforehand (in a temporal directory), which can take up some space. Internally, this is implemented with *OMP*.

To activate this mode, simply call *activate_parallel_video_processing(...)* before adding any element and frame.


---
## Configuring the interface

The gnuplot configuration can be specified:
- When calling *gnuplot_start(...)*, in the form of additional optional parameters.
- Calling *gnuplot_config(...)*, in the form of additional optional parameters, **before** drawing any element.
- In the case of a video, also when calling *new_frame(...)*.

