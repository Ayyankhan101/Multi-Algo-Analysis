import blessed from 'blessed';
import path from 'path';
import { runSweep, runComparison, SweepParams } from '../runner';
import { SweepPoint, ComparePoint } from '../types';

function formatAlgo(name: string): string {
  return name.replace(/_/g, ' ').replace(/\b\w/g, c => c.toUpperCase());
}

function fmtTime(t: number): string {
  if (t < 1e-6) return `${(t * 1e9).toFixed(1)}ns`;
  if (t < 1e-3) return `${(t * 1e6).toFixed(1)}µs`;
  if (t < 1)    return `${(t * 1e3).toFixed(1)}ms`;
  return `${t.toFixed(3)}s`;
}

export function showSweepScreen(
  binaryPath: string,
  algorithmName: string,
  params: SweepParams,
  projectRoot: string
): Promise<void> {
  return new Promise((resolve, reject) => {
    const screen = blessed.screen({ smartCSR: true, title: `Sweep: ${formatAlgo(algorithmName)}` });

    const header = blessed.box({
      top: 0, left: 0, width: '100%', height: 3, align: 'center', tags: true,
      content: `{bold}{blue-fg}Complexity Sweep: ${formatAlgo(algorithmName)}{/blue-fg}{/bold}\n{gray-fg}Varying input size N to reveal time complexity{/gray-fg}`,
    });

    const statusBar = blessed.box({
      top: 3, left: 0, width: '100%', height: 1, tags: true,
      content: '{yellow-fg}⏳ Starting sweep...{/yellow-fg}',
    });

    const tableHeader = blessed.box({
      top: 5, left: '3%', width: '94%', height: 1, tags: true,
      content: '{bold}{cyan-fg}         N | Exec Time   | CPU Time    | Memory (KB){/cyan-fg}{/bold}',
    });

    const tableBox = blessed.box({
      top: 6, left: '3%', width: '94%', height: 14, tags: true, content: '',
    });

    const summaryBox = blessed.box({
      top: 21, left: '3%', width: '94%', height: 4, tags: true, content: '',
    });

    const footer = blessed.box({
      bottom: 0, left: 0, width: '100%', height: 1, align: 'center', tags: true,
      content: '{gray-fg}Press q to cancel{/gray-fg}',
    });

    screen.append(header);
    screen.append(statusBar);
    screen.append(tableHeader);
    screen.append(tableBox);
    screen.append(summaryBox);
    screen.append(footer);

    const rows: string[] = [];
    let settled = false;

    function finish() {
      if (!settled) { settled = true; screen.destroy(); resolve(); }
    }

    screen.key(['escape', 'q', 'C-c'], finish);
    screen.render();

    runSweep(binaryPath, algorithmName, params, (pt: SweepPoint) => {
      const row = `  ${String(pt.n).padStart(9)} | ${fmtTime(pt.execution_time).padEnd(11)} | ${fmtTime(pt.cpu_time).padEnd(11)} | ${pt.memory_usage}`;
      rows.push(row);
      tableBox.setContent(rows.slice(-12).join('\n'));
      statusBar.setContent(`{yellow-fg}⏳ Measured n=${pt.n}  exec=${fmtTime(pt.execution_time)}{/yellow-fg}`);
      screen.render();
    }).then((output) => {
      const fastest = output.points.reduce((a, b) => a.execution_time < b.execution_time ? a : b, output.points[0]);
      const slowest = output.points.reduce((a, b) => a.execution_time > b.execution_time ? a : b, output.points[0]);
      statusBar.setContent(`{green-fg}✓ Sweep complete — ${output.points.length} data points{/green-fg}`);
      summaryBox.setContent(
        `{bold}Results:{/bold}  fastest n=${fastest?.n} (${fmtTime(fastest?.execution_time ?? 0)})  slowest n=${slowest?.n} (${fmtTime(slowest?.execution_time ?? 0)})\n` +
        `{cyan-fg}CSV:{/cyan-fg} ${path.join(projectRoot, output.csvFile)}\n` +
        `{cyan-fg}Plot:{/cyan-fg} ${output.pngFile ? path.join(projectRoot, output.pngFile) : 'N/A (gnuplot not installed)'}\n` +
        `{gray-fg}Press any key to return{/gray-fg}`
      );
      footer.setContent('{gray-fg}Press any key to return{/gray-fg}');
      screen.key(['escape', 'q', 'C-c', 'enter', 'space'], finish);
      screen.render();
    }).catch((err) => {
      if (!settled) {
        statusBar.setContent(`{red-fg}✗ ${err.message}{/red-fg}`);
        footer.setContent('{gray-fg}Press any key to return{/gray-fg}');
        screen.key(['escape', 'q', 'C-c', 'enter', 'space'], finish);
        screen.render();
      }
    });
  });
}

export function showComparisonScreen(
  binaryPath: string,
  params: SweepParams,
  projectRoot: string
): Promise<void> {
  return new Promise((resolve, reject) => {
    const screen = blessed.screen({ smartCSR: true, title: 'Algorithm Comparison Sweep' });

    const header = blessed.box({
      top: 0, left: 0, width: '100%', height: 3, align: 'center', tags: true,
      content: '{bold}{blue-fg}Algorithm Comparison Sweep{/blue-fg}{/bold}\n{gray-fg}All 6 algorithms benchmarked across input sizes (sorts use reverse-sorted data){/gray-fg}',
    });

    const statusBar = blessed.box({
      top: 3, left: 0, width: '100%', height: 1, tags: true,
      content: '{yellow-fg}⏳ Starting comparison...{/yellow-fg}',
    });

    const tableHeader = blessed.box({
      top: 5, left: '3%', width: '94%', height: 1, tags: true,
      content: '{bold}{cyan-fg}         N | Algorithm           | Exec Time{/cyan-fg}{/bold}',
    });

    const tableBox = blessed.box({
      top: 6, left: '3%', width: '94%', height: 14, tags: true, content: '',
    });

    const summaryBox = blessed.box({
      top: 21, left: '3%', width: '94%', height: 4, tags: true, content: '',
    });

    const footer = blessed.box({
      bottom: 0, left: 0, width: '100%', height: 1, align: 'center', tags: true,
      content: '{gray-fg}Press q to cancel{/gray-fg}',
    });

    screen.append(header);
    screen.append(statusBar);
    screen.append(tableHeader);
    screen.append(tableBox);
    screen.append(summaryBox);
    screen.append(footer);

    const rows: string[] = [];
    let settled = false;
    let lastN = 0;

    function finish() {
      if (!settled) { settled = true; screen.destroy(); resolve(); }
    }

    screen.key(['escape', 'q', 'C-c'], finish);
    screen.render();

    runComparison(binaryPath, params, (pt: ComparePoint) => {
      if (pt.n !== lastN) { rows.push(''); lastN = pt.n; }
      const row = `  ${String(pt.n).padStart(9)} | ${pt.algorithm.padEnd(20)} | ${fmtTime(pt.execution_time)}`;
      rows.push(row);
      tableBox.setContent(rows.slice(-13).join('\n'));
      statusBar.setContent(`{yellow-fg}⏳ n=${pt.n}  ${pt.algorithm}  ${fmtTime(pt.execution_time)}{/yellow-fg}`);
      screen.render();
    }).then((output) => {
      statusBar.setContent(`{green-fg}✓ Comparison complete — ${output.points.length} measurements{/green-fg}`);
      summaryBox.setContent(
        `{cyan-fg}CSV:{/cyan-fg} ${path.join(projectRoot, output.csvFile)}\n` +
        `{cyan-fg}Plot:{/cyan-fg} ${output.pngFile ? path.join(projectRoot, output.pngFile) : 'N/A (gnuplot not installed)'}\n` +
        `{gray-fg}The plot shows log-log, search comparison, sort comparison, and linear scale views.{/gray-fg}\n` +
        `{gray-fg}Press any key to return{/gray-fg}`
      );
      footer.setContent('{gray-fg}Press any key to return{/gray-fg}');
      screen.key(['escape', 'q', 'C-c', 'enter', 'space'], finish);
      screen.render();
    }).catch((err) => {
      if (!settled) {
        statusBar.setContent(`{red-fg}✗ ${err.message}{/red-fg}`);
        footer.setContent('{gray-fg}Press any key to return{/gray-fg}');
        screen.key(['escape', 'q', 'C-c', 'enter', 'space'], finish);
        screen.render();
      }
    });
  });
}
