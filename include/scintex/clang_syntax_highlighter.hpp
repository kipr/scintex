#ifndef _SCINTEX_CLANG_SYNTAX_HIGHLIGHTER_HPP_
#define _SCINTEX_CLANG_SYNTAX_HIGHLIGHTER_HPP_

#include "syntax_highlighter.hpp"

namespace scintex
{
  class ClangSyntaxHighlighter : public SyntaxHighlighter
  {
  public:
    void setIncludes(const QStringList &includes);
    const QStringList &includes() const;
    
    void setIncludeDirectories(const QStringList &includeDirectories);
    const QStringList &includeDirectories() const;
    
    
    virtual QList<StyleRegion> stylize(TextModel *const model,
    
    const StylePalette *const stylePalette) const;
    virtual StylePalette *createStylePalette() const;
    
  private:
    QStringList _includes;
    QStringList _includeDirectories;
    
  };
}

#endif