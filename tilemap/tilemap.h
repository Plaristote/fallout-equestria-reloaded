#ifndef  TILEMAP_H
# define TILEMAP_H

# include <QObject>
# include <QSize>
# include "tileset.h"
# include "tilelayer.h"

class TileMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QSize tileSize MEMBER tileSize)
  Q_PROPERTY(QSize mapSize MEMBER mapSize)
public:
  explicit TileMap(QObject *parent = nullptr);

  bool load(const QString& name);

  inline const QSize& getSize() const { return mapSize; }
  inline const QSize& getTileSize() const { return tileSize; }

  Q_INVOKABLE TileLayer* getLayer(const QString& name);

private:
  QSize               tileSize;
  QSize               mapSize;
  QVector<Tileset*>   tilesets;
  QVector<TileLayer*> layers;
};

#endif // LEVELMAP_H
