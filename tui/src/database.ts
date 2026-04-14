import Database from 'better-sqlite3';
import { ResourceMetric, HistoricalRun } from './types';

export class DatabaseService {
  private db: Database.Database;

  constructor(dbPath: string) {
    this.db = new Database(dbPath);
  }

  close(): void {
    this.db.close();
  }

  getTableNames(): string[] {
    const stmt = this.db.prepare(`
      SELECT name FROM sqlite_master
      WHERE type='table' AND (name LIKE 'binary_search_%' OR name LIKE 'linear_search_%' OR name LIKE 'merge_sort_%' OR name LIKE 'insertion_sort_%' OR name LIKE 'selection_sort_%' OR name LIKE 'bubble_sort_%')
      ORDER BY name DESC
    `);
    return (stmt.all() as Array<{ name: string }>).map(r => r.name);
  }

  getHistoricalRuns(): HistoricalRun[] {
    const tables = this.getTableNames();
    return tables.map(tableName => {
      const count = this.db.prepare(`SELECT COUNT(*) as count FROM ${tableName}`).get() as { count: number };
      // Extract timestamp from table name (e.g., "binary_search_20260414_060305" -> "20260414_060305")
      const timestamp = tableName.replace(/^(binary_search|linear_search|merge_sort|insertion_sort|selection_sort|bubble_sort)_/, '');
      // Extract algorithm name from table name
      const algoName = tableName.match(/^(binary_search|linear_search|merge_sort|insertion_sort|selection_sort|bubble_sort)_/)?.[1] ?? 'unknown';
      return {
        timestamp: this.formatTimestamp(timestamp),
        tableName,
        algorithmName: this.formatAlgoName(algoName),
        metricCount: count.count,
      };
    });
  }

  getMetrics(tableName: string): ResourceMetric[] {
    const stmt = this.db.prepare(`SELECT * FROM ${tableName} ORDER BY id`);
    return stmt.all() as ResourceMetric[];
  }

  getRunSummary(tableName: string): { count: number; avgCpuTime: number; avgMemory: number; avgExecTime: number } {
    const stmt = this.db.prepare(`
      SELECT 
        COUNT(*) as count,
        AVG(cpu_time) as avgCpuTime,
        AVG(memory_usage) as avgMemory,
        AVG(execution_time) as avgExecTime
      FROM ${tableName}
    `);
    return stmt.get() as { count: number; avgCpuTime: number; avgMemory: number; avgExecTime: number };
  }

  private formatTimestamp(raw: string): string {
    // Convert "20260414_060305" to "2026-04-14 06:03:05"
    if (raw.length === 15) {
      return `${raw.slice(0, 4)}-${raw.slice(4, 6)}-${raw.slice(6, 8)} ${raw.slice(9, 11)}:${raw.slice(11, 13)}:${raw.slice(13, 15)}`;
    }
    return raw;
  }

  private formatAlgoName(name: string): string {
    return name
      .replace(/_/g, ' ')
      .replace(/\b\w/g, c => c.toUpperCase());
  }
}
