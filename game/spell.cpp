#include "spell.h"
#include "game.h"

QMap<QString, Spell> Spell::spells;

Spell& Spell::requireSpell(const QString &name)
{
  auto it = spells.find(name);

  if (it == spells.end())
  {
    Spell spell;

    spell.script = Game::get()->loadScript(SCRIPTS_PATH + "spells/" + name + ".mjs");
    spell.initialize();
    it = spells.insert(name, spell);
  }
  return it.value();
}

void Spell::initialize()
{
  auto targetModeProperty = script.property("targetMode");

  if (targetModeProperty.isNumber())
    targetMode = static_cast<InteractionComponent::TargetMode>(targetModeProperty.toUInt());
  callback = script.property("use");
  triggerUse = script.property("triggerUse");
}

int Spell::getActionPointCost(Character *user)
{
  auto actionPointCost = script.property("actionPointCost");

  if (actionPointCost.isCallable())
    return actionPointCost.call(QJSValueList() << user->asJSValue()).toInt();
  else if (actionPointCost.isNumber())
    return actionPointCost.toInt();
  return 2;
}

QJSValue Spell::useOn(Character* user, DynamicObject* target)
{
  return callback.call(QJSValueList() << user->asJSValue() << target->asJSValue());
}

QJSValue Spell::useAt(Character* user, int x, int y)
{
  return callback.call(QJSValueList() << user->asJSValue() << x << y);
}

QJSValue Spell::use(Character* user)
{
  return callback.call(QJSValueList() << user->asJSValue());
}
