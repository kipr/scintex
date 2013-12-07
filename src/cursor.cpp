#include <scintex/cursor.hpp>

#include <QDebug>

using namespace scintex;

Cursor::Cursor(const bool trackMouse, QObject *const parent)
  : QObject(parent)
  , _row(0)
  , _column(0)
  , _trackMouse(trackMouse)
{
}

Cursor::Cursor(const quint32 row, const quint32 column, const bool trackMouse,
    QObject *const parent)
  : QObject(parent)
  , _row(row)
  , _column(column)
  , _trackMouse(trackMouse)
{
}

void Cursor::setTrackMouse(const bool trackMouse)
{
  _trackMouse = trackMouse;
}

bool Cursor::trackMouse() const
{
  return _trackMouse;
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