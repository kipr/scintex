#include <scintex/basic_style_palette.hpp>

#include <QDebug>

using namespace scintex;

BasicStylePalette::BasicStylePalette()
{
  addRole("background");
  addRole("text/base");
  addRole("text/cursor");
}

QStringList BasicStylePalette::roles() const
{
  return _roles;
}

bool BasicStylePalette::setStyle(const QString &role, const Style &style)
{
  if(_styles[role] == style) return true;
  _styles[role] = style;
  Q_EMIT roleChanged(role, style);
  return true;
}

Style BasicStylePalette::style(const QString &role) const
{
  return style(role, Style(Qt::black));
}

Style BasicStylePalette::style(const QString &role, const Style &alt) const
{
  QString path(role);
  do {
    if(_styles.contains(path)) break;
    const int in = path.lastIndexOf('/');
    path = path.left(in < 0 ? 0 : in);
  } while(!path.isEmpty());
  
  return _styles.value(path, alt);
}

bool BasicStylePalette::addRole(const QString &role)
{
  if(_roles.contains(role)) return false;
  _roles.append(role);
  return true;
}

bool BasicStylePalette::removeRole(const QString &role)
{
  if(_roles.contains(role)) return false;
  _roles.removeAll(role);
  return true;
}