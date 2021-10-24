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
  const QString entryZone = parameters.value("entryZone", "").toString();

  scheduledEncounter = parameters;
  if (!game->getLevel())
    emit game->encounterTriggered(parameters.value("title", i18n->t("messages.encounter-unknown")).toString());
  connect(game, &Game::levelChanged, this, &RandomEncounterController::initializeEncounter);
  game->switchToLevel(name, entryZone);
}

void RandomEncounterController::initializeEncounter()
{
  Game* game = Game::get();
  const QVariantList list = scheduledEncounter.value("parties").toList();
  
  disconnect(game, &Game::levelChanged, this, &RandomEncounterController::initializeEncounter);
  game->getLevel()->setProperty("persistent", scheduledEncounter.value("persistent", false));
  for (const QVariant& entry : list)
  {
    const QVariantMap partyData(entry.toMap());
    CharacterParty*   party = CharacterParty::factory(partyData, game->getLevel());

    if (party->getCharacters().size() > 0)
      party->insertIntoZone(game->getLevel(), partyData.value("zone").toString());
    else
      qDebug() << "RandomEncounterController::initializeEncounter: generated an empty CharacterParty";
  }
}
