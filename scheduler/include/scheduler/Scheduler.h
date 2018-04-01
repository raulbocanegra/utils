#pragma once
#ifndef UTILS_SCHEDULER_HEADER
#define UTILS_SCHEDULER_HEADER

#include <string>
#include <map>
#include <memory>
#include <thread>
#include <atomic>

namespace rboc { namespace utils { namespace scheduler {

	//! Enum to specify the type of the scheduler.
	enum class scheduler_type
	{
		unknown,
		periodic,
		cron
	};
	
	// Forward declaration
	//! SchedulerData
	/**
	 * Scheduler data to be passed to the system callback functions.
	 */
	struct SchedulerData;
	// Alias for platform dependent data.
	using DataDeleter = void(*)(SchedulerData*);
	using DataPtr = std::unique_ptr<SchedulerData, DataDeleter>;

	//! class Scheduler. 
	/** 
	* A class with the ability to execute periodic tasks ans scheduled tasks similar to cron.
	*/
	class Scheduler
	{
		public:
		
		void start();
		void stop();
		
		//! addPeriodicTask. 
		/** 
		 * \param name A name that the user want to give to the task.
		 * \param period The period of the task to be executed.
		 * \param callback The callback to be executed when the period of time expires.
		 * \param error Output parameter to specify the error, if any.
		 */
		void addPeriodicTask(const std::string& name, time_t period, const std::function<void(void)>& callback, int& error);
	    
		//! addCronTask. 
		/** 
		* \param name A name that the user want to give to the task.
		* \param cron A cron like string to specify when to execute the task.
		* \param callback The callback to be executed when needed.
		* \param error Output parameter to specify the error, if any.
		*/
		void addCronTask(const std::string& name, const std::string& cron, const std::function<void(void)>& callback, int& error);

		private:

		std::map<std::string, DataPtr> _data_map;
		std::thread _runner;
		std::atomic_bool _running;
	};
	
}}} // namespace rboc::utils
#endif