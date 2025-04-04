#ifndef  TILELAYER_H
# define TILELAYER_H

# include "tilemask.h"
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

class TileLayer : public TileMask
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
  const QColor& getColor() const { return color; }
  void setColor(QColor value) { color = value; emit colorChanged(); }
  void setVisible(bool value) { if (visible != value) { visible = value; emit visibleChanged(); } }
  bool isVisible() const { return visible; }
  void renderToFile(const QString& file);
  virtual void renderToImage(QImage& image, QPoint offset);
  TileLayer* getMaskLayer() const;

  Q_INVOKABLE Tile* getTile(int x, int y) const;
  Q_INVOKABLE QSize getRenderedSize();
  Q_INVOKABLE QRect getRenderedRect();
  Q_INVOKABLE bool isInside(int x, int y) const override;

  void initialize(QSize);
  void fill(Tileset* tileset, int tileId);
  void fill(QRect rect, Tileset* tileset, int tileId);
  void setTileIdAt(int x, int y, Tileset* tileset, int tileId);
  void clear();

signals:
  void visibleChanged();
  void tilesChanged();
  void colorChanged();

protected:
  void loadTiles(const QJsonArray&, const QVector<Tileset*>& tilesets);
  void prepareRenderRect();
  void prepareRenderSize();

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
  typedef int QmlSizeType;
#else
  typedef qsizetype QmlSizeType;
#endif
  QQmlListProperty<Tile> getQmlTiles();
  static QmlSizeType     tilePropertyListCount(QQmlListProperty<Tile>*);
  static Tile*           tilePropertyListAt(QQmlListProperty<Tile>*, QmlSizeType);

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
