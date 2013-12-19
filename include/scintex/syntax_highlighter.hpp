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
    SyntaxHighlighter();
    virtual ~SyntaxHighlighter();
    
    virtual void setTextModel(TextModel *const model);
    virtual TextModel *textModel() const;
    
    virtual void setStylePalette(const StylePalette *const stylePalette);
    virtual const StylePalette *stylePalette() const;
    
    virtual QList<StyleRegion> stylize(const Region &region) = 0;
    virtual StylePalette *createStylePalette() const = 0;
    
  private:
    TextModel *_model;
    const StylePalette *_stylePalette;
  };
}

#endif