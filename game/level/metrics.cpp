#include "metrics.h"
#include "../dynamicobject.h"

QString PerformanceReport::html()
{
  QString out("<table>");

  std::sort(objects.begin(), objects.end());
  for (auto it = objects.begin() ; it != objects.end() ; ++it)
  {
    double v = static_cast<double>(it->time) / 1000;
    out += "<tr>";
    out += "<th style=\"text-align:left\">" + it->object->getPath() + "</th>";
    out += "<td style=\"text-align:right\">" + QString::number(v, 'f', 3) + "s</td></tr>";
  }
  return out + "</table>";
}
	
ObjectPerformanceReport& PerformanceReport::object(const DynamicObject* object)
{
  auto it = std::find(objects.begin(), objects.end(), object);

  if (it == objects.end())
  {
    objects.push_back(object);
    return *objects.rbegin();
  }
  return *it;
}

void PerformanceReport::reset()
{
  for (auto it = objects.begin() ; it != objects.end() ; ++it)
    it->time = 0;
}

