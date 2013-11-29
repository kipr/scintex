#include <scintex/input_controller.hpp>

using namespace scintex;

InputController::~InputController()
{
}

void InputController::setTextView(TextView *const textView)
{
  _textView = textView;
}

TextView *InputController::textView() const
{
  return _textView;
}