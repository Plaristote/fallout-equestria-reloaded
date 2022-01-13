#include "playervisibility.h"

PlayerVisibilityComponent::PlayerVisibilityComponent(QObject* parent) : ParentType(parent)
{
  connect(this, &GridComponent::floorChanged, this, &PlayerVisibilityComponent::visibleObjectsChanged,    Qt::QueuedConnection);
  connect(this, &GridComponent::floorChanged, this, &PlayerVisibilityComponent::visibleCharactersChanged, Qt::QueuedConnection);
  connect(this, &LevelBase::attachedObjectsChanged, this, &PlayerVisibilityComponent::visibleObjectsChanged,    Qt::QueuedConnection);
  connect(this, &LevelBase::attachedObjectsChanged, this, &PlayerVisibilityComponent::visibleCharactersChanged, Qt::QueuedConnection);
};

void PlayerVisibilityComponent::registerDynamicObject(DynamicObject* object)
{
  ParentType::registerDynamicObject(object);
  if (!object->isCharacter())
    emit visibleObjectsChanged();
}

void PlayerVisibilityComponent::unregisterDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
    visibleCharacters.removeOne(reinterpret_cast<Character*>(object));
  ParentType::unregisterDynamicObject(object);
}

void PlayerVisibilityComponent::load(const QJsonObject& data)
{
  ParentType::load(data);
  if (isGameEditor())
    connect(this, &LevelBase::objectsChanged, this, &PlayerVisibilityComponent::visibleCharactersChanged);
  else
  {
    FieldOfView* fov = getPlayer()->getFieldOfView();
    connect(fov, &FieldOfView::refreshed, this, &PlayerVisibilityComponent::visibleCharactersChanged);
    connect(fov, &FieldOfView::refreshed, this, &PlayerVisibilityComponent::refreshHiddenObjectsDetection);
  }
}

QQmlListProperty<Character> PlayerVisibilityComponent::getQmlVisibleCharacters()
{
  const auto list = getPlayer()->getFieldOfView()->GetDetectedCharacters();

  visibleCharacters.clear();
  visibleCharacters.reserve(list.size());
  for (Character* character : list)
  {
    if (character->getCurrentFloor() == getCurrentFloor())
      visibleCharacters << character;
  }
  visibleCharacters << getPlayer();
  sortByRenderOrder(visibleCharacters);
  std::reverse(visibleCharacters.begin(), visibleCharacters.end());
  return QML_QLIST_CONSTRUCTOR(Character, visibleCharacters);
}

QQmlListProperty<DynamicObject> PlayerVisibilityComponent::getQmlVisibleObjects()
{
  visibleObjects.clear();
  eachObject([this](DynamicObject* object)
  {
    if (!object->isCharacter() && object->getCurrentFloor() == getCurrentFloor() && !object->isHidden())
      visibleObjects << object;
  });
  sortByRenderOrder(visibleObjects);
  std::reverse(visibleObjects.begin(), visibleObjects.end());
  return QML_QLIST_CONSTRUCTOR(DynamicObject, visibleObjects);
}

void PlayerVisibilityComponent::refreshHiddenObjectsDetection()
{
  bool       withDetection = false;
  Character* player = getPlayer();
  float      radius = player->getFieldOfView()->GetRadius();
  const auto hiddenObjects = findDynamicObjects([player, radius](DynamicObject& candidate) -> bool
  {
    return candidate.isHidden()
        && candidate.isSneaking()
        && !candidate.isCharacter()
        && player->getDistance(&candidate) <= radius
        && player->hasLineOfSight(&candidate);
  });
  for (auto* hiddenObject : hiddenObjects)
    withDetection = hiddenObject->tryDetection(player) || withDetection;
  if (withDetection)
    emit visibleObjectsChanged();
}
