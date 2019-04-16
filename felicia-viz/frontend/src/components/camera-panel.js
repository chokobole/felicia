import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

import ImageView from 'components/image-view';
import { CameraFrame } from 'store/camera';

export default class CameraPanel extends PureComponent {
  static propTypes = {
    // User configuration
    width: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
    height: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),

    currentTime: PropTypes.number,
    frame: PropTypes.instanceOf(CameraFrame),
  };

  static defaultProps = {
    width: '100%',
    height: 'auto',

    currentTime: 0,
    frame: null,
  };

  render() {
    const { frame, width, height } = this.props;
    return <ImageView width={width} height={height} frame={frame} />;
  }
}
