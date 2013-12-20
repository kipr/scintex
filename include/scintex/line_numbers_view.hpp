#ifndef _SCINTEX_LINE_NUMBERS_VIEW_HPP_
#define _SCINTEX_LINE_NUMBERS_VIEW_HPP_

#include "margin_view.hpp"
#include "compat.hpp"

#include <QPixmap>

namespace scintex
{
  class DLL_EXPORT LineNumbersView : public MarginView
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
    QPixmap _backing;
    quint32 _lines;
  };
}

#endif