#include "globals.h"
#include "inventoryitem.h"
#include "../inventoryitemlibrary.h"
#include "game.h"
#include "i18n.h"
#include <cmath>
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
  QJSValue retval = scriptCall("getDescription");

  if (retval.isString())
    return retval.toString();
  return I18n::get()->t("item-descriptions." + itemType);
}

QString InventoryItem::getDefaultMode() const
{
  QJSValue defaultMode = script ? script->property("defaultUseMode") : QJSValue();

  if (defaultMode.isString())
    return defaultMode.toString();
  return useMode;
}

QString InventoryItem::getDisplayName() const
{
  const I18n* i18n = I18n::get();

  return i18n->t("items." + itemType);
}

QStringList InventoryItem::getAvailableInteractions()
{
  if (script && script->hasMethod("getAvailableInteractions"))
    return DynamicObject::getAvailableInteractions();
  return QStringList() << "use" << "look" << "use-skill";
}

bool InventoryItem::triggerInteraction(Character *character, const QString &interactionType)
{
  bool result = DynamicObject::triggerInteraction(character, interactionType);

  if (!result && interactionType == "use")
  {
    Game::get()->getLevel()->pickUpItem(character, this);
    return true;
  }
  return result;
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

bool InventoryItem::isDestructible() const
{
  QJSValue destructible = script ? script->property("destructible") : QJSValue();

  return destructible.isBool() ? destructible.toBool() : true;
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
  if (user)
    scriptCall("onEquipped", QJSValueList() << user->asJSValue() << on);
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
    QJSValue result = scriptCall("isInRange", QJSValueList() << target->asJSValue());

    if (result.isNull() || result.isUndefined())
    {
      float range = getRange();
      CharacterSight* owner = reinterpret_cast<CharacterSight*>(getOwner());

      if (range == 1.f)
        return maxDistance(owner->getPosition(), target->getPosition()) <= 1;
      return std::floor(owner->getDistance(target)) <= range;
    }
    return result.toBool();
  }
  return true;
}

float InventoryItem::getRange() const
{
  QJSValue result = scriptCall("getRange");

  if (result.isNumber())
    return static_cast<float>(result.toNumber());
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
  if (script && script->hasMethod("attemptToUseOn"))
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
        return false;
      }
    }
    else
      return script->call("attemptToUseOn");
  }
  return script && script->hasMethod("useOn");
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
  else if (result.toBool())
    script->call("useOn");
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
  if (!QFileInfo(getScriptPath() + '/' + scriptName).isFile())
    scriptName = QString();
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
  data.remove("script");
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
  emit itemTypeChanged();
  emit quantityChanged();
  emit objectNameChanged();
  emit useModeChanged();
}
