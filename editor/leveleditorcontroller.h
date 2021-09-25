#ifndef LEVELEDITORCONTROLLER_H
#define LEVELEDITORCONTROLLER_H

#include "game/leveltask.h"
#include <QObject>

class LevelEditorController : public LevelTask
{
  Q_OBJECT
public:
  LevelEditorController(QObject* parent = nullptr);

  Q_INVOKABLE DynamicObject* generateFromTemplate(const QString& templateName, const QString& name);
  Q_INVOKABLE void cut(StorableObject* object);
  Q_INVOKABLE void copy(StorableObject* object);
  Q_INVOKABLE StorableObject* pasteIn(ObjectGroup* target);

private slots:
  void update() override;

private:
  QQmlListProperty<Character> getQmlVisibleCharacters() override;
  QJsonObject clipper;
};

#endif // LEVELEDITORCONTROLLER_H
