#include <scintex/text_operation.hpp>
#include <scintex/text_model.hpp>
#include <QDebug>

using namespace scintex;

TextOperation::TextOperation(const Region &ar, const QString &as, const Region &br,
    const QString &bs)
  : _ar(ar)
  , _as(as)
  , _br(br)
  , _bs(bs)
{
}

void TextOperation::setRegionA(const Region &ar)
{
  _ar = ar;
}

const Region &TextOperation::regionA() const
{
  return _ar;
}

void TextOperation::setStringA(const QString &as)
{
  _as = as;
}

const QString &TextOperation::stringA() const
{
  return _as;
}

void TextOperation::setRegionB(const Region &br)
{
  _br = br;
}

const Region &TextOperation::regionB() const
{
  return _br;
}

void TextOperation::setStringB(const QString &bs)
{
  _bs = bs;
}

const QString &TextOperation::stringB() const
{
  return _bs;
}

void TextOperation::apply(TextModel *const model) const
{
  model->remove(_ar);
  model->create(_bs, _br.start());
  qDebug() << _ar.start() << "to" << _ar.end();
}

void TextOperation::unapply(TextModel *const model) const
{
  model->remove(_br);
  model->create(_as, _ar.start());
  qDebug() << _br.start() << "to" << _br.end();
}

bool TextOperation::operator ==(const TextOperation &rhs) const
{
  return _ar == rhs._ar && _as == rhs._as && _br == rhs._br && _bs == rhs._bs;
}