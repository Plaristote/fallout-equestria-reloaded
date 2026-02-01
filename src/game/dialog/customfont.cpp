#include "customfont.h"
#include "../../globals.h"

CustomFont::CustomFont(QObject* parent) : QObject(parent)
{
}

QString CustomFont::getUri() const
{
  return ASSETS_PATH + "/fonts/" + source;
}
