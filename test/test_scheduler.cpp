#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <scheduler/Scheduler.h>
#include <chrono>

using namespace rboc::utils::scheduler;

TEST_CASE("Scheduler Tests")
{
	Scheduler sched;

	SECTION("One periodic task should work")
	{
		int error = 0;
		int counter = 0;
		sched.addPeriodicTask("task 1", 1000, 
			[&counter] () 
			{
				++counter;
			}, error);
		std::this_thread::sleep_for(std::chrono::seconds(5));

		CHECK(error == 0);
		CHECK(counter == 5);
	}
	
}