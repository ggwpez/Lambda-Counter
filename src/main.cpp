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

//#include "counter.hpp"
#include <iostream>
#include "mpz_wrapper.hpp"

#define LRU_SIZE 50000

/// Forward decl, for the functions needed my the algorith
uint64_t const& var(uint64_t const& n, uint64_t const& k);
mpz_wrapper term(uint64_t const& n, uint64_t const& k, bool L, bool R);
mpz_wrapper lam(uint64_t const& n, uint64_t const& k, mpz_wrapper sum, uint64_t i);
mpz_wrapper app(uint64_t const& n, uint64_t const& k, bool R, mpz_wrapper sum, uint64_t i);

#define USE_CACHE 1
#define MY_CACHE 0

#if USE_CACHE
	#if MY_CACHE
		#include "memoize/cache.hpp"

		/// Caches for caching the recursive functions
		auto cache_term = make_cache(term, uint64_t, uint64_t, bool, bool);
		auto cache_lam = make_cache(lam, uint64_t, uint64_t);
		auto cache_app = make_cache(app, uint64_t, uint64_t, bool);

		/// Calling a functions is now expressed by calling the corresponding cache
		#define CALL_TERM(...) cache_term.call(__VA_ARGS__)
		#define CALL_LAM(...) cache_lam.call(__VA_ARGS__)
		#define CALL_APP(...) cache_app.call(__VA_ARGS__)
	#else
		#include "memoize/lru_cache.hpp"

		// Certanly we dont want to use references here, otherwise enjoy the SIGSEGV
		lru11::Cache<std::tuple<uint64_t, uint64_t, bool, bool>, mpz_wrapper> lru_cache_term(LRU_SIZE,0);
		lru11::Cache<std::tuple<uint64_t, uint64_t>, mpz_wrapper> lru_cache_lam(LRU_SIZE,0);
		lru11::Cache<std::tuple<uint64_t, uint64_t, bool>, mpz_wrapper> lru_cache_app(LRU_SIZE,0);

		#define CALL_TERM(...) lru_cache_term.call(term, std::make_tuple(__VA_ARGS__))
		#define CALL_LAM(...) lru_cache_lam.call(lam, std::make_tuple(__VA_ARGS__))
		#define CALL_APP(...) lru_cache_app.call(app, std::make_tuple(__VA_ARGS__))
	#endif
#else
	#define CALL_TERM(...) term(__VA_ARGS__)
	#define CALL_LAM(...) lam(__VA_ARGS__, 0, 1)
	#define CALL_APP(...) app(__VA_ARGS__, 0, 0)
#endif

/// Just for reference returns
uint64_t const always_zero(0);
mpz_wrapper term(uint64_t const& n, uint64_t const& k, bool L, bool R)
{
	if (n < 2)
		return var(n, k);

	if (R && L)
	{
		// The GCC optimizer is doing a such bad job, one could assume that it not even exists
		uint64_t const nm2(n -2);

		return CALL_LAM(nm2, k) +CALL_APP(nm2, k, false) +var(n, k);
	}
	else
		return CALL_LAM(R ? (n -2u) : n, k) +CALL_APP(L ? (n -2u) : n, k, R && !L) +var(n, k);
}

inline uint64_t const& var(uint64_t const& n, uint64_t const& k)
{
	return (n == 1) ? k : always_zero;
}

mpz_wrapper lam(uint64_t const& n, uint64_t const& k, mpz_wrapper sum, uint64_t i)
{
	if (n < 2)
		return 0;
	if (i +2 >= n)
		return sum;

	uint64_t const tmp(n -2 -i);
	uint64_t const kpi(i +k);

	sum += var(tmp, kpi);
	sum += CALL_APP(tmp, kpi, false);

	return lam(n, k, sum, i +1);
}

mpz_wrapper app(uint64_t const& n, uint64_t const& k, bool R, mpz_wrapper sum, uint64_t i)
{
	if (i == n)
		return sum;

	sum += CALL_TERM(i, k, false, true) *CALL_TERM((n -i), k, true, R);
	return app(n, k, R, sum, i +1);
}

int main(int argc, char** argv)
{
	long start = 0, end = 0;

	if (argc == 2)                  // if there is only one argument, then its the end
	{
		end = std::stoul(argv[1]);
	}
	else if (argc == 3)             // if there are two arguments, its fist start then end
	{
		start = std::stol(argv[1]);
		end = std::stol(argv[2]);
	}
	else
	{
		std::cout << "Usage:" << std::endl
					<< "\t./lambda_counter [start=0] end" << std::endl
				  << "Usage for { f(0), ... ,f(end) }:" << std::endl
					<< "\t./lambda_counter end" << std::endl
				  << "Example for { f(11), ... ,f(132) }:" << std::endl
					  << "\t./lambda_counter 11 123" << std::endl;
		return -1;
	}

	counter c(-1, 30000);
	for (long n = start; n <= end; ++n)
	{
		mpz_wrapper result = CALL_TERM(n, 0, false, false);
		mpz_wrapper result = c.calc(n);
		std::cout << n << ' ' << result << std::endl;
	}

	return 0;
}
