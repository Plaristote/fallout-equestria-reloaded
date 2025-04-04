#ifndef  REACHACTION_H
# define REACHACTION_H

# include "movement.h"
# include <QJSValue>

class ReachAction : public MovementAction
{
public:
  ReachAction(Character* character, DynamicObject* object, float range) : MovementAction(character, Point{0,0,0}), object(object), range(range)
  {
  }

  ReachAction(Character* character, DynamicObject* object, float range, QJSValue callback) : MovementAction(character, Point{0,0,0}), object(object), range(range), rateCallback(callback)
  {
  }

  virtual bool trigger() override;
  virtual int getApCost() const override;

protected:
  void triggerNextMovement() override;
  QVector<Point> getCandidates(int caseDistance) const;
  virtual bool alreadyReached() const;
  virtual Point getTargetPosition() const { return object->getPoint(); }
  int getApCostForCandidates(const QVector<Point>& candidates, bool quickMode = false) const;

  DynamicObject* object;
  float range = 1.f;
  QJSValue rateCallback;
};

#endif // REACHACTION_H
