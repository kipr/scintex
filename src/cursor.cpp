#include <scintex/cursor.hpp>

using namespace scintex;

Cursor::Cursor(QObject *const parent)
  : QObject(parent)
  , _row(0)
  , _column(0)
{
}

Cursor::Cursor(const quint32 row, const quint32 column, QObject *const parent)
  : QObject(parent)
  , _row(row)
  , _column(column)
{
}

void Cursor::setRow(const quint32 row)
{
  if(_row == row) return;
  _row = row;
  Q_EMIT positionChanged(_row, _column);
}

void Cursor::setColumn(const quint32 column)
{
  if(_column == column) return;
  _column = column;
  Q_EMIT positionChanged(_row, _column);
}

quint32 Cursor::row() const
{
  return _row;
}

quint32 Cursor::column() const
{
  return _column;
}

void Cursor::right(const quint32 i)
{
  _column += i;
  Q_EMIT positionChanged(_column, _row);
}

void Cursor::left(const quint32 i)
{
  if(_column < i) _column = 0;
  else _column -= i;
  Q_EMIT positionChanged(_column, _row);
}

void Cursor::up(const quint32 i)
{
  if(_row < i) _row = 0;
  else _row -= i;
  Q_EMIT positionChanged(_column, _row);
}

void Cursor::down(const quint32 i)
{
  _row += i;
  Q_EMIT positionChanged(_column, _row);
}