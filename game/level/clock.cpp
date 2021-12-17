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
  while (minutes-- > 0)
  {
    const qint64 delta = 60 * 1000;

    for (DynamicObject* object : allDynamicObjects())
    {
      ObjectPerformanceClock clock(performanceMetrics.object(object));

      object->update(delta);
      object->updateTasks(delta);
      if (object->isCharacter())
        reinterpret_cast<Character*>(object)->getActionQueue()->update();
    }
    for (ObjectGroup* group : allObjectGroups())
      group->getTaskManager()->update(delta);
    taskRunner->update(delta);
  }
}
