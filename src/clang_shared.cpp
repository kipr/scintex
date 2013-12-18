#include "clang_shared.hpp"

using namespace scintex;

Region scintex::rangeRegion(const CXSourceRange &range)
{
  CXSourceLocation start = clang_getRangeStart(range);
  unsigned startOffset = 0;
  clang_getExpansionLocation(start, 0, 0, 0, &startOffset);
  
  CXSourceLocation end = clang_getRangeEnd(range);
  unsigned endOffset = 0;
  clang_getExpansionLocation(end, 0, 0, 0, &endOffset);
  
  return Region(startOffset, endOffset);
}

Region scintex::cursorRegion(const CXCursor &cur)
{
  const CXSourceRange range = clang_getCursorExtent(cur);
  return rangeRegion(range);
}

quint32 scintex::regionOverlap(const Region &r, const QList<Region> &rs)
{
  // Compute depth from overlapping regions
  // TODO: Very naive implementation
  quint32 depth = 0;
  QList<Region>::const_iterator it = rs.begin();
  for(; it != rs.end(); ++it) {
    const Region &b = *it;
    if(b.contains(r)) ++depth;
  }
  return depth;
}
