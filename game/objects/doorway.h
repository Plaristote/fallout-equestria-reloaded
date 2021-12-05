#ifndef  DOORWAY_H
# define DOORWAY_H

# include "../dynamicobject.h"

class Doorway : public DynamicObject
{
  Q_OBJECT

  Q_PROPERTY(bool    opened        MEMBER opened NOTIFY openedChanged)
  Q_PROPERTY(bool    locked        MEMBER locked NOTIFY lockedChanged)
  Q_PROPERTY(bool    destroyed     MEMBER destroyed NOTIFY destroyedChanged)
  Q_PROPERTY(QString keyName       MEMBER keyName NOTIFY keyNameChanged)
  Q_PROPERTY(int     lockpickLevel MEMBER lockpickLevel NOTIFY lockpickLevelChanged)
  Q_PROPERTY(bool    destructible  READ isDestructible)
  Q_PROPERTY(QString openSound   MEMBER openSound   NOTIFY soundChanged)
  Q_PROPERTY(QString closeSound  MEMBER closeSound  NOTIFY soundChanged);
  Q_PROPERTY(QString lockedSound MEMBER lockedSound NOTIFY soundChanged);
public:
  Doorway(QObject* parent = nullptr);

  void save(QJsonObject&) const;
  void load(const QJsonObject&);

  QVector<Point> getInteractionPositions() const;
  int getInteractionDistance() const override { return 0; }
  QStringList getAvailableInteractions() override;
  Q_INVOKABLE bool bustOpen(int damage);
  bool isBlockingPath() const { return false; }
  bool triggerInteraction(Character* character, const QString& interactionType) override;
  bool isDestructible() const;
  int getCoverValue() const override;
  Q_INVOKABLE bool onGoThrough(Character*);
  Q_INVOKABLE bool canGoThrough(Character*) const;

  Q_INVOKABLE bool onUse(Character* character);

signals:
  void destroyedChanged();
  void openedChanged();
  void lockedChanged();
  void keyNameChanged();
  void lockpickLevelChanged();
  void soundChanged();

public slots:
  void updateTileConnections();
  void removeTileConnections();
private slots:
  void updateAccessPath();
  void updateAnimation();

private:
  bool      destroyed = false;
  bool      opened = false;
  bool      locked = false;
  QString   keyName;
  int       lockpickLevel;
  QString   openSound, closeSound, lockedSound;
  QVector<void*> tileConnections;
};

#endif // DOORWAY_H
