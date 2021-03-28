#include "interaction.h"
#include "game.h"
#include "../characterdialog.h"
#include "../lootingcontroller.h"
#include "game/characters/actionqueue.h"

InteractionComponent::InteractionComponent(QObject *parent) : ParentType(parent)
{

}

void InteractionComponent::registerDynamicObject(DynamicObject* object)
{
  ParentType::registerDynamicObject(object);
}

void InteractionComponent::unregisterDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
    reinterpret_cast<Character*>(object)->getActionQueue()->reset();
  ParentType::unregisterDynamicObject(object);
}

bool InteractionComponent::openInteractionMenu(DynamicObject* object)
{
  auto entries = object->getAvailableInteractions();

  if (entries.length() > 0)
  {
    swapMouseMode();
    emit interactionRequired(object, entries);
    return true;
  }
  else
    qDebug() << "no interaction available for object" << object->getObjectName();
  return false;
}

void InteractionComponent::openCountdownDialog(InventoryItem *item)
{
  emit countdownRequired(item);
}

int InteractionComponent::getInteractionDistance(DynamicObject* target, const QString& interactionType)
{
  int distance;

  if (interactionType == "look")
  {
    int perception = getPlayer()->getStatistics()->property("perception").toInt();

    distance = perception;
  }
  else if (interactionType == "push" || interactionType == "talk-to")
    distance = 3;
  else
    distance = target->getInteractionDistance();
  return distance;
}

void InteractionComponent::interactOrderReceived(DynamicObject* target, const QString& interactionType)
{
  int   distance = getInteractionDistance(target, interactionType);
  auto* player = Game::get()->getPlayer();
  auto* actions = player->getActionQueue();

  actions->reset();
  actions->pushReach(target, static_cast<float>(distance));
  actions->pushInteraction(target, interactionType);
  actions->start();
}

void InteractionComponent::swapMouseMode()
{
  switch (mouseMode)
  {
    case InteractionCursor:
    case TargetCursor:
      mouseMode = MovementCursor;
      break ;
    default:
      mouseMode = InteractionCursor;
      break ;
  }
  if (activeItem) {
    activeItem = nullptr;
    emit activeItemChanged();
  }
  activeSkill = "";
  emit mouseModeChanged();
}

void InteractionComponent::setActiveItem(const QString& slotName)
{
  mouseMode = TargetCursor;
  activeItemSlot = slotName;
  activeItem = Game::get()->getPlayer()->getInventory()->getEquippedItem(slotName);
  activeItem->getItemType();
  activeSkill = "";
  emit activeItemChanged();
  emit mouseModeChanged();
  onActiveItemChanged();
}

void InteractionComponent::onActiveItemChanged()
{
  if (activeItem)
    qDebug() << "InteractionComponent::onActiveItemChanged" << activeItem->getItemType() << ": " << activeItem->requiresTarget();
  if (activeItem && !activeItem->requiresTarget())
    useItemOn(getPlayer());
}

int InteractionComponent::getTargetMode() const
{
  if (activeItem && activeItem->getItemType() == "weapon")
    return CharacterTarget;
  return AnyTarget;
}

void InteractionComponent::objectClicked(DynamicObject* object)
{
  switch (mouseMode)
  {
  case InteractionCursor:
    openInteractionMenu(object);
    break ;
  case TargetCursor:
    if (activeItem)
      useItemOn(object);
    else if (activeSkill.length() > 0)
    {
      useSkillOn(Game::get()->getPlayer(), object, activeSkill);
      activeSkill = "";
    }
    else
      qDebug() << "TODO missing behaviour for target cursor";
    break ;
  }
}

void InteractionComponent::useSkill(const QString &skill)
{
  if (skill == "sneak")
    qDebug() << "TODO: implement sneak skill";
  else
  {
    activeSkill = skill;
    mouseMode = TargetCursor;
    activeItem = nullptr;
    emit activeItemChanged();
    emit mouseModeChanged();
  }
}

void InteractionComponent::useSkillOn(Character* user, DynamicObject* target, const QString &skill)
{
  int distance = target->getInteractionDistance();
  auto* actions = user->getActionQueue();

  qDebug() << "Useskillon distance =" << distance;
  actions->reset();
  actions->pushReach(target, static_cast<float>(distance));
  actions->pushSkillUse(target, skill);
  if (actions->start())
    swapMouseMode();
}

void InteractionComponent::useItemOn(DynamicObject* target)
{
  if (activeItem)
    useItemOn(activeItem, target);
  else
    qDebug() << "InteractionComponent::useItemOn: activeItem is null";
}

void InteractionComponent::useItemOn(InventoryItem *item, DynamicObject *target)
{
  auto* actions = Game::get()->getPlayer()->getActionQueue();

  actions->reset();
  if (!item->isInRange(target))
    actions->pushReach(target, item->getRange());
  actions->pushItemUse(target, item);
  if (actions->start())
    swapMouseMode();
}

void InteractionComponent::initializeDialog(Character* npc)
{
  Character*       player = Game::get()->getPlayer();
  CharacterDialog* dialog = new CharacterDialog(this);

  if (dialog->load(npc->getDialogName(), player, npc))
    emit startDialog(dialog);
  else
    delete dialog;
}

void InteractionComponent::initializeLooting(StorageObject* target)
{
  Character* player = Game::get()->getPlayer();
  LootingController* controller = new LootingController(this);

  controller->initialize(player, target);
  emit startLooting(controller);
}

void InteractionComponent::pickUpItem(Character* character, InventoryItem* item)
{
  Inventory* inventory = character->getInventory();
  StatModel* statistics = character->getStatistics();

  if (inventory->getTotalWeight() + item->getWeight() <= statistics->get_carryWeight())
  {
    Game::get()->getLevel()->unregisterDynamicObject(item);
    inventory->addItem(item);
  }
  else
    Game::get()->appendToConsole(I18n::get()->t("message.cannot-carry-more"));
}

DynamicObject* InteractionComponent::getObjectAt(int posX, int posY) const
{
  QVector<DynamicObject*> list(objects.begin(), objects.end());

  std::sort(list.begin(), list.end(), [](DynamicObject* a, DynamicObject* b)
  {
    QPoint posA = a->getPosition();
    QPoint posB = b->getPosition();

    if (posA.x() == posB.x())
      return posA.y() > posB.y();
    return posA.x() > posB.x();
  });
  for (DynamicObject* object : qAsConst(list))
  {
    if (!object->isCharacter() || visibleCharacters.indexOf(reinterpret_cast<Character*>(object)) >= 0)
    {
      QPoint coordinates = getAdjustedOffsetFor(object);
      QRect  clip = object->getClippedRect();
      QRect  boundingBox(coordinates, clip.size());

      if (posX >= boundingBox.x() && posX <= boundingBox.x() + boundingBox.width() &&
          posY >= boundingBox.y() && posY <= boundingBox.y() + boundingBox.height())
      {
        QPoint collisionAt(posX - coordinates.x(), posY - coordinates.y());
        QPoint sheetPosition(clip.x() + collisionAt.x(), clip.y() + collisionAt.y());
        const QImage& image = object->getImage();

        if (image.pixelColor(sheetPosition) != Qt::transparent)
           return object;
      }
    }
  }
  return nullptr;
}
