#ifndef  OBJECTTILESET_H
# define OBJECTTILESET_H

# include <QObject>
# include <QSize>
# include <QMap>
# include <QStringList>

class ObjectTileset : public QObject
{
  Q_OBJECT

  struct Tile {
    QString source;
    QSize   size;
  };

public:
  explicit ObjectTileset(QObject *parent = nullptr);

  bool load(const QString& filepath, int firstGid = 1);

  const QString& getName() const { return name; }
  int getFirstGid() const { return firstGid; }
  int getLastGid() const { return firstGid + tiles.size(); }
  QString getSource(int gid) const;
  QSize   getTileSize(int gid) const;
  QStringList getAllSources() const;

signals:

private:
  QString name;
  QMap<int, Tile> tiles;
  int firstGid;
};

#endif // OBJECTTILESET_H
