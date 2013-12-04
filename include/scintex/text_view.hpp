#ifndef _SCINTEX_TEXT_VIEW_HPP_
#define _SCINTEX_TEXT_VIEW_HPP_

#include <QScrollArea>
#include <QPixmap>

#include "color_region.hpp"

namespace scintex
{
  class TextModel;
  class MarginView;
  class ColorPalette;
  class InputController;
  class Cursor;
  class TextViewport;
  
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
    
    Q_PROPERTY(ColorPalette *colorPalette
      READ colorPalette
      WRITE setColorPalette)
    void setColorPalette(ColorPalette *const colorPalette);
    ColorPalette *colorPalette() const;
    
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
    
    void setColorRegions(const QList<ColorRegion> &colorRegions);
    const QList<ColorRegion> &colorRegions() const;
    const QList<ColorRegion> &contiguousColorRegions() const;
    
    const QPixmap &backing() const;
    QPoint contentPosition() const;
    
  protected:
    virtual void changeEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    
  private Q_SLOTS:
    void fitMarginViews();
    virtual void updateDimensions();
    virtual void rehighlight();
    
    virtual void invalidateRegion(const quint32 i, const quint32 j);
  
  private:
    void updateContiguousColorRegions();
    void dirty(const QRect &region);
    void renderOn(QPaintDevice *device);
    quint32 marginSize(const Location location);
    quint32 computeHeight() const;
    quint32 computeWidth() const;
    
    ColorPalette *_colorPalette;
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
    
    QList<ColorRegion> _colorRegions;
    QList<ColorRegion> _contiguousColorRegions;
  };
}

#endif