#include <scintex/clang_syntax_highlighter.hpp>
#include <scintex/text_model.hpp>
#include <scintex/basic_style_palette.hpp>
#include <clang-c/Index.h>

#include <QDir>
#include <QFile>
#include <QByteArray>

#include <QDebug>
#include <iostream>


namespace scintex
{
  class ClangStylePalette : public BasicStylePalette
  {
  public:
    friend class ClangSyntaxHighlighter;
    
    ClangStylePalette();
    
  private:
    QString cursorToRole(const CXCursor &cursor) const;
  };
}

using namespace scintex;

ClangStylePalette::ClangStylePalette()
{
  addRole("code/preprocessor/directive");
  addRole("code/preprocessor/definition");
  addRole("code/preprocessor/expansion");
  addRole("code/literal/number");
  addRole("code/literal/string");
  addRole("code/literal/character");
  addRole("code/expression/operator/ternary");
  addRole("code/expression/operator/binary");
  addRole("code/expression/operator/unary");
  addRole("code/expression/cast/c_style");
  addRole("code/expression/cast/c++_style");
  addRole("code/reference/declaration");
  addRole("code/reference/namespace");
  addRole("code/reference/member");
  addRole("code/reference/template");
  addRole("code/reference/label");
  addRole("code/reference/type");
  addRole("code/reference/overloaded_decl");
  addRole("code/reference/variable");
  addRole("code/comment");
  addRole("code/statement");
  addRole("code/declaration/variable");
  addRole("code/declaration/enum_constant");
  addRole("code/declaration/field");
  addRole("code/declaration/class");
  addRole("code/declaration/enum");
  addRole("code/declaration/struct");
  addRole("code/declaration/union");
  addRole("code/declaration/parameter");
  addRole("code/declaration/function");
}

QString ClangStylePalette::cursorToRole(const CXCursor &cursor) const
{
  switch(cursor.kind) {
    case CXCursor_IntegerLiteral:
    case CXCursor_FloatingLiteral:
    case CXCursor_ImaginaryLiteral:
    case CXCursor_CXXBoolLiteralExpr:
      return "code/literal/number";
    case CXCursor_StringLiteral:
      return "code/literal/string";
    case CXCursor_CharacterLiteral:
      return "code/literal/character";
    case CXCursor_UnaryOperator:
      return "code/expression/operator/unary";
    case CXCursor_BinaryOperator:
      return "code/expression/operator/binary";
    case CXCursor_ConditionalOperator:
      return "code/expression/operator/ternary";
    case CXCursor_CXXDynamicCastExpr:
    case CXCursor_CXXReinterpretCastExpr:
    case CXCursor_CXXStaticCastExpr:
    case CXCursor_CXXConstCastExpr:
    case CXCursor_CXXFunctionalCastExpr:
      return "code/expression/cast/c++_style";
    case CXCursor_CStyleCastExpr:
      return "code/expression/cast/c_style";
      
    case CXCursor_DeclRefExpr:
      return "code/reference/declaration";
    case CXCursor_TemplateRef:
      return "code/reference/template";
    case CXCursor_VariableRef:
      return "code/reference/variable";
    case CXCursor_MemberRef:
      return "code/reference/member";
    case CXCursor_NamespaceRef:
      return "code/reference/namespace";
    case CXCursor_LabelRef:
      return "code/reference/label";
    case CXCursor_OverloadedDeclRef:
      return "code/reference/overloaded_decl";
    case CXCursor_TypeRef:
      return "code/reference/type";
      
    
    case CXCursor_PreprocessingDirective:
      return "code/preprocessor/directive";
    case CXCursor_MacroDefinition:
      return "code/preprocessor/definition";
    case CXCursor_MacroExpansion:
      return "code/preprocessor/expansion";
    
    case CXCursor_CaseStmt:
    case CXCursor_DefaultStmt:
    case CXCursor_IfStmt:
    case CXCursor_SwitchStmt:
    case CXCursor_WhileStmt:
    case CXCursor_DoStmt:
    case CXCursor_ForStmt:
    case CXCursor_GotoStmt:
    case CXCursor_ContinueStmt:
    case CXCursor_BreakStmt:
    case CXCursor_ReturnStmt:
    case CXCursor_CXXTryStmt:
    case CXCursor_CXXCatchStmt:
    case CXCursor_CXXForRangeStmt:
    case CXCursor_DeclStmt:
    case CXCursor_UsingDirective:
      return "code/statement";
    
    case CXCursor_ParmDecl:
      return "code/declaration/parameter";
    case CXCursor_FunctionDecl:
      return "code/declaration/function";
    case CXCursor_EnumDecl:
      return "code/declaration/enum";
    case CXCursor_ClassDecl:
      return "code/declaration/class";
    case CXCursor_StructDecl:
      return "code/declaration/struct";
    case CXCursor_EnumConstantDecl:
      return "code/declaration/enum_constant";
    case CXCursor_UnionDecl:
      return "code/declaration/union";
    case CXCursor_VarDecl:
      return "code/declaration/variable";
    case CXCursor_FieldDecl:
      return "code/declaration/field";
  }
  return "";
}

static QDebug &operator <<(QDebug &stream, const Region &r)
{
  stream << "(" << r.start() << "to" << r.end() << ")";
  return stream;
}

bool regionSize(const StyleRegion &_1, const StyleRegion &_2)
{
    return _1.region().size() > _2.region().size();
}

QList<StyleRegion> ClangSyntaxHighlighter::stylize(TextModel *const model,
  const StylePalette *const stylePalette) const
{
  const ClangStylePalette *const ccp = dynamic_cast<const ClangStylePalette *>(stylePalette);
  
  const QString path = QDir::temp().absoluteFilePath("highlight.cpp");
  QFile file(path);
  
  if(!file.open(QIODevice::WriteOnly)) {
    QFileInfo info(file);
    qWarning() << "Failed to open" << info.absoluteFilePath() << "for writing";
    return QList<StyleRegion>();
  }
  
  file.write(model->read(model->fullRegion()).toUtf8());
  
  file.close();
  
  const QByteArray pathUtf = path.toUtf8();
  
  const char *const argv[1] = { pathUtf.data() };
  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit unit = clang_parseTranslationUnit(index, 0, 
   argv, 1, 0, 0, CXTranslationUnit_None);
  
  CXCursor cursor = clang_getTranslationUnitCursor(unit);
  CXSourceRange sr = clang_getCursorExtent(cursor);
  
  CXToken *tokens;
  unsigned numTokens = 0;
  
  clang_tokenize(unit, sr, &tokens, &numTokens);
  
  CXCursor *cursors = new CXCursor[numTokens];
  clang_annotateTokens(unit, tokens, numTokens, cursors);
  
  QList<StyleRegion> ret;
  for (int i = 0; i < numTokens; ++i) {
    CXCursor &cur = cursors[i];

    CXSourceRange range = clang_getCursorExtent(cur);
    
    CXSourceLocation start = clang_getRangeStart(range);
    unsigned startOffset = 0;
    clang_getExpansionLocation(start, 0, 0, 0, &startOffset);
    
    CXSourceLocation end = clang_getRangeEnd(range);
    unsigned endOffset = 0;
    clang_getExpansionLocation(end, 0, 0, 0, &endOffset);
    
    const Region r(startOffset, endOffset);
    const QString role = ccp->cursorToRole(cur);
    
    const Style c = ccp->style(role, Style());

    if(c.color().isValid()) ret << StyleRegion(r, c);
    else qDebug() << model->read(r);
  }

  
  
  
  split:
  qSort(ret.begin(), ret.end(), regionSize);
  for(size_t i = 0; i < ret.size(); ++i) {
    const StyleRegion bc = ret[i];
    const Region br = bc.region();
  
    for(size_t j = i + 1; j < ret.size(); ++j) {
      const StyleRegion lc = ret[j];
      const Region lr = lc.region();
  
      // If this region is completely contained within another region,
      // split based off of color to create three regions (with two possibly empty)
      if(lr.start() >= br.start() && lr.end() <= br.end()) {
        const Region s[3] = {
          Region(br.start(), lr.start()),
          Region(lr.start(), lr.end()),
          Region(lr.end(), br.end())
        };

        // qDebug() << "sec1" << s[0] << "sec2" << s[1] << "sec3" << s[2];
        
        ret.removeAt(j);
        ret.removeAt(i);

        
        if(s[0].size() > 0) ret.append(StyleRegion(s[0], bc.style()));
        if(s[1].size() > 0) ret.append(StyleRegion(s[1], lc.style()));
        if(s[2].size() > 0) ret.append(StyleRegion(s[2], bc.style()));
        
        goto split;
      }
    }
  }

  
  delete[] cursors;
  clang_disposeTokens(unit, tokens, numTokens);
  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);
  
  return ret;
  
}

StylePalette *ClangSyntaxHighlighter::createStylePalette() const
{
  return new ClangStylePalette();
}