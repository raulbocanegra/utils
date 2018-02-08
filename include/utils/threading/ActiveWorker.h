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
	/*!
	  This is a worker class that has a detached thread that extract 
	  functions and arguments from a queue and executes them.
	*/
	template<typename R, typename... Args>
	class ActiveWorker
	{
		public:

		//! Default constructor
		ActiveWorker()
			: _running(true)
			, _queue{}
		{
			std::thread worker_thread(&ActiveWorker::work, this);
			worker_thread.detach();
		}

		//! Copy constructor
		ActiveWorker(const ActiveWorker& other) = default;
		//! Move constructor
		ActiveWorker(ActiveWorker&& other) = default;
		//! Destructor
		~ActiveWorker()
		{
			stop();
		}
		//! Copy assignment
		ActiveWorker& operator=(const ActiveWorker& other) = default;
		//! Move assignment
		ActiveWorker& operator=(ActiveWorker&& other) = default;
		
		/*! 
		  Adds work to the worker.
		  \param f the function to be executed by the worker
		  \param Args... the arguments to be passed to the function f.
		*/
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
		
		//! Stops the worker.
		void stop()
		{
			_running = false;
			_empty_queue_cond.notify_one();			
		}

		//! Tells if the worker is running, this means, executing a function or waiting for work.
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

	//! Specialization for 0 argument functions.
	template<typename R>
	class ActiveWorker<R>
	{
		public:

		//! Default constructor
		ActiveWorker()
			: _running(true)
			, _queue{}
		{
			std::thread worker_thread(&ActiveWorker::work, this);
			worker_thread.detach();
		}

		//! Copy constructor
		ActiveWorker(const ActiveWorker& other) = delete;
		
		//! Move constructor
		ActiveWorker(ActiveWorker&& other) = default;
		
		//! Destructor
		~ActiveWorker()
		{
			stop();
		}
		
		//! Copy assignment
		ActiveWorker& operator=(const ActiveWorker& other) = delete;

		//! Move assignment
		ActiveWorker& operator=(ActiveWorker&& other) = default;
		
		//! addWork.
		/*! 
		  Adds work to the worker.
		  \param f the function to be executed by the worker		
		*/
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
		
		//! stop.
		/*!
		  Stops the worker.
		*/
		void stop()
		{
			_running = false;
			_empty_queue_cond.notify_one();			
		}

		//! isRunning.
		/*!
		  Tells if the worker is running, this means, executing a function or waiting for work.
		*/
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