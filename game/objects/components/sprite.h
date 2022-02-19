#ifndef  SPRITE_H
# define SPRITE_H

# include <QRect>
# include "game/animationlibrary.h"
# include "controlzone.h"

class QJsonObject;

class Sprite : public ControlZoneComponent
{
  Q_OBJECT
  typedef ControlZoneComponent ParentType;

  Q_PROPERTY(bool floating MEMBER floating NOTIFY floatingChanged)
  Q_PROPERTY(QString spriteName     READ getSpriteName WRITE setSpriteName NOTIFY spriteGroupChanged)
  Q_PROPERTY(QString spriteSource   READ getSpriteSource   NOTIFY spriteSourceChanged)
  Q_PROPERTY(QPoint  spritePosition READ getSpritePosition NOTIFY spritePositionChanged)
  Q_PROPERTY(QRect   clippedRect    READ getClippedRect    NOTIFY clippedRectChanged)
public:
  explicit Sprite(QObject *parent = nullptr);

  Q_INVOKABLE void update(qint64);

  void    setSpriteName(const QString& value) { name = value; emit spriteGroupChanged(); }
  QString getSpriteName() const { return name; }
  Q_INVOKABLE virtual void setAnimation(const QString& animationName);
  Q_INVOKABLE QString getAnimation() const { return animation.name; }
  Q_INVOKABLE virtual bool hasAnimation(const QString& animationName) const;
  const QImage& getImage() const;
  void moveToCoordinates(QPoint coordinates);
  Q_INVOKABLE void setRenderPosition(QPoint coordinates);
  bool isAnimated() const;
  virtual bool isMoving() const { return spritePosition != spriteMovementTarget; }
  inline bool isFloating() const { return floating; }
  void setMovementSpeed(float value) { movementSpeed = value; }

  Q_INVOKABLE QString getSpriteSource() const { return animation.source; }
  Q_INVOKABLE QRect   getClippedRect() const  { return animation.clippedRect; }
  Q_INVOKABLE QPoint  getSpritePosition() const { return spritePosition; }
  Q_INVOKABLE QString getCurrentAnimation() const { return animation.name; }
  Q_INVOKABLE QString getShadowSource() const { return shadow.source; }

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

signals:
  void spriteGroupChanged();
  void spriteSourceChanged();
  void spritePositionChanged();
  void clippedRectChanged();
  void animationFinished();
  void movementFinished(Sprite*);
  void floatingChanged();

private:
  void runAnimation();
  void runMovement(qint64);

private:
  QPoint          spritePosition, spriteMovementTarget;
  bool            floating = false;
  QString         name;
  SpriteAnimation shadow;
  SpriteAnimation animation;
  qint64          animationElapsedTime;
protected:
  float           movementSpeed;
};

#endif // SPRITE_H
