#pragma once
#ifndef THREADING_ACTIVEWORKER_HEADER
#define THREADING_ACTIVEWORKER_HEADER

#include <deque>
#include <future>
#include <atomic>
#include <type_traits>
#include <utils/utility/Utility.h>

namespace rboc { namespace utils { namespace threading
{
	template<typename R, typename... Args>
	class ActiveWorker
	{
		public:

		ActiveWorker()
			: _running(true)
			, _queue{}
		{
			std::thread worker_thread(&ActiveWorker::work, this);
			worker_thread.detach();
		}
		
		template<typename F, typename = std::enable_if<!std::is_reference<Args...>::value>::type>
		std::future<R> addWork(F&& f, Args... args)
		{			
			static_assert(sizeof...(args) == std::tuple_size<std::tuple<Args...>>::value,
				"number of params in object declaration and adding work must match");
			std::packaged_task<R(Args...)> task{std::forward<F>(f)};
			auto result = task.get_future();
			{
				std::lock_guard<std::mutex> queue_lock(_mtx);				
				_queue.emplace_back(std::make_pair(std::move(task), std::make_tuple(args...)));
			}
			_empty_queue_cond.notify_one();
			return result;
		}
		
		void stop()
		{
			_running = false;
			_empty_queue_cond.notify_one();			
		}
		
		size_t getQueueSize() const
		{			
			std::lock_guard<std::mutex> queue_lock(_mtx);
			return _queue.size();			
		}

		bool isRunning() const
		{	
			return _running;			
		}

		private:
		
		// private functions.
		void work()
		{			
			while (true)
			{
				std::unique_lock<std::mutex> cond_lock(_mtx);
				_empty_queue_cond.wait(cond_lock, [this]{ return !_queue.empty() || !_running; });
				
				if (!_running) break;

				auto task = std::move(_queue.front());
				_queue.pop_front();
				cond_lock.unlock();
				utilities::call_task(std::move(task.first), std::move(task.second));
			}
		}

		// Private members.
		std::atomic_bool _running;
		std::deque<std::pair<std::packaged_task<R(Args...)>, std::tuple<Args...>>> _queue;
		mutable std::mutex _mtx; // Mutex to protect the queue and running bool.
		std::condition_variable _empty_queue_cond;
	};

	// Specialization for 0 argument functions.
	template<typename R>
	class ActiveWorker<R>
	{
		public:

		ActiveWorker()
			: _running(true)
			, _queue{}
		{
			std::thread worker_thread(&ActiveWorker::work, this);
			worker_thread.detach();
		}

		template<typename F>
		std::future<R> addWork(F&& f)
		{			
			std::packaged_task<R()> task{std::forward<F>(f)};
			auto result = task.get_future();
			{
				std::lock_guard<std::mutex> queue_lock(_mtx);				
				_queue.emplace_back(std::move(task));
			}
			_empty_queue_cond.notify_one();
			return result;
		}
		
		void stop()
		{
			_running = false;
			_empty_queue_cond.notify_one();			
		}

		size_t getQueueSize() const
		{			
			std::lock_guard<std::mutex> queue_lock(_mtx);
			return _queue.size();			
		}

		bool isRunning() const
		{	
			return _running;			
		}

		private:

		// private functions.
		void work()
		{			
			while (true)
			{
				std::unique_lock<std::mutex> cond_lock(_mtx);
				_empty_queue_cond.wait(cond_lock, [this](){ return !_queue.empty() || !_running; });

				if (!_running) break;

				auto task = std::move(_queue.front());
				_queue.pop_front();
				cond_lock.unlock();
				task();
			}
		}

		// Private members.
		std::atomic_bool _running;
		std::deque<std::packaged_task<R()>> _queue;
		mutable std::mutex _mtx = {}; // Mutex to protect the queue and running bool.
		std::condition_variable _empty_queue_cond;
	};

}}} // rboc::utils::threading

#endif // THREADING_ACTIVEWORKER_HEADER