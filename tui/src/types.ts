// Type definitions for Multi-Algo-Analysis

export interface ResourceMetric {
  id: number;
  timestamp: number;
  cpu_time: number;
  memory_usage: number;
  execution_time: number;
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

export interface RunConfig {
  algorithm: string;
  cpuCore: number;
  arraySize: number;
  arrayStep: number;
  targets: number[];
}

export interface HistoricalRun {
  timestamp: string;
  tableName: string;
  metricCount: number;
}

export interface AppSettings {
  projectRoot: string;
  databasePath: string;
  csvPath: string;
  pngPath: string;
  binaryPath: string;
}
