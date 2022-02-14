#ifndef  TILELAYER_H
# define TILELAYER_H

# include <QObject>
# include <QString>
# include <QSize>
# include <QPoint>
# include <QColor>
# include <QVector>
# include <QQmlListProperty>
# include "tile.h"

class QJsonObject;
class QJsonArray;
class Tileset;
class Limits;

class TileLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name    MEMBER name CONSTANT)
  Q_PROPERTY(QSize   size    MEMBER size)
  Q_PROPERTY(QPoint  offset  MEMBER offset)
  Q_PROPERTY(QColor  color   MEMBER color)
  Q_PROPERTY(bool    visible MEMBER visible NOTIFY visibleChanged)
  Q_PROPERTY(bool    prerendered MEMBER prerendered CONSTANT)
  Q_PROPERTY(QQmlListProperty<Tile> tiles READ getQmlTiles NOTIFY tilesChanged)
public:
  explicit TileLayer(QObject *parent = nullptr);

  void load(const QJsonObject&, const QVector<Tileset*>& tilesets);
  const QString& getName() const { return name; }
  const QString& getZoneName() const { return zoneName; }
  const QSize& getSize() const { return size; }
  void setVisible(bool value) { if (visible != value) { visible = value; emit visibleChanged(); } }
  bool isVisible() const { return visible; }
  void renderToFile(const QString& file);
  virtual void renderToImage(QImage& image, QPoint offset);

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
  void tilesChanged();

protected:
  void loadTiles(const QJsonArray&, const QVector<Tileset*>& tilesets);
  void prepareRenderRect();
  void prepareRenderSize();
  QQmlListProperty<Tile> getQmlTiles();
  static int             tilePropertyListCount(QQmlListProperty<Tile>*);
  static Tile*           tilePropertyListAt(QQmlListProperty<Tile>*, int);

  QString        name, zoneName;
  QSize          size;
  QPoint         offset;
  QColor         color = Qt::transparent;
  QVector<Tile*> tiles;
  bool           visible = true;
  bool           prerendered = false;
  bool           dirtyRenderRect = true, dirtyRenderSize = true;
  QRect          renderRect;
  QSize          renderSize;
};

#endif // TILELAYER_H
