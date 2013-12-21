#ifndef _SCINTEX_CLANG_TEXT_FORMATTER_HPP_
#define _SCINTEX_CLANG_TEXT_FORMATTER_HPP_

#include "text_formatter.hpp"
#include "compat.hpp"

namespace scintex
{
  class DLL_EXPORT ClangTextFormatter : public TextFormatter
  {
  public:
    virtual void format(TextModel *const model) const;
  };
}

#endif