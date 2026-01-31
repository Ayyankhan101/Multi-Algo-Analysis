# GNUplot script for performance metrics
set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000
set output 'png/merge_sort_20260131_225122_algorithm.png'

set multiplot layout 2,2 title 'Performance Analysis Dashboard' font 'Helvetica,16'
set style line 1 lc rgb '#2E86AB' lt 1 lw 3 pt 7 ps 0.8
set style line 2 lc rgb '#A23B72' lt 1 lw 3 pt 5 ps 0.8
set style line 3 lc rgb '#F18F01' lt 1 lw 3 pt 9 ps 0.8
set style line 4 lc rgb '#F19C79' lt 1 lw 3 pt 11 ps 0.8
set style line 5 lc rgb '#0ABB9F' lt 1 lw 3 pt 13 ps 0.8

set format x "%.3f"
set xtics rotate by -45
set autoscale x

set title 'Cache Hit Rate (%)'
set xlabel 'Time'
set ylabel 'Hit Rate %'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/merge_sort_20260131_225122_algorithm.dat' using 1:11 with linespoints ls 1 title 'Cache Hit Rate'
set title 'Error Rate (%)'
set xlabel 'Time'
set ylabel 'Error Rate %'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/merge_sort_20260131_225122_algorithm.dat' using 1:12 with linespoints ls 2 title 'Error Rate'
set title 'Throughput (ops/sec)'
set xlabel 'Time'
set ylabel 'Operations/sec'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/merge_sort_20260131_225122_algorithm.dat' using 1:13 with linespoints ls 3 title 'Throughput'
set title 'Response Time Percentiles (ms)'
set xlabel 'Time'
set ylabel 'Response Time (ms)'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
set key outside center bottom horizontal
plot 'png/merge_sort_20260131_225122_algorithm.dat' using 1:14 with lines ls 1 title 'P50', \
     'png/merge_sort_20260131_225122_algorithm.dat' using 1:15 with lines ls 2 title 'P95', \
     'png/merge_sort_20260131_225122_algorithm.dat' using 1:16 with lines ls 3 title 'P99'
unset multiplot
