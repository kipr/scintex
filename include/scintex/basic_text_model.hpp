#ifndef _SCINTEX_BASIC_TEXT_MODEL_HPP_
#define _SCINTEX_BASIC_TEXT_MODEL_HPP_

#include "text_model.hpp"
#include "compat.hpp"

namespace scintex
{
  class DLL_EXPORT BasicTextModel : public TextModel
  {
  Q_OBJECT
  public:
    BasicTextModel(const QString &text = QString());
    ~BasicTextModel();
    
    const QString &backing() const;
    void setBacking(const QString &backing);
    
    virtual QString read(const Region &region) const;
    virtual quint32 size() const;
    
    virtual quint32 occurencesOf(const QChar c, const Region &region) const;
    virtual qint32 indexOf(const QChar c, const Region &region) const;
    virtual quint32 charsUntil(const QChar c, const quint32 i) const;
    virtual quint32 charsPreceding(const QChar c, const quint32 i) const;
    
  protected:
    virtual TextOperation _create(const QString &str, const quint32 i);
    virtual TextOperation _update(const QString &str, const quint32 i);
    virtual TextOperation _remove(const Region &region);
    
  private:
    QString _backing;
  };
}

#endif