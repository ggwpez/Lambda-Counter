/*
 * SPDX-FileCopyrightText: 2016-2021 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "dontcache.hpp"

#include <experimental/functional>
#include <functional>

template <typename T> struct is_dc
{
    typedef std::false_type type;
};

template <> struct is_dc<DontCache_t>
{
    typedef std::true_type type;
};

template <typename T> using is_hashable_h = decltype(std::hash<T>()(std::declval<T &>()));

template <typename T> using is_hashable = typename std::experimental::is_detected<is_hashable_h, T>::type;

template <typename T> using is_compareable_h = decltype(std::declval<T &>() == std::declval<T &>());

template <typename T> using is_compareable = typename std::experimental::is_detected<is_compareable_h, T>::type;

template <typename T> using is_less_compareable_h = decltype(std::declval<T &>() < std::declval<T &>());

template <typename T>
using is_less_compareable = typename std::experimental::is_detected<is_less_compareable_h, T>::type;
