#ifndef _SCINTEX_BASIC_INPUT_CONTROLLER_HPP_
#define _SCINTEX_BASIC_INPUT_CONTROLLER_HPP_

#include "input_controller.hpp"

namespace scintex
{
  class BasicInputController : public InputController
  {
  public:
    virtual void keyPressed(QKeyEvent *const event);
    virtual void keyReleased(QKeyEvent *const event);
    virtual void mousePressed(QMouseEvent *const event);
    virtual void mouseMoved(QMouseEvent *const event);
    virtual void mouseReleased(QMouseEvent *const event);
    
  private:
    void placeCursors(const QPoint &pos);
  };
}

#endif