import blessed from 'blessed';
import { ResourceMetric, RunResult } from '../types';

// Sparkline characters (unicode block elements)
const SPARKLINE_CHARS = ['▁', '▂', '▃', '▄', '▅', '▆', '▇', '█'];

/**
 * Generate a sparkline (mini line chart) from an array of values
 */
export function generateSparkline(values: number[], width: number = 40): string {
  if (values.length === 0) return '';
  if (values.length === 1) return SPARKLINE_CHARS[7];

  const min = Math.min(...values);
  const max = Math.max(...values);
  const range = max - min || 1;

  const sampledValues = sampleArray(values, width);
  
  return sampledValues
    .map(v => {
      const index = Math.floor(((v - min) / range) * (SPARKLINE_CHARS.length - 1));
      return SPARKLINE_CHARS[Math.min(index, SPARKLINE_CHARS.length - 1)];
    })
    .join('');
}

/**
 * Sample an array to fit within a specific width
 */
function sampleArray(arr: number[], width: number): number[] {
  if (arr.length <= width) return arr;
  
  const step = arr.length / width;
  const result: number[] = [];
  
  for (let i = 0; i < width; i++) {
    const index = Math.floor(i * step);
    result.push(arr[index]);
  }
  
  return result;
}

/**
 * Generate a horizontal bar chart
 */
export function generateHorizontalBarChart(
  labels: string[],
  values: number[],
  width: number = 50,
  color: string = 'cyan'
): string {
  if (labels.length === 0 || values.length === 0) return '';
  
  const maxVal = Math.max(...values);
  const minVal = Math.min(...values);
  const range = maxVal - minVal || 1;
  
  const lines: string[] = [];
  
  for (let i = 0; i < labels.length; i++) {
    const normalized = ((values[i] - minVal) / range) * width;
    const barLength = Math.max(1, Math.floor(normalized));
    const bar = '█'.repeat(barLength);
    const label = labels[i].padEnd(12);
    
    lines.push(`  ${label} │{${color}-fg}${bar}{/${color}-fg} ${formatValue(values[i])}`);
  }
  
  return lines.join('\n');
}

/**
 * Generate a comparison table for multiple algorithm runs
 */
export function generateComparisonTable(results: RunResult[]): string {
  if (results.length === 0) return '';
  
  const lines: string[] = [];
  
  // Header
  lines.push('{bold}{cyan-fg}Run │ Target     │ Status    │ CPU Time       │ Memory(KB)  │ Exec Time{/cyan-fg}{/bold}');
  lines.push('─'.repeat(80));
  
  // Data rows
  results.forEach((r, i) => {
    const runNum = String(i + 1).padStart(3);
    const target = String(r.target).padEnd(10);
    const status = r.found 
      ? '{green-fg}Found{/green-fg}'.padEnd(9) 
      : '{red-fg}Not Found{/red-fg}'.padEnd(9);
    const cpuTime = r.cpuTime.toExponential(2).padEnd(14);
    const memory = String(r.memoryUsage).padEnd(11);
    const execTime = r.execTime.toExponential(2);
    
    lines.push(`${runNum} │ ${target} │ ${status} │ ${cpuTime} │ ${memory} │ ${execTime}`);
  });
  
  return lines.join('\n');
}

/**
 * Generate multi-algorithm comparison chart
 */
export function generateMultiAlgorithmChart(
  algorithmResults: Array<{ name: string; results: RunResult[] }>
): string {
  if (algorithmResults.length === 0) return '';
  
  const lines: string[] = [];
  
  // Calculate averages for each algorithm
  const averages = algorithmResults.map(algo => {
    const avgCpu = algo.results.reduce((sum, r) => sum + r.cpuTime, 0) / algo.results.length;
    const avgMem = algo.results.reduce((sum, r) => sum + r.memoryUsage, 0) / algo.results.length;
    const avgExec = algo.results.reduce((sum, r) => sum + r.execTime, 0) / algo.results.length;
    
    return { name: algo.name, avgCpu, avgMem, avgExec };
  });
  
  lines.push('{bold}{blue-fg}Algorithm Comparison (Averages){/blue-fg}{/bold}\n');
  
  // CPU Time comparison
  lines.push('{bold}CPU Time (s){/bold}');
  lines.push(generateHorizontalBarChart(
    averages.map(a => shortenName(a.name)),
    averages.map(a => a.avgCpu),
    40,
    'cyan'
  ));
  
  lines.push('');
  
  // Memory comparison
  lines.push('{bold}Memory Usage (KB){/bold}');
  lines.push(generateHorizontalBarChart(
    averages.map(a => shortenName(a.name)),
    averages.map(a => a.avgMem),
    40,
    'green'
  ));
  
  lines.push('');
  
  // Execution Time comparison
  lines.push('{bold}Execution Time (s){/bold}');
  lines.push(generateHorizontalBarChart(
    averages.map(a => shortenName(a.name)),
    averages.map(a => a.avgExec),
    40,
    'yellow'
  ));
  
  return lines.join('\n');
}

/**
 * Generate performance trend visualization
 */
export function generatePerformanceTrend(metrics: ResourceMetric[]): string {
  if (metrics.length === 0) return '';
  
  const lines: string[] = [];
  
  // Extract values
  const cpuTimes = metrics.map(m => m.cpu_time);
  const memoryValues = metrics.map(m => m.memory_usage);
  const execTimes = metrics.map(m => m.execution_time);
  
  // CPU Time sparkline
  const minCpu = Math.min(...cpuTimes);
  const maxCpu = Math.max(...cpuTimes);
  lines.push(`{bold}CPU Time Trend{/bold}  Min: ${minCpu.toExponential(2)}s  Max: ${maxCpu.toExponential(2)}s`);
  lines.push(`[${generateSparkline(cpuTimes, 50)}]\n`);

  // Memory sparkline
  const minMem = Math.min(...memoryValues);
  const maxMem = Math.max(...memoryValues);
  lines.push(`{bold}Memory Usage Trend (KB){/bold}  Min: ${minMem}KB  Max: ${maxMem}KB`);
  lines.push(`[${generateSparkline(memoryValues, 50)}]\n`);

  // Execution Time sparkline
  const minExec = Math.min(...execTimes);
  const maxExec = Math.max(...execTimes);
  lines.push(`{bold}Execution Time Trend{/bold}  Min: ${minExec.toExponential(2)}s  Max: ${maxExec.toExponential(2)}s`);
  lines.push(`[${generateSparkline(execTimes, 50)}]`);
  
  return lines.join('\n');
}

/**
 * Generate a comprehensive visualization screen
 */
export function showEnhancedResults(
  results: RunResult[],
  metrics: ResourceMetric[],
  algorithmName: string = 'binary_search'
): Promise<void> {
  return new Promise((resolve) => {
    const screen = blessed.screen({
      smartCSR: true,
      title: `Enhanced Results: ${algorithmName}`,
    });
    
    const header = blessed.box({
      top: 0,
      left: 0,
      width: '100%',
      height: 2,
      align: 'center',
      content: `{bold}{blue-fg}${formatAlgoName(algorithmName)} - Enhanced Visualization{/blue-fg}{/bold}`,
      tags: true,
    });
    
    // Comparison table
    const tableY = 3;
    const tableHeight = Math.min(results.length + 2, 10);
    const tableBox = blessed.box({
      top: tableY,
      left: '5%',
      width: '90%',
      height: tableHeight,
      content: generateComparisonTable(results),
      tags: true,
    });

    // Performance trends
    const trendY = tableY + tableHeight + 1;
    const trendBox = blessed.box({
      top: trendY,
      left: '5%',
      width: '90%',
      height: 10,
      content: generatePerformanceTrend(metrics),
      tags: true,
    });

    // Summary statistics
    const avgCpu = results.reduce((sum, r) => sum + r.cpuTime, 0) / results.length;
    const avgMem = results.reduce((sum, r) => sum + r.memoryUsage, 0) / results.length;
    const avgExec = results.reduce((sum, r) => sum + r.execTime, 0) / results.length;

    const summaryY = trendY + 11;
    const summaryBox = blessed.box({
      top: summaryY,
      left: '5%',
      width: '90%',
      height: 7,
      content: `{bold}Summary Statistics{/bold}

  Avg CPU Time:   {cyan-fg}${avgCpu.toExponential(2)}s{/cyan-fg}
  Avg Memory:     {green-fg}${avgMem.toFixed(0)}KB{/green-fg}
  Avg Exec Time:  {yellow-fg}${avgExec.toExponential(2)}s{/yellow-fg}
  Total Runs:     {magenta-fg}${results.length}{/magenta-fg}
  Success Rate:   {blue-fg}${((results.filter(r => r.found).length / results.length) * 100).toFixed(0)}%{/blue-fg}`,
      tags: true,
    });
    
    const footer = blessed.box({
      bottom: 0,
      left: 0,
      width: '100%',
      height: 1,
      align: 'center',
      content: '{gray-fg}Press any key to continue{/gray-fg}',
      tags: true,
    });
    
    screen.append(header);
    screen.append(tableBox);
    screen.append(trendBox);
    screen.append(summaryBox);
    screen.append(footer);
    
    screen.render();
    
    screen.key(['escape', 'q', 'enter', 'C-c'], () => {
      screen.destroy();
      resolve();
    });
  });
}

/**
 * Show comparison screen for multiple algorithms
 */
export function showAlgorithmComparison(
  algorithmResults: Array<{ name: string; results: RunResult[] }>
): Promise<void> {
  return new Promise((resolve) => {
    const screen = blessed.screen({
      smartCSR: true,
      title: 'Algorithm Comparison',
    });
    
    const header = blessed.box({
      top: 0,
      left: 0,
      width: '100%',
      height: 2,
      align: 'center',
      content: '{bold}{blue-fg}Multi-Algorithm Performance Comparison{/blue-fg}{/bold}',
      tags: true,
    });
    
    // Comparison chart
    const chartBox = blessed.box({
      top: 3,
      left: '5%',
      width: '90%',
      height: 20,
      content: generateMultiAlgorithmChart(algorithmResults),
      tags: true,
    });
    
    // Individual summaries
    let summaryContent = '{bold}Individual Algorithm Details{/bold}\n\n';
    algorithmResults.forEach(algo => {
      const avgExec = algo.results.reduce((sum, r) => sum + r.execTime, 0) / algo.results.length;
      summaryContent += `{bold}${formatAlgoName(algo.name)}{/bold} (${algo.results.length} runs)\n`;
      summaryContent += `  Avg Exec Time: ${avgExec.toExponential(2)}s\n`;
      summaryContent += `  Sparkline: [${generateSparkline(algo.results.map(r => r.execTime), 40)}]\n\n`;
    });
    
    const detailsBox = blessed.box({
      top: 24,
      left: '5%',
      width: '90%',
      height: 15,
      content: summaryContent,
      tags: true,
    });
    
    const footer = blessed.box({
      bottom: 0,
      left: 0,
      width: '100%',
      height: 1,
      align: 'center',
      content: '{gray-fg}Press any key to continue{/gray-fg}',
      tags: true,
    });
    
    screen.append(header);
    screen.append(chartBox);
    screen.append(detailsBox);
    screen.append(footer);
    
    screen.render();
    
    screen.key(['escape', 'q', 'enter', 'C-c'], () => {
      screen.destroy();
      resolve();
    });
  });
}

// Helper functions
function formatAlgoName(name: string): string {
  return name
    .replace(/_/g, ' ')
    .replace(/\b\w/g, c => c.toUpperCase());
}

function shortenName(name: string): string {
  return name.replace(/_/g, ' ').substring(0, 12);
}

function formatValue(value: number): string {
  if (value >= 1000) {
    return value.toExponential(2);
  }
  return value.toFixed(4);
}
