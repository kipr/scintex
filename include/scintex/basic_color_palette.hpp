#ifndef _SCINTEX_BASIC_COLOR_PALETTE_
#define _SCINTEX_BASIC_COLOR_PALETTE_

#include "color_palette.hpp"

#include <QColor>
#include <QMap>

namespace scintex
{
  class BasicColorPalette : public ColorPalette
  {
  Q_OBJECT
  public:
    BasicColorPalette();
    
    QStringList roles() const;
    
    bool setColor(const QString &role, const QColor &color);
    QColor color(const QString &role) const;
    QColor color(const QString &role, const QColor &alt) const;
    
  protected:
    bool addRole(const QString &role);
    bool removeRole(const QString &role);
    
  private:
    QStringList _roles;
    QMap<QString, QColor> _colors;
  };
}

#endif