#include "interactiontargetlist.h"
#include "game.h"
#include "game/mousecursor.h"

InteractionTargetList::InteractionTargetList(QObject *parent) : QObject(parent)
{
}

void InteractionTargetList::reset()
{
  targets.clear();
  cursor = 0;
  emit targetsUpdated();
}

Q_INVOKABLE DynamicObject* InteractionTargetList::nextTarget()
{
  if (targets.size() > 0)
  {
    if (static_cast<int>(cursor) + 1 < targets.size())
      cursor++;
    else
      cursor = 0;
    return targets.at(static_cast<int>(cursor));
  }
  return nullptr;
}

Q_INVOKABLE DynamicObject* InteractionTargetList::previousTarget()
{
  if (targets.size() > 0)
  {
    if (cursor > 0)
      cursor--;
    else
      cursor = static_cast<unsigned int>(targets.size()) - 1;
    return targets.at(static_cast<int>(cursor));
  }
  return nullptr;
}

void InteractionTargetList::unregisterDynamicObject(DynamicObject* object)
{
  targets.removeAll(object);
  emit targetsUpdated();
}

void InteractionTargetList::findItemTargets(InventoryItem* item, const QList<DynamicObject*>& objects, const QList<Character*>& visibleCharacters)
{
  reset();
  for (Character* character : visibleCharacters)
  {
    if (item->isValidTarget(character) && item->isInRange(character))
      targets.push_back(character);
  }
  for (DynamicObject* object : objects)
  {
    if (!object->isCharacter() && object->isVisible() && item->isValidTarget(object) && item->isInRange(object))
      targets.push_back(object);
  }
  if (targets.size() > 0)
    afterTargetUpdate();
}

void InteractionTargetList::findNearbyTargets(const QList<DynamicObject*>& objects)
{
  Character* player = Game::get()->getPlayer();

  reset();
  for (DynamicObject* object : objects)
  {
    if (object->isVisible() && (player->getDistance(object) < 20))
      targets.push_back(object);
  }
  if (targets.size() > 0)
    afterTargetUpdate();
}

void InteractionTargetList::afterTargetUpdate()
{
  Character* player = Game::get()->getPlayer();
  LevelTask* level = Game::get()->getLevel();

  if (level)
  {
    std::remove_if(targets.begin(), targets.end(), [level](DynamicObject* object)
    {
      QPoint pos        = level->getAdjustedOffsetFor(object);
      QPoint offset     = level->property("canvasOffset").toPoint();
      QSize  windowSize = MouseCursor::get()->windowSize();

      pos = pos + offset;
      return pos.x() < 0 || pos.y() < 0 || pos.x() > windowSize.width() || pos.y() > windowSize.height();
    });
    std::sort(targets.begin(), targets.end(), [player](DynamicObject* a, DynamicObject* b) { return player->getDistance(a) < player->getDistance(b); });
    cursor = static_cast<unsigned int>(targets.size()) - 1;
    emit targetsUpdated();
  }
}
