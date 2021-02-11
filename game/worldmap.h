#ifndef  WORLDMAP_H
# define WORLDMAP_H

# include <QObject>
# include <QPoint>
# include <QTimer>
# include <QQmlListProperty>
# include "timermanager.h"
# include "worldmapcity.h"

class WorldMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QPoint currentPosition MEMBER currentPosition NOTIFY currentPositionChanged)
  Q_PROPERTY(QPoint targetPosition  MEMBER targetPosition  NOTIFY targetPositionChanged)
  Q_PROPERTY(TimeManager* timeManager MEMBER timeManager NOTIFY timeManagerChanged)
  Q_PROPERTY(QQmlListProperty<WorldMapCity> cities READ getCitiesQml NOTIFY citiesChanged)
public:
  WorldMap(QObject* parent = nullptr);

  QQmlListProperty<WorldMapCity> getCitiesQml() { return QQmlListProperty<WorldMapCity>(this, &cities); }

  Q_INVOKABLE void getIntoCity(WorldMapCity*);
  Q_INVOKABLE void getIntoWasteland(QPoint);

signals:
  void currentPositionChanged();
  void targetPositionChanged();
  void timeManagerChanged();
  void citiesChanged();
  void cityEntered(QString);

private slots:
  void update();
  void onTargetPositionChanged();
  void onTargetPositionReached();

private:
  QTimer updateTimer;
  QPoint currentPosition, targetPosition;
  TimeManager* timeManager;
  QList<WorldMapCity*> cities;
};

#endif // WORLDMAP_H
