#pragma once

#include <chrono>

namespace bs
{
	//Clock/Timer
	class Clock
	{
	public:
		Clock()
		{
			restart();
		}
		//Get time since last restarted
		double getDT() const
		{
			std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t - time);
			return time_span.count();
		}
		//Get time since last restart, and restart
		double restart()
		{
			double x = getDT();
			time = std::chrono::steady_clock::now();
			return x;
		}

	private:
		std::chrono::steady_clock::time_point time;

	};
}