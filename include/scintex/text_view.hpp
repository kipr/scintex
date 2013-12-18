#ifndef _SCINTEX_TEXT_VIEW_HPP_
#define _SCINTEX_TEXT_VIEW_HPP_

#include <QScrollArea>
#include <QPixmap>
#include <QTimer>
#include <QMutex>

#include "region.hpp"
#include "style_region.hpp"

namespace scintex
{
  class TextModel;
  class MarginView;
  class StylePalette;
  class InputController;
  class Cursor;
  class TextViewport;
  class SyntaxHighlighter;
  class HighlightWorker;
  class TextOperationHistoryManager;
  
  class TextView : public QScrollArea
  {
  Q_OBJECT
  public:
    friend class TextViewport;
    
    TextView(QWidget *const parent = 0);
    ~TextView();
    
    Q_PROPERTY(TextModel *model
      READ model
      WRITE setModel)
    void setModel(TextModel *const model);
    TextModel *model() const;
    
    Q_PROPERTY(StylePalette *stylePalette
      READ stylePalette
      WRITE setStylePalette)
    void setStylePalette(StylePalette *const stylePalette);
    StylePalette *stylePalette() const;
    
    enum Location {
      Top = 0,
      Bottom,
      Left,
      Right
    };
    
    void setMarginViews(const Location location, const QList<MarginView *> marginViews);
    const QList<MarginView *> marginViews(const Location location) const;
    void addMarginView(const Location location, MarginView *const marginView);
    void removeMarginView(MarginView *const marginView);
    
    void setMarginsVisible(const bool marginsVisible);
    bool marginsVisible() const;
    
    void setTextMargins(const QMargins &textMargins);
    const QMargins &textMargins() const;
    
    void setInputController(InputController *const inputController);
    InputController *inputController() const;
    
    void addCursor(Cursor *const cursor);
    void removeCursor(Cursor *const cursor);
    void setCursors(const QList<Cursor *> &cursors);
    const QList<Cursor *> &cursors() const;
    
    void setSyntaxHighlighter(SyntaxHighlighter *const syntaxHighlighter);
    SyntaxHighlighter *syntaxHighlighter() const;
    
    void setStyleRegions(const QList<StyleRegion> &styleRegions);
    const QList<StyleRegion> &styleRegions() const;
    const QList<StyleRegion> &contiguousStyleRegions() const;
    
    const QPixmap &backing() const;
    QPoint contentPosition() const;
    
    void setSelection(const Region &region);
    void setSelection(const QList<Region> &selection);
    const QList<Region> &selection() const;
    
    Q_PROPERTY(TextOperationHistoryManager *historyManager
      READ historyManager
      WRITE setHistoryManager)
    void setHistoryManager(TextOperationHistoryManager *const historyManager);
    TextOperationHistoryManager *historyManager() const;
    
    quint32 indexUnder(const QPoint &p) const;
    
    virtual void update();
    
  protected:
    virtual void changeEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    
  private Q_SLOTS:
    virtual void sizeChanged(const Region &newRegion, const Region &oldRegion);
    void fitMarginViews();
    virtual void updateDimensions();
    virtual void rehighlight();
    
    virtual void invalidateRegion(const Region &region);
  
  private:
    void updateContiguousStyleRegions();
    void dirty(const QRect &region);
    void renderOn(QPaintDevice *device);
    quint32 marginSize(const Location location) const;
    quint32 computeHeight() const;
    quint32 computeWidth() const;
    
    QList<Region> _selection;
    
    StylePalette *_stylePalette;
    TextModel *_model;
    QList<MarginView *> _marginViews[4];
    bool _marginsVisible;
    QMargins _textMargins;
    QList<Cursor *> _cursors;
    InputController *_inputController;
    QPixmap _backing;
    QList<QRect> _dirty;
    
    qint32 _selectionStart;
    qint32 _selectionEnd;
    
    SyntaxHighlighter *_syntaxHighlighter;
    
    QList<StyleRegion> _styleRegions;
    QList<StyleRegion> _contiguousStyleRegions;
    
    QTimer _highlightTimer;
    HighlightWorker *_highlightWorker;
    QMutex _drawMutex;
    
    TextOperationHistoryManager *_historyManager;
  };
}

#endif