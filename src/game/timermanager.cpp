#include "timermanager.h"
#include <QJsonObject>

TimeManager::TimeManager(QObject *parent) : QObject(parent)
{
  dateTime = DateTime(2011, 1, 26, 8, 30, 0);
}

void TimeManager::save(QJsonObject& data) const
{
  data["y"] = dateTime.GetYear();
  data["M"] = dateTime.GetMonth();
  data["d"] = dateTime.GetDay();
  data["h"] = dateTime.GetHour();
  data["m"] = dateTime.GetMinute();
  data["s"] = dateTime.GetSecond();
}

void TimeManager::load(const QJsonObject& data)
{
  dateTime = DateTime(
    data["y"].toInt(), data["M"].toInt(), data["d"].toInt(),
    data["h"].toInt(), data["m"].toInt(), data["s"].toInt()
  );
}

void TimeManager::addElapsedMilliseconds(qint64 value)
{
  milliseconds += value;
  if (milliseconds > 1000)
  {
    qint64 seconds = milliseconds / 1000;

    if (seconds > 0)
      addElapsedTime(DateTime::Seconds(static_cast<unsigned int>(seconds)));
    milliseconds %= 1000;
  }
}

void TimeManager::addElapsedTime(DateTime::TimeUnit time)
{
  dateTime = dateTime + time;
  emit dateChanged();
}

void TimeManager::addElapsedMinutes(int minutes)
{
  dateTime = dateTime + DateTime::Minutes(minutes);
  emit dateChanged();
}

long TimeManager::secondsUntilTime(const QVariantMap &timeData) const
{
  DateTime nextTime(
    dateTime.GetYear(), dateTime.GetMonth(), dateTime.GetDay(),
    timeData.contains("hour")   ? timeData["hour"].toUInt()   : dateTime.GetHour(),
    timeData.contains("minute") ? timeData["minute"].toUInt() : 0,
    timeData.contains("second") ? timeData["second"].toUInt() : 0
  );
  std::time_t seconds;

  if (timeData.contains("weeks"))
      nextTime = nextTime + DateTime::Weeks(timeData["weeks"].toUInt());
  if (timeData.contains("days"))
      nextTime = nextTime + DateTime::Days(timeData["days"].toUInt());
  if (nextTime <= dateTime)
    nextTime = nextTime + DateTime::Days(1);
  seconds = (nextTime - dateTime).GetTimestamp();
  return static_cast<long>(seconds);
}

QString TimeManager::toStringAfter(int seconds, const QString& format) const
{
  DateTime date = dateTime + DateTime::Seconds(seconds);
  return QString::fromStdString(date.ToString(format.toStdString()));
}

TimeManager::TimePoint::TimePoint(const QVariantMap& timeData)
{
  hour   = timeData.contains("hour") ? timeData["hour"].toUInt() : 0;
  minute = timeData.contains("minute") ? timeData["minute"].toUInt() : 0;
  second = timeData.contains("second") ? timeData["second"].toUInt() : 0;
}

bool TimeManager::TimePoint::operator<(const TimeManager::TimePoint& b) const
{
  return hour < b.hour || (hour == b.hour && (minute < b.minute || (minute == b.minute && second < b.second)));
}

bool TimeManager::TimePoint::operator>(const TimeManager::TimePoint& b) const
{
  return hour > b.hour || (hour == b.hour && (minute > b.minute || (minute == b.minute && second > b.second)));
}

bool TimeManager::TimePoint::operator==(const TimeManager::TimePoint& b) const
{
  return hour == b.hour && minute == b.minute && second == b.second;
}

bool TimeManager::isWithinRange(const QVariantMap& data) const
{
  if (data.contains("from") && data.contains("to"))
  {
    QVariantMap from = data["from"].toMap();
    QVariantMap to   = data["to"].toMap();

    return isWithinRange(TimePoint(from), TimePoint(to));
  }
  qDebug() << "Invalid timeRange sent to TimeManager::isWithinRange";
  return false;
}

bool TimeManager::isWithinRange(const TimePoint& from, const TimePoint& to) const
{
  TimePoint now(*this);

  if (from > to)
    return now >= from || now < to;
  return now >= from && now < to;
}
