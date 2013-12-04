#include <scintex/scroll_overview_view.hpp>
#include <scintex/text_view.hpp>

#include <QPainter>
#include <QPaintEvent>

using namespace scintex;

ScrollOverviewView::ScrollOverviewView()
{
  
}

void ScrollOverviewView::fit()
{
  TextView *const view = MarginView::textView();
  
  resize(view->width() / 10, view->height() / 3);
}

void ScrollOverviewView::paintEvent(QPaintEvent *event)
{
  if(!isEngaged()) return;
  
  TextView *const view = MarginView::textView();
  
  QPainter pa(this);
  pa.drawPixmap(0, 0, view->backing().scaled(size(),
    Qt::KeepAspectRatio, Qt::SmoothTransformation));
}