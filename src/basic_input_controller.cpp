#include <scintex/basic_input_controller.hpp>
#include <scintex/text_view.hpp>
#include <scintex/text_model.hpp>
#include <scintex/cursor.hpp>
#include <scintex/linear_text_operation_history.hpp>
#include <scintex/clang_text_formatter.hpp>
#include <scintex/text_operation_history_manager.hpp>

#include <QDebug>
#include <QKeyEvent>

using namespace scintex;

BasicInputController::BasicInputController()
{
}

BasicInputController::~BasicInputController()
{
  flushState();
}

void BasicInputController::flushState()
{
  
}

void BasicInputController::keyPressed(QKeyEvent *const event)
{
  TextModel *const model = textView()->model();
  
  const Qt::KeyboardModifiers m = event->modifiers();
  const int k = event->key();
  
  if(m & Qt::ControlModifier) {
    if(m & Qt::ShiftModifier && k == Qt::Key_Z) {
          textView()->historyManager()->redo(model);
    } else if(k == Qt::Key_Z) {
      textView()->historyManager()->undo(model);
    } else if(k == Qt::Key_I) {
      ClangTextFormatter formatter;
      formatter.format(model);
    }
    goto done;
  }
  
  Q_FOREACH(Cursor *const cursor, textView()->cursors()) {
    const quint32 index = model->index(cursor);
    
    if(k == Qt::Key_Up) {
      cursor->up();
      if(index == model->index(cursor)) cursor->down();
    } else if(k == Qt::Key_Down) {
      cursor->down();
      if(index == model->index(cursor)) cursor->up();
    } else if(k == Qt::Key_Right && model->size() > 0 && index < model->size() - 1) {
      model->placeCursor(index + 1, cursor);
    } else if(k == Qt::Key_Left && index > 0) {
      model->placeCursor(index - 1, cursor);
    } else if(k == Qt::Key_Enter || k == Qt::Key_Return) {
      const TextOperation op = model->create("\n", index);
      textView()->historyManager()->addOperation(cursor, &op);
      cursor->down();
      cursor->setColumn(0);
    } else if(k == Qt::Key_Backspace) {
      if(index > 0) {
        const Region r(index - 1, index);
        if(model->read(r) == "\n") {
          cursor->up();
          cursor->setColumn(model->charsUntil('\n', model->index(cursor)));
        } else cursor->left();
        const TextOperation op = model->remove(r);
        textView()->historyManager()->addOperation(cursor, &op);
      }
    } else {
      const TextOperation op = model->create(event->text(), index);
      textView()->historyManager()->addOperation(cursor, &op);
      cursor->right(event->text().size());
    }
  }
  
  done:
  event->accept();
}

void BasicInputController::keyReleased(QKeyEvent *const event)
{
  event->accept();
}

void BasicInputController::mousePressed(QMouseEvent *const event)
{
  TextView *const v = textView();
  const quint32 index = v->indexUnder(event->pos());
  
  if(event->modifiers() & Qt::ShiftModifier) {
    quint32 min = index;
    quint32 max = index;
    Q_FOREACH(Cursor *const cursor, textView()->cursors()) {
      if(!cursor->trackMouse()) continue;
      const quint32 i = v->model()->index(cursor);
      max = qMax(max, i);
      min = qMin(min, i);
    }
    
    v->setSelection(Region(min, max));
  } else {
    v->setSelection(Region());
  }
  
  placeCursors(event->pos());
  event->accept();
}

void BasicInputController::mouseMoved(QMouseEvent *const event)
{
  TextView *const v = textView();
  QList<Region> selected = v->selection();
  QList<Region> movementRegions = placeCursors(event->pos());
  v->setSelection(Region::intersect(selected, movementRegions));
  qDebug() << "Selection size:" << v->selection().size();
  event->accept();
}

void BasicInputController::mouseReleased(QMouseEvent *const event)
{
  event->accept();
}

QList<Region> BasicInputController::placeCursors(const QPoint &pos)
{
  TextView *const v = textView();
  const quint32 index = v->indexUnder(pos);
  QList<Region> ret;
  Q_FOREACH(Cursor *const cursor, textView()->cursors()) {
    if(!cursor->trackMouse()) continue;
    const quint32 old = v->model()->index(cursor);
    v->model()->placeCursor(index, cursor);
    ret.append(Region(qMin(old, index), qMax(old, index)));
  }
  return ret;
}

