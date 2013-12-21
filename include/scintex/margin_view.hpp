#ifndef _SCINTEX_MARGIN_VIEW_HPP_
#define _SCINTEX_MARGIN_VIEW_HPP_

#include <QWidget>
#include "compat.hpp"

namespace scintex
{
  class TextView;
  class StylePalette;
  
  class DLL_EXPORT MarginView : public QWidget
  {
  Q_OBJECT
  public:
    MarginView();
    virtual ~MarginView();
    
    void setTextView(TextView *const textView);
    TextView *textView() const;
    
    void setStylePalette(const StylePalette *const stylePalette);
    const StylePalette *stylePalette() const;
    
    bool isEngaged() const;
    void disengage();
    
    virtual void fit() = 0;
    
  protected:
    virtual void engaged();
    
  private:
    quint32 _marginSize;
    TextView *_textView;
    const StylePalette *_stylePalette;
  };
}

#endif