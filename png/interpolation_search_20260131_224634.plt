# GNUplot script for resource monitoring
set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000
set output 'png/interpolation_search_20260131_224634.png'

set multiplot layout 2,2 title 'Algorithm Performance Analysis' font 'Helvetica,16'
set style line 1 lc rgb '#2E86AB' lt 1 lw 3 pt 7 ps 0.8
set style line 2 lc rgb '#A23B72' lt 1 lw 3 pt 5 ps 0.8
set style line 3 lc rgb '#F18F01' lt 1 lw 3 pt 9 ps 0.8
set style line 4 lc rgb '#F19C79' lt 1 lw 3 pt 11 ps 0.8
set style line 5 lc rgb '#0ABB9F' lt 1 lw 3 pt 13 ps 0.8

set format x "%.3f"
set xtics rotate by -45
set autoscale x

set title 'CPU Time Usage'
set xlabel 'Time'
set ylabel 'CPU Time (seconds)'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/interpolation_search_20260131_224634.dat' using 1:2 with linespoints ls 1 title 'CPU Time'
set title 'Memory Usage'
set xlabel 'Time'
set ylabel 'Memory (KB)'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/interpolation_search_20260131_224634.dat' using 1:3 with linespoints ls 2 title 'Memory Usage'
set title 'Execution Time'
set xlabel 'Time'
set ylabel 'Execution Time (seconds)'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/interpolation_search_20260131_224634.dat' using 1:4 with linespoints ls 3 title 'Execution Time'
set title 'All Metrics Combined'
set xlabel 'Time'
set ylabel 'Values'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
set key outside center bottom horizontal
plot 'png/interpolation_search_20260131_224634.dat' using 1:2 with lines ls 1 title 'CPU Time', \
     'png/interpolation_search_20260131_224634.dat' using 1:3 with lines ls 2 title 'Memory', \
     'png/interpolation_search_20260131_224634.dat' using 1:4 with lines ls 3 title 'Exec Time'
unset multiplot
