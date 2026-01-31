# GNUplot script for enhanced 3D surface plots
set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000
set output 'png/insertion_sort_20260131_225833_3d_enhanced_3d_surface.png'

set title 'Algorithm Performance 3D Surface' font 'Helvetica,16'
set xlabel 'Time (s)' font 'Helvetica,12'
set ylabel 'CPU Utilization (%)' font 'Helvetica,12'
set zlabel 'Memory Usage (MB)' font 'Helvetica,12'

# Enhanced 3D surface visualization
set view 60, 30
set pm3d depthorder
set pm3d lighting primary 0.4 specular 0.2
set palette defined (0 '#0000ff', 1 '#00ffff', 2 '#00ff00', 3 '#ffff00', 4 '#ff0000')
set style line 1 lc rgb '#2E86AB' lt 1 lw 2
set style line 2 lc rgb '#A23B72' lt 1 lw 2
set style line 3 lc rgb '#F18F01' lt 1 lw 2

# 3D surface showing performance evolution
splot 'png/insertion_sort_20260131_225833_3d_enhanced_3d.dat' using 1:2:3 with linespoints linewidth 2 pointtype 7 pointsize 0.8 title 'Performance Evolution'

# Set output for trajectory view
set output 'png/insertion_sort_20260131_225833_3d_enhanced_3d_trajectory.png'
set title 'Performance Trajectory Over Time' font 'Helvetica,16'
splot 'png/insertion_sort_20260131_225833_3d_enhanced_3d.dat' using 1:2:3 with lines linewidth 3 lc rgb '#F18F01' title 'Performance Path'

# Set output for top-down view
set output 'png/insertion_sort_20260131_225833_3d_enhanced_3d_topview.png'
set title 'Performance Map (Top View)' font 'Helvetica,16'
set view map
set pm3d map
plot 'png/insertion_sort_20260131_225833_3d_enhanced_3d.dat' using 1:2:3 with points palette pointtype 7 pointsize 1.2 title 'Resource Distribution'
