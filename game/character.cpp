#include "character.h"
#include "game.h"
#include "leveltask.h"
#include "characters/actionqueue.h"
#include <cmath>

Character::Character(QObject *parent) : CharacterMovement(parent)
{
  setProperty("float", true);
  fieldOfView = new FieldOfView(*this);
  actionQueue = new ActionQueue(this);
  inventory->setUser(this);
  connect(inventory, &Inventory::unequippedItem, this, &Character::initializeEmptySlot);
  connect(actionQueue, &ActionQueue::queueCompleted, this, &Character::onActionQueueCompleted);
  connect(this, &Character::characterKill, this, &Character::died);
  connect(this, &Character::died, [this]() { if (script) { script->call("onDied"); } });
  connect(this, &Character::characterSheetChanged, this, &Character::onCharacterSheetChanged);
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
    script->call("onDamageTaken", QJSValueList() << damage << Game::get()->getScriptEngine().newQObject(dealer));
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

bool Character::isAlly(const Character* other) const
{
  return faction && faction->flag == other->getFactionFlag();
}

bool Character::isEnemy(const Character* other) const
{
  if (faction)
  {
    auto otherFlag = other->getFactionFlag();

    return otherFlag == 0 ? other->isEnemy(this) : (faction->enemyMask & otherFlag) != 0;
  }
  return (enemyFlag & other->getFactionFlag()) != 0;
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

void Character::setAsEnemy(Character* other)
{
  auto* diplomacy = Game::get()->getDiplomacy();
  auto* faction   = diplomacy->getFaction(other->getFactionFlag());

  if (other->getFactionName() != "") {
    if (getFactionFlag() > 0)
      diplomacy->setAsEnemy(true, getFactionFlag(), other->getFactionFlag());
    else
      enemyFlag += other->getFactionFlag();
  }
  else if (faction)
    other->setAsEnemy(this);
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

void Character::initializeFaction()
{
  auto* characterSheet = getStatistics();
  auto* diplomacy = Game::get()->getDiplomacy();

  if (characterSheet && diplomacy)
    faction = diplomacy->getFaction(characterSheet->getFaction());
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

void Character::setCharacterSheet(const QString& name)
{
  characterSheet = name;
  emit characterSheetChanged();
}

void Character::onCharacterSheetChanged()
{
  qDebug() << "Changing character sheet";
  StatModel* charSheet;

  charSheet = Game::get()->getDataEngine()->makeStatModel(getObjectName(), characterSheet);
  charSheet->setParent(this);
  setStatistics(charSheet);
}

void Character::setStatistics(StatModel *value)
{
  if (statistics)
  {
    disconnect(statistics, &StatModel::factionChanged, this, &Character::initializeFaction);
    statistics->deleteLater();
  }
  statistics = value;
  connect(statistics, &StatModel::factionChanged, this, &Character::initializeFaction);
  qDebug() << "set stat sheet on" << getObjectName() << ':' << statistics->getName() << " with faction " << statistics->property("faction").toString();
  initializeFaction();
  emit statisticsChanged();
}

void Character::load(const QJsonObject& data)
{
  QString objectName = data["objectName"].toString();

  characterSheet = data["charsheet"].toString(objectName);
  isUnique = data["uniq"].toBool();
  enemyFlag = static_cast<unsigned int>(data["enemyFlag"].toInt(0));
  actionPoints = data["ap"].toInt();
  if (isUnique || data["stats"]["name"].toString().length() == 0)
    onCharacterSheetChanged();
  else
  {
    StatModel* charSheet = new StatModel(this);

    charSheet->fromJson(data["stats"].toObject());
    setStatistics(charSheet);
  }
  CharacterMovement::load(data);
}

void Character::save(QJsonObject& data) const
{
  data["charsheet"] = characterSheet;
  data["uniq"] = isUnique;
  data["enemyFlag"] = static_cast<int>(enemyFlag);
  data["ap"] = actionPoints;
  if (!(Game::get()->property("isGameEditor").toBool()))
  {
    if (isUnique)
      Game::get()->getDataEngine()->saveStatModel(getObjectName(), statistics);
    else
    {
      QJsonObject statData;

      statistics->toJson(statData);
      data.insert("stats", statData);
    }
  }
  CharacterMovement::save(data);
}

QJSValue Character::getActions()
{
  return Game::get()->getScriptEngine().newQObject(getActionQueue());
}
