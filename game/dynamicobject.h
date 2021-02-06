#ifndef  DYNAMICOBJECT_H
# define DYNAMICOBJECT_H

# include <QObject>
# include <QPoint>
# include "sprite.h"

class DynamicObject : public Sprite
{
  Q_OBJECT

  Q_PROPERTY(Sprite* sprite MEMBER sprite)
public:
  explicit DynamicObject(QObject *parent = nullptr);

  Q_INVOKABLE QPoint getPosition() const { return position; }
  void setPosition(QPoint value) { position = value; }

signals:

private:
  QPoint position;
  Sprite* sprite;
};

#endif // DYNAMICOBJECT_H
