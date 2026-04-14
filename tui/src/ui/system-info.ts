import blessed from 'blessed';
import { AppSettings } from '../types';
import fs from 'fs';
import path from 'path';

export async function showSystemInfo(settings: AppSettings): Promise<void> {
  const screen = blessed.screen({
    smartCSR: true,
    title: 'System Information',
  });

  // Gather system info
  const binaryExists = fs.existsSync(settings.binaryPath);
  const dbExists = fs.existsSync(settings.databasePath);
  
  let binarySize = 'N/A';
  if (binaryExists) {
    const stats = fs.statSync(settings.binaryPath);
    binarySize = `${(stats.size / 1024).toFixed(2)} KB`;
  }

  let dbSize = 'N/A';
  if (dbExists) {
    const stats = fs.statSync(settings.databasePath);
    dbSize = `${(stats.size / 1024).toFixed(2)} KB`;
  }

  // Count CSV files
  const csvCount = fs.existsSync(settings.csvPath) 
    ? fs.readdirSync(settings.csvPath).filter(f => f.endsWith('.csv')).length 
    : 0;

  // Count PNG files
  const pngCount = fs.existsSync(settings.pngPath) 
    ? fs.readdirSync(settings.pngPath).filter(f => f.endsWith('.png')).length 
    : 0;

  // Node version
  const nodeVersion = process.version;

  // Content
  const content = `{bold}{blue-fg}Multi-Algo-Analysis - System Information{/blue-fg}{/bold}

{bold}Project Structure:{/bold}
  Project Root:    {cyan-fg}${settings.projectRoot}{/cyan-fg}
  Binary:          ${binaryExists ? '{green-fg}✓{/green-fg}' : '{red-fg}✗{/red-fg}'} ${settings.binaryPath} ({binarySize})
  Database:        ${dbExists ? '{green-fg}✓{/green-fg}' : '{yellow-fg}Not created{/yellow-fg}'} ${settings.databasePath} (${dbSize})
  CSV Directory:   {cyan-fg}${settings.csvPath}{/cyan-fg} ({csvCount} files)
  PNG Directory:   {cyan-fg}${settings.pngPath}{/cyan-fg} ({pngCount} files)

{bold}Environment:{/bold}
  Node.js:         {cyan-fg}${nodeVersion}{/cyan-fg}
  Platform:        {cyan-fg}${process.platform} ${process.arch}{/cyan-fg}
  CPU Cores:       {cyan-fg}${require('os').cpus().length}{/cyan-fg}
  Total Memory:    {cyan-fg}${(require('os').totalmem() / 1024 / 1024 / 1024).toFixed(2)} GB{/cyan-fg}

{bold}Components:{/bold}
  Resource Monitor:  {green-fg}Active{/green-fg}
  Database Manager:  ${dbExists ? '{green-fg}Active{/green-fg}' : '{yellow-fg}Ready{/yellow-fg}'}
  Plot Generator:    {green-fg}Active{/green-fg}
  Binary Search:     {green-fg}Active{/green-fg}

{bold}Build System:{/bold}
  Makefile:          ${fs.existsSync(path.join(settings.projectRoot, 'Makefile')) ? '{green-fg}✓{/green-fg}' : '{red-fg}✗{/red-fg}'}
  CMakeLists.txt:    ${fs.existsSync(path.join(settings.projectRoot, 'CMakeLists.txt')) ? '{green-fg}✓{/green-fg}' : '{red-fg}✗{/red-fg}'}
`;

  const infoBox = blessed.box({
    top: 1,
    left: '5%',
    width: '90%',
    height: '100%-2',
    content: content,
    tags: true,
    border: {
      type: 'line',
    },
    style: {
      border: {
        fg: 'blue',
      },
    },
  });

  const footer = blessed.box({
    bottom: 0,
    left: 0,
    width: '100%',
    height: 1,
    align: 'center',
    content: '{gray-fg}Press any key to continue{/gray-fg}',
    tags: true,
  });

  screen.append(infoBox);
  screen.append(footer);

  screen.render();

  await new Promise<void>(resolve => {
    screen.key(['escape', 'q', 'enter', 'C-c'], () => {
      screen.destroy();
      resolve();
    });
  });
}
