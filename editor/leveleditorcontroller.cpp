#include "leveleditorcontroller.h"
#include "gameobjecttemplates.h"
#include "game/objects/doorway.h"
#include "game/objects/objectfactory.h"

QJsonObject LevelEditorController::clipper;

LevelEditorController::LevelEditorController(QObject* parent) : LevelTask(parent)
{
}

DynamicObject* LevelEditorController::generateFromTemplate(const QString& templateName, const QString& name)
{
  QJsonObject    objectData = GameObjectTemplates::get()->getObjectData(templateName);
  QString        type = objectData["_type"].toString();
  DynamicObject* object;

  if (type == "Character")
    object = new Character(this);
  else if (type == "StorageObject")
    object = new StorageObject(this);
  else if (type == "Doorway")
    object = new Doorway(this);
  else if (type == "InventoryItem")
    object = new InventoryItem(this);
  else
    object = new DynamicObject(this);
  object->load(objectData);
  object->setObjectName(name);
  object->setPosition(QPoint(-1, -1));
  object->setRenderPosition(object->getSpritePosition()); // isn't this basically self-assign ?
  appendObject(object);
  return object;
}

QQmlListProperty<DynamicObject> LevelEditorController::getQmlVisibleObjects()
{
  visibleObjects.clear();
  for (auto* object : std::as_const(attachedObjects))
  {
    if (!object->isCharacter() && object->getCurrentFloor() == getCurrentFloor())
      visibleObjects << reinterpret_cast<Character*>(object);
  }
  return QML_QLIST_CONSTRUCTOR(DynamicObject, visibleObjects);
}

QQmlListProperty<Character> LevelEditorController::getQmlVisibleCharacters()
{
  visibleCharacters.clear();
  for (auto* object : std::as_const(attachedObjects))
  {
    if (object->isCharacter() && object->getCurrentFloor() == getCurrentFloor())
      visibleCharacters << reinterpret_cast<Character*>(object);
  }
  return QML_QLIST_CONSTRUCTOR(Character, visibleCharacters);
}

void LevelEditorController::swapMouseMode()
{
  switch (mouseMode)
  {
    case WaitCursor:
    case InteractionCursor:
    case TargetCursor:
      mouseMode = MovementCursor;
      break ;
    default:
      mouseMode = InteractionCursor;
      break ;
  }
  emit mouseModeChanged();
}

void LevelEditorController::update()
{
  qint64 delta = clock.restart();

  for (DynamicObject* object : std::as_const(objects))
    object->update(delta);
  emit updated();
}

void LevelEditorController::cut(StorableObject* object)
{
  static DynamicObject sample;

  copy(object);
  if (object->metaObject()->inherits(sample.metaObject()))

    reinterpret_cast<DynamicObject*>(object)->save(clipper);
  else
    reinterpret_cast<ObjectGroup*>(object)->save(clipper);
}


void LevelEditorController::copy(StorableObject* object)
{
  static DynamicObject sample;

  clipper = QJsonObject{{"type", object->metaObject()->className()}};
  if (object->metaObject()->inherits(sample.metaObject()))
    reinterpret_cast<DynamicObject*>(object)->save(clipper);
  else
    reinterpret_cast<ObjectGroup*>(object)->save(clipper);
  emit clipperChanged();
}

template<typename RESULT>
static RESULT* pasteIn(ObjectGroup* target, const QJsonObject& clipper, RESULT* (ObjectFactory::*builder)(const QJsonObject&) const)
{
  RESULT* result = (target->factory()->*builder)(clipper);

  result->setCurrentFloor(target->getCurrentFloor());
  return result;
}

StorableObject* LevelEditorController::pasteIn(ObjectGroup* target)
{
  if (clipper["type"].toString() == target->metaObject()->className())
    return ::pasteIn<ObjectGroup>(target, clipper, &ObjectFactory::loadJsonGroup);
  return ::pasteIn<DynamicObject>(target, clipper, &ObjectFactory::loadFromJson);
}
