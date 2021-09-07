#ifndef  GAMEPADCONTROLLER_H
# define GAMEPADCONTROLLER_H

# include <QObject>
# include <QTimer>
# include <QElapsedTimer>

class QGamepad;

class GamepadController : public QObject
{
  Q_OBJECT

  static GamepadController* instance;
public:
  explicit GamepadController(QObject *parent = nullptr);

  static GamepadController* get() { return instance; }

signals:
  void cameraAxisClicked();
  void moveCameraXAxis(double movement);
  void moveCameraYAxis(double movement);
  void startClicked();
  void backClicked();
  void skilldexClicked();
  void inventoryClicked();
  void leftClicked();
  void rightClicked();
  void upClicked();
  void bottomClicked();

private slots:
  void movementAxisClicked(bool pressed);
  void updateCursorOnXAxis(double value);
  void updateCursorOnYAxis(double value);
  void startPressedChange(bool pressed);
  void buttonBPressedChange(bool pressed);
  void buttonAPressedChange(bool pressed) { clickEvent(pressed); }
  void buttonYPressedChange(bool pressed);
  void buttonXPressedChange(bool pressed);
  void buttonR1PressedChange(bool pressed);
  void buttonR2PressedChange(bool pressed);
  void buttonL1PressedChange(bool pressed);
  void buttonL2PressedChange(bool pressed);
  void upPressedChange(bool pressed);
  void downPressedChange(bool pressed);
  void leftPressedChange(bool pressed);
  void rightPressedChange(bool pressed);
  void updateCursorPosition();

private:
  void clickEvent(bool);
  void swapUseMode(const QString& slotName);
  void triggerItemUse(const QString& slotName);
  void updateCursorTimerState();

  QGamepad* gamepad = nullptr;
  QTimer    cursorTimer;
  QElapsedTimer cursorElapsedTimer;
  double    xMovement, yMovement;
};

#endif // GAMEPADCONTROLLER_H
