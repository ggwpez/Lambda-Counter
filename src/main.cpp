///
/// DISCLAIMER
///
/// This code may look very ugly since I am not able to write highly optimized and beautiful code yet.
///
///
/// This approach is still using a recursive algorithm, which makes it comparatively slow.
/// Some example code can be found in the OEIS links.
///
///

#define LRU_MAX 400000

#include "memoize/cache.hpp"
#include "mpz_wrapper.hpp"
#include <iostream>
#include <tuple>

/// Forward decl functions
uint64_t const &var(uint64_t const &n, uint64_t const &k);
mpz_wrapper term(uint64_t const &n, uint64_t const &k, bool L, bool R);
mpz_wrapper lam(uint64_t const &n, uint64_t const &k);
mpz_wrapper app(uint64_t const &n, uint64_t const &k, bool R);

/// Caches for caching the recursive functions
auto cache_term = make_cache(term, uint64_t, uint64_t, bool, bool);
auto cache_lam = make_cache(lam, uint64_t, uint64_t);
auto cache_app = make_cache(app, uint64_t, uint64_t, bool);

/// Calling a functions is now expressed by calling the corresponding cache
#define CALL_TERM(...) cache_term.call(__VA_ARGS__)
#define CALL_LAM(...) cache_lam.call(__VA_ARGS__)
#define CALL_APP(...) cache_app.call(__VA_ARGS__)

/// Just for reference returns
uint64_t const always_zero(0);
mpz_wrapper term(uint64_t const &n, uint64_t const &k, bool L, bool R)
{
	if (n < 2)
		return var(n, k);

	if (R && L)
	{
		// The GCC optimizer is doing a such bad job, one could assume that it not even exists
		uint64_t const nm2(n - 2);

		return CALL_LAM(nm2, k) + CALL_APP(nm2, k, false) + var(n, k);
	}
	else
		return CALL_LAM(R ? (n - 2u) : n, k) + CALL_APP(L ? (n - 2u) : n, k, R && !L) + var(n, k);
}

inline uint64_t const &var(uint64_t const &n, uint64_t const &k)
{
	return (n == 1) ? k : always_zero;
}

mpz_wrapper lam(uint64_t const &n, uint64_t const &k)
{
	if (n < 2)
		return always_zero;

	mpz_wrapper sum(always_zero);
	uint64_t const nm2(n - 2);

	for (uint64_t i = 1; i < nm2; ++i)
	{
		uint64_t const tmp(nm2 - i);
		uint64_t const kpi(i + k);

		sum += var(tmp, kpi);
		sum += CALL_APP(tmp, kpi, false);
	}

	return sum;
}

mpz_wrapper app(uint64_t const &n, uint64_t const &k, bool R)
{
	mpz_wrapper sum(always_zero);

	for (uint64_t i = 1; i < n; ++i)
		sum += (CALL_TERM(i, k, false, true) * CALL_TERM((n - i), k, true, R));

	return sum;
}

void signal_handler(int s)
{
	std::cerr << "Exit after signal " << strsignal(s) << std::endl;
	exit(1);
}


int main(int argc, char **argv)
{
	signal(SIGINT, signal_handler);
	unsigned long start = 0, end = 0;

	if (argc == 2) // if there is only one argument, then its the end
	{
		end = std::stoul(argv[1]);
	}
	else if (argc == 3) // if there are two arguments, its fist start then end
	{
		start = std::stoul(argv[1]);
		end = std::stoul(argv[2]);
	}
	else
	{
		std::cout << "This program calculates the number closed Lambda terms with the given length." << std::endl
				  << "github.com/ggwpez/Lambda-Counter" << std::endl
				  << std::endl
				  << "Usage:" << std::endl
				  << "\t./lambda_counter [start=0] end" << std::endl
				  << "Example for { f(11), ..., f(132) }:" << std::endl
				  << "\t./lambda_counter 11 123" << std::endl;
		return -1;
	}

	for (unsigned n = start; n <= end; ++n)
	{
		mpz_wrapper result = CALL_TERM(n, 0, false, false);
		std::cout << n << ' ' << result << std::endl;
	}

	return 0;
}
