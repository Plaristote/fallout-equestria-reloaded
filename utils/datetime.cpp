#include "datetime.hpp"
#include <sstream>

using namespace std;

DateTime DateTime::operator-(const DateTime& date) const
{
  return operator-(static_cast<unsigned int>(date.GetTimestamp()));
}

DateTime DateTime::operator-(unsigned int seconds) const
{
  return DateTime(GetTimestamp() - seconds);
}

DateTime DateTime::operator-(TimeUnit time_unit) const
{
  return operator-(time_unit.seconds);
}

DateTime DateTime::operator+(const DateTime& date) const
{
  DateTime result(*this);

  result.year   += date.year;
  result.month  += date.month;
  result.day    += date.day;
  result.hour   += date.hour;
  result.minute += date.minute;
  result.second += date.second;
  result.adjust_values();
  return (result);
}

DateTime DateTime::operator+(unsigned int seconds) const
{
  DateTime result(*this);
  
  result.second += seconds;
  result.adjust_values();
  return (result);
}

DateTime DateTime::operator+(TimeUnit time_unit) const
{
  DateTime result(*this);

  result.second += time_unit.seconds;
  result.adjust_values();
  return (result);
}

bool DateTime::operator>=(const DateTime& time) const
{
  return *this == time || *this > time;
}

bool DateTime::operator<=(const DateTime& time) const
{
  return *this == time || *this < time;
}

bool DateTime::operator>(const DateTime& time) const
{
  RecursiveChecker<int>::CompFunc comp([](int a, int b) { return (a > b); });

  return (recursive_compare(comp, time));
}

bool DateTime::operator<(const DateTime& time) const
{
  RecursiveChecker<int>::CompFunc comp([](int a, int b) { return (a < b); });

  return (recursive_compare(comp, time));
}

bool DateTime::operator!=(const DateTime& item) const
{
  return (second != item.second || minute != item.minute || hour != item.hour || day != item.day || month != item.month || year != item.year);
}

bool DateTime::operator==(const DateTime& item) const
{
  return (second == item.second && minute == item.minute && hour == item.hour && day == item.day && month == item.month && year == item.year);
}

time_t DateTime::GetTimestamp(void) const
{
  if (year != 0)
  {
    struct tm timeinfo;

#ifdef	__USE_MISC
    timeinfo.tm_gmtoff = 0;
    timeinfo.tm_zone = nullptr;
#endif
    timeinfo.tm_isdst = 0;
    timeinfo.tm_wday = 0;
    timeinfo.tm_yday = 0;
    timeinfo.tm_year = (year < 1900 ? 0 : year - 1900);
    timeinfo.tm_mon  = static_cast<int>(month - 1);
    timeinfo.tm_mday = static_cast<int>(day);
    timeinfo.tm_hour = static_cast<int>(hour);
    timeinfo.tm_min  = static_cast<int>(minute);
    timeinfo.tm_sec  = static_cast<int>(second);
    return (mktime(&timeinfo));
  }
  return (GetTimeOfTheMonth().seconds);
}

DateTime::Seconds DateTime::GetTimeOfTheDay() const
{
  return (Hours(hour).seconds + Minutes(minute).seconds + second);
}

DateTime::Seconds DateTime::GetTimeOfTheWeek() const
{
  return (GetTimeOfTheDay().seconds + Days(GetDayOfTheWeek() - 1).seconds);
}

DateTime::Seconds DateTime::GetTimeOfTheMonth() const
{
  return (GetTimeOfTheDay().seconds + Days(day).seconds);
}

unsigned short DateTime::GetDayOfTheWeek() const
{
  time_t         timestamp = GetTimestamp();
  struct tm*     time      = gmtime(&timestamp);
  unsigned short wday      = time->tm_wday + 1;

  return (wday);
}

std::string DateTime::ToString(const std::string& format) const
{
  std::stringstream result;

  for (unsigned int i = 0 ; i < format.size() ; ++i)
  {
    if (format[i] == '%')
    {
      short number;
      
      switch (format[i + 1])
      {
        case 'd':
          number  = static_cast<short>(day);
          break ;
        case 'M':
          number  = static_cast<short>(month);
          break ;
        case 'y':
          number  = static_cast<short>(year);
          break ;
        case 'h':
          number  = static_cast<short>(hour);
          break ;
        case 'm':
          number  = static_cast<short>(minute);
          break ;
        case 's':
          number  = static_cast<short>(second);
          break ;
        default:
          number  = -1;
          result << format[i] << format[i + 1];
          break ;
      }
      if (number > -1)
      {
        if (number < 10)
          result << '0';
        result << number;
      }
      ++i;
    }
    else
      result << format[i];
  }
  return (result.str());
}

unsigned short DateTime::days_per_months(unsigned short month, unsigned short year)
{
  if (month == 2)
    return (year % 4 == 0 ? 29 : 28);
  return ((month <= 7 ? month % 2 == 0 : month % 2 != 0) ? 30 : 31); 
}

bool DateTime::recursive_compare(RecursiveChecker<int>::CompFunc comp, const DateTime& time) const
{
  RecursiveChecker<int>::Queue    checkers;

  checkers.push(RecursiveChecker<int>(year,   time.year,   comp));
  checkers.push(RecursiveChecker<int>(month,  time.month,  comp));
  checkers.push(RecursiveChecker<int>(day,    time.day,    comp));
  checkers.push(RecursiveChecker<int>(hour,   time.hour,   comp));
  checkers.push(RecursiveChecker<int>(minute, time.minute, comp));
  checkers.push(RecursiveChecker<int>(second, time.second, comp));    
  return (checkers.front().Execute(checkers));
}

void DateTime::adjust_values(void)
{
  if (second > 59)
  {
    minute += (second / 60);
    second  = second % 60;
  }
  if (minute > 59)
  {
    hour += (minute / 60);
    minute= minute % 60;
  }
  if (hour > 23)
  {
    day += (hour / 24);
    hour = hour % 24;
  }
  if (month > 12)
  {
    year += (month / 12);
    month = month % 12;
    if (month == 0)
    { month = 1; }
  }
  while (day > days_per_months(month, year))
  {
    day -= days_per_months(month, year);
    month += 1;
    if (month > 12)
    {
      month = 1;
      year += 1;
    }
  }
}

unsigned short DateTime::GetDayOfTheYear() const
{
  unsigned short result = 0;
  unsigned short month_it;

  for (month_it = 1 ; month_it < month ; month_it++)
    result += days_per_months(month_it, year);
  return (result + day);
}

static float ComputeLengthOfDay(unsigned short day)
{
  float         length_of_day;

  // 83 (débuts des longs jours)
  // 261 (fin des longs jours)
  // longs jours: 172
  // courts jourts: 193

  if (day > 83 && day < 261)
  {
    static unsigned char summer_solstice = 172;

    day -= 83;
    if (day > summer_solstice)
      length_of_day = (18 - ((day - summer_solstice) / 86.f) * 3);
    else
      length_of_day = (18 - ((summer_solstice - day) / 86.f) * 3);
  }
  else
  {
    static unsigned char winter_solstice = 359 - 261;

    if (day > 0 && day < 83)
      day += 365;
    day -= 261;
    if (day > winter_solstice)
      length_of_day = (9 + ((day - winter_solstice) / 96.5f) * 3);
    else
      length_of_day = (9 + ((winter_solstice - day) / 96.5f) * 3);
  }
  return (length_of_day);
}

DateTime::DayLength DateTime::GetDayLength() const
{
  static DayLength day_lengths[357];
  unsigned short   day = GetDayOfTheYear();

  if (day <= 356)
  {
    if (day_lengths[day].length == 0)
    {
      day_lengths[day].length = ComputeLengthOfDay(day);
      day_lengths[day].begin  = 14 - day_lengths[day].length / 2;
      day_lengths[day].end    = 14 + day_lengths[day].length / 2;
    }
    return (day_lengths[day]);
  }
  return (day_lengths[0]);
}
