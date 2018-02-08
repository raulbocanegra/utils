#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <utility>
#include <utils/threading/ActiveWorker.h>
#include <utils/threading/ThreadPool.h>
#include <utils/utility/Utility.h>

using namespace rboc::utils;
using namespace rboc::utils::threading;

namespace // Anonymous namespace for test functions
{
	int increment(int i)
	{
		return ++i;
	}

	void increment_ref(int& i)
	{
		++i;
	}
	
	struct Incrementer
	{
		int increment(int i) const
		{
			return ++i;
		}		
	};
}

TEST_CASE("Active Worker tests should pass", "[active_worker]")
{
	SECTION("ActiveWorker<int, int> should pass")
	{
		int test = 0;
		ActiveWorker<int, int> worker;
		auto fut_w1 = worker.addWork([](int i){ return ++i; }, test);		
		auto result_1 = fut_w1.get();
		auto fut_w2 = worker.addWork(increment, result_1);
		Incrementer inc;
		auto fut_w3 = worker.addWork(std::bind(&Incrementer::increment, &inc, std::placeholders::_1), result_1);
		
		CHECK(worker.isRunning());		
		worker.stop();
		auto w3 = fut_w3.get();
		CHECK_FALSE(worker.isRunning());
		CHECK(result_1 == (test + 1));
		CHECK(fut_w2.get() == (test + 2));
		CHECK(w3 == (test + 2));
	}
	SECTION("ActiveWorker<void> should pass")
	{
		int works = 1000;
		int test = 0;
		ActiveWorker<void> worker;
		auto inc_lambda = [&test]{ ++test; };
		for (size_t i = 0; i < works - 1; ++i)
		{
			worker.addWork(inc_lambda);
		}		
		auto fut_last_result = worker.addWork(inc_lambda);
		CHECK(worker.isRunning());
		fut_last_result.get();		
		worker.stop();
		CHECK_FALSE(worker.isRunning());
		CHECK(test == works);
	}
	SECTION("ActiveWorker<int> with no args should pass")
	{
		int test = 0;
		ActiveWorker<int> worker;
		auto fut_w1 = worker.addWork([&]{ return ++test; });
		auto fut_w2 = worker.addWork([&]{ return ++test; });
		auto fut_w3 = worker.addWork([&]{ return ++test; });
		CHECK(worker.isRunning());
		auto w3 = fut_w3.get();
		worker.stop();		
		CHECK_FALSE(worker.isRunning());
		CHECK(fut_w1.get() == 1);
		CHECK(fut_w2.get() == 2);
		CHECK(w3 == 3);
	}
}

TEST_CASE("ThreadPool tests should pass", "[thread_pool]")
{	
	SECTION("ThreadPool<int> with no args should pass")
	{
		std::mutex tc_mtx;
		int inc = 0;
		auto task = [&]
		{
			std::lock_guard<std::mutex> lock(tc_mtx);
			++inc;
		};
		ThreadPool<void> tp{8};
		std::vector<std::future<void>> results;
		for (size_t i = 0; i < 10000; i++)
		{
			results.emplace_back(tp.addTask(task));
		}
		// when all are ready
		std::for_each(results.begin(), results.end(), [](auto& fut) { fut.get(); });
		CHECK(inc == 10000);		
	}
}
