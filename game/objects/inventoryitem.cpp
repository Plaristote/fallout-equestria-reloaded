#include "globals.h"
#include "inventoryitem.h"
#include "../inventoryitemlibrary.h"
#include "game.h"
#include "i18n.h"
#include <QDebug>

InventoryItem::InventoryItem(QObject* parent) : DynamicObject(parent), quantity(1)
{
  blocksPath = false;
  connect(this, &InventoryItem::quantityChanged, this, &InventoryItem::weightChanged);
  connect(this, &InventoryItem::quantityChanged, this, &InventoryItem::valueChanged);
  connect(this, &InventoryItem::itemTypeChanged, this, &InventoryItem::updateScript);
  connect(this, &InventoryItem::itemTypeChanged, this, &InventoryItem::updateSprite);
  setSpriteName("items");
}

QString InventoryItem::getIcon() const
{
  auto itemData = InventoryItemLibrary::get()->getObject(itemType);

  if (itemData.isObject())
    return itemData["icon"].toString("any.png");
  return "any.png";
}

QString InventoryItem::getCategory() const
{
  auto itemData = InventoryItemLibrary::get()->getObject(itemType);

  if (itemData.isObject())
    return itemData["type"].toString("misc");
  return "weapon";
}

QString InventoryItem::getDescription() const
{
  if (script && script->hasMethod("getDescription"))
    return script->call("getDescription").toString();
  return I18n::get()->t("item-descriptions." + itemType);
}

QString InventoryItem::getDefaultMode() const
{
  QJSValue defaultMode = script ? script->property("defaultUseMode") : QJSValue();

  if (defaultMode.isString())
    return defaultMode.toString();
  return useMode;
}

bool InventoryItem::defaultLookInteraction()
{
  const I18n* i18n = I18n::get();

  Game::get()->appendToConsole(i18n->t("inspection.item", {
    {"item", i18n->t("items." + itemType)}
  }));
  return true;
}

QStringList InventoryItem::getAvailableInteractions()
{
  auto list = DynamicObject::getAvailableInteractions();

  if (list.empty())
    list << "use" << "look" << "use-skill";
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

int InventoryItem::evaluateValue(Character* buyer, Character* seller)
{
  if (script && script->hasMethod("evaluateValue"))
  {
    auto result = script->call("evaluateValue", QJSValueList() << buyer->asJSValue() << seller->asJSValue());

    if (result.isNumber())
      return result.toInt();
  }
  return getValue();
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

    args << (other ? other->asJSValue() : QJSValue())
         << result;
    result = script->call("isGroupable", args).toBool();
  }
  return result;
}

QStringList InventoryItem::getUseModes() const
{
  if (script)
  {
    QJSValue useModes = script->property("useModes");

    if (useModes.isArray())
      return script->property("useModes").toVariant().toStringList();
  }
  return QStringList() << "use";
}

bool InventoryItem::requiresTarget() const
{
  if (script)
    return script->property("requiresTarget").toBool();
  return true;
}

bool InventoryItem::usesZoneTarget() const
{
  if (script)
    return script->property("zoneTarget").toBool();
  return false;
}

int InventoryItem::getZoneTargetSize() const
{
  if (script)
    return script->property("zoneSize").toInt();
  return 0;
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
  resetUseMode();
  if (script && user)
  {
    QJSValueList args;

    args << user->asJSValue() << on;
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
  return slotType == "any";
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

static int maxDistance(QPoint a, QPoint b)
{
  return std::max(std::abs(a.x() - b.x()), std::abs(a.y() - b.y()));
}

bool InventoryItem::isInRange(DynamicObject *target)
{
  if (target)
  {
    if (script && script->hasMethod("isInRange"))
      return script->call("isInRange", QJSValueList() << target->asJSValue()).toBool();
    else
    {
      float range = getRange();

      if (range == 1.f)
        return maxDistance(getOwner()->getPosition(), target->getPosition()) <= 1;
      return reinterpret_cast<Character*>(getOwner())->getDistance(target) <= range;
    }
  }
  return true;
}

float InventoryItem::getRange() const
{
  if (script && script->hasMethod("getRange"))
    return static_cast<float>(script->call("getRange").toNumber());
  return 1.f;
}

bool InventoryItem::isValidTarget(DynamicObject* target)
{
  if (target && script && script->hasMethod("isValidTarget"))
    return script->call("isValidTarget", QJSValueList() << target->asJSValue()).toBool();
  return false;
}

QJSValue InventoryItem::useOn(DynamicObject* target)
{
  if (script)
  {
    if (this->requiresTarget() || target)
    {
      if (target && isValidTarget(target))
        return script->call("attemptToUseOn", QJSValueList() << target->asJSValue());
      else
      {
        const DynamicObject* owner = getOwner();
        Game* game = Game::get();

        if (owner && owner == game->getPlayer())
          game->appendToConsole(I18n::get()->t("messages.invalid-target"));
      }
    }
    else
      return script->call("attemptToUseOn");
  }
  return false;
}

void InventoryItem::useReload(bool refill)
{
  QStringList modes = getUseModes();

  if (modes.indexOf("reload") >= 0)
  {
    const auto backup = useMode;

    useFromInventory(refill ? "reload" : "unload");
  }
}

QJSValue InventoryItem::useAt(int x, int y)
{
  if (script)
  {
    if (this->usesZoneTarget())
      return script->call("attemptToUseAt", QJSValueList() << x << y);
  }
  return false;
}

void InventoryItem::useFromInventory(QString mode)
{
  const auto backup = useMode;
  QJSValue   result;

  if (mode.isEmpty())
    mode = getUseModes().first();
  useMode = mode;
  result = useOn(nullptr);
  if (result.isObject())
  {
    QJSValue callback = result.property("callback");

    if (callback.isCallable())
      callback.call();
  }
  useMode = backup;
}

void InventoryItem::setCountdown(int value)
{
  if (script && script->hasMethod("onCountdownReceived"))
    script->call("onCountdownReceived", QJSValueList() << value);
}

void InventoryItem::swapUseMode()
{
  QStringList useModes = getUseModes();
  int currentIndex = useModes.indexOf(useMode);

  switch (useModes.length())
  {
  case 0:
    useMode = "use";
    break ;
  case 1:
    useMode = useModes.first();
    break;
  default:
    if (currentIndex + 1 >= useModes.length() || currentIndex == -1)
      useMode = useModes.first();
    else
      useMode = useModes[currentIndex + 1];
    break ;
  }
  emit useModeChanged();
}

void InventoryItem::resetUseMode()
{
  QStringList useModes = getUseModes();

  useMode = useModes.length() > 0 ? useModes.first() : "use";
  emit useModeChanged();
}

void InventoryItem::setUseMode(const QString& mode)
{
  useMode = mode;
  emit useModeChanged();
}

int InventoryItem::getUseSuccessRate(DynamicObject* target)
{
  if (isValidTarget(target))
  {
    if (script && script->hasMethod("getUseSuccessRate"))
      return script->call("getUseSuccessRate", QJSValueList() << target->asJSValue()).toInt();
    else if (isInRange(target))
      return 95;
  }
  return 0;
}

int InventoryItem::getUseAtSuccessRate(int x, int y)
{
  if (usesZoneTarget())
  {
    DynamicObject* owner = getOwner();

    if (script && script->hasMethod("getUseAtSuccessRate"))
      return script->call("getUseAtSuccessRate", QJSValueList() << x << y).toInt();
    else if (owner)
    {
      QPoint position = owner->getPosition();
      QPoint targetPosition(x, y);

      if (getRange() >= Game::get()->getLevel()->getDistance(position, targetPosition))
        return 95;
    }
  }
  return 0;
}

DynamicObject* InventoryItem::getOwner() const
{
  const char* parentType = parent()->metaObject()->className();

  if (parentType == QString("Inventory"))
  {
    Inventory* inventory = reinterpret_cast<Inventory*>(parent());

    return inventory->getUser();
  }
  else if (parentType == QString("Character"))
    return reinterpret_cast<DynamicObject*>(parent());
  else
    qDebug() << "WARNING InventoryItem has no owner" << parent()->metaObject()->className();
  return nullptr;
}

void InventoryItem::updateScript()
{
  auto itemData = InventoryItemLibrary::get()->getObject(itemType);
  QString scriptName = itemType + ".mjs";

  if (itemData.isObject())
    scriptName = itemData["scriptName"].toString(scriptName);
  setScript(scriptName);
  emit useModesChanged();
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
  data["useMode"] = useMode;
  if (ammo > 0)
    data["ammo"] = ammo;
  if (maxAmmo > 0)
    data["maxAmmo"] = maxAmmo;
  DynamicObject::save(data);
}

void InventoryItem::load(const QJsonObject& data)
{
  itemType = data["itemType"].toString();
  quantity = data["quantity"].toInt(1);
  virtualItem = data["virtual"].toBool();
  useMode = data["useMode"].toString();
  ammo = data["ammo"].toInt(0);
  maxAmmo = data["maxAmmo"].toInt(0);
  DynamicObject::load(data);
  emit quantityChanged();
  emit objectNameChanged();
  emit useModesChanged();
  emit useModeChanged();
}
