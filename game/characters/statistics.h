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
  Q_PROPERTY(QString characterSheet MEMBER characterSheet NOTIFY characterSheetChanged)
public:
  explicit CharacterStatistics(QObject *parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  Q_INVOKABLE bool isAlive() const { return statistics->getHitPoints() > 0; }
  virtual bool isUnconscious() const { return isAlive(); }

  StatModel* getStatistics() const { return statistics; }
  void setStatistics(StatModel* value);
  void setCharacterSheet(const QString& name);
  bool getIsUnique() const { return isUnique; }
  void setUnique(bool value) { isUnique = value; }

public slots:
  void updateSpriteSheet();

private slots:
  void onCharacterSheetChanged();
  virtual void initializeFaction() = 0;

signals:
  void statisticsChanged();
  void characterSheetChanged();
  void uniqueChanged();

protected:
  QString characterSheet;
  StatModel* statistics = nullptr;
  bool isUnique = false;
};

#endif // CHARACTERSTATISTICS_H
