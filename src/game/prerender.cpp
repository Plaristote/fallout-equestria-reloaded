#include <QStandardPaths>
#include <QString>
#include <QDebug>

QString preRenderRoot()
{
  auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/prerender/";
  return path;
}
