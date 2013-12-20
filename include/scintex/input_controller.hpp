#ifndef _SCINTEX_INPUT_CONTROLLER_HPP_
#define _SCINTEX_INPUT_CONTROLLER_HPP_

#include <QObject>
#include "compat.hpp"

class QKeyEvent;
class QMouseEvent;

namespace scintex
{
  class TextView;
  
  class DLL_EXPORT InputController : public QObject
  {
  Q_OBJECT
  public:
    virtual ~InputController();
    
    void setTextView(TextView *const textView);
    TextView *textView() const;
    
    virtual void flushState() = 0;
    
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