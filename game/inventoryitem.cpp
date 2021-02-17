#include "globals.h"
#include "inventoryitem.h"
#include "inventoryitemlibrary.h"
#include "game.h"

InventoryItem::InventoryItem(QObject* parent) : DynamicObject(parent), quantity(1)
{
  connect(this, &InventoryItem::quantityChanged, this, &InventoryItem::weightChanged);
  connect(this, &InventoryItem::quantityChanged, this, &InventoryItem::valueChanged);
  connect(this, &InventoryItem::objectNameChanged, this, &InventoryItem::updateScript);
  connect(this, &InventoryItem::objectNameChanged, this, &InventoryItem::updateSprite);
  setSpriteName("items");
}

QString InventoryItem::getItemType() const
{
  auto itemData = InventoryItemLibrary::get()->getObject(getObjectName());

  if (itemData.isObject())
    return itemData["type"].toString("misc");
  return "misc";
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
  auto itemData = InventoryItemLibrary::get()->getObject(getObjectName());

  if (itemData.isObject())
    return itemData["weight"].toInt(1) * getQuantity();
  return getQuantity();
}

int InventoryItem::getValue() const
{
  auto itemData = InventoryItemLibrary::get()->getObject(getObjectName());

  if (itemData.isObject())
    return itemData["value"].toInt(1);
  return 1;
}

bool InventoryItem::isGroupable(InventoryItem* other)
{
  auto itemData = InventoryItemLibrary::get()->getObject(getObjectName());
  auto callback = script.property("isGroupable");
  bool result   = true;

  if (itemData.isObject())
    result = itemData["isGroupable"].toBool(result);
  if (callback.isCallable())
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(this)
         << Game::get()->getScriptEngine().newQObject(other)
         << result;
    result = callback.call(args).toBool();
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
  QJSValue callback = script.property("onEquipped");

  if (callback.isCallable())
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(user) << on;
    Game::get()->scriptCall(callback, args, "Item::onEquipped");
  }
}

bool InventoryItem::canEquipInSlot(const QString& slotType)
{
  QJSValue callback = script.property("canEquipInSlotType");

  if (callback.isCallable())
  {
    QJSValueList args;

    args << slotType;
    return Game::get()->scriptCall(callback, args, "Item::canEquipInSlotType").toBool();
  }
  return  slotType == "any";
}

void InventoryItem::updateScript()
{
  auto itemData = InventoryItemLibrary::get()->getObject(getObjectName());
  QString scriptName = getObjectName() + ".mjs";

  if (itemData.isObject())
    scriptName = itemData["script"].toString(scriptName);
  setScript(scriptName);
}

void InventoryItem::updateSprite()
{
  QString animationName = "any";
  auto itemData = InventoryItemLibrary::get()->getObject(getObjectName());

  if (itemData.isObject())
    animationName = itemData["sprite"].toString(animationName);
  setAnimation(animationName);
}

void InventoryItem::save(QJsonObject& data) const
{
  data["quantity"] = quantity;
  DynamicObject::save(data);
}

void InventoryItem::load(const QJsonObject& data)
{
  quantity = data["quantity"].toInt(1);
  DynamicObject::load(data);
}
