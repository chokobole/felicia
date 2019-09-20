import KEY_BINDING from '@felicia-viz/ui/util/key-binding';
// @ts-ignore
import { withTheme } from '@streetscape.gl/monochrome';
import CSS from 'csstype';
import React, { Component } from 'react';
import AutoSuggest, {
  ChangeEvent,
  InputProps,
  SuggestionsFetchRequestedParams,
  Theme as AutoSuggestionTheme,
} from 'react-autosuggest';
import { COMMAND_PANEL_THEME, Theme } from '../custom-styles';
import COMMANDS, { Command, matchCommand, runAction } from './commands';

export interface Props {
  onBlur: (event: React.FocusEvent<HTMLInputElement>) => void;
  theme?: Theme;
}

export interface State {
  value: string;
  suggestions: ReadonlyArray<Command>;
}

class CommandPanel extends Component<Props, State> {
  state = {
    value: '',
    suggestions: [],
  };

  private keyBindingIds: number[] | null = null;

  componentDidMount(): void {
    if (!this.input) return;
    KEY_BINDING.bind(this.input);
    this.keyBindingIds = [];
    this.keyBindingIds.push(
      KEY_BINDING.registerAction(['Escape'], (e: KeyboardEvent) => {
        this.blur();
        e.preventDefault();
      })
    );
    this.keyBindingIds.push(
      KEY_BINDING.registerAction(['Enter'], (e: KeyboardEvent) => {
        const { value } = this.state;
        if (value !== '') {
          runAction(value);
          this.blur();
          e.preventDefault();
        }
      })
    );
    this.focus();
  }

  componentWillUnmount(): void {
    if (this.keyBindingIds) this.keyBindingIds.forEach(id => KEY_BINDING.unregisterAction(id));
    this.keyBindingIds = null;
    if (this.input) KEY_BINDING.unbind(this.input);
  }

  _onChange = (_event: React.FormEvent<HTMLInputElement>, { newValue }: ChangeEvent): void => {
    this.setState({
      value: newValue,
    });
  };

  _onBlur = (event: React.FocusEvent<HTMLInputElement>): void => {
    const { onBlur } = this.props;
    onBlur(event);
  };

  _onSuggestionsFetchRequested = ({ value }: SuggestionsFetchRequestedParams): void => {
    this.setState({
      suggestions: matchCommand(value, COMMANDS),
    });
  };

  _onSuggestionsClearRequested = (): void => {
    this.setState({
      suggestions: [],
    });
  };

  _getSuggestionValue = (suggestion: Command): string => suggestion.name;

  _renderInputComponent = (inputProps: InputProps<Command>): JSX.Element => (
    <input {...inputProps as React.InputHTMLAttributes<HTMLInputElement>} spellCheck={false} />
  );

  _renderSuggestion = (suggestion: Command, { query }: { query: string }): JSX.Element | null => {
    const { matches } = matchCommand(query, [suggestion])[0];
    if (!matches) return null;
    const parts = [];
    let lastIndex = 0;
    for (let i = 0; i < matches.length; i += 1) {
      if (lastIndex !== matches[i])
        parts.push({ text: suggestion.name.substring(lastIndex, matches[i]), highlight: false });
      parts.push({ text: suggestion.name.charAt(matches[i]), highlight: true });
      lastIndex = matches[i] + 1;
    }
    parts.push({
      text: suggestion.name.substring(lastIndex, suggestion.name.length),
      highlight: false,
    });
    return (
      <span>
        {parts.map((part, index) => {
          const className = part.highlight ? 'highlight' : undefined;
          return (
            // eslint-disable-next-line react/no-array-index-key
            <span className={className} key={index}>
              {part.text}
            </span>
          );
        })}
      </span>
    );
  };

  _onRef = (autosuggest: AutoSuggest<Command>): void => {
    if (autosuggest !== null) {
      this.input = autosuggest.input;
    }
  };

  private input?: HTMLInputElement;

  focus(): void {
    if (this.input) {
      this.input.focus();
    }
  }

  blur(): void {
    if (this.input) {
      this.input.blur();
    }
  }

  render(): JSX.Element {
    const { value, suggestions } = this.state;
    const { theme } = this.props;

    const newTheme: AutoSuggestionTheme = Object.assign(COMMAND_PANEL_THEME, {});
    if (theme) {
      if (theme.background) {
        newTheme.input = Object.assign(newTheme.input, {
          background: theme.background as CSS.BackgroundProperty<string>,
        });
      }
      if (theme.backgroundHighlight) {
        newTheme.suggestionHighlighted = Object.assign(newTheme.suggestionHighlighted, {
          background: theme.backgroundHighlight as CSS.BackgroundProperty<string>,
        });
      }
      if (theme.textColorPrimary) {
        newTheme.input = Object.assign(newTheme.input, {
          color: theme.textColorPrimary as CSS.ColorProperty,
        });
        newTheme.suggestion = Object.assign(newTheme.suggestion, {
          color: theme.textColorPrimary as CSS.ColorProperty,
        });
      }
    }

    const inputProps = {
      value,
      onChange: this._onChange,
      onBlur: this._onBlur,
      type: 'search',
    };

    return (
      <div id='command-panel'>
        <AutoSuggest
          alwaysRenderSuggestions
          suggestions={suggestions}
          onSuggestionsFetchRequested={this._onSuggestionsFetchRequested}
          onSuggestionsClearRequested={this._onSuggestionsClearRequested}
          getSuggestionValue={this._getSuggestionValue}
          renderInputComponent={this._renderInputComponent}
          renderSuggestion={this._renderSuggestion}
          inputProps={inputProps}
          ref={this._onRef}
          theme={newTheme}
        />
      </div>
    );
  }
}

export default withTheme(CommandPanel);
