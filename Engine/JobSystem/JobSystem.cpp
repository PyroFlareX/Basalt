#include "JobSystem.h"


JobSystem::JobSystem()	:	normalPriority(NORMAL_PRIORITY_SIZE), lowPriority(LOW_PRIORITY_SIZE), highPriority(HIGH_PRIORITY_SIZE), nonCounterJobs(UNCOUNTED_SIZE)
{
	unsigned int coreCount = std::thread::hardware_concurrency();

	coreCount--;
	
	running.store(true);

	for (int i = coreCount; i > 0; i--)
	{
		threads.emplace_back(std::thread(&JobSystem::threadLoop, this));
	}
	threads.shrink_to_fit();
}

Job JobSystem::createJob(JobFn job, void** data, Counter* p_counter)
{
	Job j;
	j.job_Function = job;
	j.data = data;
	
	return j;
}

void JobSystem::schedule(Job job, bool counted)
{
	if(counted)
	{	
		m_counter++;
		normalPriority.emplace(job);
	}
	else
	{
		m_bcounter++;
		nonCounterJobs.emplace(job);
	}
}

void JobSystem::wait(unsigned int counterTarget, bool stayOnThread, Counter* p_counter)
{
	Job job;

	while (counterTarget < m_counter.load())
	{
		//Do some jobs on the waiting thread
		if(!stayOnThread)
		{
			if(normalPriority.try_pop(job))
			{
				job.job_Function(job);
				m_counter--;
			}
		}
	}
	
	return;
}

int JobSystem::getThreadIndex()
{
	auto id = std::this_thread::get_id();

	for (int i = 0; i < threads.size(); ++i)
	{
		if (id == threads.at(i).get_id())
		{
			return i;
		}
	}
	return -1;
}

JobSystem::~JobSystem()
{
	running.store(false);
	for (auto& t : threads)
	{
		t.join();
	}
}

void JobSystem::threadLoop()
{
	Job job;
	using namespace std::chrono_literals;
	auto d = 100ns;
	while (running)
	{
		if (normalPriority.try_pop(job))
		{
			job.job_Function(job);
			m_counter--;
		}
		else if(nonCounterJobs.try_pop(job))
		{
			job.job_Function(job);
			m_bcounter--;
		}
		else
		{
			std::this_thread::sleep_for(d);
		}
	}
}