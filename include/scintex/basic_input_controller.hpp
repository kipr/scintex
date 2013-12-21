#ifndef _SCINTEX_BASIC_INPUT_CONTROLLER_HPP_
#define _SCINTEX_BASIC_INPUT_CONTROLLER_HPP_

#include "input_controller.hpp"
#include "region.hpp"
#include "compat.hpp"

#include <QMap>

namespace scintex
{
  class LinearTextOperationHistory;
  class Cursor;
  class TextOperation;
  
  class DLL_EXPORT BasicInputController : public InputController
  {
  public:
    BasicInputController();
    ~BasicInputController();
    
    virtual void flushState();
    
    virtual void keyPressed(QKeyEvent *const event);
    virtual void keyReleased(QKeyEvent *const event);
    virtual void mousePressed(QMouseEvent *const event);
    virtual void mouseMoved(QMouseEvent *const event);
    virtual void mouseReleased(QMouseEvent *const event);
    
  private:
    QList<Region> placeCursors(const QPoint &pos);
  };
}

#endif