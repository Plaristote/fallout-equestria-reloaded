#ifndef  GAME_H
# define GAME_H

# include <QObject>
# include "game/dataengine.h"
# include "game/leveltask.h"
# include "game/characterparty.h"
# include "game/timermanager.h"
# include "game/worldmap.h"
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
  LevelTask* getLevel() const { return currentLevel; }
  QJSEngine& getScriptEngine() { return scriptEngine; }
  QJSValue loadScript(const QString& path);
  QJSValue scriptCall(QJSValue callable, const QJSValueList& args, const QString& scriptName);
  void loadCmapTraits();

  QMap<QString, Trait>& getCmapTraits() { return cmapTraits; }

  Q_INVOKABLE CharacterParty* getPlayerParty() { return playerParty; }
  Q_INVOKABLE Character* getPlayer() { return player; }
  Q_INVOKABLE StatModel* getPlayerStatistics() { return getPlayer()->getStatistics(); }

signals:
  void levelChanged();
  void levelSwapped();
  void consoleUpdated();

public slots:
  void onCityEntered(QString name);
  void changeZone(TileZone*);
  void deleteLater();

private:
  DataEngine* dataEngine = nullptr;
  TimeManager* timeManager;
  WorldMap* worldmap;
  LevelTask*  currentLevel = nullptr;
  CharacterParty* playerParty = nullptr;
  Character* player = nullptr;
  QStringList consoleMessages;
  QJSEngine   scriptEngine;

  QMap<QString, Trait> cmapTraits;
};

#endif // GAME_H
