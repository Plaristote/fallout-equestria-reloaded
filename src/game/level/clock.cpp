#include "clock.h"
#include "game.h"
#include "game/characters/actionqueue.h"

ClockComponent::ClockComponent(QObject *parent) : ParentType{parent}
{

}

void ClockComponent::load(const QJsonObject& data)
{
  timeManager = Game::get()->getTimeManager();
  ParentType::load(data);
}

void ClockComponent::advanceTime(unsigned int minutes)
{
  const qint64 minuteDelta = 60 * 1000;

  while (minutes-- > 0)
  {
    for (DynamicObject* object : allDynamicObjects())
    {
      ObjectPerformanceClock clock(performanceMetrics.object(object));
      Character* character = qobject_cast<Character*>(object);

      if (character && !character->getActionQueue()->isEmpty())
      {
        qint64 remaining = minuteDelta;
        qint64 subStep = 2 * 1000;

        while (remaining > 0)
        {
          qint64 delta = std::min(subStep, remaining);

          character->update(delta);
          character->getActionQueue()->update();
          remaining -= subStep;
        }
      }
      else
        object->update(minuteDelta);
      object->updateTasks(minuteDelta);
    }
    for (ObjectGroup* group : allObjectGroups())
      group->getTaskManager()->update(minuteDelta);
    taskRunner->update(minuteDelta);
    Game::get()->getTaskManager()->update(minuteDelta);
  }
}
