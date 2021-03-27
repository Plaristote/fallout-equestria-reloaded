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
    connect(getPlayer()->getFieldOfView(), &FieldOfView::refreshed, this, &PlayerVisibilityComponent::visibleCharactersChanged);
}

QQmlListProperty<Character> PlayerVisibilityComponent::getQmlVisibleCharacters()
{
  visibleCharacters = getPlayer()->getFieldOfView()->GetDetectedCharacters();
  visibleCharacters << getPlayer();
  return QQmlListProperty<Character>(this, &visibleCharacters);
}
