import React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'mobx-react';

import App from './components/app';
import STORE from './store';

// import './stylesheets/main.scss';

// require('./static/favicon.png');

const rootElement = document.getElementById('root');
ReactDOM.render(
  <Provider store={STORE}>
    <App />
  </Provider>,
  rootElement
);
