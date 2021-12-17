#include "leveltask.h"
#include "game.h"
#include "characterdialog.h"
#include <QJsonArray>
#include "characters/actionqueue.h"
#include "objects/inventoryitem.h"
#include "objects/doorway.h"
#include "objects/bloodstain.h"
#include "game/animationSequence/movementhintanimationpart.h"
#include "game/mousecursor.h"
#include <QDebug>
#include "objects/objectfactory.h"

LevelTask::LevelTask(QObject *parent) : ParentType(parent)
{
  soundManager = new SoundManager(this);
  taskRunner = new TaskRunner(this);
  updateTimer.setInterval(30);
  updateTimer.setSingleShot(false);
  connect(&updateTimer, &QTimer::timeout, this, &LevelTask::update);
  connect(this, &LevelTask::pausedChanged, this, &LevelTask::onPauseChanged);
  connect(this, &LevelTask::pausedChanged, MouseCursor::get(), &MouseCursor::updatePointerType);
  connect(this, &LevelTask::combatChanged, this, &LevelTask::onCombatChanged);
  connect(this, &InteractionComponent::playerMovingTo, this, &LevelTask::displayMovementTargetHint);
}

LevelTask::~LevelTask()
{
  qDebug() << "LevelTask::destroyed";
}

void LevelTask::deleteLater()
{
  updateTimer.stop();
  ParentType::deleteLater();
}

void LevelTask::displayMovementTargetHint(QPoint targetPosition)
{
  AnimationSequence* animation = new AnimationSequence;

  animation->addAnimationPart(new MovementHintAnimationPart(targetPosition));
  addAnimationSequence(animation);
}

void LevelTask::registerDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
  {
    Character* character = reinterpret_cast<Character*>(object);

    addCharacterObserver(character, connect(character, &Character::itemDropped, this, [this, character](InventoryItem* item) { onItemDropped(item, character->getPosition()); }));
    addCharacterObserver(character, connect(character, &Character::characterKill, this, &LevelTask::onCharacterKill));
    if (character == getPlayer())
      addCharacterObserver(character, connect(character, &Character::floorChanged, this, [this]() { setCurrentFloor(getPlayer()->getCurrentFloor()); }, Qt::QueuedConnection));
  }

  connect(object, &DynamicObject::lightZoneAdded, getTileMap(), &TileMap::addLightLayer);
  connect(object, &DynamicObject::lightZoneRemoved, getTileMap(), &TileMap::removeLightLayer);
  object->reloadLightzone();

  ParentType::registerDynamicObject(object);
}

void LevelTask::unregisterDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
  {
    auto* character = reinterpret_cast<Character*>(object);

    eachObject([character](DynamicObject* entry)
    {
      if (entry->isCharacter())
        reinterpret_cast<Character*>(entry)->getFieldOfView()->removeCharacter(character);
    });
  }

  performanceMetrics.removeObject(object);
  disconnect(object, &DynamicObject::lightZoneAdded, getTileMap(), &TileMap::addLightLayer);
  disconnect(object, &DynamicObject::lightZoneRemoved, getTileMap(), &TileMap::removeLightLayer);

  ParentType::unregisterDynamicObject(object);
}

void LevelTask::onItemDropped(InventoryItem* item, QPoint itemPosition, unsigned char itemFloor)
{
  item->setParent(this);
  appendObject(item);
  setObjectPosition(item, itemPosition.x(), itemPosition.y(), itemFloor);
}

void LevelTask::onCharacterKill(Character* victim, Character* killer)
{
  auto* playerParty = Game::get()->getPlayerParty();
  auto* victimSheet = victim->getStatistics();
  QString victimRace = victimSheet->property("race").toString();

  Game::get()->getQuestManager()->onCharacterKilled(victim, killer);
  if (killer)
    killer->getStatistics()->addKill(victimRace.length() > 0 ? victimRace : "Others");
  if (playerParty->containsCharacter(killer))
  {
    const unsigned int xp         = victim->getXpValue();
    const QString      victimName = victimSheet->getName();

    playerParty->grantXp(xp);
    displayConsoleMessage("You killed " + victimName + " and earned " + QString::number(xp) + " experience points.");
  }
}

void LevelTask::onPauseChanged()
{
  qDebug() << "LevelTask::onPauseChanged:" << paused;
  if (paused)
    updateTimer.stop();
  else
  {
    updateTimer.start();
    clock.restart();
  }
}

void LevelTask::update()
{
  qint64 delta = clock.restart();

  if ((combat && isCharacterTurn(getPlayer())) || !combat)
  {
    bool koMode   = getPlayer()->isUnconscious();
    bool busyMode = !getPlayer()->getActionQueue()->canInterrupt();

    enableWaitingMode(koMode || busyMode);
  }

  for (TileLayer* roof : getTileMap()->getRoofs())
  {
    auto point = getPlayer()->getPoint();
    roof->setVisible(!roof->isInside(point.x, point.y));
  }

  ParentType::update(delta);
  if (!combat)
    realTimeTask(delta);
  else
  {
    if (!isCharacterTurn(getPlayer()) || finalizeTurnRemainingTime > 0)
      delta *= static_cast<int>(combatSpeedOption);
    if (finalizeTurnRemainingTime <= 0)
      combatTask(delta);
    else
      endTurnTask(delta);
  }
  updateVisualEffects(delta);
  soundManager->update();
  emit updated();
}

void LevelTask::realTimeTask(qint64 delta)
{
  const auto objectList   = attachedObjects;
  const auto objectGroups = allObjectGroups();

  timeManager->addElapsedMilliseconds(delta);
  for (DynamicObject* object : objectList)
  {
    ObjectPerformanceClock clock(performanceMetrics.object(object));
    Character* asCharacter = reinterpret_cast<Character*>(object);

    object->update(delta);
    object->updateTasks(delta);
    if (object->isCharacter() && asCharacter->isAlive())
    {
      asCharacter->getFieldOfView()->update(delta);
      asCharacter->getActionQueue()->update();
    }
  }
  for (ObjectGroup* group : objectGroups)
    group->getTaskManager()->update(delta);
  taskRunner->update(delta);
  Game::get()->getTaskManager()->update(delta);
}

void LevelTask::combatTask(qint64 delta)
{
  const auto objectList = allDynamicObjects();

  for (DynamicObject* object : objectList)
  {
    ObjectPerformanceClock clock(performanceMetrics.object(object));

    object->update(delta);
  }
  if (combattants.size() > combatIterator && combatIterator >= 0)
  {
    auto* combattant = combattants.at(combatIterator);
    ObjectPerformanceClock clock(performanceMetrics.object(combattant));
    bool hasActionPoints  = combattant->getActionPoints() > 0;
    bool hasRunningAction = !combattant->getActionQueue()->isEmpty();
    bool isIdle           = combattant == getPlayer() && hasActionPoints;

    if (hasRunningAction || isIdle)
      combattant->getActionQueue()->update();
    else
      onNextCombatTurn();
  }
  else
    throw std::runtime_error("combattants.size() > combatIterator");
}

void LevelTask::endTurnTask(qint64 delta)
{
  const auto objectList = allDynamicObjects();
  unsigned short affectedCharacters = 0;

  for (DynamicObject* object : objectList)
  {
    Character* asCharacter = reinterpret_cast<Character*>(object);

    if (object->isCharacter() && combattants.indexOf(asCharacter) < 0 && asCharacter->isAlive())
    {
      ObjectPerformanceClock clock(performanceMetrics.object(object));

      asCharacter->getActionQueue()->update();
      if (!asCharacter->getActionQueue()->isEmpty())
        ++affectedCharacters;
    }
  }
  if (affectedCharacters > 0)
    finalizeTurnRemainingTime -= delta;
  else
    finalizeTurnRemainingTime = 0;
  enableWaitingMode(finalizeTurnRemainingTime > 0);
}

void LevelTask::onCombatChanged()
{
  for (auto* object : allDynamicObjects())
  {
    if (object->isCharacter())
    {
      auto* actionQueue = reinterpret_cast<Character*>(object)->getActionQueue();

      if (combat)
        actionQueue->pause();
      else
        actionQueue->unpause();
    }
  }
}

void LevelTask::finalizeRound()
{
  finalizeTurnRemainingTime = WORLDTIME_TURN_DURATION;
  for (DynamicObject* object : allDynamicObjects())
  {
    Character* asCharacter = reinterpret_cast<Character*>(object);

    if (!object->isCharacter() || (!isInCombat(asCharacter) && asCharacter->isAlive()))
      object->getTaskManager()->update(WORLDTIME_TURN_DURATION);
    if (object->isCharacter() && object != getPlayer())
    {
      qDebug() << "Update field of view for " << asCharacter->getStatistics()->getName();
      asCharacter->getFieldOfView()->update(WORLDTIME_TURN_DURATION);
      qDebug() << "-> Enemy count" << asCharacter->getFieldOfView()->GetDetectedEnemies().length();
    }
  }
  for (ObjectGroup* group : allObjectGroups())
    group->getTaskManager()->update(WORLDTIME_TURN_DURATION);
  taskRunner->update(WORLDTIME_TURN_DURATION);
  Game::get()->getTaskManager()->update(WORLDTIME_TURN_DURATION);
  ParentType::finalizeRound();
}

void LevelTask::addBloodStainAt(QPoint position_, unsigned char floor_)
{
  factory()->addBloodStainAt(position_, floor_);
}

QVariantList LevelTask::previewPathTo(int x, int y)
{
  auto& grid = getPathfinder();
  QList<Point> path;
  QVariantList result;
  Point target{x, y, static_cast<unsigned char>(getCurrentFloor())};

  if (getPlayer() && grid.findPath(getPlayer()->getPoint(), target, path, getPlayer()))
  {
    for (const auto& point : qAsConst(path))
      result.push_back(QPoint(point));
  }
  return result;
}
