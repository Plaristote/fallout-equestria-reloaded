#ifndef  TILE_H
# define TILE_H

# include <QObject>
# include <QRect>

class Tileset;

class Tile : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString image          READ getImage         CONSTANT)
  Q_PROPERTY(QRect   clippedRect    MEMBER rect           CONSTANT)
  Q_PROPERTY(QPoint  position       MEMBER position       CONSTANT)
  Q_PROPERTY(QPoint  renderPosition MEMBER renderPosition CONSTANT)
public:
  explicit Tile(QObject* parent = nullptr);

  void prepare(QPoint offset, const Tileset*, int tid, QPoint position);
  void setRect(const QRect& value) { rect = value; }
  void setPosition(const QPoint& value) { position = value; }
  void setRenderPosition(const QPoint& value) { renderPosition = value; }
  void setImage(const QString* value) { image = value; }
  void setTexture(const QImage* value) { texture = value; }
  inline const QImage& getTexture() const { return *texture; }
  inline const QString& getImage() const { return *image; }
  inline const QRect& getRect() const { return rect; }
  inline const QPoint& getPosition() const { return position; }
  inline const QPoint& getRenderPosition() const { return renderPosition; }
  inline QRect getRenderRect() const { return QRect(renderPosition, rect.size()); }
  QVariant getProperty(const QByteArray& name) const;

private:
  int            tid = 0;
  const QString* image = nullptr;
  const QImage*  texture;
  const Tileset* tileset;
  QRect          rect;
  QPoint         position;
  QPoint         renderPosition;
};

#endif // TILE_H
