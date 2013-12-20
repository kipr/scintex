#ifndef _SCINTEX_STYLE_HPP_
#define _SCINTEX_STYLE_HPP_

#include <QColor>
#include "compat.hpp"

namespace scintex
{
  class DLL_EXPORT Style
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
    
    void setUnderlineColor(const QColor &underlineColor);
    const QColor &underlineColor() const;
    
    void setUnderline(const bool underline);
    bool underline() const;
    
    bool operator ==(const Style &rhs) const;
    
  private:
    QColor _color;
    bool _bold;
    bool _italic;
    QColor _underlineColor;
    bool _underline;
  };
}

#endif