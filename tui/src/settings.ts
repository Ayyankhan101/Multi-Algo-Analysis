import path from 'path';
import fs from 'fs';
import { AppSettings, AlgorithmParams } from './types';

export function resolveProjectRoot(): string {
  // Go up two levels from tui/dist/ directory to project root
  // __dirname points to tui/dist/, so we need ../.. to reach project root
  return path.resolve(__dirname, '../..');
}

export function getDefaultAlgorithmParams(): AlgorithmParams {
  return {
    dataSize: 1000000,
    dataStep: 3,
    cpuCore: 0,
    totalRuns: 5,
    customTargets: [0, 249998, 499998, 749997, 999999],
    useCustomTargets: false,
  };
}

export function getDefaultSettings(): AppSettings {
  const root = resolveProjectRoot();

  return {
    projectRoot: root,
    databasePath: path.join(root, 'database', 'resource_metrics.db'),
    csvPath: path.join(root, 'csv'),
    pngPath: path.join(root, 'png'),
    binaryPath: path.join(root, 'resource_monitor_app'),
    algorithmParams: getDefaultAlgorithmParams(),
  };
}

export function validateEnvironment(settings: AppSettings): string[] {
  const issues: string[] = [];

  if (!fs.existsSync(settings.binaryPath)) {
    issues.push(`Binary not found: ${settings.binaryPath}\n  Run 'make' in the project root first.`);
  }

  if (!fs.existsSync(settings.databasePath)) {
    // Not critical - will be created on first run
    console.log('Database will be created on first run.');
  }

  // Ensure output directories exist
  const dirs = [
    path.dirname(settings.databasePath),
    settings.csvPath,
    settings.pngPath,
  ];

  for (const dir of dirs) {
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, { recursive: true });
    }
  }

  return issues;
}
