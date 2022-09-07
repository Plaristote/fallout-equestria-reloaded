#ifndef WORLDMAPCITY_H
#define WORLDMAPCITY_H

#include <QObject>
#include <QString>
#include <QPoint>
#include <QMap>

class QJsonObject;

class CitySplashscreen : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString imagePath MEMBER imagePath NOTIFY imageChanged)
  Q_PROPERTY(QStringList entryPoints READ getEntryPoints NOTIFY entryPointsChanged)
public:
  explicit CitySplashscreen(QObject* parent = nullptr) : QObject(parent) {}

  void load(const QJsonObject&);
  void save(QJsonObject&);

  QStringList getEntryPoints() const { return entryPoints.keys(); }
  Q_INVOKABLE void addEntryPoint(const QString& key) { entryPoints.insert(key, QPoint()); emit entryPointsChanged(); }
  Q_INVOKABLE void removeEntryPoint(const QString& key) { entryPoints.remove(key); emit entryPointsChanged(); }
  Q_INVOKABLE QPoint getPositionFor(const QString& key) const { return entryPoints.contains(key) ? entryPoints[key] : QPoint(); }
  Q_INVOKABLE void setPositionFor(const QString& key, QPoint position);// { entryPoints.insert(key, position); emit entryPointsChanged(); }
  Q_INVOKABLE void setEntryPointAt(const QString& key, const QString& level);// { entryPoints.insert(level, entryPoints[key]); entryPoints.remove(key); emit entryPointsChanged(); }

signals:
  void imageChanged();
  void entryPointsChanged();

private:
  QString imagePath;
  QMap<QString, QPoint> entryPoints;
};

class WorldMapCity : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name     MEMBER name     NOTIFY nameChanged)
  Q_PROPERTY(QString level    MEMBER level    NOTIFY levelChanged)
  Q_PROPERTY(QPoint  position MEMBER position NOTIFY positionChanged)
  Q_PROPERTY(int     size     MEMBER size     NOTIFY sizeChanged)
  Q_PROPERTY(CitySplashscreen* splashscreen READ getSplashscreen NOTIFY splashscreenChanged)
public:
  explicit WorldMapCity(QObject* parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&);

  const QString& getName() const { return name; }
  const QString& getLevel() const { return level; }
  QPoint         getPosition() const { return position; }
  int            getSize() const { return size; }
  CitySplashscreen* getSplashscreen() const { return splashscreen; }

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
  void splashscreenChanged();

private:
  QString name, level;
  QPoint  position;
  int     size;
  CitySplashscreen* splashscreen;
};

#endif // WORLDMAPCITY_H
