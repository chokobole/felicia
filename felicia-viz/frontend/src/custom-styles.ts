/* eslint import/prefer-default-export: "off" */
import { Theme as AutoSuggestionTheme } from 'react-autosuggest';
type ThemeKey =
  | 'extends'
  | 'background'
  | 'backgroundAlt'
  | 'backgroundHighlight'
  | 'controlColorPrimary'
  | 'controlColorSecondary'
  | 'controlColorDisabled'
  | 'controlColorHovered'
  | 'controlColorActive'
  | 'textColorPrimary'
  | 'textColorSecondary'
  | 'textColorDisabled'
  | 'textColorInvert'
  | 'textColorHighlight'
  | 'borderThin'
  | 'borderStrong'
  | 'shadowSmall'
  | 'spacingTiny'
  | 'spacingSmall'
  | 'spacingNormal'
  | 'spacingLarge'
  | 'spacingHuge';
export type Theme =
  | Record<string, string | React.CSSProperties>
  | Partial<Record<ThemeKey, string | number | React.CSSProperties>>;

export const UI_THEME: Theme = {
  extends: 'dark',
  background: 'rgba(51, 51, 51, 0.9)',
  backgroundAlt: '#222222',
  backgroundHighlight: 'rgba(30, 84, 201, 0.8)',

  controlColorPrimary: '#858586',
  controlColorSecondary: '#636364',
  controlColorDisabled: '#404042',
  controlColorHovered: '#F8F8F9',
  controlColorActive: '#5B91F4',

  textColorPrimary: '#F8F8F9',
  textColorSecondary: '#D0D0D1',
  textColorDisabled: '#717172',
  textColorInvert: '#1B1B1C',
  textColorHighlight: '#110ece',

  fontFamily: '-apple-system, BlinkMacSystemFont, "Segoe UI", Arial, sans-serif',
  fontSize: 14,
  fontWeight: 200,

  borderThin: 'solid 1px',
  borderStrong: 'solid 2px',

  shadowSmall: '0 2px 4px 0 rgba(0, 0, 0, 0.15)',

  spacingTiny: 4,
  spacingSmall: 8,
  spacingNormal: 12,
  spacingLarge: 24,
  spacingHuge: 48,
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
  wrapper: (props: any): React.CSSProperties => ({
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

export const COMMAND_PANEL_THEME: AutoSuggestionTheme = {
  input: {
    width: '100%',
    height: '30px',
    padding: '10px 20px',
    fontWeight: 300,
    fontSize: '16px',
    border: '1px solid #aaa',
    borderRadius: '4px',
  },
  suggestion: {
    cursor: 'pointer',
    padding: '10px 20px',
  },
  suggestionHighlighted: {},
  suggestionsContainerOpen: {
    maxHeight: '300px',
    overflowY: 'auto',
    boxShadow: '0 3px 6px rgba(0,0,0,0.16), 0 3px 6px rgba(0,0,0,0.23)',
  },
  suggestionsList: {
    margin: 0,
    padding: 0,
    listStyleType: 'none',
  },
};
