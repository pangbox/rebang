#pragma once

template <class T>
__forceinline T Max(T a, T b)
{
	return (a < b) ? b : a;
}

template <class T>
__forceinline T Min(T a, T b)
{
	return (a > b) ? b : a;
}
