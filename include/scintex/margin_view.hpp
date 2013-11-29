#ifndef _SCINTEX_MARGIN_VIEW_HPP_
#define _SCINTEX_MARGIN_VIEW_HPP_

#include <QWidget>

namespace scintex
{
  class TextView;
  class ColorPalette;
  
  class MarginView : public QWidget
  {
  Q_OBJECT
  public:
    MarginView();
    
    void setTextView(TextView *const textView);
    TextView *textView() const;
    
    void setColorPalette(const ColorPalette *const colorPalette);
    const ColorPalette *colorPalette() const;
    
    bool isEngaged() const;
    void disengage();
    
    virtual void fit() = 0;
    
  protected:
    virtual void engaged();
    
  private:
    quint32 _marginSize;
    TextView *_textView;
    const ColorPalette *_colorPalette;
  };
}

#endif