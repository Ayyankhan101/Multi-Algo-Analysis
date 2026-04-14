import blessed from 'blessed';
import { HistoricalRun, ResourceMetric } from '../types';
import { DatabaseService } from '../database';

export async function showHistoricalRuns(db: DatabaseService): Promise<void> {
  const screen = blessed.screen({
    smartCSR: true,
    title: 'Historical Runs',
  });

  const runs = db.getHistoricalRuns();

  if (runs.length === 0) {
    const msg = blessed.box({
      top: 'center',
      left: 'center',
      width: 60,
      height: 3,
      align: 'center',
      content: '{yellow-fg}No historical runs found.\nRun the algorithm first to see results.{/yellow-fg}',
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

  // Header
  const header = blessed.box({
    top: 0,
    left: 0,
    width: '100%',
    height: 1,
    align: 'center',
    content: '{bold}Historical Execution Runs{/bold}',
    tags: true,
  });

  // Table header
  const tableHeader = blessed.box({
    top: 2,
    left: '5%',
    width: '90%',
    height: 1,
    content: '{bold}{cyan-fg}#   Algorithm           Timestamp             Metrics{/cyan-fg}{/bold}',
    tags: true,
  });

  // Runs list
  const runsBox = blessed.list({
    top: 4,
    left: '5%',
    width: '90%',
    height: Math.min(runs.length + 2, 20),
    keys: true,
    vi: true,
    mouse: true,
    tags: true,
    style: {
      selected: {
        bg: 'blue',
        fg: 'white',
      },
      item: {
        bg: 'black',
        fg: 'white',
      },
    },
    items: runs.map((run, i) =>
      ` ${String(i + 1).padEnd(3)} ${run.algorithmName.padEnd(20)} ${run.timestamp.padEnd(22)} ${run.metricCount} metrics`
    ),
  });

  // Detail box
  const detailBox = blessed.box({
    top: runs.length + 7,
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
    content: '{gray-fg}Select a run to view details...{/gray-fg}',
  });

  runsBox.on('select', (item, index) => {
    const run = runs[index];
    const summary = db.getRunSummary(run.tableName);

    detailBox.setContent(`{bold}Run Details: {blue-fg}${run.algorithmName}{/blue-fg}{/bold}

  Timestamp:   {yellow-fg}${run.timestamp}{/yellow-fg}
  Table:       {yellow-fg}${run.tableName}{/yellow-fg}
  Metrics:     {yellow-fg}${run.metricCount}{/yellow-fg}
  Avg CPU Time:  {cyan-fg}${summary.avgCpuTime.toExponential(2)}s{/cyan-fg}
  Avg Memory:    {cyan-fg}${summary.avgMemory.toFixed(0)}KB{/cyan-fg}
  Avg Exec Time: {cyan-fg}${summary.avgExecTime.toExponential(2)}s{/cyan-fg}`);

    screen.render();
  });

  runsBox.on('item change', (item, index) => {
    const run = runs[index];
    const summary = db.getRunSummary(run.tableName);

    detailBox.setContent(`{bold}Run Details: {blue-fg}${run.algorithmName}{/blue-fg}{/bold}

  Timestamp:   {yellow-fg}${run.timestamp}{/yellow-fg}
  Table:       {yellow-fg}${run.tableName}{/yellow-fg}
  Metrics:     {yellow-fg}${run.metricCount}{/yellow-fg}
  Avg CPU Time:  {cyan-fg}${summary.avgCpuTime.toExponential(2)}s{/cyan-fg}
  Avg Memory:    {cyan-fg}${summary.avgMemory.toFixed(0)}KB{/cyan-fg}
  Avg Exec Time: {cyan-fg}${summary.avgExecTime.toExponential(2)}s{/cyan-fg}`);

    screen.render();
  });

  // Footer
  const footer = blessed.box({
    bottom: 0,
    left: 0,
    width: '100%',
    height: 1,
    align: 'center',
    content: '{gray-fg}↑/↓ Navigate | Enter View Details | q Back{/gray-fg}',
    tags: true,
  });

  screen.append(header);
  screen.append(tableHeader);
  screen.append(runsBox);
  screen.append(detailBox);
  screen.append(footer);

  runsBox.focus();
  screen.render();

  await new Promise<void>(resolve => {
    screen.key(['escape', 'q', 'C-c'], () => {
      screen.destroy();
      resolve();
    });
  });
}

export function showMetricsDetail(metrics: ResourceMetric[]): Promise<void> {
  return new Promise((resolve) => {
    const screen = blessed.screen({
      smartCSR: true,
      title: 'Run Metrics Details',
    });

    const content = metrics.map((m, i) => 
      `[${i + 1}] CPU: ${m.cpu_time.toExponential(2)}s | Mem: ${m.memory_usage}KB | Exec: ${m.execution_time.toExponential(2)}s`
    ).join('\n');

    const box = blessed.box({
      top: 1,
      left: '5%',
      width: '90%',
      height: '100%-2',
      content: `{bold}Metrics ({metrics.length} records){/bold}\n\n${content}`,
      tags: true,
    });

    const footer = blessed.box({
      bottom: 0,
      left: 0,
      width: '100%',
      height: 1,
      align: 'center',
      content: '{gray-fg}Press any key to go back{/gray-fg}',
      tags: true,
    });

    screen.append(box);
    screen.append(footer);

    screen.key(['escape', 'q', 'enter', 'C-c'], () => {
      screen.destroy();
      resolve();
    });

    screen.render();
  });
}
