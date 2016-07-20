#include "mpz_wrapper.hpp"
#include <memoize/cache.hpp>

#include <iostream>

mpz_wrapper const& var(mpz_wrapper const& n, mpz_wrapper const& k);
mpz_wrapper lam(mpz_wrapper const& n, mpz_wrapper const& k);
mpz_wrapper app(mpz_wrapper const& n, mpz_wrapper const& k, bool R);
mpz_wrapper term(mpz_wrapper const& n, mpz_wrapper const& k, bool L, bool R);

auto cache_term = make_cache(term, mpz_wrapper, mpz_wrapper, bool, bool);
auto cache_lam = make_cache(lam, mpz_wrapper, mpz_wrapper);
auto cache_app = make_cache(app, mpz_wrapper, mpz_wrapper, bool);

#define CALL_TERM(...) cache_term.call(__VA_ARGS__)
#define CALL_LAM(...) cache_lam.call(__VA_ARGS__)
#define CALL_APP(...) cache_app.call(__VA_ARGS__)

mpz_wrapper const always_zero;
mpz_wrapper term(mpz_wrapper const& n, mpz_wrapper const& k, bool L, bool R)
{
    if (R && L)
    {
        mpz_wrapper const nm2(n -2);

        return var(n, k) +CALL_LAM(nm2, k) +CALL_APP(nm2, k, false);
    }
    else
        return var(n, k) +CALL_LAM(R ? n -2u : n, k) +CALL_APP(L ? n -2u : n, k, R && !L);
}

inline mpz_wrapper const& var(mpz_wrapper const& n, mpz_wrapper const& k)
{
    return (n == 1) ? k : always_zero;
}

mpz_wrapper lam(mpz_wrapper const& n, mpz_wrapper const& k)
{
    mpz_wrapper sum;
    mpz_wrapper const tmpp(n -2);

    for (mpz_wrapper i = 1; i < tmpp; ++i)
    {
        mpz_wrapper const tmp(tmpp -i);
        mpz_wrapper const kpi(k +i);

        sum += var(tmp, kpi);
        sum += CALL_APP(tmp, kpi, false);
    }

    return sum;
}

mpz_wrapper app(mpz_wrapper const& n, mpz_wrapper const& k, bool R)
{
    mpz_wrapper sum;

    for (mpz_wrapper i = 1; i < n; ++i)
        sum += (CALL_TERM(i, k, false, true) *CALL_TERM(n -i, k, true, R));

    return sum;
}

int main(int argc, char** argv)
{
    mpz_wrapper start, end;

    if (argc == 2)                  // if there is only one argument, then its the end
    {
        end = std::stoul(argv[1]);
    }
    else if (argc == 3)             // if there are two arguments, its fist start then end
    {
        start = std::stoul(argv[1]);
        end = std::stoul(argv[2]);
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

    for (mpz_wrapper n = start; n <= end; ++n)
    {
        mpz_wrapper result = CALL_TERM(n, 0, false, false);
        std::cout << n << ' ' << result << std::endl;
    }

    return 0;
}
