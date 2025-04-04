#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "../dynamicobject.h"

class LevelGrid;

class Elevator : public DynamicObject
{
  Q_OBJECT

  Q_PROPERTY(QPoint       positionA MEMBER positionA NOTIFY elevatorChanged)
  Q_PROPERTY(unsigned int floorA    MEMBER floorA    NOTIFY elevatorChanged)
  Q_PROPERTY(QPoint       positionB MEMBER positionB NOTIFY elevatorChanged)
  Q_PROPERTY(unsigned int floorB    MEMBER floorB    NOTIFY elevatorChanged)
public:
  explicit Elevator(QObject *parent = nullptr);

  void load(const QJsonObject&) override;
  void save(QJsonObject&) const override;

  QStringList getAvailableInteractions() override;
  Q_INVOKABLE void setPositionA(int x, int y, unsigned int z) { setPositionA({x,y}, static_cast<unsigned char>(z)); }
  Q_INVOKABLE void setPositionB(int x, int y, unsigned int z) { setPositionB({x,y}, static_cast<unsigned char>(z)); }
  void setPositionA(QPoint, unsigned char);
  void setPositionB(QPoint, unsigned char);

  bool isValid() const { return floorA != NULL_FLOOR && floorB != NULL_FLOOR; }
  void connectCases();
  void disconnectCases();
  bool triggerInteraction(Character*, const QString& interactionType) override;

signals:
  void elevatorChanged();

public slots:
  void onLevelFloorChanged();

private:
  void updatePositions();

  QPoint positionA, positionB;
  unsigned char floorA, floorB;
};

#endif // ELEVATOR_H
