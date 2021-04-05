#ifndef  TILESET_H
# define TILESET_H

# include <QObject>
# include <QRect>
# include <QImage>

class Tileset : public QObject
{
  Q_OBJECT
public:
  explicit Tileset(QObject *parent = nullptr);

  bool load(const QString& filepath, int firstGid = 1);

  inline const QString& getName() const { return name; }
  inline const QString& getSource() const { return source; }
  QRect getClipRectFor(int tileId) const;
  QSize getTileSize() const { return tileSize; }
  int getFirstGid() const { return firstGid; }
  int getLastGid() const { return firstGid + tileCount - 1; }
  int getTileCount() const { return tileCount; }
  const QImage& getImage() const { return image; }
  bool isInRange(int tileId) { return tileId >= firstGid && tileId < firstGid + tileCount; }
  inline int getGidAt(int i) const { return firstGid + i; }

private:
  QImage  image;
  QString name;
  QString source;
  int     columns;
  QSize   tileSize, sourceSize;
  int     spacing;
  int     tileCount;
  int     firstGid;
};

#endif // TILESET_H
