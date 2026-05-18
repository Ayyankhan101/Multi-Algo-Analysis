#!/usr/bin/env node

import { getDefaultSettings, validateEnvironment, resolveProjectRoot } from './settings';
import { showMainMenu } from './ui/main-menu';
import { showExecutionScreen } from './ui/execution-screen';
import { showLiveExecutionScreen } from './ui/live-execution';
import { showSettingsScreen } from './ui/settings-screen';
import { showHistoricalRuns } from './ui/historical-runs';
import { showLatestResults } from './ui/latest-results';
import { showSystemInfo } from './ui/system-info';
import { showLoadingIndeterminate } from './ui/loading';
import { showEnhancedResults } from './ui/visualizations';
import { showExportScreen } from './ui/export-screen';
import { showSweepScreen, showComparisonScreen } from './ui/complexity-sweep';
import { readCSVFile } from './runner';
import { DatabaseService } from './database';
import chalk from 'chalk';
import figlet from 'figlet';
import fs from 'fs';
import path from 'path';

async function waitForKeypress(): Promise<void> {
  return new Promise<void>(resolve => {
    const blessed = require('blessed');
    const screen = blessed.screen({ smartCSR: true });
    const box = blessed.box({
      top: 'center',
      left: 'center',
      width: 60,
      height: 5,
      align: 'center',
      content: `{red-fg}Error occurred{/red-fg}\n\n{gray-fg}Press any key to continue{/gray-fg}`,
      tags: true,
    });
    screen.append(box);
    screen.render();
    screen.key(['escape', 'q', 'enter', 'C-c'], () => {
      screen.destroy();
      resolve();
    });
  });
}

async function main() {
  // Display banner
  console.clear();
  const banner = figlet.textSync('Multi-Algo', {
    font: 'Standard',
    horizontalLayout: 'default',
    verticalLayout: 'default',
  });
  
  console.log(chalk.blue.bold(banner));
  console.log(chalk.green('Resource Monitoring Dashboard v1.0.0\n'));

  // Initialize settings
  let settings = getDefaultSettings();
  
  // Validate environment
  const issues = validateEnvironment(settings);
  if (issues.length > 0) {
    console.log(chalk.yellow('⚠ Warnings:'));
    issues.forEach(issue => console.log(chalk.yellow(`  • ${issue}`)));
    console.log();
  }

  // Initialize database service (if DB exists)
  let db: DatabaseService | null = null;
  try {
    const fs = require('fs');
    if (fs.existsSync(settings.databasePath)) {
      db = new DatabaseService(settings.databasePath);
    }
  } catch (err) {
    console.log(chalk.yellow('⚠ Database not available, history features will be limited'));
  }

  // Main loop
  let running = true;
  while (running) {
    const choice = await showMainMenu(settings);

    // Helper to run an algorithm with live execution and results display
    const runAlgorithmCase = async (algoName: string) => {
      try {
        const params = settings.algorithmParams;
        const output = await showLiveExecutionScreen(settings.binaryPath, algoName, {
          dataSize: params.dataSize,
          dataStep: params.dataStep,
          cpuCore: params.cpuCore,
          totalRuns: params.totalRuns,
          customTargets: params.useCustomTargets ? params.customTargets : undefined,
        });

        if (output.results.length > 0) {
          let metrics: any[] = [];
          if (output.csvFile && fs.existsSync(path.join(settings.projectRoot, output.csvFile))) {
            try {
              metrics = await readCSVFile(path.join(settings.projectRoot, output.csvFile));
              await showEnhancedResults(output.results, metrics, algoName);
            } catch (err) {
              await showExecutionScreen(output.results, output.rawOutput, algoName);
            }
          } else {
            await showExecutionScreen(output.results, output.rawOutput, algoName);
          }
          await showExportScreen(output.results, algoName, settings.projectRoot);
        } else {
          console.log(chalk.yellow('No results captured. Check the binary output.'));
        }

        // Refresh database connection if it was just created
        if (!db) {
          try {
            if (fs.existsSync(settings.databasePath)) {
              db = new DatabaseService(settings.databasePath);
            }
          } catch (err) { /* Ignore */ }
        }
      } catch (err) {
        const errorMsg = err instanceof Error ? err.message : String(err);
        if (errorMsg.includes('Cancelled')) {
          console.log(chalk.yellow('\n⏹ Execution cancelled by user'));
        } else {
          console.error(chalk.red(`\n❌ Error running algorithm: ${err}`));
        }
        await waitForKeypress();
      }
    };

    switch (choice) {
      case 0:  await runAlgorithmCase('binary_search');        break;
      case 1:  await runAlgorithmCase('linear_search');        break;
      case 2:  await runAlgorithmCase('interpolation_search'); break;
      case 3:  await runAlgorithmCase('merge_sort');           break;
      case 4:  await runAlgorithmCase('quick_sort');           break;
      case 5:  await runAlgorithmCase('heap_sort');            break;
      case 6:  await runAlgorithmCase('shell_sort');           break;
      case 7:  await runAlgorithmCase('insertion_sort');       break;
      case 8:  await runAlgorithmCase('selection_sort');       break;
      case 9:  await runAlgorithmCase('bubble_sort');          break;

      case 10: // Complexity Sweep
        try {
          const sweepAlgo = await chooseSweepAlgorithm();
          if (sweepAlgo) {
            await showSweepScreen(settings.binaryPath, sweepAlgo, {
              cpuCore: settings.algorithmParams.cpuCore,
            }, settings.projectRoot);
          }
        } catch (err) {
          console.error(chalk.red(`\n❌ Sweep error: ${err}`));
          await new Promise<void>(resolve => setTimeout(resolve, 2000));
        }
        break;

      case 11: // Compare All Algorithms
        try {
          await showComparisonScreen(settings.binaryPath, {
            cpuCore: settings.algorithmParams.cpuCore,
          }, settings.projectRoot);
        } catch (err) {
          console.error(chalk.red(`\n❌ Comparison error: ${err}`));
          await new Promise<void>(resolve => setTimeout(resolve, 2000));
        }
        break;

      case 12: // View Historical Runs
        if (!db) {
          console.log(chalk.yellow('\n⚠ No database available. Run the algorithm first.'));
          await new Promise<void>(resolve => setTimeout(resolve, 2000));
        } else {
          await showHistoricalRuns(db);
        }
        break;

      case 13: // View Latest Results
        await showLatestResults(settings.csvPath);
        break;

      case 14: // System Info
        await showSystemInfo(settings);
        break;

      case 15: { // Settings
        const updated = await showSettingsScreen(settings);
        if (updated !== null) {
          settings = updated;
          saveSettings(settings);
          console.log(chalk.green('\n✓ Settings saved'));
          await new Promise<void>(resolve => setTimeout(resolve, 1000));
        }
        break;
      }

      case 16: // Exit
        running = false;
        console.log(chalk.green('\n Goodbye!\n'));
        break;

      default:
        console.warn('Unexpected menu choice:', choice);
        break;
    }
  }

  // Cleanup
  if (db) {
    db.close();
  }
}

// Prompt user to pick one algorithm for the complexity sweep
async function chooseSweepAlgorithm(): Promise<string | null> {
  const algorithms = [
    'binary_search', 'linear_search', 'interpolation_search',
    'merge_sort', 'quick_sort', 'heap_sort', 'shell_sort',
    'insertion_sort', 'selection_sort', 'bubble_sort',
  ];
  return new Promise<string | null>(resolve => {
    const blessed = require('blessed');
    const screen = blessed.screen({ smartCSR: true });
    const list = blessed.list({
      top: 'center', left: 'center', width: 40, height: algorithms.length + 4,
      label: ' Choose algorithm ', border: { type: 'line' },
      keys: true, vi: true, mouse: true, tags: true,
      style: { selected: { bg: 'blue' }, item: { fg: 'white' }, border: { fg: 'cyan' } },
      items: algorithms.map(a => ` ${a}`),
    });
    screen.append(list);
    list.on('select', (_item: any, idx: number) => {
      screen.destroy();
      resolve(algorithms[idx]);
    });
    screen.key(['escape', 'q', 'C-c'], () => { screen.destroy(); resolve(null); });
    list.focus();
    screen.render();
  });
}

// Save settings to disk
function saveSettings(settings: any): void {
  const settingsPath = path.join(resolveProjectRoot(), '.settings.json');
  fs.writeFileSync(settingsPath, JSON.stringify(settings.algorithmParams, null, 2));
}

// Handle errors
main().catch(err => {
  console.error(chalk.red('\n❌ Fatal error:'), err);
  process.exit(1);
});
