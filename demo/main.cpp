#include <scintex/basic_text_model.hpp>
#include <scintex/text_view.hpp>
#include <scintex/style_palette.hpp>
#include <scintex/line_numbers_view.hpp>
#include <scintex/scroll_overview_view.hpp>
#include <scintex/basic_input_controller.hpp>
#include <scintex/clang_syntax_highlighter.hpp>
#include <scintex/cursor.hpp>

#include <QApplication>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QFile>
#include <QDebug>

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
  view.setSyntaxHighlighter(new scintex::ClangSyntaxHighlighter);
  
  view.stylePalette()->setStyle("background", scintex::Style(Qt::white));
  view.stylePalette()->setStyle("text/base", scintex::Style(Qt::black));
  scintex::StylePalette *const cp = view.stylePalette();
  const double goldenAngle = 137.508;
  double h = 0;
  double s = 1.0;
  Q_FOREACH(const QString &role, cp->roles()) {
    if(!role.startsWith("code/")) continue;
    qDebug() << role << "->" << QColor::fromHsvF(h / 360.0, 1.0, 1.0);
    cp->setStyle(role, scintex::Style(QColor::fromHsvF(h / 360.0, s, 1.0), false, true));
    h += 10.0;
    if(h > 360.0) {
      h -= 360.0;
      s -= 0.1;
    }
  }
  
  view.stylePalette()->setStyle("code/keyword", scintex::Style(Qt::blue, true));
  
  view.addCursor(new scintex::Cursor(true));
  view.setTextMargins(QMargins(5, 5, 5, 5));
  view.setModel(&model);
  view.setFont(QFont("Menlo", 14));

  
  scintex::LineNumbersView *v = new scintex::LineNumbersView;
  v->setFont(QFont("Helvetica", 12));
  view.addMarginView(scintex::TextView::Left, v);
  
  QMainWindow w;
  w.setCentralWidget(&view);
  
  w.resize(640, 480);
  w.show();
  w.raise();
  
  return app.exec();
}