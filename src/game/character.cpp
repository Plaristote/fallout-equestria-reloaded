#include "character.h"
#include "game.h"
#include "leveltask.h"
#include "characters/actionqueue.h"
#include "cmap/race.h"
#include <cmath>

Character::Character(QObject *parent) : ParentType(parent)
{
  dices = new BalancedDiceRoller(this);
  setProperty("float", true);
  actionQueue = new ActionQueue(this);
  inventory->setUser(this);
  connect(actionQueue, &ActionQueue::queueCompleted, this, &Character::onActionQueueCompleted);
  connect(this, &Character::characterKill, this, &Character::died);
  connect(this, &Character::died, [this]() { scriptCall("onDied"); });
  connect(this, &Character::died, this, &CharacterBuffs::clearBuffs, Qt::QueuedConnection);
  connect(this, &Character::died, this, &Character::blocksPathChanged);
  connect(this, &Character::died, getFieldOfView(), &FieldOfView::reset, Qt::QueuedConnection);
  connect(this, &Character::died, this, &DynamicObject::zIndexChanged);
  connect(this, &Sprite::animationFinished, this, &Character::afterDeathAnimation);
}

void Character::update(qint64 delta)
{
  ParentType::update(delta);
  if (isAlive())
  {
    auto* level = LevelTask::get();

    if (level && shouldJoinFight())
      level->joinCombat(this);
  }
}

bool Character::shouldJoinFight() const
{
  auto* level = LevelTask::get();
  bool canJoin = attacksOnSight || (level && level->alreadyInCombat());

  return canJoin && !unconscious && morale > 0 && hasLivingEnemiesInSight();
}

void Character::onActionQueueCompleted()
{
  scriptCall("onActionQueueCompleted");
}

void Character::afterDeathAnimation()
{
  QString animationName = getAnimation();

  if ((animationName.startsWith("fall") || animationName.startsWith("death")) && !isAlive())
  {
    unequipUseSlots();
    setAnimation("dead");
    LevelTask::get()->addBloodStainAt(getPosition(), static_cast<unsigned char>(getCurrentFloor()));
  }
  else if (getAnimation().startsWith("get-up"))
    setAnimation("idle");
}

void Character::onIdle()
{
  if (isUnconscious() || !isAlive())
    setAnimation("dead");
  else
    ParentType::onIdle();
}

void Character::takeMitigableDamage(int damage, const QString& type, Character* dealer)
{
  if (isAlive())
  {
    QJSValueList params = QJSValueList() << damage << type;
    QJSValue result;

    if (dealer)
      params << dealer->asJSValue();
    result = scriptCall("mitigateDamage", QJSValueList() << damage << type << dealer->asJSValue());
    if (result.isNumber())
      damage = result.toInt();
    takeDamage(damage, dealer);
  }
}

void Character::takeDamage(int damage, Character* dealer)
{
  if (isAlive())
  {
    auto hp = getStatistics()->getHitPoints() - damage;

    if (hasAnimation("damaged"))
      setAnimation("damaged");
    getStatistics()->setHitPoints(hp);
    if (hp <= 0)
    {
      if (!setDeathAnimation())
        afterDeathAnimation();
      blocksPath = false;
      emit characterKill(this, dealer);
      emit blocksPathChanged();
    }
    else
      attackedBy(dealer);
    if (hp > 0)
    {
      QJSValueList args = QJSValueList() << damage;

      if (dealer)
        args << dealer->asJSValue();
      scriptCall("onDamageTaken", args);
    }
  }
}

void Character::attackedBy(Character* dealer)
{
  if (dealer != nullptr && !isAlly(dealer))
  {
    if (!isEnemy(dealer))
      setAsEnemy(dealer);
    getFieldOfView()->setEnemyDetected(dealer);
    emit requireJoinCombat();
  }
}

int Character::getInteractionDistance() const
{
  return isAlive() ? 1 : 0;
}

int Character::getSneakAbility() const
{
  return statistics ? statistics->get_sneak() : DetectableComponent::getSneakAbility();
}

void Character::moveAway(Character* target)
{
  auto* level = LevelTask::get();
  QVector<QPoint> candidates = getAvailableSurroundingCases();

  if (!actionQueue->isEmpty() || (level->isInCombat(this) && getActionPoints() == 0))
    return ;
  std::sort(candidates.begin(), candidates.end(), [target](QPoint a, QPoint b)
  {
    return target->getDistance(a) > target->getDistance(b);
  });
  if (candidates.length() > 0)
  {
    QPoint destination = candidates.first();

    actionQueue->reset();
    actionQueue->pushMovement(destination.x(), destination.y());
    actionQueue->start();
  }
}

QString Character::getDialogName()
{
  return script ? script->property("dialog").toString() : "";
}

unsigned int Character::getXpValue() const
{
  return script ? script->property("xpValue").toUInt() : 25;
}

bool Character::useActionPoints(int amount, const QString& actionType)
{
  auto* level = LevelTask::get();

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

        updateFieldOfView(duration);
      }
      return true;
    }
    return false;
  }
  return isAlive();
}

void Character::fallAwayFrom(int x, int y, int distance)
{
  int directionFlag = 0;

  if (x > position.x())
    directionFlag |= LeftDir;
  else if (x < position.x())
    directionFlag |= RightDir;
  if (y > position.y())
    directionFlag |= UpperDir;
  else if (y < position.y())
    directionFlag |= BottomDir;
  fall(distance, static_cast<Direction>(directionFlag));
}

void Character::fall(int distance, const QString &directionName)
{
  if (distance && directionByName.contains(directionName))
  {
    Direction direction = directionByName[directionName];

    fall(distance, direction);
  }
  else
    setFallAnimation();
}

void Character::fall(int distance, Direction direction)
{
  QPoint from   = getPosition();
  QPoint target = from;

  if (direction != NoDir)
  {
    auto* level = LevelTask::get();
    auto* grid  = level->getGrid();

    while (distance > 0)
    {
      QPoint candidate = target;
      auto*  currentCase = grid->getGridCase(candidate.x(), candidate.y());

      if (direction & UpperDir)
        candidate.ry() -= 1;
      else if (direction & BottomDir)
        candidate.ry() += 1;
      if (direction & LeftDir)
        candidate.rx() -= 1;
      else if (direction & RightDir)
        candidate.rx() += 1;
      if (currentCase && !currentCase->isLinkedTo(candidate))
        break ;
      target = candidate;
      distance--;
    }
  }
  actionQueue->reset();
  actionQueue->pushSliding(target);
  actionQueue->start();
}

void Character::fallUnconscious()
{
  if (!unconscious)
  {
    unconscious = true;
    actionPoints = 0;
    setFallAnimation();
    emit actionPointsChanged();
    emit unconsciousChanged();
  }
}

void Character::wakeUp()
{
  if (unconscious)
  {
    unconscious = false;
    setAnimation("get-up");
    emit unconsciousChanged();
  }
}

bool Character::setFallAnimation()
{
  if (!getAnimation().startsWith("fall"))
  {
    if (hasAnimation("fall"))
    {
      setAnimation("fall");
      scriptCall("onFell");
      return true;
    }
  }
  return false;
}

bool Character::setDeathAnimation()
{
  if (!getAnimation().startsWith("death"))
  {
    if (hasAnimation("death"))
    {
      setAnimation("death");
      return true;
    }
    return setFallAnimation();
  }
  return false;
}

void Character::resetActionPoints()
{
  actionPoints = isAlive() ? getStatistics()->get_actionPoints() : 0;
  emit actionPointsChanged();
}

void Character::load(const QJsonObject& data)
{
  actionPoints   = data["ap"].toInt();
  unconscious    = !(data["ko"].toBool(true));
  morale         = data["morale"].toInt(CHARACTER_MAX_MORALE);
  attacksOnSight = data["attacksOnSight"].toBool(true);
  ParentType::load(data);
}

void Character::save(QJsonObject& data) const
{
  data["ap"] = actionPoints;
  if (unconscious)
    data["ko"] = unconscious;
  if (morale != CHARACTER_MAX_MORALE)
    data["morale"] = morale;
  if (!attacksOnSight)
    data["attacksOnSight"] = attacksOnSight;
  ParentType::save(data);
}

QJSValue Character::getActions()
{
  if (jsActionQueue.isUndefined())
    jsActionQueue = Game::get()->getScriptEngine().newQObject(getActionQueue());
  return jsActionQueue;
}
