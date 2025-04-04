#include "statmodel.h"
#include "plugin.h"
#include "game.h"

CmapPlugin::CmapPlugin()
{

}

void CmapPlugin::toogle(StatModel* model, bool value) const
{
  Game* game = Game::get();
  QJSValueList args;
  QJSValue callback = script.property("onToggled");

  args << game->getScriptEngine().newQObject(model) << value;
  if (callback.isCallable())
    game->scriptCall(callback, args, "CmapPlugin::onToggled");
}

int CmapPlugin::modifyBaseStatistic(StatModel* model, const QString &attribute, int baseValue) const
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

int CmapPlugin::modifyBaseSkill(StatModel *model, const QString &attribute, int baseValue) const
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
