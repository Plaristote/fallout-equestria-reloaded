#include "character.h"
#include "game.h"

Character::Character(QObject *parent) : StorageObject(parent)
{

}

QPoint Character::getInteractionPosition() const
{
  auto* player = Game::get()->getPlayerParty()->getCharacters().first();
  auto* level  = Game::get()->getLevel();
  auto* grid   = level->getGrid();
  auto  center = getPosition();

  for (int x = -1 ; x <= 1 ; ++x)
  {
    if (x < 0) continue ;
    for (int y = -1 ; y <= 1 ; ++y)
    {
      QPoint position(center);
      DynamicObject* occupant;

      if (y < 0) continue ;
      position.rx() += x;
      position.ry() += y;
      occupant = grid->getOccupant(position.x(), position.y());
      if (!grid->isOccupied(position.x(), position.y()) || occupant == player)
        return position;
    }
  }
  qDebug() << "No path to reach character";
  return QPoint(-1, -1);
}

QString Character::getDialogName()
{
  return script.property("dialog").toString();
}

void Character::load(const QJsonObject& data)
{
  QString objectName = data["objectName"].toString();

  isUnique = data["uniq"].toBool();
  if (isUnique)
    statistics = Game::get()->getDataEngine()->makeStatModel(objectName);
  else
  {
    statistics = new StatModel(this);
    statistics->fromJson(data["stats"].toObject());
  }
  StorageObject::load(data);
}

void Character::save(QJsonObject& data) const
{
  data["uniq"] = isUnique;
  if (isUnique)
    Game::get()->getDataEngine()->saveStatModel(getObjectName(), statistics);
  else
  {
    QJsonObject statData;

    statistics->toJson(statData);
    data.insert("stats", statData);
  }
  StorageObject::save(data);
}
