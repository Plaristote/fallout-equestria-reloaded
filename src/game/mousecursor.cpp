#include "mousecursor.h"
#include "globals.h"
#include "game.h"
#include <QPixmap>
#include <QGuiApplication>
#include <QDebug>
#include <QCursor>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QMouseEvent>

static const QString cursorPath = ":/assets/ui/cursors/";

static bool hasHoveredTile(LevelTask* level)
{
  const TileLayer* layer = level->getGrid()->getTilemap()->getLayer("ground");
  const QSize      size  = layer ? layer->getSize() : QSize();
  const QRect      rect(0, 0, size.width(), size.height());

  return rect.contains(level->getHoveredTilePosition());
}

static QPixmap makeEmptyPixmap()
{
  QPixmap view(5, 5);
  view.fill(Qt::transparent);
  return view;
}

template<bool NEW_VALUE = true>
struct BoolLock
{
  bool& value;
  BoolLock(bool& value) : value(value) { value = NEW_VALUE; }
  ~BoolLock() { value = !NEW_VALUE; }
};

MouseCursor* MouseCursor::instance = nullptr;

MouseCursor::MouseCursor(QGuiApplication* parent) : QObject(parent), emptyView(makeEmptyPixmap())
{
  QString pointerView(cursorPath + "interaction.png");
  QString activeView (cursorPath + "interaction-available.png");
  QString targetView (cursorPath + "target.png");
  QString moveView   (cursorPath + "move.png");

  instance = this;
  cursors.insert(NormalPointer, new AnimatedCursor(parent, pointerView, QSize(64, 50)));
  cursors.insert(ActivePointer, new AnimatedCursor(parent, activeView, QSize(64, 50)));
  cursors.insert(TargetPointer, new AnimatedCursor(parent, targetView, QSize(50, 50), QPoint(25, 25)));
  cursors.insert(MovePointer,   new AnimatedCursor(parent, moveView, QSize(72, 36), QPoint(5, -5)));
  cursors.insert(WaitPointer,   makeWaitCursor());
  cursors.insert(EmptyPointer,  new AnimatedCursor(parent));
  cursors[EmptyPointer]->addFrame(emptyView);

  connect(this, &MouseCursor::virtualPointerEnabledChanged, this, &MouseCursor::virtualPointerChanged);
  connect(this, &MouseCursor::pointerTypeChanged,           this, &MouseCursor::virtualPointerChanged);

  position = QPoint(0,0);
  if (parent)
    parent->installEventFilter(this);
}

AnimatedCursor* MouseCursor::makeWaitCursor() const
{
  QPixmap waitView(cursorPath + "wait.png");
  auto* waitCursor = new AnimatedCursor(reinterpret_cast<QGuiApplication*>(parent()));

  for (int i = 0 ; i < waitView.width() / 50 ; ++i)
  {
    QPixmap waitFrame = waitView.copy(i * 50, 0, 50, 50);

    waitCursor->addFrame(waitFrame.scaled(25, 25));
  }
  return waitCursor;
}

void MouseCursor::enableVirtualPointer()
{
  virtualPointerEnabled = true;
  emit virtualPointerEnabledChanged();
  cursors[currentPointer]->enable(false);
  cursors[EmptyPointer]->enable(true);
}

void MouseCursor::disableVirtualPointer()
{
  virtualPointerEnabled = false;
  emit virtualPointerEnabledChanged();
  cursors[currentPointer]->enable(true);
}

void MouseCursor::updatePointerType()
{
  LevelTask* level = LevelTask::get();

  if (level && level->mapIncludesMouse() && !level->isPaused())
  {
    switch (level->getMouseMode())
    {
    case InteractionComponent::MovementCursor:
      if (!hasHoveredTile(level))
        setCurrentPointer(NormalPointer);
      else
        setCurrentPointer(withMoveCursor ? MovePointer : EmptyPointer);
      break ;
    case InteractionComponent::InteractionCursor:
      setCurrentPointer(NormalPointer);
      break ;
    case InteractionComponent::TargetCursor:
      setCurrentPointer(TargetPointer);
      break ;
    case InteractionComponent::WaitCursor:
      setCurrentPointer(WaitPointer);
      break ;
    }
  }
  else
    setCurrentPointer(NormalPointer);
}

void MouseCursor::onObjectHovered(QObject* object)
{
  LevelTask* level = LevelTask::get();

  if (level->getMouseMode() == InteractionComponent::InteractionCursor)
    setCurrentPointer(object ? ActivePointer : NormalPointer);
}

void MouseCursor::setCurrentPointer(PointerType type)
{
  if (currentPointer != type)
  {
    if (!virtualPointerEnabled)
    {
      cursors[currentPointer]->enable(false);
      cursors[type]->enable(true);
    }
    currentPointer = type;
    emit pointerTypeChanged();
  }
}

QPoint MouseCursor::windowOffset() const
{
  if (window)
    return QPoint(window->property("x").toInt(), window->property("y").toInt());
  return QPoint(0, 0);
}

QSize MouseCursor::windowSize() const
{
  if (window)
    return QSize(window->property("width").toInt(), window->property("height").toInt());
  return QSize(0, 0);
}

QPoint MouseCursor::relativePosition() const
{
  return position;
}

bool MouseCursor::setRelativePosition(const QPoint& position)
{
  QSize limits(windowSize());

  if (position == this->position) return true;
  if (position.x() > 0 && position.y() > 0 && position.x() < limits.width() && position.y() < limits.height())
  {
    BoolLock simulationLock(simulatingEvent);
    QMouseEvent event(
      QEvent::MouseMove,
      QPointF(position),
      window->mapToGlobal(position),
      Qt::NoButton, Qt::NoButton, Qt::NoModifier
    );

    this->position = position;
    emit positionChanged(position);
    if (!virtualPointerEnabled)
      enableVirtualPointer();
    QCoreApplication::sendEvent(window, &event);
    return true;
  }
  return false;
}

void MouseCursor::click(bool pressed)
{
  if (window)
  {
    QMouseEvent event(
      pressed ? QEvent::MouseButtonPress : QEvent::MouseButtonRelease,
      QPointF(position),
      window->mapToGlobal(position),
      Qt::LeftButton,
      pressed ? Qt::LeftButton : Qt::NoButton,
      Qt::NoModifier
    );

    QCoreApplication::sendEvent(window, &event);
  }
}

void MouseCursor::onSystemMouseMoved(const QMouseEvent* event)
{
  //position = event->pos();
  position = (event->globalPosition() - windowOffset()).toPoint();
  if (virtualPointerEnabled)
      disableVirtualPointer();
  emit positionChanged(position);
}

bool MouseCursor::eventFilter(QObject* watched, QEvent* event)
{
 if (!simulatingEvent && event->type() == QEvent::MouseMove)
    onSystemMouseMoved(static_cast<QMouseEvent*>(event));
  return QObject::eventFilter(watched, event);
}

AnimatedCursor::AnimatedCursor(QGuiApplication* parent) : QObject(parent), currentFrame(0)
{
  timer.setInterval(250);
  connect(&timer, &QTimer::timeout, this, &AnimatedCursor::moveToNextFrame);
}

AnimatedCursor::AnimatedCursor(QGuiApplication* parent, const QString& texturePath, QSize size, QPoint hotPoint) : QObject(parent), currentFrame(0)
{
  initialize(size, hotPoint);
  addFrame(texturePath);
}

void AnimatedCursor::initialize(QSize a, QPoint b)
{
  this->cursorSize = a;
  this->hotPoint = b;
}

void AnimatedCursor::enable(bool enabled)
{
  if (enabled)
  {
    moveToNextFrame();
    if (frames.length() > 1)
      timer.start();
  }
  else
    timer.stop();
}

void AnimatedCursor::addFrame(const QString& texturePath)
{
  QPixmap pixmap(texturePath);
  framePaths << texturePath;
  frames << QCursor(pixmap.scaled(cursorSize), hotPoint.x(), hotPoint.y());
}

void AnimatedCursor::addFrame(const QPixmap& pixmap)
{
  frames << QCursor(pixmap);
}

void AnimatedCursor::moveToNextFrame()
{
  if (!frames.isEmpty())
  {
    currentFrame++;
    if (currentFrame >= frames.size())
      currentFrame = 0;
    reinterpret_cast<QGuiApplication*>(parent())->setOverrideCursor(frames[currentFrame]);
    emit currentFrameChanged();
  }
}
