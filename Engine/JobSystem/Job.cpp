#include "JobSystem.h"

Job::Job() noexcept	:	job_task(nullptr), counter(nullptr), user_data(nullptr)
{

}

Job::Job(JobFn task) noexcept	:	job_task(task), counter(nullptr), user_data(nullptr)
{

}

Job::Job(JobFn task, void** data) noexcept	:	job_task(task), counter(nullptr), user_data(data)
{

}

Job::Job(JobFn task, Counter& counter) noexcept	:	job_task(task), counter(&counter), user_data(nullptr)
{

}

Job::Job(JobFn task, Counter& counter, void** data) noexcept	:	job_task(task), counter(&counter), user_data(data)
{

}

#if JOB_STRUCT_PADDING
Job::Job(JobFn task, void** data, const size_t array_len) noexcept : job_task(task), counter(&counter), user_data(data)
{
	length = array_len;
}

Job::Job(JobFn task, Counter& counter, void** data, const size_t array_len) noexcept : job_task(task), counter(&counter), user_data(data)
{
	length = array_len;
}
#endif