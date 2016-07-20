#ifndef MPZ_CLASS_HPP
#define MPZ_CLASS_HPP

#include <gmp.h>
#include <algorithm>
#include <cstring>
#include <csignal>
#include <ostream>

/// 2.3% faster than GNU mpz_class, and the copy assign wont SIGSEGV!
class mpz_wrapper
{
public:
    mpz_wrapper()
        : mpz_wrapper(0)
    { }

    mpz_wrapper(unsigned n)
        : should_free(true)
    {
        mpz_init_set_ui(v, n);
    }

    mpz_wrapper(mpz_wrapper const& obj)     // copy ctor
        : should_free(true)
    {
        mpz_init_set(v, obj.get_value());
    }

    mpz_wrapper(mpz_wrapper&& obj)          // move ctor
        : should_free(true)
    {
#ifdef DEBUG
        if (std::addressof(obj) == this)
            std::raise(SIGSEGV);
#endif

        std::memcpy(this, std::addressof(obj), sizeof(mpz_wrapper));
        obj.lost_ptr_ownership();
    }

    inline mpz_wrapper& operator =(mpz_wrapper const& obj)  // copy assign
    {
        mpz_set(this->v, obj.get_value());

        return *this;
    }

    inline mpz_wrapper& operator =(mpz_wrapper&& obj)       // move assign
    {
#ifdef DEBUG
        if (std::addressof(obj) == this)
            std::raise(SIGSEGV);
#endif

        std::memcpy(this, std::addressof(obj), sizeof(mpz_wrapper));
        obj.lost_ptr_ownership();

        return *this;
    }

    inline bool operator ==(mpz_wrapper const& obj) const
    {
        return (mpz_cmp(v, obj.get_value()) == 0);
    }

    inline bool operator ==(unsigned const& num) const
    {
        return (mpz_cmp_ui(v, num) == 0);
    }

    inline bool operator <(mpz_wrapper const& obj) const
    {
        return (mpz_cmp(v, obj.get_value()) < 0);
    }

    inline bool operator <(unsigned const& num) const
    {
        return (mpz_cmp_ui(v, num) < 0);
    }

    inline bool operator <=(mpz_wrapper const& obj) const
    {
        return (*this < obj) || (*this == obj);
    }

    inline bool operator >(mpz_wrapper const& obj) const
    {
        return (mpz_cmp(v, obj.get_value()) > 0);
    }

    inline bool operator >(unsigned const& num) const
    {
        return (mpz_cmp_ui(v, num) > 0);
    }

    inline mpz_wrapper operator +(mpz_wrapper const& obj) const
    {
        mpz_wrapper tmp(*this);
        tmp += obj;

        return tmp;
    }

    inline mpz_wrapper operator +(unsigned const& obj) const
    {
        mpz_wrapper tmp(*this);
        tmp += obj;

        return tmp;
    }

    inline mpz_wrapper& operator +=(mpz_wrapper const& obj)
    {
        mpz_add(v, v, obj.get_value());

        return *this;
    }

    inline mpz_wrapper& operator +=(unsigned const& obj)
    {
        mpz_add_ui(v, v, obj);

        return *this;
    }

    inline mpz_wrapper operator -(mpz_wrapper const& obj) const
    {
        mpz_wrapper tmp(*this);
        tmp -= obj;

        return tmp;
    }

    inline mpz_wrapper operator -(unsigned const& obj) const
    {
        mpz_wrapper tmp(*this);
        tmp -= obj;

        return tmp;
    }

    inline mpz_wrapper& operator -=(mpz_wrapper const& obj)
    {
        mpz_sub(v, v, obj.get_value());

        return *this;
    }

    inline mpz_wrapper& operator -=(unsigned const& obj)
    {
        mpz_sub_ui(v, v, obj);

        return *this;
    }

    inline mpz_wrapper operator *(mpz_wrapper const& obj) const
    {
        mpz_wrapper tmp(*this);
        tmp *= obj;

        return tmp;
    }

    inline mpz_wrapper& operator *=(mpz_wrapper const& obj)
    {
        mpz_mul(v, v, obj.get_value());

        return *this;
    }

    inline mpz_wrapper& operator ++()
    {
        mpz_add_ui(v, v, 1);

        return *this;
    }

    inline mpz_wrapper& operator --()
    {
        mpz_sub_ui(v, v, 1);

        return *this;
    }

    inline mpz_wrapper operator ++(int) const    // (int) - bjarne pls
    {
        mpz_wrapper tmp(*this);
        ++tmp;

        return tmp;
    }

    inline mpz_wrapper operator --(int) const
    {
        mpz_wrapper tmp(*this);
        --tmp;

        return tmp;
    }

    friend std::ostream& operator <<(std::ostream& stream, mpz_wrapper const& obj);

    inline unsigned get_ui() const
    {
        return mpz_get_ui(v);
    }

    ~mpz_wrapper()
    {
        if (should_free)
        {
            mpz_clear(v);
            should_free = 0;
        }
    }

    inline void lost_ptr_ownership()
    {
        this->should_free = false;
    }

    inline mpz_t const& get_value() const
    {
        return v;
    }

protected:
    mpz_t v;
    bool should_free;
};

std::ostream& operator <<(std::ostream& stream, mpz_wrapper const& obj)
{
    char* str = nullptr;

    if (gmp_asprintf(&str, "%Zd", obj.get_value()) == -1)
        std::raise(SIGSEGV);                                // = throw std::bad_alloc();

    stream << str;
    std::free(str);

    return stream;
}

namespace std
{
    template<>
    struct hash<mpz_wrapper>
    {
        inline std::size_t operator()(mpz_wrapper const& obj) const
        {
            return obj.get_ui();
        }
    };
}

#endif // MPZ_CLASS_HPP
