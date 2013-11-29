#ifndef _SCINTEX_COLOR_PALETTE_HPP_
#define _SCINTEX_COLOR_PALETTE_HPP_

#include <QObject>
#include <QString>
#include <QColor>

namespace scintex
{
  class ColorPalette : public QObject
  {
  Q_OBJECT
  public:
    virtual ~ColorPalette();
    
    virtual QStringList roles() const = 0;
    
    virtual bool setColor(const QString &role, const QColor &color) = 0;
    virtual QColor color(const QString &role) const = 0;
    virtual QColor color(const QString &role, const QColor &alt) const = 0;
    
  Q_SIGNALS:
    void roleChanged(const QString &role, const QColor &color);
  };
}

#endif