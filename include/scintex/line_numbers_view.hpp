#ifndef _SCINTEX_LINE_NUMBERS_VIEW_HPP_
#define _SCINTEX_LINE_NUMBERS_VIEW_HPP_

#include "margin_view.hpp"

namespace scintex
{
  class LineNumbersView : public MarginView
  {
  public:
    LineNumbersView();
    virtual void fit();
    
  protected:
    void changeEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);
    virtual void engaged();
    
  private:
    void fit(const quint32 lines);
  };
}

#endif