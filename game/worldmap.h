#ifndef  WORLDMAP_H
# define WORLDMAP_H

# include <QObject>
# include <QPoint>
# include <QSize>
# include <QTimer>
# include <QQmlListProperty>
# include "timermanager.h"
# include "worldmapcity.h"

class WorldMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QSize  mapSize MEMBER mapSize NOTIFY mapSizeChanged)
  Q_PROPERTY(QSize  caseSize MEMBER caseSize NOTIFY mapSizeChanged)
  Q_PROPERTY(QSize  caseCount MEMBER caseCount NOTIFY mapSizeChanged)
  Q_PROPERTY(QPoint currentPosition MEMBER currentPosition NOTIFY currentPositionChanged)
  Q_PROPERTY(QPoint targetPosition  MEMBER targetPosition  NOTIFY targetPositionChanged)
  Q_PROPERTY(TimeManager* timeManager MEMBER timeManager NOTIFY timeManagerChanged)
  Q_PROPERTY(QQmlListProperty<WorldMapCity> cities READ getCitiesQml NOTIFY citiesChanged)
public:
  WorldMap(QObject* parent = nullptr);

  void load(const QJsonObject&);
  QJsonObject save() const;

  QQmlListProperty<WorldMapCity> getCitiesQml() { return QQmlListProperty<WorldMapCity>(this, &cities); }

  Q_INVOKABLE void getIntoCity(WorldMapCity*);
  Q_INVOKABLE void getIntoWasteland(QPoint);
  Q_INVOKABLE bool isVisible(int x, int y) const;
  Q_INVOKABLE void revealCaseAt(int x, int y) { revealCaseAt(QPoint(x, y)); }
  void             revealCaseAt(const QPoint);

signals:
  void currentPositionChanged();
  void targetPositionChanged();
  void mapSizeChanged();
  void timeManagerChanged();
  void citiesChanged();
  void cityEntered(QString);
  void caseRevealed(int caseX, int caseY);

private slots:
  void update();
  void onCurrentPositionChanged();
  void onTargetPositionChanged();
  void onTargetPositionReached();

private:
  QTimer updateTimer;
  QPoint currentPosition, targetPosition;
  TimeManager* timeManager;
  QList<WorldMapCity*> cities;
  QVector<bool> discovered;
  QSize         mapSize;
  QSize         caseSize, caseCount;
};

#endif // WORLDMAP_H
