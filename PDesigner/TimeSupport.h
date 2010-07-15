#ifndef TIMESUPPORT_H
#define TIMESUPPORT_H

#include <time.h>
#include <String.h>

#define SECONDS_PER_DAY		   86400
#define SECONDS_PER_WEEK	  604800
#define SECONDS_PER_YEAR	31557600

time_t IncrementDateByDay(const time_t &t);
time_t DecrementDateByDay(const time_t &t);

time_t IncrementDateByMonth(const time_t &t);
time_t DecrementDateByMonth(const time_t &t);
time_t IncrementDateByQuarter(const time_t &t);
time_t IncrementDateByYear(const time_t &t);
time_t DecrementDateByYear(const time_t &t);


time_t GetCurrentDate(void);
time_t GetCurrentMonth(void);
time_t GetCurrentQuarter(void);
time_t GetCurrentYear(void);

time_t GetLastMonth(void);
time_t GetLastQuarter(void);
time_t GetLastYear(void);

BString GetShortMonthName(const uint8 &number);

int GetQuarterForDate(const time_t &t);
int GetQuarterMonthForDate(const time_t &t);

bool IsLeapYear(int year);
int DaysInMonth(int month, int year);
int DayOfWeek(int day, int month, int year);
int DayOfYear(int day, int month, int year);

#endif
