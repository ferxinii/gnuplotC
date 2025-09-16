set terminal pngcairo enhanced font 'Arial,18' size 1080,1080 enhanced
 set output 'GNUPLOT_FRAMES_TMP/frame_0051.png'
set parametric
set title 'video'
set pm3d depthorder
unset colorbox
unset border
unset tics
set isosamples 100
set view equal xyz
set view 70, 245.000000
splot [u=-pi/2:pi/2] [v=0:2*pi] 0.500000+0.200000*cos(u)*cos(v),0.500000+0.200000*cos(u)*sin(v),0.500000+0.200000*sin(u) w lines notitle , [u=-pi/2:pi/2] [v=0:2*pi] 0.800000+0.100000*cos(u)*cos(v),0.800000+0.100000*cos(u)*sin(v),0.800000+0.100000*sin(u) w pm3d notitle
