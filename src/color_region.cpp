#include <scintex/color_region.hpp>

using namespace scintex;

ColorRegion::ColorRegion(const quint32 start, const quint32 end, const QColor &color)
  : _start(start)
  , _end(end)
  , _color(color)
{
}

quint32 ColorRegion::start() const
{
  return _start;
}

void ColorRegion::setStart(const quint32 start)
{
  Q_ASSERT(_end > start);
  _start = start;
}

quint32 ColorRegion::end() const
{
  return _end;
}

void ColorRegion::setEnd(const quint32 end)
{
  Q_ASSERT(end >= _start);
  _end = end;
}

quint32 ColorRegion::size() const
{
  return _end - _start;
}

const QColor &ColorRegion::color() const
{
  return _color;
}

void ColorRegion::setColor(const QColor &color)
{
  _color = color;
}

bool ColorRegion::operator <(const ColorRegion &rhs) const
{
  return _start < rhs._start;
}