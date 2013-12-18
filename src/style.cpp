#include <scintex/style.hpp>

#include <QDebug>

using namespace scintex;

Style::Style()
  : _bold(false)
  , _italic(false)
  , _underline(false)
{
}

Style::Style(const QColor &color, const bool bold, const bool italic)
  : _color(color)
  , _bold(bold)
  , _italic(italic)
  , _underline(false)
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

void Style::setUnderlineColor(const QColor &underlineColor)
{
  _underlineColor = underlineColor;
}

const QColor &Style::underlineColor() const
{
  return _underlineColor;
}

void Style::setUnderline(const bool underline)
{
  _underline = underline;
}

bool Style::underline() const
{
  return _underline;
}

bool Style::operator ==(const Style &rhs) const
{
  return _color == rhs._color && _bold == rhs._bold && _italic == rhs._italic
    && _underline == rhs._underline && _underlineColor == rhs._underlineColor;
}
