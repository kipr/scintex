#ifndef _SCINTEX_STYLE_HPP_
#define _SCINTEX_STYLE_HPP_

#include <QColor>

namespace scintex
{
  class Style
  {
  public:
    Style();
    Style(const QColor &color, const bool bold = false, const bool italic = false);
    
    void setColor(const QColor &color);
    const QColor &color() const;
    
    void setBold(const bool bold);
    bool isBold() const;
    
    void setItalic(const bool italic);
    bool isItalic() const;
    
    bool operator ==(const Style &rhs) const;
    
  private:
    QColor _color;
    bool _bold;
    bool _italic;
  };
}

#endif