#include <scintex/line_numbers_view.hpp>
#include <scintex/text_view.hpp>
#include <scintex/text_model.hpp>

#include <QPainter>
#include <QEvent>
#include <QPaintEvent>
#include <QDebug>

using namespace scintex;

LineNumbersView::LineNumbersView()
{
  resize(10, 10);
}

void LineNumbersView::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::FontChange) {
    // fit();
  }
}

void LineNumbersView::paintEvent(QPaintEvent *event)
{
  if(!isEngaged()) return;
  const TextView *const view = MarginView::textView();
  const TextModel *const model = view->model();
  
  const ColorPalette *const colorPalette = MarginView::colorPalette();

  const quint32 lineHeight = view->fontMetrics().height();
  
  QPainter p(this);
  p.fillRect(0, 0, width(), height(), Qt::white);
  p.setPen(Qt::lightGray);
  
  const QRect r = event->rect();
  
  quint32 line1 = (r.y() - view->y()) / lineHeight;
  quint32 line2 = line1 + r.height() / lineHeight + 1;
  
  for(quint32 i = 0; i <= (line2 - line1); ++i) {
    p.drawText(0, i * lineHeight + view->textMargins().top() + -(-view->y() % lineHeight),
      width(), lineHeight, Qt::AlignHCenter | Qt::AlignVCenter,
      QString::number(i + 1 + line1));
  }
}

void LineNumbersView::engaged()
{
}

void LineNumbersView::fit()
{
  if(!isEngaged()) return;
  const TextView *const view = MarginView::textView();
  const TextModel *const model = view->model();
  const quint32 lines = model->occurencesOf('\n', 0, model->size()) + 1;
  fit(lines);
}

void LineNumbersView::fit(const quint32 lines)
{
  const quint32 actual = qMax(lines, (quint32)100);
  resize(fontMetrics().width(QString::number(actual)) + 10, textView()->height());
}