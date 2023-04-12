#include "javascriptforeach.h"
#include <QDebug>

void forEach(QJSValue array, std::function<void (QJSValue)> callback)
{
  unsigned int length = array.property("length").toUInt();
  QJSValue at = array.property("at");

  for (unsigned int i = 0 ; i < length ; ++i)
    callback(array.property(i));
}
