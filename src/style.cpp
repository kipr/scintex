#include <scintex/style.hpp>

using namespace scintex;

Style::Style()
  : _color()
  , _bold(false)
  , _italic(false)
{
}

Style::Style(const QColor &color, const bool bold, const bool italic)
  : _color(color)
  , _bold(bold)
  , _italic(italic)
{
}

void Style::setColor(const QColor &color)
{
  _color = color;
}

const QColor &Style::color() const
{
  return _color;
}

void Style::setBold(const bool bold)
{
  _bold = bold;
}

bool Style::isBold() const
{
  return _bold;
}

void Style::setItalic(const bool italic)
{
  _italic = italic;
}

bool Style::isItalic() const
{
  return _italic;
}

bool Style::operator ==(const Style &rhs) const
{
  return _color == rhs._color && _bold == rhs._bold && _italic && rhs._italic;
}
