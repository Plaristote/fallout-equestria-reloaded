#ifndef  GAME_H
# define GAME_H

# include <QObject>
# include "game/dataengine.h"
# include "game/leveltask.h"
# include "game/characterparty.h"
# include <QJSEngine>
# include "cmap/trait.h"

class Game : public QObject
{
  Q_OBJECT

  Q_PROPERTY(LevelTask* level MEMBER currentLevel NOTIFY levelChanged)
  Q_PROPERTY(QStringList consoleMessages MEMBER consoleMessages NOTIFY consoleUpdated)
  Q_PROPERTY(CharacterParty* playerParty MEMBER playerParty NOTIFY playerPartyChanged)
  Q_PROPERTY(DataEngine* dataEngine MEMBER dataEngine NOTIFY dataEngineChanged)

  static Game* instance;

public:
  explicit Game(QObject *parent = nullptr);
  ~Game();

  void loadFromDataEngine();
  void save();
  void newPlayerParty(StatModel*);

  Q_INVOKABLE void appendToConsole(const QString&);
  Q_INVOKABLE void goToLevel(const QString& name);
  Q_INVOKABLE void switchToLevel(const QString& name, const QString& targetZone);
  void exitLevel();

  static Game* get() { return instance; }

  DataEngine* getDataEngine() const { return dataEngine; }
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
  void consoleUpdated();
  void playerPartyChanged();
  void dataEngineChanged();

public slots:
  void changeZone(TileZone*);
  void deleteLater();

private:
  DataEngine* dataEngine = nullptr;
  LevelTask*  currentLevel = nullptr;
  CharacterParty* playerParty = nullptr;
  Character* player = nullptr;
  QStringList consoleMessages;
  QJSEngine   scriptEngine;

  QMap<QString, Trait> cmapTraits;
};

#endif // GAME_H
