/* eslint import/prefer-default-export: "off" */
import React from 'react';

import { Label } from '@streetscape.gl/monochrome';

import PanelItemContainer from './panel-item-container';

export function renderText(self) {
  const { title, value } = self;
  return (
    <PanelItemContainer>
      <Label>{title}</Label>
      <Label>{value}</Label>
    </PanelItemContainer>
  );
}
