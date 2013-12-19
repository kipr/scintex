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

Region Region::shift(const qint32 offset) const
{
  const qint32 start = _start + offset;
  const qint32 end = _end + offset;
  return Region(start < 0 ? 0 : start, end < 0 ? 0 : end);
}

bool Region::overlaps(const Region &r) const
{
  return (r._start < _start && r._end > _start)
    || (r._start < _end && r._end >= _end)
    || contains(r);
}

bool Region::contains(const Region &r) const
{
  return r._start >= _start && r._end <= _end;
}

QList<Region> Region::intersect(const Region &region) const
{
  // No intersection, no split
  if(!contains(region)) {
    return QList<Region>() << *this;
  }
  
  // Sort points and create 3 regions
  QList<int> points;
  points << _start << _end << region.start() << region.end();
  qSort(points);
  
  const Region left   = Region(points[0], points[1]);
  const Region middle = Region(points[1], points[2]);
  const Region right  = Region(points[2], points[3]);
  
  QList<Region> ret;
  ret << left << middle << right;
  return ret;
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

QList<Region> Region::intersect(const QList<Region> &a, const QList<Region> &b)
{
  QList<Region> ret;
  QList<Region>::const_iterator ait = a.begin();
  QList<Region>::const_iterator bit = b.begin();
  for(; bit != b.end(); ++bit) {
    for(; ait != a.end(); ++ait) {
      const QList<Region> regions = (*ait).intersect(*bit);
      if(regions.size() == 1) {
        ret << regions[0] << *bit;
        continue;
      }
      QList<Region>::const_iterator rit = regions.begin();
      for(quint32 i = 0; rit != regions.end(); ++rit, ++i) {
        if(i % 2 || (*rit).size() < 1) continue;
        ret << *rit;
      }
    }
  }
  
  return ret;
}

bool Region::operator <(const Region &rhs) const
{
  return _start < rhs._start;
}

bool Region::operator ==(const Region &rhs) const
{
  return _start == rhs._start && _end == rhs._end;
}