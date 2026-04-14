import { spawn } from 'child_process';
import fs from 'fs';
import path from 'path';
import csv from 'csv-parser';
import { RunResult, ResourceMetric } from './types';

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
      cwd: path.dirname(binaryPath),
    });

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
      reject(err);
    });
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
    .sort()
    .reverse();

  return files.length > 0 ? path.join(csvDir, files[0]) : null;
}
