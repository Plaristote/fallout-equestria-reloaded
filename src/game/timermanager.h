#ifndef  TIMEMANAGER_H
# define TIMEMANAGER_H

# include <QObject>
# include "utils/datetime.hpp"

class QJsonObject;

class TimeManager : public QObject
{
  Q_OBJECT

  Q_PROPERTY(int year    READ getYear    NOTIFY dateChanged)
  Q_PROPERTY(int month   READ getMonth   NOTIFY dateChanged)
  Q_PROPERTY(int day     READ getDay     NOTIFY dateChanged)
  Q_PROPERTY(int hour    READ getHour    NOTIFY dateChanged)
  Q_PROPERTY(int minute  READ getMinute  NOTIFY dateChanged)
  Q_PROPERTY(int second  READ getSecond  NOTIFY dateChanged)
  Q_PROPERTY(int weekDay READ getWeekDay NOTIFY dateChanged)
public:
  struct TimePoint
  {
    TimePoint(const TimeManager& tm) : hour(tm.getHour()), minute(tm.getMinute()), second(tm.getSecond()) {}
    TimePoint(const QVariantMap& tm);
    bool operator==(const TimePoint&) const;
    bool operator>(const TimePoint&) const;
    bool operator<(const TimePoint&) const;
    bool operator>=(const TimePoint& b) const { return operator==(b) || operator>(b); }
    bool operator<=(const TimePoint& b) const { return operator==(b) || operator<(b); }
    unsigned int hour = 0, minute = 0, second = 0;
  };

  explicit TimeManager(QObject *parent = nullptr);

  void save(QJsonObject&) const;
  void load(const QJsonObject&);

  DateTime getDateTime() const { return dateTime; }
  void setDateTime(const DateTime& value) { dateTime = value; emit dateChanged(); }

  Q_INVOKABLE void addElapsedMilliseconds(qint64);
  Q_INVOKABLE void addElapsedTime(DateTime::TimeUnit);
  Q_INVOKABLE void addElapsedMinutes(int);
  Q_INVOKABLE long secondsUntilTime(const QVariantMap& timeData) const;
  // Return timestamp in seconds
  Q_INVOKABLE long getTimestamp() const { return getDateTime().GetTimestamp(); }

  Q_INVOKABLE bool isWithinRange(const QVariantMap&) const;
  bool isWithinRange(const TimePoint& from, const TimePoint& to) const;

  int getYear()   const { return dateTime.GetYear(); }
  int getMonth()  const { return dateTime.GetMonth(); }
  int getDay()    const { return dateTime.GetDay(); }
  int getHour()   const { return dateTime.GetHour(); }
  int getMinute() const { return dateTime.GetMinute(); }
  int getSecond() const { return dateTime.GetSecond(); }
  int getWeekDay() const { return dateTime.GetDayOfTheWeek(); }

signals:
  void dateChanged();

private:
  qint64 milliseconds = 0;
  DateTime dateTime;
};

#endif // TIMERMANAGER_H
