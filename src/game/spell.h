#ifndef  SPELL_H
# define SPELL_H

# include <QJSValue>
# include "level/interaction.h"

class Spell
{
public:
  static Spell& requireSpell(const QString& name);

  InteractionComponent::TargetMode getTargetMode() const { return targetMode; }
  int      getActionPointCost(Character* user);
  QJSValue use(Character* user);
  QJSValue useOn(Character* user, DynamicObject* target);
  QJSValue useAt(Character* user, int x, int y);
  QJSValue triggerUse;

private:
  void initialize();

  static QMap<QString, Spell>      spells;
  QJSValue                         script;
  QJSValue                         callback;
  InteractionComponent::TargetMode targetMode = InteractionComponent::AnyTarget;
};

#endif // SPELL_H
