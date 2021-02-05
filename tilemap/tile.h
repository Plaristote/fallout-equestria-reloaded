#ifndef  TILE_H
# define TILE_H

# include <QObject>
# include <QRect>

class Tile : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString image       READ getImage)
  Q_PROPERTY(QRect   clippedRect MEMBER rect)
  Q_PROPERTY(QPoint  position    MEMBER position)
public:
  explicit Tile(QObject* parent = nullptr);

  void setRect(const QRect& value) { rect = value; }
  void setPosition(const QPoint& value) { position = value; }
  void setImage(const QString* value) { image = value; }
  inline const QString& getImage() const { return *image; }
  inline const QRect& getRect() const { return rect; }
  inline const QPoint& getPosition() const { return position; }

private:
  const QString* image;
  QRect          rect;
  QPoint         position;
};

#endif // TILE_H
