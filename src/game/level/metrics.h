#ifndef  LEVEL_METRICS_H
# define LEVEL_METRICS_H

# include <QVector>
# include <QElapsedTimer>

class DynamicObject;

struct ObjectPerformanceReport
{
  ObjectPerformanceReport(const DynamicObject* object) : object(object) {}
  bool operator==(const DynamicObject* other) const { return object == other; }
  bool operator>(const ObjectPerformanceReport& other) const { return time < other.time; }
  bool operator<(const ObjectPerformanceReport& other) const { return time > other.time; }

  const DynamicObject* object;
  qint64               time = 0;
};

struct ObjectPerformanceClock
{
  ObjectPerformanceClock(ObjectPerformanceReport& report) : report(report)
  {
    timer.start();
  }

  ~ObjectPerformanceClock()
  {
    report.time += timer.elapsed();
  }

  QElapsedTimer timer;
  ObjectPerformanceReport& report;
};

struct PerformanceReport
{
  QString                  html();
  ObjectPerformanceReport& object(const DynamicObject* object);
  void                     reset();
  void                     removeObject(const DynamicObject* object);

  QVector<ObjectPerformanceReport> objects;
};

#endif
