#include "save.h"
#include "game.h"
#include "tutorialcomponent.h"

SaveComponent::SaveComponent(QObject *parent) : ParentType{parent}
{

}

void SaveComponent::loadTutorial()
{
  if (TutorialComponent::hasTutorialForLevel(name))
  {
    tutorial = new TutorialComponent(this);
    emit tutorialChanged();
  }
}

void SaveComponent::load(const QString& levelName, DataEngine* dataEngine)
{
  QJsonObject levelData   = dataEngine->getLevelData(levelName);
  QJsonValue  lastUpdate  = levelData["lastUpdate"];
  bool        initialized = levelData["init"].toBool(false);

  levelData["name"]   = levelName;
  levelData["script"] = levelName + ".mjs";
  levelData["init"]   = true; // Delay level initialization
  qDebug() << "LevelTask::load" << levelName;
  ParentType::load(levelData);
  registerAllDynamicObjects();
  taskRunner->setScriptController(script);
  taskRunner->load(levelData["tasks"].toObject());
  if (!lastUpdate.isUndefined() && !lastUpdate.isNull())
    passElapsedTime(lastUpdate.toInt());
  if (!initialized)
    ScriptableComponent::initializeIfNeeded();
  loadTutorial();
}

void SaveComponent::passElapsedTime(int lastUpdate)
{
  std::time_t timestamp    = static_cast<std::time_t>(lastUpdate);
  std::time_t newTimestamp = Game::get()->getTimeManager()->getDateTime().GetTimestamp();
  std::time_t elapsedTime  = newTimestamp - timestamp;

  if (elapsedTime > 0)
  {
    DateTime timeFrom(static_cast<unsigned int>(timestamp));
    DateTime timeTo(static_cast<unsigned int>(newTimestamp));

    qDebug() << "Level::SaveComponent: advancing time in level from " << timeFrom.ToString().c_str() << "to" << timeTo.ToString().c_str();
    eachObject([elapsedTime](DynamicObject* object)
    {
      object->getTaskManager()->update(elapsedTime * 1000);
    });
    taskRunner->update(elapsedTime * 1000);
  }
}

void SaveComponent::persist()
{
  save(Game::get()->getDataEngine());
}

void SaveComponent::save(DataEngine* dataEngine)
{
  if (persistent || isGameEditor())
  {
    Game* game = Game::get();
    QJsonObject levelData = dataEngine->getLevelData(name);

    ParentType::save(levelData);
    levelData.remove("name");
    levelData.remove("script");
    if (!isGameEditor())
      levelData["lastUpdate"] = static_cast<int>(game->getTimeManager()->getDateTime().GetTimestamp());
    dataEngine->setLevelData(name, levelData);
  }
  else
    qDebug() << "(!) Level is not persistent and it's current state has been discarded.";
}

bool SaveComponent::canSave() const
{
  return combat || !saveEnabled;
}

void SaveComponent::toggleSaveEnabled(bool value)
{
  saveEnabled = value;
  emit canSaveChanged();
}
