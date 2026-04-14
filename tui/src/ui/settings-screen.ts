import blessed from 'blessed';
import { AppSettings, AlgorithmParams } from '../types';

export function showSettingsScreen(settings: AppSettings): Promise<AppSettings> {
  return new Promise((resolve) => {
    const screen = blessed.screen({
      smartCSR: true,
      fullUnicode: true,
      title: 'Algorithm Parameters Settings',
    });

    // Clone current params
    let params: AlgorithmParams = { ...settings.algorithmParams };
    let focusedField = 0;
    let isEditing = false;
    let inputBuffer = '';

    const fields: Array<{ key: keyof AlgorithmParams; label: string; description: string }> = [
      { key: 'dataSize', label: 'Data Size', description: 'Number of elements to generate (default: 1000000)' },
      { key: 'dataStep', label: 'Data Step', description: 'Step between values (default: 3)' },
      { key: 'cpuCore', label: 'CPU Core', description: 'Core ID to bind (default: 0)' },
      { key: 'totalRuns', label: 'Total Runs', description: 'Number of iterations (default: 5)' },
      { key: 'useCustomTargets', label: 'Use Custom Targets', description: 'Toggle to use custom targets (default: No)' },
      { key: 'customTargets', label: 'Target Values', description: 'Comma-separated values for search algorithms' },
    ];

    // Create UI elements
    const header = blessed.box({
      top: 0,
      left: 0,
      width: '100%',
      height: 2,
      align: 'center',
      content: '{bold}{blue-fg}⚙  Algorithm Parameters Configuration{/blue-fg}{/bold}',
      tags: true,
    });

    const contentBox = blessed.box({
      top: 3,
      left: '5%',
      width: '90%',
      height: '80%-3',
      tags: true,
      content: renderContent(),
    });

    const saveBtn = blessed.button({
      bottom: 2,
      left: '20%',
      width: 16,
      content: ' [S] Save ',
      tags: true,
      style: { focus: { bg: 'green', fg: 'white' } },
    });

    const resetBtn = blessed.button({
      bottom: 2,
      left: '42%',
      width: 16,
      content: ' [R] Reset ',
      tags: true,
      style: { focus: { bg: 'yellow', fg: 'black' } },
    });

    const cancelBtn = blessed.button({
      bottom: 2,
      left: '64%',
      width: 16,
      content: ' [Q] Cancel ',
      tags: true,
      style: { focus: { bg: 'red', fg: 'white' } },
    });

    screen.append(header);
    screen.append(contentBox);
    screen.append(saveBtn);
    screen.append(resetBtn);
    screen.append(cancelBtn);

    saveBtn.on('press', saveAndClose);
    resetBtn.on('press', resetToDefaults);
    cancelBtn.on('press', cancelClose);

    // Key handlers
    screen.key(['s'], () => { if (!isEditing) saveAndClose(); });
    screen.key(['r'], () => { if (!isEditing) resetToDefaults(); });
    screen.key(['q'], () => { if (!isEditing) cancelClose(); });
    screen.key(['escape', 'C-c'], () => {
      if (isEditing) {
        isEditing = false;
        inputBuffer = '';
        render();
      } else {
        cancelClose();
      }
    });

    screen.key(['up', 'S-tab'], () => {
      if (isEditing) return;
      focusedField = (focusedField - 1 + fields.length) % fields.length;
      render();
    });

    screen.key(['down', 'tab'], () => {
      if (isEditing) return;
      focusedField = (focusedField + 1) % fields.length;
      render();
    });

    screen.key(['enter'], () => {
      const field = fields[focusedField].key;

      if (field === 'useCustomTargets') {
        params.useCustomTargets = !params.useCustomTargets;
        render();
      } else if (field === 'customTargets') {
        showTextInput('Enter targets (comma-separated):', params.customTargets.join(', ')).then(value => {
          if (value && value.trim()) {
            const parts = value.split(',').map(p => parseInt(p.trim())).filter(n => !isNaN(n) && n > 0);
            if (parts.length > 0) {
              params.customTargets = parts;
            }
          }
          isEditing = false;
          inputBuffer = '';
          render();
        });
      } else {
        isEditing = true;
        inputBuffer = String(params[field]);
        render();
      }
    });

    screen.key(['0', '1', '2', '3', '4', '5', '6', '7', '8', '9'], (ch) => {
      if (!isEditing) return;
      if (inputBuffer.length < 12) {
        inputBuffer += ch;
        render();
      }
    });

    screen.key(['backspace', 'delete'], () => {
      if (!isEditing || inputBuffer.length === 0) return;
      inputBuffer = inputBuffer.slice(0, -1);
      render();
    });

    screen.render();

    function saveAndClose() {
      settings.algorithmParams = params;
      resolve(settings);
      screen.destroy();
    }

    function resetToDefaults() {
      const { getDefaultAlgorithmParams } = require('../settings');
      params = getDefaultAlgorithmParams();
      focusedField = 0;
      isEditing = false;
      inputBuffer = '';
      render();
    }

    function cancelClose() {
      resolve(settings);
      screen.destroy();
    }

    function render() {
      contentBox.setContent(renderContent());
      screen.render();
    }

    function renderContent(): string {
      const lines: string[] = [];
      const indent = '      ';

      fields.forEach((field, idx) => {
        const isFocused = focusedField === idx && !isEditing;
        const isCurrentEdit = focusedField === idx && isEditing;
        const value = params[field.key];

        lines.push(`{bold}{cyan-fg}${field.label}{/cyan-fg}{/bold}`);

        if (field.key === 'useCustomTargets') {
          const display = value ? '{green-fg}[✓] Yes{/green-fg}' : '{gray-fg}[ ] No{/gray-fg}';
          lines.push(`${indent}${isFocused ? highlight(display) : display}`);
        } else if (field.key === 'customTargets') {
          if (isCurrentEdit) {
            lines.push(`${indent}{bg-blue}{white-fg} ${inputBuffer || 'typing...'} {/white-fg}{/bg-blue}`);
          } else {
            lines.push(`${indent}${isFocused ? highlight(String(value).replace(/,/g, ', ')) : String(value).replace(/,/g, ', ')}`);
          }
        } else {
          if (isCurrentEdit) {
            lines.push(`${indent}{bg-blue}{white-fg} ${inputBuffer || 'typing...'} {/white-fg}{/bg-blue}`);
          } else {
            lines.push(`${indent}${isFocused ? highlight(String(value)) : String(value)}`);
          }
        }

        lines.push(`${indent}{gray-fg}${field.description}{/gray-fg}`);
        lines.push('');
      });

      if (isEditing) {
        lines.push('{yellow-fg}↲ Enter to confirm | Esc to cancel | 0-9 to input | Backspace to delete{/yellow-fg}');
      } else {
        lines.push('{gray-fg}↑/↓ Navigate | ↲ Enter to edit | S Save | R Reset | Q Quit{/gray-fg}');
      }

      return lines.join('\n');
    }

    function highlight(text: string): string {
      return `{bg-blue}{white-fg} ${text} {/white-fg}{/bg-blue}`;
    }

    function showTextInput(label: string, initialValue: string): Promise<string> {
      return new Promise((resolveInput) => {
        isEditing = true;
        inputBuffer = initialValue;

        const promptBox = blessed.box({
          top: 'center',
          left: 'center',
          width: '70%',
          height: 7,
          tags: true,
          border: { type: 'line' },
          style: { border: { fg: 'cyan' } },
          content: `{bold}${label}{/bold}\n\n{bg-blue}{white-fg} ${inputBuffer} {/white-fg}{/bg-blue}\n\n{gray-fg}Type value, Enter to confirm, Esc to cancel{/gray-fg}`,
        });

        screen.append(promptBox);
        screen.render();

        function finishInput(value: string) {
          screen.remove(promptBox);
          isEditing = false;
          resolveInput(value);
        }

        // Temporary key handlers
        const enterHandler = () => {
          cleanup();
          finishInput(inputBuffer);
        };

        const escHandler = () => {
          cleanup();
          finishInput('');
        };

        const numHandler = (ch: string) => {
          if (inputBuffer.length < 50 && /[0-9, ]/.test(ch)) {
            inputBuffer += ch;
            updatePrompt();
          }
        };

        const backspaceHandler = () => {
          if (inputBuffer.length > 0) {
            inputBuffer = inputBuffer.slice(0, -1);
            updatePrompt();
          }
        };

        function updatePrompt() {
          promptBox.setContent(`{bold}${label}{/bold}\n\n{bg-blue}{white-fg} ${inputBuffer || '...'} {/white-fg}{/bg-blue}\n\n{gray-fg}Type value, Enter to confirm, Esc to cancel{/gray-fg}`);
          screen.render();
        }

        function cleanup() {
          screen.removeListener('enter', enterHandler);
          screen.removeListener('escape', escHandler);
          screen.removeListener('C-c', escHandler);
          screen.removeListener('0', numHandler);
          screen.removeListener('1', numHandler);
          screen.removeListener('2', numHandler);
          screen.removeListener('3', numHandler);
          screen.removeListener('4', numHandler);
          screen.removeListener('5', numHandler);
          screen.removeListener('6', numHandler);
          screen.removeListener('7', numHandler);
          screen.removeListener('8', numHandler);
          screen.removeListener('9', numHandler);
          screen.removeListener(',', numHandler);
          screen.removeListener('backspace', backspaceHandler);
          screen.removeListener('delete', backspaceHandler);
        }

        screen.key(['enter'], enterHandler);
        screen.key(['escape', 'C-c'], escHandler);
        screen.key(['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ','], numHandler);
        screen.key(['backspace', 'delete'], backspaceHandler);
      });
    }
  });
}
