#include <scintex/basic_input_controller.hpp>
#include <scintex/text_view.hpp>
#include <scintex/text_model.hpp>
#include <scintex/cursor.hpp>

#include <QDebug>
#include <QKeyEvent>

using namespace scintex;

void BasicInputController::keyPressed(QKeyEvent *const event)
{
  TextModel *const model = textView()->model();
  Q_FOREACH(Cursor *const cursor, textView()->cursors()) {
    const quint32 index = model->index(cursor);
    
    if(event->key() == Qt::Key_Up) {
      cursor->up();
      if(index == model->index(cursor)) cursor->down();
    } else if(event->key() == Qt::Key_Down) {
      cursor->down();
      if(index == model->index(cursor)) cursor->up();
    } else if(event->key() == Qt::Key_Right) {
      cursor->right();
      if(index == model->index(cursor)) cursor->left();
    } else if(event->key() == Qt::Key_Left) {
      cursor->left();
      if(index == model->index(cursor)) cursor->right();
    } else if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
      model->create("\n", index);
      cursor->down();
      cursor->setColumn(0);
    } else if(event->key() == Qt::Key_Backspace) {
      if(index > 0) {
        const Region r(index - 1, index);
        const QString c = model->read(r);
        if(c == "\n") {
          cursor->up();
          cursor->setColumn(model->charsUntil('\n', model->index(cursor)));
        } else cursor->left();
        model->remove(r);
      }
    } else {
      model->create(event->text(), index);
      cursor->right(event->text().size());
    }
  }
  
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
  // selected.
  placeCursors(event->pos());
  // Region::coalesce();
  event->accept();
}

void BasicInputController::mouseReleased(QMouseEvent *const event)
{
  event->accept();
}

void BasicInputController::placeCursors(const QPoint &pos)
{
  TextView *const v = textView();
  const quint32 index = v->indexUnder(pos);
  Q_FOREACH(Cursor *const cursor, textView()->cursors()) {
    if(!cursor->trackMouse()) continue;
    v->model()->placeCursor(index, cursor);
  }
}