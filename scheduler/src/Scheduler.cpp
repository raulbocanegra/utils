#include <scheduler/Scheduler.h>

using namespace rboc::utils::scheduler;

void Scheduler::stop()
{
	_running = false;
}

void Scheduler::addCronTask(const std::string & name, const std::string & cron, const std::function<void(void)>& callback, int & error)
{
	/*details::SchedulerData data(scheduler_type::cron, name, nullptr);
	auto result = _data_map.emplace(std::make_pair(name, std::move(data)));
	error = (result.second) ? 0 : 1;*/
}
