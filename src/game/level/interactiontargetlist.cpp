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

void InteractionTargetList::findNearbyTargets(const QVector<DynamicObject*> objects)
{
  Character* player = Game::get()->getPlayer();

  reset();
  for (DynamicObject* object : objects)
  {
    bool hasOverlay        = object->hasInteractionOverlay();
    bool isHiddenLevel     = object->getCurrentFloor() != player->getCurrentFloor();
    bool isHiddenCharacter = isHiddenLevel || (object->isCharacter() && !player->getFieldOfView()->isDetected(reinterpret_cast<Character*>(object)));
    bool isHiddenObject    = isHiddenCharacter || object->isHidden();

    if (!isHiddenObject && (hasOverlay || object->isCharacter()) && player->getDistance(object) < 20)
      targets.push_back(object);
  }
  if (targets.size() > 0)
    afterTargetUpdate();
}

template<typename LIST>
void remove_matching_entries(LIST& list, std::function<bool (typename LIST::value_type)> callback)
{
  auto it = list.begin();

  while (it != list.end())
  {
    if (callback(*it))
      it = list.erase(it);
    else
      ++it;
  }
}

void InteractionTargetList::afterTargetUpdate()
{
  auto* game = Game::get();
  Character* player = game->getPlayer();
  LevelTask* level = game->getLevel();

  if (level)
  {
    remove_matching_entries(targets, [level](DynamicObject* object)
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
