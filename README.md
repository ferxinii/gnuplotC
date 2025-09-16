# gnuplotC

Simple and easy C interface to plot using gnuplot. Allows for video creation.


### Modes
Currently, gnuplotC supports the following modes:


|    | **png** | **pdf** | **eps** | **video (mp4)** |
| ---: | :---: | :---: | :---: | :---: |
| **2D** | PNG_2D | PDF_2D | EPS_2D | VIDEO_2D |
| **3D** | PNG_3D | PDF_3D |  | VIDEO_3D |


Each mode selects the appropriate gnuplot terminal. 



---
### Making a plot
To make a plot, simply follow the pipeline:

1. **Start gnuplot interface.** Select mode, file name, figure size, ...
2. *(Optional)* **Configure the interface.** Before adding elements!
3. **Add elements**
4. **End**


---
### Making a video
Videos are built using *ffmpeg*, which must be installed and available.

The usage is very similar to a single plot. Treat each frame as its own independent figure.

1. **Start gnuplot interface.** Select framerate, ...
2. *(Optional)* **Configure the interface.** Before adding elements!
3. **Add elements**
4. **Start a new frame**
5. **Repeat from step 2 as needed**
6. **End**


#### Faster parallel video processing
Parallel processing of the frames is possible, with each thread running gnuplot in parallel resulting in much faster times. The downside is that all the frames must be saved to disc beforehand (in a temporal directory), which can take up some space.

To activate this mode, simply call *activate_parallel_video_processing(...)* before adding any element and frame.
