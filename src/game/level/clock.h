#ifndef  CLOCKCOMPONENT_H
# define CLOCKCOMPONENT_H

# include "combat.h"

class TimeManager;

class ClockComponent : public CombatComponent
{
  Q_OBJECT
  typedef CombatComponent ParentType;
public:
  explicit ClockComponent(QObject *parent = nullptr);

  void load(const QJsonObject&);

  Q_INVOKABLE void advanceTime(unsigned int minutes);

protected:
  TimeManager* timeManager = nullptr;
};

#endif // CLOCKCOMPONENT_H
