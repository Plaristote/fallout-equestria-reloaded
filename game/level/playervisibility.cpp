#include "playervisibility.h"

PlayerVisibilityComponent::PlayerVisibilityComponent(QObject* parent) : ParentType(parent)
{

};

void PlayerVisibilityComponent::load()
{
  ParentType::load();
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
  return QQmlListProperty<Character>(this, &visibleCharacters);
}

void PlayerVisibilityComponent::refreshHiddenObjectsDetection()
{
  Character* player = getPlayer();
  float      radius = player->getFieldOfView()->GetRadius();
  const auto hiddenObjects = findDynamicObjects([player, radius](DynamicObject& candidate) -> bool
  {
    return candidate.isHidden() && !candidate.isCharacter() && player->getDistance(&candidate) <= radius;
  });
  for (auto* hiddenObject : hiddenObjects)
    hiddenObject->tryDetection(player);
}
