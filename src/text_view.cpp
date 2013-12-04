#include <scintex/text_view.hpp>
#include <scintex/text_model.hpp>
#include <scintex/basic_color_palette.hpp>
#include <scintex/input_controller.hpp>
#include <scintex/margin_view.hpp>
#include <scintex/cursor.hpp>

#include <QPainter>
#include <QEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QDebug>

using namespace scintex;

namespace scintex
{
  class TextViewport : public QWidget
  {
  public:
    TextViewport(TextView *const parent = 0);
  protected:
    void paintEvent(QPaintEvent *event);
  private:
    TextView *const _tv;
  };
}

TextViewport::TextViewport(TextView *const parent)
  : QWidget(parent)
  , _tv(parent)
{
  Q_ASSERT(_tv);
}

void TextViewport::paintEvent(QPaintEvent *event)
{
  TextView *const v = _tv;
  bool mvis = v->_marginsVisible;
  
  qint32 left = 0;
  qint32 right = 0;
  qint32 top = 0;
  qint32 bottom = 0;
  
  if(mvis) {
    v->fitMarginViews();
    // Compute margin sizes
    left = v->marginSize(TextView::Left);
    right = v->marginSize(TextView::Right);
    top = v->marginSize(TextView::Top);
    bottom = v->marginSize(TextView::Bottom);
  }
  
  TextModel *const mo = v->_model;
  ColorPalette *const cp = v->_colorPalette;
  const QMargins &tm = v->_textMargins;
  
  QPainter p(this);
  const quint32 lineHeight = v->fontMetrics().height();
  p.fillRect(event->rect(), cp->color("background", Qt::white));
  const QRect target = event->rect().translated(left + tm.left(),
    top + tm.top());
  const QRect source = event->rect().translated(-x(), -y());
  p.drawPixmap(target, v->_backing, source);
  p.setPen(QPen(cp->color("text/cursor", Qt::black), 2));
  
  Q_FOREACH(Cursor *const cursor, v->_cursors) {
    const quint32 yOff = cursor->row() * lineHeight + top + tm.top();
    const quint32 i = mo->offset(cursor->row());
    const QString line = mo->read(i, mo->index(cursor));
    const quint32 xOff = v->fontMetrics().width(line) + left + tm.left();
    if(-x() > xOff || xOff + x() < left + tm.left()) continue;
    p.drawLine(xOff, yOff, xOff, yOff + lineHeight);
  }
  
  p.setPen(Qt::lightGray);
  if(left > 0)   p.drawLine(-x() + left, -y(), -x() + left, -y() + height());
  if(right > 0)  p.drawLine(width() + x() + right, -y(), x() + width() + right, -y() + height());
  if(top > 0)    p.drawLine(-x(), -y() + top, width(), -y() + top);
  if(bottom > 0) p.drawLine(-x(), -y() + bottom, -x() + width(), -y() + bottom);
  
  p.end();
  
  if(mvis) {
    QList<MarginView *> *const mv = v->_marginViews;
    {
      quint32 offset = 0;
      Q_FOREACH(MarginView *const marginView, mv[TextView::Left]) {
        marginView->render(this, QPoint(-x() + offset, -y()), QRegion(), 0);
        offset += marginView->width();
      }
    }
    {
      quint32 offset = -x();
      Q_FOREACH(MarginView *const marginView, mv[TextView::Right]) {
        offset += marginView->width();
        // qDebug() << logicalDpiX() << physicalDpiX();
        marginView->render(this, QPoint(v->width() - x() - offset, geometry().y()), QRegion(), 0);
      }
    }
  }
}

TextView::TextView(QWidget *const parent)
  : QScrollArea(parent)
  , _colorPalette(0)
  , _model(0)
  , _marginsVisible(true)
  , _cursors()
  , _inputController(0)
  , _selectionStart(-1)
  , _selectionEnd(-1)
{
  setAttribute(Qt::WA_OpaquePaintEvent);
  setColorPalette(new BasicColorPalette);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFocusPolicy(Qt::StrongFocus);
  setWidget(new TextViewport(this));
  setWidgetResizable(true);
}

TextView::~TextView()
{
  // delete _colorPalette;
  // delete _inputController;
  // qDeleteAll(_cursors);
  for(quint8 i = 0; i < 4; ++i){
    // qDeleteAll(_marginViews[(TextView::Location)i]);
  }
  // delete takeWidget();
}

void TextView::setModel(TextModel *const model)
{
  _model = model;
  connect(_model, SIGNAL(updated(quint32, quint32)), SLOT(updateDimensions()));
  connect(_model, SIGNAL(updated(quint32, quint32)),
    SLOT(invalidateRegion(quint32, quint32)));
  updateDimensions();
  invalidateRegion(0, model->size());
  setColorRegions(QList<ColorRegion>());
}

TextModel *TextView::model() const
{
  return _model;
}

void TextView::setColorPalette(ColorPalette *const colorPalette)
{
  Q_ASSERT(colorPalette != 0);
  delete _colorPalette;
  _colorPalette = colorPalette;
  connect(_colorPalette, SIGNAL(roleChanged(QString, QColor)), SLOT(rehighlight()));
}

ColorPalette *TextView::colorPalette() const
{
  return _colorPalette;
}

void TextView::setMarginViews(const Location location, const QList<MarginView *> marginViews)
{
  _marginViews[location] = marginViews;
  update();
}

const QList<MarginView *> TextView::marginViews(const Location location) const
{
  return _marginViews[location];
}

void TextView::addMarginView(const Location location, MarginView *const marginView)
{
  marginView->setTextView(this);
  marginView->setColorPalette(_colorPalette);
  _marginViews[location].append(marginView);

  update();
}

void TextView::removeMarginView(MarginView *const marginView)
{
  for(quint8 i = 0; i < 4; ++i) {
    _marginViews[i].removeAll(marginView);
  }
  update();
}

void TextView::setTextMargins(const QMargins &textMargins)
{
  _textMargins = textMargins;
}

const QMargins &TextView::textMargins() const
{
  return _textMargins;
}

void TextView::updateDimensions()
{
  // Compute margin sizes
  qint32 left = 0;
  qint32 right = 0;
  qint32 top = 0;
  qint32 bottom = 0;
  
  if(_marginsVisible) {
    left = marginSize(TextView::Left);
    right = marginSize(TextView::Right);
    top = marginSize(TextView::Top);
    bottom = marginSize(TextView::Bottom);
  }
  
  const quint32 textWidth = computeWidth();
  const quint32 textHeight = computeHeight();
  const quint32 width = textWidth + left + right;
  const quint32 height = textHeight + top + bottom;

  widget()->setMinimumSize(textWidth + left, textHeight + top);
  
  if(_backing.width() == textWidth && _backing.height() == textHeight) {
    return;
  }
  _backing = QPixmap(textWidth, textHeight);
  dirty(QRect(0, 0, textWidth, textHeight));
}

void TextView::rehighlight()
{
  update();
}

void TextView::invalidateRegion(const quint32 i, const quint32 j)
{
  const quint32 lineHeight = fontMetrics().height();
  
  const quint32 lines = _model->occurencesOf('\n', 0, i);
  const quint32 invalidSize = _model->occurencesOf('\n', i, j) + 2;
  const quint32 yOff = lines * lineHeight;
  const quint32 height = invalidSize * lineHeight;

  dirty(QRect(0, yOff, width(), height));
}

void TextView::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::FontChange) {
    invalidateRegion(0, _model->size());
    updateDimensions();
  }
}

void TextView::paintEvent(QPaintEvent *event)
{
  QScrollArea::paintEvent(event);
}

void TextView::keyPressEvent(QKeyEvent *event)
{
  if(!_inputController) {
    event->ignore();
    return;
  }
  
  _inputController->pressed(event);
}

void TextView::keyReleaseEvent(QKeyEvent *event)
{
  if(!_inputController) {
    event->ignore();
    return;
  }
  
  _inputController->released(event);
}

void TextView::mouseMoveEvent(QMouseEvent *event)
{
  
}

void TextView::mousePressEvent(QMouseEvent *event)
{
  
}

void TextView::mouseReleaseEvent(QMouseEvent *event)
{
  
}

void TextView::fitMarginViews()
{
  for(quint8 i = 0; i < 4; ++i) {
    Q_FOREACH(MarginView *const marginView, _marginViews[(TextView::Location)i])
      marginView->fit();
  }
  updateDimensions();
}

quint32 TextView::marginSize(const Location location)
{
  if(_marginViews[location].isEmpty()) return 0;
  MarginView *const view = _marginViews[location].back();
  switch(location) {
    case TextView::Bottom: return view->y();
    case TextView::Top: return view->y() + view->height();
    case TextView::Left: return view->x() + view->width();
    case TextView::Right: return view->x();
  }
  return 0;
}

quint32 TextView::computeHeight() const
{
  const quint32 lines = _model->occurencesOf('\n', 0, _model->size()) + 1;
  return lines * fontMetrics().height();
}

quint32 TextView::computeWidth() const
{
  quint32 largestRunStart = 0;
  quint32 largestRunEnd = 0;
  quint32 last = 0;
  for(;;) {
    const qint32 next = _model->indexOf('\n', last, _model->size());
    if(next < 0) break;
    if(largestRunEnd - largestRunStart < next - last) {
      largestRunStart = last;
      largestRunEnd = next;
    }
    last = next + 1;
  }
  return fontMetrics().width(_model->read(largestRunStart, largestRunEnd));
}

void TextView::setMarginsVisible(const bool marginsVisible)
{
  _marginsVisible = marginsVisible;
  updateDimensions();
  update();
}

bool TextView::marginsVisible() const
{
  return _marginsVisible;
}

void TextView::setInputController(InputController *const inputController)
{
  if(_inputController == inputController) return;
  delete _inputController;
  _inputController = inputController;
  if(!_inputController) return;
  _inputController->setParent(this);
  _inputController->setTextView(this);
}

InputController *TextView::inputController() const
{
  return _inputController;
}

void TextView::addCursor(Cursor *const cursor)
{
  connect(cursor, SIGNAL(positionChanged(quint32, quint32)), SLOT(update()));
  _cursors.append(cursor);
}

void TextView::removeCursor(Cursor *const cursor)
{
  _cursors.removeAll(cursor);
}

void TextView::setCursors(const QList<Cursor *> &cursors)
{
  _cursors = cursors;
}

const QList<Cursor *> &TextView::cursors() const
{
  return _cursors;
}

void TextView::setColorRegions(const QList<ColorRegion> &colorRegions)
{
  _colorRegions = colorRegions;
  qSort(_colorRegions);
  
  _contiguousColorRegions.clear();
  if(!_model) return;
  
  const QColor textBase = _colorPalette
    ? _colorPalette->color("text/base", Qt::black)
    : QColor(Qt::black);
  
  if(_colorRegions.isEmpty()) {
    _contiguousColorRegions.append(ColorRegion(0, _model->size(), textBase));
    return;
  }
  
  QList<ColorRegion>::const_iterator it = _colorRegions.begin();
  quint32 step = 0;
  for(; it != _colorRegions.end(); ++it) {
    const ColorRegion &c = *it;
    if(step != c.start()) {
      _contiguousColorRegions.append(ColorRegion(step, c.start(), textBase));
    }
    _contiguousColorRegions.append(c);
    step = c.end();
  }
  if(step < _model->size()) {
    _contiguousColorRegions.append(ColorRegion(step, _model->size(), textBase));
  }
  
  // TODO: Make less naive
  dirty(QRect(0, 0, _backing.width(), _backing.height()));
  update();
}

const QList<ColorRegion> &TextView::colorRegions() const
{
  return _colorRegions;
}

const QList<ColorRegion> &TextView::contiguousColorRegions() const
{
  return _contiguousColorRegions;
}

const QPixmap &TextView::backing() const
{
  return _backing;
}

const QRect &TextView::internalGeometry() const
{
  return widget()->geometry();
}

void TextView::dirty(const QRect &region)
{
  _dirty.append(region);
  renderOn(&_backing);
  update();
}

void TextView::renderOn(QPaintDevice *device)
{
  QPainter p(device);
  if(!_model) return;
  
  const quint32 lineHeight = fontMetrics().height();
  const quint32 modelSize = _model->size();
  
  p.setFont(font());
  
  Q_FOREACH(const QRect &r, _dirty) {
    p.fillRect(r, _colorPalette->color("background", Qt::white));
    
    const quint32 line1 = r.y() / lineHeight;
    const quint32 line2 = line1 + r.height() / lineHeight + 1;

    // Fast forward to first line index
    quint32 curLine = 0;
    quint32 first = 0;
    for(; curLine < line1; ++curLine) {
      const qint32 p = _model->indexOf('\n', first, modelSize) + 1;
      if(p > 0) first = p;
      else break;
    }

    // Now find last line index
    quint32 last = first;
    for(; curLine < line2 + 1; ++curLine) {
      const qint32 p = _model->indexOf('\n', last, modelSize) + 1;
      if(p > 0) last = p;
      else {
        last = modelSize;
        break;
      }
    }

    QList<ColorRegion>::const_iterator it = _contiguousColorRegions.begin();
  
    // Fast forward to dirty lines
    QList<ColorRegion>::const_iterator lit = it;
    for(; it != _contiguousColorRegions.end(); ++it) {
      if((*it).start() > first) {
        break;
      }
      lit = it;
    }
  
    quint32 xOff = 0;
    quint32 line = line1;
    for(; lit != _contiguousColorRegions.end(); ++lit) {
      const ColorRegion &c = *lit;
      if(c.start() > last) break;
      p.setPen(c.color());
      quint32 begin = qMax(c.start(), first);
      const quint32 bound = qMin(c.end(), last);
      while(begin < bound) {
        qint32 end = _model->indexOf('\n', begin, c.end());
        const quint32 actualEnd = end < 0 ? c.end() : end + 1;
        const QString chunk = _model->read(begin, actualEnd);
        p.drawText(xOff, line * lineHeight, widget()->width() - xOff, lineHeight,
          0, chunk);
        xOff += fontMetrics().width(chunk);
        if(end >= 0) {
          xOff = 0;
          ++line;
        }
        begin = actualEnd;
      }
    }
  }
  _dirty.clear();
}