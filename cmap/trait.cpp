#include "statmodel.h"
#include "trait.h"
#include "game.h"
#include <QDebug>

CmapPlugin::CmapPlugin()
{

}

void CmapPlugin::toogle(StatModel* model, bool value)
{
  Game* game = Game::get();
  QJSValueList args;
  QJSValue callback = script.property("onToggled");

  args << game->getScriptEngine().newQObject(model) << value;
  if (callback.isCallable())
    game->scriptCall(callback, args, "CmapPlugin::onToggled");
}

int CmapPlugin::modifyBaseStatistic(StatModel* model, const QString &attribute, int baseValue)
{
  Game* game = Game::get();
  QJSValueList args;
  QJSValue callback = script.property("modifyBaseStatistic");

  if (callback.isCallable())
  {
    args << game->getScriptEngine().newQObject(model) << attribute << baseValue;
    return game->scriptCall(callback, args, "CmapPlugin::modifyBaseValueFor").toInt();
  }
  return baseValue;
}

int CmapPlugin::modifyBaseSkill(StatModel *model, const QString &attribute, int baseValue)
{
  Game* game = Game::get();
  QJSValueList args;
  QJSValue callback = script.property("modifyBaseSkill");

  if (callback.isCallable())
  {
    args << game->getScriptEngine().newQObject(model) << attribute << baseValue;
    return game->scriptCall(callback, args, "CmapPlugin::modifyBaseValueFor").toInt();
  }
  return baseValue;
}

bool Race::isPlayable() const
{
  if (script.hasProperty("isPlayable"))
    return script.property("isPlayable").toBool();
  return false;
}
