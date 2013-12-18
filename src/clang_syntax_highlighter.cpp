#include <scintex/clang_syntax_highlighter.hpp>
#include <scintex/text_model.hpp>
#include <scintex/basic_style_palette.hpp>
#include <clang-c/Index.h>

#include "clang_shared.hpp"

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
  addRole("code/preprocessor/definition");
  addRole("code/preprocessor/expansion");
  addRole("code/preprocessor/include");
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
  addRole("code/statement/return");
  addRole("code/statement/while");
  addRole("code/statement/do");
  addRole("code/statement/for");
  addRole("code/statement/goto");
  addRole("code/statement/indirect_goto");
  addRole("code/statement/break");
  addRole("code/statement/continue");
  addRole("code/statement/switch");
  addRole("code/statement/case");
  addRole("code/statement/default_case");
  addRole("code/statement/if");
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
  addRole("code/declaration/parameter/type");
  addRole("code/declaration/parameter/name");
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

static Region next(const QString &str, const QRegExp &reg, int &i, const Region &r)
{
  if(i < 0 || (i = reg.indexIn(str, i)) < 0) return Region(0, 0);
  const Region ret(i + r.start(), i + r.start() + reg.matchedLength());
  i += reg.matchedLength();
  return ret;
}

static QList<StyleRegion> handleDecl(const QString &str, const QRegExp &reg, const Region &r,
  const QString &first, const QString &second, TextModel *const model, const StylePalette *const p)
{
  QList<StyleRegion> ret;
  int i = 0;
  Region _1 = next(str, reg, i, r);
  Region _2 = next(str, reg, i, r);
  ret << StyleRegion(_1, p->style("code/declaration/" + first, Style()));
  if(model->read(_2) != "{") {
    ret << StyleRegion(_2, p->style("code/declaration/" + second, Style()));
  }
  return ret;
}

static CXChildVisitResult visitor(CXCursor cur, CXCursor parent, CXClientData data)
{
  const Region r = cursorRegion(cur);
  if(r.size() < 1 || clang_isPreprocessing(parent.kind)) return CXChildVisit_Continue;
  
  Data *const d = (Data *)data;
  const StylePalette *const p = d->palette;
  TextModel *const m = d->model;
  const QString s = m->read(r);
  
  if(s.isEmpty()) return CXChildVisit_Continue;
  
  QList<StyleRegion> &ret = *d->ret;
  const QRegExp id("\\w+");
  
  bool recurse = true;
  
  if(cur.kind == CXCursor_FunctionDecl) ret << handleDecl(s, id, r, "function/return_type",
    "function/name", m, p);
  else if(cur.kind == CXCursor_ClassDecl) ret << handleDecl(s, id, r, "class/keyword",
    "class/name", m, p);
  else if(cur.kind == CXCursor_StructDecl) ret << handleDecl(s, id, r, "struct/keyword",
    "struct/name", m, p);
  else if(cur.kind == CXCursor_VarDecl) ret << handleDecl(s, id, r, "variable/type",
    "variable/name", m, p);
  else if(cur.kind == CXCursor_UnionDecl) ret << handleDecl(s, id, r, "union/keyword",
    "union/name", m, p);
  else if(cur.kind == CXCursor_EnumDecl) ret << handleDecl(s, id, r, "enum/keyword",
    "enum/name", m, p);
  else if(cur.kind == CXCursor_FieldDecl) ret << handleDecl(s, id, r, "field/type",
    "field/name", m, p);
  else if(cur.kind == CXCursor_ParmDecl) ret << handleDecl(s, id, r, "parameter/type",
    "parameter/name", m, p);
  else if(cur.kind == CXCursor_IntegerLiteral || cur.kind == CXCursor_FloatingLiteral) {
    ret << StyleRegion(r, p->style("code/literal/number"));
  } else if(cur.kind == CXCursor_CharacterLiteral) {
    ret << StyleRegion(r, p->style("code/literal/character"));
  } else if(cur.kind == CXCursor_StringLiteral) {
    ret << StyleRegion(r, p->style("code/literal/string"));
  } else if(cur.kind == CXCursor_DeclRefExpr) {
    ret << StyleRegion(r, p->style("code/reference/declaration"));
  } else if(cur.kind == CXCursor_ReturnStmt) {
    ret << StyleRegion(r, p->style("code/statement/return"));
  } else if(cur.kind == CXCursor_IfStmt) {
    ret << StyleRegion(r, p->style("code/statement/if"));
  } else if(cur.kind == CXCursor_BreakStmt) {
    ret << StyleRegion(r, p->style("code/statement/break"));
  } else if(cur.kind == CXCursor_GotoStmt) {
    ret << StyleRegion(r, p->style("code/statement/goto"));
  } else if(cur.kind == CXCursor_ContinueStmt) {
    ret << StyleRegion(r, p->style("code/statement/continue"));
  } else if(cur.kind == CXCursor_WhileStmt) {
    int i = 0;
    Region _1 = next(s, id, i, r);
    ret << StyleRegion(_1, p->style("code/statement/while"));
  } else if(cur.kind == CXCursor_DoStmt) {
    ret << StyleRegion(r, p->style("code/statement/do"));
  } else if(cur.kind == CXCursor_IndirectGotoStmt) {
    ret << StyleRegion(r, p->style("code/statement/indirect_goto"));
  } else if(cur.kind == CXCursor_SwitchStmt) {
    int i = 0;
    Region _1 = next(s, id, i, r);
    ret << StyleRegion(_1, p->style("code/statement/switch"));
  } else if(cur.kind == CXCursor_ForStmt) {
    int i = 0;
    Region _1 = next(s, id, i, r);
    ret << StyleRegion(_1, p->style("code/statement/for"));
  } else if(cur.kind == CXCursor_DefaultStmt) {
    ret << StyleRegion(r, p->style("code/statement/default"));
  } else if(cur.kind == CXCursor_ReturnStmt) {
    Style s = p->style("code/statement/return");
    s.setUnderline(true);
    s.setUnderlineColor(Qt::black);
    ret << StyleRegion(r, s);
  } else if(cur.kind == CXCursor_MacroDefinition) {
    qDebug() << "MACRO DEFINITION" << m->read(r);
    ret << StyleRegion(r, p->style("code/preprocessor/definition"));
    recurse = false;
  } else if(cur.kind == CXCursor_MacroExpansion) {
    ret << StyleRegion(r, p->style("code/preprocessor/expansion"));
    recurse = false;
  } else if(cur.kind == CXCursor_InclusionDirective) {
    ret << StyleRegion(r, p->style("code/preprocessor/include"));
    recurse = false;
  } else {
    qDebug() << "unhandled" << cur.kind << " " << m->read(cursorRegion(cur));
  }
  
  Region comment = rangeRegion(clang_Cursor_getCommentRange(cur));
  if(comment.size() > 0) {
    d->ret->append(StyleRegion(comment, d->palette->style("code/documentation",
      Style())));
  }
  
  
  return recurse ? CXChildVisit_Recurse : CXChildVisit_Continue;
}

void ClangSyntaxHighlighter::setIncludes(const QStringList &includes)
{
  _includes = includes;
}

const QStringList &ClangSyntaxHighlighter::includes() const
{
  return _includes;
}

void ClangSyntaxHighlighter::setIncludeDirectories(const QStringList &includeDirectories)
{
  _includeDirectories = includeDirectories;
}

const QStringList &ClangSyntaxHighlighter::includeDirectories() const
{
  return _includeDirectories;
}

QList<StyleRegion> ClangSyntaxHighlighter::stylize(TextModel *const model,
  const StylePalette *const stylePalette) const
{
  const ClangStylePalette *const ccp = dynamic_cast<const ClangStylePalette *>(stylePalette);
  
  ////////////////////////////////////////
  // PART 0 -- Create file to highlight //
  ////////////////////////////////////////
  const QString path = QDir::temp().absoluteFilePath("highlight.cpp");
  QFile file(path);
  
  if(!file.open(QIODevice::WriteOnly)) {
    QFileInfo info(file);
    qWarning() << "Failed to open" << info.absoluteFilePath() << "for writing";
    return QList<StyleRegion>();
  }
  
  const QString contents = model->read(model->fullRegion());
  file.write(contents.toUtf8());
  file.close();
  
  QString argString;
  
  Q_FOREACH(const QString &includeDirectory, _includeDirectories) {
    argString += "-I\"" + includeDirectory + "\" ";
  }
  
  Q_FOREACH(const QString &include, _includes) {
    argString += "-include \"" + include + "\" ";
  }
  
  const QStringList args = argString.trimmed().split(" ");
  char **const argv = new char *[args.size() + 1];
  const QByteArray pathUtf = path.toUtf8();
  argv[0] = new char[pathUtf.size() + 1];
  memcpy(argv[0], pathUtf.data(), pathUtf.size());
  argv[0][pathUtf.size()] = 0;
  for(quint32 i = 0; i < args.size(); ++i) {
    const QByteArray arg = args[i].toUtf8().data();
    argv[i + 1] = new char[arg.size() + 1];
    memcpy(argv[i + 1], arg.data(), arg.size());
    argv[i + 1][arg.size()] = 0;
  }
  

  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit unit = clang_parseTranslationUnit(index, 0, 
   argv, args.size() + 1, 0, 0, CXTranslationUnit_None);
  
  for(quint32 i = 0; i < args.size() + 1; ++i) delete[] argv[i];
  delete[] argv;
  
  CXCursor cursor = clang_getTranslationUnitCursor(unit);
  CXSourceRange sr = clang_getCursorExtent(cursor);
  
  QList<StyleRegion> ret;
  
  /////////////////////////////////////
  // PART 1 -- Get comment locations //
  /////////////////////////////////////
  CXToken *tokens;
  unsigned numTokens = 0;
  clang_tokenize(unit, sr, &tokens, &numTokens);
  for (int i = 0; i < numTokens; ++i) {
    CXToken &tok = tokens[i];
    const Region r = rangeRegion(clang_getTokenExtent(unit, tok));
    
    const CXTokenKind kind = clang_getTokenKind(tok);
    Style c;
    if(kind == CXToken_Comment) c = ccp->style("code/comment");
    else continue;
    
    
    if(c.color().isValid()) ret << StyleRegion(r, c);
  }
  
  clang_disposeTokens(unit, tokens, numTokens);
  
  {
    bool sawCont = false;
    bool inPrep = false;
    bool inString = false;
    Region current;
    for(quint32 i = 0; i < contents.size(); ++i) {
      const QChar c = contents[i];
      if(c == '\"' || c == '\'') {
        if(contents[i - 1] != '\\') inString = !inString;
      }
      if(!inString && c == '#') {
        inPrep = true;
        current.setStart(i);
      }
      if(inPrep && c == '\\') sawCont = true;
      if(inPrep && c == '\n') {
        inPrep = sawCont;
        sawCont = false;
      
        if(!inPrep) {
          current.setEnd(i);
          ret << StyleRegion(current, ccp->style("code/preprocessor/definition"));
        }
      }
    }
  }

  ////////////////////////////
  // PART 2 -- Traverse AST //
  ////////////////////////////
  Data data;
  data.model = model;
  data.palette = ccp;
  data.ret = &ret;
  clang_visitChildren(clang_getTranslationUnitCursor(unit), &visitor, (void *)&data);


  /////////////////////////////
  // PART 2 -- Split regions //
  /////////////////////////////
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