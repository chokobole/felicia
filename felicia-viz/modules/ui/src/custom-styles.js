/* eslint import/prefer-default-export: "off" */

export const FORM_STYLE = {
  wrapper: {
    padding: 12,
  },
};

export const METRIC_CARD_STYLE = {
  title: {
    fontSize: 12,
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
