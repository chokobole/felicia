// @ts-ignore
import { Label, TextBox } from '@streetscape.gl/monochrome';
import React, { Component } from 'react';
import { FormProps, PanelItemContainer } from './panel-item';
import { Vector3Message } from '@felicia-viz/proto/messages/geometry';

export interface State {
  x: string;
  y: string;
  z: string;
}

const REGEX = /^-?([0-9]*|[0-9]+\.?[0-9]*)$/g;

export default class Vector3Input extends Component<FormProps<Vector3Message | null>, State> {
  constructor(props: FormProps<Vector3Message | null>) {
    super(props);

    const { value } = this.props;
    if (value) {
      const { x, y, z } = value;

      this.state = {
        x: x.toString(),
        y: y.toString(),
        z: z.toString(),
      };
    } else {
      this.state = {
        x: '',
        y: '',
        z: '',
      };
    }
  }

  private _onChangeX = (value: string): void => {
    const m = value.match(REGEX);
    if (m) {
      this.setState({
        x: m[0],
      });
      const { name, onChange } = this.props;
      const { y, z } = this.state;
      if (y.match(REGEX) && z.match(REGEX)) {
        onChange(
          name,
          new Vector3Message({
            x: parseFloat(m[0]),
            y: parseFloat(y),
            z: parseFloat(z),
          })
        );
      }
    }
  };

  private _onChangeY = (value: string): void => {
    const m = value.match(REGEX);
    if (m) {
      this.setState({
        y: m[0],
      });
      const { name, onChange } = this.props;
      const { x, z } = this.state;
      if (x.match(REGEX) && z.match(REGEX)) {
        onChange(
          name,
          new Vector3Message({
            x: parseFloat(x),
            y: parseFloat(m[0]),
            z: parseFloat(z),
          })
        );
      }
    }
  };

  private _onChangeZ = (value: string): void => {
    const m = value.match(REGEX);
    if (m) {
      this.setState({
        z: m[0],
      });
      const { name, onChange } = this.props;
      const { x, y } = this.state;
      if (x.match(REGEX) && y.match(REGEX)) {
        onChange(
          name,
          new Vector3Message({
            x: parseFloat(x),
            y: parseFloat(y),
            z: parseFloat(m[0]),
          })
        );
      }
    }
  };

  render(): JSX.Element {
    const { title } = this.props;
    const { x, y, z } = this.state;
    const style = {
      wrapper: {
        marginLeft: '10px',
      },
    };

    return (
      <PanelItemContainer>
        <Label>{title}</Label>
        <div>
          <PanelItemContainer>
            <Label>{'x'}</Label>
            <TextBox style={style} value={x} onChange={this._onChangeX} showClearButton={false} />
          </PanelItemContainer>
          <PanelItemContainer>
            <Label>{'y'}</Label>
            <TextBox style={style} value={y} onChange={this._onChangeY} showClearButton={false} />
          </PanelItemContainer>
          <PanelItemContainer>
            <Label>{'z'}</Label>
            <TextBox style={style} value={z} onChange={this._onChangeZ} showClearButton={false} />
          </PanelItemContainer>
        </div>
      </PanelItemContainer>
    );
  }
}
