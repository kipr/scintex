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
  Q_EMIT updated(0, _backing.size());
}

void BasicTextModel::create(const QString &str, const quint32 i)
{
  Q_ASSERT(i < _backing.size());
  _backing.insert(i, str);
  Q_EMIT updated(i, _backing.size() - i);
}

QString BasicTextModel::read(const quint32 i, const quint32 j) const
{
  Q_ASSERT(i < _backing.size());
  Q_ASSERT(j < _backing.size());
  Q_ASSERT(j > i);
  return _backing.mid(i, j - i);
}

void BasicTextModel::update(const QString &str, const quint32 i)
{
  Q_ASSERT(i + str.size() < _backing.size());
  _backing.replace(i, str.size(), str);
  Q_EMIT updated(i, i + str.size());
}

void BasicTextModel::remove(const quint32 i, const quint32 j)
{
  Q_ASSERT(i < _backing.size());
  Q_ASSERT(j < _backing.size());
  Q_ASSERT(j > i);
  _backing.remove(i, j - i);
  Q_EMIT updated(i, _backing.size());
}

quint32 BasicTextModel::size() const
{
  return _backing.size();
}

quint32 BasicTextModel::occurencesOf(const QChar c, const quint32 i, const quint32 j) const
{
  Q_ASSERT(i < _backing.size());
  Q_ASSERT(j < _backing.size());
  Q_ASSERT(j > i);
  return _backing.midRef(i, j - i).count(c);
}

qint32 BasicTextModel::indexOf(const QChar c, const quint32 i, const quint32 j) const
{
  Q_ASSERT(i < _backing.size());
  Q_ASSERT(j < _backing.size());
  Q_ASSERT(j > i);
  
  // TODO: This unnecessarily scans past j
  const qint32 res = _backing.indexOf(c, i);
  if(res > j) return -1;
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

quint32 BasicTextModel::line(const quint32 i) const
{
  return occurencesOf('\n', 0, i);
}

quint32 BasicTextModel::offset(const quint32 line) const
{
  qint32 last = 0;
  for(quint32 i = 0;
    last >= 0 && line > i;
    last = indexOf('\n', last, size()) + 1, ++i);
  return last;
}

quint32 BasicTextModel::index(const Cursor *const cursor) const
{
  const quint32 off = offset(cursor->row());
  quint32 c = qMin(charsUntil('\n', off), cursor->column());
  return c + off;
}