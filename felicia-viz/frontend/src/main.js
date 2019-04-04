import React from 'react';
import { hydrate } from 'react-dom';
import { AppContainer } from 'react-hot-loader';
import { Provider } from 'mobx-react';

import App from './components/app';
import STORE from './store';

const rootElement = document.getElementById('root');
function renderApp() {
  hydrate(
    <Provider store={STORE}>
      <AppContainer>
        <App />
      </AppContainer>
    </Provider>,
    rootElement
  );
}

renderApp();

if (module.hot) module.hot.accept('./components/app', () => renderApp());
