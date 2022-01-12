#include "interaction.h"
#include "game.h"
#include "../characterdialog.h"
#include "../lootingcontroller.h"
#include "game/characters/actionqueue.h"
#include "game/mousecursor.h"
#include "game/spell.h"
#include <QDebug>

int InteractionComponent::movementModeOption = InteractionComponent::MixedMovementMode;

InteractionComponent::InteractionComponent(QObject *parent) : ParentType(parent)
{
  connect(this, &InteractionComponent::mouseModeChanged,   this, &InteractionComponent::mouseStateChanged);
  connect(this, &InteractionComponent::mouseStateChanged,  MouseCursor::get(), &MouseCursor::updatePointerType);
  connect(this, &InteractionComponent::hoveredTileChanged, MouseCursor::get(), &MouseCursor::updatePointerType);
}

void InteractionComponent::registerDynamicObject(DynamicObject* object)
{
  ParentType::registerDynamicObject(object);
}

void InteractionComponent::unregisterDynamicObject(DynamicObject* object)
{
  targetList.unregisterDynamicObject(object);
  if (object->isCharacter())
    reinterpret_cast<Character*>(object)->getActionQueue()->reset();
  ParentType::unregisterDynamicObject(object);
}

bool InteractionComponent::openInteractionMenu(DynamicObject* object)
{
  auto entries = object->getAvailableInteractions();

  if (entries.length() > 0)
  {
    //swapMouseMode();
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

void InteractionComponent::setDefaultMovementMode()
{
  switch (movementModeOption)
  {
    case RunMovementMode:
      getPlayer()->setMovementMode("running");
      break ;
    case WalkMovementMode:
      getPlayer()->setMovementMode("walking");
      break ;
  }
}

void InteractionComponent::interactOrderReceived(DynamicObject* target, const QString& interactionType)
{
  int   distance = getInteractionDistance(target, interactionType);
  auto* player = Game::get()->getPlayer();
  auto* actions = player->getActionQueue();

  setDefaultMovementMode();
  actions->reset();
  actions->pushReach(target, static_cast<float>(distance));
  actions->pushInteraction(target, interactionType);
  actions->start();
  if (!actions->isEmpty())
    swapMouseMode();
}

void InteractionComponent::swapMouseMode()
{
  switch (mouseMode)
  {
    case WaitCursor:
      return ;
    case InteractionCursor:
    case TargetCursor:
      targetList.reset();
      mouseMode = MovementCursor;
      break ;
    default:
      targetList.findNearbyTargets(allDynamicObjects());
      mouseMode = InteractionCursor;
      break ;
  }
  resetInteractionMode();
  emit mouseModeChanged();
}

void InteractionComponent::resetInteractionMode()
{
  interactionType = NoInteraction;
  activeSkill     = "";
  if (activeItem)
  {
    activeItem    = nullptr;
    emit activeItemChanged();
  }
}

void InteractionComponent::enableWaitingMode(bool active)
{
  if ((mouseMode != WaitCursor && active) || (mouseMode == WaitCursor && !active))
  {
    mouseMode = active ? WaitCursor : MovementCursor;
    emit mouseModeChanged();
  }
}

void InteractionComponent::setActiveItem(const QString& slotName)
{
  resetInteractionMode();
  mouseMode = TargetCursor;
  interactionType = ItemUse;
  activeItemSlot = slotName;
  activeItem = Game::get()->getPlayer()->getInventory()->getEquippedItem(slotName);
  emit activeItemChanged();
  emit mouseModeChanged();
  onActiveItemChanged();
}

void InteractionComponent::onActiveItemChanged()
{
  if (activeItem)
    qDebug() << "InteractionComponent::onActiveItemChanged" << activeItem->getItemType() << ": " << activeItem->requiresTarget();
  if (activeItem && !activeItem->requiresTarget())
    useItemOn(nullptr);
  else if (activeItem)
    targetList.findItemTargets(activeItem, visibleObjects, visibleCharacters);
}

int InteractionComponent::getTargetMode() const
{
  switch (interactionType)
  {
  case ItemUse:
    if (activeItem->usesZoneTarget())
      return ZoneTarget;
    else if (activeItem->getCategory() == "weapon")
      return CharacterTarget;
    break ;
  case SpellUse:
    return Spell::requireSpell(activeSkill).getTargetMode();
  case SkillUse:
  case NoInteraction:
    break ;
  }
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
    switch (interactionType)
    {
    case ItemUse:
      useItemOn(object);
      break ;
    case SkillUse:
      useSkillOn(Game::get()->getPlayer(), object, activeSkill);
      break ;
    case SpellUse:
      useSpellOn(Game::get()->getPlayer(), object, activeSkill);
      break ;
    }
    resetInteractionMode();
    break ;
  }
}

void InteractionComponent::useSpell(const QString &spellName)
{
  auto& spell = Spell::requireSpell(spellName);
  auto* actions = getPlayer()->getActionQueue();

  resetInteractionMode();
  switch (spell.getTargetMode())
  {
  case AnyTarget:
    actions->reset();
    actions->pushSpellUse(spellName);
    if (actions->start())
      swapMouseMode();
    break ;
  case CharacterTarget:
  case ZoneTarget:
    interactionType = SpellUse;
    mouseMode = TargetCursor;
    activeSkill = spellName;
    targetList.findNearbyTargets(allDynamicObjects());
    emit mouseModeChanged();
    break ;
  }
}

void InteractionComponent::useSkill(const QString &skill)
{
  if (skill == "sneak")
    useSneak(getPlayer());
  else
  {
    interactionType = SkillUse;
    activeSkill = skill;
    mouseMode = TargetCursor;
    activeItem = nullptr;
    targetList.findNearbyTargets(allDynamicObjects());
    emit activeItemChanged();
    emit mouseModeChanged();
  }
}

void InteractionComponent::useSpellOn(Character* user, DynamicObject* target, const QString &spellName)
{
  auto* actions = user->getActionQueue();

  actions->reset();
  actions->pushSpellUseOn(target, spellName);
  if (actions->start())
    swapMouseMode();
}

void InteractionComponent::useSpellAt(Character* user, int posX, int posY, const QString &spellName)
{
  auto* actions = user->getActionQueue();

  actions->reset();
  actions->pushSpellUseAt(posX, posY, spellName);
  if (actions->start())
    swapMouseMode();
}

bool InteractionComponent::canSneak(Character* user)
{
  return findCharacters([user](Character& other)
  {
    if (other.isAlive() && !other.isAlly(user))
    {
      float radius   = other.getFieldOfView()->GetRadius();
      float distance = other.getDistance(user);

      return distance <= radius && other.hasLineOfSight(user);
    }
    return false;
  }).size() == 0;
}

bool InteractionComponent::useSneak(Character* user)
{
  if (canSneak(user))
  {
    user->toggleSneaking(true);
    return true;
  }
  else if (user == getPlayer())
    Game::get()->appendToConsole(I18n::get()->t("messages.cannot-sneak"));
  return false;
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
    useItemOn(getPlayer(), activeItem, target);
  else
    qDebug() << "InteractionComponent::useItemOn: activeItem is null";
}

void InteractionComponent::useItemOn(InventoryItem *item, DynamicObject *target, const QString& useMode)
{
  DynamicObject* owner = item->getOwner();

  if (owner && owner->isCharacter())
    useItemOn(reinterpret_cast<Character*>(owner), item, target, useMode);
}

void InteractionComponent::useItemOn(Character* user, InventoryItem* item, DynamicObject* target, const QString& useMode)
{
  auto* actions = user->getActionQueue();

  actions->reset();
  if (target && (!item->isInRange(target) || !user->hasLineOfSight(target)))
    actions->pushReach(target, item->getRange());
  actions->pushItemUse(target, item, useMode);
  if (actions->start())
    swapMouseMode();
}

void InteractionComponent::useItemAt(int x, int y)
{
  if (activeItem)
    useItemAt(getPlayer(), activeItem, x, y);
  else
    qDebug() << "InteractionComponent::useItemAt: activeItem is null";
}

void InteractionComponent::useItemAt(InventoryItem *item, int x, int y)
{
  DynamicObject* owner = item->getOwner();

  if (owner && owner->isCharacter())
    useItemAt(reinterpret_cast<Character*>(owner), item, x, y);
}

void InteractionComponent::useItemAt(Character *user, InventoryItem *item, int x, int y)
{
  auto* actions = user->getActionQueue();

  actions->reset();
  if (user->getDistance(QPoint(x, y)) > item->getRange())
    actions->pushReachCase(x, y, item->getRange());
  actions->pushItemUseAt(x, y, item);
  if (actions->start())
    swapMouseMode();
}

void InteractionComponent::initializeDialog(Character* npc)
{
  initializeDialog(npc, npc->getDialogName());
}

void InteractionComponent::initializeDialog(DynamicObject* object, const QString& dialogName)
{
  if (object)
  {
    Character*       player = Game::get()->getPlayer();
    CharacterDialog* dialog = new CharacterDialog(this);

    player->lookAt(object);
    if (object->isCharacter())
    {
      Character* npc = reinterpret_cast<Character*>(object);

      npc->lookAt(player);
      player->getFieldOfView()->setCharacterDetected(npc);
    }
    if (dialog->load(dialogName, player, object))
      emit startDialog(dialog);
    else
      delete dialog;
  }
  else
    qDebug() << "/!\\ Called level.initializeDialog with invalid parameters.";
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
  QVector<DynamicObject*> list;

  list.reserve(objectCount());
  eachObject([this, &list](DynamicObject* object)
  {
    if (object->getCurrentFloor() == getCurrentFloor() && (object->isCharacter() || object->hasInteractionOverlay()))
      list.push_back(object);
  });
  sortByRenderOrder(list);
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

QPoint InteractionComponent::getClickableOffsetFor(const DynamicObject *target) const
{
  QPoint position = getAdjustedOffsetFor(target);
  const QImage& image = target->getImage();

  for (int x = 0 ; x < image.width() ; ++x)
  {
    for (int y = 0 ; y < image.height() ; ++y)
    {
      QPoint pixelPosition = position + QPoint(x, y);

      if (image.pixelColor(x, y) != Qt::transparent && getObjectAt(pixelPosition) == target)
        return pixelPosition;
    }
  }
  return position;
}

void InteractionComponent::centerCursorOn(DynamicObject *object)
{
  if (object)
  {
    QPoint position = canvasOffset + getClickableOffsetFor(object);

    MouseCursor::get()->setRelativePosition(position);
  }
}

void InteractionComponent::movePlayerTo(int x, int y)
{
  DynamicObject* occupant = getFloorGrid(getPlayer()->getCurrentFloor())->getOccupant(x, y);
  auto* actions = getPlayer()->getActionQueue();
  QPoint oldTarget(-1, -1);

  if (getPlayer()->getCurrentPath().length() > 0)
    oldTarget = getPlayer()->getCurrentPath().last();
  actions->reset();
  if (occupant)
    actions->pushReach(occupant, 1);
  else
    actions->pushMovement(QPoint(x, y));
  if (!(actions->start()))
    Game::get()->appendToConsole(I18n::get()->t("no-path"));
  else if (!getPlayer()->getCurrentPath().empty())
  {
    switch (movementModeOption)
    {
      case MixedMovementMode:
        getPlayer()->setMovementMode(oldTarget == getPlayer()->getCurrentPath().last() ? "running" : "walking");
        break ;
      default:
        setDefaultMovementMode();
        break ;
    }
    emit playerMovingTo(getPlayer()->getCurrentPath().last());
  }
}

void InteractionComponent::targetTileClicked(int x, int y)
{
  switch (interactionType)
  {
  case ItemUse:
    useItemAt(x, y);
    break ;
  case SpellUse:
    useSpellAt(getPlayer(), x, y, activeSkill);
    break ;
  default:
    break ;
  }
}

void InteractionComponent::tileClicked(int x, int y)
{
  switch (mouseMode)
  {
  case MovementCursor:
    movePlayerTo(x, y);
    break ;
  case TargetCursor:
    if (getTargetMode() == ZoneTarget)
      targetTileClicked(x, y);
    break ;
  default:
    break ;
  }
}
