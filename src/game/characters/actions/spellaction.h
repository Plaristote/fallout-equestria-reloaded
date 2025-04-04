#ifndef  SPELLACTION_H
# define SPELLACTION_H

# include "animatedaction.h"
# include "game/spell.h"

class SpellAction : public AnimatedAction
{
public:
  enum TargetType { NoTarget, ObjectTarget, PositionTarget };

  SpellAction(Character* c, const QString& spell)
    : AnimatedAction(c), spell(Spell::requireSpell(spell)), targetType(NoTarget)
  {
  }

  SpellAction(Character* c, DynamicObject* target, const QString& spell)
    : AnimatedAction(c), spell(Spell::requireSpell(spell)), target(target), targetType(ObjectTarget)
  {
  }

  SpellAction(Character* c, QPoint target, const QString& spell)
    : AnimatedAction(c), spell(Spell::requireSpell(spell)), targetPosition(target), targetType(PositionTarget)
  {
  }

  int  getApCost() const override;
  bool trigger() override;

private:
  void   performAction() override;
  QPoint getTargetPosition() const { return target ? target->getPosition() : targetPosition; }
  void   lookAtTarget() override;
  QJSValue getDefaultCallback() override;

  Spell&         spell;
  TargetType     targetType;
  DynamicObject* target = nullptr;
  QPoint         targetPosition;
};

#endif // SPELLACTION_H
