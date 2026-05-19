import { spawn, ChildProcess } from 'child_process';
import fs from 'fs';
import path from 'path';
import csv from 'csv-parser';
import { RunResult, ResourceMetric, SweepPoint, SweepOutput, ComparePoint, ComparisonOutput } from './types';

const PROCESS_TIMEOUT_MS = 120000; // 120 seconds

function resolveRunCwd(binaryPath: string): string {
  // Walk up from the binary's directory to find the project root (CMakeLists.txt).
  // The binary lives in build/ so we need one level up.
  let dir = path.dirname(binaryPath);
  for (let i = 0; i < 5; i++) {
    if (fs.existsSync(path.join(dir, 'CMakeLists.txt'))) return dir;
    const parent = path.dirname(dir);
    if (parent === dir) break;
    dir = parent;
  }
  return path.dirname(binaryPath);
}

export interface RunOutput {
  results: RunResult[];
  csvFile: string;
  pngFile?: string;
  rawOutput: string;
}

interface JsonLine {
  type: string;
  [key: string]: any;
}

export function runAlgorithm(
  binaryPath: string,
  algorithmName: string = 'binary_search',
  params?: {
    dataSize?: number;
    dataStep?: number;
    cpuCore?: number;
    totalRuns?: number;
    customTargets?: number[];
  }
): Promise<RunOutput> {
  return new Promise((resolve, reject) => {
    if (!fs.existsSync(binaryPath)) {
      reject(new Error(`Binary not found: ${binaryPath}`));
      return;
    }

    const results: RunResult[] = [];
    let rawOutput = '';
    let csvFile = '';
    let pngFile: string | undefined;

    // Build command arguments
    const args: string[] = ['--algorithm', algorithmName, '--json'];
    
    if (params?.dataSize) args.push('--data-size', String(params.dataSize));
    if (params?.dataStep) args.push('--data-step', String(params.dataStep));
    if (params?.cpuCore !== undefined) args.push('--core', String(params.cpuCore));
    if (params?.totalRuns) args.push('--runs', String(params.totalRuns));
    if (params?.customTargets && params.customTargets.length > 0) {
      args.push('--targets', params.customTargets.join(','));
    }

    const proc = spawn(binaryPath, args, {
      cwd: resolveRunCwd(binaryPath),
    });

    // Set timeout
    const timeout = setTimeout(() => {
      proc.kill('SIGTERM');
      reject(new Error(`Process timed out after ${PROCESS_TIMEOUT_MS / 1000}s`));
    }, PROCESS_TIMEOUT_MS);

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
            case 'run_start':
              // Start a new run
              results.push({
                target: parsed.target || 0,
                found: false,
                cpuTime: 0,
                memoryUsage: 0,
                execTime: 0,
              });
              break;
              
            case 'run_result':
              // Update the last run with results
              if (results.length > 0) {
                const last = results[results.length - 1];
                last.found = parsed.found || false;
                if (parsed.index !== undefined) last.index = parsed.index;
                if (parsed.value !== undefined) last.value = parsed.value;
                last.cpuTime = parsed.cpu_time || 0;
                last.memoryUsage = parsed.memory_usage || 0;
                last.execTime = parsed.exec_time || 0;
              }
              break;
              
            case 'files':
              if (parsed.csv) csvFile = parsed.csv;
              if (parsed.png) pngFile = parsed.png;
              break;
          }
        } catch (e) {
          // Ignore non-JSON lines (e.g., stderr mixed in)
        }
      }
    });

    proc.stderr.on('data', (data: Buffer) => {
      rawOutput += data.toString();
    });

    proc.on('close', (code) => {
      clearTimeout(timeout);
      if (code !== 0) {
        reject(new Error(`Process exited with code ${code}`));
      } else {
        resolve({
          results,
          csvFile,
          pngFile,
          rawOutput,
        });
      }
    });

    proc.on('error', (err) => {
      clearTimeout(timeout);
      reject(err);
    });
  });
}

export interface SweepParams {
  sweepMin?: number;
  sweepMax?: number;
  sweepPoints?: number;
  cpuCore?: number;
}

const SWEEP_TIMEOUT_MS = 300000; // 5 minutes — quadratic sorts are slow

export function runSweep(
  binaryPath: string,
  algorithmName: string,
  params: SweepParams = {},
  onPoint?: (point: SweepPoint) => void
): Promise<SweepOutput> {
  return new Promise((resolve, reject) => {
    if (!fs.existsSync(binaryPath)) {
      reject(new Error(`Binary not found: ${binaryPath}`));
      return;
    }

    const args: string[] = ['--algorithm', algorithmName, '--sweep', '--json'];
    if (params.sweepMin) args.push('--sweep-min', String(params.sweepMin));
    if (params.sweepMax) args.push('--sweep-max', String(params.sweepMax));
    if (params.sweepPoints) args.push('--sweep-points', String(params.sweepPoints));
    if (params.cpuCore !== undefined) args.push('--core', String(params.cpuCore));

    const points: SweepPoint[] = [];
    let csvFile = '';
    let pngFile: string | undefined;

    const proc = spawn(binaryPath, args, { cwd: path.dirname(binaryPath) });

    const timeout = setTimeout(() => {
      proc.kill('SIGTERM');
      reject(new Error(`Sweep timed out after ${SWEEP_TIMEOUT_MS / 1000}s`));
    }, SWEEP_TIMEOUT_MS);

    proc.stdout.on('data', (data: Buffer) => {
      for (const line of data.toString().split('\n')) {
        const trimmed = line.trim();
        if (!trimmed) continue;
        try {
          const parsed = JSON.parse(trimmed);
          if (parsed.type === 'sweep_point') {
            const pt: SweepPoint = {
              n: parsed.n,
              execution_time: parsed.execution_time,
              mean_time:   parsed.mean_time,
              stddev_time: parsed.stddev_time,
              median_time: parsed.median_time,
              p95_time:    parsed.p95_time,
              cpu_time: parsed.cpu_time,
              memory_usage: parsed.memory_usage,
            };
            points.push(pt);
            if (onPoint) onPoint(pt);
          } else if (parsed.type === 'sweep_done') {
            if (parsed.csv) csvFile = parsed.csv;
            if (parsed.png) pngFile = parsed.png;
          }
        } catch { /* ignore non-JSON */ }
      }
    });

    proc.on('close', (code) => {
      clearTimeout(timeout);
      if (code === 0) resolve({ points, csvFile, pngFile });
      else reject(new Error(`Sweep process exited with code ${code}`));
    });

    proc.on('error', (err) => { clearTimeout(timeout); reject(err); });
  });
}

export function runComparison(
  binaryPath: string,
  params: SweepParams = {},
  onPoint?: (point: ComparePoint) => void
): Promise<ComparisonOutput> {
  return new Promise((resolve, reject) => {
    if (!fs.existsSync(binaryPath)) {
      reject(new Error(`Binary not found: ${binaryPath}`));
      return;
    }

    const args: string[] = ['--compare', '--json'];
    if (params.sweepMin) args.push('--sweep-min', String(params.sweepMin));
    if (params.sweepMax) args.push('--sweep-max', String(params.sweepMax));
    if (params.sweepPoints) args.push('--sweep-points', String(params.sweepPoints));
    if (params.cpuCore !== undefined) args.push('--core', String(params.cpuCore));

    const points: ComparePoint[] = [];
    let csvFile = '';
    let pngFile: string | undefined;

    const proc = spawn(binaryPath, args, { cwd: path.dirname(binaryPath) });

    const timeout = setTimeout(() => {
      proc.kill('SIGTERM');
      reject(new Error(`Comparison timed out after ${SWEEP_TIMEOUT_MS / 1000}s`));
    }, SWEEP_TIMEOUT_MS);

    proc.stdout.on('data', (data: Buffer) => {
      for (const line of data.toString().split('\n')) {
        const trimmed = line.trim();
        if (!trimmed) continue;
        try {
          const parsed = JSON.parse(trimmed);
          if (parsed.type === 'compare_point') {
            const pt: ComparePoint = {
              n: parsed.n,
              algorithm: parsed.algorithm,
              execution_time: parsed.execution_time,
            };
            points.push(pt);
            if (onPoint) onPoint(pt);
          } else if (parsed.type === 'compare_done') {
            if (parsed.csv) csvFile = parsed.csv;
            if (parsed.png) pngFile = parsed.png;
          }
        } catch { /* ignore non-JSON */ }
      }
    });

    proc.on('close', (code) => {
      clearTimeout(timeout);
      if (code === 0) resolve({ points, csvFile, pngFile });
      else reject(new Error(`Comparison process exited with code ${code}`));
    });

    proc.on('error', (err) => { clearTimeout(timeout); reject(err); });
  });
}

export function readCSVFile(filePath: string): Promise<ResourceMetric[]> {
  return new Promise((resolve, reject) => {
    const results: ResourceMetric[] = [];

    fs.createReadStream(filePath)
      .pipe(csv())
      .on('data', (row: any) => {
        results.push({
          id: parseInt(row.id) || 0,
          timestamp: parseFloat(row.timestamp) || 0,
          cpu_time: parseFloat(row.cpu_time) || 0,
          memory_usage: parseInt(row.memory_usage) || 0,
          execution_time: parseFloat(row.execution_time) || 0,
        });
      })
      .on('end', () => resolve(results))
      .on('error', reject);
  });
}

export function findLatestCSV(csvDir: string): string | null {
  if (!fs.existsSync(csvDir)) return null;

  const files = fs.readdirSync(csvDir)
    .filter(f => f.endsWith('.csv'))
    .map(f => ({
      name: f,
      path: path.join(csvDir, f),
      mtime: fs.statSync(path.join(csvDir, f)).mtimeMs,
    }))
    .sort((a, b) => b.mtime - a.mtime);

  return files.length > 0 ? files[0].path : null;
}
