#ifndef _SCINTEX_TEXT_MODEL_HPP_
#define _SCINTEX_TEXT_MODEL_HPP_

#include <QObject>
#include <QString>

#include "region.hpp"
#include "text_operation.hpp"

namespace scintex
{
  class Cursor;
  
  class TextModel : public QObject
  {
  Q_OBJECT
  public:
    TextModel();
    virtual ~TextModel();
    
    // CRUD
    TextOperation create(const QString &str, const quint32 i);
    virtual QString read(const Region &region) const = 0;
    TextOperation update(const QString &str, const quint32 i);
    TextOperation remove(const Region &region);
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
    
    QList<TextOperation> removePattern(const QRegExp &regex);
    
    quint32 revision() const;
    
  Q_SIGNALS:
    void updated(const Region &region);
    void sizeChanged(const Region &newRegion, const Region &oldRegion);
    
  protected:
    virtual TextOperation _create(const QString &str, const quint32 i) = 0;
    virtual TextOperation _update(const QString &str, const quint32 i) = 0;
    virtual TextOperation _remove(const Region &region) = 0;
    
  private:
    quint32 _revision;
  };
}

#endif