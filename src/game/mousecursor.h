#ifndef  MOUSECURSOR_H
# define MOUSECURSOR_H

# include <QObject>
# include <QCursor>
# include <QMap>
# include <QTimer>
# include <QPixmap>

class QGuiApplication;
class QQuickWindow;
class QMouseEvent;

/*
 * This is a bit more complicated than it could be.
 * MouseCursor handles the appearence of the system cursor.
 * But it also provide data for rendering a virtual cursor
 * from the QML user interface.
 * It's unfortunately not all that's happening here.
 * It also handles both real mouse events, and virtual mouse
 * events that could be triggered by shortcuts or through
 * the GamepadController.
 */

class AnimatedCursor : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QSize        size     MEMBER cursorSize CONSTANT)
  Q_PROPERTY(QPoint       hotPoint MEMBER hotPoint CONSTANT)
  Q_PROPERTY(QStringList  frames   MEMBER framePaths CONSTANT)
  Q_PROPERTY(unsigned int frameIndex MEMBER currentFrame NOTIFY currentFrameChanged)
public:
  AnimatedCursor(QGuiApplication* parent);
  AnimatedCursor(QGuiApplication* parent, const QString& texturePath, QSize cursorSize, QPoint hotPoint = QPoint(0, 0));

  void enable(bool);
  void initialize(QSize a, QPoint b);
  void addFrame(const QString& texturePath);
  void addFrame(const QPixmap&);

signals:
  void currentFrameChanged();

private slots:
  void moveToNextFrame();

private:
  QTimer           timer;
  QVector<QCursor> frames;
  QStringList      framePaths;
  QSize            cursorSize;
  QPoint           hotPoint;
  unsigned int     currentFrame;
};

class MouseCursor : public QObject
{
  Q_OBJECT
public:
  enum PointerType
  {
    NormalPointer,
    ActivePointer,
    MovePointer,
    TargetPointer,
    WaitPointer,
    EmptyPointer
  };
  Q_ENUM(PointerType)

private:
  static MouseCursor* instance;

  Q_PROPERTY(bool            withMoveCursor        MEMBER withMoveCursor        NOTIFY withMoveCursorChanged)
  Q_PROPERTY(bool            virtualPointerEnabled MEMBER virtualPointerEnabled NOTIFY virtualPointerEnabledChanged)
  Q_PROPERTY(QPoint          position              MEMBER position              NOTIFY positionChanged)
  Q_PROPERTY(PointerType     pointerType           MEMBER currentPointer        NOTIFY pointerTypeChanged)
  Q_PROPERTY(AnimatedCursor* virtualPointer        READ getVirtualPointer       NOTIFY virtualPointerChanged)
public:
  explicit MouseCursor(QGuiApplication *parent = nullptr);

  static MouseCursor* get() { return instance; }

  QPoint windowOffset() const;
  QSize  windowSize() const;
  QPoint relativePosition() const;
  Q_INVOKABLE bool setRelativePosition(const QPoint&);
  void setWindow(QQuickWindow* value) { window = value; }
  AnimatedCursor* getVirtualPointer() const { return virtualPointerEnabled ? cursors[currentPointer] : nullptr; }

public slots:
  void click(bool pressed);
  void updatePointerType();
  void setCurrentPointer(PointerType);
  void onObjectHovered(QObject*);

signals:
  void withMoveCursorChanged();
  void virtualPointerEnabledChanged();
  void positionChanged(QPoint);
  void pointerTypeChanged();
  void virtualPointerChanged();

private:
  AnimatedCursor* makeWaitCursor() const;
  void enableVirtualPointer();
  void disableVirtualPointer();
  bool eventFilter(QObject* watched, QEvent* event) override;
  void onSystemMouseMoved(const QMouseEvent*);

  QMap<PointerType, AnimatedCursor*> cursors;
  PointerType currentPointer = EmptyPointer;
  QQuickWindow* window = nullptr;
  bool withMoveCursor = false;
  bool virtualPointerEnabled = false;
  QPoint position;
  const QPixmap emptyView;
  bool simulatingEvent = false;
};

#endif // MOUSECURSOR_H
