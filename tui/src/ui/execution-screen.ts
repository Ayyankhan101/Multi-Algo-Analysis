import blessed from 'blessed';
import { RunResult } from '../types';

function formatAlgoName(name: string): string {
  return name
    .replace(/_/g, ' ')
    .replace(/\b\w/g, c => c.toUpperCase());
}

function getAlgorithmDescription(name: string): string {
  switch (name) {
    case 'binary_search': return 'O(log n) search on sorted array';
    case 'linear_search': return 'O(n) sequential search';
    case 'merge_sort': return 'O(n log n) sorting algorithm';
    default: return '';
  }
}

export function showExecutionScreen(results: RunResult[], rawOutput: string, algorithmName: string = 'binary_search'): Promise<void> {
  return new Promise((resolve) => {
    const displayName = formatAlgoName(algorithmName);
    const description = getAlgorithmDescription(algorithmName);
    const isSearch = algorithmName.includes('search');

    const screen = blessed.screen({
      smartCSR: true,
      title: `${displayName} Execution Results`,
    });

    // Header
    const header = blessed.box({
      top: 0,
      left: 0,
      width: '100%',
      height: 2,
      align: 'center',
      content: `{bold}${displayName} Execution Results{/bold}\n{gray-fg}${description}{/gray-fg}`,
      tags: true,
    });

    // Results table
    const tableHeaderY = 3;
    const tableHeader = blessed.box({
      top: tableHeaderY,
      left: '5%',
      width: '90%',
      height: 1,
      content: isSearch
        ? '{bold}{cyan-fg}Run | Target     | Status    | Index    | CPU Time(s)    | Memory(KB) | Exec Time(s){/cyan-fg}{/bold}'
        : '{bold}{cyan-fg}Run | CPU Time(s)    | Memory(KB) | Exec Time(s){/cyan-fg}{/bold}',
      tags: true,
    });

    const resultsY = tableHeaderY + 2;
    const resultsHeight = Math.min(results.length + 1, 12);
    const resultsBox = blessed.box({
      top: resultsY,
      left: '5%',
      width: '90%',
      height: resultsHeight,
      tags: true,
      content: isSearch
        ? results.map((r, i) => {
            const status = r.found ? '{green-fg}✓ Found{/green-fg}' : '{red-fg}✗ Not Found{/red-fg}';
            const index = r.found && r.index !== undefined ? String(r.index) : 'N/A';
            const runNum = String(i + 1).padStart(3);
            return `  ${runNum} | ${String(r.target).padEnd(10)} | ${status.padEnd(9)} | ${index.padEnd(8)} | ${r.cpuTime.toExponential(2).padEnd(14)} | ${String(r.memoryUsage).padEnd(10)} | ${r.execTime.toExponential(2)}`;
          }).join('\n')
        : results.map((r, i) => {
            const runNum = String(i + 1).padStart(3);
            return `  ${runNum} | ${r.cpuTime.toExponential(2).padEnd(14)} | ${String(r.memoryUsage).padEnd(10)} | ${r.execTime.toExponential(2)}`;
          }).join('\n'),
    });

    // Summary
    const foundCount = results.filter(r => r.found).length;
    const avgCpu = results.reduce((sum, r) => sum + r.cpuTime, 0) / results.length;
    const avgMem = results.reduce((sum, r) => sum + r.memoryUsage, 0) / results.length;
    const avgExec = results.reduce((sum, r) => sum + r.execTime, 0) / results.length;

    const summaryY = resultsY + resultsHeight + 1;
    const summary = blessed.box({
      top: summaryY,
      left: '5%',
      width: '90%',
      height: 7,
      tags: true,
      content: `{bold}Summary:{/bold}
  ${isSearch ? `Total Searches: {yellow-fg}${results.length}{/yellow-fg}
  Found: {green-fg}${foundCount}{/green-fg} | Not Found: {red-fg}${results.length - foundCount}{/red-fg}` : `Total Runs: {yellow-fg}${results.length}{/yellow-fg}` }
  Avg CPU Time: {blue-fg}${avgCpu.toExponential(2)}s{/blue-fg}
  Avg Memory: {blue-fg}${avgMem.toFixed(0)}KB{/blue-fg}
  Avg Exec Time: {blue-fg}${avgExec.toExponential(2)}s{/blue-fg}
  Complexity: {magenta-fg}${description}{/magenta-fg}`,
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
    screen.append(resultsBox);
    screen.append(summary);
    screen.append(footer);

    screen.key(['escape', 'q', 'enter', 'C-c', 'space'], () => {
      resolve();
      screen.destroy();
    });

    screen.render();
  });
}
