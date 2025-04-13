#ifndef  SAVECOMPONENT_H
# define SAVECOMPONENT_H

# include "clock.h"

class TutorialComponent;
class TimeManager;

class SaveComponent : public ClockComponent
{
  Q_OBJECT
  typedef ClockComponent ParentType;

  Q_PROPERTY(bool canSave READ canSave NOTIFY canSaveChanged)
  Q_PROPERTY(bool persistent MEMBER persistent NOTIFY persistentChanged)
public:
  explicit SaveComponent(QObject *parent = nullptr);

  void load(const QString& levelName, DataEngine*);
  void passElapsedTime(int lastUpdate);
  void loadTutorial();
  void save(DataEngine*, bool isActive);
  bool canSave() const;

  Q_INVOKABLE void toggleSaveEnabled(bool);
  Q_INVOKABLE void persist();

signals:
  void canSaveChanged();
  void persistentChanged();
  void tutorialChanged();

protected:
  bool persistent = true;
  bool saveEnabled = false;
  TutorialComponent* tutorial = nullptr;
};

#endif // SAVECOMPONENT_H
