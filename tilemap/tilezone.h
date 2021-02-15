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
  Q_PROPERTY(QRect   clippedRect MEMBER clippedRect)
  Q_PROPERTY(bool    accessBlocked MEMBER accessBlocked)
public:
  explicit TileZone(QObject *parent = nullptr);

  void load(const QJsonObject&, const QSize mapSize);
  const QString& getName() const { return name; }
  const QString& getType() const { return type; }
  const QString& getTarget() const { return target; }
  bool getIsDefault() const { return isDefault; }

  inline bool getAccessBlocked() const { return accessBlocked; }
  inline void setAccessBlocked(bool value) { accessBlocked = value; }
  Q_INVOKABLE bool isInside(int x, int y) const;
  const QList<QPoint>& getPositions() const { return tiles; }

  Q_INVOKABLE int    getPositionCount() const { return tiles.size(); }
  Q_INVOKABLE QPoint getPositionAt(int i) const { return tiles.at(i); }
  Q_INVOKABLE void   addPosition(QPoint);
  Q_INVOKABLE void   removePosition(QPoint);

signals:
  void enteredZone(DynamicObject*, TileZone*);
  void exitedZone(DynamicObject*, TileZone*);
  void tilesChanged();

private:
  QString       name;
  QString       type;
  QString       target;
  bool          isDefault = false;
  bool          accessBlocked = false;
  QRect         clippedRect;
  QList<QPoint> tiles;
};

#endif // TILEZONE_H
