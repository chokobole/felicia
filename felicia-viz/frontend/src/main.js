import React from 'react';
import { hydrate } from 'react-dom';
import { AppContainer } from 'react-hot-loader';
import { Provider } from 'mobx-react';
import { ThemeProvider } from '@streetscape.gl/monochrome';

import STORE from '@felicia-viz/ui/store';

import App from './components/app';
import { UI_THEME } from './custom-styles';

const rootElement = document.getElementById('root');
function renderApp() {
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

if (module.hot) module.hot.accept('./components/app', () => renderApp());
