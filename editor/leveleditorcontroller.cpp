#include "leveleditorcontroller.h"

LevelEditorController::LevelEditorController(QObject* parent) : LevelTask(parent)
{
}

QQmlListProperty<Character> LevelEditorController::getQmlVisibleCharacters()
{
  visibleCharacters.clear();
  for (auto* object : qAsConst(objects))
  {
    if (object->isCharacter())
      visibleCharacters << reinterpret_cast<Character*>(object);
  }
  return QQmlListProperty<Character>(this, &visibleCharacters);
}

void LevelEditorController::update()
{
  qint64 delta = clock.restart();

  for (DynamicObject* object : qAsConst(objects))
    object->update(delta);
  emit updated();
}
