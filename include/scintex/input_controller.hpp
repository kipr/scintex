#ifndef _SCINTEX_INPUT_CONTROLLER_HPP_
#define _SCINTEX_INPUT_CONTROLLER_HPP_

#include <QObject>

class QKeyEvent;
class QMouseEvent;

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
    
    virtual void keyPressed(QKeyEvent *const event) = 0;
    virtual void keyReleased(QKeyEvent *const event) = 0;
    
    virtual void mousePressed(QMouseEvent *const event) = 0;
    virtual void mouseMoved(QMouseEvent *const event) = 0;
    virtual void mouseReleased(QMouseEvent *const event) = 0;
    
  private:
    TextView *_textView;
  };
}

#endif