#include <scintex/text_operation_history_manager.hpp>
#include <scintex/linear_text_operation_history.hpp>
#include <scintex/text_model.hpp>

#include <QDebug>

using namespace scintex;

TextOperationHistoryManager::TextOperationHistoryManager()
{
}

TextOperationHistoryManager::~TextOperationHistoryManager()
{
  flush();
}

void TextOperationHistoryManager::flush()
{
  qDeleteAll(_history.values());
  _history.clear();
}

void TextOperationHistoryManager::addOperation(Cursor *const cursor, const TextOperation *const op)
{
  if(!_history.contains(cursor)) _history[cursor] = new LinearTextOperationHistory();
  _history[cursor]->addTextOperation(*op);
}

void TextOperationHistoryManager::removeCursor(Cursor *const cursor)
{
  if(!_history.contains(cursor)) return;
  delete _history.take(cursor);
}

void TextOperationHistoryManager::redo(TextModel *const model)
{
  QMap<Cursor *, LinearTextOperationHistory *>::const_iterator it = _history.begin();
  for(; it != _history.end(); ++it) {
    if(it.value()->redosAvailable() < 1) continue;
    const TextOperation &op = it.value()->redo();
    op.apply(model);
    model->placeCursor(op.regionB().end(), it.key());
  }
}

void TextOperationHistoryManager::undo(TextModel *const model)
{
  QMap<Cursor *, LinearTextOperationHistory *>::const_iterator it = _history.begin();
  for(; it != _history.end(); ++it) {
    if(it.value()->undosAvailable() < 1) continue;
    const TextOperation &op = it.value()->undo();
    op.unapply(model);
    model->placeCursor(op.regionA().end(), it.key());
  }
}
