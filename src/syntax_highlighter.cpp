#include <scintex/syntax_highlighter.hpp>

using namespace scintex;

SyntaxHighlighter::SyntaxHighlighter()
  : _model(0)
  , _stylePalette(0)
{
}

void SyntaxHighlighter::setTextModel(TextModel *const model)
{
  _model = model;
}

TextModel *SyntaxHighlighter::textModel() const
{
  return _model;
}

void SyntaxHighlighter::setStylePalette(const StylePalette *const stylePalette)
{
  _stylePalette = stylePalette;
}

const StylePalette *SyntaxHighlighter::stylePalette() const
{
  return _stylePalette;
}

SyntaxHighlighter::~SyntaxHighlighter()
{
}