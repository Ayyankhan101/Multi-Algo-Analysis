import blessed from 'blessed';
import { ResourceMetric, RunResult } from '../types';
import { formatAlgoName, generateSparkline } from '../utils';

/**
 * Generate a comparison table for multiple algorithm runs
 */
export function generateComparisonTable(results: RunResult[], isSearch: boolean = true): string {
  if (results.length === 0) return '';

  const lines: string[] = [];

  if (isSearch) {
    lines.push('{bold}{cyan-fg}Run │ Target     │ Status    │ CPU Time       │ Memory(KB)  │ Exec Time{/cyan-fg}{/bold}');
    lines.push('─'.repeat(80));
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
  } else {
    lines.push('{bold}{cyan-fg}Run │ CPU Time       │ Memory(KB)  │ Exec Time{/cyan-fg}{/bold}');
    lines.push('─'.repeat(55));
    results.forEach((r, i) => {
      const runNum = String(i + 1).padStart(3);
      const cpuTime = r.cpuTime.toExponential(2).padEnd(14);
      const memory = String(r.memoryUsage).padEnd(11);
      const execTime = r.execTime.toExponential(2);
      lines.push(`${runNum} │ ${cpuTime} │ ${memory} │ ${execTime}`);
    });
  }

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

  // Helper to compute min/max safely
  const safeMinMax = (arr: number[]) => {
    let min = arr[0], max = arr[0];
    for (let i = 1; i < arr.length; i++) {
      if (arr[i] < min) min = arr[i];
      if (arr[i] > max) max = arr[i];
    }
    return { min, max };
  };

  // CPU Time sparkline
  const cpuMinMax = safeMinMax(cpuTimes);
  lines.push(`{bold}CPU Time Trend{/bold}  Min: ${cpuMinMax.min.toExponential(2)}s  Max: ${cpuMinMax.max.toExponential(2)}s`);
  lines.push(`[${generateSparkline(cpuTimes, 50)}]\n`);

  // Memory sparkline
  const memMinMax = safeMinMax(memoryValues);
  lines.push(`{bold}Memory Usage Trend (KB){/bold}  Min: ${memMinMax.min}KB  Max: ${memMinMax.max}KB`);
  lines.push(`[${generateSparkline(memoryValues, 50)}]\n`);

  // Execution Time sparkline
  const execMinMax = safeMinMax(execTimes);
  lines.push(`{bold}Execution Time Trend{/bold}  Min: ${execMinMax.min.toExponential(2)}s  Max: ${execMinMax.max.toExponential(2)}s`);
  lines.push(`[${generateSparkline(execTimes, 50)}]`);

  // PerfCounter sparklines (if available)
  const instructions = metrics.map(m => m.instructions || 0).filter(v => v > 0);
  if (instructions.length > 0) {
    const instrMinMax = safeMinMax(instructions);
    lines.push(`\n{bold}Instructions Trend{/bold}  Min: ${instrMinMax.min.toLocaleString()}  Max: ${instrMinMax.max.toLocaleString()}`);
    lines.push(`[${generateSparkline(instructions, 50)}]`);
  }

  const cacheMisses = metrics.map(m => m.cache_misses || 0).filter(v => v > 0);
  if (cacheMisses.length > 0) {
    const cacheMinMax = safeMinMax(cacheMisses);
    lines.push(`\n{bold}Cache Misses Trend{/bold}  Min: ${cacheMinMax.min.toLocaleString()}  Max: ${cacheMinMax.max.toLocaleString()}`);
    lines.push(`[${generateSparkline(cacheMisses, 50)}]`);
  }

  const branchMisses = metrics.map(m => m.branch_misses || 0).filter(v => v > 0);
  if (branchMisses.length > 0) {
    const branchMinMax = safeMinMax(branchMisses);
    lines.push(`\n{bold}Branch Misses Trend{/bold}  Min: ${branchMinMax.min.toLocaleString()}  Max: ${branchMinMax.max.toLocaleString()}`);
    lines.push(`[${generateSparkline(branchMisses, 50)}]`);
  }
  
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
    
    const isSearch = algorithmName.includes('search');

    // Comparison table
    const tableY = 3;
    const tableHeight = Math.min(results.length + 2, 10);
    const tableBox = blessed.box({
      top: tableY,
      left: '5%',
      width: '90%',
      height: tableHeight,
      content: generateComparisonTable(results, isSearch),
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
    const avgCpu  = results.length > 0 ? results.reduce((sum, r) => sum + r.cpuTime, 0) / results.length : 0;
    const avgMem  = results.length > 0 ? results.reduce((sum, r) => sum + r.memoryUsage, 0) / results.length : 0;
    const avgExec = results.length > 0 ? results.reduce((sum, r) => sum + r.execTime, 0) / results.length : 0;
    const execTimes = results.map(r => r.execTime).sort((a, b) => a - b);
    const stddevExec = results.length > 1
      ? Math.sqrt(execTimes.reduce((s, t) => s + (t - avgExec) ** 2, 0) / results.length)
      : 0;
    const p95Exec = execTimes.length > 0 ? execTimes[Math.floor(execTimes.length * 0.95)] : 0;

    if (results.length === 0) {
      const footer = blessed.box({
        bottom: 0,
        left: 0,
        width: '100%',
        height: 1,
        align: 'center',
        content: '{gray-fg}Press any key to continue{/gray-fg}',
        tags: true,
      });

      const emptySummary = blessed.box({
        top: trendY + 2,
        left: '5%',
        width: '90%',
        height: 3,
        tags: true,
        content: `{bold}No results to display.{/bold}`,
        align: 'center',
      });
      screen.append(header);
      screen.append(tableBox);
      screen.append(trendBox);
      screen.append(emptySummary);
      screen.append(footer);
      screen.key(['escape', 'q', 'enter', 'C-c'], () => { resolve(); screen.destroy(); });
      screen.render();
      return;
    }

    const summaryY = trendY + 11;
    const summaryBox = blessed.box({
      top: summaryY,
      left: '5%',
      width: '90%',
      height: 7,
      content: `{bold}Summary Statistics{/bold}

  Avg Exec Time:  {yellow-fg}${avgExec.toExponential(2)}s{/yellow-fg}   Stddev: {gray-fg}${stddevExec.toExponential(2)}s{/gray-fg}   p95: {red-fg}${p95Exec.toExponential(2)}s{/red-fg}
  Avg CPU Time:   {cyan-fg}${avgCpu.toExponential(2)}s{/cyan-fg}
  Avg Memory:     {green-fg}${avgMem.toFixed(0)}KB{/green-fg}
  Total Runs:     {magenta-fg}${results.length}{/magenta-fg}${isSearch ? `
  Success Rate:   {blue-fg}${((results.filter(r => r.found).length / results.length) * 100).toFixed(0)}%{/blue-fg}` : ''}`,
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

function formatValue(value: number): string {
  if (value >= 1000) {
    return value.toExponential(2);
  }
  return value.toFixed(4);
}
