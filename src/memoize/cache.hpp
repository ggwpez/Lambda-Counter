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
#include <list>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <functional>
#include <algorithm>
#include <cassert>

/// How old values should timeout.
/// Actually NEVER is the only one supported.
enum class timeout_policy_t
{
	NEVER,
	LRU
};

#define LRU_MAX 50000

/// This cache is able to have different function and cache types.
/// If you e.g. have a function fib(int* n) which calculates something for *n,
/// then you would like to cache the function for *n, and not n itself.
///
///     make_cache(fib, int);
///
/// would do this.
/// It dereferences the second argument and saves it as key.
///
///
/// fptr_t: Function type
/// Keys:   Types of the Keys
///
template<timeout_policy_t tp, class fptr_t, class... Keys>
struct Cache
{
	static_assert(std::is_function<fptr_t>::value, "Pass a function pointer type as fptr_t template argument.");

	using fptr_trait = function_traits<fptr_t>;
	typedef typename fptr_trait::return_type R;

	std::function<fptr_t> const fptr;             // I could also use fptr_t*, but is that even legal?

	constexpr static const bool all_hashable = for_all<is_hashable, Keys...>::value;
	constexpr static const bool all_compareable = for_all<is_compareable, Keys...>::value;
	constexpr static const bool all_less_compareable = for_all<is_less_compareable, Keys...>::value;

	constexpr static const bool use_unordered = (all_hashable && all_compareable);
	constexpr static const bool use_ordered = all_less_compareable;

	static_assert(use_unordered || use_ordered, "Either all passed types must be (Hashable and Compareable) or all must be LessThanCompareable.");
	static_assert((tp == timeout_policy_t::NEVER) ^ (tp == timeout_policy_t::LRU), "Invalid enum value for timeout_policy_t tp.");

	typedef std::tuple<Keys...> key_t;
	typedef std::tuple<std::decay_t<Keys>...> decayed_key_t;

	typedef std::conditional_t<use_unordered,
								std::unordered_map<key_t, R>,
								std::map<key_t, R>> cache_map_t;

	typedef std::list<R> internal_cache_t;                              //use list for constant insert operations, traversion done in const time with iterators
	typedef typename internal_cache_t::iterator internal_cache_it_t;

	typedef std::conditional_t<use_unordered,
								std::unordered_map<key_t, internal_cache_it_t>,
								std::map<key_t, internal_cache_it_t>> cache_vector_t;

	typedef std::conditional23_t<tp == timeout_policy_t::NEVER,
								 tp == timeout_policy_t::LRU,
								 cache_map_t,
								 cache_vector_t,
								 void> cache_t; // void means error, already caught through static_assert

	typedef typename cache_t::iterator cache_it_t;

	// Internal cache, only used in LRU mode
	internal_cache_t internal_cache;
	internal_cache_it_t internal_it;

	// Always used, in LRU it uses the internal_cache
	cache_t cache;
	cache_it_t cache_it;

	Cache(fptr_t arg)
		: fptr(arg),
		  internal_cache(),
		  internal_it(internal_cache.begin()),
		  cache(),
		  cache_it(cache.begin())
	{ }

	// const& -> &
	template<class T>
	inline constexpr std::remove_const_t<std::remove_reference_t<T>> const& getKey(std::remove_reference_t<T>& key)
	{
		return key;
	}

	// const& -> const&
	template<class T>
	inline constexpr T const& getKey(T const& key)
	{
		return key;
	}

	// * const& -> const&
	template<class T>
	inline constexpr T const& getKey(T* const& key)
	{
		return *key;
	}

	/// This overload is for the cache_map_t
	template<class... Args>
	typename std::enable_if<(sizeof...(Args), std::is_same<cache_t, cache_map_t>::value) && tp == timeout_policy_t::NEVER, std::remove_const_t<std::decay_t<R>>>::type
	const& call(Args const&... args)
	{
		decayed_key_t new_keys(getKey(args)...);
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
	template<class... Args>
	typename std::enable_if<(sizeof...(Args), std::is_same<cache_t, cache_vector_t>::value) && tp == timeout_policy_t::NEVER, std::remove_const_t<std::decay_t<R>>>::type
	const& call(Args const&... args)
	{
		decayed_key_t new_keys(getKey(args)...);
		auto&& it(cache.find(new_keys));

		if (it == cache.end())      // not cached
		{
			if (cache.size() == LRU_MAX)        // cache is full, time to overwrite old objects
			{
				if (internal_it == internal_cache.end())
				{
					assert(cache_it == cache.end());
					internal_it = internal_cache.begin();
					cache_it = cache.begin();
				}

				*internal_it = fptr(args...);
				cache.erase(cache_it);
				cache.insert(cache_it, std::pair<key_t, internal_cache_it_t>(new_keys, internal_it));
				++cache_it;
				return *internal_it++;
			}
			else
			{
				// Maybe emplace_back
				internal_cache.push_back(fptr(args...));
				cache[new_keys] = internal_cache.end();
				return internal_cache.back();
			}
		}
		else                    // was cached
			return *it->second;
	};

	inline std::size_t size() const
	{
		return cache.size();
	}

	inline void clear()
	{
		cache.clear();
	}
};

#define make_cache(fptr, ...) (Cache<timeout_policy_t::NEVER, decltype(fptr), __VA_ARGS__>(fptr))
#define get_cache_type(fptr, ...) Cache<timeout_policy_t::NEVER, decltype(fptr), __VA_ARGS__>

#endif // CACHE_HPP
