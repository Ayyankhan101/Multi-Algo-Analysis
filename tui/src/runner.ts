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

export function runAlgorithm(binaryPath: string): Promise<RunOutput> {
  return new Promise((resolve, reject) => {
    if (!fs.existsSync(binaryPath)) {
      reject(new Error(`Binary not found: ${binaryPath}`));
      return;
    }

    const results: RunResult[] = [];
    let rawOutput = '';
    let csvFile = '';
    let pngFile: string | undefined;

    const proc = spawn(binaryPath, [], {
      cwd: path.dirname(binaryPath),
    });

    proc.stdout.on('data', (data: Buffer) => {
      const text = data.toString();
      rawOutput += text;

      // Parse search results from output
      const searchMatch = text.match(/Searching for target: (\d+)/);
      if (searchMatch) {
        results.push({
          target: parseInt(searchMatch[1]),
          found: false,
          cpuTime: 0,
          memoryUsage: 0,
          execTime: 0,
        });
      }

      const foundMatch = text.match(/Found at index: (\d+) \(value: (\d+)\)/);
      if (foundMatch && results.length > 0) {
        const last = results[results.length - 1];
        last.found = true;
        last.index = parseInt(foundMatch[1]);
        last.value = parseInt(foundMatch[2]);
      }

      const metricsMatch = text.match(/CPU Time: ([\d.]+(?:e[+-]?\d+)?)s, Memory: (\d+)KB, Exec Time: ([\d.]+(?:e[+-]?\d+)?)/);
      if (metricsMatch && results.length > 0) {
        const last = results[results.length - 1];
        last.cpuTime = parseFloat(metricsMatch[1]);
        last.memoryUsage = parseInt(metricsMatch[2]);
        last.execTime = parseFloat(metricsMatch[3]);
      }

      const csvMatch = text.match(/Resource data saved to CSV file: (.+)/);
      if (csvMatch) {
        csvFile = csvMatch[1].trim();
      }

      const pngMatch = text.match(/Visualization: (.+)/);
      if (pngMatch) {
        pngFile = pngMatch[1].trim();
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
