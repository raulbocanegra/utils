#pragma once
#ifndef THREADING_THREADPOOL_HEADER
#define THREADING_THREADPOOL_HEADER

#include <vector>
#include <utils/threading/ActiveWorker.h>

namespace rboc { namespace utils { namespace threading
{
	template<typename R, typename... Args>
	class ThreadPool
	{
		public:

		ThreadPool(unsigned num_threads = 1)
			:_workers(num_threads)
		{}

		void start()
		{
			std::lock_guard<std::mutex> lock(_mtx);
			for (auto& worker : _workers)
			{
				worker.start();
			}
		}

		void stop()
		{
			std::lock_guard<std::mutex> lock(_mtx);
			for (auto& worker : _workers)
			{
				worker.stop();
			}
		}
		
		template<typename F>
		std::future<R> addTask(F&& f, Args... args)
		{
			std::lock_guard<std::mutex> lock(_mtx);
			_idx = (_idx % _workers.size());
			return _workers[_idx++].addWork(std::forward<F>(f), std::forward<Args>(args)...);			
		}

		private:

		unsigned _idx = 0;
		std::vector<ActiveWorker<R,Args...>> _workers{1};
		mutable std::mutex _mtx = {}; // Mutex to protect the workers.
	};

}}} // rboc::utils::threading
#endif THREADING_THREADPOOL_HEADER