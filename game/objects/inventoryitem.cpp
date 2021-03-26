#include "globals.h"
#include "inventoryitem.h"
#include "../inventoryitemlibrary.h"
#include "game.h"

InventoryItem::InventoryItem(QObject* parent) : DynamicObject(parent), quantity(1)
{
  blocksPath = false;
  connect(this, &InventoryItem::quantityChanged, this, &InventoryItem::weightChanged);
  connect(this, &InventoryItem::quantityChanged, this, &InventoryItem::valueChanged);
  connect(this, &InventoryItem::itemTypeChanged, this, &InventoryItem::updateScript);
  connect(this, &InventoryItem::itemTypeChanged, this, &InventoryItem::updateSprite);
  setSpriteName("items");
}

QString InventoryItem::getCategory() const
{
  auto itemData = InventoryItemLibrary::get()->getObject(itemType);

  if (itemData.isObject())
    return itemData["type"].toString("misc");
  return "weapon";
}

QStringList InventoryItem::getAvailableInteractions()
{
  auto list = DynamicObject::getAvailableInteractions();

  if (list.empty())
    list << "use";
  return list;
}

int InventoryItem::getWeight() const
{
  auto itemData = InventoryItemLibrary::get()->getObject(itemType);

  if (itemData.isObject())
    return itemData["weight"].toInt(1) * getQuantity();
  return getQuantity();
}

int InventoryItem::getValue() const
{
  auto itemData = InventoryItemLibrary::get()->getObject(itemType);

  if (itemData.isObject())
    return itemData["value"].toInt(1);
  return 1;
}

bool InventoryItem::isGroupable(InventoryItem* other)
{
  auto itemData = InventoryItemLibrary::get()->getObject(itemType);
  bool result   = true;

  if (itemData.isObject())
    result = itemData["isGroupable"].toBool(result);
  if (script && script->hasMethod("isGroupable"))
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(other)
         << result;
    result = script->call("isGroupable", args).toBool();
  }
  return result;
}

void InventoryItem::add(int amount)
{
  quantity += amount;
  emit quantityChanged();
}

bool InventoryItem::remove(int amount)
{
  if (quantity > amount)
  {
    quantity -= amount;
    emit quantityChanged();
    return true;
  }
  return false;
}

void InventoryItem::onEquippedBy(Character* user, bool on)
{
  if (script)
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(user) << on;
    script->call("onEquipped", args);
  }
}

bool InventoryItem::canEquipInSlot(const QString& slotType)
{
  if (script && script->hasMethod("canEquipInSlotType"))
  {
    QJSValueList args;

    args << slotType;
    return script->call("canEquipInSlotType", args).toBool();
  }
  return  slotType == "any";
}

int InventoryItem::getActionPointCost()
{
  if (script && script->hasMethod("getActionPointCost"))
    return script->call("getActionPointCost").toInt();
  return 2;
}

bool InventoryItem::isCombatItem()
{
  QJSValue value;

  if (script)
    value = script->property("triggersCombat");
  return value.isBool() ? value.toBool() : false;
}

bool InventoryItem::isInRange(DynamicObject *target)
{
  if (script && script->hasMethod("isInRange"))
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(target);
    return script->call("isInRange", args).toBool();
  }
  return true;
}

bool InventoryItem::isValidTarget(DynamicObject* target)
{
  if (script && script->hasMethod("isValidTarget"))
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(target);
    return script->call("isValidTarget", args).toBool();
  }
  return false;
}

QJSValue InventoryItem::useOn(DynamicObject* target)
{
  if (script && isValidTarget(target))
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(target);
    return script->call("attemptToUseOn", args);
  }
  return false;
}

int InventoryItem::getUseSuccessRate(DynamicObject* target)
{
  if (isValidTarget(target))
  {
    if (script && script->hasMethod("getUseSuccessRate"))
    {
      QJSValueList args;

      args << Game::get()->getScriptEngine().newQObject(target);
      return script->call("getUseSuccessRate", args).toInt();
    }
    else if (isInRange(target))
      return 95;
  }
  return 0;
}

DynamicObject* InventoryItem::getOwner() const
{
  if (parent()->metaObject()->className() == QString("Inventory"))
  {
    Inventory* inventory = reinterpret_cast<Inventory*>(parent());

    return inventory->getUser();
  }
  return nullptr;
}

void InventoryItem::updateScript()
{
  auto itemData = InventoryItemLibrary::get()->getObject(itemType);
  QString scriptName = itemType + ".mjs";

  if (itemData.isObject())
    scriptName = itemData["script"].toString(scriptName);
  setScript(scriptName);
}

void InventoryItem::updateSprite()
{
  QString animationName = "any";
  auto itemData = InventoryItemLibrary::get()->getObject(itemType);

  if (itemData.isObject())
    animationName = itemData["sprite"].toString(animationName);
  setAnimation(animationName);
}

void InventoryItem::save(QJsonObject& data) const
{
  data["itemType"] = itemType;
  data["quantity"] = quantity;
  if (virtualItem)
    data["virtual"] = virtualItem;
  DynamicObject::save(data);
}

void InventoryItem::load(const QJsonObject& data)
{
  itemType = data["itemType"].toString();
  quantity = data["quantity"].toInt(1);
  virtualItem = data["virtual"].toBool();
  DynamicObject::load(data);
  emit quantityChanged();
  emit objectNameChanged();
}
