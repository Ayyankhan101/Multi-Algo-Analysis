import path from 'path';
import fs from 'fs';
import { AppSettings, AlgorithmParams } from './types';
import { resolveProjectRoot } from './utils';

export { resolveProjectRoot };

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

export function getSettingsPath(): string {
  // Settings live in the tui/ directory (one level above src/ or dist/)
  return path.join(__dirname, '..', '.settings.json');
}

export function getDefaultSettings(): AppSettings {
  const root = resolveProjectRoot();

  // Try to load saved settings
  let savedParams: Partial<AlgorithmParams> | null = null;
  const settingsPath = getSettingsPath();
  if (fs.existsSync(settingsPath)) {
    try {
      savedParams = JSON.parse(fs.readFileSync(settingsPath, 'utf-8'));
    } catch {
      // Ignore corrupted settings file
    }
  }

  const algorithmParams = savedParams
    ? { ...getDefaultAlgorithmParams(), ...savedParams }
    : getDefaultAlgorithmParams();

  return {
    projectRoot: root,
    databasePath: path.join(root, 'database', 'resource_metrics.db'),
    csvPath: path.join(root, 'csv'),
    pngPath: path.join(root, 'png'),
    binaryPath: path.join(root, 'build', 'resource_monitor_app'),
    algorithmParams,
  };
}

export function validateEnvironment(settings: AppSettings): string[] {
  const issues: string[] = [];

  if (!fs.existsSync(settings.binaryPath)) {
    issues.push(`Binary not found: ${settings.binaryPath}\n  Run 'cmake -B build && cmake --build build' in the project root first.`);
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
