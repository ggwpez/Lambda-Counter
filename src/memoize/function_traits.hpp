/*
 * SPDX-FileCopyrightText: 2016-2021 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <cstdint>
#include <tuple>
#include <type_traits>

template <class F> struct function_traits;

template <class R, class... Args> struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)>
{
};

template <class R, class... Args> struct function_traits<R(Args...)>
{
    static constexpr std::size_t arg_count = sizeof...(Args);

  private:
    template <std::size_t i> struct argt_type_h
    {
        static_assert(i < arg_count, "Argument index ouf of bounds");

        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };

  public:
    typedef R return_type;

    template <std::size_t i> using arg_type = typename argt_type_h<i>::type;
};
