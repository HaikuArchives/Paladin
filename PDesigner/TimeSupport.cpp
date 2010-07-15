#include "TimeSupport.h"
#include <OS.h>

time_t IncrementDateByDay(const time_t &t)
{
	struct tm timestruct = *localtime(&t);
	timestruct.tm_mday++;
	return mktime(&timestruct);
}

time_t DecrementDateByDay(const time_t &t)
{
	struct tm timestruct = *localtime(&t);
	timestruct.tm_mday--;
	return mktime(&timestruct);
}

time_t IncrementDateByMonth(const time_t &t)
{
	struct tm timestruct = *localtime(&t);
	timestruct.tm_mon++;
	return mktime(&timestruct);
}

time_t DecrementDateByMonth(const time_t &t)
{
	struct tm timestruct = *localtime(&t);
	timestruct.tm_mon--;
	return mktime(&timestruct);
}

time_t IncrementDateByQuarter(const time_t &t)
{
	struct tm timestruct = *localtime(&t);
	int quarter = (timestruct.tm_mon + 1) / 3;
	if( (timestruct.tm_mon + 1) % 3 > 0)
		quarter++;
	
	if(quarter == 4)
	{
		quarter = 1;
		timestruct.tm_year++;
	}
	else
		quarter++;
	
	// We subtract 3 becuase the actual month is (quarter * 3) - 2, but we
	// also have to factor in that this number is indexed from 0, not 1.
	timestruct.tm_mon = (quarter * 3) - 3;
	return mktime(&timestruct);
}

time_t IncrementDateByYear(const time_t &t)
{
	struct tm timestruct = *localtime(&t);
	timestruct.tm_year++;
	return mktime(&timestruct);
}

time_t DecrementDateByYear(const time_t &t)
{
	struct tm timestruct = *localtime(&t);
	timestruct.tm_year--;
	return mktime(&timestruct);
}

time_t GetCurrentDate(void)
{
	time_t rawtime;
	time(&rawtime);
	struct tm timestruct = *localtime(&rawtime);
	timestruct.tm_sec = 0;
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	return mktime(&timestruct);
}

// Returns the value for the first day of the month.
time_t GetCurrentMonth(void)
{
	time_t rawtime;
	time(&rawtime);
	struct tm timestruct = *localtime(&rawtime);
	timestruct.tm_sec = 0;
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_mday = 1;
	return mktime(&timestruct);
}

// Returns the value for the first day of the quarter
time_t GetCurrentQuarter(void)
{
	time_t rawtime;
	time(&rawtime);
	struct tm timestruct = *localtime(&rawtime);
	timestruct.tm_sec = 0;
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_mday = 1;
	
	// Calculate the current quarter
	int quarter = (timestruct.tm_mon + 1) / 3;
	if( (timestruct.tm_mon + 1) % 3 > 0)
		quarter++;
	
	// We subtract 3 becuase the actual month is (quarter * 3) - 2, but we
	// also have to factor in that this number is indexed from 0, not 1.
	timestruct.tm_mon = (quarter * 3) - 3;
	return mktime(&timestruct);
}

// Returns the value for the first day of the year
time_t GetCurrentYear(void)
{
	time_t rawtime;
	time(&rawtime);
	struct tm timestruct = *localtime(&rawtime);
	timestruct.tm_sec = 0;
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_mday = 1;
	timestruct.tm_mon = 0;
	timestruct.tm_yday = 0;
	return mktime(&timestruct);
}

time_t GetLastMonth(void)
{
	time_t rawtime;
	time(&rawtime);
	struct tm timestruct = *localtime(&rawtime);
	timestruct.tm_sec = 0;
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_mday = 1;
	timestruct.tm_mon--;
	
	return mktime(&timestruct);
}

time_t GetLastQuarter(void)
{
	time_t rawtime;
	time(&rawtime);
	struct tm timestruct = *localtime(&rawtime);
	timestruct.tm_sec = 0;
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_mday = 1;
	
	// Calculate the current quarter
	int quarter = (timestruct.tm_mon + 1) / 3;
	if( (timestruct.tm_mon + 1) % 3 > 0)
		quarter++;
	
	if(quarter == 1)
	{
		quarter = 4;
		timestruct.tm_year--;
	}
	else
		quarter--;
	
	// We subtract 3 becuase the actual month is (quarter * 3) - 2, but we
	// also have to factor in that this number is indexed from 0, not 1.
	timestruct.tm_mon = (quarter * 3) - 3;
	return mktime(&timestruct);
}

time_t GetLastYear(void)
{
	time_t rawtime;
	time(&rawtime);
	struct tm timestruct = *localtime(&rawtime);
	timestruct.tm_sec = 0;
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_mday = 1;
	timestruct.tm_mon = 0;
	timestruct.tm_yday = 0;
	timestruct.tm_year--;
	return mktime(&timestruct);
}

BString GetShortMonthName(const uint8 &number)
{
	time_t rawtime;
	time(&rawtime);
	struct tm timestruct = *localtime(&rawtime);
	timestruct.tm_mon = number;
	
	char string[32];
	strftime(string,32,"%b",&timestruct);
	return BString(string);
}

int DayOfWeek(int day, int month, int year)
{
	// adjust months so February is the last one
	month -= 2;
	if(month < 1)
	{
		month += 12;
		--year;
	}
	
	// split by century
	int cent = year / 100;
	year %= 100;
	return (((26 * month - 2) / 10 + day + year + year / 4 + cent / 4 - 2 * cent) % 7);
} 

int DaysInMonth(int month, int year)
{
	if(month > 11 || month < 0 || year < 0)
	{
		debugger("Bad value in DaysInMonth");
		return 0;
	}
	
	if(year < 200)
		year += 1900;
	
	switch(month)
	{
		case 0:	// January
		case 2:	// March
		case 4: // May
		case 6: // July
		case 7: // August
		case 9: // October
		case 11: // December
		{
			return 31;
		}
		case 1:
		{
			return (IsLeapYear(year) ? 29 : 28);
		}
		default:
			break;
	}
	
	return 30;
}

bool IsLeapYear(int year)
{
	if(year < 0)
	{
		debugger("Bad value in IsLeapYear");
		return false;
	}
	
	if(year < 200)
		year += 1900;
	
	return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) == 1;
}

int DayOfYear(int day, int month, int year)
{
	if(month > 11 || month < 0 || year < 0 || day < 0 || day > 31)
	{
		debugger("Bad value in DayOfYear");
		return 0;
	}
	
	if(year < 200)
		year += 1900;
	
	int daysinmonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	
	int days = 0;
	for(int i=0; i<month; i++)
		days += daysinmonth[i];
	
	if(IsLeapYear(year))
		days++;
	
	days += day - 1;
	
	return days;
}

int GetQuarterForDate(const time_t &t)
{
	struct tm timestruct = *localtime(&t);
	timestruct.tm_sec = 0;
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_mday = 1;
	
	// Calculate the current quarter
	int quarter = (timestruct.tm_mon + 1) / 3;
	if( (timestruct.tm_mon + 1) % 3 > 0)
		quarter++;
	return quarter;
}

int GetQuarterMonthForDate(const time_t &t)
{
	struct tm timestruct = *localtime(&t);
	timestruct.tm_sec = 0;
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_mday = 1;
	
	// Calculate the current quarter
	int quarter = (timestruct.tm_mon + 1) / 3;
	if( (timestruct.tm_mon + 1) % 3 > 0)
		quarter++;
	
	// We subtract 3 becuase the actual month is (quarter * 3) - 2, but we
	// also have to factor in that this number is indexed from 0, not 1.
	return (quarter * 3) - 2;
}
