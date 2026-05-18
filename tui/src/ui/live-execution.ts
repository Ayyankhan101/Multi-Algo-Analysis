import blessed from 'blessed';
import { spawn, ChildProcess } from 'child_process';
import path from 'path';
import fs from 'fs';
import { RunResult } from '../types';

function resolveProjectRoot(binaryPath: string): string {
  let dir = path.dirname(binaryPath);
  for (let i = 0; i < 5; i++) {
    if (fs.existsSync(path.join(dir, 'CMakeLists.txt'))) return dir;
    const parent = path.dirname(dir);
    if (parent === dir) break;
    dir = parent;
  }
  return path.dirname(binaryPath);
}

const PROCESS_TIMEOUT_MS = 60000; // 60 seconds

export interface LiveExecutionOutput {
  results: RunResult[];
  csvFile: string;
  pngFile?: string;
  rawOutput: string;
}

interface LiveMetric {
  run: number;
  sample: number;
  cpu_time: number;
  memory_usage: number;
  elapsed: number;
  target?: number;
  found?: boolean;
  index?: number;
  value?: number;
  exec_time?: number;
}

interface JsonLine {
  type: string;
  [key: string]: any;
}

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
    case 'insertion_sort': return 'O(n^2) simple sorting algorithm';
    case 'selection_sort': return 'O(n^2) in-place sorting algorithm';
    case 'bubble_sort': return 'O(n^2) simple comparison sorting';
    default: return '';
  }
}

export function showLiveExecutionScreen(
  binaryPath: string,
  algorithmName: string = 'binary_search',
  params?: {
    dataSize?: number;
    dataStep?: number;
    cpuCore?: number;
    totalRuns?: number;
    customTargets?: number[];
  }
): Promise<LiveExecutionOutput> {
  return new Promise((resolve, reject) => {
    const screen = blessed.screen({
      smartCSR: true,
      title: `Live: ${formatAlgoName(algorithmName)}`,
    });

    const displayName = formatAlgoName(algorithmName);
    const description = getAlgorithmDescription(algorithmName);

    // Header
    const header = blessed.box({
      top: 0,
      left: 0,
      width: '100%',
      height: 3,
      align: 'center',
      content: `{bold}{blue-fg}${displayName}{/blue-fg}{/bold}\n{gray-fg}${description}{/gray-fg}`,
      tags: true,
    });

    // Status bar
    const statusBar = blessed.box({
      top: 3,
      left: 0,
      width: '100%',
      height: 1,
      content: `{yellow-fg}⏳ Executing...{/yellow-fg}`,
      tags: true,
    });

    // Progress box
    const progressY = 4;
    const progressBox = blessed.box({
      top: progressY,
      left: '5%',
      width: '90%',
      height: 2,
      content: 'Run: 0/5\nProgress: ░░░░░░░░░░ 0%',
      tags: false,
    });

    // Live metrics table
    const metricsHeaderY = progressY + 3;
    const metricsHeader = blessed.box({
      top: metricsHeaderY,
      left: '5%',
      width: '90%',
      height: 1,
      content: '{bold}{cyan-fg}Run | Sample | Target     | CPU Time(s)    | Memory(KB) | Elapsed(s){/cyan-fg}{/bold}',
      tags: true,
    });

    const metricsY = metricsHeaderY + 1;
    const metricsHeight = 12;
    const metricsBox = blessed.box({
      top: metricsY,
      left: '5%',
      width: '90%',
      height: metricsHeight,
      content: '',
      tags: false,
    });

    // Summary box
    const summaryY = metricsY + metricsHeight + 1;
    const summaryBox = blessed.box({
      top: summaryY,
      left: '5%',
      width: '90%',
      height: 7,
      tags: true,
      content: 'Waiting for results...',
    });

    // Footer
    const footer = blessed.box({
      bottom: 0,
      left: 0,
      width: '100%',
      height: 1,
      align: 'center',
      content: '{gray-fg}Press q to cancel{/gray-fg}',
      tags: true,
    });

    screen.append(header);
    screen.append(statusBar);
    screen.append(progressBox);
    screen.append(metricsHeader);
    screen.append(metricsBox);
    screen.append(summaryBox);
    screen.append(footer);

    // State
    const metrics: LiveMetric[] = [];
    const results: RunResult[] = [];
    let currentRun = 0;
    let totalRuns = 5;
    let completedRuns = 0;
    let settled = false;
    let csvFile = '';
    let pngFile: string | undefined;
    let renderPending = false;

    function safeResolve() { 
      if (!settled) { 
        settled = true; 
        resolve({ results, csvFile, pngFile, rawOutput }); 
      } 
    }
    function safeReject(err: Error) { if (!settled) { settled = true; reject(err); } }

    function scheduleRender() {
      if (!renderPending) {
        renderPending = true;
        setTimeout(() => {
          renderPending = false;
          screen.render();
        }, 50); // Throttle to ~20fps max
      }
    }

    // Build command arguments
    const args: string[] = ['--algorithm', algorithmName, '--stream'];
    
    if (params?.dataSize) args.push('--data-size', String(params.dataSize));
    if (params?.dataStep) args.push('--data-step', String(params.dataStep));
    if (params?.cpuCore !== undefined) args.push('--core', String(params.cpuCore));
    if (params?.totalRuns) args.push('--runs', String(params.totalRuns));
    if (params?.customTargets && params.customTargets.length > 0) {
      args.push('--targets', params.customTargets.join(','));
    }

    const proc = spawn(binaryPath, args, {
      cwd: resolveProjectRoot(binaryPath),
    });

    // Set timeout
    const timeout = setTimeout(() => {
      proc.kill('SIGTERM');
      statusBar.setContent(`{red-fg}✗ Process timed out{/red-fg}`);
      screen.render();
      setTimeout(() => {
        safeReject(new Error(`Process timed out after ${PROCESS_TIMEOUT_MS / 1000}s`));
        screen.destroy();
      }, 2000);
    }, PROCESS_TIMEOUT_MS);

    let rawOutput = '';

    proc.stdout.on('data', (data: Buffer) => {
      const text = data.toString();
      rawOutput += text;

      // Parse JSON lines
      for (const line of text.split('\n')) {
        const trimmed = line.trim();
        if (!trimmed) continue;
        
        try {
          const parsed: JsonLine = JSON.parse(trimmed);
          
          switch (parsed.type) {
            case 'config':
              totalRuns = parsed.total_runs || 5;
              break;
              
            case 'run_start':
              currentRun = parsed.run || 0;
              // Update progress
              const progressPct = ((currentRun - 1) / totalRuns) * 100;
              const progressBar = createProgressBar(progressPct);
              progressBox.setContent(
                `Run: ${currentRun}/${totalRuns}\nProgress: ${progressBar} ${progressPct.toFixed(0)}%`
              );
              statusBar.setContent(`{yellow-fg}⏳ Running ${currentRun}/${totalRuns}...{/yellow-fg}`);
              break;
              
            case 'metrics':
              // Add live metric
              metrics.push({
                run: parsed.run || 0,
                sample: parsed.sample || 0,
                cpu_time: parsed.cpu_time || 0,
                memory_usage: parsed.memory_usage || 0,
                elapsed: parsed.elapsed || 0,
              });
              updateMetricsDisplay();
              break;
              
            case 'run_result':
              completedRuns++;
              // Update metric with final result
              metrics.push({
                run: parsed.run || 0,
                sample: 999, // Mark as final
                cpu_time: parsed.cpu_time || 0,
                memory_usage: parsed.memory_usage || 0,
                elapsed: parsed.exec_time || 0,
                target: parsed.target,
                found: parsed.found,
                index: parsed.index,
                value: parsed.value,
                exec_time: parsed.exec_time,
              });
              
              // Add to results array
              results.push({
                target: parsed.target || 0,
                found: parsed.found || false,
                index: parsed.index,
                value: parsed.value,
                cpuTime: parsed.cpu_time || 0,
                memoryUsage: parsed.memory_usage || 0,
                execTime: parsed.exec_time || 0,
              });
              
              updateMetricsDisplay();
              updateSummary();
              
              // Update progress
              const finalProgressPct = (completedRuns / totalRuns) * 100;
              const finalProgressBar = createProgressBar(finalProgressPct);
              progressBox.setContent(
                `Run: ${completedRuns}/${totalRuns}\nProgress: ${finalProgressBar} ${finalProgressPct.toFixed(0)}%`
              );
              break;
              
            case 'done':
              statusBar.setContent(`{green-fg}✓ Execution Complete{/green-fg}`);
              const completeProgressPct = 100;
              const completeProgressBar = createProgressBar(completeProgressPct);
              progressBox.setContent(
                `Run: ${totalRuns}/${totalRuns}\nProgress: ${completeProgressBar} ${completeProgressPct.toFixed(0)}%`
              );
              screen.render();

              // Auto-close after 2 seconds
              setTimeout(() => {
                safeResolve();
                screen.destroy();
              }, 2000);
              break;
              
            case 'files':
              if (parsed.csv) csvFile = parsed.csv;
              if (parsed.png) pngFile = parsed.png;
              break;
          }

          scheduleRender();
        } catch (e) {
          // Ignore non-JSON lines
        }
      }
    });

    proc.stderr.on('data', (data: Buffer) => {
      rawOutput += data.toString();
    });

    proc.on('close', (code) => {
      clearTimeout(timeout);
      if (settled) return; // Already resolved by 'done' message
      if (code === 0) {
        // If not already closed by 'done' message
        setTimeout(() => {
          safeResolve();
          screen.destroy();
        }, 2000);
      } else {
        statusBar.setContent(`{red-fg}✗ Process exited with code ${code}{/red-fg}`);
        scheduleRender();
        setTimeout(() => {
          safeReject(new Error(`Process exited with code ${code}`));
          screen.destroy();
        }, 2000);
      }
    });

    proc.on('error', (err) => {
      clearTimeout(timeout);
      statusBar.setContent(`{red-fg}✗ Error: ${err.message}{/red-fg}`);
      screen.render();
      setTimeout(() => {
        safeReject(err);
        screen.destroy();
      }, 2000);
    });

    // Allow user to cancel
    screen.key(['escape', 'q', 'C-c'], () => {
      clearTimeout(timeout);
      proc.kill();
      safeReject(new Error('Cancelled by user'));
      screen.destroy();
    });

    screen.render();

    function updateMetricsDisplay() {
      const lines = metrics.slice(-15).map(m => {
        const targetStr = m.target !== undefined ? String(m.target).padEnd(10) : 'N/A'.padEnd(10);
        const sampleStr = m.sample === 999 ? 'FINAL' : `S${m.sample}`.padEnd(6);
        const statusIcon = m.sample === 999 ? (m.found ? '{green-fg}✓{/green-fg}' : '{red-fg}✗{/red-fg}') : '⏳';
        return `  ${String(m.run).padEnd(3)} | ${sampleStr} | ${targetStr} | ${m.cpu_time.toExponential(2).padEnd(14)} | ${String(m.memory_usage).padEnd(10)} | ${m.elapsed.toExponential(2)} ${statusIcon}`;
      });
      metricsBox.setContent(lines.join('\n'));
    }

    function updateSummary() {
      const finalResults = metrics.filter(m => m.sample === 999);
      if (finalResults.length === 0) {
        summaryBox.setContent('Waiting for results...');
        return;
      }
      const foundCount = finalResults.filter(r => r.found).length;
      const avgCpu = finalResults.reduce((sum, r) => sum + r.cpu_time, 0) / finalResults.length;
      const avgMem = finalResults.reduce((sum, r) => sum + r.memory_usage, 0) / finalResults.length;
      const avgExec = finalResults.reduce((sum, r) => sum + (r.exec_time || 0), 0) / finalResults.length;

      summaryBox.setContent(
        `{bold}Summary:{/bold}
  Total Runs: {yellow-fg}${completedRuns}{/yellow-fg}
  Found: {green-fg}${foundCount}{/green-fg} | Not Found: {red-fg}${completedRuns - foundCount}{/red-fg}
  Avg CPU Time: {blue-fg}${avgCpu.toExponential(2)}s{/blue-fg}
  Avg Memory: {blue-fg}${avgMem.toFixed(0)}KB{/blue-fg}
  Avg Exec Time: {blue-fg}${avgExec.toExponential(2)}s{/blue-fg}`
      );
    }

    function createProgressBar(pct: number): string {
      const width = 20;
      const filled = Math.floor((pct / 100) * width);
      const empty = width - filled;
      return '█'.repeat(filled) + '░'.repeat(empty);
    }
  });
}
