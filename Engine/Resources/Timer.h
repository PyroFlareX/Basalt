#pragma once

#include <chrono>

namespace vn
{
	class Clock
	{
	public:
		Clock()
		{
			restart();
		}

		double getDT() const
		{
			std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t - time);
			return time_span.count();
		}

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