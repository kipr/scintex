#include <scintex/basic_color_palette.hpp>

using namespace scintex;

BasicColorPalette::BasicColorPalette()
{
  addRole("background");
  addRole("text/base");
  addRole("text/cursor");
}

QStringList BasicColorPalette::roles() const
{
  return _roles;
}

bool BasicColorPalette::setColor(const QString &role, const QColor &color)
{
  if(!_roles.contains(role)) return false;
  if(_colors[role] == color) return true;
  _colors[role] = color;
  Q_EMIT roleChanged(role, color);
  return true;
}

QColor BasicColorPalette::color(const QString &role) const
{
  return color(role, Qt::black);
}

QColor BasicColorPalette::color(const QString &role, const QColor &alt) const
{
  Q_ASSERT(_roles.contains(role));
  return _colors.value(role, alt);
}

bool BasicColorPalette::addRole(const QString &role)
{
  if(_roles.contains(role)) return false;
  _roles.append(role);
  return true;
}

bool BasicColorPalette::removeRole(const QString &role)
{
  if(_roles.contains(role)) return false;
  _roles.removeAll(role);
  return true;
}