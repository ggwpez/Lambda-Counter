#include "counter.hpp"
#include <iostream>

counter::counter(std::size_t max_rec_depth, std::size_t stack_size)
	: state(state_t::NONE), rec_depth(0), max_rec_depth(max_rec_depth),
	  next_states(stack_size), term_args(stack_size), lam_args(stack_size), app_args(stack_size), return_values(stack_size), return_states(stack_size)
{

}

mpz_wrapper counter::calc(std::size_t n)
{
	reset();
	next_states.push(state_t::INIT);

	while (rec_depth++ < max_rec_depth)
	{
		state = next_states.toppop();
		state_t	base_state = (state_t)((uint32_t)state &state_sub_mask),
				sub_state = (state_t)((uint32_t)state &~state_sub_mask);

		switch (base_state)
		{
			case state_t::CALL_CONTINUE:
			{
				continue;
			} break;
			case state_t::INIT:
			{
				place_call(state_t::TERM, state_t::DONE, term_args_t{n,0,false,false});
			} break;
			case state_t::DONE:
			{
				mpz_wrapper&& ret = std::move(return_values.toppop());
				assert_stack_empty();

				return std::move(ret);
			} break;
			case state_t::TERM:
			{
				uint64_t const& n = std::get<0>(term_args.top());
				uint64_t const& k = std::get<1>(term_args.top());
				bool L			  = std::get<2>(term_args.top());
				bool R		      = std::get<3>(term_args.top());
				uint64_t const nm2(n -2);

				if (sub_state == state_t::SUB0)
				{
					if (n < 2)
					{
						place_return_from_term(var(n,k));
						break;
					}

					if (R && L)
					{
						place_call(state_t::LAM, state_or(state_t::TERM, state_t::SUB1), lam_args_t{nm2, k,mpz_wrapper(),1});
						place_call(state_t::APP, state_t::CALL_CONTINUE, app_args_t{nm2, k, false,mpz_wrapper(),1});
					}
					else
					{
						place_call(state_t::LAM, state_or(state_t::TERM, state_t::SUB1), lam_args_t{R ? (n -2u) : n, k,mpz_wrapper(),1});
						place_call(state_t::APP, state_t::CALL_CONTINUE, app_args_t{L ? (n -2u) : n, k, R && !L,mpz_wrapper(),1});
					}
				}
				else if (sub_state == state_t::SUB1)
				{
					mpz_wrapper&& x = return_values.toppop();
					mpz_wrapper&& y = return_values.toppop();

					x += y;
					x += var(n, k);

					place_return_from_term(std::move(x));
				}
				else
					__builtin_unreachable();
			} break;
			case state_t::LAM:
			{
				uint64_t const& n = std::get<0>(lam_args.top());
				uint64_t const& k = std::get<1>(lam_args.top());
				mpz_wrapper& sum  = std::get<2>(lam_args.top());
				uint64_t& i		  = std::get<3>(lam_args.top());

				if (sub_state == state_t::SUB0)
				{
					if (n < 2 || i +2 >= n)
					{
						place_return_from_lam(0);
						break;
					}

					sum = 0;
					next_states.push(state_or(state_t::LAM, state_t::SUB1));
				}
				else if (sub_state == state_t::SUB1)	// Iter
				{
					if (i +2 >= n)
					{
						place_return_from_lam(std::move(sum));
						break;
					}

					uint64_t const tmp(n -2 -i);
					uint64_t const kpi(i +k);

					sum += var(tmp, kpi);
					place_call(state_t::APP, state_or(state_t::LAM, state_t::SUB2), app_args_t{tmp,kpi,false,mpz_wrapper(),1});
				}
				else if (sub_state == state_t::SUB2)
				{
					sum += return_values.toppop();
					++i;
					next_states.push(state_or(state_t::LAM, state_t::SUB1));
				}
				else
					__builtin_unreachable();
			} break;
			case state_t::APP:
			{
				uint64_t const& n = std::get<0>(app_args.top());
				uint64_t const& k = std::get<1>(app_args.top());
				bool R			  = std::get<2>(app_args.top());
				mpz_wrapper& sum  = std::get<3>(app_args.top());
				uint64_t& i		  = std::get<4>(app_args.top());

				if (sub_state == state_t::SUB0)
				{
					if (i >= n)
					{
						place_return_from_app(0);
						break;
					}

					sum = 0;	// Avoid unnecessary mpz_wrapper mallocs at all costs!
					next_states.push(state_or(state_t::APP, state_t::SUB1));
				}
				else if (sub_state == state_t::SUB1)	// Iter
				{
					if (i >= n)
					{
						place_return_from_app(std::move(sum));
						break;
					}

					place_call(state_t::TERM, state_or(state_t::APP, state_t::SUB2), term_args_t{i,k,false,true});
					place_call(state_t::TERM, state_t::CALL_CONTINUE, term_args_t{n -i, k, true, R});
				}
				else if (sub_state == state_t::SUB2)
				{
					mpz_wrapper&& x = return_values.toppop();
					mpz_wrapper&& y = return_values.toppop();

					y *= x;
					sum += y;
					++i;
					next_states.push(state_or(state_t::APP, state_t::SUB1));
				}
				else
					__builtin_unreachable();
			} break;
			case state_t::VAR:
			case state_t::NONE:
			default:
			{
				__builtin_unreachable();
			} break;
		}
	}

	if (rec_depth >= max_rec_depth)
	{
		std::cout << "Iteration limit reached (" << max_rec_depth << ')' << std::endl
				  << "Machine in state:	" << (int)state << std::endl
				  << "Return value stack size: " << return_values.size() << std::endl
				  << "Return state stack size: " << return_states.size() << std::endl;
	}

	return 0;
}

void counter::reset()
{
	next_states.reset();
	term_args.reset();

	lam_args.reset();
	app_args.reset();

	return_values.reset();
	return_states.reset();
}

void counter::assert_stack_empty()
{
	if (DEBUG)
	if (next_states.size() || term_args.size() || lam_args.size() || app_args.size() || return_values.size() || return_states.size())
			die("Stack not empty");
}
