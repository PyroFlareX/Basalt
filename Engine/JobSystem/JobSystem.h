#pragma once

#include <atomic>
#include <vector>
#include <thread>

#include "Fiber.h"
#include "../Types/rigtorp/MPMCQueue.h"

using Counter = std::atomic<unsigned int>;

constexpr unsigned int NUM_FIBERS = 256;
constexpr unsigned int NUM_FIBERS_WAITING = 256;

constexpr unsigned int HIGH_PRIORITY_SIZE = 256;
constexpr unsigned int NORMAL_PRIORITY_SIZE = 256;
constexpr unsigned int LOW_PRIORITY_SIZE = 256;

struct Job;

using JobFn = void (*)(Job);

struct Job {
	JobFn job_Function;
	void **data;
	Counter *counter;
};


class JobSystem {
public:
	JobSystem();

	//Returns a Job from a function pointer and parameters, does not add the job to the list
	static Job createJob(JobFn job, void **data = nullptr);

	static uint8_t numThreads() {
		return std::thread::hardware_concurrency();
	}

	//Schedules a job
	void schedule(Job job);

	//Blocks execution until the job counter reaches the target
	void wait(unsigned int counterTarget = 0, bool stayOnThread = false);

	// Returns Index of thread the function is running in, where the index is from the threads vector
	int getThreadIndex();

	~JobSystem();

private:
	std::vector<std::thread> threads;
	//std::vector<Fiber> fibers;


	rigtorp::MPMCQueue<Job> normalPriority;


	void threadLoop();

	std::atomic<bool> running;
	Counter m_counter;
};

