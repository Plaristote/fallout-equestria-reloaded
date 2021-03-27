#include "race.h"
#include "game.h"
#include "pluginloader.h"

QMap<QString, Race> Race::races;

bool Race::isPlayable() const
{
  if (script.hasProperty("isPlayable"))
    return script.property("isPlayable").toBool();
  return false;
}

QString Race::getSpriteSheet(StatModel* model) const
{
  QJSValue value = script.property("spriteSheet");

  if (value.isCallable())
  {
    auto& scriptEngine = Game::get()->getScriptEngine();

    value = value.call(QJSValueList() << scriptEngine.newQObject(model));
  }
  return value.toString();
}

void Race::initialize()
{
  races.clear();
  races = CmapPlugins::load<Race>(SCRIPTS_PATH + "cmap/races");
}
