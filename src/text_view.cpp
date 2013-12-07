#include <scintex/text_view.hpp>
#include <scintex/text_model.hpp>
#include <scintex/basic_style_palette.hpp>
#include <scintex/input_controller.hpp>
#include <scintex/margin_view.hpp>
#include <scintex/cursor.hpp>
#include <scintex/syntax_highlighter.hpp>

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
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
  private:
    TextView *const _tv;
  };
}

static QDebug &operator <<(QDebug &stream, const Region &r)
{
  stream << "(" << r.start() << "to" << r.end() << ")";
  return stream;
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
  StylePalette *const cp = v->_stylePalette;
  const QMargins &tm = v->_textMargins;
  
  QPainter p(this);
  const quint32 lineHeight = v->fontMetrics().height();
  p.fillRect(event->rect(), cp->style("background", Style(Qt::white)).color());
  const QRect target = event->rect().translated(left + tm.left(),
    top + tm.top());
  const qreal dpr = v->_backing.devicePixelRatio();
  
  QRect source = event->rect().translated(-x(), -y());
  source.translate(source.x() * (dpr - 1.0),
    source.y() * (dpr - 1.0));
  source.setWidth(source.width() * dpr);
  source.setHeight(source.height() * dpr);
  
  p.drawPixmap(target, v->_backing, source);
  p.setPen(QPen(cp->style("text/cursor", Style(Qt::black)).color(), 2));
  
  Q_FOREACH(Cursor *const cursor, v->_cursors) {
    const quint32 yOff = cursor->row() * lineHeight + top + tm.top();
    const quint32 i = mo->offset(cursor->row());
    const QString line = mo->read(Region(i, mo->index(cursor)));
    const quint32 xOff = v->fontMetrics().width(line) + left + tm.left();
    if(-x() > xOff || xOff + x() < left + tm.left()) continue;
    p.drawLine(xOff, yOff, xOff, yOff + lineHeight);
  }
  
  Q_FOREACH(const Region &r, v->_selection) {
    quint32 begin = r.start();
    
    const QString org = mo->read(Region(r.start() - mo->charsPreceding('\n',
      r.start()), r.start()));
    quint32 line = mo->occurencesOf('\n', r.left());
    quint32 xOff = fontMetrics().width(org);
    while(begin < r.end()) {
      qint32 end = mo->indexOf('\n', Region(begin, r.end()));
      const quint32 actualEnd = end < 0 ? r.end() : end + 1;
      const QString chunk = mo->read(Region(begin, actualEnd));
      const quint32 chunkWidth = fontMetrics().width(chunk);
      const QColor c = cp->style("selection", Style(QColor(127, 127, 127, 127))).color();
      p.fillRect(xOff, line * lineHeight, chunkWidth, lineHeight, c);
      xOff += chunkWidth;
      if(end >= 0) {
        xOff = 0;
        ++line;
      }
      begin = actualEnd;
    }
  }
  
  p.translate(v->geometry().x(), v->geometry().y());
  if(mvis) {
    QList<MarginView *> *const mv = v->_marginViews;
    {
      quint32 offset = 0;
      Q_FOREACH(MarginView *const marginView, mv[TextView::Left]) {
        marginView->render(&p, QPoint(offset, 0), QRegion(), 0);
        offset += marginView->width();
      }
    }
    {
      quint32 offset = -x();
      Q_FOREACH(MarginView *const marginView, mv[TextView::Right]) {
        offset += marginView->width();
        marginView->render(&p, QPoint(v->width() - x() - offset, geometry().y()), QRegion(), 0);
      }
    }
  }
  
  p.resetTransform();
  p.setPen(QColor(240, 240, 240));
  if(left > 0)   p.drawLine(-x() + left, -y(), -x() + left, -y() + height());
  if(right > 0)  p.drawLine(width() + x() + right, -y(), x() + width() + right, -y() + height());
  if(top > 0)    p.drawLine(-x(), -y() + top, width(), -y() + top);
  if(bottom > 0) p.drawLine(-x(), -y() + bottom, -x() + width(), -y() + bottom);
}

void TextViewport::mouseMoveEvent(QMouseEvent *event)
{
  if(!_tv->_inputController) {
    event->ignore();
    return;
  }
  
  _tv->_inputController->mouseMoved(event);
}

void TextViewport::mousePressEvent(QMouseEvent *event)
{
  if(!_tv->_inputController) {
    event->ignore();
    return;
  }
  
  _tv->_inputController->mousePressed(event);
}

void TextViewport::mouseReleaseEvent(QMouseEvent *event)
{
  if(!_tv->_inputController) {
    event->ignore();
    return;
  }
  
  _tv->_inputController->mouseReleased(event);
}

TextView::TextView(QWidget *const parent)
  : QScrollArea(parent)
  , _stylePalette(0)
  , _model(0)
  , _marginsVisible(true)
  , _cursors()
  , _inputController(0)
  , _selectionStart(-1)
  , _selectionEnd(-1)
{
  setAttribute(Qt::WA_OpaquePaintEvent);
  setStylePalette(new BasicStylePalette);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFocusPolicy(Qt::StrongFocus);
  setWidget(new TextViewport(this));
  setWidgetResizable(true);
  updateDimensions();
}

TextView::~TextView()
{
  // delete _colorPalette;
  // delete _inputController;
  // qDeleteAll(_cursors);
  for(quint8 i = 0; i < 4; ++i){
    // qDeleteAll(_marginViews[(TextView::Location)i]);
  }
  // takeWidget();
}

void TextView::setModel(TextModel *const model)
{
  _model = model;
  connect(_model, &TextModel::updated, this, &TextView::updateDimensions);
  connect(_model, &TextModel::updated, this, &TextView::rehighlight);
  connect(_model, &TextModel::updated, this, &TextView::invalidateRegion);
  updateDimensions();
  invalidateRegion(model ? model->fullRegion() : Region());
  setStyleRegions(QList<StyleRegion>());
  setSelection(Region());
}

TextModel *TextView::model() const
{
  return _model;
}

void TextView::setStylePalette(StylePalette *const stylePalette)
{
  Q_ASSERT(stylePalette != 0);
  delete _stylePalette;
  _stylePalette = stylePalette;
  connect(_stylePalette, SIGNAL(roleChanged(QString, QColor)), SLOT(rehighlight()));
}

StylePalette *TextView::stylePalette() const
{
  return _stylePalette;
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
  marginView->setStylePalette(_stylePalette);
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
  
  const qreal dpr = devicePixelRatio();
  if(_backing.width() == textWidth * dpr
    && _backing.height() == textHeight * dpr) {
    return;
  }
  
  _backing = QPixmap(textWidth * dpr, textHeight * dpr);

  _backing.setDevicePixelRatio(dpr);
  dirty(QRect(0, 0, _backing.width(), _backing.height()));
}

void TextView::rehighlight()
{
  if(_syntaxHighlighter && _model) {
    _styleRegions = _syntaxHighlighter->stylize(_model, _stylePalette);
    qSort(_styleRegions);
  }
  updateContiguousStyleRegions();
  dirty(QRect(0, 0, _backing.width(), _backing.height()));
}

void TextView::invalidateRegion(const Region &region)
{
  const quint32 lineHeight = fontMetrics().height();
  
  const quint32 lines = _model->occurencesOf('\n', region.left());
  const quint32 invalidSize = _model->occurencesOf('\n', region) + 2;
  const quint32 yOff = lines * lineHeight;
  const quint32 height = invalidSize * lineHeight;

  dirty(QRect(0, yOff, width(), height));
}

void TextView::changeEvent(QEvent *event)
{
  if(!_model) return;
  if(event->type() == QEvent::FontChange) {
    invalidateRegion(_model->fullRegion());
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
  
  _inputController->keyPressed(event);
}

void TextView::keyReleaseEvent(QKeyEvent *event)
{
  if(!_inputController) {
    event->ignore();
    return;
  }
  
  _inputController->keyReleased(event);
}

void TextView::fitMarginViews()
{
  for(quint8 i = 0; i < 4; ++i) {
    Q_FOREACH(MarginView *const marginView, _marginViews[(TextView::Location)i])
      marginView->fit();
  }
  updateDimensions();
}

quint32 TextView::marginSize(const Location location) const
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
  if(!_model) return 1;
  const quint32 lines = _model->occurencesOf('\n', _model->fullRegion()) + 1;
  return lines * fontMetrics().height();
}

quint32 TextView::computeWidth() const
{
  if(!_model) return 1;
  Region largestRun;
  quint32 last = 0;
  for(;;) {
    const qint32 next = _model->indexOf('\n', Region(last, _model->size()));
    if(next < 0) break;
    if(largestRun.size() < next - last) {
      largestRun.setStart(last);
      largestRun.setEnd(next);
    }
    last = next + 1;
  }
  return fontMetrics().width(_model->read(largestRun));
}

void TextView::setMarginsVisible(const bool marginsVisible)
{
  _marginsVisible = marginsVisible;
  updateDimensions();
  widget()->update();
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
  connect(cursor, &Cursor::positionChanged, this, &TextView::update);
  _cursors.append(cursor);
}

void TextView::removeCursor(Cursor *const cursor)
{
  _cursors.removeAll(cursor);
  cursor->disconnect(this);
}

void TextView::setCursors(const QList<Cursor *> &cursors)
{
  _cursors = cursors;
}

const QList<Cursor *> &TextView::cursors() const
{
  return _cursors;
}

void TextView::setSyntaxHighlighter(SyntaxHighlighter *const syntaxHighlighter)
{
  _syntaxHighlighter = syntaxHighlighter;
  setStylePalette(_syntaxHighlighter->createStylePalette());
}

SyntaxHighlighter *TextView::syntaxHighlighter() const
{
  return _syntaxHighlighter;
}

void TextView::updateContiguousStyleRegions()
{
  _contiguousStyleRegions.clear();
  if(!_model) return;
  
  const QColor textBase = _stylePalette
    ? _stylePalette->style("text/base", Style(Qt::black)).color()
    : QColor(Qt::black);
  
  if(_styleRegions.isEmpty()) {
    _contiguousStyleRegions.append(StyleRegion(_model->fullRegion(), textBase));
    return;
  }
  
  QList<StyleRegion>::const_iterator it = _styleRegions.begin();
  quint32 step = 0;
  for(; it != _styleRegions.end(); ++it) {
    const StyleRegion &c = *it;
    if(step > c.region().start()) continue;
    if(step != c.region().start()) {
      const Region add(step, c.region().start());
      _contiguousStyleRegions.append(StyleRegion(add, textBase));
    }
    _contiguousStyleRegions.append(c);
    step = c.region().end();
  }
  if(step < _model->size()) {
    const Region add(step, _model->size());
    _contiguousStyleRegions.append(StyleRegion(add, textBase));
  }
  
  // TODO: Make less naive
  dirty(QRect(0, 0, _backing.width(), _backing.height()));
}

void TextView::setStyleRegions(const QList<StyleRegion> &styleRegions)
{
  _styleRegions = styleRegions;
  qSort(_styleRegions);
  rehighlight();
}

const QList<StyleRegion> &TextView::styleRegions() const
{
  return _styleRegions;
}

const QList<StyleRegion> &TextView::contiguousStyleRegions() const
{
  return _contiguousStyleRegions;
}

const QPixmap &TextView::backing() const
{
  return _backing;
}

QPoint TextView::contentPosition() const
{
  return widget()->pos();
}

void TextView::setSelection(const Region &region)
{
  if(region.size() == 0) setSelection(QList<Region>());
  else setSelection(QList<Region>() << region);
}

void TextView::setSelection(const QList<Region> &selection)
{
  _selection = selection;
  update();
}

const QList<Region> &TextView::selection() const
{
  return _selection;
}

quint32 TextView::indexUnder(const QPoint &p) const
{
  const qint32 actualY = p.y() - marginSize(TextView::Top) - _textMargins.top();
  const qint32 actualX = p.x() - marginSize(TextView::Left) - _textMargins.left();
  
  if(actualY < 0) return 0;
  
  const quint32 lineHeight = fontMetrics().height();
  const quint32 line = actualY / lineHeight;
  
  quint32 lines = 0;
  Region lineRegion(0, _model->size());
  for(; lines < line; ++lines) {
    const qint32 next = _model->indexOf('\n', lineRegion);
    if(next < 0) break;
    lineRegion.setStart(next + 1);
  }
  
  const qint32 next = _model->indexOf('\n', lineRegion);
  lineRegion.setEnd(next < 0 ? lineRegion.end() : next);
  
  if(actualX < 0) return lineRegion.start();
  
  const QString chunk = _model->read(lineRegion);
  
  quint32 best = 0;
  quint32 dist = 0xFFFFFFFF;
  const QFontMetrics &m = fontMetrics();
  for(quint32 i = 0; i <= chunk.size(); ++i) {
    const quint32 thisDist = qAbs(m.width(chunk, i) - actualX);
    if(thisDist < dist) {
      dist = thisDist;
      best = i;
    } else break;
  }
  
  return lineRegion.start() + best;
}

void TextView::dirty(const QRect &region)
{
  _dirty.append(region);
  renderOn(&_backing);
  widget()->update();
}

void TextView::renderOn(QPaintDevice *device)
{
  if(_backing.isNull() || _backing.width() == 0 || _backing.height() == 0) return;
  QPainter p(device);
  if(!_model) return;
  
  const quint32 lineHeight = fontMetrics().height();
  const quint32 modelSize = _model->size();
  
  p.setFont(font());
  
  Q_FOREACH(const QRect &r, _dirty) {
    p.fillRect(r, _stylePalette->style("background", Style(Qt::white)).color());
    
    const quint32 line1 = r.y() / lineHeight;
    const quint32 line2 = line1 + r.height() / lineHeight + 1;

    // Fast forward to first line index
    quint32 curLine = 0;
    Region searchRegion(0, modelSize);
    for(; curLine < line1; ++curLine) {
      const qint32 p = _model->indexOf('\n', searchRegion) + 1;
      if(p > 0) searchRegion.setStart(p);
      else break;
    }
    const quint32 first = searchRegion.start();

    // Now find last line index
    quint32 last = first;
    for(; curLine < line2 + 1; ++curLine) {
      const qint32 p = _model->indexOf('\n', Region(last, modelSize)) + 1;
      if(p > 0) last = p;
      else {
        last = modelSize;
        break;
      }
    }
    
    const Region dirty(first, last);

    QList<StyleRegion>::const_iterator it = _contiguousStyleRegions.begin();
  
    // Fast forward to dirty lines
    QList<StyleRegion>::const_iterator lit = it;
    for(; it != _contiguousStyleRegions.end(); ++it) {
      if(dirty < (*it).region()) break;
      lit = it;
    }
    
    quint32 xOff = 0;
    for(; lit != _contiguousStyleRegions.end(); ++lit) {
      const StyleRegion &s = *lit;
      const Region &r = s.region();
      const Style &st = s.style();
      if(r.start() > last) break;
      p.setPen(st.color());
      QFont f = p.font();
      f.setBold(st.isBold());
      f.setItalic(st.isItalic());
      p.setFont(f);
      quint32 begin = qMax(r.start(), dirty.start());
      // qint32 newline = _model->charsPreceding('\n', begin);
      // const QString beginChunk = _model->read(Region(begin - newline, begin));
      // qDebug() << "newline" << newline << "Begin chunk" << beginChunk;
      // xOff += fontMetrics().width(beginChunk);
      quint32 line = _model->occurencesOf('\n', Region(0, begin));
      const quint32 bound = qMin(r.end(), dirty.end());
      while(begin < bound) {
        qint32 end = _model->indexOf('\n', Region(begin, r.end()));
        const quint32 actualEnd = qMin(end < 0 ? r.end() : end + 1, r.end());
        const Region cr(begin, actualEnd);
        const QString chunk = _model->read(cr);
        
        if(!chunk.trimmed().isEmpty()) {
          p.drawText(xOff, line * lineHeight, widget()->width() - xOff, lineHeight,
            0, chunk);
        }
        xOff += fontMetrics().width(chunk);
        if(end >= 0) {
          xOff = 0;
          line += _model->occurencesOf('\n', cr);
        }
        begin = actualEnd;
      }
    }
  }
  _dirty.clear();
}

void TextView::update()
{
  widget()->update();
  QScrollArea::update();
}