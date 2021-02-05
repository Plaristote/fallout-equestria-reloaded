#ifndef  TILESET_H
# define TILESET_H

# include <QObject>
# include <QRect>

class Tileset : public QObject
{
  Q_OBJECT
public:
  explicit Tileset(QObject *parent = nullptr);

  bool load(const QString& filepath, int firstGid = 1);

  inline const QString& getSource() const { return source; }
  QRect getClipRectFor(int tileId) const;
  bool isInRange(int tileId) { return tileId >= firstGid && tileId < firstGid + tileCount; }

private:
  QString name;
  QString source;
  int     columns;
  QSize   tileSize, sourceSize;
  int     spacing;
  int     tileCount;
  int     firstGid;
};

#endif // TILESET_H
