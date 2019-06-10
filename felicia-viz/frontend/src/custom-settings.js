/* eslint import/prefer-default-export: "off" */

export const FLOAT_PANEL_SETTINGS = {
  movable: true,
  minimizable: false,
  resizable: true,
};

export function panelInitialState(window) {
  return {
    panelState: {
      x: window.innerWidth - 500,
      y: 20,
      width: 480,
      height: 320,
    },
  };
}
