#ifndef WORLDMAPZONE_H
#define WORLDMAPZONE_H

#include <QObject>
#include <QPoint>
#include <QMap>
#include <QVector>
#include <QJsonObject>

class WorldMapZone : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged)
  Q_PROPERTY(int movementSpeed MEMBER movementSpeed NOTIFY movementSpeedChanged)
public:
  explicit WorldMapZone(QObject *parent = nullptr);

  void load(const QJsonObject&);
  QJsonObject save() const;

  const QString&     getName() const { return name; }
  Q_INVOKABLE int    caseCount() const { return cases.size(); }
  Q_INVOKABLE QPoint caseAt(int index) const { return index < cases.size() ? cases[index] : QPoint(); }
  Q_INVOKABLE bool   containsCase(int x, int y) const { return cases.contains(QPoint(x, y)); }
  Q_INVOKABLE void   addCase(int x, int y)    { addCase(QPoint(x, y)); }
  Q_INVOKABLE void   removeCase(int x, int y) { removeCase(QPoint(x, y)); }
  void               addCase(QPoint position);
  void               removeCase(QPoint position);
  int                getMovementSpeed() const { return movementSpeed; }

  const QVector<QPoint>& getCases() const { return cases; }

signals:
  void nameChanged();
  void casesChanged();
  void movementSpeedChanged();

private:
  QString            name;
  QVector<QPoint>    cases;
  QMap<QString, int> encounters;
  int                movementSpeed;
};

#endif // WORLDMAPZONE_H
