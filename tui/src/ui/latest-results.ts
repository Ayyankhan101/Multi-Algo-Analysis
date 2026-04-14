import blessed from 'blessed';
import { ResourceMetric } from '../types';
import { readCSVFile, findLatestCSV } from '../runner';
import path from 'path';

export async function showLatestResults(csvDir: string): Promise<void> {
  const screen = blessed.screen({
    smartCSR: true,
    title: 'Latest Results',
  });

  const latestCSV = findLatestCSV(csvDir);

  if (!latestCSV) {
    const msg = blessed.box({
      top: 'center',
      left: 'center',
      width: 60,
      height: 3,
      align: 'center',
      content: '{yellow-fg}No CSV files found.\nRun the algorithm first to generate data.{/yellow-fg}',
      tags: true,
    });
    screen.append(msg);
    screen.render();

    await new Promise<void>(resolve => {
      screen.key(['escape', 'q', 'enter', 'C-c'], () => {
        screen.destroy();
        resolve();
      });
    });
    return;
  }

  const metrics = await readCSVFile(latestCSV);

  // Header
  const header = blessed.box({
    top: 0,
    left: 0,
    width: '100%',
    height: 2,
    align: 'center',
    content: `{bold}Latest Results: {blue-fg}${path.basename(latestCSV)}{/blue-fg}{/bold}`,
    tags: true,
  });

  // Table header
  const tableHeader = blessed.box({
    top: 3,
    left: '5%',
    width: '90%',
    height: 1,
    content: '{bold}{cyan-fg}#   Timestamp        CPU Time(s)      Memory(KB)   Exec Time(s){/cyan-fg}{/bold}',
    tags: true,
  });

  // Metrics display
  const metricsContent = metrics.map((m, i) => 
    ` ${String(i + 1).padEnd(3)} ${m.timestamp.toFixed(2).padEnd(16)} ${m.cpu_time.toExponential(2).padEnd(14)} ${String(m.memory_usage).padEnd(11)} ${m.execution_time.toExponential(2)}`
  ).join('\n');

  const metricsBox = blessed.box({
    top: 5,
    left: '5%',
    width: '90%',
    height: Math.min(metrics.length + 2, 15),
    content: metricsContent,
    tags: true,
  });

  // Summary stats
  const avgCpu = metrics.reduce((sum, m) => sum + m.cpu_time, 0) / metrics.length;
  const avgMem = metrics.reduce((sum, m) => sum + m.memory_usage, 0) / metrics.length;
  const avgExec = metrics.reduce((sum, m) => sum + m.execution_time, 0) / metrics.length;
  const minMem = Math.min(...metrics.map(m => m.memory_usage));
  const maxMem = Math.max(...metrics.map(m => m.memory_usage));

  const summary = blessed.box({
    top: metrics.length + 7,
    left: '5%',
    width: '90%',
    height: 8,
    tags: true,
    border: {
      type: 'line',
    },
    style: {
      border: {
        fg: 'cyan',
      },
    },
    content: `{bold}Summary Statistics{/bold}

  Total Measurements: {yellow-fg}${metrics.length}{/yellow-fg}
  Avg CPU Time:   {cyan-fg}${avgCpu.toExponential(2)}s{/cyan-fg}
  Avg Memory:     {cyan-fg}${avgMem.toFixed(0)}KB{/cyan-fg}  (Range: {green-fg}${minMem}{/green-fg} - {red-fg}${maxMem}{/red-fg}KB)
  Avg Exec Time:  {cyan-fg}${avgExec.toExponential(2)}s{/cyan-fg}`,
  });

  // ASCII Bar Chart for Memory
  const chartTitle = '{bold}Memory Usage Chart (KB){/bold}';
  const maxVal = Math.max(...metrics.map(m => m.memory_usage));
  const minVal = Math.min(...metrics.map(m => m.memory_usage));
  const range = maxVal - minVal || 1;
  const barWidth = 40;

  const bars = metrics.slice(0, 10).map((m, i) => {
    const normalized = ((m.memory_usage - minVal) / range) * barWidth;
    const bar = '█'.repeat(Math.max(1, Math.floor(normalized)));
    return ` ${String(i + 1).padEnd(2)} │${bar} ${m.memory_usage}`;
  }).join('\n');

  const chartBox = blessed.box({
    top: metrics.length + 16,
    left: '5%',
    width: '90%',
    height: Math.min(metrics.length, 10) + 2,
    content: `${chartTitle}\n\n${bars}`,
    tags: true,
  });

  // Footer
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
  screen.append(tableHeader);
  screen.append(metricsBox);
  screen.append(summary);
  screen.append(chartBox);
  screen.append(footer);

  screen.render();

  await new Promise<void>(resolve => {
    screen.key(['escape', 'q', 'enter', 'C-c'], () => {
      screen.destroy();
      resolve();
    });
  });
}
