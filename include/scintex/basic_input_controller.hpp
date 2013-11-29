#ifndef _SCINTEX_BASIC_INPUT_CONTROLLER_HPP_
#define _SCINTEX_BASIC_INPUT_CONTROLLER_HPP_

#include "input_controller.hpp"

namespace scintex
{
  class BasicInputController : public InputController
  {
  public:
    virtual void pressed(QKeyEvent *const event);
    virtual void released(QKeyEvent *const event);
  };
}

#endif