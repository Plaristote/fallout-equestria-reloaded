#include "character.h"
#include "game.h"
#include "leveltask.h"
#include "characters/actionqueue.h"
#include "cmap/race.h"
#include <cmath>

Character::Character(QObject *parent) : ParentType(parent)
{
  setProperty("float", true);
  actionQueue = new ActionQueue(this);
  inventory->setUser(this);
  connect(actionQueue, &ActionQueue::queueCompleted, this, &Character::onActionQueueCompleted);
  connect(this, &Character::characterKill, this, &Character::died);
  connect(this, &Character::died, [this]() { if (script) { script->call("onDied"); } });
  connect(this, &Character::died, this, &CharacterBuffs::clearBuffs, Qt::QueuedConnection);
  connect(this, &Sprite::animationFinished, this, &Character::afterDeathAnimation);
}

void Character::update(qint64 delta)
{
  ParentType::update(delta);
  if (isAlive())
  {
    auto* level = Game::get()->getLevel();

    if (level->getPlayer() != this && hasLivingEnemiesInSight())
      level->joinCombat(this);
  }
}

void Character::onActionQueueCompleted()
{
  if (script)
    script->call("onActionQueueCompleted");
}

void Character::afterDeathAnimation()
{
  if (getAnimation().startsWith("fall") && !isAlive())
  {
    setAnimation("dead");
    Game::get()->getLevel()->addBloodStainAt(getPosition());
  }
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
    if (!setFallAnimation())
      afterDeathAnimation();
    blocksPath = false;
    emit characterKill(this, dealer);
    emit blocksPathChanged();
  }
  else
    attackedBy(dealer);
}

void Character::attackedBy(Character* dealer)
{
  if (dealer != nullptr && !isAlly(dealer) && !isEnemy(dealer))
  {
    setAsEnemy(dealer);
    emit requireJoinCombat();
  }
}

int Character::getInteractionDistance() const
{
  return isAlive() ? 1 : 0;
}

QVector<QPoint> Character::getAvailableSurroundingCases() const
{
  QVector<QPoint> candidates;
  auto* grid = Game::get()->getLevel()->getGrid();

  for (int x = position.x() - 1 ; x <= position.x() + 1 ; ++x)
  {
    for (int y = position.y() - 1 ; y <= position.y() + 1 ; ++y)
    {
      auto* caseContent = grid->getGridCase(x, y);

      if (caseContent && !caseContent->isBlocked())
        candidates << QPoint(x, y);
    }
  }
  return candidates;
}

void Character::moveAway(Character* target)
{
  auto* level = Game::get()->getLevel();
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

    actionQueue->pushMovement(destination.x(), destination.y());
    actionQueue->start();
    return ;
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

        updateFieldOfView(duration);
      }
      return true;
    }
    return false;
  }
  return isAlive();
}

static QMap<QString, CharacterMovement::Direction> directionByName = {
  {"up",           CharacterMovement::UpperDir},
  {"up-left",      CharacterMovement::UpperLeftDir},
  {"up-right",     CharacterMovement::UpperRightDir},
  {"left",         CharacterMovement::LeftDir},
  {"right",        CharacterMovement::RightDir},
  {"bottom",       CharacterMovement::BottomDir},
  {"bottom-left",  CharacterMovement::BottomLeftDir},
  {"bottom-right", CharacterMovement::BottomRightDir}
};

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
  QPoint    from      = getPosition();
  QPoint    target    = from;

  if (direction != NoDir)
  {
    auto*     level     = Game::get()->getLevel();
    auto*     grid      = level->getGrid();

    while (distance > 0)
    {
      QPoint candidate = target;

      if (direction & UpperDir)
        candidate.ry() -= 1;
      else if (direction & BottomDir)
        candidate.ry() += 1;
      if (direction & LeftDir)
        candidate.rx() -= 1;
      else if (direction & RightDir)
        candidate.rx() += 1;
      if (grid->isOccupied(candidate.x(), candidate.y()))
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
  }
}

void Character::wakeUp()
{
  unconscious = false;
  setAnimation("idle");
}

bool Character::setFallAnimation()
{
  if (getAnimation().startsWith("fall"))
  {
    setAnimation("fall");
    return true;
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
  actionPoints = data["ap"].toInt();
  unconscious  = !(data["ko"].toBool(false));
  ParentType::load(data);
}

void Character::save(QJsonObject& data) const
{
  data["ap"] = actionPoints;
  data["ko"] = !unconscious;
  ParentType::save(data);
}

QJSValue Character::getActions()
{
  if (jsActionQueue.isUndefined())
    jsActionQueue = Game::get()->getScriptEngine().newQObject(getActionQueue());
  return jsActionQueue;
}
