#ifndef REACHDOORACTION_H
#define REACHDOORACTION_H

# include "reach.h"
# include "game/objects/doorway.h"

class Doorway;

class ReachDoorAction : public ReachAction
{
public:
  ReachDoorAction(Character* character, Doorway* object, float range) : ReachAction(character, object, range), doorway(object)
  {
  }

  ReachDoorAction(Character* character, Doorway* object, float range, QJSValue callback) : ReachAction(character, object, range, callback), doorway(object)
  {
  }

  bool trigger() override;
  int getApCost() const override;

private:
  bool alreadyReached() const override;

  Doorway* doorway;
};

#endif // REACHDOORACTION_H
