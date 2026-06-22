import path from 'path';
import fs from 'fs';

export const PROCESS_TIMEOUT_MS = 120000; // 120 seconds

export function formatAlgoName(name: string): string {
  return name
    .replace(/_/g, ' ')
    .replace(/\b\w/g, c => c.toUpperCase());
}

export function getAlgorithmDescription(name: string): string {
  switch (name) {
    case 'binary_search':        return 'O(log n) search on sorted array';
    case 'linear_search':        return 'O(n) sequential search';
    case 'interpolation_search': return 'O(log log n) avg search on uniform data';
    case 'merge_sort':           return 'O(n log n) stable sorting algorithm';
    case 'quick_sort':           return 'O(n log n) avg, O(n^2) worst-case sort';
    case 'heap_sort':            return 'O(n log n) in-place comparison sort';
    case 'shell_sort':           return 'O(n log^2 n) gap-sequence sort';
    case 'insertion_sort':       return 'O(n^2) simple sorting algorithm';
    case 'selection_sort':       return 'O(n^2) in-place sorting algorithm';
    case 'bubble_sort':          return 'O(n^2) simple comparison sorting';
    default: return '';
  }
}

export function resolveProjectRoot(startDir?: string): string {
  let dir = startDir || __dirname;
  for (let i = 0; i < 10; i++) {
    if (fs.existsSync(path.join(dir, 'CMakeLists.txt'))) return dir;
    const parent = path.dirname(dir);
    if (parent === dir) break;
    dir = parent;
  }
  // Fallback: tui lives one level below the project root
  return path.resolve(startDir || __dirname, '../..');
}

// Sparkline characters (unicode block elements)
const SPARKLINE_CHARS = ['▁', '▂', '▃', '▄', '▅', '▆', '▇', '█'];

function sampleArray(arr: number[], width: number): number[] {
  if (arr.length <= width) return arr;
  const step = arr.length / width;
  const result: number[] = [];
  for (let i = 0; i < width; i++) {
    result.push(arr[Math.floor(i * step)]);
  }
  return result;
}

export function generateSparkline(values: number[], width: number = 40): string {
  if (values.length === 0) return '';
  if (values.length === 1) return SPARKLINE_CHARS[7];

  let min = values[0];
  let max = values[0];
  for (let i = 1; i < values.length; i++) {
    if (values[i] < min) min = values[i];
    if (values[i] > max) max = values[i];
  }
  const range = max - min || 1;

  return sampleArray(values, width)
    .map(v => {
      const index = Math.floor(((v - min) / range) * (SPARKLINE_CHARS.length - 1));
      return SPARKLINE_CHARS[Math.min(index, SPARKLINE_CHARS.length - 1)];
    })
    .join('');
}
