import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

export default class ResizeDetector extends PureComponent {
  static propTypes = {
    onResize: PropTypes.func.isRequired,
  };

  constructor(props) {
    super(props);

    const { onResize } = this.props;

    this.resizeObserver = new ResizeObserver(entries => {
      onResize(entries[0]);
    });
  }

  componentWillUnmount() {
    this.resizeObserver.unobserve(this.watchable);
  }

  _onDivLoad = ref => {
    this.watchable = ref.parentNode;
    this.resizeObserver.observe(this.watchable);
  };

  render() {
    return <div ref={this._onDivLoad} />;
  }
}
