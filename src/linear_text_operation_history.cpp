#include <scintex/linear_text_operation_history.hpp>

#include <QDebug>

using namespace scintex;

LinearTextOperationHistory::LinearTextOperationHistory(const quint32 maxSize)
  : _maxSize(maxSize)
  , _head(0)
{
}

void LinearTextOperationHistory::addTextOperation(const TextOperation &textOperation)
{
  rewind();
  ++_head;
  _track.append(textOperation);
  if(_track.size() >= _maxSize) {
    _track.removeFirst();
    --_head;
  }
}

quint32 LinearTextOperationHistory::undosAvailable() const
{
  if(_track.isEmpty()) return 0;
  return _head;
}

const TextOperation &LinearTextOperationHistory::undo()
{
  return _track[_head-- - 1];
}

quint32 LinearTextOperationHistory::redosAvailable() const
{
  if(_track.isEmpty()) return 0;
  return _track.size() - _head;
}

const TextOperation &LinearTextOperationHistory::redo()
{
  return _track[++_head - 1];
}

void LinearTextOperationHistory::rewind()
{
  while(_track.size() > _head) _track.removeLast();
}