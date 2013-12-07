#ifndef _SCINTEX_CLANG_SYNTAX_HIGHLIGHTER_HPP_
#define _SCINTEX_CLANG_SYNTAX_HIGHLIGHTER_HPP_

#include "syntax_highlighter.hpp"

namespace scintex
{
  class ClangSyntaxHighlighter : public SyntaxHighlighter
  {
  public:
    virtual QList<StyleRegion> stylize(TextModel *const model,
      const StylePalette *const stylePalette) const;
    virtual StylePalette *createStylePalette() const;
  };
}

#endif