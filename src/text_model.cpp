#include <scintex/text_model.hpp>
#include <scintex/cursor.hpp>

#include <QDebug>

using namespace scintex;

TextModel::~TextModel()
{
}

quint32 TextModel::offset(const quint32 line) const
{
  qint32 last = 0;
  for(quint32 i = 0;
    last >= 0 && line > i;
    last = indexOf('\n', Region(last, size())) + 1, ++i);
  return last;
}

quint32 TextModel::line(const quint32 i) const
{
  return occurencesOf('\n', Region(0, i));
}

quint32 TextModel::index(const Cursor *const cursor) const
{
  const quint32 off = offset(cursor->row());
  quint32 c = qMin(charsUntil('\n', off), cursor->column());
  return c + off;
}

void TextModel::placeCursor(const quint32 index, Cursor *const cursor) const
{
  const quint32 line = occurencesOf('\n', Region(0, index));
  cursor->setRow(line);
  cursor->setColumn(index - offset(line));
}

quint32 TextModel::lines() const
{
  return occurencesOf('\n', fullRegion());
}

Region TextModel::fullRegion() const
{
  return Region(0, size());
}