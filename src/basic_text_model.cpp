#include <scintex/basic_text_model.hpp>
#include <scintex/cursor.hpp>

#include <QDebug>

using namespace scintex;



BasicTextModel::BasicTextModel(const QString &text)
  : _backing(text)
{
}

BasicTextModel::~BasicTextModel()
{
}

const QString &BasicTextModel::backing() const
{
  return _backing;
}

void BasicTextModel::setBacking(const QString &backing)
{
  _backing = backing;
  Q_EMIT updated(fullRegion());
}

TextOperation BasicTextModel::_create(const QString &str, const quint32 i)
{
  Q_ASSERT(i < _backing.size());
  _backing.insert(i, str);
  return TextOperation(Region(i, i), QString(), Region(i, i + str.size()), str);
}

QString BasicTextModel::read(const Region &region) const
{
  Q_ASSERT(region.start() < _backing.size());
  Q_ASSERT(region.end() < _backing.size());
  return _backing.mid(region.start(), region.size());
}

TextOperation BasicTextModel::_update(const QString &str, const quint32 i)
{
  Q_ASSERT(i + str.size() < _backing.size());
  const Region update(i, i + str.size());
  const QString before = _backing.mid(update.start(), update.size());
  _backing.replace(i, str.size(), str);
  return TextOperation(update, before, update, str);
}

TextOperation BasicTextModel::_remove(const Region &region)
{
  Q_ASSERT(region.start() < _backing.size());
  Q_ASSERT(region.end() < _backing.size());
  const QString before = _backing.mid(region.start(), region.size());
  _backing.remove(region.start(), region.size());
  return TextOperation(region, before, Region(region.start(), region.start()), QString());
}

quint32 BasicTextModel::size() const
{
  return _backing.size();
}

quint32 BasicTextModel::occurencesOf(const QChar c, const Region &region) const
{
  Q_ASSERT(region.start() < _backing.size());
  Q_ASSERT(region.end() < _backing.size());
  return _backing.midRef(region.start(), region.size()).count(c);
}

qint32 BasicTextModel::indexOf(const QChar c, const Region &region) const
{
  Q_ASSERT(region.start() < _backing.size());
  Q_ASSERT(region.end() < _backing.size());
  
  // TODO: This unnecessarily scans past j
  const qint32 res = _backing.indexOf(c, region.start());
  if(res > region.end()) return -1;
  return res;
}

quint32 BasicTextModel::charsUntil(const QChar c, const quint32 i) const
{
  Q_ASSERT(i < _backing.size());
  qint32 j = _backing.indexOf(c, i);
  if(j < 0) j = _backing.size();
  return j - i;
}

quint32 BasicTextModel::charsPreceding(const QChar c, const quint32 i) const
{
  Q_ASSERT(i < _backing.size());
  qint32 j = _backing.lastIndexOf(c, i);
  if(j < 0) j = 0;
  return i - j;
}

static QDebug &operator <<(QDebug &stream, const Region &r)
{
  stream << "(" << r.start() << "to" << r.end() << ")";
  return stream;
}

