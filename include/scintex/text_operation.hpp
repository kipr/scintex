#ifndef _SCINTEX_TEXT_OPERATION_HPP_
#define _SCINTEX_TEXT_OPERATION_HPP_

#include "region.hpp"
#include <QString>

namespace scintex
{
  class TextModel;
  
  class TextOperation
  {
  public:
    TextOperation();
    TextOperation(const Region &ar, const QString &as, const Region &br, const QString &bs);
    
    bool isValid() const;
    
    void setRegionA(const Region &ar);
    const Region &regionA() const;
    
    void setStringA(const QString &as);
    const QString &stringA() const;
    
    void setRegionB(const Region &br);
    const Region &regionB() const;
    
    void setStringB(const QString &bs);
    const QString &stringB() const;
    
    void apply(TextModel *const model) const;
    void unapply(TextModel *const model) const;
    
    bool operator ==(const TextOperation &rhs) const;
    
  private:
    bool _valid;
    Region _ar;
    QString _as;
    Region _br;
    QString _bs;
  };
}

#endif