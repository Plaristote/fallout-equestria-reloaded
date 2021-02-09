#include "character.h"
#include "game.h"

Character::Character(QObject *parent) : DynamicObject(parent)
{

}

void Character::setScript(const QString& name)
{
  script = Game::get()->loadScript(":/scripts/pnjs/" + name);
  QJSValue callback = script.property("initialize");

  if (callback.isCallable())
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(this);
    Game::get()->scriptCall(callback, args, "initialize");
  }

}
