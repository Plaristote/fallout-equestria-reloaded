#ifndef  GAME_H
# define GAME_H

# include <QObject>
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

  static Game* instance;

public:
  explicit Game(QObject *parent = nullptr);
  ~Game();

  void newPlayerParty();

  Q_INVOKABLE void appendToConsole(const QString&);
  Q_INVOKABLE void goToLevel(const QString& name);
  Q_INVOKABLE void switchToLevel(const QString& name, const QString& targetZone);
  void exitLevel();

  static Game* get() { return instance; }

  QJSEngine& getScriptEngine() { return scriptEngine; }
  QJSValue loadScript(const QString& path);

  void loadCmapTraits();

  QMap<QString, Trait>& getCmapTraits() { return cmapTraits; }
  CharacterParty* getPlayerParty() { return playerParty; }

signals:
  void levelChanged();
  void consoleUpdated();
  void playerPartyChanged();

public slots:
  void changeZone(TileZone*);

private:
  LevelTask*  currentLevel = nullptr;
  CharacterParty* playerParty = nullptr;
  QStringList consoleMessages;
  QJSEngine   scriptEngine;

  QMap<QString, Trait> cmapTraits;
};

#endif // GAME_H
