#include "playervisibility.h"

PlayerVisibilityComponent::PlayerVisibilityComponent(QObject* parent) : ParentType(parent)
{

};

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
  visibleCharacters = getPlayer()->getFieldOfView()->GetDetectedCharacters();
  visibleCharacters << getPlayer();
  sortByRenderOrder(visibleCharacters);
  std::reverse(visibleCharacters.begin(), visibleCharacters.end());
  return QML_QLIST_CONSTRUCTOR(Character, visibleCharacters);
}

void PlayerVisibilityComponent::refreshHiddenObjectsDetection()
{
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
    hiddenObject->tryDetection(player);
}
