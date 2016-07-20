#ifndef CACHE_HPP
#define CACHE_HPP

#include "function_traits.hpp"
#include "tuple_hash.hpp"
#include "dontcache.hpp"
#include "template_filter.hpp"
#include "template_for_all.hpp"
#include "template_predicates.hpp"
#include "template_conditional2.hpp"

#include <type_traits>
#include <experimental/type_traits>
#include <map>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <functional>
#include <algorithm>

/// How old values should timeout.
/// Actually is only NEVER supported,
enum class timeout_policy_t
{
    NEVER,
    LRU
};

/// This cache is able to have different function and cache types.
/// If you e.g. have a function fib(int* n) which calculates something for *n,
/// then you would like to cache the function for *n, and not n itself.
///
///     make_cache(fib, int);
///
/// would do this.
///
/// fptr_t: Function type
/// Keys: Types of the Keys
template<timeout_policy_t tp, class fptr_t, class... Keys>
struct Cache
{
    static_assert(std::is_function<fptr_t>::value, "Pass a function pointer type as fptr_t template argument.");

    using fptr_trait = function_traits<fptr_t>;
    typedef typename fptr_trait::return_type R;

    std::function<fptr_t> const fptr;             // i could also use fptr_t*, but is that even legal?

    constexpr static const bool all_hashable = for_all<is_hashable, Keys...>::value;
    constexpr static const bool all_compareable = for_all<is_compareable, Keys...>::value;
    constexpr static const bool all_less_compareable = for_all<is_less_compareable, Keys...>::value;

    constexpr static const bool use_unordered = (all_hashable && all_compareable);
    constexpr static const bool use_ordered = all_less_compareable;

    static_assert(use_unordered || use_ordered, "Either all passed types must be (Hashable and Compareable) or all must be LessThanCompareable.");
    static_assert((tp == timeout_policy_t::NEVER) ^ (tp == timeout_policy_t::LRU), "Invalid enum value for timeout_policy_t tp.");

    typedef std::tuple<Keys...> key_t;
    typedef std::tuple<std::decay_t<Keys>...> d_key_t;

    typedef std::conditional_t<use_unordered,
                                std::unordered_map<key_t, R>,
                                std::map<key_t, R>> cache_map_t;

    typedef std::size_t internal_cache_it_t;
    typedef std::vector<R> internal_cache_t;

    typedef std::conditional_t<use_unordered,
                                std::unordered_map<key_t, internal_cache_it_t>,
                                std::map<key_t, internal_cache_it_t>> cache_vector_t;

    typedef std::conditional23_t<tp == timeout_policy_t::NEVER,
                                 tp == timeout_policy_t::LRU,
                                 cache_map_t,
                                 cache_vector_t,
                                 void> cache_t; // void means error, already catched througth static_assert

    //internal_cache_t internal_cache;
    //internal_cache_it_t internal_it;

    cache_t cache;
    //std::size_t cache_it;

    Cache(fptr_t arg)
        : fptr(arg),
          //internal_cache(50000),  // <- this is massive overhead for small calculations
          //internal_it(0),
          cache()
    { }

    template<class T>
    inline constexpr std::remove_const_t<T> const& getKey(std::remove_const_t<T>& key)
    {
        return key;
    }

    template<class T>
    inline constexpr T const& getKey(T const& key)
    {
        return key;
    }

    template<class T>
    inline constexpr T const& getKey(T* const& key)
    {
        return *key;
    }

    /// This overload is for the cache_map_t
    template<class... Args>
    typename std::enable_if<(sizeof...(Args), std::is_same<cache_t, cache_map_t>::value), std::remove_const_t<std::decay_t<R>>>::type
    const& call(Args const&... args)
    {
        //std::tuple<typename filter<is_dc, Args...>::type> filtered = std::tuple<typename filter<is_dc, Args...>::type>();
        d_key_t new_keys(getKey(args)...);
        auto&& it(cache.find(new_keys));

        if (it == cache.end())      //not cached
        {
            auto&& pos(cache.insert(std::pair<key_t, R>(std::move(new_keys), fptr(args...))));

            return pos.first->second;
        }
        else                        //was cached
            return it->second;
    };

    /// This overload is for the cache_vector_t
    /*template<class... Args>
    typename std::enable_if<(sizeof...(Args), std::is_same<cache_t, cache_vector_t>::value), std::remove_const_t<std::decay_t<R>>>::type
    const& call(Args const&... args)
    {
        d_key_t new_keys(getKey(args)...);

        auto&& it(cache.find(new_keys));

        if (it == cache.end())      // not cached
        {
            if (internal_it == cache.size())    // reached end, time to overwrite something
                internal_it = 0;

           // if (cache.size() >= 50000)


            internal_cache[internal_it] = fptr(args...);

            return internal_cache[internal_it++];
        }
        else                        // was cached
            return internal_cache[it->second];
    };*/

    inline void clear()
    {
        cache.clear();
    }
};

#define make_cache(fptr, ...) (Cache<timeout_policy_t::NEVER, decltype(fptr), __VA_ARGS__>(fptr))
#define get_cache_type(fptr, ...) Cache<timeout_policy_t::NEVER, decltype(fptr), __VA_ARGS__>

#endif // CACHE_HPP
