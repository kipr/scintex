#ifndef _SCINTEX_TEXT_MODEL_HPP_
#define _SCINTEX_TEXT_MODEL_HPP_

#include <QObject>
#include <QString>

#include "region.hpp"

namespace scintex
{
  class Cursor;
  
  class TextModel : public QObject
  {
  Q_OBJECT
  public:
    virtual ~TextModel();
    // CRUD
    virtual void create(const QString &str, const quint32 i) = 0;
    virtual QString read(const Region &region) const = 0;
    virtual void update(const QString &str, const quint32 i) = 0;
    virtual void remove(const Region &region) = 0;
    virtual quint32 size() const = 0;
    
    virtual quint32 occurencesOf(const QChar c, const Region &region) const = 0;
    virtual qint32 indexOf(const QChar c, const Region &region) const = 0;
    virtual quint32 charsUntil(const QChar c, const quint32 i) const = 0;
    virtual quint32 charsPreceding(const QChar c, const quint32 i) const = 0;
    
    virtual quint32 offset(const quint32 line) const;
    virtual quint32 line(const quint32 i) const;
    
    virtual quint32 index(const Cursor *const cursor) const;
    virtual void placeCursor(const quint32 index, Cursor *const cursor) const;
    
    quint32 lines() const;
    Region fullRegion() const;
    
  Q_SIGNALS:
    void updated(const Region &region);
  };
}

#endif