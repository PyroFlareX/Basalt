#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <functional>

#include "../Types/rigtorp/MPMCQueue.h"

using Counter = std::atomic<unsigned int>;

constexpr unsigned int NUM_FIBERS = 256;
constexpr unsigned int NUM_FIBERS_WAITING = 256;

constexpr unsigned int HIGH_PRIORITY_SIZE = 1024;
constexpr unsigned int NORMAL_PRIORITY_SIZE = 1024;
constexpr unsigned int LOW_PRIORITY_SIZE = 1024;
constexpr unsigned int UNCOUNTED_SIZE = 1024;

struct Job;

//using JobFn = void(*)(Job);

using JobFn = std::function<void(Job)>;

struct Job
{
	JobFn job_Function = JobFn(nullptr);
	void** data = nullptr;
	
	Job() noexcept = default;
	Job(Job&&) noexcept = default;
	Job(Job const&) noexcept = default;
	Job& operator=(Job&&) noexcept = default;
	Job& operator=(Job const&) noexcept = default;
	~Job() noexcept = default;
};


class JobSystem
{
public:
	JobSystem();

	//Returns a Job from a function pointer and parameters, does not add the job to the list
	static Job createJob(JobFn job, void** data = nullptr, Counter* p_counter = nullptr);
	
	//Get number of hardware threads
	static uint8_t numThreads() noexcept
	{
		return std::thread::hardware_concurrency();
	}

	unsigned int remainingJobs() const noexcept
	{
		return m_counter.load();
	}

	unsigned int backgroundJobs() const noexcept
	{
		return m_bcounter.load();
	}

	//Schedules a job
	void schedule(Job job, bool counted = true);

	//Blocks execution until the job counter reaches the target
	void wait(unsigned int counterTarget = 0, bool stayOnThread = false, Counter* p_counter = nullptr);

	// Returns Index of thread the function is running in, where the index is from the threads vector
	int getThreadIndex();

	~JobSystem();
private:
	std::vector<std::thread> threads;

	rigtorp::MPMCQueue<Job> highPriority;
	rigtorp::MPMCQueue<Job> normalPriority;
	rigtorp::MPMCQueue<Job> lowPriority;
	rigtorp::MPMCQueue<Job> nonCounterJobs;


	void threadLoop();

	std::atomic<bool> running;
	Counter m_counter;


	Counter m_bcounter;
};


extern JobSystem jobSystem;