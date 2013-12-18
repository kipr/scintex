#ifndef _SCINTEX_CLANG_SHARED_HPP_
#define _SCINTEX_CLANG_SHARED_HPP_

#include <clang-c/Index.h>
#include <scintex/region.hpp>

namespace scintex
{
  Region rangeRegion(const CXSourceRange &range);
  Region cursorRegion(const CXCursor &cur);
}

#endif