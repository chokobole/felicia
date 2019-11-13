// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import STORE from '@felicia-viz/ui/store';
// @ts-ignore
import { ThemeProvider } from '@streetscape.gl/monochrome';
import { Provider } from 'mobx-react';
import React from 'react';
import { hydrate } from 'react-dom';
import { AppContainer } from 'react-hot-loader';
import App from './components/app';
import { UI_THEME } from './custom-styles';

const rootElement = document.getElementById('root');
function renderApp(): void {
  hydrate(
    <Provider store={STORE}>
      <AppContainer>
        <ThemeProvider theme={UI_THEME}>
          <App />
        </ThemeProvider>
      </AppContainer>
    </Provider>,
    rootElement
  );
}

renderApp();

if ((module as any).hot) (module as any).hot.accept('./components/app', () => renderApp());
