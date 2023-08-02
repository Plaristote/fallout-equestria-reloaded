#include "gamepadcontroller.h"
#include <QCursor>
#include "mousecursor.h"
#include <QDebug>
#include "game.h"
#include "game/leveltask.h"
#ifdef WITH_GAMEPAD
# include <QGamepad>
#endif

GamepadController* GamepadController::instance = nullptr;

GamepadController::GamepadController(QObject *parent) : QObject(parent)
{
}

void GamepadController::initialize()
{
#ifdef WITH_GAMEPAD
# ifdef _WIN32
  windowsWorkaroundTimer.setInterval(2500);
  windowsWorkaroundTimer.setSingleShot(false);
  windowsWorkaroundTimer.start();
  connect(&windowsWorkaroundTimer, &QTimer::timeout, this, &GamepadController::lookForController);
# else
  connect(QGamepadManager::instance(), &QGamepadManager::connectedGamepadsChanged, this, &GamepadController::lookForController);
  lookForController();
# endif
#endif
}

void GamepadController::lookForController()
{
#ifdef WITH_GAMEPAD
  qDebug() << "GamepadController::lookForController";
  if (!isConnected())
  {
    auto gamepads = QGamepadManager::instance()->connectedGamepads();

    instance = this;
    if (gamepads.isEmpty())
      qDebug() << "GamepadController: Did not find any connected gamepads";
    else
      connectGamepad(*gamepads.begin());
  }
#endif
}

void GamepadController::connectGamepad(int id)
{
#ifdef WITH_GAMEPAD
  qDebug() << "GamepadController: Connected gamepad" << id;
  if (gamepad)
    delete gamepad;
  gamepad = new QGamepad(id, this);
  connect(gamepad, &QGamepad::axisLeftXChanged,    this, &GamepadController::updateCursorOnXAxis);
  connect(gamepad, &QGamepad::axisLeftYChanged,    this, &GamepadController::updateCursorOnYAxis);
  connect(gamepad, &QGamepad::axisRightXChanged,   this, &GamepadController::updateCameraOnXAxis);
  connect(gamepad, &QGamepad::axisRightYChanged,   this, &GamepadController::updateCameraOnYAxis);
  connect(gamepad, &QGamepad::buttonStartChanged,  this, &GamepadController::startPressedChange);
  connect(gamepad, &QGamepad::buttonSelectChanged, this, &GamepadController::selectPressedChange);
  connect(gamepad, &QGamepad::buttonBChanged,      this, &GamepadController::buttonBPressedChange);
  connect(gamepad, &QGamepad::buttonAChanged,      this, &GamepadController::buttonAPressedChange);
  connect(gamepad, &QGamepad::buttonYChanged,      this, &GamepadController::buttonYPressedChange);
  connect(gamepad, &QGamepad::buttonXChanged,      this, &GamepadController::buttonXPressedChange);
  connect(gamepad, &QGamepad::buttonR1Changed,     this, &GamepadController::buttonR1PressedChange);
  connect(gamepad, &QGamepad::buttonR2Changed,     this, &GamepadController::buttonR2PressedChange);
  connect(gamepad, &QGamepad::buttonR3Changed,     this, &GamepadController::cameraAxisClicked);
  connect(gamepad, &QGamepad::buttonL1Changed,     this, &GamepadController::buttonL1PressedChange);
  connect(gamepad, &QGamepad::buttonL2Changed,     this, &GamepadController::buttonL2PressedChange);
  connect(gamepad, &QGamepad::buttonL3Changed,     this, &GamepadController::movementAxisClicked);
  connect(gamepad, &QGamepad::buttonUpChanged,     this, &GamepadController::upPressedChange);
  connect(gamepad, &QGamepad::buttonDownChanged,   this, &GamepadController::downPressedChange);
  connect(gamepad, &QGamepad::buttonLeftChanged,   this, &GamepadController::leftPressedChange);
  connect(gamepad, &QGamepad::buttonRightChanged,  this, &GamepadController::rightPressedChange);
  connect(gamepad, &QGamepad::connectedChanged,    this, &GamepadController::connectedChanged);
  connect(&cursorTimer.ticker, &QTimer::timeout, this, &GamepadController::updateCursorPosition);
  connect(&cameraTimer.ticker, &QTimer::timeout, this, &GamepadController::updateCameraPosition);
  emit connectedChanged();
#endif
}

bool GamepadController::isEnabled() const
{
#ifdef WITH_GAMEPAD
  return true;
#else
  return false;
#endif
}

bool GamepadController::isConnected() const
{
#ifdef WITH_GAMEPAD
  return gamepad && gamepad->isConnected();
#else
  return false;
#endif
}

GamepadController::CursorTimer::CursorTimer()
{
  x = y = 0;
  ticker.setInterval(10);
  ticker.setSingleShot(false);
}

void GamepadController::CursorTimer::updateState()
{
  if (y == 0.0 && x == 0.0)
    ticker.stop();
  else if (!ticker.isActive())
  {
    ticker.start();
    timer.restart();
  }
}

void GamepadController::updateCameraPosition()
{
  long ticks = cameraTimer.ticks();
  auto x = (cameraTimer.x / 25) * static_cast<double>(ticks);
  auto y = (cameraTimer.y / 25) * static_cast<double>(ticks);

  if (x != 0.0)
    emit moveCameraXAxis(x);
  if (y != 0.0)
    emit moveCameraYAxis(y);
}

void GamepadController::updateCameraOnXAxis(double value)
{
  cameraTimer.x = value;
  cameraTimer.updateState();
}

void GamepadController::updateCameraOnYAxis(double value)
{
  cameraTimer.y = value;
  cameraTimer.updateState();
}

void GamepadController::updateCursorPosition()
{
  QPoint p = QCursor::pos();
  long ticks = cursorTimer.ticks();

  p.rx() += static_cast<int>(cursorTimer.x * 25) * ticks;
  p.ry() += static_cast<int>(cursorTimer.y * 25) * ticks;
  QCursor::setPos(p);
  cursorTimer.timer.restart();
}

void GamepadController::updateCursorOnXAxis(double value)
{
  cursorTimer.x = value;
  cursorTimer.updateState();
}

void GamepadController::updateCursorOnYAxis(double value)
{
  cursorTimer.y = value;
  cursorTimer.updateState();
}

void GamepadController::startPressedChange(bool pressed)
{
  if (!pressed)
    emit startClicked();
}

void GamepadController::selectPressedChange(bool pressed)
{
  if (!pressed)
    emit selectClicked();
}

void GamepadController::buttonBPressedChange(bool pressed)
{
  if (!pressed)
  {
    auto* game = Game::get();

    if (game && game->getLevel() && game->getLevel()->getMouseMode() != LevelTask::MovementCursor)
      game->getLevel()->swapMouseMode();
    else
      emit backClicked();
  }
}

void GamepadController::buttonYPressedChange(bool pressed)
{
  if (!pressed)
    emit skilldexClicked();
}

void GamepadController::buttonXPressedChange(bool pressed)
{
  if (!pressed)
    emit inventoryClicked();
}

void GamepadController::buttonL1PressedChange(bool pressed)
{
  if (!pressed)
  {
    if (isLevelMode())
      swapUseMode("use-1");
    else
      emit leftTriggerClicked();
  }
}

void GamepadController::buttonL2PressedChange(bool pressed)
{
  if (!pressed)
  {
    if (isLevelMode())
      triggerItemUse("use-1");
    else
      emit leftTriggerClicked();
  }
}

void GamepadController::buttonR1PressedChange(bool pressed)
{
  if (!pressed)
  {
    if (isLevelMode())
      swapUseMode("use-2");
    else
      emit rightTriggerClicked();
  }
}

void GamepadController::buttonR2PressedChange(bool pressed)
{
  if (!pressed)
  {
    if (isLevelMode())
      triggerItemUse("use-2");
    else
      emit rightTriggerClicked();
  }
}

void GamepadController::upPressedChange(bool pressed)
{
  if (!pressed)
    emit upClicked();
}

void GamepadController::downPressedChange(bool pressed)
{
  if (!pressed)
    emit bottomClicked();
}

void GamepadController::leftPressedChange(bool pressed)
{
  if (!pressed)
    emit leftClicked();
}

void GamepadController::rightPressedChange(bool pressed)
{
  if (!pressed)
    emit rightClicked();
}

void GamepadController::movementAxisClicked(bool pressed)
{
  if (!pressed)
  {
    if (isLevelMode())
      Game::get()->getLevel()->swapMouseMode();
  }
}

void GamepadController::clickEvent(bool pressed)
{
  MouseCursor::get()->click(pressed);
}

void GamepadController::swapUseMode(const QString &slotName)
{
  auto* game = Game::get();
  InventoryItem* item = game->getLevel()->getPlayer()->getInventory()->getEquippedItem(slotName);

  if (item)
    item->swapUseMode();
}

void GamepadController::triggerItemUse(const QString &slotName)
{
  auto* game = Game::get();

  game->getLevel()->setActiveItem(slotName);
}

bool GamepadController::isLevelMode() const
{
  auto* game = Game::get();

  return game && game->getLevel() && !game->getLevel()->isPaused();
}
