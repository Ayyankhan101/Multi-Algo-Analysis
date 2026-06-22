// Type definitions for Multi-Algo-Analysis

export interface ResourceMetric {
  id: number;
  timestamp: number;
  cpu_time: number;
  memory_usage: number;
  execution_time: number;
  instructions?: number;
  cache_misses?: number;
  branch_misses?: number;
}

export interface RunResult {
  target: number;
  found: boolean;
  index?: number;
  value?: number;
  cpuTime: number;
  memoryUsage: number;
  execTime: number;
}

export interface HistoricalRun {
  timestamp: string;
  tableName: string;
  algorithmName: string;
  metricCount: number;
}

export interface AlgorithmParams {
  dataSize: number;
  dataStep: number;
  cpuCore: number;
  totalRuns: number;
  customTargets: number[];
  useCustomTargets: boolean;
}

export interface AppSettings {
  projectRoot: string;
  databasePath: string;
  csvPath: string;
  pngPath: string;
  binaryPath: string;
  algorithmParams: AlgorithmParams;
}

export interface SweepPoint {
  n: number;
  execution_time: number;
  mean_time?: number;
  stddev_time?: number;
  median_time?: number;
  p95_time?: number;
  cpu_time: number;
  memory_usage: number;
  instructions?: number;
  cache_misses?: number;
  branch_misses?: number;
}

export interface SweepOutput {
  points: SweepPoint[];
  csvFile: string;
  pngFile?: string;
}

export interface ComparePoint {
  n: number;
  algorithm: string;
  execution_time: number;
}

export interface ComparisonOutput {
  points: ComparePoint[];
  csvFile: string;
  pngFile?: string;
}
