#include <scintex/region.hpp>

using namespace scintex;

Region::Region()
  : _start(0)
  , _end(0)
{
}

Region::Region(const quint32 start, const quint32 end)
  : _start(start)
  , _end(end)
{
}

quint32 Region::start() const
{
  return _start;
}

void Region::setStart(const quint32 start)
{
  Q_ASSERT(_end > start);
  _start = start;
}

quint32 Region::end() const
{
  return _end;
}

void Region::setEnd(const quint32 end)
{
  Q_ASSERT(end >= _start);
  _end = end;
}

quint32 Region::size() const
{
  return _end - _start;
}

Region Region::left() const
{
  return Region(0, _start);
}

Region Region::right(const quint32 end) const
{
  return Region(_end, end);
}

QList<Region> Region::coalesce(QList<Region> regions)
{
  if(regions.isEmpty()) return QList<Region>();
  
  qSort(regions);
  QList<Region>::iterator it = regions.begin();
  Region last = *it;
  for(; it != regions.end(); ++it) {
    const Region current = *it;
    if(last.end() <= current.start()) {
      it = regions.erase(it);
      it = regions.erase(it);
      it = regions.insert(it, Region(last.start(), current.end()));
    }
    last = *it;
  }
  return regions;
}

bool Region::operator <(const Region &rhs) const
{
  return _start < rhs._start;
}