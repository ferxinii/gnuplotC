set parametric
set urange [-pi/2:pi/2]
set vrange [0:2*pi]
set isosamples 40,40
set hidden3d

splot [u=-pi/2:pi/2] [v=0:2*pi] 0.000000+1.000000*cos(u)*cos(v),0.000000+1.000000*cos(u)*sin(v),0.000000+1.000000*sin(u) w pm3d fc 'red'
