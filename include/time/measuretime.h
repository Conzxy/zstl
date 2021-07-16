/*
 * @version: 0.1 2021-6-1
 * @author: Conzxy
 * use RAII technique to measure time
 */
#ifndef _MEASURE_TIME_H
#define _MEASURE_TIME_H

#include <chrono>
#include <functional>
#include <utility>
#include <ctime>
#include <stdexcept>

#define CHRONO std::chrono
#define STATIC

namespace zxy{

#define TIME_INTERVAL \
	(CHRONO::high_resolution_clock::now() - begin_)
#define DURATION_CAST(timetype, d) \
	(CHRONO::duration_cast<CHRONO::timetype>(d))
#define MEASURE_CLOCK \
	 CHRONO::high_resolution_clock

enum class TimeType: int
{
	seconds,
	milliseconds,
	microseconds,
	nanoseconds,
};

class MeasureTime
{
public:
	using count_t = unsigned long long;

	explicit MeasureTime(char const* func_name, TimeType ty=TimeType::nanoseconds)
		: func_name_{func_name},
		  begin_{MEASURE_CLOCK::now()},
		  time_t_{ty}
	{};
	
	void reset()
	{
		begin_ = MEASURE_CLOCK::now();
	}

	auto interval() const
	{
		return TIME_INTERVAL;
	}

	~MeasureTime()
	{
		switch(time_t_)
		{
			case TimeType::seconds:
				printTime(DURATION_CAST(seconds, TIME_INTERVAL));break;
			case TimeType::milliseconds:
				printTime(DURATION_CAST(milliseconds, TIME_INTERVAL));break;
			case TimeType::microseconds:
				printTime(DURATION_CAST(microseconds, TIME_INTERVAL));break;
			case TimeType::nanoseconds:
				printTime(DURATION_CAST(nanoseconds, TIME_INTERVAL));break;
		}
	}

STATIC
	static count_t diff(MEASURE_CLOCK::time_point const& left,
						MEASURE_CLOCK::time_point const& right,
						TimeType ty=TimeType::nanoseconds)
	{
		auto interval = right - left;
		switch(ty)
		{
			case TimeType::seconds:
				return static_cast<count_t>(DURATION_CAST(seconds, interval).count());break;
			case TimeType::milliseconds:
				return static_cast<count_t>(DURATION_CAST(milliseconds, interval).count());break;
			case TimeType::microseconds:
				return static_cast<count_t>(DURATION_CAST(microseconds, interval).count());break;
			case TimeType::nanoseconds:
				return static_cast<count_t>(DURATION_CAST(nanoseconds, interval).count());break;
		}
	}
	
private:
	void printTime(CHRONO::seconds const& dura) const
	{
		printf("Function { %s }: %llus\n", func_name_, 
				static_cast<count_t>(dura.count()));
	}
	
	void printTime(CHRONO::milliseconds const& dura) const
	{
		printf("Function { %s }: %llums\n", func_name_, 
				static_cast<count_t>(dura.count()));	
	}

	void printTime(CHRONO::microseconds const& dura) const
	{
		printf("Function { %s }: %lluus\n", func_name_, 
				static_cast<count_t>(dura.count()));
	}

	void printTime(CHRONO::nanoseconds const& dura) const
	{
		printf("Function { %s }: %lluns\n", func_name_, 
				static_cast<count_t>(dura.count()));
	}

private:
	MEASURE_CLOCK::time_point begin_;
	char const* func_name_;
	TimeType time_t_;
};

#define MEASURETIME(tag, func_name) \
	auto mt_##tag = zxy::MeasureTime{func_name}

#define MEASURETIMEty(tag, func_name, ty)\
	auto mt_##tag = zxy::MeasureTime{func_name, ty}

} //namespace zxy

#endif
