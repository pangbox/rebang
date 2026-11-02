#pragma once

#include <map>
#include <string>
#include <typeinfo>

typedef void (*converter_func)(const void* from, void* to);

struct typename_pair
{
	typename_pair() { }

	const char* from;
	const char* to;

	bool operator<(const typename_pair& p) const
	{
		return std::string(from) + to < std::string(p.from) + p.to;
	}
};

class type_convert
{
	typedef std::map<typename_pair, converter_func> converter_map;

public:
	template <class From, class To>
	type_convert(const From* from, To* to)
	{
		m_bFailed = false;
		typename_pair key;
		key.from = typeid(From).name();
		key.to = typeid(To).name();

		converter_map::const_iterator it = convmap().find(key);
		if (it != convmap().end())
			it->second(from, to);
		else
			m_bFailed = true;
	}
	template <class From, class To>
	static bool insert_converter(converter_func func)
	{
		typename_pair key;

		key.from = typeid(From).name();
		key.to = typeid(To).name();
		converter_map::iterator it = convmap().find(key);

		if (it == convmap().end())
			convmap().insert(converter_map::value_type(key, func));

		return true;
	}

private:
	static converter_map& convmap()
	{
		static converter_map _cvmap;
		return _cvmap;
	}

	bool m_bFailed;
};
