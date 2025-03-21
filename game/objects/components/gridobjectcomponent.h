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
  Q_PROPERTY(int cover MEMBER cover NOTIFY coverChanged)
public:
  explicit GridObjectComponent(QObject *parent = nullptr);

  virtual QPoint getPosition() const { return position; }
  virtual Point  getPoint() const { return {position.x(), position.y(), floor}; }
  virtual void   setPosition(QPoint value) { position = value; emit positionChanged(); }
  unsigned int   getCurrentFloor() const { return floor; }
  virtual void   setCurrentFloor(unsigned int value);
  virtual int    getCoverValue() const { return cover; }

  Q_INVOKABLE QJSValue positionSplat() const;

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

signals:
  void positionChanged();
  void floorChanged();
  void coverChanged();

protected:
  QPoint        position;
  unsigned char floor = 0;
  char          cover = 100;
};

#endif // GRIDOBJECTCOMPONENT_H
