#pragma once

#include "die.hpp"
#include <vector>

#define DEBUG 1

template<typename T>
class static_stack
{
public:
	inline static_stack(std::size_t length)
		: _data(length), _iter(_data.begin())
	{ }

	inline void reset()
	{
		_iter = _data.begin();
	}

	inline void push(T const& obj)
	{
		if (DEBUG && _iter == _data.end())
			die("Stack overflow");

		*_iter++ = obj;
	}

	inline void push(T&& obj)
	{
		if (DEBUG && _iter == _data.end())
			die("Stack overflow");

		*_iter++ = std::move(obj);
	}

	inline T& top()
	{
		if (DEBUG && _iter == _data.begin())
			die("Stack empty");

		return *(_iter -1);
	}

	inline T&& toppop()
	{
		if (DEBUG && _iter == _data.begin())
			die("Stack underflow");

		return std::move(*--_iter);
	}

	inline void pop()
	{
		if (DEBUG && _iter == _data.begin())
			die("Stack underflow");

		--_iter;
	}

	inline std::size_t size()
	{
		return std::distance(_data.begin(), _iter);
	}

private:
	std::vector<T> _data;
	typename std::vector<T>::iterator _iter;
};
