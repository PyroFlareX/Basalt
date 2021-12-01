#include "JobSystem.h"

#include "../Types/Types.h"

namespace bs
{
	JobSystem::JobSystem()	:	normalPriority(JobSystem::NORM_PRIORITY_SIZE),
			lowPriority(JobSystem::LOW_PRIORITY_SIZE), highPriority(JobSystem::HIGH_PRIORITY_SIZE),
			nonCounterJobs(JobSystem::UNCOUNTED_SIZE),	m_running(true),
			m_main_counter(0),	m_background_counter(0)
	{
		const u32 coreCount = numThreads() - 1;

		for(u32 i = 0; i < coreCount; ++i)
		{
			//Construct Threads
			m_threads.emplace_back(&JobSystem::thread_loop, this);
		}
		m_threads.shrink_to_fit();
	}

	Job JobSystem::createJob(JobFn task, void** data) const noexcept
	{
		return Job(task, *const_cast<Counter*>(&m_main_counter), data);
	}

	void JobSystem::schedule(Job job)
	{
		job.counter = &m_main_counter;
		*job.counter++;
		normalPriority.emplace(job);
	}

	void JobSystem::schedule(Job job, Counter& counter)
	{
		job.counter = &counter;
		*job.counter++;
		normalPriority.emplace(job);
	}

	void JobSystem::scheduleHighPriority(Job job)
	{
		job.counter = &m_main_counter;
		*job.counter++;
		highPriority.emplace(job);
	}

	void JobSystem::scheduleHighPriority(Job job, Counter& counter)
	{
		job.counter = &counter;
		*job.counter++;
		highPriority.emplace(job);
	}

	void JobSystem::scheduleLowPriority(Job job)
	{
		job.counter = &m_main_counter;
		*job.counter++;
		lowPriority.emplace(job);
	}

	void JobSystem::scheduleLowPriority(Job job, Counter& counter)
	{
		job.counter = &counter;
		*job.counter++;
		lowPriority.emplace(job);
	}

	void JobSystem::scheduleBackground(Job job)
	{
		job.counter = &m_background_counter;
		*job.counter++;
		nonCounterJobs.emplace(job);
	}

	void JobSystem::wait(const unsigned int target)
	{
		waitWithCounter(target, m_main_counter);
	}

	void JobSystem::waitBackground(const unsigned int target)
	{
		waitWithCounter(target, m_background_counter);
	}

	void JobSystem::waitWithCounter(const unsigned int target, const Counter& counter)
	{
		bool shouldExecuteOtherJobs = true;

		while(target < counter.load())
		{
			//Do some jobs on the waiting thread
			if(shouldExecuteOtherJobs)
			{
				Job job;
				if(highPriority.try_pop(job))
				{
					job.execute();
				}
				else if(normalPriority.try_pop(job))
				{
					job.execute();
				}
				else if(lowPriority.try_pop(job))
				{
					job.execute();
				}
				else if(nonCounterJobs.try_pop(job))
				{
					job.execute();
				}
			}
		}
	}

	int JobSystem::getThreadIndex() const
	{
		const auto id = std::this_thread::get_id();
		for(auto i = 0; i < m_threads.size(); ++i)
		{
			if(id == m_threads[i].get_id())
			{
				return i;
			}
		}
		return -1;
	}

	const unsigned int JobSystem::numThreads() noexcept
	{
		return std::thread::hardware_concurrency();
	}

	unsigned int JobSystem::getNumRemainingJobs() const noexcept
	{
		return m_main_counter.load();
	}

	unsigned int JobSystem::getNumBackgroundJobs() const noexcept
	{
		return m_background_counter.load();
	}

	JobSystem::~JobSystem()
	{
		m_running.store(false);
		for(auto& t : m_threads)
		{
			t.join();
		}
	}

	void JobSystem::thread_loop()
	{
		using namespace std::chrono_literals;
		constexpr auto sleep_duration = 100ns;
		while(m_running)
		{
			Job job;

			if(highPriority.try_pop(job))
			{
				job.execute();
			}
			else if(normalPriority.try_pop(job))
			{
				job.execute();
			}
			else if(lowPriority.try_pop(job))
			{
				job.execute();
			}
			else if(nonCounterJobs.try_pop(job))
			{
				job.execute();
			}
			else
			{
				std::this_thread::sleep_for(sleep_duration);
			}
		}
	}


	//For getting the jobsystem
	JobSystem& getJobSystem()
	{
		static JobSystem jobSystem;
		return jobSystem;
	}
}