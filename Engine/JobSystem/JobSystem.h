#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <functional>

#include "../Types/rigtorp/MPMCQueue.h"

using Counter = std::atomic<unsigned int>;

struct Job;
using JobFn = std::function<void(Job)>;

#define JOB_STRUCT_PADDING 0

struct Job
{
	Job() noexcept;
	Job(JobFn task) noexcept;
	Job(JobFn task, void** data) noexcept;
	Job(JobFn task, Counter& counter) noexcept;
	Job(JobFn task, Counter& counter, void** data) noexcept;
	
#if JOB_STRUCT_PADDING
	Job(JobFn task, void** data, const size_t array_len) noexcept;
	Job(JobFn task, Counter& counter, void** data, const size_t array_len) noexcept;
#endif
	//No-throw Destructable
	~Job() noexcept = default;

	///	Members
	//The task function, a lambda
	JobFn job_task;

	//Pointer to the Counter this job is associated with
	Counter* counter;

	//Pointer to user data, i.e. pointer to an array of pointers
	void** user_data;

#if JOB_STRUCT_PADDING
	size_t length = 0;	//Length of the userdata pointer array
	size_t null = 0; //Filler to get to 96 bytes (maybe cacheline idk)
#endif

	///Helper Functions
	//Execute the job
	inline void execute()
	{
		job_task(*this);
		
		assert(counter != nullptr);
		*counter--;
	}
	//Retreive the pointer passed into the job userdata at this index
	template<typename T>
	T* getPointerAtIndex(const int index)
	{
		assert((length > index) && (user_data != nullptr));
		return reinterpret_cast<T*>(user_data[index]);
	}
	//Get a reference to the item passed in this index of the userdata
	template<typename T>
	T& getReferenceAtIndex(const int index)
	{
		return *getPointerAtIndex<T>(index);
	}

	Job(Job&&) noexcept = default;
	Job(Job const&) noexcept = default;
	Job& operator=(Job&&) noexcept = default;
	Job& operator=(Job const&) noexcept = default;
};

namespace bs
{
	class JobSystem
	{
	public:
		JobSystem();
		~JobSystem();
		
		//Schedule a job
		void schedule(Job job);
		void schedule(Job job, Counter& counter);

		void scheduleHighPriority(Job job);
		void scheduleHighPriority(Job job, Counter& counter);

		void scheduleLowPriority(Job job);
		void scheduleLowPriority(Job job, Counter& counter);

		void scheduleBackground(Job job);

		//Blocks execution until the job counter reaches the target
		void wait(const unsigned int target);
		void waitBackground(const unsigned int target);
		void waitWithCounter(const unsigned int target, const Counter& counter);

		///Info and Utility Functions

		// Returns Index of thread the function is running in, where the index is from the threads vector
		int getThreadIndex() const;

		//Get number of hardware threads
		static const unsigned int numThreads() noexcept;

		//Get the remaining number of Jobs on the counter
		unsigned int getNumRemainingJobs() const noexcept;

		//Get the remaining number of Jobs on the background counter
		unsigned int getNumBackgroundJobs() const noexcept;

		constexpr static auto HIGH_PRIORITY_SIZE	=	1024;
		constexpr static auto NORM_PRIORITY_SIZE	=	1024;
		constexpr static auto LOW_PRIORITY_SIZE		=	1024;
		constexpr static auto UNCOUNTED_SIZE		=	1024;
	private:
		//Executed by the job threads
		void thread_loop();

		std::vector<std::thread> m_threads;

		rigtorp::MPMCQueue<Job> highPriority;
		rigtorp::MPMCQueue<Job> normalPriority;
		rigtorp::MPMCQueue<Job> lowPriority;
		rigtorp::MPMCQueue<Job> nonCounterJobs;

		std::atomic<bool> m_running;

		Counter m_main_counter;
		Counter m_background_counter;
	};

	JobSystem& getJobSystem();
}