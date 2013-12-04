#ifndef _SCINTEX_CLANG_SYNTAX_HIGHLIGHTER_HPP_
#define _SCINTEX_CLANG_SYNTAX_HIGHLIGHTER_HPP_

#include "syntax_highlighter.hpp"

namespace scintex
{
  class ClangSyntaxHighlighter : public SyntaxHighlighter
  {
  public:
    virtual QList<ColorRegion> colorize(TextModel *const model) const;
    virtual ColorPalette *createColorPalette() const;
    
  private:
    
  };
}

#endif