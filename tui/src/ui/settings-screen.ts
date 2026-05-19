import blessed from 'blessed';
import { AppSettings, AlgorithmParams } from '../types';

// Returns updated settings on save, null on cancel.
export function showSettingsScreen(settings: AppSettings): Promise<AppSettings | null> {
  return new Promise((resolve) => {
    const screen = blessed.screen({
      smartCSR: true,
      fullUnicode: true,
      title: 'Settings',
    });

    let params: AlgorithmParams = { ...settings.algorithmParams };
    let focusedField = 0;
    let dialogOpen = false;

    const fields = [
      { key: 'dataSize' as const, label: 'Data Size', desc: 'Number of elements to generate' },
      { key: 'dataStep' as const, label: 'Data Step', desc: 'Step between values' },
      { key: 'cpuCore' as const, label: 'CPU Core', desc: 'Core ID to bind' },
      { key: 'totalRuns' as const, label: 'Total Runs', desc: 'Number of iterations' },
      { key: 'useCustomTargets' as const, label: 'Use Custom Targets', desc: 'Press Enter to toggle' },
      { key: 'customTargets' as const, label: 'Target Values', desc: 'Press Enter to edit' },
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

    // Create list for navigation
    const settingsList = blessed.list({
      top: 2,
      left: '5%',
      width: '90%',
      height: fields.length + 2,
      keys: true,
      vi: true,
      mouse: true,
      tags: true,
      items: buildListItems(),
      style: {
        selected: { bg: 'blue', fg: 'white' },
        item: { fg: 'white' },
      },
    });

    // Description box
    const descBox = blessed.box({
      top: fields.length + 5,
      left: '5%',
      width: '90%',
      height: 2,
      tags: true,
      align: 'center',
      content: `{gray-fg}${fields[0].desc}{/gray-fg}`,
    });

    // Instructions
    const instructions = blessed.box({
      bottom: 0,
      left: 0,
      width: '100%',
      height: 1,
      content: '{gray-fg}S: Save & Exit | R: Reset | Q/Esc: Cancel | Up/Down: Navigate | Enter: Edit/Toggle{/gray-fg}',
      tags: true,
      align: 'center',
    });

    screen.append(header);
    screen.append(settingsList);
    screen.append(descBox);
    screen.append(instructions);

    settingsList.select(0);
    settingsList.focus();

    // blessed emits 'select item' (not 'item change') on navigation
    // Do NOT call updateList() here — setItems→select→'select item' = infinite recursion
    settingsList.on('select item', (_item: any, index: number) => {
      focusedField = index;
      descBox.setContent(`{gray-fg}${fields[index].desc}{/gray-fg}`);
      screen.render();
    });

    // Enter to edit/toggle — use 'select' not 'action'; 'action' also fires on Escape
    // (cancelSelected) which destroys screen before dialog creation → "No active screen"
    settingsList.on('select', (_item: any, index: number) => {
      if (dialogOpen) return;
      focusedField = index;
      const field = fields[focusedField].key;

      if (field === 'useCustomTargets') {
        params.useCustomTargets = !params.useCustomTargets;
        updateList();
        screen.render();
      } else if (field === 'customTargets') {
        showCustomTargetsDialog();
      } else {
        showNumberEditDialog(field);
      }
    });

    // S = save, Q/Escape/C-c = cancel (no save)
    screen.key(['s'], () => {
      if (dialogOpen) return;
      settings.algorithmParams = params;
      resolve(settings);
      screen.destroy();
    });
    screen.key(['r'], () => {
      if (dialogOpen) return;
      resetToDefaults();
    });
    screen.key(['q', 'escape', 'C-c'], () => {
      if (dialogOpen) return;
      resolve(null);
      screen.destroy();
    });

    screen.render();

    function resetToDefaults() {
      const { getDefaultAlgorithmParams } = require('../settings');
      params = getDefaultAlgorithmParams();
      focusedField = 0;
      updateList();
      descBox.setContent(`{gray-fg}${fields[0].desc}{/gray-fg}`);
      screen.render();
    }

    function buildListItems(): string[] {
      return fields.map((f) => `  ${f.label}: ${
        f.key === 'useCustomTargets' ? (params[f.key] ? '{green-fg}Yes{/green-fg}' : '{gray-fg}No{/gray-fg}') :
        f.key === 'customTargets' ? (params[f.key] as number[]).join(', ') :
        String(params[f.key])
      }`);
    }

    function updateList() {
      settingsList.setItems(buildListItems());
      settingsList.select(focusedField);
      screen.render();
    }

    function showNumberEditDialog(fieldKey: keyof AlgorithmParams) {
      const field = fields.find(f => f.key === fieldKey)!;
      const currentValue = String(params[fieldKey]);

      const form = blessed.form({
        top: 'center',
        left: 'center',
        width: '50%',
        height: 8,
        keys: true,
        mouse: true,
        border: { type: 'line' },
        style: { border: { fg: 'cyan' } },
        tags: true,
      });

      const title = blessed.box({
        top: 0,
        left: 0,
        width: '100%',
        height: 1,
        content: `  {bold}Edit ${field.label}{/bold}`,
        tags: true,
      });

      const textBox = blessed.textbox({
        top: 2,
        left: '10%',
        width: '80%',
        height: 1,
        value: currentValue,
        inputOnFocus: true,
        border: { type: 'line' },
        style: {
          fg: 'white',
          bg: 'black',
          focus: { fg: 'white', bg: 'blue', border: { fg: 'cyan' } },
        },
      });

      const helpText = blessed.box({
        top: 4,
        left: 0,
        width: '100%',
        height: 1,
        content: '  {gray-fg}Enter: confirm | Esc: cancel{/gray-fg}',
        tags: true,
        align: 'center',
      });

      form.append(title);
      form.append(textBox);
      form.append(helpText);
      screen.append(form);
      textBox.focus();
      dialogOpen = true;
      screen.render();

      function closeDialog() {
        dialogOpen = false;
        screen.remove(form);
        settingsList.focus();
        screen.render();
      }

      // Escape on the textbox itself so it fires even when inputOnFocus captures keys
      textBox.key(['escape'], () => closeDialog());

      textBox.on('submit', (val: string) => {
        const num = parseInt(val);
        if (!isNaN(num) && num >= 0) {
          (params as any)[fieldKey] = num;
          updateList();
        }
        closeDialog();
      });
    }

    function showCustomTargetsDialog() {
      const currentValue = params.customTargets.join(', ');

      const form = blessed.form({
        top: 'center',
        left: 'center',
        width: '60%',
        height: 8,
        keys: true,
        mouse: true,
        border: { type: 'line' },
        style: { border: { fg: 'cyan' } },
        tags: true,
      });

      const title = blessed.box({
        top: 0,
        left: 0,
        width: '100%',
        height: 1,
        content: '  {bold}Edit Target Values{/bold}',
        tags: true,
      });

      const textBox = blessed.textbox({
        top: 2,
        left: '5%',
        width: '90%',
        height: 1,
        value: currentValue,
        inputOnFocus: true,
        border: { type: 'line' },
        style: {
          fg: 'white',
          bg: 'black',
          focus: { fg: 'white', bg: 'blue', border: { fg: 'cyan' } },
        },
      });

      const helpText = blessed.box({
        top: 4,
        left: 0,
        width: '100%',
        height: 1,
        content: '  {gray-fg}Comma-separated values (e.g., 1000, 50000, 100000) | Enter: confirm | Esc: cancel{/gray-fg}',
        tags: true,
        align: 'center',
      });

      form.append(title);
      form.append(textBox);
      form.append(helpText);
      screen.append(form);
      textBox.focus();
      dialogOpen = true;
      screen.render();

      function closeDialog() {
        dialogOpen = false;
        screen.remove(form);
        settingsList.focus();
        screen.render();
      }

      // Escape on the textbox itself so it fires even when inputOnFocus captures keys
      textBox.key(['escape'], () => closeDialog());

      textBox.on('submit', (val: string) => {
        const parts = val.split(',').map(p => parseInt(p.trim())).filter(n => !isNaN(n) && n >= 0);
        if (parts.length > 0) {
          params.customTargets = parts;
          updateList();
        }
        closeDialog();
      });
    }
  });
}
