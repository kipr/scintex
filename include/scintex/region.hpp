#ifndef _SCINTEX_REGION_HPP_
#define _SCINTEX_REGION_HPP_

#include <QtGlobal>
#include <QList>

namespace scintex
{
  class Region
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
    
    static QList<Region> coalesce(QList<Region> regions);
    
    bool operator <(const Region &rhs) const;
    
  private:
    quint32 _start;
    quint32 _end;
  };
}

#endif