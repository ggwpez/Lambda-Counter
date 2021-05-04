/*
 * SPDX-FileCopyrightText: 2016-2021 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <experimental/type_traits>

/// Will indicate that a function argument is insignificant for the caching.
struct DontCache : std::experimental::nonesuch
{
};

typedef DontCache DontCache_t;
