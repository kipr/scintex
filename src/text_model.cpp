#include <scintex/text_model.hpp>
#include <scintex/cursor.hpp>

#include <QDebug>

using namespace scintex;

TextModel::TextModel()
  : _revision(0)
{
}

TextModel::~TextModel()
{
}

TextOperation TextModel::create(const QString &str, const quint32 i)
{
  if(str.isEmpty()) return TextOperation();
  const TextOperation op = _create(str, i);
  ++_revision;
  const qint32 newline = indexOf('\n', Region(i + str.size(), size()));
  const Region update(i, newline < 0 ? size() : newline);
  Q_EMIT updated(update);
  Q_EMIT sizeChanged(Region(i, i + str.size()), Region(i, i));
  return op;
}

TextOperation TextModel::update(const QString &str, const quint32 i)
{
  if(str.isEmpty()) return TextOperation();
  const TextOperation op = _update(str, i);
  ++_revision;
  Q_EMIT updated(Region(i, i + str.size()));
  return op;
}

TextOperation TextModel::remove(const Region &region)
{
  const TextOperation op = _remove(region);
  ++_revision;
  const qint32 newline = indexOf('\n', Region(region.start(), size()));
  Q_EMIT updated(Region(region.start(), newline < 0 ? size() : newline));
  Q_EMIT sizeChanged(Region(region.start(), region.start()), region);
  return op;
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

QList<TextOperation> TextModel::removePattern(const QRegExp &regex)
{
  const Region before = fullRegion();
  const QString text = read(before);
  
  QList<Region> regions;
  int i = 0;
  while((i = regex.indexIn(text, i)) >= 0) {
    regions.prepend(Region(i, i + regex.matchedLength()));
    i += regex.matchedLength();
  }
  
  QList<TextOperation> operations;
  Q_FOREACH(const Region &r, regions) operations.append(_remove(r));
  const Region after = fullRegion();
  
  Q_EMIT update(read(after), 0);
  Q_EMIT sizeChanged(after, before);
  
  return operations;
}

quint32 TextModel::revision() const
{
  return _revision;
}