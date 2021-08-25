#ifndef  DETECTABLECOMPONENT_H
 #define DETECTABLECOMPONENT_H

# include "scriptable.h"

class Character;

class DetectableComponent : public ScriptableComponent
{
  Q_OBJECT
  typedef ScriptableComponent ParentType;

  Q_PROPERTY(bool sneaking MEMBER sneaking NOTIFY sneakingChanged)
  Q_PROPERTY(bool hidden MEMBER hidden NOTIFY hiddenChanged)
  Q_PROPERTY(bool interruptOnDetection MEMBER interruptOnDetection NOTIFY interruptOnDetectionChanged)
public:
  explicit DetectableComponent(QObject *parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  bool tryDetection(const Character*);
  bool isHidden() const { return hidden; }
  bool isSneaking() const { return sneaking; }
  Q_INVOKABLE virtual void toggleSneaking(bool);
  Q_INVOKABLE virtual int getSneakAbility() const;

signals:
  void sneakingChanged();
  void hiddenChanged();
  void detected();
  void interruptOnDetectionChanged();

private slots:
  void onDetected();

private:
  bool sneaking = false;
  bool hidden = false;
  bool interruptOnDetection = false;
};

#endif // DETECTABLECOMPONENT_H
