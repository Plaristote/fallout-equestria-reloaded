#ifndef ORIENTEDSPRITE_H
#define ORIENTEDSPRITE_H

#include "sprite.h"
#include <QObject>

class OrientedSprite : public Sprite
{
  typedef Sprite ParentType;
  Q_OBJECT

  Q_PROPERTY(QString orientation READ getOrientationName WRITE setOrientation NOTIFY orientationChanged)
public:
  enum Direction
  {
    NoDir          = 0,
    UpperDir       = 1,
    LeftDir        = 2,
    RightDir       = 4,
    BottomDir      = 8,
    UpperLeftDir   = 3,
    UpperRightDir  = 5,
    BottomLeftDir  = 10,
    BottomRightDir = 14
  };
  Q_ENUM(Direction)

  explicit OrientedSprite(QObject *parent = nullptr);

  virtual void load(const QJsonObject&);
  virtual void save(QJsonObject&) const;

  QString       getAnimationBaseName() const;
  virtual void  setAnimation(const QString &animationName) override;
  QString       getOrientationName() const;
  Direction     getOrientation() const { return orientation; }
  void          setOrientation(const QString&);
  void          setOrientation(Direction);

signals:
  void orientationChanged();

protected:
  static const QMap<QString, OrientedSprite::Direction> directionByName;

private:
  Direction orientation = NoDir;
};

#endif // ORIENTEDSPRITE_H
