import React, { PureComponent } from 'react';
import ResizeObserver from 'resize-observer-polyfill';

export interface Props {
  onResize: (entry: ResizeObserverEntry) => void;
}

export default class ResizeDetector extends PureComponent<Props> {
  private watchable: Element | null = null;

  constructor(props: Props) {
    super(props);

    const { onResize } = this.props;

    this.resizeObserver = new ResizeObserver(
      (entries: ResizeObserverEntry[]): void => {
        onResize(entries[0]);
      }
    );
  }

  componentWillUnmount(): void {
    if (this.watchable) this.resizeObserver.unobserve(this.watchable);
  }

  private _onDivLoad = (ref: HTMLDivElement): void => {
    if (!ref) return;
    this.watchable = ref.parentNode as Element;
    if (this.watchable) this.resizeObserver.observe(this.watchable);
  };

  private resizeObserver: ResizeObserver;

  render(): JSX.Element {
    return <div ref={this._onDivLoad} />;
  }
}
