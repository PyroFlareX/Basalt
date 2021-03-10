#include "JobSystem.h"


JobSystem::JobSystem() : normalPriority(NORMAL_PRIORITY_SIZE) {
	unsigned int coreCount = std::thread::hardware_concurrency();

	coreCount--;

	running.store(true);

	for (int i = coreCount; i > 0; i--) {
		threads.emplace_back(std::thread(&JobSystem::threadLoop, this));
	}
	threads.shrink_to_fit();
}

Job JobSystem::createJob(JobFn job, void **data) {
	return Job{job, data, nullptr};
}

void JobSystem::schedule(Job job) {
	m_counter++;
	normalPriority.emplace(job);
}

void JobSystem::wait(unsigned int counterTarget, bool stayOnThread) {
	Job job;

	while (counterTarget < m_counter.load()) {
		//Do some jobs on the waiting thread
		if (normalPriority.try_pop(job)) {
			job.job_Function(job);
			m_counter--;
		}
	}

	return;
}

int JobSystem::getThreadIndex() {
	auto id = std::this_thread::get_id();

	for (int i = 0; i < threads.size(); ++i) {
		if (id == threads.at(i).get_id()) {
			return i;
		}
	}
	return -1;
}

JobSystem::~JobSystem() {
	running.store(false);
	for (auto &t : threads) {
		t.join();
	}
}

void JobSystem::threadLoop() {
	Job job;
	using namespace std::chrono_literals;
	auto d = 1ms;
	while (running) {
		if (normalPriority.try_pop(job)) {
			job.job_Function(job);
			m_counter--;
		} else {
			std::this_thread::sleep_for(d);
		}
	}
}