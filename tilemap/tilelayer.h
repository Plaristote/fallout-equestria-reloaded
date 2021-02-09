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

  Q_PROPERTY(QString name    MEMBER name)
  Q_PROPERTY(QSize   size    MEMBER size)
  Q_PROPERTY(QPoint  offset  MEMBER offset)
  Q_PROPERTY(bool    visible MEMBER visible)
public:
  explicit TileLayer(QObject *parent = nullptr);

  void load(const QJsonObject&, const QVector<Tileset*>& tilesets);
  const QString& getName() const { return name; }
  void setVisible(bool value) { visible = value; }
  bool isVisible() const { return visible; }

  Q_INVOKABLE Tile* getTile(int x, int y);

private:
  void loadTiles(const QJsonArray&, const QVector<Tileset*>& tilesets);

  QString        name;
  QSize          size;
  QPoint         offset;
  QVector<Tile*> tiles;
  bool           visible = true;
};

#endif // TILELAYER_H
