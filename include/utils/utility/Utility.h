#pragma once
#ifndef UTILS_UTILITY_HEADER
#define UTILS_UTILITY_HEADER

namespace rboc { namespace utils { namespace utilities {

	template <std::size_t... Seq>
	struct IntegerSequence{};

	template<std::size_t N, std::size_t... Seq>
	struct Generator : Generator<N - 1, N - 1, Seq...> {};

	template<std::size_t... Seq>
	struct Generator<0, Seq...>
	{
		using type = IntegerSequence<Seq...>;
	};

	namespace details
	{
		template<typename Func, typename Tuple, std::size_t... Seq>
		void call_task_impl(Func&& f, Tuple&& tup, IntegerSequence<Seq...>)
		{
			std::forward<Func>(f)(std::get<Seq>(std::forward<Tuple>(tup))...);
		}
	}	

	template<typename F, typename Tuple>	
	void call_task(F&& f, Tuple&& tup)
	{
		return details::call_task_impl(
			std::forward<F>(f), std::forward<Tuple>(tup), 
			typename Generator<std::tuple_size<typename std::remove_reference<Tuple>::type>::value>::type{});
	}

}}} // namespace rboc::utils
#endif