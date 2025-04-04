#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

# include <QObject>
# include "game/animationlibrary.h"
# include "game/inventoryitemlibrary.h"

class GamePackage;
class GameManager;
class MusicManager;
class SoundManager;
class I18n;
class QQmlEngine;

class GameContext : public QObject
{
  Q_OBJECT
public:
  GameContext(QObject* parent = nullptr);

  void load(const GamePackage& package);
  void initializeQmlProperties(QQmlEngine& engine);

private:
  AnimationLibrary     animationLibrary;
  InventoryItemLibrary itemLibrary;
  I18n*                i18n = nullptr;
  GameManager*         gameManager = nullptr;
  MusicManager*        musicManager = nullptr;
  SoundManager*        soundManager = nullptr;
  QString              fileProtocol, rootPath, scriptPath, assetPath;
};

#endif // GAMECONTEXT_H
