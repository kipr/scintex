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
  QList<Region> compounds;
};

static CXChildVisitResult visitor(CXCursor cur, CXCursor parent, CXClientData data)
{
  Data *const d = reinterpret_cast<Data *>(data);
  if(cur.kind != CXCursor_CompoundStmt) return CXChildVisit_Recurse;
  Region r = cursorRegion(cur);
  
  // Chop off '{' and '}'
  if(r.size() >= 2) {
    r.setStart(r.start() + 1);
    r.setEnd(r.end() - 1);
  }
  
  qDebug() << d->model->read(r);
  
  d->compounds << r;
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
  
  
  const quint32 lines = model->lines();
  for(quint32 i = 0; i < lines; ++i) {
    quint32 offset = model->offset(i);
    const Region r(offset, offset + 1);
    quint32 depth = regionOverlap(r, data.compounds);
    if(depth > 0) {
      for(; r.end() < model->size();) {
        const QString next = model->read(r);
        if(next.trimmed().isEmpty()) model->remove(r);
        else break;
      }
    }
    
    model->create(QString().fill(' ', depth * 2), offset);
  }
  
  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);
}