#include "buffs.h"
#include <QJsonArray>

CharacterBuffs::CharacterBuffs(QObject* parent) : ParentType(parent)
{

}

void CharacterBuffs::updateTasks(qint64 delta)
{
  ParentType::updateTasks(delta);
  for (auto it = buffs.begin() ; it != buffs.end() ; ++it)
    (*it)->update(delta);
}

void CharacterBuffs::load(const QJsonObject& data)
{
  const QJsonArray buffArray = data["buffs"].toArray();

  for (const QJsonValue& buffData : buffArray)
  {
    Buff* buff = new Buff(reinterpret_cast<Character*>(this));

    buff->load(buffData.toObject());
    buffs.push_back(buff);
    onBuffAdded(buff);
  }
  ParentType::load(data);
}

void CharacterBuffs::save(QJsonObject& data) const
{
  QJsonArray buffArray;

  for (Buff* buff : buffs)
  {
    QJsonObject buffData;

    buff->save(buffData);
    buffArray << buffData;
  }
  data["buffs"] = buffArray;
  ParentType::save(data);
}

Buff* CharacterBuffs::addBuff(const QString& name)
{
  Buff* buff = getBuff(name);

  if (!buff)
  {
    buff = new Buff(reinterpret_cast<Character*>(this));
    buff->setProperty("name", name);
    buffs.push_back(buff);
    onBuffAdded(buff);
    buff->initialize(name);
  }
  else
    buff->addNewCharge();
  return buff;
}

Buff* CharacterBuffs::getBuff(const QString& name) const
{
  for (auto it = buffs.begin() ; it != buffs.end() ; ++it)
  {
    if ((*it)->getName() == name)
      return *it;
  }
  return nullptr;
}

void CharacterBuffs::onBuffAdded(Buff* buff)
{
  connect(buff, &Buff::finish, this, &CharacterBuffs::onBuffRemoved, Qt::QueuedConnection);
  statistics->rbuffs().push_back(buff->getName());
  emit statistics->buffsChanged();
  qDebug() << "Buffs are now = to" << buff->getName();
}

void CharacterBuffs::onBuffRemoved(Buff* buff)
{
  buffs.removeOne(buff);
  statistics->rbuffs().removeAll(buff->getName());
  emit statistics->buffsChanged();
}

void CharacterBuffs::clearBuffs()
{
  for (auto it = buffs.begin() ; it != buffs.end() ; ++it)
  {
    Buff* buff = *it;

    onBuffRemoved(buff);
    buff->deleteLater();
  }
  buffs.clear();
}
