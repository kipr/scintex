#ifndef _SCINTEX_BASIC_STYLE_PALETTE_
#define _SCINTEX_BASIC_STYLE_PALETTE_

#include "style_palette.hpp"

#include <QStyle>
#include <QMap>

namespace scintex
{
  class BasicStylePalette : public StylePalette
  {
  Q_OBJECT
  public:
    BasicStylePalette();
    
    QStringList roles() const;
    
    bool setStyle(const QString &role, const Style &Style);
    Style style(const QString &role) const;
    Style style(const QString &role, const Style &alt) const;
    
  protected:
    bool addRole(const QString &role);
    bool removeRole(const QString &role);
    
  private:
    QStringList _roles;
    QMap<QString, Style> _styles;
  };
}

#endif