import blessed from 'blessed';
import { AppSettings } from '../types';

interface MenuOption {
  label: string;
  description: string;
}

export function showMainMenu(settings: AppSettings): Promise<number> {
  return new Promise((resolve) => {
    const screen = blessed.screen({
      smartCSR: true,
      title: 'Multi-Algo-Analysis TUI',
    });

    const options: MenuOption[] = [
      { label: 'Run Binary Search', description: 'Execute O(log n) search on sorted array' },
      { label: 'Run Linear Search', description: 'Execute O(n) sequential search' },
      { label: 'Run Merge Sort', description: 'Execute O(n log n) sorting algorithm' },
      { label: 'Run Insertion Sort', description: 'Execute O(n^2) simple sorting algorithm' },
      { label: 'Run Selection Sort', description: 'Execute O(n^2) in-place sorting algorithm' },
      { label: 'Run Bubble Sort', description: 'Execute O(n^2) simple comparison sorting' },
      { label: 'View Historical Runs', description: 'Browse past execution results' },
      { label: 'View Latest Results', description: 'Show most recent execution data' },
      { label: 'System Info', description: 'Display project and environment info' },
      { label: 'Settings', description: 'Configure algorithm parameters' },
      { label: 'Exit', description: 'Close the application' },
    ];

    // Header
    const header = blessed.box({
      top: 0,
      left: 0,
      width: '100%',
      height: 3,
      align: 'center',
      content: '{bold}Multi-Algo-Analysis TUI{/bold}\n{blue-fg}Resource Monitoring Dashboard{/blue-fg}',
      tags: true,
    });

    // Footer
    const footer = blessed.box({
      bottom: 0,
      left: 0,
      width: '100%',
      height: 2,
      align: 'center',
      content: '{gray-fg}↑/↓ Navigate | 1-0 Select | q Quit{/gray-fg}',
      tags: true,
    });

    // Menu list
    const menu = blessed.list({
      top: 4,
      left: '10%',
      width: '80%',
      height: options.length + 1,
      keys: true,
      vi: true,
      mouse: true,
      tags: true,
      style: {
        selected: {
          bg: 'blue',
          fg: 'white',
        },
        item: {
          bg: 'black',
          fg: 'white',
        },
      },
      items: options.map((opt, i) => ` ${i + 1}. ${opt.label}`),
    });

    // Description box
    const descBox = blessed.box({
      top: options.length + 6,
      left: '10%',
      width: '80%',
      height: 2,
      align: 'center',
      tags: true,
      content: `{yellow-fg}${options[0].description}{/yellow-fg}`,
    });

    menu.on('select', (item, index) => {
      resolve(index);
      screen.destroy();
    });

    menu.on('item change', (item, index) => {
      descBox.setContent(`{yellow-fg}${options[index].description}{/yellow-fg}`);
      screen.render();
    });

    screen.key(['escape', 'q', 'C-c'], () => {
      resolve(options.length - 1); // Exit
      screen.destroy();
    });

    screen.key(['1', '2', '3', '4', '5', '6', '7', '8', '9', '0'], (ch) => {
      let idx: number;
      if (ch === '0') {
        // Key 0 maps to last option (Exit)
        idx = options.length - 1;
      } else {
        // Keys 1-9 map to options 0-8
        idx = parseInt(ch) - 1;
      }
      if (idx >= 0 && idx < options.length) {
        resolve(idx);
        screen.destroy();
      }
    });

    screen.append(header);
    screen.append(footer);
    screen.append(menu);
    screen.append(descBox);

    menu.focus();
    screen.render();
  });
}
