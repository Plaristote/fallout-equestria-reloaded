#include "race.h"
#include "game.h"

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
