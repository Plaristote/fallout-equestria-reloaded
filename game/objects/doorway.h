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
public:
  Doorway(QObject* parent = nullptr);

  void save(QJsonObject&) const;
  void load(const QJsonObject&);

  QStringList getAvailableInteractions() override;
  Q_INVOKABLE bool bustOpen(int damage);
  bool isBlockingPath() const { return !opened; }
  bool triggerInteraction(Character* character, const QString& interactionType) override;
  bool isDestructible() const;

  Q_INVOKABLE bool onUse(Character* character);

signals:
  void destroyedChanged();
  void openedChanged();
  void lockedChanged();
  void keyNameChanged();
  void lockpickLevelChanged();

private slots:
  void updateAccessPath();
  void updateAnimation();

private:
  bool      destroyed = false;
  bool      opened = false;
  bool      locked = false;
  QString   keyName;
  int       lockpickLevel;
};

#endif // DOORWAY_H
