#include <scintex/text_model.hpp>

using namespace scintex;

TextModel::~TextModel()
{
}

quint32 TextModel::lines() const
{
  return occurencesOf('\n', 0, size());
}