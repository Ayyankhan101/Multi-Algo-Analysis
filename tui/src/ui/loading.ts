import blessed from 'blessed';

export function showLoadingIndeterminate(message: string = 'Running algorithm...'): {
  screen: blessed.Widgets.Screen;
  close: () => void;
} {
  const screen = blessed.screen({
    smartCSR: true,
    title: 'Please Wait',
  });

  const spinner = ['⠋', '⠙', '⠹', '⠸', '⠼', '⠴', '⠦', '⠧', '⠇', '⠏'];
  let index = 0;

  const loadingBox = blessed.box({
    top: 'center',
    left: 'center',
    width: 60,
    height: 5,
    align: 'center',
    border: {
      type: 'line',
    },
    style: {
      border: {
        fg: 'cyan',
      },
    },
    content: `{bold}{cyan-fg}${message}{/cyan-fg}{/bold}\n\n{gray-fg}${spinner[0]} Processing...{/gray-fg}`,
    tags: true,
  });

  screen.append(loadingBox);
  screen.render();

  const interval = setInterval(() => {
    index = (index + 1) % spinner.length;
    loadingBox.setContent(`{bold}{cyan-fg}${message}{/cyan-fg}{/bold}\n\n{gray-fg}${spinner[index]} Processing...{/gray-fg}`);
    screen.render();
  }, 100);

  return {
    screen,
    close: () => {
      clearInterval(interval);
      screen.destroy();
    },
  };
}
