#include "character.h"
#include "game.h"
#include "leveltask.h"
#include "characters/actionqueue.h"
#include "cmap/race.h"
#include <cmath>

Character::Character(QObject *parent) : ParentType(parent)
{
  setProperty("float", true);
  fieldOfView = new FieldOfView(*this);
  actionQueue = new ActionQueue(this);
  inventory->setUser(this);
  connect(inventory, &Inventory::unequippedItem, this, &Character::initializeEmptySlot);
  connect(actionQueue, &ActionQueue::queueCompleted, this, &Character::onActionQueueCompleted);
  connect(this, &Character::characterKill, this, &Character::died);
  connect(this, &Character::died, [this]() { if (script) { script->call("onDied"); } });
}

void Character::update(qint64 delta)
{
  auto* level = Game::get()->getLevel();

  DynamicObject::update(delta);
  if (isAlive())
  {
    if (level->getPlayer() != this && fieldOfView->hasLivingEnemiesInSight())
      level->joinCombat(this);
  }
}

void Character::onActionQueueCompleted()
{
  if (script)
    script->call("onActionQueueCompleted");
}

void Character::takeDamage(int damage, Character* dealer)
{
  auto hp = getStatistics()->getHitPoints() - damage;

  setAnimation("damaged");
  getStatistics()->setHitPoints(hp);
  if (script && hp > 0)
  {
    QJSValueList args = QJSValueList() << damage;

    if (dealer)
      args << dealer->asJSValue();
    script->call("onDamageTaken", args);
  }
  if (hp <= 0)
  {
    setAnimation("death");
    blocksPath = false;
    emit characterKill(this, dealer);
    emit blocksPathChanged();
  }
  else if (dealer != nullptr && !isAlly(dealer) && !isEnemy(dealer))
  {
    setAsEnemy(dealer);
    emit requireJoinCombat();
  }
}

QPoint Character::getInteractionPosition() const
{
  if (isAlive())
  {
    auto* level = Game::get()->getLevel();

    if (level && level->getPlayer())
    {
      QList<QPoint> path;

      if (level->getGrid()->findPath(level->getPlayer()->getPosition(), getPosition(), path))
      {
        if (path.length() > 1)
          return path.at(path.length() - 2);
        return level->getPlayer()->getPosition();
      }
      qDebug() << "Character::getInteractionPosition: No path to reach character" << getObjectName();
    }
    else
      qDebug() << "Character::getInteractionPosition: called outside level";
  }
  return getPosition();
}

QString Character::getDialogName()
{
  return script ? script->property("dialog").toString() : "";
}

unsigned int Character::getXpValue() const
{
  return script ? script->property("xpValue").toUInt() : 25;
}

bool Character::hasLineOfSight(const Character* other) const
{
  auto* level = Game::get()->getLevel();

  if (level)
  {
    auto*  grid   = level->getGrid();
    QPoint target = other->getPosition();
    int    score  = grid->getVisionQuality(position.x(), position.y(), target.x(), target.y());

    return score > 0;
  }
  return false;
}

float Character::getDistance(const DynamicObject* target) const
{
  auto self  = getPosition();
  auto other = target->getPosition();
  auto a = self.x() - other.x();
  auto b = self.y() - other.y();

  return std::sqrt(static_cast<float>(a * a + b * b));
}

float Character::getDistance(DynamicObject* target) const
{
  auto self  = getPosition();
  auto other = target->getPosition();
  auto a = self.x() - other.x();
  auto b = self.y() - other.y();

  return std::sqrt(static_cast<float>(a * a + b * b));
}

bool Character::useActionPoints(int amount, const QString& actionType)
{
  auto* level = Game::get()->getLevel();

  if (level && level->isInCombat(this))
  {
    if (amount <= actionPoints)
    {
      actionPoints -= amount;
      emit actionPointsChanged();
      if (level->getPlayer() == this)
      {
        auto* stats = getStatistics();
        auto  maxActionPoints = stats->get_actionPoints();
        double duration = std::ceil(static_cast<double>(WORLDTIME_TURN_DURATION) / static_cast<double>(maxActionPoints) * static_cast<double>(amount));

        fieldOfView->update(duration);
      }
      return true;
    }
    return false;
  }
  return isAlive();
}

void Character::resetActionPoints()
{
  actionPoints = isAlive() ? getStatistics()->get_actionPoints() : 0;
  emit actionPointsChanged();
}

void Character::updateInventorySlots()
{
  QMap<QString, QString> slotTypes({{"armor", "armor"},{"saddle","saddle"},{"use-1","any"},{"use-2","any"}});

  inventory->setSlots(slotTypes);
}

void Character::initializeEmptySlots()
{
  initializeEmptySlot("use-1");
  initializeEmptySlot("use-2");
}

void Character::initializeEmptySlot(const QString& slotName)
{
  if (inventory->getEquippedItem(slotName) == nullptr)
  {
    InventoryItem* item = new InventoryItem(this);

    item->setObjectName(getDefaultItemForSlot(slotName));
    item->setItemType(item->getObjectName());
    item->setVirtual(true);
    inventory->equipItem(item, slotName);
  }
}

QString Character::getDefaultItemForSlot(const QString& name)
{
  if (script && script->hasMethod("getDefaultItem"))
    return script->call("getDefaultItem", QJSValueList() << name).toString();
  return "melee";
}

void Character::setScript(const QString& name)
{
  DynamicObject::setScript(name);
  updateInventorySlots();
  initializeEmptySlots();
}

void Character::load(const QJsonObject& data)
{
  actionPoints = data["ap"].toInt();
  ParentType::load(data);
}

void Character::save(QJsonObject& data) const
{
  data["ap"] = actionPoints;
  ParentType::save(data);
}

QJSValue Character::getActions()
{
  if (jsActionQueue.isUndefined())
    jsActionQueue = Game::get()->getScriptEngine().newQObject(getActionQueue());
  return jsActionQueue;
}
