#ifndef  GAME_H
# define GAME_H

# include <QObject>
# include "utils/storableobject.h"
# include "game/dataengine.h"
# include "game/leveltask.h"
# include "game/characterparty.h"
# include "game/timermanager.h"
# include "game/questmanager.h"
# include "game/worldmap/worldmap.h"
# include "game/worldmap/randomencountercontroller.h"
# include "game/diplomacy.hpp"
# include <QJSEngine>
# include "cmap/trait.h"
# include "cmap/race.h"
# include "game/timepasser.h"

class Game : public StorableObject
{
  Q_OBJECT

  Q_PROPERTY(QStringList     consoleMessages MEMBER consoleMessages NOTIFY consoleUpdated)
  Q_PROPERTY(LevelTask*      level       MEMBER currentLevel NOTIFY levelChanged)
  Q_PROPERTY(WorldMap*       worldmap    MEMBER worldmap CONSTANT)
  Q_PROPERTY(Character*      player      MEMBER player CONSTANT)
  Q_PROPERTY(CharacterParty* playerParty MEMBER playerParty CONSTANT)
  Q_PROPERTY(DataEngine*     dataEngine  MEMBER dataEngine CONSTANT)
  Q_PROPERTY(TimeManager*    timeManager MEMBER timeManager CONSTANT)
  Q_PROPERTY(TaskRunner*     tasks       MEMBER taskManager CONSTANT)
  Q_PROPERTY(QuestManager*   quests      MEMBER quests CONSTANT)
  Q_PROPERTY(RandomEncounterController* randomEncounters MEMBER randomEncounters CONSTANT)
  Q_PROPERTY(bool fastPassTime READ isFastPassingTime NOTIFY fastPassingChanged)
  Q_PROPERTY(bool isGameEditor MEMBER isGameEditor NOTIFY gameEditorEnabled)

  static Game* instance;
public:
  explicit Game(QObject *parent = nullptr);
  ~Game() override;

  Q_INVOKABLE QStringList getCharacterTemplateList() const;
  Q_INVOKABLE void prepareEditor();
  void loadFromDataEngine();
  Q_INVOKABLE void save();
  void newPlayerParty(StatModel*);

  Q_INVOKABLE void appendToConsole(const QString&);
  Q_INVOKABLE void switchToLevel(const QString name, const QString targetZone);
  void exitLevel(bool silence = false);

  static Game* get() { return instance; }

  Q_INVOKABLE DataEngine* getDataEngine() const { return dataEngine; }
  TimeManager* getTimeManager() const { return timeManager; }
  WorldDiplomacy* getDiplomacy() const { return diplomacy; }
  Q_INVOKABLE QuestManager* getQuestManager() const { return quests; }
  WorldMap* getWorldmap() const { return worldmap; }
  LevelTask* getLevel() const { return currentLevel; }
  TaskRunner* getTaskManager() const { return taskManager; }
  QJSEngine& getScriptEngine() { return scriptEngine; }
  QJSValue loadScript(const QString& path);
  QJSValue scriptCall(QJSValue callable, const QJSValueList& args, const QString& scriptName);
  QJSValue eval(const QString& command);
  Q_INVOKABLE QString consoleEval(const QString& command);
  void loadCmapTraits();
  void loadCmapRaces();

  Q_INVOKABLE CharacterParty* getPlayerParty() { return playerParty; }
  Q_INVOKABLE Character* getPlayer() { return player; }
  Q_INVOKABLE StatModel* getPlayerStatistics() { return getPlayer()->getStatistics(); }
  Q_INVOKABLE void       advanceTime(unsigned int minutes);
  Q_INVOKABLE void       asyncAdvanceTime(unsigned int minutes);
  Q_INVOKABLE void       asyncAdvanceTime(unsigned int minutes, QJSValue callback);
  bool                   isFastPassingTime() const { return timePasser.isActive(); }
  Q_INVOKABLE void       setFactionAsEnemy(const QString &faction, const QString &enemyFaction, bool set);

signals:
  void gameEditorEnabled();
  void levelChanged();
  void levelSwapped();
  void consoleUpdated();
  void gameOver();
  void transitionRequired(const QString& video, int elapsingTime = 0);
  void encounterTriggered(const QString& encounterTitle);
  void encounterNotify(const QString& encounterName, const QVariantMap& parameter);
  void loadError(const QString&);
  void fastPassingChanged();
  void requireScreenshot(QString);

public slots:
  void onCityEntered(QString name);
  void onCityEnteredAt(const QString& city, const QString& zone);
  void changeZone(TileZone*);
  void deleteLater();
  void loadLevel(const QString& name, const QString& targetZone = "");
  void onDiplomacyUpdate(const QStringList& factions, bool enemy);

private slots:
  void onLevelChanged();
  void onGameOver();

private:
  void initializeScript();
  LevelTask* newLevelTask();

  bool isGameEditor = false;
  DataEngine* dataEngine = nullptr;
  TimeManager* timeManager;
  WorldDiplomacy* diplomacy;
  QuestManager* quests;
  WorldMap* worldmap;
  RandomEncounterController* randomEncounters;
  LevelTask*  currentLevel = nullptr;
  CharacterParty* playerParty = nullptr;
  Character* player = nullptr;
  QStringList consoleMessages;
  QJSEngine   scriptEngine;
  ScriptController* script = nullptr;
  TaskRunner* taskManager = nullptr;
  TimePasser timePasser;

  QMap<QString, Trait> cmapTraits;
  QMap<QString, Race>  cmapRaces;
};

#endif // GAME_H
