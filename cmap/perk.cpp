#include "perk.h"
#include "game.h"
#include "pluginloader.h"
#include "statmodel.h"

QMap<QString, Perk> Perk::perks;

void Perk::initialize()
{
  perks.clear();
  perks = CmapPlugins::load<Perk>(SCRIPTS_PATH + "cmap/perks");
}

QStringList Perk::getAvailablePerks(StatModel* statistics)
{
  Game* game = Game::get();
  QStringList results;

  for (auto it = perks.begin() ; it != perks.end() ; ++it)
  {
    QJSValue callback = it->script.property("isAvailableFor");
    bool available = true;

    if (callback.isCallable())
    {
      auto& scriptEngine = game->getScriptEngine();

      available = game->scriptCall(callback, QJSValueList() << scriptEngine.newQObject(statistics), "Perk::isAvailableFor").toBool();
    }
    if (available)
      results << it->name;
  }
  return results;
}
