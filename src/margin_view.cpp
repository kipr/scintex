#include <scintex/margin_view.hpp>

using namespace scintex;

MarginView::MarginView()
  : _textView(0)
  , _stylePalette(0)
{
}

MarginView::~MarginView()
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

void MarginView::setStylePalette(const StylePalette *const stylePalette)
{
  _stylePalette = stylePalette;
  if(isEngaged()) engaged();
}

const StylePalette *MarginView::stylePalette() const
{
  return _stylePalette;
}

bool MarginView::isEngaged() const
{
  return _textView && _stylePalette;
}

void MarginView::disengage()
{
  setTextView(0);
  setStylePalette(0);
}

void MarginView::engaged()
{
  
}