#ifndef _SCINTEX_TEXT_FORMATTER_HPP_
#define _SCINTEX_TEXT_FORMATTER_HPP_

namespace scintex
{
  class TextModel;
  
  class TextFormatter
  {
  public:
    virtual ~TextFormatter();
    
    virtual void format(TextModel *const model) const = 0;
  };
}

#endif