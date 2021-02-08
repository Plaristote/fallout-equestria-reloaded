#include "statmodel.h"
#include "trait.h"
#include "game.h"
#include <QDebug>

Trait::Trait()
{

}

void Trait::toogle(StatModel* model, bool value)
{
  QJSValueList args;
  QJSValue retval;

  args << Game::get()->getScriptEngine().newQObject(model) << value;
  retval = script.property("onToggled").call(args);
  qDebug() << "Euh... allo ?";
  if (retval.isError())
  {
    qDebug() << "Trait" << name << "crashed at line:" << retval.property("lineNumber").toInt() << ":" << retval.toString();
  }
  qDebug() << retval.toVariant().typeName() << retval.toString();
  qDebug() << script.property("name").toString() << "zzz";
}
