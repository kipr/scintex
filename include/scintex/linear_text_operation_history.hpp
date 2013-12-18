#ifndef _SCINTEX_LINEAR_TEXT_OPERATION_HISTORY_HPP_
#define _SCINTEX_LINEAR_TEXT_OPERATION_HISTORY_HPP_

#include "text_operation.hpp"

#include <QList>

namespace scintex
{
  class LinearTextOperationHistory
  {
  public:
    LinearTextOperationHistory(const quint32 maxSize = 0xFFFFFFFF);
    
    void addTextOperation(const TextOperation &textOperation);
    
    quint32 undosAvailable() const;
    const TextOperation &undo();
    
    quint32 redosAvailable() const;
    const TextOperation &redo();
    
  private:
    void rewind();
    
    quint32 _maxSize;
    QList<TextOperation> _track;
    quint32 _head;
  };
}

#endif