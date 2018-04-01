#include <scheduler/Scheduler.h>
#include <windows.h>
#include <iostream>

using namespace rboc::utils::scheduler;

// Constant to calculate time in 100ns fragments.
static constexpr int32_t ONE_SECOND = 10000000;

//! SchedulerData
struct rboc::utils::scheduler::SchedulerData
{
	explicit SchedulerData(
		scheduler_type scheduler_type, 
		const std::string& name, 
		std::function<void(void)> callback)
		: _type(scheduler_type)
		, _timer(NULL)
		, _name(name)
		, _callback(std::move(callback))
	{}

	scheduler_type _type = scheduler_type::unknown;
	PTP_TIMER _timer = NULL;
	std::string _name = "";			
	std::function<void(void)> _callback = nullptr;
};

//! platform_deleter
/**
 * Custom deleter for unique_ptr<SchedulerData> to free Windows timer resources
 */
void platform_deleter(rboc::utils::scheduler::SchedulerData* p)
{
	SetThreadpoolTimer(p->_timer, NULL, 0, 0);
	WaitForThreadpoolTimerCallbacks(p->_timer, TRUE);
	CloseThreadpoolTimer(p->_timer);
}

void rboc::utils::scheduler::Scheduler::start()
{

}

static VOID CALLBACK timerCallback(
	PTP_CALLBACK_INSTANCE Instance,
	PVOID                 Context,
	PTP_TIMER             Timer
)
{
	auto sched_data_ptr = static_cast<SchedulerData*>(Context);
	//std::cout << "Executing timerCallback!" << std::endl;
	sched_data_ptr->_callback();
}

//! addPeriodicTask
void Scheduler::addPeriodicTask(
	const std::string & name, time_t period, const std::function<void(void)>& callback, int & error)
{
	// TODO: SetThreadpoolTimer accepts unsigned for period but we use time_t which is signed. Check for negative values.	

	auto sched_data = DataPtr(new SchedulerData(scheduler_type::periodic, name, std::move(callback)), platform_deleter);
	auto timer = CreateThreadpoolTimer(timerCallback, sched_data.get(), NULL);
	sched_data->_timer = timer; // set the timer here to be freed when the pointer is deleted.

	// Set the timer to fire in 'period' ms.
	ULARGE_INTEGER start_time;
    start_time.QuadPart = static_cast<ULONGLONG>(-((period / 1000) * ONE_SECOND));
	FILETIME fire_time;
	fire_time.dwHighDateTime = start_time.HighPart;
	fire_time.dwLowDateTime  = start_time.LowPart;

	SetThreadpoolTimer(timer, &fire_time, period, 0);
	
	auto result = _data_map.emplace(std::make_pair(name, std::move(sched_data)));
	
	if (!result.second)
	{
		error = 1;
	}
}

