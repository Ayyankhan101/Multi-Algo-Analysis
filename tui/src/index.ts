#!/usr/bin/env node

import { getDefaultSettings, validateEnvironment } from './settings';
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
import { runAlgorithm, readCSVFile } from './runner';
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

    switch (choice) {
      case 0: // Run Binary Search
        try {
          const params = settings.algorithmParams;
          await showLiveExecutionScreen(settings.binaryPath, 'binary_search', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
            customTargets: params.useCustomTargets ? params.customTargets : undefined,
          });

          // Also run normally to capture results for display
          const output = await runAlgorithm(settings.binaryPath, 'binary_search', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
            customTargets: params.useCustomTargets ? params.customTargets : undefined,
          });

          if (output.results.length > 0) {
            // Try to load metrics from CSV for enhanced visualization
            let metrics: any[] = [];
            if (output.csvFile && fs.existsSync(path.join(settings.projectRoot, output.csvFile))) {
              try {
                metrics = await readCSVFile(path.join(settings.projectRoot, output.csvFile));
                await showEnhancedResults(output.results, metrics, 'binary_search');
              } catch (err) {
                await showExecutionScreen(output.results, output.rawOutput, 'binary_search');
              }
            } else {
              await showExecutionScreen(output.results, output.rawOutput, 'binary_search');
            }
            
            // Show export option
            await showExportScreen(output.results, 'binary_search');
          } else {
            console.log(chalk.yellow('No results captured. Check the binary output.'));
          }

          // Refresh database connection if it was just created
          if (!db) {
            try {
              const fs = require('fs');
              if (fs.existsSync(settings.databasePath)) {
                db = new DatabaseService(settings.databasePath);
              }
            } catch (err) {
              // Ignore
            }
          }
        } catch (err) {
          console.error(chalk.red(`\n❌ Error running algorithm: ${err}`));
          await waitForKeypress();
        }
        break;

      case 1: // Run Linear Search
        try {
          const params = settings.algorithmParams;
          await showLiveExecutionScreen(settings.binaryPath, 'linear_search', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
            customTargets: params.useCustomTargets ? params.customTargets : undefined,
          });

          // Also run normally to capture results for display
          const output = await runAlgorithm(settings.binaryPath, 'linear_search', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
            customTargets: params.useCustomTargets ? params.customTargets : undefined,
          });

          if (output.results.length > 0) {
            // Try to load metrics from CSV for enhanced visualization
            let metrics: any[] = [];
            if (output.csvFile && fs.existsSync(path.join(settings.projectRoot, output.csvFile))) {
              try {
                metrics = await readCSVFile(path.join(settings.projectRoot, output.csvFile));
                await showEnhancedResults(output.results, metrics, 'linear_search');
              } catch (err) {
                await showExecutionScreen(output.results, output.rawOutput, 'linear_search');
              }
            } else {
              await showExecutionScreen(output.results, output.rawOutput, 'linear_search');
            }
            
            // Show export option
            await showExportScreen(output.results, 'linear_search');
          } else {
            console.log(chalk.yellow('No results captured. Check the binary output.'));
          }

          // Refresh database connection if it was just created
          if (!db) {
            try {
              const fs = require('fs');
              if (fs.existsSync(settings.databasePath)) {
                db = new DatabaseService(settings.databasePath);
              }
            } catch (err) {
              // Ignore
            }
          }
        } catch (err) {
          console.error(chalk.red(`\n❌ Error running algorithm: ${err}`));
          await waitForKeypress();
        }
        break;

      case 2: // Run Merge Sort
        try {
          const params = settings.algorithmParams;
          await showLiveExecutionScreen(settings.binaryPath, 'merge_sort', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
          });

          // Also run normally to capture results for display
          const output = await runAlgorithm(settings.binaryPath, 'merge_sort', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
          });

          if (output.results.length > 0) {
            let metrics: any[] = [];
            if (output.csvFile && fs.existsSync(path.join(settings.projectRoot, output.csvFile))) {
              try {
                metrics = await readCSVFile(path.join(settings.projectRoot, output.csvFile));
                await showEnhancedResults(output.results, metrics, 'merge_sort');
              } catch (err) {
                await showExecutionScreen(output.results, output.rawOutput, 'merge_sort');
              }
            } else {
              await showExecutionScreen(output.results, output.rawOutput, 'merge_sort');
            }

            // Show export option
            await showExportScreen(output.results, 'merge_sort');
          } else {
            console.log(chalk.yellow('No results captured. Check the binary output.'));
          }

          // Refresh database connection if it was just created
          if (!db) {
            try {
              const fs = require('fs');
              if (fs.existsSync(settings.databasePath)) {
                db = new DatabaseService(settings.databasePath);
              }
            } catch (err) {
              // Ignore
            }
          }
        } catch (err) {
          console.error(chalk.red(`\n❌ Error running algorithm: ${err}`));
          await waitForKeypress();
        }
        break;

      case 3: // Run Insertion Sort
        try {
          const params = settings.algorithmParams;
          await showLiveExecutionScreen(settings.binaryPath, 'insertion_sort', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
          });

          const output = await runAlgorithm(settings.binaryPath, 'insertion_sort', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
          });

          if (output.results.length > 0) {
            let metrics: any[] = [];
            if (output.csvFile && fs.existsSync(path.join(settings.projectRoot, output.csvFile))) {
              try {
                metrics = await readCSVFile(path.join(settings.projectRoot, output.csvFile));
                await showEnhancedResults(output.results, metrics, 'insertion_sort');
              } catch (err) {
                await showExecutionScreen(output.results, output.rawOutput, 'insertion_sort');
              }
            } else {
              await showExecutionScreen(output.results, output.rawOutput, 'insertion_sort');
            }
            await showExportScreen(output.results, 'insertion_sort');
          } else {
            console.log(chalk.yellow('No results captured. Check the binary output.'));
          }

          if (!db) {
            try {
              const fs = require('fs');
              if (fs.existsSync(settings.databasePath)) {
                db = new DatabaseService(settings.databasePath);
              }
            } catch (err) { /* Ignore */ }
          }
        } catch (err) {
          console.error(chalk.red(`\n❌ Error running algorithm: ${err}`));
          await waitForKeypress();
        }
        break;

      case 4: // Run Selection Sort
        try {
          const params = settings.algorithmParams;
          await showLiveExecutionScreen(settings.binaryPath, 'selection_sort', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
          });

          const output = await runAlgorithm(settings.binaryPath, 'selection_sort', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
          });

          if (output.results.length > 0) {
            let metrics: any[] = [];
            if (output.csvFile && fs.existsSync(path.join(settings.projectRoot, output.csvFile))) {
              try {
                metrics = await readCSVFile(path.join(settings.projectRoot, output.csvFile));
                await showEnhancedResults(output.results, metrics, 'selection_sort');
              } catch (err) {
                await showExecutionScreen(output.results, output.rawOutput, 'selection_sort');
              }
            } else {
              await showExecutionScreen(output.results, output.rawOutput, 'selection_sort');
            }
            await showExportScreen(output.results, 'selection_sort');
          } else {
            console.log(chalk.yellow('No results captured. Check the binary output.'));
          }

          if (!db) {
            try {
              const fs = require('fs');
              if (fs.existsSync(settings.databasePath)) {
                db = new DatabaseService(settings.databasePath);
              }
            } catch (err) { /* Ignore */ }
          }
        } catch (err) {
          console.error(chalk.red(`\n❌ Error running algorithm: ${err}`));
          await waitForKeypress();
        }
        break;

      case 5: // Run Bubble Sort
        try {
          const params = settings.algorithmParams;
          await showLiveExecutionScreen(settings.binaryPath, 'bubble_sort', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
          });

          const output = await runAlgorithm(settings.binaryPath, 'bubble_sort', {
            dataSize: params.dataSize,
            dataStep: params.dataStep,
            cpuCore: params.cpuCore,
            totalRuns: params.totalRuns,
          });

          if (output.results.length > 0) {
            let metrics: any[] = [];
            if (output.csvFile && fs.existsSync(path.join(settings.projectRoot, output.csvFile))) {
              try {
                metrics = await readCSVFile(path.join(settings.projectRoot, output.csvFile));
                await showEnhancedResults(output.results, metrics, 'bubble_sort');
              } catch (err) {
                await showExecutionScreen(output.results, output.rawOutput, 'bubble_sort');
              }
            } else {
              await showExecutionScreen(output.results, output.rawOutput, 'bubble_sort');
            }
            await showExportScreen(output.results, 'bubble_sort');
          } else {
            console.log(chalk.yellow('No results captured. Check the binary output.'));
          }

          if (!db) {
            try {
              const fs = require('fs');
              if (fs.existsSync(settings.databasePath)) {
                db = new DatabaseService(settings.databasePath);
              }
            } catch (err) { /* Ignore */ }
          }
        } catch (err) {
          console.error(chalk.red(`\n❌ Error running algorithm: ${err}`));
          await waitForKeypress();
        }
        break;

      case 6: // View Historical Runs
        if (!db) {
          console.log(chalk.yellow('\n⚠ No database available. Run the algorithm first.'));
          await new Promise<void>(resolve => setTimeout(resolve, 2000));
        } else {
          await showHistoricalRuns(db);
        }
        break;

      case 7: // View Latest Results
        await showLatestResults(settings.csvPath);
        break;

      case 8: // System Info
        await showSystemInfo(settings);
        break;

      case 9: // Settings
        settings = await showSettingsScreen(settings);
        console.log(chalk.green('\n✓ Settings updated'));
        await new Promise<void>(resolve => setTimeout(resolve, 1000));
        break;

      case 10: // Exit
        running = false;
        console.log(chalk.green('\n👋 Goodbye!\n'));
        break;
    }
  }

  // Cleanup
  if (db) {
    db.close();
  }
}

// Handle errors
main().catch(err => {
  console.error(chalk.red('\n❌ Fatal error:'), err);
  process.exit(1);
});
