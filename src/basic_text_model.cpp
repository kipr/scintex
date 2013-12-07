#include <scintex/basic_text_model.hpp>
#include <scintex/cursor.hpp>

#include <QDebug>

using namespace scintex;

BasicTextModel::BasicTextModel(const QString &text)
  : _backing(text)
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

void BasicTextModel::create(const QString &str, const quint32 i)
{
  Q_ASSERT(i < _backing.size());
  _backing.insert(i, str);
  Q_EMIT updated(Region(i, _backing.size() - i));
}

QString BasicTextModel::read(const Region &region) const
{
  Q_ASSERT(region.start() < _backing.size());
  Q_ASSERT(region.end() < _backing.size());
  return _backing.mid(region.start(), region.size());
}

void BasicTextModel::update(const QString &str, const quint32 i)
{
  Q_ASSERT(i + str.size() < _backing.size());
  _backing.replace(i, str.size(), str);
  Q_EMIT updated(Region(i, i + str.size()));
}

void BasicTextModel::remove(const Region &region)
{
  Q_ASSERT(region.start() < _backing.size());
  Q_ASSERT(region.end() < _backing.size());
  _backing.remove(region.start(), region.size());
  Q_EMIT updated(Region(region.start(), _backing.size()));
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

