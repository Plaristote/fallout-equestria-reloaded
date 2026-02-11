#ifndef  REACHELEVATORACTION_H
# define REACHELEVATORACTION_H

# include "reach.h"
# include "game/objects/elevator.h"

class Elevator;

class ReachElevatorAction : public ReachAction
{
public:
  ReachElevatorAction(Character* character, Elevator* object, float range) : ReachAction(character, object, range), elevator(object)
  {
  }

  ReachElevatorAction(Character* character, Elevator* object, float range, QJSValue callback) : ReachAction(character, object, range, callback), elevator(object)
  {
  }

protected:
  Point getTargetPosition() const override;

private:
  Elevator* elevator;
};

#endif // REACHELEVATORACTION_H

