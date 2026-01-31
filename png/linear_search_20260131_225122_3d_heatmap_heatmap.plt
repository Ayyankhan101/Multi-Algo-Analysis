# GNUplot script for 3D heatmaps
set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000
set output 'png/linear_search_20260131_225122_3d_heatmap_heatmap.png'

set title 'Performance Correlation Heatmap' font 'Helvetica,16'
set xlabel 'Time (s)' font 'Helvetica,12'
set ylabel 'CPU Utilization (%)' font 'Helvetica,12'
set zlabel 'Memory Usage (MB)' font 'Helvetica,12'

# Configure pm3d for heatmap
set pm3d map
set palette defined (0 '#0000ff', 1 '#00ffff', 2 '#00ff00', 3 '#ffff00', 4 '#ff0000')
set cbrange [0:]
set view map

# 2D heatmap projection (since we have time-series data)
plot 'png/linear_search_20260131_225122_3d_heatmap_heatmap.dat' using 1:2:3 with points palette pt 7 ps 1.2 title 'CPU vs Time Colored by Memory'
