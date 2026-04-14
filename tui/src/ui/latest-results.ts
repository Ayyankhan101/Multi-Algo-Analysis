import blessed from 'blessed';
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

  if (metrics.length === 0) {
    const msg = blessed.box({
      top: 'center',
      left: 'center',
      width: 60,
      height: 3,
      align: 'center',
      content: '{yellow-fg}CSV file is empty.{/yellow-fg}',
      tags: true,
    });
    screen.append(msg);
    screen.render();
    await new Promise<void>(resolve => {
      screen.key(['escape', 'q', 'enter', 'C-c'], () => { screen.destroy(); resolve(); });
    });
    return;
  }

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
  const tableHeaderY = 3;
  const tableHeader = blessed.box({
    top: tableHeaderY,
    left: '5%',
    width: '90%',
    height: 1,
    content: '{bold}{cyan-fg}#   | Timestamp        | CPU Time(s)      | Memory(KB)   | Exec Time(s){/cyan-fg}{/bold}',
    tags: true,
  });

  // Metrics display
  const metricsY = tableHeaderY + 2;
  const metricsHeight = Math.min(metrics.length + 1, 12);
  const metricsContent = metrics.map((m, i) =>
    ` ${String(i + 1).padStart(3)} | ${m.timestamp.toFixed(2).padEnd(16)} | ${m.cpu_time.toExponential(2).padEnd(14)} | ${String(m.memory_usage).padEnd(11)} | ${m.execution_time.toExponential(2)}`
  ).join('\n');

  const metricsBox = blessed.box({
    top: metricsY,
    left: '5%',
    width: '90%',
    height: metricsHeight,
    content: metricsContent,
    tags: true,
  });

  // Summary stats
  const avgCpu = metrics.reduce((sum, m) => sum + m.cpu_time, 0) / metrics.length;
  const avgMem = metrics.reduce((sum, m) => sum + m.memory_usage, 0) / metrics.length;
  const avgExec = metrics.reduce((sum, m) => sum + m.execution_time, 0) / metrics.length;
  const minMem = metrics.reduce((min, m) => Math.min(min, m.memory_usage), Infinity);
  const maxMem = metrics.reduce((max, m) => Math.max(max, m.memory_usage), -Infinity);

  const summaryY = metricsY + metricsHeight + 1;
  const summary = blessed.box({
    top: summaryY,
    left: '5%',
    width: '90%',
    height: 7,
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
  screen.append(footer);

  screen.render();

  await new Promise<void>(resolve => {
    screen.key(['escape', 'q', 'enter', 'C-c'], () => {
      screen.destroy();
      resolve();
    });
  });
}
