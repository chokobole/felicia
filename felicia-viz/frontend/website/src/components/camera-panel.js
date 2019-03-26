import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

import ImageSequence from './image-sequence';

export default class CameraPanel extends PureComponent {
  static propTypes = {
    // User configuration
    width: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
    height: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),

    images: PropTypes.arrayOf(PropTypes.string),

    currentTime: PropTypes.number.isRequired
  };

  static defaultProps = {
    width: '100%',
    height: 'auto',
    images: []
  };

  render() {
    const { currentTime, images, width, height } = this.props;

    return <ImageSequence width={width} height={height} src={images} currentTime={currentTime} />;
  }
}
