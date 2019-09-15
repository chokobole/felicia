import PropTypes from 'prop-types';
import React, { Component } from 'react';
import AutoSuggest from 'react-autosuggest';
import { withTheme } from '@streetscape.gl/monochrome';

import { COMMAND_PANEL_THEME } from 'custom-styles';
import COMMANDS, { matchCommand, runAction } from './commands';

class CommandPanel extends Component {
  static propTypes = {
    onBlur: PropTypes.func.isRequired,
  };

  state = {
    value: '',
    suggestions: [],
  };

  componentDidMount() {
    this.input.addEventListener('keydown', this._onKeyDown);
    this.focus();
  }

  _onKeyDown = event => {
    switch (event.keyCode) {
      case 27: {
        // ESC
        this.blur();
        event.preventDefault();
        break;
      }
      case 13: {
        // Enter
        const { value } = this.state;
        if (value !== '') {
          runAction(value);
          this.blur();
          event.preventDefault();
        }
        break;
      }
      default:
        break;
    }
  };

  // eslint-disable-next-line no-unused-vars
  _onChange = (event, { newValue, method }) => {
    this.setState({
      value: newValue,
    });
  };

  // eslint-disable-next-line no-unused-vars
  _onBlur = (event, { highlightedSuggestion }) => {
    const { onBlur } = this.props;
    onBlur(event);
  };

  _onSuggestionsFetchRequested = ({ value }) => {
    this.setState({
      suggestions: matchCommand(value, COMMANDS),
    });
  };

  _onSuggestionsClearRequested = () => {
    this.setState({
      suggestions: [],
    });
  };

  _getSuggestionValue = suggestion => suggestion.name;

  _renderInputComponent = inputProps => <input {...inputProps} spellCheck={false} />;

  _renderSuggestion = (suggestion, { query }) => {
    const { matches } = matchCommand(query, [suggestion])[0];
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

  _onRef = autosuggest => {
    if (autosuggest !== null) {
      this.input = autosuggest.input;
    }
  };

  focus() {
    if (this.input) {
      this.input.focus();
    }
  }

  blur() {
    if (this.input) {
      this.input.blur();
    }
  }

  render() {
    const { value, suggestions } = this.state;
    // eslint-disable-next-line react/prop-types
    const { theme } = this.props;

    const inputProps = {
      value,
      onChange: this._onChange,
      onBlur: this._onBlur,
      type: 'search',
    };

    const newTheme = Object.assign(COMMAND_PANEL_THEME, {});
    newTheme.input.background = theme.background;
    newTheme.input.color = theme.textColorPrimary;
    newTheme.suggestionHighlighted.backgroundColor = theme.backgroundHighlight;
    newTheme.suggestion.color = theme.textColorPrimary;

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
