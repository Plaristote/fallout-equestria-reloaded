#ifndef  CHARACTER_H
# define CHARACTER_H

# include "globals.h"
# include "storageobject.h"
# include "cmap/statmodel.h"
# include "diplomacy.hpp"
# include "characters/field_of_view.hpp"

class Character : public StorageObject
{
  Q_OBJECT

  Q_PROPERTY(StatModel*   statistics  MEMBER statistics)
  Q_PROPERTY(FieldOfView* fieldOfView MEMBER fieldOfView)
  Q_PROPERTY(bool actionPoints MEMBER actionPoints NOTIFY actionPointsChanged)
public:
  explicit Character(QObject *parent = nullptr);

  void update(qint64) override;
  void load(const QJsonObject&) override;
  void save(QJsonObject&) const override;
  void setScript(const QString&) override;

  void         setStatistics(StatModel* value) { statistics = value; }
  StatModel*   getStatistics() const { return statistics; }
  QString      getFactionName() const { return faction ? faction->name : QString(); }
  unsigned int getFactionFlag() const { return faction ? faction->flag : 0; }
  QString      getDialogName();
  bool         getIsUnique() const { return isUnique; }
  void         setUnique(bool value) { isUnique = value; }

  Q_INVOKABLE bool renderOnTile() const { return true; }
  Q_INVOKABLE QPoint getInteractionPosition() const override;
  bool isBlockingPath() const override { return isAlive(); }

  Q_INVOKABLE bool isAlive() const { return getStatistics()->getHitPoints() > 0; }
  Q_INVOKABLE bool isAlly(const Character*) const;
  Q_INVOKABLE bool isEnemy(const Character*) const;
  Q_INVOKABLE bool hasLineOfSight(const Character*) const;
  Q_INVOKABLE bool isSneaking() const { return sneakEnabled; }
  Q_INVOKABLE float getDistance(const DynamicObject*) const;

  Q_INVOKABLE bool useActionPoints(int amount = 1, const QString& motive = "");
  inline int       getActionPoints() const { return actionPoints; }
  void             resetActionPoints();

  Q_INVOKABLE void updateInventorySlots();

signals:
  void actionPointsChanged();

protected:
  virtual QString getScriptPath() const override { return SCRIPTS_PATH + "pnjs"; }

private slots:
  void initializeFaction();
private:
  StatModel* statistics = nullptr;
  FieldOfView* fieldOfView;
  WorldDiplomacy::Faction* faction = nullptr;
  bool isUnique = false;
  bool sneakEnabled = false;
  unsigned int enemyFlag = 0;
  int actionPoints = 0;
};

#endif // CHARACTER_H
