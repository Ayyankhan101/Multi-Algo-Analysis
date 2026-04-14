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
    const listItems = fields.map((f, i) => `${i === focusedField ? '▸' : ' '} ${f.label}: ${
      f.key === 'useCustomTargets' ? (params[f.key] ? 'Yes' : 'No') :
      f.key === 'customTargets' ? (params[f.key] as number[]).join(', ') :
      String(params[f.key])
    }`);

    const settingsList = blessed.list({
      top: 2,
      left: '5%',
      width: '90%',
      height: fields.length + 2,
      keys: true,
      vi: true,
      mouse: true,
      tags: true,
      items: listItems,
      style: {
        selected: {
          bg: 'blue',
          fg: 'white',
        },
        item: {
          fg: 'white',
        },
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
      content: '{gray-fg}S: Save | R: Reset | Q: Quit | ↑/↓: Navigate | Enter: Edit/Toggle | Esc: Back{/gray-fg}',
      tags: true,
      align: 'center',
    });

    screen.append(header);
    screen.append(settingsList);
    screen.append(descBox);
    screen.append(instructions);

    settingsList.select(0);
    settingsList.focus();

    // Key handlers
    screen.key(['s'], () => { settings.algorithmParams = params; resolve(settings); screen.destroy(); });
    screen.key(['r'], () => { resetToDefaults(); });
    screen.key(['q', 'escape'], () => { resolve(settings); screen.destroy(); });

    settingsList.on('select', (item, index) => {
      focusedField = index;
      descBox.setContent(`{gray-fg}${fields[index].desc}{/gray-fg}`);
      screen.render();
    });

    settingsList.on('item change', (item, index) => {
      focusedField = index;
      descBox.setContent(`{gray-fg}${fields[index].desc}{/gray-fg}`);
      screen.render();
    });

    screen.key(['enter'], () => {
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

    screen.render();

    function resetToDefaults() {
      const { getDefaultAlgorithmParams } = require('../settings');
      params = getDefaultAlgorithmParams();
      focusedField = 0;
      updateList();
      screen.render();
    }

    function updateList() {
      const items = fields.map((f, i) => `${i === focusedField ? '▸' : ' '} ${f.label}: ${
        f.key === 'useCustomTargets' ? (params[f.key] ? '{green-fg}Yes{/green-fg}' : 'No') :
        f.key === 'customTargets' ? (params[f.key] as number[]).join(', ') :
        String(params[f.key])
      }`);
      settingsList.setItems(items);
      settingsList.select(focusedField);
    }

    function showNumberEditDialog(fieldKey: keyof AlgorithmParams) {
      const field = fields.find(f => f.key === fieldKey)!;
      let currentValue = String(params[fieldKey]);

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
          focus: {
            border: { fg: 'blue' },
          },
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
      screen.render();

      textBox.on('submit', (val: string) => {
        const num = parseInt(val);
        if (!isNaN(num) && num >= 0) {
          (params as any)[fieldKey] = num;
          updateList();
        }
        screen.remove(form);
        settingsList.focus();
        screen.render();
      });

      const escHandler = () => {
        screen.remove(form);
        screen.removeListener('escape', escHandler);
        settingsList.focus();
        screen.render();
      };
      screen.key(['escape'], escHandler);
    }

    function showCustomTargetsDialog() {
      let currentValue = params.customTargets.join(', ');

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
          focus: {
            border: { fg: 'blue' },
          },
        },
      });

      const helpText = blessed.box({
        top: 4,
        left: 0,
        width: '100%',
        height: 1,
        content: '  {gray-fg}Enter numbers separated by commas | Enter: confirm | Esc: cancel{/gray-fg}',
        tags: true,
        align: 'center',
      });

      form.append(title);
      form.append(textBox);
      form.append(helpText);
      screen.append(form);
      textBox.focus();
      screen.render();

      textBox.on('submit', (val: string) => {
        const parts = val.split(',').map(p => parseInt(p.trim())).filter(n => !isNaN(n) && n > 0);
        if (parts.length > 0) {
          params.customTargets = parts;
          updateList();
        }
        screen.remove(form);
        settingsList.focus();
        screen.render();
      });

      const escHandler = () => {
        screen.remove(form);
        screen.removeListener('escape', escHandler);
        settingsList.focus();
        screen.render();
      };
      screen.key(['escape'], escHandler);
    }
  });
}
