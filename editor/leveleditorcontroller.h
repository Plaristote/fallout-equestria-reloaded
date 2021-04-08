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

private slots:
  void update() override;

private:
  QQmlListProperty<Character> getQmlVisibleCharacters() override;
};

#endif // LEVELEDITORCONTROLLER_H
