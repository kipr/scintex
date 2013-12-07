#include <scintex/style_region.hpp>

using namespace scintex;

StyleRegion::StyleRegion(const Region &region, const Style &style)
  : _region(region)
  , _style(style)
{
}

const Region &StyleRegion::region() const
{
  return _region;
}

void StyleRegion::setRegion(const Region &region)
{
  _region = region;
}

const Style &StyleRegion::style() const
{
  return _style;
}

void StyleRegion::setStyle(const Style &style)
{
  _style = style;
}

bool StyleRegion::operator <(const StyleRegion &rhs) const
{
  return _region < rhs._region;
}