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
public:
  explicit TileZone(QObject *parent = nullptr);

  void load(const QJsonObject&, const QSize mapSize);
  const QString& getName() const { return name; }
  const QString& getType() const { return type; }
  const QString& getTarget() const { return target; }
  bool getIsDefault() const { return isDefault; }

  Q_INVOKABLE bool isInside(int x, int y) const;

signals:
  void enteredZone(DynamicObject*, TileZone*);
  void exitedZone(DynamicObject*, TileZone*);

private:
  QString         name;
  QString         type;
  QString         target;
  bool            isDefault = false;
  QRect           clippedRect;
  QVector<QPoint> tiles;
};

#endif // TILEZONE_H
