#include <scintex/basic_input_controller.hpp>
#include <scintex/text_view.hpp>
#include <scintex/text_model.hpp>
#include <scintex/cursor.hpp>

#include <QDebug>
#include <QKeyEvent>

using namespace scintex;

void BasicInputController::pressed(QKeyEvent *const event)
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
        const QString c = model->read(index - 1, index);
        if(c == "\n") {
          cursor->up();
          cursor->setColumn(model->charsUntil('\n', model->index(cursor)));
        } else cursor->left();
        model->remove(index - 1, index);
      }
    } else {
      model->create(event->text(), index);
      cursor->right(event->text().size());
    }
  }
  
  event->accept();
}

void BasicInputController::released(QKeyEvent *const event)
{
  event->accept();
}