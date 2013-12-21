#ifndef _SCINTEX_CLANG_SYNTAX_HIGHLIGHTER_HPP_
#define _SCINTEX_CLANG_SYNTAX_HIGHLIGHTER_HPP_

#include "syntax_highlighter.hpp"
#include "compat.hpp"

namespace scintex
{
  class ClangContext;
  
  class DLL_EXPORT ClangSyntaxHighlighter : public SyntaxHighlighter
  {
  public:
    ClangSyntaxHighlighter();
    
    void setIncludes(const QStringList &includes);
    const QStringList &includes() const;
    
    void setIncludeDirectories(const QStringList &includeDirectories);
    const QStringList &includeDirectories() const;
    
    virtual void setTextModel(TextModel *const model);
    
    virtual QList<StyleRegion> stylize(const Region &region);
    
    virtual StylePalette *createStylePalette() const;
    
  private:
    QStringList args() const;
    bool writeModel() const;
    void refreshContext();
    
    QStringList _includes;
    QStringList _includeDirectories;
    QString _filePath;
    ClangContext *_context;
  };
}

#endif