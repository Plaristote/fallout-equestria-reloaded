#ifndef  REACHACTION_H
# define REACHACTION_H

# include "movement.h"
# include <QJSValue>

class ReachAction : public MovementAction
{
public:
  ReachAction(Character* character, DynamicObject* object, float range) : MovementAction(character, QPoint(0,0)), object(object), range(range)
  {
  }

  ReachAction(Character* character, DynamicObject* object, float range, QJSValue callback) : MovementAction(character, QPoint(0,0)), object(object), range(range), rateCallback(callback)
  {
  }

  bool trigger() override;
  int getApCost() const override;

protected:
  void triggerNextMovement() override;
  QVector<QPoint> getCandidates(int caseDistance) const;
  virtual bool alreadyReached() const;
  virtual QPoint getTargetPosition() const { return object->getPosition(); }

  DynamicObject* object;
  float range = 1.f;
  QJSValue rateCallback;
};

#endif // REACHACTION_H
