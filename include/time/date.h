/*
 * @version: 0.1 2021-6-1
 * @author: Conzxy
 * Date class
 */

#ifndef _DATE_H
#define _DATE_H

#include <chrono>
#include <cstring>
#include <stdexcept>

namespace zxy{

#define SYS_TIME_POINT\
	std::chrono::system_clock::time_point
#define SYS_CLOCK\
	std::chrono::system_clock

class DateException : std::runtime_error
{
public:
	explicit DateException(char const* msg)
		: std::runtime_error{msg}
	{}

};

class Date
{
	static SYS_TIME_POINT
	makeTimePoint(int year, int mon, int day,
				  int hour, int min, int sec=0)
	{
		struct std::tm t;
		t.tm_year = year - 1900;	//since 1900
		t.tm_mon = mon - 1;	//[0 - 11]
		t.tm_mday = day;	//[1 - 31]
		t.tm_hour = hour;	//[0 - 23]
		t.tm_min = min;
		t.tm_sec = sec;
		t.tm_isdst = -1; //daylight saving time flag

		std::time_t tt = std::mktime(&t);
		if(tt == -1)
			throw DateException{"not a valid system time"};

		return SYS_CLOCK::from_time_t(tt);
	}

	static char const* asString(SYS_TIME_POINT const& tp)
	{
		auto time = SYS_CLOCK::to_time_t(tp);
		auto time_str = std::ctime(&time);
		time_str[std::strlen(time_str)-1] = 0;
		return time_str;
	}

	static char const* now()
	{
		return asString(SYS_CLOCK::now());
	}
};

}//namespace zxy

#endif
