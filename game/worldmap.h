#ifndef  WORLDMAP_H
# define WORLDMAP_H

# include <QObject>
# include <QPoint>
# include <QSize>
# include <QTimer>
# include <QQmlListProperty>
# include "timermanager.h"
# include "worldmapcity.h"
# include "worldmapzone.h"

class WorldMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QSize  mapSize   MEMBER mapSize NOTIFY mapSizeChanged)
  Q_PROPERTY(QSize  caseSize  MEMBER caseSize NOTIFY mapSizeChanged)
  Q_PROPERTY(QSize  caseCount MEMBER caseCount NOTIFY caseCountChanged)
  Q_PROPERTY(QPoint currentPosition MEMBER currentPosition NOTIFY currentPositionChanged)
  Q_PROPERTY(QPoint targetPosition  MEMBER targetPosition  NOTIFY targetPositionChanged)
  Q_PROPERTY(TimeManager* timeManager MEMBER timeManager NOTIFY timeManagerChanged)
  Q_PROPERTY(QQmlListProperty<WorldMapCity> cities READ getCitiesQml NOTIFY citiesChanged)
  Q_PROPERTY(QQmlListProperty<WorldMapZone> zones READ getZonesQml NOTIFY zonesChanged)
  Q_PROPERTY(QStringList discoveredCities MEMBER discoveredCities NOTIFY discoveredCitiesChanged)
public:
  WorldMap(QObject* parent = nullptr);

  void load(const QJsonObject&);
  QJsonObject save() const;

  QQmlListProperty<WorldMapCity> getCitiesQml() { return QQmlListProperty<WorldMapCity>(this, &cities); }
  QQmlListProperty<WorldMapZone> getZonesQml() { return QQmlListProperty<WorldMapZone>(this, &zones); }

  Q_INVOKABLE void getIntoCity(WorldMapCity*);
  Q_INVOKABLE void getIntoWasteland(QPoint);
  Q_INVOKABLE bool isVisible(int x, int y) const;
  Q_INVOKABLE void revealCaseAt(int x, int y) { revealCaseAt(QPoint(x, y)); }
  void             revealCaseAt(const QPoint);
  Q_INVOKABLE void revealCity(const QString& name);
  Q_INVOKABLE void revealCity(WorldMapCity*);

  Q_INVOKABLE WorldMapCity* getCity(const QString& name) const;
  Q_INVOKABLE WorldMapZone* getZone(const QString& zone) const;
  Q_INVOKABLE WorldMapCity* createCity(const QString& name);
  Q_INVOKABLE WorldMapZone* createZone(const QString& name);
  Q_INVOKABLE void removeCity(WorldMapCity*);
  Q_INVOKABLE void removeZone(WorldMapZone*);

  Q_INVOKABLE QPoint getCaseAt(QPoint position) const;
  Q_INVOKABLE WorldMapZone* getCurrentZone() const;

signals:
  void currentPositionChanged();
  void targetPositionChanged();
  void mapSizeChanged();
  void caseCountChanged();
  void timeManagerChanged();
  void citiesChanged();
  void cityEntered(QString);
  void zonesChanged();
  void caseRevealed(int caseX, int caseY);
  void discoveredCitiesChanged();

private slots:
  void update();
  void onCurrentPositionChanged();
  void onTargetPositionChanged();
  void onTargetPositionReached();

private slots:
  void onMapSizeChanged();

private:
  float getCurrentMovementSpeed() const;

  QTimer updateTimer;
  QPoint currentPosition, targetPosition;
  TimeManager* timeManager;
  QList<WorldMapCity*> cities;
  QList<WorldMapZone*> zones;
  QVector<bool> discovered;
  QStringList   discoveredCities;
  QSize         mapSize;
  QSize         caseSize, caseCount;
};

#endif // WORLDMAP_H
