#include <scintex/basic_text_model.hpp>
#include <scintex/text_view.hpp>
#include <scintex/color_palette.hpp>
#include <scintex/line_numbers_view.hpp>
#include <scintex/scroll_overview_view.hpp>
#include <scintex/basic_input_controller.hpp>
#include <scintex/cursor.hpp>

#include <QApplication>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QFile>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  
  QFile f("test.txt");
  if(!f.open(QIODevice::ReadOnly)) {
    return 1;
  }
  
  QString data(f.readAll());
  
  f.close();
  
  scintex::BasicTextModel model(data);
  scintex::TextView view;
  view.setInputController(new scintex::BasicInputController);
  view.addCursor(new scintex::Cursor);
  view.setTextMargins(QMargins(5, 5, 5, 5));
  view.setModel(&model);
  view.setFont(QFont("Menlo", 14));
  view.colorPalette()->setColor("background", Qt::white);
  view.colorPalette()->setColor("text/base", Qt::black);
  
  scintex::LineNumbersView *v = new scintex::LineNumbersView;
  v->setFont(QFont("Helvetica", 12));
  view.addMarginView(scintex::TextView::Left, v);
  view.setColorRegions(QList<scintex::ColorRegion>() << scintex::ColorRegion(0, 100, Qt::green));
  
  QMainWindow w;
  w.setCentralWidget(&view);
  
  w.resize(640, 480);
  w.show();
  w.raise();
  
  return app.exec();
}