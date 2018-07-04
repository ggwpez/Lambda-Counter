#pragma once

#include "die.hpp"
#include "mpz_wrapper.hpp"
#include "static_stack.hpp"
#include <tuple>

class counter
{
public:
	counter(std::size_t max_rec_depth, std::size_t stack_size);
	mpz_wrapper calc(std::size_t n);
	mpz_wrapper tick();
	void reset();
	void assert_stack_empty();

private:
	typedef std::tuple<uint64_t, uint64_t, bool, bool> term_args_t;
	typedef std::tuple<uint64_t, uint64_t, mpz_wrapper, uint64_t> lam_args_t;
	typedef std::tuple<uint64_t, uint64_t, bool, mpz_wrapper, uint64_t> app_args_t;

	constexpr static uint64_t state_sub_bits = 16;
	constexpr static uint64_t state_sub_mask = (1ull << state_sub_bits) -1;
	constexpr static uint64_t state_trace_bits = 32;
	constexpr static uint64_t state_trace_mask = (1ull << state_trace_bits) -1;
	enum class state_t : uint64_t
	{
		NONE = 0,

		INIT = 1,
		TERM = 2,
		LAM = 3,
		APP = 4,
		VAR = 5,
		CALL_CONTINUE = 6,
		DONE = 7,

		SUB0 = 0 << state_sub_bits,
		SUB1 = 1 << state_sub_bits,
		SUB2 = 2 << state_sub_bits,
		SUB3 = 4 << state_sub_bits,
		SUB4 = 8 << state_sub_bits,

		TRACE0 = 0ull << state_trace_bits,
		TRACE1 = 1ull << state_trace_bits,
		TRACE2 = 4ull << state_trace_bits,
		TRACE3 = 8ull << state_trace_bits,
		TRACE4 = 16ull << state_trace_bits
	};

	template<typename... Args>
	constexpr state_t state_or(Args... args)
	{
		return (state_t)(... | (std::underlying_type_t<state_t>)args);
	}

	template<typename T>
	inline void place_call(state_t target, state_t return_to, T&& args)
	{
		next_states.push(target);
		return_states.push(return_to);

		if constexpr (std::is_same_v<T, term_args_t>)
			term_args.push(std::move(args));
		else if constexpr (std::is_same_v<T, lam_args_t>)
			lam_args.push(std::move(args));
		else if constexpr (std::is_same_v<T, app_args_t>)
			app_args.push(std::move(args));
		else
			throw 1;
	}

	// Dont call on your own!
	inline void place_return_base(mpz_wrapper&& arg)
	{
		next_states.push(return_states.toppop());
		return_values.push(std::move(arg));
	}
	inline void place_return_from_term(mpz_wrapper&& arg)
	{
		place_return_base(std::move(arg));
		term_args.pop();
	}
	inline void place_return_from_lam(mpz_wrapper&& arg)
	{
		place_return_base(std::move(arg));
		lam_args.pop();
	}
	inline void place_return_from_app(mpz_wrapper&& arg)
	{
		place_return_base(std::move(arg));
		app_args.pop();
	}

	constexpr uint64_t var(uint64_t const& n, uint64_t const& k)
	{
		return (n == 1) ? k : 0;
	}

private:
	state_t state;
	std::size_t rec_depth, max_rec_depth;
	static_stack<state_t> next_states;

	static_stack<term_args_t> term_args;
	static_stack<lam_args_t> lam_args;
	static_stack<app_args_t> app_args;

	static_stack<mpz_wrapper> return_values;
	static_stack<state_t> return_states;
};
