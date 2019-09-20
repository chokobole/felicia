/* eslint import/prefer-default-export: "off" */
// @ts-ignore
import { Label } from '@streetscape.gl/monochrome';
import React from 'react';
import styled from 'styled-components';

export const PanelItemContainer = styled.div`
  display: flex;
  flex-direction: row;
  width: 100%;
  justify-content: space-between;
`;

export interface TitledValue {
  title: string;
  value: string;
}

export function renderText({ title, value }: TitledValue): JSX.Element {
  return (
    <PanelItemContainer>
      <Label>{title}</Label>
      <Label>{value}</Label>
    </PanelItemContainer>
  );
}
