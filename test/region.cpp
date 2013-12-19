#include <scintex/region.hpp>

#include <QtTest/QtTest>

using namespace scintex;

class TestRegion : public QObject
{
Q_OBJECT
private Q_SLOTS:
  void overlaps();
};

void TestRegion::overlaps()
{
  const Region a(10, 20);
  QCOMPARE(a.overlaps(Region(5, 9)), false);
  QCOMPARE(a.overlaps(Region(5, 10)), false);
  QCOMPARE(a.overlaps(Region(5, 11)), true);
  QCOMPARE(a.overlaps(Region(11, 19)), true);
  QCOMPARE(a.overlaps(Region(20, 25)), false);
  QCOMPARE(a.overlaps(Region(21, 25)), false);
}


QTEST_MAIN(TestRegion)
#include "region.moc"