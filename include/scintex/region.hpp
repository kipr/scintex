#ifndef _SCINTEX_REGION_HPP_
#define _SCINTEX_REGION_HPP_

#include <QtGlobal>
#include <QList>
#include "compat.hpp"

namespace scintex
{
  class DLL_EXPORT Region
  {
  public:
    Region();
    Region(const quint32 start, const quint32 end);
    
    quint32 start() const;
    void setStart(const quint32 start);
    
    quint32 end() const;
    void setEnd(const quint32 end);
    
    quint32 size() const;
    
    Region left() const;
    Region right(const quint32 end) const;
    
    Region shift(const qint32 offset) const;
    
    bool overlaps(const Region &r) const;
    bool contains(const Region &r) const;
    
    QList<Region> intersect(const Region &region) const;
    
    static QList<Region> coalesce(QList<Region> regions);
    static QList<Region> intersect(const QList<Region> &a, const QList<Region> &b);
    
    bool operator <(const Region &rhs) const;
    bool operator ==(const Region &rhs) const;
    
  private:
    quint32 _start;
    quint32 _end;
  };
}

#endif