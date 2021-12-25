#include "mousecursor.h"
#include "globals.h"
#include "game.h"
#include <QPixmap>
#include <QGuiApplication>
#include <QDebug>
#include <QCursor>
#include <QGuiApplication>
#include <QMouseEvent>

static const QString cursorPath = ":/assets/ui/cursors/";

static bool hasHoveredTile(LevelTask* level)
{
  const TileLayer* layer = level->getGrid()->getTilemap()->getLayer("ground");
  const QSize      size  = layer ? layer->getSize() : QSize();
  const QRect      rect(0, 0, size.width(), size.height());

  return rect.contains(level->getHoveredTilePosition());
}

MouseCursor* MouseCursor::instance = nullptr;

MouseCursor::MouseCursor(QGuiApplication* parent) : QObject(parent)
{
  QPixmap pointerView(cursorPath + "interaction.png");
  QPixmap activeView (cursorPath + "interaction-available.png");
  QPixmap targetView (cursorPath + "target.png");
  QPixmap emptyView(5, 5);

  emptyView.fill(Qt::transparent);
  instance = this;
  cursors.insert(NormalPointer, new AnimatedCursor(parent, pointerView.scaled(64, 50)));
  cursors.insert(ActivePointer, new AnimatedCursor(parent, activeView.scaled(64, 50)));
  cursors.insert(TargetPointer, new AnimatedCursor(parent, targetView.scaled(50, 50), QPoint(25, 25)));
  cursors.insert(WaitPointer,   makeWaitCursor());
  cursors.insert(EmptyPointer,  new AnimatedCursor(parent, emptyView));
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

void MouseCursor::updatePointerType()
{
  Game* game = Game::get();
  LevelTask* level = game ? game->getLevel() : nullptr;

  if (level && level->mapIncludesMouse() && !level->isPaused())
  {
    switch (level->getMouseMode())
    {
    case InteractionComponent::MovementCursor:
      if (!hasHoveredTile(level))
        setCurrentPointer(NormalPointer);
      else
        setCurrentPointer(EmptyPointer);
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

void MouseCursor::setCurrentPointer(PointerType type)
{
  if (currentPointer != type)
  {
    cursors[currentPointer]->enable(false);
    cursors[type]->enable(true);
    currentPointer = type;
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
  return QCursor::pos() - windowOffset();
}

bool MouseCursor::setRelativePosition(const QPoint& position)
{
  QSize limits(windowSize());

  if (position.x() > 0 && position.y() > 0 && position.x() < limits.width() && position.y() < limits.height())
  {
    QCursor::setPos(position + windowOffset());
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
      QCursor::pos() - windowOffset(),
      Qt::LeftButton,
      Qt::NoButton,
      Qt::NoModifier
    );

    QGuiApplication::sendEvent(window, &event);
  }
}

AnimatedCursor::AnimatedCursor(QGuiApplication* parent) : QObject(parent), currentFrame(0)
{
  timer.setInterval(250);
  connect(&timer, &QTimer::timeout, this, &AnimatedCursor::moveToNextFrame);
}

AnimatedCursor::AnimatedCursor(QGuiApplication* parent, const QPixmap& pixmap, QPoint hotPoint) : QObject(parent), currentFrame(0)
{
  addFrame(pixmap, hotPoint);
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

void AnimatedCursor::addFrame(QPixmap pixmap, QPoint hotPoint)
{
  frames << QCursor(pixmap, hotPoint.x(), hotPoint.y());
}

void AnimatedCursor::moveToNextFrame()
{
  if (!frames.isEmpty())
  {
    currentFrame++;
    if (currentFrame >= frames.size())
      currentFrame = 0;
    reinterpret_cast<QGuiApplication*>(parent())->setOverrideCursor(frames[currentFrame]);
  }
}
