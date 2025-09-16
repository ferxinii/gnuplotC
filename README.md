# gnuplotC

Simple and easy C interface to plot using gnuplot. Allows for video creation.


### Modes
Currently, gnuplotC supports the following modes:


|    | **png** | **pdf** | **eps** | **video (mp4)** |
| ---: | :---: | :---: | :---: | :---: |
| **2D** | PNG_2D | PDF_2D | EPS_2D | VIDEO_2D |
| **3D** | PNG_3D | PDF_3D |  | VIDEO_3D |


Each mode selects the appropriate gnuplot terminal. Note that EPS is not supported in 3D due to how pm3d translucent surfaces are not fully supported.





