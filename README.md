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
void example() {
    t_gnuplot *ifc = gnuplot_start(PNG_2D, "segment.png", figsize, fontsize, framerate);
    draw_segment_2d(ifc, 1, 0, 0.4, 0.6, "lw 2"); 
    gnuplot_end(ifc);
}
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


### Faster parallel video processing
Parallel processing of the frames is possible, with each thread running *gnuplot* in parallel resulting in much faster processing times. The downside is that all the frames must be saved to disc beforehand (in a temporal directory), which can take up some space. Internally, this is implemented with *OMP*.

To activate this mode, simply call *activate_parallel_video_processing(...)* before adding any element and frame.


