#include "Date.h"

namespace bs
{
	Date::Date()	:	m_day(1), m_year(1)	
	{

	}

	Date::Date(const u16 year, const u16 day)	:	m_day(day), m_year(year)
	{
		assert(m_day <= 365);
	}

	Date::Date(const u16 year, const u16 month, const u16 day)	:	m_day(day), m_year(year)
	{
		//Month bounds: [1, 12] (Jan = 1, Dec = 12)
		assert(month >= 1);
		assert(month <= NUM_MONTHS);
		assert(m_day <= 31);

		//Convert the month into the days format
		switch (month)
		{
			case 12://Nov
				m_day += NOVEMBER_DAYS;
			case 11://Oct
				m_day += OCTOBER_DAYS;
			case 10://Sep
				m_day += SEPTEMBER_DAYS;
			case 9:	//Aug
				m_day += AUGUST_DAYS;
			case 8:	//Jul
				m_day += JULY_DAYS;
			case 7:	//Jun
				m_day += JUNE_DAYS;
			case 6:	//May
				m_day += MAY_DAYS;
			case 5:	//Apr
				m_day += APRIL_DAYS;
			case 4:	//March
				m_day += MARCH_DAYS;
			case 3:	//Feb
				m_day += FEBRUARY_DAYS;
			case 2: //Jan
				m_day += JANUARY_DAYS;
				break;
		}
		
		assert(m_day <= 365);
	}

	u16& Date::getDayInYear()
	{
		return m_day;
	}

	u16& Date::getYear()
	{
		return m_year;
	}

	u16 Date::getMonth() const
	{
		auto numdays = m_day;
		if(numdays <= 31)
		{
			//January
			return 1;
		}
		else
		{
			numdays -= 31;
			//February
			if(numdays <= 28)
			{
				return 2;
			}
			//Continue this
				numdays -= 28;
				//March
				if(numdays <= 31)
				{
					return 3;
				}
				else
				{
					numdays -= 31;
					//April
					if(numdays <= 30)
					{
						return 4;
					}
					else
					{
						numdays -= 30;
						//May
						if(numdays <= 31)
						{
							return 5;
						}
						else
						{
							numdays -= 31;
							//June
							if(numdays <= 30)
							{
								return 6;
							}
							else 
							{
								numdays -= 30;
								//July
								if(numdays <= 31)
								{
									return 7;
								}
								else
								{
									numdays -= 31;
									//August
									if(numdays <= 31)
									{
										return 8;
									}
									else
									{
										numdays -= 31;
										//September
										if(numdays <= 30)
										{
											return 9;
										}
										else
										{
											numdays -= 30;
											//October
											if(numdays <= 31)
											{
												return 10;
											}
											else
											{
												numdays -= 31;
												//November
												if(numdays <= 30)
												{
													return 11;
												}
												else
												{
													numdays -= 30;
													//December
													return 12;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			
		}
	}

	const std::string Date::getText() const
	{
		std::string d = "";
		auto lday = m_day;
		auto lmonth = getMonth();

		switch(lmonth)
		{
			case 1:
				d += "January ";
				break;
			case 2:
				d += "February ";
				lday -= 31;
				break;
			case 3:
				d += "March ";
				lday -= 31 + 28;
				break;
			case 4:
				d += "April ";
				lday -= 31 + 28 + 31;
				break;
			case 5:
				d += "May ";
				lday -= 31 + 28 + 31 + 30;
				break;
			case 6:
				d += "June ";
				lday -= 31 + 28 + 31 + 30 + 31;
				break;
			case 7:
				d += "July ";
				lday -= 31 + 28 + 31 + 30 + 31 + 30;
				break;
			case 8:
				d += "August ";
				lday -= 31 + 28 + 31 + 30 + 31 + 30 + 31;
				break;
			case 9:
				d += "September ";
				lday -= 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31;
				break;
			case 10:
				d += "October ";
				lday -= 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30;
				break;
			case 11:
				d += "November ";
				lday -= 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31;
				break;
			case 12:
				d += "December ";
				lday -= 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30;
				break;
		}

		d += std::to_string(lday);

		d += ", ";

		//Year
		d += std::to_string(m_year);

		//February 7, 1836
		//Month day, year
		return d;
	}
}