# GNUplot script for resource monitoring
set terminal pngcairo enhanced font 'Arial,10' size 1200,800
set output 'png/interpolation_search_20260131_190624.png'

set multiplot layout 2,2 title 'Resource Monitoring Metrics' font 'Arial,14'
set style line 1 lc rgb '#0060ad' lt 1 lw 2
set style line 2 lc rgb '#dd181f' lt 1 lw 2
set style line 3 lc rgb '#008040' lt 1 lw 2

set title 'CPU Time Usage'
set xlabel 'Time'
set ylabel 'CPU Time (seconds)'
set grid
plot 'png/interpolation_search_20260131_190624.dat' using 1:2 with linespoints ls 1 title 'CPU Time'
set title 'Memory Usage'
set xlabel 'Time'
set ylabel 'Memory (KB)'
set grid
plot 'png/interpolation_search_20260131_190624.dat' using 1:3 with linespoints ls 2 title 'Memory Usage'
set title 'Execution Time'
set xlabel 'Time'
set ylabel 'Execution Time (seconds)'
set grid
plot 'png/interpolation_search_20260131_190624.dat' using 1:4 with linespoints ls 3 title 'Execution Time'
set title 'All Metrics Combined'
set xlabel 'Time'
set ylabel 'Values'
set grid
plot 'png/interpolation_search_20260131_190624.dat' using 1:2 with lines ls 1 title 'CPU Time', \
     'png/interpolation_search_20260131_190624.dat' using 1:3 with lines ls 2 title 'Memory', \
     'png/interpolation_search_20260131_190624.dat' using 1:4 with lines ls 3 title 'Exec Time'
unset multiplot
