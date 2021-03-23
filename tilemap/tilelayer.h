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
class Limits;

class TileLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name    MEMBER name)
  Q_PROPERTY(QSize   size    MEMBER size)
  Q_PROPERTY(QPoint  offset  MEMBER offset)
  Q_PROPERTY(bool    visible MEMBER visible NOTIFY visibleChanged)
public:
  explicit TileLayer(QObject *parent = nullptr);

  void load(const QJsonObject&, const QVector<Tileset*>& tilesets);
  const QString& getName() const { return name; }
  const QSize& getSize() const { return size; }
  void setVisible(bool value) { visible = value; emit visibleChanged(); }
  bool isVisible() const { return visible; }
  void renderToFile(const QString& file) const;

  Q_INVOKABLE Tile* getTile(int x, int y) const;
  Q_INVOKABLE QSize getRenderedSize() const;
  Q_INVOKABLE QRect getRenderedRect() const;
  Q_INVOKABLE bool isInside(int x, int y) const;

signals:
  void visibleChanged();

private:
  void loadTiles(const QJsonArray&, const QVector<Tileset*>& tilesets);

  QString        name;
  QSize          size;
  QPoint         offset;
  QVector<Tile*> tiles;
  bool           visible = true;
};

#endif // TILELAYER_H
