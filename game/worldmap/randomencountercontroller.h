#ifndef  RANDOMENCOUNTERCONTROLLER_H
# define RANDOMENCOUNTERCONTROLLER_H

# include <QObject>
# include <QTimer>
# include <QVariantMap>

class RandomEncounterController : public QObject
{
  Q_OBJECT
public:
  RandomEncounterController(QObject* parent = nullptr);

public slots:
  void prepareEncounter(const QString&, const QVariantMap& = QVariantMap());
  void startEncounter(const QString&, const QVariantMap& = QVariantMap());
  void triggerScheduledEncounter();

private slots:
  void initializeEncounter();

private:
  QTimer      encounterTimer;
  QString     scheduledEncounterName;
  QVariantMap scheduledEncounter;
};

#endif // RANDOMENCOUNTERCONTROLLER_H
