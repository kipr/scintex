#ifndef _SCINTEX_CURSOR_HPP_
#define _SCINTEX_CURSOR_HPP_

#include <QObject>

namespace scintex
{
  class Cursor : public QObject
  {
  Q_OBJECT
  public:
    Cursor(QObject *const parent = 0);
    Cursor(const quint32 row, const quint32 column, QObject *const parent = 0);
    
    void setRow(const quint32 row);
    void setColumn(const quint32 column);
    
    quint32 row() const;
    quint32 column() const;
    
    void right(const quint32 i = 1);
    void left(const quint32 i = 1);
    void up(const quint32 i = 1);
    void down(const quint32 i = 1);
    
  Q_SIGNALS:
    void positionChanged(quint32 row, quint32 column);
    
  private:
    quint32 _row;
    quint32 _column;
  };
}

#endif