#ifndef _SCINTEX_COLOR_REGION_HPP_
#define _SCINTEX_COLOR_REGION_HPP_

#include <QtGlobal>
#include <QColor>

namespace scintex
{
  class ColorRegion
  {
  public:
    ColorRegion(const quint32 start, const quint32 end, const QColor &color);
    
    quint32 start() const;
    void setStart(const quint32 start);
    
    quint32 end() const;
    void setEnd(const quint32 end);
    
    quint32 size() const;
    
    const QColor &color() const;
    void setColor(const QColor &color);
    
    bool operator <(const ColorRegion &rhs) const;
    
  private:
    quint32 _start;
    quint32 _end;
    QColor _color;
  };
}

#endif