#ifndef  REACHCASE_H
# define REACHCASE_H

# include "reach.h"
# include <QJSValue>

class ReachCaseAction : public ReachAction
{
public:
  ReachCaseAction(Character* character, Point _target, float range) : ReachAction(character, nullptr, range)
  {
    target = _target;
  }

  ReachCaseAction(Character* character, Point _target, float range, QJSValue callback) : ReachAction(character, nullptr, range, callback)
  {
    target = _target;
  }

protected:
  bool alreadyReached() const;
  Point getTargetPosition() const override { return target; }
};

#endif // REACHCASE_H
