#ifndef  ZONE_MOVEMENT_ACTION_H
# define ZONE_MOVEMENT_ACTION_H

# include "movement.h"

class ZoneMovementAction : public MovementAction
{
public:
  ZoneMovementAction(Character* character, const TileZone* zone) : MovementAction(character, Point{0,0,0}), zone(zone)
  {
  }

  bool trigger() override;

private:
  const TileZone* zone;
};

#endif
