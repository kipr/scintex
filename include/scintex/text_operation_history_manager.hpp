#ifndef _SCINTEX_TEXT_OPERATION_HISTORY_MANAGER_HPP_
#define _SCINTEX_TEXT_OPERATION_HISTORY_MANAGER_HPP_

#include <QMap>
#include "compat.hpp"

namespace scintex
{
  class Cursor;
  class LinearTextOperationHistory;
  class TextModel;
  class TextOperation;
  
  class DLL_EXPORT TextOperationHistoryManager
  {
  public:
    TextOperationHistoryManager();
    ~TextOperationHistoryManager();
    
    void flush();
    
    void addOperation(Cursor *const cursor, const TextOperation *const op);
    void removeCursor(Cursor *const cursor);
    
    void redo(TextModel *const model);
    void undo(TextModel *const model);
    
  private:
    QMap<Cursor *, LinearTextOperationHistory *> _history;
  };
}

#endif