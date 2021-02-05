#ifndef  TILELAYER_H
# define TILELAYER_H

# include <QObject>
# include <QString>
# include <QSize>
# include <QPoint>
# include <QVector>
# include "tile.h"

class QJsonObject;
class QJsonArray;
class Tileset;

class TileLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name MEMBER name)
  Q_PROPERTY(QSize   size MEMBER size)
public:
  explicit TileLayer(QObject *parent = nullptr);

  void load(const QJsonObject&, const QVector<Tileset*>& tilesets);
  const QString& getName() const { return name; }

  Q_INVOKABLE Tile* getTile(int x, int y);

private:
  void loadTiles(const QJsonArray&, const QVector<Tileset*>& tilesets);

  QString        name;
  QSize          size;
  QPoint         offset;
  QVector<Tile*> tiles;
};

#endif // TILELAYER_H
