import blessed from 'blessed';
import { AppSettings, AlgorithmParams } from '../types';

export function showSettingsScreen(settings: AppSettings): Promise<AppSettings> {
  return new Promise((resolve) => {
    const screen = blessed.screen({
      smartCSR: true,
      fullUnicode: true,
      title: 'Settings',
    });

    let params: AlgorithmParams = { ...settings.algorithmParams };
    let focusedField = 0;
    let isEditing = false;
    let inputBuffer = '';

    const fields = [
      { key: 'dataSize' as const, label: 'Data Size', desc: 'Number of elements to generate' },
      { key: 'dataStep' as const, label: 'Data Step', desc: 'Step between values' },
      { key: 'cpuCore' as const, label: 'CPU Core', desc: 'Core ID to bind' },
      { key: 'totalRuns' as const, label: 'Total Runs', desc: 'Number of iterations' },
      { key: 'useCustomTargets' as const, label: 'Use Custom Targets', desc: 'Enter to toggle' },
      { key: 'customTargets' as const, label: 'Target Values', desc: 'Enter to edit' },
    ];

    // Header
    const header = blessed.box({
      top: 0,
      left: 0,
      width: '100%',
      height: 1,
      content: '{bold}Settings - Algorithm Parameters{/bold}',
      tags: true,
      align: 'center',
    });

    // Content area
    const contentBox = blessed.box({
      top: 2,
      left: 0,
      width: '100%',
      height: '100%-5',
      tags: true,
      content: renderContent(),
    });

    // Separator line
    const separator = blessed.box({
      bottom: 3,
      left: 0,
      width: '100%',
      height: 1,
      content: '{gray-fg}────────────────────────────────────────────────────────────────{/gray-fg}',
      tags: true,
    });

    // Instructions
    const instructions = blessed.box({
      bottom: 2,
      left: 0,
      width: '100%',
      height: 1,
      content: '{gray-fg}S: Save | R: Reset | Q: Quit | Up/Down: Navigate | Enter: Edit{/gray-fg}',
      tags: true,
      align: 'center',
    });

    screen.append(header);
    screen.append(contentBox);
    screen.append(separator);
    screen.append(instructions);

    screen.key(['s'], () => { if (!isEditing) { settings.algorithmParams = params; resolve(settings); screen.destroy(); } });
    screen.key(['r'], () => { if (!isEditing) { resetToDefaults(); } });
    screen.key(['q'], () => { if (!isEditing) { resolve(settings); screen.destroy(); } });
    screen.key(['up', 'S-tab'], () => { if (!isEditing) { focusedField = (focusedField - 1 + fields.length) % fields.length; render(); } });
    screen.key(['down', 'tab'], () => { if (!isEditing) { focusedField = (focusedField + 1) % fields.length; render(); } });

    screen.key(['enter'], () => {
      const field = fields[focusedField].key;
      
      if (field === 'useCustomTargets') {
        params.useCustomTargets = !params.useCustomTargets;
        render();
      } else if (field === 'customTargets') {
        showCustomTargetsDialog();
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

    screen.key(['escape', 'C-c'], () => {
      if (isEditing) {
        isEditing = false;
        inputBuffer = '';
        render();
      } else {
        resolve(settings);
        screen.destroy();
      }
    });

    screen.render();

    function resetToDefaults() {
      const { getDefaultAlgorithmParams } = require('../settings');
      params = getDefaultAlgorithmParams();
      focusedField = 0;
      isEditing = false;
      inputBuffer = '';
      render();
    }

    function render() {
      contentBox.setContent(renderContent());
      if (isEditing) {
        instructions.setContent('{yellow-fg}Editing: Type digits, Enter to confirm, Esc to cancel{/yellow-fg}');
      } else {
        instructions.setContent('{gray-fg}S: Save | R: Reset | Q: Quit | Up/Down: Navigate | Enter: Edit{/gray-fg}');
      }
      screen.render();
    }

    function renderContent(): string {
      const lines: string[] = [];
      
      fields.forEach((field, idx) => {
        const isFocused = focusedField === idx;
        const value = params[field.key];

        // Field label
        const marker = isFocused ? '>' : ' ';
        lines.push(`{bold}${marker} ${field.label}{/bold}`);

        // Field value
        if (field.key === 'useCustomTargets') {
          const display = value ? '{green-fg}[✓] Yes{/green-fg}' : '[ ] No';
          lines.push(`    ${isFocused ? `{bg-blue} ${display} {/bg-blue}` : display}`);
        } else if (field.key === 'customTargets') {
          const display = (value as number[]).join(', ');
          lines.push(`    ${isFocused ? `{bg-blue} ${display} {/bg-blue}` : display}`);
        } else {
          const display = String(value);
          if (isEditing && isFocused) {
            lines.push(`    {bg-blue} ${inputBuffer || '...'} {/bg-blue}`);
          } else {
            lines.push(`    ${isFocused ? `{bg-blue} ${display} {/bg-blue}` : display}`);
          }
        }

        // Description
        lines.push(`    {gray-fg}${field.desc}{/gray-fg}`);
        lines.push('');
      });

      return lines.join('\n');
    }

    function showCustomTargetsDialog() {
      isEditing = true;
      inputBuffer = params.customTargets.join(', ');

      const promptBox = blessed.box({
        top: 'center',
        left: 'center',
        width: '60%',
        height: 8,
        tags: true,
        border: { type: 'line' },
        style: { border: { fg: 'cyan' } },
        content: `  {bold}Edit Target Values{/bold}\n\n  {bg-blue} ${inputBuffer} {/bg-blue}\n\n  {gray-fg}Type numbers separated by commas{/gray-fg}\n  {gray-fg}Enter: confirm | Esc: cancel{/gray-fg}`,
      });

      screen.append(promptBox);
      screen.render();

      function cleanup() {
        screen.remove(promptBox);
        isEditing = false;
      }

      const enterHandler = () => {
        cleanup();
        const parts = inputBuffer.split(',').map(p => parseInt(p.trim())).filter(n => !isNaN(n) && n > 0);
        if (parts.length > 0) {
          params.customTargets = parts;
        }
        render();
      };

      const escHandler = () => {
        cleanup();
        render();
      };

      const inputHandler = (ch: string) => {
        if (inputBuffer.length < 100 && /[0-9, ]/.test(ch)) {
          inputBuffer += ch;
          promptBox.setContent(`  {bold}Edit Target Values{/bold}\n\n  {bg-blue} ${inputBuffer} {/bg-blue}\n\n  {gray-fg}Type numbers separated by commas{/gray-fg}\n  {gray-fg}Enter: confirm | Esc: cancel{/gray-fg}`);
          screen.render();
        }
      };

      const backspaceHandler = () => {
        if (inputBuffer.length > 0) {
          inputBuffer = inputBuffer.slice(0, -1);
          promptBox.setContent(`  {bold}Edit Target Values{/bold}\n\n  {bg-blue} ${inputBuffer || '...'} {/bg-blue}\n\n  {gray-fg}Type numbers separated by commas{/gray-fg}\n  {gray-fg}Enter: confirm | Esc: cancel{/gray-fg}`);
          screen.render();
        }
      };

      screen.key(['enter'], enterHandler);
      screen.key(['escape', 'C-c'], escHandler);
      screen.key(['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ','], inputHandler);
      screen.key(['backspace', 'delete'], backspaceHandler);

      // Store handlers to remove later
      (screen as any)._customTargetHandlers = { enterHandler, escHandler, inputHandler, backspaceHandler };
    }
  });
}
