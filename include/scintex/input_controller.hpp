#ifndef _SCINTEX_INPUT_CONTROLLER_HPP_
#define _SCINTEX_INPUT_CONTROLLER_HPP_

#include <QObject>

class QKeyEvent;

namespace scintex
{
  class TextView;
  
  class InputController : public QObject
  {
  Q_OBJECT
  public:
    virtual ~InputController();
    
    void setTextView(TextView *const textView);
    TextView *textView() const;
    
    virtual void pressed(QKeyEvent *const event) = 0;
    virtual void released(QKeyEvent *const event) = 0;
    
  private:
    TextView *_textView;
  };
}

#endif