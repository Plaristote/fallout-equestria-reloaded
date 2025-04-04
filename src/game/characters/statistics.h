#ifndef CHARACTERSTATISTICS_H
# define CHARACTERSTATISTICS_H

# include "movement.h"
# include "cmap/statmodel.h"

class CharacterStatistics : public CharacterMovement
{
  Q_OBJECT
  typedef CharacterMovement ParentType;

  Q_PROPERTY(StatModel* statistics READ getStatistics WRITE setStatistics NOTIFY statisticsChanged)
  Q_PROPERTY(bool isUnique MEMBER isUnique NOTIFY uniqueChanged)
  Q_PROPERTY(bool unconscious READ isUnconscious NOTIFY unconsciousChanged)
  Q_PROPERTY(bool alive READ isAlive NOTIFY died)
  Q_PROPERTY(QString characterSheet MEMBER characterSheet NOTIFY characterSheetChanged)
public:
  explicit CharacterStatistics(QObject *parent = nullptr);

  void load(const QJsonObject&) override;
  void save(QJsonObject&) const override;

  Q_INVOKABLE bool isAlive() const { return statistics ? statistics->getHitPoints() > 0 : true; }
  virtual bool isUnconscious() const { return !isAlive(); }

  StatModel* getStatistics() const { return statistics; }
  void setStatistics(StatModel* value);
  QString getCharacterSheet() const { return characterSheet; }
  void setCharacterSheet(const QString& name);
  bool getIsUnique() const { return isUnique; }
  void setUnique(bool value) { isUnique = value; }
  QString getDisplayName() const override;

public slots:
  void updateSpriteSheet();

private slots:
  void onCharacterSheetChanged();
  virtual void initializeFaction() = 0;

signals:
  void statisticsChanged();
  void characterSheetChanged();
  void raceChanged();
  void uniqueChanged();
  void died();
  void unconsciousChanged();

protected:
  QString characterSheet;
  StatModel* statistics = nullptr;
  bool isUnique = false;
};

#endif // CHARACTERSTATISTICS_H
