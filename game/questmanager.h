#ifndef QUESTMANAGER_H
#define QUESTMANAGER_H

#include <QObject>
#include <QQmlListProperty>
#include <QJsonObject>
#include "quest.h"

class QuestManager : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QQmlListProperty<Quest> list READ getQmlList NOTIFY listChanged)
public:
  explicit QuestManager(QObject *parent = nullptr);

  void load(const QJsonObject&);
  QJsonObject save() const;

  Q_INVOKABLE void   addQuest(const QString&);
  Q_INVOKABLE Quest* getQuest(const QString&);

  QQmlListProperty<Quest> getQmlList() { return QQmlListProperty<Quest>(this, &list); }

signals:
  void listChanged();

public slots:
  void onCharacterKilled(Character* victim, Character* killer);
  void onItemPicked(InventoryItem* item);

private:
  QList<Quest*> list;
};

#endif // QUESTMANAGER_H
