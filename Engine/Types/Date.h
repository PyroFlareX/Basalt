#pragma once

#include "Types.h"

namespace bs
{
	constexpr auto JANUARY_DAYS =	31;
	constexpr auto FEBRUARY_DAYS =	28;
	constexpr auto MARCH_DAYS	=	31;
	constexpr auto APRIL_DAYS	=	30;
	constexpr auto MAY_DAYS		=	31;
	constexpr auto JUNE_DAYS	=	30;
	constexpr auto JULY_DAYS	=	31;
	constexpr auto AUGUST_DAYS	=	31;
	constexpr auto SEPTEMBER_DAYS =	30;
	constexpr auto OCTOBER_DAYS =	31;
	constexpr auto NOVEMBER_DAYS =	30;
	constexpr auto DECEMBER_DAYS =	31;

	constexpr auto DAYS_IN_YEAR	=	365;
	constexpr auto NUM_MONTHS	=	12;

	class Date
	{
	public:
		Date();
		Date(const u16 year, const u16 day);
		Date(const u16 year, const u16 month, const u16 day);

		u16& getDayInYear();
		u16& getYear();

		u16 getMonth() const;

		const std::string getText() const;

	private:
		//Obvious, a year
		u16 m_year;

		//From 1 to 365, for each day of the year
		u16 m_day;
	};
}