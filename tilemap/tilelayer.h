#ifndef  TILELAYER_H
# define TILELAYER_H

# include <QObject>
# include <QString>
# include <QSize>
# include <QPoint>
# include <QColor>
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
  Q_PROPERTY(QColor  color   MEMBER color)
  Q_PROPERTY(bool    visible MEMBER visible NOTIFY visibleChanged)
  Q_PROPERTY(bool    prerendered MEMBER prerendered)
public:
  explicit TileLayer(QObject *parent = nullptr);

  void load(const QJsonObject&, const QVector<Tileset*>& tilesets);
  const QString& getName() const { return name; }
  const QSize& getSize() const { return size; }
  void setVisible(bool value) { visible = value; emit visibleChanged(); }
  bool isVisible() const { return visible; }
  void renderToFile(const QString& file);
  void renderToImage(QImage& image, QPoint offset);

  Q_INVOKABLE Tile* getTile(int x, int y) const;
  Q_INVOKABLE QSize getRenderedSize();
  Q_INVOKABLE QRect getRenderedRect();
  Q_INVOKABLE bool isInside(int x, int y) const;

  void initialize(QSize);
  void fill(Tileset* tileset, int tileId);
  void fill(QRect rect, Tileset* tileset, int tileId);
  void setTileIdAt(int x, int y, Tileset* tileset, int tileId);
  void clear();

signals:
  void visibleChanged();

private:
  void loadTiles(const QJsonArray&, const QVector<Tileset*>& tilesets);
  void prepareTile(Tile*, const Tileset*, int tid, QPoint position);
  void prepareRenderRect();
  void prepareRenderSize();

  QString        name;
  QSize          size;
  QPoint         offset;
  QColor         color;
  QVector<Tile*> tiles;
  bool           visible = true;
  bool           prerendered = false;
  bool           dirtyRenderRect = true, dirtyRenderSize = true;
  QRect          renderRect;
  QSize          renderSize;
};

#endif // TILELAYER_H
