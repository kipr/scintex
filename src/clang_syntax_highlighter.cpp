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
  addRole("code/documentation");
  addRole("code/statement");
  addRole("code/declaration/variable/type");
  addRole("code/declaration/variable/name");
  addRole("code/declaration/enum_constant");
  addRole("code/declaration/field/type");
  addRole("code/declaration/field/name");
  addRole("code/declaration/class/keyword");
  addRole("code/declaration/class/name");
  addRole("code/declaration/struct/keyword");
  addRole("code/declaration/struct/name");
  addRole("code/declaration/enum/keyword");
  addRole("code/declaration/enum/name");
  addRole("code/declaration/struct");
  addRole("code/declaration/union/keyword");
  addRole("code/declaration/union/name");
  addRole("code/declaration/parameter");
  addRole("code/declaration/function/return_type");
  addRole("code/declaration/function/name");
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

struct Data
{
  TextModel *model;
  const ClangStylePalette *palette;
  QList<StyleRegion> *ret;
};

Region rangeRegion(const CXSourceRange &range)
{
  CXSourceLocation start = clang_getRangeStart(range);
  unsigned startOffset = 0;
  clang_getExpansionLocation(start, 0, 0, 0, &startOffset);
  
  CXSourceLocation end = clang_getRangeEnd(range);
  unsigned endOffset = 0;
  clang_getExpansionLocation(end, 0, 0, 0, &endOffset);
  
  return Region(startOffset, endOffset);
}

Region cursorRegion(const CXCursor &cur)
{
  const CXSourceRange range = clang_getCursorExtent(cur);
  return rangeRegion(range);
}

Region next(const QString &str, const QRegExp &reg, int &i, const Region &r)
{
  if(i < 0 || (i = reg.indexIn(str, i)) < 0) return Region(0, 0);
  const Region ret(i + r.start(), i + r.start() + reg.matchedLength());
  i += reg.matchedLength();
  return ret;
}

static CXChildVisitResult visitor(CXCursor cur, CXCursor parent, CXClientData data)
{
  Data *const d = (Data *)data;

  const Region r = cursorRegion(cur);
  const QString str = d->model->read(r);
  
  const QRegExp word("\\w+");
  
  if(cur.kind == CXCursor_FunctionDecl) {
    int i = 0;
    Region returnType = next(str, word, i, r);
    Region name = next(str, word, i, r);
    d->ret->append(StyleRegion(returnType, d->palette->style("code/declaration/function/return_type",
      Style())));
    if(d->model->read(name) != "{") {
      d->ret->append(StyleRegion(name, d->palette->style("code/declaration/function/name",
        Style())));
    }
  } else if(cur.kind == CXCursor_ClassDecl) {
    int i = 0;
    Region keyword  = next(str, word, i, r);
    Region name = next(str, word, i, r);
    d->ret->append(StyleRegion(keyword, d->palette->style("code/declaration/class/keyword",
      Style())));
    if(d->model->read(name) != "{") {
      d->ret->append(StyleRegion(name, d->palette->style("code/declaration/class/name",
        Style())));
    }
  } else if(cur.kind == CXCursor_StructDecl) {
    int i = 0;
    Region keyword  = next(str, word, i, r);
    Region name = next(str, word, i, r);
    d->ret->append(StyleRegion(keyword, d->palette->style("code/declaration/struct/keyword",
      Style())));
    if(d->model->read(name) != "{") {
      d->ret->append(StyleRegion(name, d->palette->style("code/declaration/struct/name",
        Style())));
    }
  } else if(cur.kind == CXCursor_VarDecl) {
    int i = 0;
    Region type = next(str, word, i, r);
    Region name = next(str, word, i, r);
    d->ret->append(StyleRegion(type, d->palette->style("code/declaration/variable/type",
      Style())));
    d->ret->append(StyleRegion(name, d->palette->style("code/declaration/variable/name",
      Style())));
  } else if(cur.kind == CXCursor_UnionDecl) {
    int i = 0;
    Region type = next(str, word, i, r);
    Region name = next(str, word, i, r);
    d->ret->append(StyleRegion(type, d->palette->style("code/declaration/union/keyword",
      Style())));
    if(d->model->read(name) != "{") {
      d->ret->append(StyleRegion(name, d->palette->style("code/declaration/union/name",
        Style())));
    }
  } else if(cur.kind == CXCursor_EnumDecl) {
    int i = 0;
    Region type = next(str, word, i, r);
    Region name = next(str, word, i, r);
    d->ret->append(StyleRegion(type, d->palette->style("code/declaration/enum/keyword",
      Style())));
    if(d->model->read(name) != "{") {
      d->ret->append(StyleRegion(name, d->palette->style("code/declaration/enum/name",
        Style())));
    }
  } else if(cur.kind == CXCursor_FieldDecl) {
    int i = 0;
    Region type = next(str, word, i, r);
    Region name = next(str, word, i, r);
    d->ret->append(StyleRegion(type, d->palette->style("code/declaration/field/type",
      Style())));
    d->ret->append(StyleRegion(name, d->palette->style("code/declaration/field/name",
      Style())));
  }
  
  
  
  Region comment = rangeRegion(clang_Cursor_getCommentRange(cur));
  if(comment.size() > 0) {
    d->ret->append(StyleRegion(comment, d->palette->style("code/documentation",
      Style())));
  }
  
  
  return CXChildVisit_Recurse;
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
  
  QList<StyleRegion> ret;
  Data data;
  data.model = model;
  data.palette = ccp;
  data.ret = &ret;
  clang_visitChildren(clang_getTranslationUnitCursor(unit), &visitor, (void *)&data);

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
        
        ret.removeAt(j);
        ret.removeAt(i);
        
        if(s[0].size() > 0) ret.append(StyleRegion(s[0], bc.style()));
        if(s[1].size() > 0) ret.append(StyleRegion(s[1], lc.style()));
        if(s[2].size() > 0) ret.append(StyleRegion(s[2], bc.style()));
        
        goto split;
      }
    }
  }

  
  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);
  
  return ret;
  
}

StylePalette *ClangSyntaxHighlighter::createStylePalette() const
{
  return new ClangStylePalette();
}