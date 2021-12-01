#pragma once
#pragma warning (disable: 4267 4244 4005) // size_t to int conversion, to float conversion

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cctype>
#include <functional>
#include <string>

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
	
typedef char		i8;
typedef short		i16;
typedef int			i32;
typedef long long	i64;

namespace bs
{
	typedef glm::vec2	vec2;
	typedef glm::vec3	vec3;
	typedef glm::vec4	vec4;

	typedef glm::ivec2	vec2i;
	typedef glm::ivec3	vec3i;
	typedef glm::ivec4	vec4i;

	typedef glm::u8vec2	u8vec2;
	typedef glm::u8vec3	u8vec3;
	typedef glm::u8vec4	u8vec4;
	
	typedef glm::mat4	mat4;
	typedef glm::mat3	mat3;

	typedef struct ray_t
	{
		vec3 start;
		vec3 direction;
	} Ray;

	//A Transform, stores the information for 3d orientation
	class Transform
	{
	public:
		vec3 pos = vec3(0.0f, 0.0f, 0.0f);
		vec3 rot = vec3(0.0f, 0.0f, 0.0f);
		vec3 scale = vec3(1.0f, 1.0f, 1.0f);
		vec3 origin = vec3(0.0f, 0.0f, 0.0f);

		static Transform& translate(Transform& entity, const vec3& offset)
		{
			entity.pos += offset;
			return entity;
		}

		static Transform& rotate(Transform& entity, const vec3& angle)
		{
			entity.rot += angle;
			return entity;
		}

		static Transform& rescale(Transform& entity, const vec3& relativeScale)
		{
			entity.scale = entity.scale * relativeScale;
			return entity;
		}
	};
}

class Date
{
public:
	constexpr Date()
	{
		day = 1;
		year = 1836;
	}

	constexpr Date(u16 year, u16 day)
	{
		this->day = day;
		this->year = year;
	}

	constexpr Date(u16 year, u16 month, u16 day)
	{
		this->year = year;
		this->day = day;


		//for(int i = month; i > 0; --i)
		{
			switch (month)
			{
			case 12://Nov
			{
				this->day += 30;
				//break;
			}
			case 11://Oct
			{
				this->day += 31;
				//break;
			}
			case 10://Sep
			{
				this->day += 30;
				//break;
			}
			case 9:	//Aug
			{
				this->day += 31;
				// break;
			}
			case 8:	//Jul
			{
				this->day += 31;
				// break;
			}
			case 7:	//Jun
			{
				this->day += 30;
				// break;
			}
			case 6:	//May
			{
				this->day += 31;
				// break;
			}
			case 5:	//Apr
			{
				this->day += 30;
				// break;
			}
			case 4:	//March
			{
				this->day += 31;
				// break;
			}
			case 3:	//Feb
			{
				this->day += 28;
				// break;
			}
			case 2: //Jan
			{
				this->day += 31;
				break;
			}
				
			default:
				break;
			}
		}
	
	}

	static uint8_t getMonth(Date date)
	{
		if(date.day <= 31)
		{
			//January
			return 1;
		}
		else
		{
			date.day -= 31;
			//February
			if(date.day <= 28)
			{
				return 2;
			}
			else
			{
				date.day -= 28;
				//March
				if(date.day <= 31)
				{
					return 3;
				}
				else
				{
					date.day -= 31;
					//April
					if(date.day <= 30)
					{
						return 4;
					}
					else
					{
						date.day -= 30;
						//May
						if(date.day <= 31)
						{
							return 5;
						}
						else
						{
							date.day -= 31;
							//June
							if(date.day <= 30)
							{
								return 6;
							}
							else 
							{
								date.day -= 30;
								//July
								if(date.day <= 31)
								{
									return 7;
								}
								else
								{
									date.day -= 31;
									//August
									if(date.day <= 31)
									{
										return 8;
									}
									else
									{
										date.day -= 31;
										//September
										if(date.day <= 30)
										{
											return 9;
										}
										else
										{
											date.day -= 30;
											//October
											if(date.day <= 31)
											{
												return 10;
											}
											else
											{
												date.day -= 31;
												//November
												if(date.day <= 30)
												{
													return 11;
												}
												else
												{
													date.day -= 30;
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
	}

	u16& getDayInYear()
	{
		return day;
	}

	u16& getYear()
	{
		return year;
	}

	u16 getMonth()
	{
		short numdays = day;
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
			else
			{
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
	}

	const std::string getText() const
	{
		std::string d = "";

		int lday = this->day;
		int lmonth = this->getMonth(*this);
		int lyear = this->year;

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
		d += std::to_string(lyear);


		//February 7, 1836
		//Month day, year
		return d;
	}

private:
	//Obvious, a year
	u16 year;

	//From 1 to 365, for each day of the year
	u16 day;
};

//For hashing (e.g. usage in std::maps)
namespace std
{
	template<>
	struct hash<bs::vec3>
	{
		size_t operator()(const bs::vec3& vect) const noexcept
		{
			std::hash<decltype(vect.x)> hasher;

			auto hash1 = hasher(vect.x);
			auto hash2 = hasher(vect.y);
			auto hash3 = hasher(vect.z);

			return std::hash<decltype(vect.x)>{}((hash1 ^ (hash2 << hash3) ^ hash3));
		}
	};
}