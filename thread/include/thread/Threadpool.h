#pragma once
#ifndef THREADING_THREADPOOL_HEADER
#define THREADING_THREADPOOL_HEADER

#include <vector>
#include <thread/ActiveWorker.h>

namespace rboc { namespace utils { namespace threading
{
	/*! 
	 * This is a Thread Pool class that consists in a vector of ActiveWorkers 
	 * that will have tasks scheduled in a round robin fashion.
	 */
	template<typename R, typename... Args>
	class ThreadPool
	{
		public:

		//! Default cosntructor
		ThreadPool()
			: _workers(1)
		{}
		
		//! Explicit constructor
		explicit ThreadPool(size_t num_threads)
			: _workers(num_threads)
		{}
		
		//! stop
		/*!
		 * Stops the thread pool.
		 */
		void stop()
		{
			std::lock_guard<std::mutex> lock(_mtx);
			for (auto& worker : _workers)
			{
				worker.stop();
			}
		}
		
		//! addTask.
		/** 
		 * Adds tasks to the thread pool.
		 * \param f the function to be executed.
		 * \param Args... the arguments to be passed to the function f.
		 */
		template<typename F>
		std::future<R> addTask(F&& f, Args... args)
		{
			std::lock_guard<std::mutex> lock(_mtx);
			_idx = (_idx % _workers.size());
			return _workers[_idx++].addWork(std::forward<F>(f), std::forward<Args>(args)...);			
		}

		private:

		size_t _idx = 0;
		std::vector<ActiveWorker<R,Args...>> _workers{1};
		mutable std::mutex _mtx = {}; /*! < Mutex to protect the workers. */
	};

}}} // rboc::utils::threading
#endif // THREADING_THREADPOOL_HEADER