#ifndef WORLDMAPCITY_H
#define WORLDMAPCITY_H

#include <QObject>
#include <QString>
#include <QPoint>

class WorldMapCity : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name     MEMBER name     NOTIFY nameChanged)
  Q_PROPERTY(QString level    MEMBER level    NOTIFY levelChanged)
  Q_PROPERTY(QPoint  position MEMBER position NOTIFY positionChanged)
  Q_PROPERTY(int     size     MEMBER size     NOTIFY sizeChanged)
public:
  explicit WorldMapCity(QObject* parent = nullptr);

  const QString& getName() const { return name; }
  const QString& getLevel() const { return level; }
  QPoint         getPosition() const { return position; }
  int            getSize() const { return size; }

  void setName(const QString& value) { name = value; }
  void setLevel(const QString& value) { level = value; }
  void setPosition(QPoint value) { position = value; }
  void setSize(int value) { size = value; }

  Q_INVOKABLE bool isInside(QPoint);

signals:
  void nameChanged();
  void levelChanged();
  void positionChanged();
  void sizeChanged();

private:
  QString name, level;
  QPoint  position;
  int     size;
};

#endif // WORLDMAPCITY_H
