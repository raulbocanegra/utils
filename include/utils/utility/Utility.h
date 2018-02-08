#pragma once
#ifndef UTILS_UTILITY_HEADER
#define UTILS_UTILITY_HEADER

namespace rboc { namespace utils { namespace utilities {

	//! A template representing an index sequence. Similar to std::index_sequence from C++14
	template <std::size_t... Seq>
	struct IndexSequence{};

	//! A helper template to generate an index sequence.
	template<std::size_t N, std::size_t... Seq>
	struct GenerateSequence : GenerateSequence<N - 1, N - 1, Seq...> {};

	//! A helper template to generate an index sequence. (Base case)
	template<std::size_t... Seq>
	struct GenerateSequence<0, Seq...>
	{
		using type = IndexSequence<Seq...>;
	};

	namespace details
	{
		//! Template function that implements the call to f with the arguments expanded.
		template<typename Func, typename Tuple, std::size_t... Seq>
		void call_task_impl(Func&& f, Tuple&& tup, IndexSequence<Seq...>)
		{
			std::forward<Func>(f)(std::get<Seq>(std::forward<Tuple>(tup))...);
		}
	}	

	//! call_task is a template function to call the function F with the arguments passed as a Tuple.
	/*! 
	 * Is useful to be used inside our ActiveWorker class
	 * \param f, the function to be called 
	 * \param tup, the tuple that stores the argumenst of the call to f.
	 * \see ActiveWorker
 	 */
	template<typename F, typename Tuple>	
	void call_task(F&& f, Tuple&& tup)
	{
		return details::call_task_impl(
			std::forward<F>(f), std::forward<Tuple>(tup), 
			typename GenerateSequence<std::tuple_size<typename std::remove_reference<Tuple>::type>::value>::type{});
	}

}}} // namespace rboc::utils
#endif