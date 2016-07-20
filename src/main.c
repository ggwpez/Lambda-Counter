#include <cstdint>
#include <iostream>
#include <gmp.h>

#define I(x) mpz_init_set_ui(x, 0)
#define F(x) mpz_clear(x)
#define MEMO_CONTEXT 0

void var(mpz_t n, mpz_t k, mpz_t res);
void lam(mpz_t n, mpz_t k, mpz_t res);
void app(mpz_t n, mpz_t k, bool R, mpz_t res);
//void term(mpz_t n, mpz_t k, bool L, bool R, mpz_t res);

auto t = make_cache(term, mpz_t, mpz_t, bool, bool);
auto l = make_cache(lam, mpz_t, mpz_t);
auto a = make_cache(app, mpz_t, mpz_t, bool);

#define CALL_TERM(...) t.lookup(__VA_ARGS__)
#define CALL_LAM(...) l.lookup(__VA_ARGS__)
#define CALL_APP(...) a.lookup(__VA_ARGS__)

void var(mpz_t n, mpz_t k, mpz_t res)
{
    /// k if n==1 else 0

    if (! mpz_cmp_ui(n, 1))   // n == 1 ?
        mpz_init_set(res, k);
    else
        mpz_set_ui(res, 0);
}

void lam(mpz_t n, mpz_t k, mpz_t res)
{
    // sum(var(n-v-2,k+v) + app(n-v-2,k+v,0) for v in range(1,n-2))

    mpz_t end, v;
    I(end), I(v);     //init

    mpz_set_ui(v, 1);               // start at 1
    mpz_sub_ui(end, n, 2);                 // end at n -2

    // while locals:
    mpz_t res_var, res_app, arg0, arg1;
    I(res_var), I(res_app), I(arg0), I(arg1);

    while (mpz_cmp(v, end) < 0)       //v < end
    {
        /// var(n-v-2,k+v)
        mpz_sub(arg0, n, v);        // arg0 = n -v
        mpz_sub_ui(arg0, arg0, 2);  // -2
        mpz_add(arg1, k, v);        // arg1 = k +v

        var(arg0, arg1, res_var);

        /// app(n-v-2,k+v,0)
        // reuse arg0 = n -v -2
        // reuse arg1 = k +v
        // arg3 const = false

        app(arg0, arg1, false, res_app);

        /// Sum up
        mpz_add(res, res, res_var); // sum += res_var
        mpz_add(res, res, res_app); // sum += res_app

        mpz_add_ui(v, v, 1);
    }

    F(end), F(v);
    F(res_var), F(res_app), F(arg0), F(arg1);
}

void app(mpz_t n, mpz_t k, bool R, mpz_t res)
{
    // sum(term(u,k,0,1) * term(n-u,k,1,R) for u in range(1,n))

    mpz_t u;
    I(u);

    mpz_set_ui(u, 1);           // start = 1

    // while locals:
    mpz_t res_term_a, res_term_b, arg0, arg1;
    I(res_term_a), I(res_term_b), I(arg0), I(arg1);

    while (mpz_cmp(u, n) < 0)   // u < end
    {
        /// term(u,k,0,1)
        term(u, k, false, true, res_term_a);

        /// term(n-u,k,1,R)
        mpz_sub(arg0, n, u);    // arg0 = n -u

        term(arg0, k, true, R, res_term_b);

        mpz_addmul(res, res_term_a, res_term_b);            // sum up the product

        mpz_add_ui(u, u, 1);
    }

    F(u);
    F(res_term_a), F(res_term_b), F(arg0), F(arg1);
}

MEMOIZED_FUNCT_PTR_RET(MEMO_CONTEXT, term, __mpz_struct, res, mpz_t, n, mpz_t, k, bool, L, bool, R)
//void term(mpz_t n, mpz_t k, bool L, bool R, mpz_t res)
{
    // var(n,k) + lam(n-2 if R else n, k) + app(n-2 if L else n, k, R and not L)
    mpz_t res_var, res_lam, res_app, arg0, arg1;
    I(res_var), I(res_lam), I(res_app), I(arg0), I(arg1);

    /// var(n,k)
    var(n, k, res_var);


    /// lam(n-2 if R else n, k)
    if (R)
        mpz_sub_ui(arg0, n, 2);    // arg0 = n -2
    else
        mpz_set(arg0, n);      // arg0 = n

    mpz_set(arg1, k);          // arg1 = k

    lam(arg0, arg1, res_lam);


    /// app(n-2 if L else n, k, R and not L)
    if (L)
        mpz_sub_ui(arg0, n, 2);    // arg0 = n -2
    else
        mpz_set(arg0, n);      // arg0 = n

    mpz_set(arg1, k);          // arg1 = k

    app(arg0, arg1, R & !L, res_app);

    mpz_add(res, res_var, res_lam);
    mpz_add(res, res, res_app);

    F(res_var), F(res_lam), F(res_app), F(arg0), F(arg1);
}

void run_with_memo(unsigned end)
{


    mpz_t arg0, arg1;
    __mpz_struct t;
    __mpz_struct* res = &t;
    I(arg0), I(arg1), I(res);

    for (unsigned i = 0; i <= end; i++)
    {
        mpz_set_ui(res, 0);
        mpz_set_ui(arg0, i);
        term(arg0, arg1, false, false, res);

        gmp_printf("test(%Zd, %Zd, false, false) = %Zd\n", arg0, arg1, res);
    }

    F(arg0), F(arg1), F(res);
}

int main(int argc, char** argv)
{
    (void)argc, (void)argv;

    run_with_memo(10);
    return 0;
}
