#ifndef _SCINTEX_SYNTAX_HIGHLIGHTER_HPP_
#define _SCINTEX_SYNTAX_HIGHLIGHTER_HPP_

#include <QList>
#include "style_region.hpp"

namespace scintex
{
  class StylePalette;
  class TextModel;
  
  class SyntaxHighlighter
  {
  public:
    virtual ~SyntaxHighlighter();
    
    virtual QList<StyleRegion> stylize(TextModel *const model,
      const StylePalette *const stylePalette) const = 0;
    virtual StylePalette *createStylePalette() const = 0;
  };
}

#endif