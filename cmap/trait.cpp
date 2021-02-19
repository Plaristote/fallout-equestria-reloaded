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
