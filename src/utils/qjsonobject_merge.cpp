#include <QJsonObject>

void merge(QJsonObject& target, const QJsonObject& source)
{
  for (auto it = source.constBegin() ; it != source.constEnd() ; ++it)
  {
    QJsonObject::iterator target_iterator = target.find(it.key());

    if (target_iterator != target.end())
    {
      if (target_iterator->isObject() && it->isObject())
      {
        QJsonObject newObject = target_iterator->toObject();

        merge(newObject, it->toObject());
        target.insert(it.key(), newObject);
      }
      else
        qDebug() << "merge(QJsonObject,QJsonObject): duplicate key cannot be merged:" << it.key();
    }
    else
      target.insert(it.key(), it.value());
  }
}
