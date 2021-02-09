#ifndef  SPRITE_H
# define SPRITE_H

# include <QObject>
# include <QTimer>
# include <QSize>
# include <QRect>
# include "animationlibrary.h"

class Sprite : public QObject
{
  Q_OBJECT

public:
  explicit Sprite(QObject *parent = nullptr);

  void setSpriteName(const QString& value) { name = value; }
  void setAnimation(const QString& animationName);
  void moveToCoordinates(QPoint coordinates);
  void forceMoveToCoordinates(QPoint coordinates);
  void setSpriteAnimation(const SpriteAnimation& value) { animation = value; }

  Q_INVOKABLE QString getSpriteSource() const { return animation.source; }
  Q_INVOKABLE QRect   getClippedRect() const  { return animation.clippedRect; }
  Q_INVOKABLE QPoint  getSpritePosition() const { return spritePosition; }
  Q_INVOKABLE QString getCurrentAnimation() const { return animation.name; }
  Q_INVOKABLE QString getShadowSource() const { return shadow.source; }
  Q_INVOKABLE bool    renderOnTile() const { return false; }

signals:
  void animationFinished();
  void movementFinished(Sprite*);

private slots:
  void runAnimation();
  void runMovement();

private:
  QPoint          spritePosition, spriteMovementTarget;
  QString         name;
  SpriteAnimation shadow;
  SpriteAnimation animation;
  QTimer          animationTimer;
  QTimer          movementTimer;
  float           movementSpeed;
};

#endif // SPRITE_H
