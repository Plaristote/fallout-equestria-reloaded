#include "questmanager.h"
#include "game.h"
#include <QJsonArray>

QuestManager::QuestManager(QObject *parent) : QObject(parent)
{
  connect(Game::get(), &Game::levelChanged, this, &QuestManager::onLevelChanged);
}

void QuestManager::addQuest(const QString& name)
{
  if (!getQuest(name))
  {
    Quest* quest = new Quest(this);

    list << quest;
    quest->initialize(name);
    emit listChanged();
  }
}

Quest* QuestManager::getQuest(const QString& name) const
{
  for (Quest* quest : qAsConst(list))
  {
    if (quest->property("name").toString() == name)
      return quest;
  }
  return nullptr;
}

void QuestManager::load(const QJsonObject& data)
{
  const QJsonArray jsonList = data["list"].toArray();

  for (const QJsonValue& jsonQuest : jsonList)
  {
    Quest* quest = new Quest(this);

    quest->load(jsonQuest.toObject());
    list << quest;
  }
  emit listChanged();
}

QJsonObject QuestManager::save() const
{
  QJsonObject data;
  QJsonArray jsonList;

  for (const Quest* quest : list)
    jsonList << quest->save();
  data.insert("list", jsonList);
  return data;
}

void QuestManager::onCharacterKilled(Character* victim, Character* killer)
{
  for (Quest* quest : qAsConst(list))
    quest->onCharacterKilled(victim, killer);
}

void QuestManager::onItemPicked(InventoryItem* item)
{
  for (Quest* quest : qAsConst(list))
    quest->onItemPicked(item);
}

void QuestManager::onLevelChanged()
{
  for (Quest* quest : qAsConst(list))
    quest->onLevelChanged();
}
