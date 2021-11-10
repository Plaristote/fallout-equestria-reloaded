#ifndef  TILEZONE_H
# define TILEZONE_H

# include <QObject>
# include <QSize>
# include <QPoint>
# include <QRect>
# include <QVector>

class QJsonObject;
class DynamicObject;

class TileZone : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString      name MEMBER name CONSTANT)
  Q_PROPERTY(QString      type MEMBER type CONSTANT)
  Q_PROPERTY(QPoint       offset MEMBER offset NOTIFY tilesChanged)
  Q_PROPERTY(unsigned int floor READ getFloor NOTIFY floorChanged)
  Q_PROPERTY(QRect        clippedRect MEMBER clippedRect CONSTANT)
  Q_PROPERTY(bool         accessBlocked MEMBER accessBlocked)
  Q_PROPERTY(int          positionCount READ getPositionCount NOTIFY tilesChanged)
public:
  explicit TileZone(QObject *parent = nullptr);

  void load(const QJsonObject&, const QSize mapSize);
  const QString& getName() const { return name; }
  const QString& getType() const { return type; }
  const QString& getTarget() const { return target; }
  const QString& getTargetZone() const { return targetZone; }
  bool getIsDefault() const { return isDefault; }

  inline bool getAccessBlocked() const { return accessBlocked; }
  inline void setAccessBlocked(bool value) { accessBlocked = value; }
  Q_INVOKABLE bool isInside(int x, int y, unsigned char z) const;
  const QList<QPoint>& getPositions() const { return positions; }
  QVector<QPoint> getAbsolutePositions() const;
  void setOffset(QPoint value) { offset = value; emit tilesChanged(); }
  void setOffset(QPoint value, unsigned char newFloor) { if (floor != newFloor) { floor = newFloor; emit floorChanged(this); } offset = value; emit tilesChanged();  }
  QPoint getOffset() const { return offset; }
  unsigned int getFloor() const { return static_cast<unsigned int>(floor); }
  void setFloor(unsigned char value) { floor = value; emit floorChanged(this); }

  Q_INVOKABLE int    getPositionCount() const { return positions.size(); }
  Q_INVOKABLE QPoint getPositionAt(int i) const { return offset + positions.at(i); }
  Q_INVOKABLE QPoint getRelativePosition(int i) const { return positions.at(i); }
  Q_INVOKABLE void   addPosition(QPoint);
  Q_INVOKABLE void   removePosition(QPoint);
  Q_INVOKABLE void   addRelativePosition(QPoint);
  Q_INVOKABLE void   removeRelativePosition(QPoint);

signals:
  void enteredZone(DynamicObject*, TileZone*);
  void exitedZone(DynamicObject*, TileZone*);
  void tilesChanged();
  void floorChanged(TileZone*);

private:
  QString       name;
  QString       type;
  QString       target, targetZone;
  bool          isDefault = false;
  bool          accessBlocked = false;
  QRect         clippedRect;
  QList<QPoint> positions;
  QPoint        offset;
  unsigned char floor = 0;
};

#endif // TILEZONE_H
