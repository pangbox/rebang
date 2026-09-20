#pragma once

#include <map>
extern "C" __declspec(dllimport) int __cdecl _mbscmp(const unsigned char*,
	const unsigned char*);

namespace list
{
	template <class T>
	struct less_str : public std::binary_function<T, T, bool>
	{
		bool operator()(const T& left, const T& right) const
		{
			T rightText = right;
			T leftText = left;
			return _mbscmp((const unsigned char*)leftText,
					   (const unsigned char*)rightText) < 0
				? true
				: false;
		}
	};

	template <class T>
	class multimap_str
		: public std::multimap<const char*, T, less_str<const char*> >
	{
	};
}
