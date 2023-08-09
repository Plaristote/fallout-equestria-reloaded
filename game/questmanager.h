#ifndef QUESTMANAGER_H
#define QUESTMANAGER_H

#include <QObject>
#include <QQmlListProperty>
#include <QJsonObject>
#include "quest.h"
#include "globals.h"

class QuestManager : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QQmlListProperty<Quest> list READ getQmlList NOTIFY listChanged)
public:
  enum QuestFlags
  {
    NormalQuest = 0,
    HiddenQuest = 1
  };
  Q_ENUM(QuestFlags)

  explicit QuestManager(QObject *parent = nullptr);

  void load(const QJsonObject&);
  QJsonObject save() const;

  Q_INVOKABLE Quest* addQuest(const QString&, int flags = NormalQuest);
  Q_INVOKABLE Quest* getQuest(const QString&) const;
  Q_INVOKABLE bool   hasQuest(const QString& name) const { return getQuest(name) != nullptr; }

  QQmlListProperty<Quest> getQmlList() { return QML_QLIST_CONSTRUCTOR(Quest, list); }

signals:
  void listChanged();

public slots:
  void onCharacterKilled(Character* victim, Character* killer);
  void onItemPicked(InventoryItem* item);
private slots:
  void onLevelChanged();

private:
  QList<Quest*> list;
};

#endif // QUESTMANAGER_H
