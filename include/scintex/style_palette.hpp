#ifndef _SCINTEX_STYLE_PALETTE_HPP_
#define _SCINTEX_STYLE_PALETTE_HPP_

#include <QObject>
#include <QString>
#include "style.hpp"
#include "compat.hpp"

namespace scintex
{
  class DLL_EXPORT StylePalette : public QObject
  {
  Q_OBJECT
  public:
    virtual ~StylePalette();
    
    virtual QStringList roles() const = 0;
    
    virtual bool setStyle(const QString &role, const Style &style) = 0;
    virtual Style style(const QString &role) const = 0;
    virtual Style style(const QString &role, const Style &alt) const = 0;
    
  Q_SIGNALS:
    void roleChanged(const QString &role, const Style &color);
  };
}

#endif