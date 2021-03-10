#ifndef  GAME_H
# define GAME_H

# include <QObject>
# include "game/dataengine.h"
# include "game/leveltask.h"
# include "game/characterparty.h"
# include "game/timermanager.h"
# include "game/questmanager.h"
# include "game/worldmap.h"
# include "game/diplomacy.hpp"
# include <QJSEngine>
# include "cmap/trait.h"

class Game : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QStringList     consoleMessages MEMBER consoleMessages NOTIFY consoleUpdated)
  Q_PROPERTY(LevelTask*      level       MEMBER currentLevel NOTIFY levelChanged)
  Q_PROPERTY(WorldMap*       worldmap    MEMBER worldmap)
  Q_PROPERTY(CharacterParty* playerParty MEMBER playerParty)
  Q_PROPERTY(DataEngine*     dataEngine  MEMBER dataEngine)
  Q_PROPERTY(TimeManager*    timeManager MEMBER timeManager)
  Q_PROPERTY(bool isGameEditor MEMBER isGameEditor)

  static Game* instance;

public:
  explicit Game(QObject *parent = nullptr);
  ~Game();

  void loadFromDataEngine();
  Q_INVOKABLE void save();
  void newPlayerParty(StatModel*);

  Q_INVOKABLE void appendToConsole(const QString&);
  Q_INVOKABLE void goToLevel(const QString& name);
  Q_INVOKABLE void switchToLevel(const QString& name, const QString& targetZone);
  void exitLevel(bool silence = false);

  static Game* get() { return instance; }

  Q_INVOKABLE DataEngine* getDataEngine() const { return dataEngine; }
  TimeManager* getTimeManager() const { return timeManager; }
  WorldDiplomacy* getDiplomacy() const { return diplomacy; }
  Q_INVOKABLE QuestManager* getQuestManager() const { return quests; }
  WorldMap* getWorldmap() const { return worldmap; }
  LevelTask* getLevel() const { return currentLevel; }
  QJSEngine& getScriptEngine() { return scriptEngine; }
  QJSValue loadScript(const QString& path);
  QJSValue scriptCall(QJSValue callable, const QJSValueList& args, const QString& scriptName);
  void loadCmapTraits();

  QMap<QString, Trait>& getCmapTraits() { return cmapTraits; }

  Q_INVOKABLE CharacterParty* getPlayerParty() { return playerParty; }
  Q_INVOKABLE Character* getPlayer() { return player; }
  Q_INVOKABLE StatModel* getPlayerStatistics() { return getPlayer()->getStatistics(); }
  Q_INVOKABLE void       advanceTime(unsigned int minutes);

signals:
  void levelChanged();
  void levelSwapped();
  void consoleUpdated();
  void gameOver();

public slots:
  void onCityEntered(QString name);
  void changeZone(TileZone*);
  void deleteLater();

private:
  bool isGameEditor = false;
  DataEngine* dataEngine = nullptr;
  TimeManager* timeManager;
  WorldDiplomacy* diplomacy;
  QuestManager* quests;
  WorldMap* worldmap;
  LevelTask*  currentLevel = nullptr;
  CharacterParty* playerParty = nullptr;
  Character* player = nullptr;
  QStringList consoleMessages;
  QJSEngine   scriptEngine;

  QMap<QString, Trait> cmapTraits;
};

#endif // GAME_H
