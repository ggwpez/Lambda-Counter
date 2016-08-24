/*#include "mpz_wrapper.hpp"
#include "multiarray.hpp"

#include <iostream>

template<typename T>
struct Container
{
    Container()
        : valid(false)
    { }

    Container(T&& i)
        : v(i), valid(true)
    { }

    T v;
    bool valid;
};
#define SIZE 26

multiarray<Container<mpz_wrapper>, SIZE, SIZE, 2, 2>::type* term_cache;
multiarray<Container<mpz_wrapper>, SIZE, SIZE>::type* lam_cache;
multiarray<Container<mpz_wrapper>, SIZE, SIZE, 2>::type* app_cache;

int64_t const& var(int64_t const& n, int64_t const& k);
mpz_wrapper lam(int64_t const& n, int64_t const& k);
mpz_wrapper app(int64_t const& n, int64_t const& k, bool R);
mpz_wrapper term(int64_t const& n, int64_t const& k, bool L, bool R);

inline mpz_wrapper const& call_term(int64_t const& n, int64_t const& k, bool L, bool R)
{
    auto& tmp = term_cache->at(n)[k][L][R];

    if (tmp.valid)
        return tmp.v;
    else
        return (tmp = term(n, k, L, R)).v;
}

inline mpz_wrapper const& call_lam(int64_t const& n, int64_t const& k)
{
    auto& tmp = lam_cache->at(n)[k];

    if (tmp.valid)
        return tmp.v;
    else
        return (tmp = lam(n, k)).v;
}

inline mpz_wrapper const& call_app(int64_t const& n, int64_t const& k, bool R)
{
    auto& tmp = app_cache->at(n)[k][R];

    if (tmp.valid)
        return tmp.v;
    else
        return (tmp = app(n, k, R)).v;
}

#define CALL_TERM(...) call_term(__VA_ARGS__)
#define CALL_LAM(...) call_lam(__VA_ARGS__)
#define CALL_APP(...) call_app(__VA_ARGS__)

int64_t const always_zero(0);
mpz_wrapper term(int64_t const& n, int64_t const& k, bool L, bool R)
{
    if (n < 2)
        return var(n, k);

    if (R && L)
    {
        int64_t const nm2(n -2);
        return CALL_LAM(nm2, k) +CALL_APP(nm2, k, false) +var(n, k);
    }
    else
        return CALL_LAM(R ? (n -2u) : n, k) +CALL_APP(L ? (n -2u) : n, k, R && !L) +var(n, k);
}

inline int64_t const& var(int64_t const& n, int64_t const& k)
{
    return (n == 1) ? k : always_zero;
}

mpz_wrapper lam(int64_t const& n, int64_t const& k)
{
    mpz_wrapper sum(0);
    int64_t const tmpp(n -2);

    for (int64_t i = 1; i < tmpp; ++i)
    {
        int64_t const tmp(tmpp -i);
        int64_t const kpi(i +k);

        sum += var(tmp, kpi);
        sum += CALL_APP(tmp, kpi, false);
    }

    return sum;
}

mpz_wrapper app(int64_t const& n, int64_t const& k, bool R)
{
    mpz_wrapper sum(0);

    for (int64_t i = 1; i < n; ++i)
        sum += (CALL_TERM(i, k, false, true) *CALL_TERM((n -i), k, true, R));

    return sum;
}

int main(int argc, char** argv)
{
    term_cache = new std::remove_pointer_t<decltype(term_cache)>();
    app_cache = new std::remove_pointer_t<decltype(app_cache)>();
    lam_cache = new std::remove_pointer_t<decltype(lam_cache)>();

    unsigned start = 0, end = 0;

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

    for (unsigned n = start; n <= SIZE -1; ++n)
    {
        mpz_wrapper result = CALL_TERM(n, 0, false, false);
        std::cout << n << ' ' << result << std::endl;
    }

    delete term_cache;
    delete lam_cache;
    delete app_cache;

    return 0;
}*/
