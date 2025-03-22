#include "promptdialog.h"
#include <QJSValueIterator>
#include <QDebug>

PromptDialog::PromptDialog(const QString& title, QJSValue object, QObject *parent)
  : QObject{parent}, title(title)
{
  QJSValueIterator it(object);

  while (it.hasNext())
  {
    QJSValue value = it.value();
    QJSValue label = value.property("label");
    QJSValue callback = value.property("callback");

    if (label.isString())
    {
      labels << value.property("label").toString();
      callbacks << value.property("callback");
    }
    it.next();
  }
}

void PromptDialog::pick(int index)
{
  QJSValue callback = callbacks.size() > index ? callbacks.at(index) : QJSValue();

  if (callback.isCallable())
    callback.call();
}
