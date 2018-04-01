#include <scheduler/Scheduler.h>
#include <iostream>

#include <signal.h>
#include <time.h>

using namespace rboc::utils::scheduler;

/**
 * SchedulerData
 */
struct rboc::utils::scheduler::SchedulerData
{
	explicit SchedulerData(
		scheduler_type scheduler_type, 
		const std::string& name, 
		std::function<void(void)> callback)
		: _type(scheduler_type)
		, _timer_id(0)
		, _name(name)
		, _callback(std::move(callback))
	{}

	scheduler_type _type = scheduler_type::unknown;
	timer_t _timer_id = 0;
	std::string _name = "";			
	std::function<void(void)> _callback = nullptr;
};

/**
 * platform_deleter: Custom deleter for unique_ptr<SchedulerData> to free Windows timer resources
 */
void platform_deleter(rboc::utils::scheduler::SchedulerData* p)
{
	if (timer_delete(p->_timer_id) == -1)
	{
		// ERROR: Setting the timer.
	}
}

/**
 * start
 */
void rboc::utils::scheduler::Scheduler::start()
{

}


/**
 * timerCallback: The callback to be used by CreateThreadpoolTimer.
 */
static void timerCallback(sigval Context)
{
	auto sched_data_ptr = static_cast<SchedulerData*>(Context.sival_ptr);
	//std::cout << "Executing timerCallback!" << std::endl;
	sched_data_ptr->_callback();
}

/**
 * addPeriodicTask
 */
void Scheduler::addPeriodicTask(
	const std::string & name, time_t period, const std::function<void(void)>& callback, int & error)
{
	auto sched_data = DataPtr(new SchedulerData(scheduler_type::periodic, name, std::move(callback)), platform_deleter);

	struct sigevent signal_data;
	signal_data.sigev_notify = SIGEV_THREAD;
	signal_data.sigev_signo = 0;
	signal_data.sigev_value.sival_ptr = sched_data.get();
	signal_data.sigev_notify_function = timerCallback;
	signal_data.sigev_notify_attributes = NULL;

	timer_t timer_id;
	if (timer_create(CLOCK_REALTIME, &signal_data, &timer_id) == -1)
	{
		error = errno;
		return;
	}
	sched_data->_timer_id = timer_id;
	
	struct itimerspec fire_time;
	fire_time.it_value.tv_sec = period / 1000;
	fire_time.it_value.tv_nsec = (period % 1000) * 1000000;
	fire_time.it_interval.tv_sec = fire_time.it_value.tv_sec;
	fire_time.it_interval.tv_nsec = fire_time.it_value.tv_nsec;

	if (timer_settime(timer_id, 0, &fire_time, NULL) == -1)
	{
		error = errno;
		return;
	}

	auto result = _data_map.emplace(std::make_pair(name, std::move(sched_data)));

	if (!result.second)
	{
		error = 1;
		return;
	}
}