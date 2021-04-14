#ifndef  TILEZONE_H
# define TILEZONE_H

# include <QObject>
# include <QSize>
# include <QPoint>
# include <QRect>

class QJsonObject;
class DynamicObject;

class TileZone : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name MEMBER name)
  Q_PROPERTY(QString type MEMBER type)
  Q_PROPERTY(QPoint  offset MEMBER offset NOTIFY tilesChanged)
  Q_PROPERTY(QRect   clippedRect MEMBER clippedRect)
  Q_PROPERTY(bool    accessBlocked MEMBER accessBlocked)
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
  Q_INVOKABLE bool isInside(int x, int y) const;
  const QList<QPoint>& getPositions() const { return positions; }
  QVector<QPoint> getAbsolutePositions() const;
  void setOffset(QPoint value) { offset = value; emit tilesChanged(); }
  QPoint getOffset() const { return offset; }

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

private:
  QString       name;
  QString       type;
  QString       target, targetZone;
  bool          isDefault = false;
  bool          accessBlocked = false;
  QRect         clippedRect;
  QList<QPoint> positions;
  QPoint        offset;
};

#endif // TILEZONE_H
