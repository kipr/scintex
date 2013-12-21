#ifndef _SCINTEX_TEXT_FORMATTER_HPP_
#define _SCINTEX_TEXT_FORMATTER_HPP_

#include "compat.hpp"

namespace scintex
{
  class TextModel;
  
  class DLL_EXPORT TextFormatter
  {
  public:
    virtual ~TextFormatter();
    
    virtual void format(TextModel *const model) const = 0;
  };
}

#endif