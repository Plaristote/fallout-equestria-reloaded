#include "randomencountercontroller.h"
#include "game.h"
#include "i18n.h"

RandomEncounterController::RandomEncounterController(QObject *parent) : QObject(parent)
{
  encounterTimer.setSingleShot(true);
  encounterTimer.setInterval(2000);
  connect(&encounterTimer, &QTimer::timeout, this, &RandomEncounterController::triggerScheduledEncounter);
}

void RandomEncounterController::prepareEncounter(const QString &name, const QVariantMap &parameters)
{
  Game* game = Game::get();

  scheduledEncounter = parameters;
  scheduledEncounterName = name;
  if (!parameters.value("optional", false).toBool())
    encounterTimer.start();
  emit game->encounterNotify(scheduledEncounterName, scheduledEncounter);
}

void RandomEncounterController::triggerScheduledEncounter()
{
  startEncounter(scheduledEncounterName, scheduledEncounter);
}

void RandomEncounterController::startEncounter(const QString &name, const QVariantMap &parameters)
{
  Game* game = Game::get();
  I18n* i18n = I18n::get();
  const QVariantList list = parameters.value("parties").toList();
  const QString entryZone = parameters.value("entryZone", "").toString();

  if (!game->getLevel())
  {
    emit game->encounterTriggered(parameters.value("title", i18n->t("messages.encounter-unknown")).toString());
    game->goToLevel(name);
    game->getLevel()->insertPartyIntoZone(game->getPlayerParty(), entryZone);
  }
  else
    game->switchToLevel(name, entryZone);
  game->getLevel()->setProperty("persistent", parameters.value("persistent", false));
  for (const QVariant& entry : list)
  {
    const QVariantMap partyData(entry.toMap());
    CharacterParty*   party = CharacterParty::factory(partyData, game->getLevel());

    party->insertIntoZone(game->getLevel(), partyData.value("zone").toString());
  }
}
