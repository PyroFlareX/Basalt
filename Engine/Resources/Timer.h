#pragma once

#include <chrono>

namespace bs
{
	//Clock/Timer
	class Clock
	{
	public:
		Clock()	:	time(std::chrono::steady_clock::now())	{	}

		//Get time since last restarted
		double getDT() const
		{
			const auto t = std::chrono::steady_clock::now();
			const auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t - time);
			return time_span.count();
		}
		//Get time since last restart, and restart
		double restart()
		{
			const auto x = getDT();
			time = std::chrono::steady_clock::now();
			return x;
		}
	private:
		std::chrono::steady_clock::time_point time;	
	};
}