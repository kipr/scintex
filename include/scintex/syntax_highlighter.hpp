#ifndef _SCINTEX_SYNTAX_HIGHLIGHTER_HPP_
#define _SCINTEX_SYNTAX_HIGHLIGHTER_HPP_

#include <QList>
#include "color_region.hpp"

namespace scintex
{
  class ColorPalette;
  class TextModel;
  
  class SyntaxHighlighter
  {
  public:
    virtual ~SyntaxHighlighter();
    
    virtual QList<ColorRegion> colorize(TextModel *const model) const = 0;
    virtual ColorPalette *createColorPalette() const = 0;
  };
}

#endif