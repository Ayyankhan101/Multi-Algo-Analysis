#!/usr/bin/env node

import { getDefaultSettings, validateEnvironment } from './settings';
import { showMainMenu } from './ui/main-menu';
import { showExecutionScreen } from './ui/execution-screen';
import { showHistoricalRuns } from './ui/historical-runs';
import { showLatestResults } from './ui/latest-results';
import { showSystemInfo } from './ui/system-info';
import { showLoadingIndeterminate } from './ui/loading';
import { runAlgorithm } from './runner';
import { DatabaseService } from './database';
import chalk from 'chalk';
import figlet from 'figlet';

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
  const settings = getDefaultSettings();
  
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
          const loading = showLoadingIndeterminate('Executing Binary Search...');
          
          const output = await runAlgorithm(settings.binaryPath);
          
          loading.close();
          
          if (output.results.length > 0) {
            await showExecutionScreen(output.results, output.rawOutput);
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
          await new Promise<void>(resolve => {
            const blessed = require('blessed');
            const screen = blessed.screen({ smartCSR: true });
            const box = blessed.box({
              top: 'center',
              left: 'center',
              width: 60,
              height: 5,
              align: 'center',
              content: `{red-fg}Error: ${err}{/red-fg}\n\n{gray-fg}Press any key to continue{/gray-fg}`,
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
        break;

      case 1: // View Historical Runs
        if (!db) {
          console.log(chalk.yellow('\n⚠ No database available. Run the algorithm first.'));
          await new Promise<void>(resolve => setTimeout(resolve, 2000));
        } else {
          await showHistoricalRuns(db);
        }
        break;

      case 2: // View Latest Results
        await showLatestResults(settings.csvPath);
        break;

      case 3: // System Info
        await showSystemInfo(settings);
        break;

      case 4: // Exit
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
