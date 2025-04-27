#ifndef  GAMEPADCONTROLLER_H
# define GAMEPADCONTROLLER_H

# include <QObject>
# include <QTimer>
# include <QElapsedTimer>

class QGamepad;
class MouseCursor;

class GamepadController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool enabled READ isEnabled NOTIFY connectedChanged)
  Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
  Q_PROPERTY(bool hoverScrollable MEMBER hoverScrollable NOTIFY hoverScrollableChanged)

  struct CursorTimer
  {
    CursorTimer();
    void updateState();
    long ticks() const { return timer.elapsed() / ticker.interval(); }
    QTimer        ticker;
    QElapsedTimer timer;
    double        x, y;
  };

  static GamepadController* instance;
public:
  explicit GamepadController(QObject *parent = nullptr);

  void initialize();
  static GamepadController* get() { return instance; }

  bool isConnected() const;
  bool isEnabled() const;

signals:
  void hoverScrollableChanged();
  void connectedChanged();
  void cameraAxisClicked();
  void moveCameraXAxis(double movement);
  void moveCameraYAxis(double movement);
  void startClicked();
  void selectClicked();
  void backClicked();
  void skilldexClicked();
  void inventoryClicked();
  void leftClicked();
  void rightClicked();
  void upClicked();
  void bottomClicked();
  void leftTriggerClicked();
  void rightTriggerClicked();
  void altLeftTriggerClicked();
  void altRightTriggerClicked();

public slots:
  void lookForController();

private slots:
  void movementAxisClicked(bool pressed);
  void updateCursorPosition();
  void updateCursorOnXAxis(double value);
  void updateCursorOnYAxis(double value);
  void updateCameraPosition();
  void updateCameraOnXAxis(double value);
  void updateCameraOnYAxis(double value);
  void startPressedChange(bool pressed);
  void selectPressedChange(bool pressed);
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

private:
  void connectGamepad(int);
  void clickEvent(bool);
  bool isLevelMode() const;
  void swapUseMode(const QString& slotName);
  void triggerItemUse(const QString& slotName);
  void updateCursorTimerState();

  MouseCursor* cursor = nullptr;
  QGamepad* gamepad = nullptr;
  CursorTimer cursorTimer, cameraTimer;
  double    xMovement, yMovement;
  bool hoverScrollable = false;
  bool initialized = false;

  QTimer windowsWorkaroundTimer;
};

#endif // GAMEPADCONTROLLER_H
