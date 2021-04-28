#include "mousecursor.h"
#include "globals.h"
#include "game.h"
#include <QPixmap>
#include <QGuiApplication>
#include <QDebug>
#include <QCursor>

static const QString cursorPath = ASSETS_PATH + "ui/cursors/";

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
  InteractionComponent* level = game ? game->getLevel() : nullptr;

  if (level && level->mapIncludesMouse())
  {
    switch (level->getMouseMode())
    {
    case InteractionComponent::MovementCursor:
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
