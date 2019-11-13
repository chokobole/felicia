// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

export interface FormProps<T> {
  name: string;
  title: string;
  value: T;
  isEnabled: boolean;
  onChange: (settings: string, newValue: T) => void;
}

export function renderText({ title, value }: FormProps<string>): JSX.Element {
  return (
    <PanelItemContainer>
      <Label>{title}</Label>
      <Label>{value}</Label>
    </PanelItemContainer>
  );
}
