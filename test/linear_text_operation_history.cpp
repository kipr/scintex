#include <scintex/linear_text_operation_history.hpp>

#include <QtTest/QtTest>

using namespace scintex;

class TestLinearTextOperationHistory : public QObject
{
Q_OBJECT
private Q_SLOTS:
  void undo();
  void redo();
  void addTextOperation();
};

void TestLinearTextOperationHistory::undo()
{
  LinearTextOperationHistory history;
  const TextOperation op(Region(0, 1), "a", Region(2, 3), "b");
  history.addTextOperation(op);
  QCOMPARE(history.undosAvailable(), 1U);
  QCOMPARE(history.undo(), op);
  QCOMPARE(history.undosAvailable(), 0U);
}

void TestLinearTextOperationHistory::redo()
{
  LinearTextOperationHistory history;
  const TextOperation op(Region(0, 1), "a", Region(2, 3), "b");
  history.addTextOperation(op);
  QCOMPARE(history.redosAvailable(), 0U);
  QCOMPARE(history.undo(), op);
  QCOMPARE(history.redosAvailable(), 1U);
  QCOMPARE(history.redo(), op);
  QCOMPARE(history.redosAvailable(), 0U);
}

void TestLinearTextOperationHistory::addTextOperation()
{
  LinearTextOperationHistory history;
  const TextOperation op1(Region(0, 1), "a", Region(2, 3), "b");
  const TextOperation op2(Region(4, 5), "c", Region(6, 7), "d");
  history.addTextOperation(op1);
  QCOMPARE(history.undosAvailable(), 1U);
  QCOMPARE(history.redosAvailable(), 0U);
  QCOMPARE(history.undo(), op1);
  history.addTextOperation(op2);
  QCOMPARE(history.undosAvailable(), 1U);
  QCOMPARE(history.redosAvailable(), 0U);
  QCOMPARE(history.undo(), op2);
  QCOMPARE(history.undosAvailable(), 0U);
  QCOMPARE(history.redosAvailable(), 1U);
}

QTEST_MAIN(TestLinearTextOperationHistory)
#include "linear_text_operation_history.moc"