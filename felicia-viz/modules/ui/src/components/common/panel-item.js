/* eslint import/prefer-default-export: "off" */
import React from 'react';
import styled from 'styled-components';
import { Label } from '@streetscape.gl/monochrome';

export const PanelItemContainer = styled.div`
  display: flex;
  flex-direction: row;
  width: 100%;
  justify-content: space-between;
`;

export function renderText(self) {
  const { title, value } = self;
  return (
    <PanelItemContainer>
      <Label>{title}</Label>
      <Label>{value}</Label>
    </PanelItemContainer>
  );
}
