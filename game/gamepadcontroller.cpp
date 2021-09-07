#include "gamepadcontroller.h"
#include <QGamepad>
#include <QCursor>
#include "mousecursor.h"
#include <QDebug>
#include "game.h"
#include "game/leveltask.h"

GamepadController* GamepadController::instance = nullptr;

GamepadController::GamepadController(QObject *parent) : QObject(parent)
{
  auto gamepads = QGamepadManager::instance()->connectedGamepads();

  instance = this;
  xMovement = yMovement = 0;
  cursorTimer.setInterval(10);
  cursorTimer.setSingleShot(false);
  if (gamepads.isEmpty())
    qDebug() << "Did not find any connected gamepads";
  else
  {
    gamepad = new QGamepad(*gamepads.begin(), this);
    connect(gamepad, &QGamepad::axisLeftXChanged,   this, &GamepadController::updateCursorOnXAxis);
    connect(gamepad, &QGamepad::axisLeftYChanged,   this, &GamepadController::updateCursorOnYAxis);
    connect(gamepad, &QGamepad::axisRightXChanged,  this, &GamepadController::moveCameraXAxis);
    connect(gamepad, &QGamepad::axisRightYChanged,  this, &GamepadController::moveCameraYAxis);
    connect(gamepad, &QGamepad::buttonStartChanged, this, &GamepadController::startPressedChange);
    connect(gamepad, &QGamepad::buttonBChanged,     this, &GamepadController::buttonBPressedChange);
    connect(gamepad, &QGamepad::buttonAChanged,     this, &GamepadController::buttonAPressedChange);
    connect(gamepad, &QGamepad::buttonYChanged,     this, &GamepadController::buttonYPressedChange);
    connect(gamepad, &QGamepad::buttonXChanged,     this, &GamepadController::buttonXPressedChange);
    connect(gamepad, &QGamepad::buttonR1Changed,    this, &GamepadController::buttonR1PressedChange);
    connect(gamepad, &QGamepad::buttonR2Changed,    this, &GamepadController::buttonR2PressedChange);
    connect(gamepad, &QGamepad::buttonR3Changed,    this, &GamepadController::cameraAxisClicked);
    connect(gamepad, &QGamepad::buttonL1Changed,    this, &GamepadController::buttonL1PressedChange);
    connect(gamepad, &QGamepad::buttonL2Changed,    this, &GamepadController::buttonL2PressedChange);
    connect(gamepad, &QGamepad::buttonL3Changed,    this, &GamepadController::movementAxisClicked);
    connect(gamepad, &QGamepad::buttonUpChanged,    this, &GamepadController::upPressedChange);
    connect(gamepad, &QGamepad::buttonDownChanged,  this, &GamepadController::downPressedChange);
    connect(gamepad, &QGamepad::buttonLeftChanged,  this, &GamepadController::leftPressedChange);
    connect(gamepad, &QGamepad::buttonRightChanged, this, &GamepadController::rightPressedChange);
    connect(&cursorTimer, &QTimer::timeout, this, &GamepadController::updateCursorPosition);
  }
}

void GamepadController::updateCursorPosition()
{
  QPoint p = QCursor::pos();
  int ticks = cursorElapsedTimer.elapsed() / cursorTimer.interval();

  p.rx() += static_cast<int>(xMovement * 25) * ticks;
  p.ry() += static_cast<int>(yMovement * 25) * ticks;
  QCursor::setPos(p);
  cursorElapsedTimer.restart();
}

void GamepadController::updateCursorTimerState()
{
  if (yMovement == 0.0 && xMovement == 0.0)
    cursorTimer.stop();
  else if (!cursorTimer.isActive())
  {
    cursorTimer.start();
    cursorElapsedTimer.restart();
  }
}

void GamepadController::updateCursorOnXAxis(double value)
{
  xMovement = value;
  updateCursorTimerState();
}

void GamepadController::updateCursorOnYAxis(double value)
{
  yMovement = value;
  updateCursorTimerState();
}

void GamepadController::startPressedChange(bool pressed)
{
  if (!pressed)
    emit startClicked();
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
    swapUseMode("use-1");
}

void GamepadController::buttonL2PressedChange(bool pressed)
{
  if (!pressed)
    triggerItemUse("use-1");
}

void GamepadController::buttonR1PressedChange(bool pressed)
{
  if (!pressed)
    swapUseMode("use-2");
}

void GamepadController::buttonR2PressedChange(bool pressed)
{
  if (!pressed)
    triggerItemUse("use-2");
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
    auto* game = Game::get();

    if (game && game->getLevel())
      game->getLevel()->swapMouseMode();
  }
}

void GamepadController::clickEvent(bool pressed)
{
  MouseCursor::get()->click(pressed);
}

void GamepadController::swapUseMode(const QString &slotName)
{
  auto* game = Game::get();

  if (game && game->getLevel())
  {
    InventoryItem* item = game->getLevel()->getPlayer()->getInventory()->getEquippedItem(slotName);

    if (item)
      item->swapUseMode();
  }
}

void GamepadController::triggerItemUse(const QString &slotName)
{
  auto* game = Game::get();

  if (game && game->getLevel())
    game->getLevel()->setActiveItem(slotName);
}
