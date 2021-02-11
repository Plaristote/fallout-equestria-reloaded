#ifndef WORLDMAPCITY_H
#define WORLDMAPCITY_H

#include <QObject>
#include <QString>
#include <QPoint>

class WorldMapCity : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name     MEMBER name     NOTIFY nameChanged)
  Q_PROPERTY(QPoint  position MEMBER position NOTIFY positionChanged)
  Q_PROPERTY(int     size     MEMBER size     NOTIFY sizeChanged)
public:
  WorldMapCity(QObject* parent = nullptr);

  const QString& getName() const { return name; }
  QPoint         getPosition() const { return position; }

  void setName(const QString& value) { name = value; }
  void setPosition(QPoint value) { position = value; }
  void setSize(int value) { size = value; }

  Q_INVOKABLE bool isInside(QPoint);

signals:
  void nameChanged();
  void positionChanged();
  void sizeChanged();

private:
  QString name;
  QPoint  position;
  int     size;
};

#endif // WORLDMAPCITY_H
