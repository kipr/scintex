#ifndef _SCINTEX_SYNTAX_HIGHLIGHTER_HPP_
#define _SCINTEX_SYNTAX_HIGHLIGHTER_HPP_

namespace scintex
{
  class ColorPalette;
  
  class SyntaxHighlighter
  {
  public:
    virtual ~SyntaxHighlighter();
    

    virtual ColorPalette *createColorPalette() const = 0;
  };
}

#endif