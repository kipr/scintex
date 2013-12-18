#include <scintex/clang_text_formatter.hpp>
#include <scintex/text_model.hpp>

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <clang-c/Index.h>

using namespace scintex;

void ClangTextFormatter::format(TextModel *const model) const
{
  // model->removePattern(QRegExp("\\n(\\t| )*"));
  
  const QString path = QDir::temp().absoluteFilePath("format.cpp");
  QFile file(path);
  
  if(!file.open(QIODevice::WriteOnly)) {
    QFileInfo info(file);
    qWarning() << "Failed to open" << info.absoluteFilePath() << "for writing";
    return;
  }
  
  QString contents = model->read(model->fullRegion());
  
  file.write(contents.toUtf8());
  
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
  
  for (int i = 0; i < numTokens; ++i) {
    CXCursor &cur = cursors[i];
    if(cur.kind != CXCursor_CompoundStmt) continue;
    
    CXSourceRange range = clang_getCursorExtent(cur);
    
    CXSourceLocation start = clang_getRangeStart(range);
    unsigned startOffset = 0;
    clang_getExpansionLocation(start, 0, 0, 0, &startOffset);
    
    CXSourceLocation end = clang_getRangeEnd(range);
    unsigned endOffset = 0;
    clang_getExpansionLocation(end, 0, 0, 0, &endOffset);
    model->create("\n", startOffset);
    
    qDebug() << cur.kind << model->read(Region(startOffset, endOffset));
  }

  delete[] cursors;
  clang_disposeTokens(unit, tokens, numTokens);
  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);
}