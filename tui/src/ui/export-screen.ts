import blessed from 'blessed';
import fs from 'fs';
import path from 'path';
import { RunResult, ResourceMetric } from '../types';

/**
 * Generate a text report from run results
 */
export function generateTextReport(
  results: RunResult[],
  algorithmName: string,
  timestamp: string,
  params?: {
    dataSize?: number;
    cpuCore?: number;
    totalRuns?: number;
  }
): string {
  const lines: string[] = [];
  
  // Header
  lines.push('═'.repeat(80));
  lines.push(`  Multi-Algo-Analysis Execution Report`);
  lines.push('═'.repeat(80));
  lines.push('');
  
  // Metadata
  lines.push(`Algorithm:     ${algorithmName}`);
  lines.push(`Timestamp:     ${timestamp}`);
  lines.push(`Total Runs:    ${params?.totalRuns || results.length}`);
  if (params?.dataSize) lines.push(`Data Size:     ${params.dataSize} elements`);
  if (params?.cpuCore !== undefined) lines.push(`CPU Core:      ${params.cpuCore}`);
  lines.push('');
  
  // Results table
  lines.push('─'.repeat(80));
  lines.push('EXECUTION RESULTS');
  lines.push('─'.repeat(80));
  lines.push('');
  
  results.forEach((r, i) => {
    lines.push(`Run #${i + 1}:`);
    lines.push(`  Target:       ${r.target}`);
    lines.push(`  Status:       ${r.found ? 'Found' : 'Not Found'}`);
    if (r.found && r.index !== undefined) {
      lines.push(`  Index:        ${r.index}`);
      lines.push(`  Value:        ${r.value}`);
    }
    lines.push(`  CPU Time:     ${r.cpuTime.toExponential(4)}s`);
    lines.push(`  Memory:       ${r.memoryUsage}KB`);
    lines.push(`  Exec Time:    ${r.execTime.toExponential(4)}s`);
    lines.push('');
  });
  
  // Summary statistics
  lines.push('─'.repeat(80));
  lines.push('SUMMARY STATISTICS');
  lines.push('─'.repeat(80));
  lines.push('');
  
  const avgCpu = results.reduce((sum, r) => sum + r.cpuTime, 0) / results.length;
  const avgMem = results.reduce((sum, r) => sum + r.memoryUsage, 0) / results.length;
  const avgExec = results.reduce((sum, r) => sum + r.execTime, 0) / results.length;
  const minCpu = Math.min(...results.map(r => r.cpuTime));
  const maxCpu = Math.max(...results.map(r => r.cpuTime));
  const minMem = Math.min(...results.map(r => r.memoryUsage));
  const maxMem = Math.max(...results.map(r => r.memoryUsage));
  const minExec = Math.min(...results.map(r => r.execTime));
  const maxExec = Math.max(...results.map(r => r.execTime));
  const foundCount = results.filter(r => r.found).length;
  
  lines.push(`Success Rate:     ${((foundCount / results.length) * 100).toFixed(0)}% (${foundCount}/${results.length})`);
  lines.push('');
  lines.push('CPU Time:');
  lines.push(`  Average:  ${avgCpu.toExponential(4)}s`);
  lines.push(`  Min:      ${minCpu.toExponential(4)}s`);
  lines.push(`  Max:      ${maxCpu.toExponential(4)}s`);
  lines.push('');
  lines.push('Memory Usage:');
  lines.push(`  Average:  ${avgMem.toFixed(0)}KB`);
  lines.push(`  Min:      ${minMem}KB`);
  lines.push(`  Max:      ${maxMem}KB`);
  lines.push('');
  lines.push('Execution Time:');
  lines.push(`  Average:  ${avgExec.toExponential(4)}s`);
  lines.push(`  Min:      ${minExec.toExponential(4)}s`);
  lines.push(`  Max:      ${maxExec.toExponential(4)}s`);
  lines.push('');
  
  lines.push('═'.repeat(80));
  lines.push('End of Report');
  lines.push('═'.repeat(80));
  
  return lines.join('\n');
}

/**
 * Export results to CSV format
 */
export function generateCSVReport(results: RunResult[]): string {
  const lines: string[] = [];
  
  // CSV header
  lines.push('run,target,found,index,value,cpu_time,memory_usage,exec_time');
  
  // Data rows
  results.forEach((r, i) => {
    const found = r.found ? 1 : 0;
    const index = r.index !== undefined ? r.index : '';
    const value = r.value !== undefined ? r.value : '';
    
    lines.push(`${i + 1},${r.target},${found},${index},${value},${r.cpuTime},${r.memoryUsage},${r.execTime}`);
  });
  
  return lines.join('\n');
}

/**
 * Generate a JSON report
 */
export function generateJSONReport(
  results: RunResult[],
  algorithmName: string,
  timestamp: string,
  params?: any
): string {
  const report = {
    metadata: {
      algorithm: algorithmName,
      timestamp: timestamp,
      total_runs: params?.totalRuns || results.length,
      data_size: params?.dataSize || null,
      cpu_core: params?.cpuCore || null,
    },
    results: results.map((r, i) => ({
      run: i + 1,
      target: r.target,
      found: r.found,
      index: r.index || null,
      value: r.value || null,
      cpu_time: r.cpuTime,
      memory_usage: r.memoryUsage,
      exec_time: r.execTime,
    })),
    summary: {
      success_rate: (results.filter(r => r.found).length / results.length) * 100,
      avg_cpu_time: results.reduce((sum, r) => sum + r.cpuTime, 0) / results.length,
      avg_memory_usage: results.reduce((sum, r) => sum + r.memoryUsage, 0) / results.length,
      avg_exec_time: results.reduce((sum, r) => sum + r.execTime, 0) / results.length,
    },
  };
  
  return JSON.stringify(report, null, 2);
}

/**
 * Show export screen with options
 */
export function showExportScreen(
  results: RunResult[],
  algorithmName: string,
  projectRoot: string,
  metrics?: ResourceMetric[]
): Promise<void> {
  return new Promise((resolve) => {
    const screen = blessed.screen({
      smartCSR: true,
      title: 'Export Results',
    });
    
    const header = blessed.box({
      top: 0,
      left: 0,
      width: '100%',
      height: 2,
      align: 'center',
      content: '{bold}{blue-fg}📤 Export Results{/blue-fg}{/bold}',
      tags: true,
    });
    
    const timestamp = new Date().toISOString();
    
    // Export options
    const options = [
      { label: '📄 Text Report', description: 'Generate formatted text report', key: 'text' },
      { label: '📊 CSV Export', description: 'Export results as CSV', key: 'csv' },
      { label: '🔧 JSON Report', description: 'Export as structured JSON', key: 'json' },
      { label: '📋 Full Report (Text + CSV)', description: 'Generate both text and CSV reports', key: 'full' },
    ];
    
    const optionsBox = blessed.box({
      top: 3,
      left: '10%',
      width: '80%',
      height: 10,
      tags: true,
      content: options.map((opt, i) =>
        `  {yellow-fg}${i + 1}.{/yellow-fg} {bold}${opt.label}{/bold}\n     {gray-fg}${opt.description}{/gray-fg}`
      ).join('\n\n'),
    });

    const previewY = 12;
    const previewBox = blessed.box({
      top: previewY,
      left: '5%',
      width: '90%',
      height: 15,
      tags: true,
      border: { type: 'line' },
      style: { border: { fg: 'cyan' } },
      content: '{gray-fg}Preview will appear here after selecting an export option{/gray-fg}',
    });
    
    const footer = blessed.box({
      bottom: 0,
      left: 0,
      width: '100%',
      height: 2,
      align: 'center',
      content: '{gray-fg}1-4 Select export | s Save to file | q Quit{/gray-fg}',
      tags: true,
    });
    
    screen.append(header);
    screen.append(optionsBox);
    screen.append(previewBox);
    screen.append(footer);
    
    screen.render();
    
    let selectedExport: string | null = null;
    let generatedReport = '';
    
    screen.key(['1', '2', '3', '4'], (ch) => {
      const idx = parseInt(ch) - 1;
      if (idx >= 0 && idx < options.length) {
        selectedExport = options[idx].key;
        
        // Generate preview
        switch (selectedExport) {
          case 'text':
            generatedReport = generateTextReport(results, algorithmName, timestamp);
            break;
          case 'csv':
            generatedReport = generateCSVReport(results);
            break;
          case 'json':
            generatedReport = generateJSONReport(results, algorithmName, timestamp);
            break;
          case 'full':
            generatedReport = generateTextReport(results, algorithmName, timestamp) + '\n\n--- CSV ---\n\n' + generateCSVReport(results);
            break;
        }
        
        // Show preview (truncated)
        const preview = generatedReport.substring(0, 800) + (generatedReport.length > 800 ? '\n\n... (truncated)' : '');
        previewBox.setContent(`{bold}Preview ({selectedExport}):{/bold}\n\n${preview}`);
        screen.render();
      }
    });
    
    screen.key(['s'], () => {
      if (!selectedExport || !generatedReport) {
        previewBox.setContent('{red-fg}Please select an export option first (1-4){/red-fg}');
        screen.render();
        return;
      }
      
      // Save to file
      const exportDir = path.join(projectRoot, 'exports');
      if (!fs.existsSync(exportDir)) {
        fs.mkdirSync(exportDir, { recursive: true });
      }
      
      const ext = selectedExport === 'csv' ? 'csv' : selectedExport === 'json' ? 'json' : 'txt';
      const filename = `${algorithmName}_${Date.now()}.${ext}`;
      const filepath = path.join(exportDir, filename);
      
      try {
        fs.writeFileSync(filepath, generatedReport);
        previewBox.setContent(`{green-fg}✓ Report saved to: ${filepath}{/green-fg}`);
      } catch (err) {
        previewBox.setContent(`{red-fg}✗ Error saving file: ${err}{/red-fg}`);
      }
      screen.render();
    });
    
    screen.key(['escape', 'q', 'C-c', 'enter'], () => {
      screen.destroy();
      resolve();
    });
  });
}
