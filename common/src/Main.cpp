#include <iostream>
#include <scheduler/Scheduler.h>
#include <chrono>

using namespace rboc::utils::scheduler;

int main()
{
	Scheduler sched;
	int error = 0;
	std::function<void(void)> f = [] () 
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::cout << "Executing callback from task 1 at " 
			<< now.time_since_epoch().count() <<std::endl;
	};

	sched.addPeriodicTask("task 1", 1000, f, error);

	std::this_thread::sleep_for(std::chrono::seconds(10));
}
