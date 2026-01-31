# GNUplot script for performance metrics
set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000
set output 'png/heap_sort_20260131_225833_resources.png'

set multiplot layout 2,2 title 'Performance Analysis Dashboard' font 'Helvetica,16'
set style line 1 lc rgb '#2E86AB' lt 1 lw 3 pt 7 ps 0.8
set style line 2 lc rgb '#A23B72' lt 1 lw 3 pt 5 ps 0.8
set style line 3 lc rgb '#F18F01' lt 1 lw 3 pt 9 ps 0.8
set style line 4 lc rgb '#F19C79' lt 1 lw 3 pt 11 ps 0.8
set style line 5 lc rgb '#0ABB9F' lt 1 lw 3 pt 13 ps 0.8

set format x "%.3f"
set xtics rotate by -45
set autoscale x

set title 'Disk Read I/O (MB/s)'
set xlabel 'Time'
set ylabel 'Read MB/s'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/heap_sort_20260131_225833_resources.dat' using 1:4 with linespoints ls 1 title 'Disk Read'
set title 'Disk Write I/O (MB/s)'
set xlabel 'Time'
set ylabel 'Write MB/s'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/heap_sort_20260131_225833_resources.dat' using 1:5 with linespoints ls 2 title 'Disk Write'
set title 'Network Input (MB/s)'
set xlabel 'Time'
set ylabel 'Input MB/s'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
plot 'png/heap_sort_20260131_225833_resources.dat' using 1:6 with linespoints ls 3 title 'Network In'
set title 'Network Output (MB/s)'
set xlabel 'Time'
set ylabel 'Output MB/s'
set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1
set border linewidth 1.5
set key outside center bottom horizontal
plot 'png/heap_sort_20260131_225833_resources.dat' using 1:7 with linespoints ls 4 title 'Network Out'
unset multiplot
