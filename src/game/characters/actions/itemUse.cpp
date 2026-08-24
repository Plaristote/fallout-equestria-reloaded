#include "itemUse.h"
#include "game/animationSequence/objectanimationpart.h"
#include "game.h"
#include "i18n.h"
#include <QDebug>

ItemAction::~ItemAction()
{
  if (targetWatcher);
    QObject::disconnect(targetWatcher);
  if (itemWatcher);
    QObject::disconnect(itemWatcher);
}

void ItemAction::setTarget(DynamicObject* newTarget)
{
  target = newTarget;
  if (targetWatcher) [[unlikely]]
    QObject::disconnect(targetWatcher);
  if (target) [[likely]]
    targetWatcher = QObject::connect(target, &QObject::destroyed, [this]() { target = nullptr; });
}

void ItemAction::setItem(InventoryItem* newItem)
{
  item = newItem;
  if (itemWatcher) [[unlikely]]
    QObject::disconnect(itemWatcher);
  if (item) [[likely]]
    itemWatcher = QObject::connect(item, &QObject::destroyed, [this]() { item = nullptr; });
}

QJSValue ItemAction::callItemUseOn()
{
  return item->useOn(target);
}

QJSValue ItemZoneAction::callItemUseOn()
{
  return item->useAt(target.x(), target.y());
}

void ItemZoneAction::lookAtTarget()
{
  character->lookTo(getTargetPosition());
}

void ItemAction::lookAtTarget()
{
  if (target)
    character->lookTo(getTargetPosition());
}

QJSValue ItemAction::getDefaultCallback()
{
  return item->getScriptObject().property("useOn");
}

bool ItemAction::trigger()
{
  if (item)
  {
    QString backupMode;
    QJSValue result;

    if (useMode != item->getUseMode() && !useMode.isEmpty())
    {
      backupMode = item->getUseMode();
      item->setUseMode(useMode);
    }
    result = callItemUseOn();
    triggerAnimation(result);
    if (!backupMode.isEmpty())
      item->setUseMode(backupMode);
  }
  else
  {
    qDebug() << "no item to use";
    state = Interrupted;
  }
  return state != Interrupted;
}

void ItemAction::performAction()
{
  bool success;

  if (callback.isCallable())
    success = Game::get()->scriptCall(callback, QJSValueList(), "ItemAction::performAction").toBool();
  else
  {
    QJSValueList args;

    if (target)
      args << target->asJSValue();
    callback = getDefaultCallback();
    success = Game::get()->scriptCall(callback, args, "ItemAction::performAction").toBool();
  }
  state = success ? Done : Interrupted;
}

int ItemAction::getApCost() const
{
  if (item)
    return item->getActionPointCost();
  return 2;
}
