#ifndef  TILESET_H
# define TILESET_H

# include <QObject>
# include <QRect>
# include <QImage>
# include <QHash>
# include <QVariantMap>

class QJsonDocument;

class Tileset : public QObject
{
  Q_OBJECT

  typedef QHash<int, QVariantMap> TileProps;
public:

  explicit Tileset(QObject *parent = nullptr);

  bool load(const QString& filepath, int firstGid = 1);

  inline const QString& getName() const { return name; }
  inline const QString& getSource() const { return source; }
  QRect getClipRectFor(int tileId) const;
  QVariant getProperty(int tileId, const QByteArray& name) const;
  QSize getTileSize() const { return tileSize; }
  int getFirstGid() const { return firstGid; }
  int getLastGid() const { return firstGid + tileCount - 1; }
  int getTileCount() const { return tileCount; }
  void overrideLastGid(int value) { tileCount = value - firstGid; }
  const QImage& getImage() const { return image; }
  bool isInRange(int tileId) { return tileId >= firstGid && tileId < firstGid + tileCount; }
  inline int getGidAt(int i) const { return firstGid + i; }

private:
  void loadProperties(const QJsonDocument&);

  QString    tiledVersion;
  QImage     image;
  QString    name;
  QString    source;
  int        columns;
  QSize      tileSize, sourceSize;
  int        spacing;
  int        tileCount;
  int        firstGid;
  TileProps  tileProperties;
};

#endif // TILESET_H
