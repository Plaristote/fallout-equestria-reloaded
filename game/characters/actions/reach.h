#ifndef REACHACTION_H
#define REACHACTION_H

# include "movement.h"

class ReachAction : public MovementAction
{
public:
  ReachAction(Character* character, DynamicObject* object, float range) : MovementAction(character, QPoint(0,0)), object(object), range(range)
  {
  }

  bool trigger() override;

protected:
  void triggerNextMovement() override;
  QVector<QPoint> getCandidates(int caseDistance);

  DynamicObject* object;
  float range = 1.f;
};

#endif // REACHACTION_H