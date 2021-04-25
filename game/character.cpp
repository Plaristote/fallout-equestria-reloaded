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
  if (getAnimation().startsWith("fall"))
  {
    if (!isAlive())
    {
      setAnimation("dead");
      Game::get()->getLevel()->addBloodStainAt(getPosition());
    }
    else if (isUncounscious())
      Game::get()->getLevel()->getGrid()->triggerZone(this, getPosition().x(), getPosition().y());
    else
      setAnimation("get-up");
  }
  else if (getAnimation().startsWith("get-up"))
    Game::get()->getLevel()->getGrid()->triggerZone(this, getPosition().x(), getPosition().y());
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
    setAnimation("fall");
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

void Character::fall(int distance, const QString &directionName)
{
  if (distance && directionByName.contains(directionName))
  {
    QPoint    from      = getPosition();
    QPoint    target    = from;
    Direction direction = directionByName[directionName];
    auto*     level     = Game::get()->getLevel();
    auto*     grid      = level->getGrid();

    while (distance > 0)
    {
      QPoint candidate;

      switch (direction)
      {
      case UpperDir:
        candidate = QPoint(target.x(), target.y() - 1);
        break ;
      case UpperLeftDir:
        candidate = QPoint(target.x() - 1, target.y() - 1);
        break ;
      case UpperRightDir:
        candidate = QPoint(target.x() + 1, target.y() - 1);
        break ;
      case LeftDir:
        candidate = QPoint(target.x() - 1, target.y());
        break ;
      case RightDir:
        candidate = QPoint(target.x() + 1, target.y());
        break ;
      case BottomDir:
        candidate = QPoint(target.x(), target.y() + 1);
        break ;
      case BottomLeftDir:
        candidate = QPoint(target.x() - 1, target.y() + 1);
        break ;
      case BottomRightDir:
        candidate = QPoint(target.x() + 1, target.y() + 1);
        break ;
      }
      if (grid->isOccupied(candidate.x(), candidate.y()))
        break ;
      target = candidate;
    }
    moveToCoordinates(level->getRenderPositionForTile(target.x(), target.y()));
    grid->moveObject(this, target.x(), target.y());
    lookTo(from);
  }
  setAnimation("fall");
}

void Character::resetActionPoints()
{
  actionPoints = isAlive() ? getStatistics()->get_actionPoints() : 0;
  emit actionPointsChanged();
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
