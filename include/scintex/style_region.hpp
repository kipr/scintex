#ifndef _SCINTEX_COLOR_REGION_HPP_
#define _SCINTEX_COLOR_REGION_HPP_

#include <QtGlobal>

#include "region.hpp"
#include "style.hpp"

namespace scintex
{
  class StyleRegion
  {
  public:
    StyleRegion(const Region &region, const Style &style);
    
    const Region &region() const;
    void setRegion(const Region &region);
    
    const Style &style() const;
    void setStyle(const Style &style);
    
    bool operator <(const StyleRegion &rhs) const;
    
  private:
    Region _region;
    Style _style;
  };
}

#endif