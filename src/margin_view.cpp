#include <scintex/margin_view.hpp>

using namespace scintex;

MarginView::MarginView()
  : _textView(0)
  , _colorPalette(0)
{
}

void MarginView::setTextView(TextView *const textView)
{
  _textView = textView;
  if(isEngaged()) engaged();
}

TextView *MarginView::textView() const
{
  return _textView;
}

void MarginView::setColorPalette(const ColorPalette *const colorPalette)
{
  _colorPalette = colorPalette;
  if(isEngaged()) engaged();
}

const ColorPalette *MarginView::colorPalette() const
{
  return _colorPalette;
}

bool MarginView::isEngaged() const
{
  return _textView && _colorPalette;
}

void MarginView::disengage()
{
  setTextView(0);
  setColorPalette(0);
}

void MarginView::engaged()
{
  
}