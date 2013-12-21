#include <scintex/text_view.hpp>
#include <scintex/text_model.hpp>
#include <scintex/basic_style_palette.hpp>
#include <scintex/input_controller.hpp>
#include <scintex/margin_view.hpp>
#include <scintex/cursor.hpp>
#include <scintex/syntax_highlighter.hpp>
#include <scintex/text_operation_history_manager.hpp>

#include <QPainter>
#include <QEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QRunnable>
#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QWaitCondition>

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
  
  class HighlightWorker : public QRunnable, public QObject
  {
  public:
    HighlightWorker(TextView *const textView);
    virtual void run();
    virtual void highlight();
    virtual void exit();
    
  private:
    QMutex _mut;
    TextView *const _textView;
    QWaitCondition _highlight;
    bool _exit;
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
  v->_drawMutex.lock();
  
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
  p.fillRect(event->rect(), cp->style("text/background", Style(Qt::white)).color());
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
    quint32 yOff = cursor->row() * lineHeight + top + tm.top() + y();
    // TODO: Make cursor obey margins
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
      const QColor c = cp->style("text/selection", Style(QColor(127, 127, 127, 127))).color();
      p.fillRect(xOff, line * lineHeight, chunkWidth, lineHeight, c);
      xOff += chunkWidth;
      if(end >= 0) {
        xOff = 0;
        ++line;
      }
      begin = actualEnd;
    }
  }
  
  p.translate(0, 0);
  if(mvis) {
    QList<MarginView *> *const mv = v->_marginViews;
    {
      quint32 offset = -x();
      qDebug() << "OFFSET" << offset << " X " << v->geometry().x();
      Q_FOREACH(MarginView *const marginView, mv[TextView::Left]) {
        marginView->setStylePalette(v->stylePalette());
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
  p.setPen(cp->style("text/margin_divider", Style(QColor(240, 240, 240))).color());
  if(left > 0)   p.drawLine(-x() + left, -y(), -x() + left, -y() + height());
  if(right > 0)  p.drawLine(width() + x() + right, -y(), x() + width() + right, -y() + height());
  if(top > 0)    p.drawLine(-x(), -y() + top, width(), -y() + top);
  if(bottom > 0) p.drawLine(-x(), -y() + bottom, -x() + width(), -y() + bottom);
  
  v->_drawMutex.unlock();
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

HighlightWorker::HighlightWorker(TextView *const textView)
  : _textView(textView)
  , _exit(false)
{
}

void HighlightWorker::run()
{
  for(;;) {
    _mut.lock();
    _highlight.wait(&_mut);
    qDebug() << "Highlight!!";
    if(_exit) {
      _mut.unlock();
      return;
    }
    SyntaxHighlighter *const sh = _textView->syntaxHighlighter();
    TextModel *const m = _textView->model();
    StylePalette *const s = _textView->stylePalette();
    if(!sh || !m || !s) continue;
    
    const quint32 rev = m->revision();
    QList<StyleRegion> styleRegions = sh->stylize(m->fullRegion());
    if(m->revision() == rev) {
      qSort(styleRegions);
      _textView->_drawMutex.lock();
      _textView->_styleRegions = styleRegions;
      _textView->_drawMutex.unlock();
      _textView->updateContiguousStyleRegions();
    }
    _mut.unlock();
  }
}

void HighlightWorker::highlight()
{
  _highlight.wakeAll();
}

void HighlightWorker::exit()
{
  _exit = true;
  _highlight.wakeAll();
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
  , _syntaxHighlighter(0)
  , _highlightWorker(new HighlightWorker(this))
  , _historyManager(new TextOperationHistoryManager())
{
  setAttribute(Qt::WA_OpaquePaintEvent);
  setStylePalette(new BasicStylePalette);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFocusPolicy(Qt::StrongFocus);
  setWidget(new TextViewport(this));
  setWidgetResizable(true);
  updateDimensions();
  _highlightWorker->setAutoDelete(true);
  _highlightTimer.setSingleShot(true);
  QThreadPool::globalInstance()->start(_highlightWorker);
  connect(&_highlightTimer, &QTimer::timeout, this, &TextView::_rehighlight);
}

TextView::~TextView()
{
  delete _historyManager;
  // delete _colorPalette;
  // delete _inputController;
  // qDeleteAll(_cursors);
  for(quint8 i = 0; i < 4; ++i){
    // qDeleteAll(_marginViews[(TextView::Location)i]);
  }
  // takeWidget();
  _highlightWorker->exit();
}

void TextView::setModel(TextModel *const model)
{
  _drawMutex.lock();
  _model = model;
  if(_syntaxHighlighter) _syntaxHighlighter->setTextModel(model);
  connect(_model, &TextModel::updated, this, &TextView::updateDimensions);
  connect(_model, &TextModel::updated, this, &TextView::rehighlight);
  connect(_model, &TextModel::updated, this, &TextView::invalidateRegion);
  connect(_model, &TextModel::sizeChanged, this, &TextView::sizeChanged);
  _drawMutex.unlock();
  updateDimensions();
  invalidateRegion(model ? model->fullRegion() : Region());
  setStyleRegions(QList<StyleRegion>());
  setSelection(Region());
  updateContiguousStyleRegions();
}

TextModel *TextView::model() const
{
  return _model;
}

void TextView::setStylePalette(StylePalette *const stylePalette)
{
  _drawMutex.lock();
  Q_ASSERT(stylePalette != 0);
  delete _stylePalette;
  _stylePalette = stylePalette;
  if(_syntaxHighlighter) _syntaxHighlighter->setStylePalette(_stylePalette);
  connect(_stylePalette, SIGNAL(roleChanged(QString, QColor)), SLOT(rehighlight()));
  _drawMutex.unlock();
}

StylePalette *TextView::stylePalette() const
{
  return _stylePalette;
}

void TextView::setMarginViews(const Location location, const QList<MarginView *> marginViews)
{
  _drawMutex.lock();
  _marginViews[location] = marginViews;
  _drawMutex.unlock();
  update();
}

const QList<MarginView *> TextView::marginViews(const Location location) const
{
  return _marginViews[location];
}

void TextView::addMarginView(const Location location, MarginView *const marginView)
{
  _drawMutex.lock();
  marginView->setTextView(this);
  marginView->setStylePalette(_stylePalette);
  _marginViews[location].append(marginView);
  _drawMutex.unlock();
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
  _drawMutex.lock();
  _textMargins = textMargins;
  _drawMutex.unlock();
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
  dirty(QRect());
}

void TextView::rehighlight()
{
  _highlightTimer.start(200);
}

void TextView::invalidateRegion(const Region &region)
{
  const quint32 lineHeight = fontMetrics().height();
  
  const quint32 lines = _model->occurencesOf('\n', region.left());
  const quint32 invalidSize = _model->occurencesOf('\n', region) + 2;
  const quint32 yOff = lines * lineHeight;
  const quint32 height = invalidSize * lineHeight;
  
  // qDebug() << "Invalidate" << region;

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

void TextView::sizeChanged(const Region &newRegion, const Region &oldRegion)
{
  _drawMutex.lock();
  const qint32 shift = (qint32)newRegion.size() - (qint32)oldRegion.size();
  QList<StyleRegion>::iterator it = _styleRegions.begin();
  for(; it != _styleRegions.end(); ++it) {
    StyleRegion &sr = *it;
    Region r = sr.region();
    if(r.contains(oldRegion)) {
      if(shift < 0 && -shift > r.end()) r.setEnd(0);
      else r.setEnd(r.end() + shift);
    } else if(r.end() >= oldRegion.end()) {
      r = r.shift(shift);
    }
    sr.setRegion(r);
  }
  _drawMutex.unlock();
  updateContiguousStyleRegions();
  update();
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
    if(last >= _model->size()) break;
    qint32 next = _model->indexOf('\n', Region(last, _model->size()));
    if(next < 0) next = _model->size();
    if(largestRun.size() < next - last) {
      largestRun.setStart(last);
      largestRun.setEnd(next);
    }
    last = next + 1;
  }
  return qMax(fontMetrics().width(_model->read(largestRun)), 1);
}

void TextView::setMarginsVisible(const bool marginsVisible)
{
  _drawMutex.lock();
  _marginsVisible = marginsVisible;
  _drawMutex.unlock();
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
  _drawMutex.lock();
  delete _inputController;
  _inputController = inputController;
  _drawMutex.unlock();
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
  _drawMutex.lock();
  connect(cursor, &Cursor::positionChanged, this, &TextView::update);
  _cursors.append(cursor);
  _drawMutex.unlock();
}

void TextView::removeCursor(Cursor *const cursor)
{
  _drawMutex.lock();
  _cursors.removeAll(cursor);
  _historyManager->removeCursor(cursor);
  cursor->disconnect(this);
  _drawMutex.unlock();
}

void TextView::setCursors(const QList<Cursor *> &cursors)
{
  _drawMutex.lock();
  _cursors = cursors;
  _drawMutex.unlock();
}

const QList<Cursor *> &TextView::cursors() const
{
  return _cursors;
}

void TextView::setSyntaxHighlighter(SyntaxHighlighter *const syntaxHighlighter)
{
  _drawMutex.lock();
  if(_syntaxHighlighter) {
    _syntaxHighlighter->setTextModel(0);
    _syntaxHighlighter->setStylePalette(0);
  }
  _syntaxHighlighter = syntaxHighlighter;
  if(_syntaxHighlighter) {
    _syntaxHighlighter->setTextModel(_model);
    _syntaxHighlighter->setStylePalette(_stylePalette);
  }
  _drawMutex.unlock();
  setStylePalette(_syntaxHighlighter->createStylePalette());
}

SyntaxHighlighter *TextView::syntaxHighlighter() const
{
  return _syntaxHighlighter;
}

void TextView::updateContiguousStyleRegions()
{
  _drawMutex.lock();
  _contiguousStyleRegions.clear();
  _drawMutex.unlock();
  
  if(!_model) return;
  
  const Style textBase = _stylePalette
    ? _stylePalette->style("text/base", Style(Qt::black))
    : Style(QColor(Qt::black));
  
  _drawMutex.lock();
  if(_styleRegions.isEmpty()) {
    _contiguousStyleRegions.append(StyleRegion(_model->fullRegion(), textBase));
  } else {
    QList<StyleRegion>::const_iterator it = _styleRegions.begin();
    quint32 step = 0;
    for(; it != _styleRegions.end(); ++it) {
      const StyleRegion c = *it;

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
  }
  _drawMutex.unlock();
  
  // TODO: Make less naive
  dirty(QRect());
}

void TextView::setStyleRegions(const QList<StyleRegion> &styleRegions)
{
  _drawMutex.lock();
  _styleRegions = styleRegions;
  qSort(_styleRegions);
  _drawMutex.unlock();
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
  _drawMutex.lock();
  _selection = selection;
  _drawMutex.unlock();
  update();
}

const QList<Region> &TextView::selection() const
{
  return _selection;
}

void TextView::setHistoryManager(TextOperationHistoryManager *const historyManager)
{
  _historyManager = historyManager;
}

TextOperationHistoryManager *TextView::historyManager() const
{
  return _historyManager;
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
  if(region.isNull()) _dirty.append(QRect(0, 0, _backing.width(), _backing.height()));
  else _dirty.append(region);
  renderOn(&_backing);
  widget()->update();
}

void TextView::renderOn(QPaintDevice *device)
{
  if(_backing.isNull() || _backing.width() == 0 || _backing.height() == 0) return;
  if(!_model) return;
  
  _drawMutex.lock();
  QPainter p(device);
  
  const quint32 lineHeight = fontMetrics().height();
  const quint32 modelSize = _model->size();
  
  p.setFont(font());
  const Style bg = _stylePalette->style("text/background", Style(Qt::white));
  
  Q_FOREACH(const QRect &r, _dirty) {
    p.fillRect(r, bg.color());
    
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

    QList<Region> other;
    other << _model->fullRegion();
    
    Q_FOREACH(const StyleRegion &styleRegion, _styleRegions) {
      if(!dirty.overlaps(styleRegion.region())) continue;
      QList<Region>::iterator it = other.begin();
      for(; it != other.end(); ++it) {
        QList<Region> ret = it->intersect(styleRegion.region());
        it = other.erase(it);
        if(ret.size() == 1) {
          if(ret[0].size() > 0) it = other.insert(it, ret[0]);
        } else if(ret.size() == 3) {
          if(ret[0].size() > 0) it = other.insert(it, ret[0]);
          if(ret[2].size() > 0) it = other.insert(it, ret[2]);
        }
      }
      
      drawStyleRegion(p, styleRegion, dirty);
    }
    
    // Draw all other non-styled regions
    const Style s = _stylePalette->style("text/base", Style(Qt::black));
    Q_FOREACH(const Region &o, other) drawStyleRegion(p, StyleRegion(o, s), dirty);
  }
  _dirty.clear();
  
  _drawMutex.unlock();
}

void TextView::drawStyleRegion(QPainter &p, const StyleRegion &sr, const Region &dirty) const
{
  const quint32 lineHeight = fontMetrics().height();
  const quint32 modelSize = _model->size();
  const Region &r = sr.region();
  const Style &st = sr.style();
  
  p.save();
  
  quint32 xOff = 0;
  quint32 begin = qMax(r.start(), dirty.start());
  const quint32 bound = qMin(r.end(), dirty.end());
  
  p.setPen(st.color());
  
  QFont f = p.font();
  f.setBold(st.isBold());
  f.setItalic(st.isItalic());
  p.setFont(f);
  
  const quint32 preludeBegin = begin - _model->charsPreceding('\n', begin) + 1;
  const Region prelude(preludeBegin, begin);
  const QString preludeChunk = _model->read(prelude);
  
  xOff = fontMetrics().width(preludeChunk);
  quint32 line = _model->line(begin);
  while(begin < bound) {
    qint32 end = _model->indexOf('\n', Region(begin, r.end()));
    const quint32 actualEnd = qMin(end < 0 ? r.end() : end + 1, r.end());
    
    const Region chunkRegion(begin, actualEnd);
    const QString chunk = _model->read(chunkRegion);
    
    if(!chunk.trimmed().isEmpty()) {
      QRect bounding;
      p.drawText(xOff, line * lineHeight, widget()->width() - xOff, lineHeight,
        0, chunk, &bounding);
      if(st.underline()) {
        p.save();
        p.setPen(st.underlineColor());
        p.drawLine(bounding.bottomLeft(), bounding.bottomRight());
        p.restore();
      }
    }
    xOff += fontMetrics().width(chunk);
    if(end >= 0) {
      xOff = 0;
      line += _model->occurencesOf('\n', chunkRegion);
    }
    begin = actualEnd;
  }
  p.restore();
}

void TextView::update()
{
  widget()->update();
  QScrollArea::update();
}

void TextView::_rehighlight()
{
  _highlightWorker->highlight();
}
