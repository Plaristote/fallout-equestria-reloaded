#include "statmodel.h"
#include "trait.h"
#include "game.h"
#include <QDebug>

Trait::Trait()
{

}

void Trait::toogle(StatModel* model, bool value)
{
  Game* game = Game::get();
  QJSValueList args;
  QJSValue callback = script.property("onToggled");

  args << game->getScriptEngine().newQObject(model) << value;
  game->scriptCall(callback, args, "Trait::onToggled");
}

int Trait::modifyBaseStatistic(StatModel* model, const QString &attribute, int baseValue)
{
  Game* game = Game::get();
  QJSValueList args;
  QJSValue callback = script.property("modifyBaseStatistic");

  if (callback.isCallable())
  {
    args << game->getScriptEngine().newQObject(model) << attribute << baseValue;
    return game->scriptCall(callback, args, "Trait::modifyBaseValueFor").toInt();
  }
  return baseValue;
}

int Trait::modifyBaseSkill(StatModel *model, const QString &attribute, int baseValue)
{
  Game* game = Game::get();
  QJSValueList args;
  QJSValue callback = script.property("modifyBaseSkill");

  if (callback.isCallable())
  {
    args << game->getScriptEngine().newQObject(model) << attribute << baseValue;
    return game->scriptCall(callback, args, "Trait::modifyBaseValueFor").toInt();
  }
  return baseValue;
}
