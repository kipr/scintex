#include <scintex/line_numbers_view.hpp>
#include <scintex/text_view.hpp>
#include <scintex/text_model.hpp>

#include <QPainter>
#include <QEvent>
#include <QPaintEvent>
#include <QDebug>

using namespace scintex;

LineNumbersView::LineNumbersView()
  : _lines(0)
{
  resize(10, 10);
}

void LineNumbersView::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::FontChange) {
    fit();
  }
}

void LineNumbersView::paintEvent(QPaintEvent *event)
{
  if(!isEngaged()) return;
  
  const TextView *const view = MarginView::textView();
  QPainter p(this);
  p.drawPixmap(0, view->textMargins().top() + view->internalGeometry().y() * 2, _backing);
}

void LineNumbersView::engaged()
{
}

void LineNumbersView::fit()
{
  if(!isEngaged()) return;
  const TextModel *const model = textView()->model();
  const quint32 lines = model->occurencesOf('\n', 0, model->size()) + 1;
  fit(lines);
}

void LineNumbersView::fit(const quint32 lines)
{
  const quint32 actual = qMax(lines, (quint32)100);
  
  if(_lines == lines) return;
  const TextView *const view = MarginView::textView();
  const ColorPalette *const colorPalette = MarginView::colorPalette();

  const quint32 lineHeight = view->fontMetrics().height();
  
  _backing = QPixmap(fontMetrics().width(QString::number(actual)) + 10,
    textView()->backing().height());
  resize(_backing.width(), _backing.height());
  
  QPainter p(&_backing);
  p.fillRect(0, 0, _backing.width(), _backing.height(), Qt::white);
  p.setPen(Qt::lightGray);
  
  for(quint32 i = 0; i < lines; ++i) {
    p.drawText(0, i * lineHeight,
      _backing.width(), lineHeight, Qt::AlignHCenter | Qt::AlignVCenter,
      QString::number(i + 1));
  }
  _lines = lines;
}