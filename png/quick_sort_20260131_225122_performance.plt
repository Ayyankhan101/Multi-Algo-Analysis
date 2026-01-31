# GNUplot script for performance metrics
set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000
set output 'png/quick_sort_20260131_225122_performance.png'

set multiplot layout 2,2 title 'Performance Analysis Dashboard' font 'Helvetica,16'
set style line 1 lc rgb '#2E86AB' lt 1 lw 3 pt 7 ps 0.8
set style line 2 lc rgb '#A23B72' lt 1 lw 3 pt 5 ps 0.8
set style line 3 lc rgb '#F18F01' lt 1 lw 3 pt 9 ps 0.8
set style line 4 lc rgb '#F19C79' lt 1 lw 3 pt 11 ps 0.8
set style line 5 lc rgb '#0ABB9F' lt 1 lw 3 pt 13 ps 0.8

set format x "%.3f"
set xtics rotate by -45
set autoscale x

set title 'CPU Utilization (%)'
set xlabel 'Time'
set ylabel 'CPU %'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/quick_sort_20260131_225122_performance.dat' using 1:2 with linespoints ls 1 title 'CPU %'
set title 'Memory Usage (MB)'
set xlabel 'Time'
set ylabel 'Memory (MB)'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/quick_sort_20260131_225122_performance.dat' using 1:3 with linespoints ls 2 title 'Memory'
set title 'Execution Time (s)'
set xlabel 'Time'
set ylabel 'Execution Time (s)'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/quick_sort_20260131_225122_performance.dat' using 1:8 with linespoints ls 3 title 'Exec Time'
set title 'Thread Count'
set xlabel 'Time'
set ylabel 'Number of Threads'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
set key outside center bottom horizontal
plot 'png/quick_sort_20260131_225122_performance.dat' using 1:9 with linespoints ls 4 title 'Threads'
unset multiplot
