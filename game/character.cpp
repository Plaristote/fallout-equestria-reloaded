#include "character.h"
#include "game.h"

Character::Character(QObject *parent) : DynamicObject(parent)
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
