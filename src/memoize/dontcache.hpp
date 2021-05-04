#pragma once

#include <experimental/type_traits>

/// Will indicate that a function argument is insignificant for the caching.
struct DontCache : std::experimental::nonesuch
{
};

typedef DontCache DontCache_t;
