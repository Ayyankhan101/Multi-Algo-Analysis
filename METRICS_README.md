# DSA Analysis Theory - Comprehensive Metrics System

This project implements a comprehensive metrics collection and visualization system for analyzing algorithm performance and resource utilization.

## Features

### Metrics Collection
- **Performance Metrics**: CPU utilization, memory usage, execution time
- **Resource Metrics**: Disk I/O, network throughput, thread count
- **Algorithm Metrics**: Cache hit rate, error rate, throughput
- **System Health**: Page faults, response time percentiles

### Visualization Capabilities
- Performance dashboards with multiple metrics
- Category-specific graphs (Performance, Resources, Algorithms)
- Algorithm comparison charts
- Statistical analysis and trend detection

### Architecture

#### Core Components
1. **MetricsCollector** - Collects real-time system and algorithm metrics
2. **DataProcessor** - Processes and analyzes collected metrics
3. **PlotGenerator** - Creates visualizations using gnuplot
4. **AnalysisManager** - Orchestrates the entire analysis workflow

#### File Organization
- `hpp/` - Header files for all components
- `src/` - Implementation files
- `csv/` - Raw and processed metric data
- `png/` - Generated visualization graphs
- `database/` - Persistent storage for metrics

## Usage

### Basic Analysis
```cpp
AnalysisManager manager;

// Define your algorithm
auto my_algorithm = []() {
    // Your algorithm implementation
};

// Perform analysis
manager.perform_analysis("my_algorithm", my_algorithm);
```

### Algorithm Comparison
```cpp
std::vector<std::pair<std::string, std::function<void()>>> algorithms = {
    {"algorithm1", []() {/* implementation */}},
    {"algorithm2", []() {/* implementation */}}
};

manager.compare_algorithms(algorithms);
```

## Generated Files

The system automatically generates:
- Raw metric data in CSV format (`csv/` directory)
- Processed statistics in CSV format (`csv/` directory)
- Performance dashboards (`png/` directory)
- Category-specific graphs (`png/` directory)
- Algorithm comparison charts (`png/` directory)

## Building and Running

```bash
# Build the main application
make

# Build and run the enhanced application with 3D capabilities
make run_3d

# Run the comprehensive analysis test
make test

# Clean generated files
make clean
```

## Enhanced Features

### Larger Datasets for Better Analysis
- **Increased array sizes**: From 50K to 200K elements for more meaningful data
- **Expanded algorithm test ranges**: More comprehensive performance evaluation
- **Higher Fibonacci values**: 30-50 for more computational intensity
- **Larger sort arrays**: 5K-25K elements for better resource variation

### Improved Data Quality
- **Reduced gnuplot warnings**: More varied data reduces "empty range" warnings
- **Better algorithm differentiation**: Clearer performance patterns in visualizations
- **More realistic metrics**: Better execution times and memory usage patterns

### Dual Visualization System
- **Traditional 2D plots**: Maintained for backward compatibility
- **Enhanced 3D visualizations**: New capabilities for deeper insights
- **Both run simultaneously**: Comprehensive analysis with multiple perspectives

### Output Statistics
- **171 total visualization files** generated (27 traditional + 144 3D)
- **All 9 algorithm types** with both 2D and 3D visualizations
- **Enhanced analysis capabilities** with multi-dimensional insights

## Key Metrics Tracked

### Performance Metrics
- CPU Utilization (%)
- Memory Usage (MB)
- Execution Time (seconds)
- Thread Count

### Resource Metrics
- Disk Read/Write I/O (MB/s)
- Network Input/Output (MB/s)
- Page Faults

### Algorithm-Specific Metrics
- Cache Hit Rate (%)
- Error Rate (%)
- Throughput (operations/second)
- Response Time Percentiles (P50, P95, P99)

## Visualization Improvements

### Timestamp Formatting
- **Relative Time Display**: Shows time as seconds elapsed since measurement start
- **Non-overlapping Labels**: X-axis labels are rotated at -45 degrees to prevent overlap
- **Millisecond Precision**: Timestamps displayed with 3 decimal places (millisecond precision)
- **Automatic Scaling**: X-axis automatically scales to fit the data range

## Visualization Types

### Performance Dashboard
- CPU Utilization over time
- Memory Usage over time
- Execution Time trends
- Thread Count variations

### Resource Utilization
- Disk I/O patterns
- Network throughput
- System resource consumption

### Algorithm Efficiency
- Cache performance
- Error rates
- Throughput analysis
- Response time distributions

### Comparative Analysis
- Side-by-side algorithm comparison
- Performance ranking
- Resource utilization differences
- Efficiency metrics comparison

### 3D Visualization
- **Enhanced 3D Surface Plots**: Time vs CPU vs Memory utilization with lighting effects
- **3D Trajectory Plots**: Performance evolution over time with connected paths
- **3D Heatmaps**: Resource correlation analysis with color mapping
- **Top-Down Views**: 2D projections of 3D performance data
- **Lighting Effects**: Enhanced visualization with specular highlights
- **Color Mapping**: Multi-dimensional data representation

## Extensibility

The system is designed to be easily extensible:
- Add new metrics by extending the `PerformanceMetrics` struct
- Create new visualization categories by adding to `MetricCategory`
- Implement custom analysis functions in the `DataProcessor`
- Extend the `MetricsCollector` with platform-specific metric gathering