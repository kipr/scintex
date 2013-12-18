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
  // This is arbitrary.
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

  const QRect target(0, view->textMargins().top()
    + view->contentPosition().y(), width(), height());
  const QRect source(0, 0, _backing.width(), _backing.height());
  p.drawPixmap(target, _backing, source);
}

void LineNumbersView::engaged()
{
}

void LineNumbersView::fit()
{
  if(!isEngaged()) return;
  const TextModel *const model = textView()->model();
  const quint32 lines = model->lines() + 1;
  fit(lines);
}

void LineNumbersView::fit(const quint32 lines)
{
  const quint32 actual = qMax(lines, (quint32)100);
  
  if(_lines == lines) return;
  const TextView *const view = MarginView::textView();
  const StylePalette *const stylePalette = MarginView::stylePalette();

  const quint32 lineHeight = view->fontMetrics().height();
  
  const qreal dpr = devicePixelRatio();
  _backing = QPixmap((fontMetrics().width(QString::number(actual))) * 2 * dpr,
    textView()->backing().height());
  _backing.setDevicePixelRatio(dpr);
  resize(_backing.width() / dpr, _backing.height() / dpr);
  
  QPainter p(&_backing);
  p.fillRect(0, 0, width(), height(), Qt::white);
  p.setPen(Qt::lightGray);
  
  for(quint32 i = 0; i < lines; ++i) {
    p.drawText(0, i * lineHeight,
      width(), lineHeight, Qt::AlignHCenter | Qt::AlignVCenter,
      QString::number(i + 1));
  }
  _lines = lines;
}