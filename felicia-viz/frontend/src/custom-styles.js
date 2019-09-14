/* eslint import/prefer-default-export: "off" */

export const UI_THEME = {
  extends: 'dark',
  background: 'rgba(51,51,51,0.9)',
  backgroundAlt: '#222222',

  controlColorPrimary: '#858586',
  controlColorSecondary: '#636364',
  controlColorDisabled: '#404042',
  controlColorHovered: '#F8F8F9',
  controlColorActive: '#5B91F4',

  textColorPrimary: '#F8F8F9',
  textColorSecondary: '#D0D0D1',
  textColorDisabled: '#717172',
  textColorInvert: '#1B1B1C',

  fontFamily: '-apple-system, BlinkMacSystemFont, "Segoe UI", Arial, sans-serif',
  fontSize: 14,
  fontWeight: 200,

  shadow: '0 2px 4px 0 rgba(0, 0, 0, 0.15)',
};

export const TOOLTIP_STYLE = {
  arrowSize: 0,
  borderWidth: 0,
  background: '#CCCCCC',
  body: {
    color: '#141414',
    whiteSpace: 'nowrap',
    fontSize: 12,
  },
};

export const TOOLBAR_BUTTON_STYLE = {
  size: 60,
  wrapper: props => ({
    fontSize: 32,
    background: props.isHovered ? 'rgba(129,133,138,0.3)' : props.theme.background,
  }),
};

export const TOOLBAR_MENU_STYLE = {
  arrowSize: 0,
  borderWidth: 0,
  body: {
    left: 56,
    boxShadow: 'none',
  },
};

export const METRIC_CARD_STYLE = {
  title: {
    fontSize: 12,
  },
};

export const FORM_STYLE = {
  wrapper: {
    padding: 12,
  },
};

export function babylonCanvasStyle(props) {
  const { width, height } = props;
  const margin = '30px';

  return {
    width: `calc(${width} - ${margin} * 2)`,
    height: `calc(${height} - ${margin} * 2)`,
    margin,
    outline: 'none',
  };
}
