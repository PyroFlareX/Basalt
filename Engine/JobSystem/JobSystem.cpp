#include "JobSystem.h"


JobSystem::JobSystem()	:	normalPriority(NORMAL_PRIORITY_SIZE)
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

Job JobSystem::createJob(JobFn job, void* data)
{
	return Job{ job, data, nullptr };
}

void JobSystem::schedule(Job job)
{
	m_counter++;
	normalPriority.emplace(job);
}

void JobSystem::wait(unsigned int counterTarget, bool stayOnThread)
{
	/*
	if (counterTarget <= m_counter.load)
	{
		return;
	}
	*/

	while (counterTarget > m_counter.load())
	{

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
	while (running)
	{
		if (normalPriority.try_pop(job))
		{
			m_counter--;
			job.job_Function(job);
		}
	}
}