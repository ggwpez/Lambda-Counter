/*
 * SPDX-FileCopyrightText: 2016-2021 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <type_traits>

template <template <typename> typename, typename...> struct for_all;

// For all on empty sets is always true
template <template <typename> typename Pred> struct for_all<Pred>
{
    using type = std::true_type;
    constexpr static bool value = false;
};

template <template <typename> typename Pred, typename X, typename... XS> struct for_all<Pred, X, XS...>
{
    using type = typename std::conditional<std::is_same<typename Pred<X>::type, std::true_type>::value,
                                           typename for_all<Pred, XS...>::type, std::false_type>::type;
    constexpr static bool value = type::value;
};
