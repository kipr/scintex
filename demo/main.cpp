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
  const QColor base03("#002b36");
  const QColor base02("#073642");
  const QColor base01("#586e75");
  const QColor base00("#657b83");
  const QColor base0("#839496");
  const QColor base1("#93a1a1");
  const QColor base2("#eee8d5");
  const QColor base3("#fdf6e3");
  const QColor yellow("#b58900");
  const QColor orange("#cb4b16");
  const QColor red("#dc322f");
  const QColor magenta("#d33682");
  const QColor violet("#6c71c4");
  const QColor blue("#268bd2");
  const QColor cyan("#2aa198");
  const QColor green("#859900");
  
  using namespace scintex;
  
  StylePalette *const cp = view.stylePalette();
  cp->setStyle("text/background", Style(base3));
  cp->setStyle("line_numbers/background", Style(base3));
  cp->setStyle("line_numbers/base", Style(base01));
  cp->setStyle("text/base", Style(base01));
  cp->setStyle("text/margin_divider", Style(base01));
  Q_FOREACH(const QString &role, cp->roles()) {
    if(!role.startsWith("code/")) continue;
    if(role.startsWith("code/declaration") && role.endsWith("type")) {
      cp->setStyle(role, Style(red, true));
      continue;
    }
    if(role.startsWith("code/declaration") && role.endsWith("keyword")) {
      cp->setStyle(role, Style(base01, true));
      continue;
    }
  }
  
  cp->setStyle("code/declaration", Style(base01));
  cp->setStyle("code/reference", Style(base1));
  cp->setStyle("code/statement", Style(green));
  cp->setStyle("code/literal", Style(magenta));
  cp->setStyle("code/comment", Style(base1, false, true));
  cp->setStyle("code/documentation", Style(base1, false, true));
  cp->setStyle("code/preprocessor", Style(violet));
  
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