import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

import ImageView from 'components/image-view';

export default class CameraPanel extends PureComponent {
  static propTypes = {
    // User configuration
    width: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
    height: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),

    currentTime: PropTypes.number,
  };

  static defaultProps = {
    width: '100%',
    height: 'auto',

    currentTime: 0,
  };

  render() {
    const { currentTime, width, height } = this.props;
    return <ImageView width={width} height={height} src={`images/image${currentTime}.jpg`} />;
  }
}
