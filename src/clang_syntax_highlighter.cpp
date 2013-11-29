#include <scintex/clang_syntax_highlighter.hpp>
// #include <clang/clang.h>


using namespace scintex;

/*void ClangSyntaxHighlighter::parse(const TextModel *const model)
{
  translation_unit_ = clang_parseTranslationUnit(index_, 0, 0, 0, 0, 0,
      clang_defaultEditingTranslationUnitOptions()); 

  // Here I reparse again every time file changes.
  clang_reparseTranslationUnit(translation_unit_, 0, 0,
      clang_defaultReparseOptions(translation_unit_));

  // This is how I get diagnositics
  CXDiagnostic diag = clang_getDiagnostic(translation_unit_, i);
}*/