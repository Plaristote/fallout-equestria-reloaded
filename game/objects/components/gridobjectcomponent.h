#ifndef  GRIDOBJECTCOMPONENT_H
# define GRIDOBJECTCOMPONENT_H

# include "scriptable.h"
# include "utils/point.h"
# include <climits>

# define NULL_FLOOR UCHAR_MAX

class GridObjectComponent : public ScriptableComponent
{
  Q_OBJECT

  Q_PROPERTY(QPoint position READ getPosition WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(unsigned int floor READ getCurrentFloor WRITE setCurrentFloor NOTIFY floorChanged)
public:
  explicit GridObjectComponent(QObject *parent = nullptr);

  virtual QPoint getPosition() const { return position; }
  virtual Point  getPoint() const { return {position.x(), position.y(), floor}; }
  virtual void   setPosition(QPoint value) { position = value; emit positionChanged(); }
  unsigned int   getCurrentFloor() const { return floor; }
  virtual void   setCurrentFloor(unsigned int value) { floor = static_cast<unsigned short>(value); emit floorChanged(); }

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

signals:
  void positionChanged();
  void floorChanged();

protected:
  QPoint        position;
  unsigned char floor = 0;
};

#endif // GRIDOBJECTCOMPONENT_H
