#pragma once

#include <array>
#include <cstdint>

template <typename T, std::size_t s, std::size_t... ss> struct multiarray;

template <typename T, std::size_t s, std::size_t... ss> struct multiarray
{
	typedef std::array<typename multiarray<T, ss...>::type, s> type;
};

template <typename T, std::size_t s> struct multiarray<T, s>
{
	typedef std::array<T, s> type;
};
