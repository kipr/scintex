#ifndef _SCINTEX_SCROLL_OVERVIEW_VIEW_HPP_
#define _SCINTEX_SCROLL_OVERVIEW_VIEW_HPP_

#include "margin_view.hpp"
#include "compat.hpp"

namespace scintex
{
  class DLL_EXPORT ScrollOverviewView : public MarginView
  {
  public:
    ScrollOverviewView();
    
    virtual void fit();
  protected:
    void paintEvent(QPaintEvent *event);
  };
}

#endif