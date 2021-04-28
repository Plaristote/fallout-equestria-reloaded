#ifndef  MOUSECURSOR_H
# define MOUSECURSOR_H

# include <QObject>
# include <QCursor>
# include <QMap>


# include <QTimer>

class QGuiApplication;

class AnimatedCursor : public QObject
{
  Q_OBJECT
public:
  AnimatedCursor(QGuiApplication* parent);
  AnimatedCursor(QGuiApplication* parent, const QPixmap& pixmap, QPoint hotPoint = QPoint(0, 0));

  void enable(bool);
  void addFrame(QPixmap pixmap, QPoint hotPoint = QPoint(0, 0));

private slots:
  void moveToNextFrame();

private:
  QTimer           timer;
  QVector<QCursor> frames;
  unsigned int     currentFrame;
};

class MouseCursor : public QObject
{
  Q_OBJECT

  enum PointerType
  {
    NormalPointer,
    ActivePointer,
    TargetPointer,
    WaitPointer,
    EmptyPointer
  };

  static MouseCursor* instance;

public:
  explicit MouseCursor(QGuiApplication *parent = nullptr);

  static MouseCursor* get() { return instance; }

public slots:
  void updatePointerType();

private:
  void setCurrentPointer(PointerType);
  AnimatedCursor* makeWaitCursor() const;

  QMap<PointerType, AnimatedCursor*> cursors;
  PointerType currentPointer = EmptyPointer;
};

#endif // MOUSECURSOR_H
