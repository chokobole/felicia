/* eslint import/prefer-default-export: "off" */
import CSS from 'csstype';

export const FORM_STYLE = {
  wrapper: {
    padding: '4px',
  },
};

export const METRIC_CARD_STYLE = {
  title: {
    fontSize: '12px',
  },
};

export interface SizeProps {
  width: string;
  height: string;
}

export function babylonCanvasStyle({
  width,
  height,
}: SizeProps): {
  width: CSS.WidthProperty<string>;
  height: CSS.WidthProperty<string>;
  margin: CSS.MarginProperty<string>;
  outline: CSS.OutlineProperty<string>;
} {
  const margin = '30px';

  return {
    width: `calc(${width} - ${margin} * 2)`,
    height: `calc(${height} - ${margin} * 2)`,
    margin,
    outline: 'none',
  };
}
