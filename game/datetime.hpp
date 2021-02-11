#ifndef  DATE_TIME_HPP
# define DATE_TIME_HPP

# include <queue>
# include <functional>
# include <string>
# include <ctime>

template<typename TYPE>
struct RecursiveChecker
{
  typedef std::queue<RecursiveChecker>     Queue;
  typedef std::function<bool (TYPE, TYPE)> CompFunc;
  
  RecursiveChecker(TYPE first, TYPE second, CompFunc comp) : first(first), second(second), comp(comp)
  {
  }
  
  bool Execute(std::queue<RecursiveChecker> checkers)
  {
    if (first == second)
    {
      checkers.pop();
      if (!(checkers.empty()))
        return (checkers.front().Execute(checkers));
      return (true);
    }
    return (comp(first, second));
  }

  TYPE                             first, second;
  std::function<bool (TYPE, TYPE)> comp;
};

class DateTime
{
public:
  struct TimeUnit
  {
    TimeUnit(unsigned int value) : seconds(value) {}
    unsigned int seconds;

    TimeUnit operator+(const TimeUnit& time) const
    {
      TimeUnit retval(time);

      retval.seconds += seconds;
      return (retval);
    }
  };

  struct Seconds : public TimeUnit { Seconds(unsigned int value) : TimeUnit(value)      {} };
  struct Minutes : public Seconds  { Minutes(unsigned int value) : Seconds (value * 60) {} };
  struct Hours   : public Minutes  { Hours(unsigned int value)   : Minutes (value * 60) {} };
  struct Days    : public Hours    { Days(unsigned int value)    : Hours   (value * 24) {} };
  struct Weeks   : public Days     { Weeks(unsigned int value)   : Days    (value * 7)  {} };

  struct DayLength
  {
    DayLength() : length(0), begin(0), end(0) {}

    float length;
    float begin;
    float end;
  };

  DateTime(unsigned int timestamp);
  DateTime(TimeUnit time) : year(0), month(0), day(0), hour(0), minute(0), second(time.seconds) { adjust_values(); }
  DateTime(unsigned int h, unsigned int m, unsigned int s) : year(0), month(0), day(0), hour(h), minute(m), second(s) { adjust_values(); }
  DateTime(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second) : year(year), month(month), day(day), hour(hour), minute(minute), second(second) { adjust_values(); }
  DateTime() : year(0), month(0), day(0), hour(0), minute(0), second(0) {}

  DateTime    operator+(const DateTime& date) const;
  DateTime    operator+(unsigned int seconds) const;
  DateTime    operator+(TimeUnit time_unit) const;
  bool        operator>=(const DateTime& time) const;
  bool        operator<=(const DateTime& time) const;
  bool        operator>(const DateTime& item) const;
  bool        operator<(const DateTime& item) const;
  bool        operator!=(const DateTime& item) const;
  bool        operator==(const DateTime& item) const;

  std::string     ToString(const std::string& format = "%d/%M/%y %h:%m:%s") const;

  DayLength       GetDayLength(void)      const;
  unsigned short  GetYear(void)           const { return (year);   }
  unsigned short  GetMonth(void)          const { return (month);  }
  unsigned short  GetDay(void)            const { return (day);    }
  unsigned short  GetHour(void)           const { return (hour);   }
  unsigned short  GetMinute(void)         const { return (minute); }
  unsigned short  GetSecond(void)         const { return (second); }
  Seconds         GetTimeOfTheDay(void)   const;
  Seconds         GetTimeOfTheWeek(void)  const;
  Seconds         GetTimeOfTheMonth(void) const;
  unsigned short  GetDayOfTheWeek(void)   const;
  unsigned short  GetDayOfTheYear(void)   const;
  time_t          GetTimestamp(void)      const;
  void            SetFromTimestamp(time_t);

  static unsigned short days_per_months(unsigned short month, unsigned short year = 1);

private:
  bool           recursive_compare(RecursiveChecker<int>::CompFunc comp, const DateTime& time) const;
  void           adjust_values(void);

  unsigned short year;
  unsigned int   month, day, hour, minute, second;
};

#endif
