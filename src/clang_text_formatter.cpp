#include <scintex/clang_text_formatter.hpp>
#include <scintex/text_model.hpp>

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <clang-c/Index.h>

#include "clang_shared.hpp"

using namespace scintex;

struct Data
{
  CXTranslationUnit unit;
  TextModel *model;
};

static CXChildVisitResult visitor(CXCursor cur, CXCursor parent, CXClientData data)
{
  Data *const d = reinterpret_cast<Data *>(data);
  
  quint32 depth = 0;
  CXCursor c = cur;
  CXCursor root = clang_getTranslationUnitCursor(d->unit);
  while(!clang_equalCursors(c, root)) {
    c = clang_getCursorLexicalParent(c);
    ++depth;
  }
  
  qDebug() << d->model->read(cursorRegion(cur)) << depth << cur.kind;
  return CXChildVisit_Recurse;
}

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
  
  const QString contents = model->read(model->fullRegion());
  
  file.write(contents.toUtf8());
  file.close();
  
  const QByteArray pathUtf = path.toUtf8();
  const char *const argv[1] = { pathUtf.data() };
  
  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit unit = clang_parseTranslationUnit(index, 0, 
   argv, 1, 0, 0, CXTranslationUnit_None);
  
  CXCursor cursor = clang_getTranslationUnitCursor(unit);
  
  Data data;
  data.unit = unit;
  data.model = model;
  clang_visitChildren(cursor, &visitor, reinterpret_cast<void *>(&data));
  
  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);
}