import blessed from 'blessed';
import { AppSettings, AlgorithmParams } from '../types';

export function showSettingsScreen(settings: AppSettings): Promise<AppSettings> {
  return new Promise((resolve) => {
    const screen = blessed.screen({
      smartCSR: true,
      title: 'Algorithm Parameters Settings',
    });

    // Clone current params to avoid mutation until saved
    let params: AlgorithmParams = { ...settings.algorithmParams };

    // Navigation state
    let focusedField = 0;
    const fields = [
      'dataSize',
      'dataStep',
      'cpuCore',
      'totalRuns',
      'useCustomTargets',
      'customTargets',
    ];

    // Header
    const header = blessed.box({
      top: 0,
      left: 0,
      width: '100%',
      height: 2,
      align: 'center',
      content: '{bold}{blue-fg}⚙ Algorithm Parameters Configuration{/blue-fg}{/bold}',
      tags: true,
    });

    // Settings list
    const settingsBox = blessed.box({
      top: 3,
      left: '5%',
      width: '90%',
      height: 20,
      tags: true,
      content: formatSettings(params),
    });

    // Buttons
    const saveBtn = blessed.button({
      bottom: 3,
      left: '30%',
      width: 15,
      content: ' Save ',
      tags: true,
      style: {
        focus: {
          bg: 'green',
          fg: 'white',
        },
      },
    });

    const resetBtn = blessed.button({
      bottom: 3,
      left: '50%',
      width: 15,
      content: ' Reset ',
      tags: true,
      style: {
        focus: {
          bg: 'yellow',
          fg: 'black',
        },
      },
    });

    const cancelBtn = blessed.button({
      bottom: 3,
      left: '70%',
      width: 15,
      content: ' Cancel ',
      tags: true,
      style: {
        focus: {
          bg: 'red',
          fg: 'white',
        },
      },
    });

    screen.append(header);
    screen.append(settingsBox);
    screen.append(saveBtn);
    screen.append(resetBtn);
    screen.append(cancelBtn);

    saveBtn.on('press', () => {
      settings.algorithmParams = params;
      resolve(settings);
      screen.destroy();
    });

    resetBtn.on('press', () => {
      const { getDefaultAlgorithmParams } = require('../settings');
      params = getDefaultAlgorithmParams();
      settingsBox.setContent(formatSettings(params));
      screen.render();
    });

    cancelBtn.on('press', () => {
      resolve(settings); // Return unchanged
      screen.destroy();
    });

    screen.key(['up', 'down', 'tab', 'S-tab'], (ch) => {
      if (ch === 'up' || ch === 'S-tab') {
        focusedField = (focusedField - 1 + fields.length) % fields.length;
        updateFocus();
      } else if (ch === 'down' || ch === 'tab') {
        focusedField = (focusedField + 1) % fields.length;
        updateFocus();
      }
      screen.render();
    });

    // Handle input for numeric fields
    screen.key(['0', '1', '2', '3', '4', '5', '6', '7', '8', '9'], (ch) => {
      const field = fields[focusedField];
      if (field === 'customTargets') return; // Handled separately
      
      const current = (params as any)[field];
      (params as any)[field] = parseInt(current.toString() + ch);
      settingsBox.setContent(formatSettings(params));
      updateFocus();
      screen.render();
    });

    screen.key(['backspace', 'delete'], () => {
      const field = fields[focusedField];
      if (field === 'customTargets') return;
      
      const current = (params as any)[field].toString();
      if (current.length > 1) {
        (params as any)[field] = parseInt(current.slice(0, -1));
      }
      settingsBox.setContent(formatSettings(params));
      updateFocus();
      screen.render();
    });

    screen.key(['enter'], () => {
      const field = fields[focusedField];
      if (field === 'useCustomTargets') {
        params.useCustomTargets = !params.useCustomTargets;
        settingsBox.setContent(formatSettings(params));
        updateFocus();
        screen.render();
      }
    });

    screen.key(['escape', 'q', 'C-c'], () => {
      resolve(settings);
      screen.destroy();
    });

    // Focus management for buttons
    let buttonFocus = -1;
    const buttons = [saveBtn, resetBtn, cancelBtn];

    screen.key(['right'], (ch) => {
      if (focusedField >= fields.length - 1) {
        buttonFocus = (buttonFocus + 1) % buttons.length;
        buttons.forEach((b, i) => {
          if (i === buttonFocus) b.focus();
        });
        screen.render();
      }
    });

    screen.key(['left'], (ch) => {
      if (buttonFocus >= 0) {
        buttonFocus = (buttonFocus - 1 + buttons.length) % buttons.length;
        buttons.forEach((b, i) => {
          if (i === buttonFocus) b.focus();
        });
        screen.render();
      }
    });

    updateFocus();
    screen.render();

    function updateFocus() {
      if (focusedField < fields.length) {
        buttonFocus = -1;
      }
    }

    function formatSettings(p: AlgorithmParams): string {
      const lines: string[] = [];
      const indent = '    ';

      lines.push('{bold}{cyan-fg}Data Generation{/cyan-fg}{/bold}');
      lines.push(`${indent}{yellow-fg}Data Size:{/yellow-fg} ${highlight(p.dataSize.toString(), focusedField === 0)}`);
      lines.push(`${indent}{gray-fg}Number of elements to generate (default: 1000000){/gray-fg}`);
      lines.push('');
      lines.push(`${indent}{yellow-fg}Data Step:{/yellow-fg} ${highlight(p.dataStep.toString(), focusedField === 1)}`);
      lines.push(`${indent}{gray-fg}Step between values (default: 3){/gray-fg}`);
      lines.push('');
      lines.push('{bold}{cyan-fg}Execution{/cyan-fg}{/bold}');
      lines.push(`${indent}{yellow-fg}CPU Core:{/yellow-fg} ${highlight(p.cpuCore.toString(), focusedField === 2)}`);
      lines.push(`${indent}{gray-fg}Core ID to bind (default: 0){/gray-fg}`);
      lines.push('');
      lines.push(`${indent}{yellow-fg}Total Runs:{/yellow-fg} ${highlight(p.totalRuns.toString(), focusedField === 3)}`);
      lines.push(`${indent}{gray-fg}Number of iterations (default: 5){/gray-fg}`);
      lines.push('');
      lines.push('{bold}{cyan-fg}Search Targets{/cyan-fg}{/bold}');
      lines.push(`${indent}{yellow-fg}Custom Targets:{/yellow-fg} ${highlight(p.useCustomTargets ? '[X] Yes' : '[ ] No', focusedField === 4)}`);
      lines.push(`${indent}{gray-fg}Press Enter to toggle (default: No){/gray-fg}`);
      lines.push('');
      lines.push(`${indent}{yellow-fg}Target Values:{/yellow-fg} ${highlight(p.customTargets.join(', '), focusedField === 5)}`);
      lines.push(`${indent}{gray-fg}Comma-separated values for search algorithms{/gray-fg}`);
      lines.push('');
      lines.push('{gray-fg}Use ↑/↓ to navigate | Enter to toggle | 0-9 to edit | Backspace to delete{/gray-fg}');

      return lines.join('\n');
    }

    function highlight(text: string, isActive: boolean): string {
      if (isActive) {
        return `{bg-blue}{white-fg} ${text} {/white-fg}{/bg-blue}`;
      }
      return `{white-fg}${text}{/white-fg}`;
    }
  });
}
