#pragma once

#include <map>
#include <algorithm>

class FunctionMapper;
typedef void (FunctionMapper::*FUNCTION_PTR)();

struct sFunctionHandler
{
	int id;
	int type;
	FUNCTION_PTR pFunc;
};

enum eFunctionType
{
	FUNCTIONTYPE_VOID = 1,
	FUNCTIONTYPE_BOOL,
	FUNCTIONTYPE_ARG,
	FUNCTIONTYPE_BOOL_ARG,
};

class FunctionMapper
{
public:
	FunctionMapper() { }
	virtual ~FunctionMapper() { }

	void AddFunctionType(int id, eFunctionType type, FUNCTION_PTR pFunc)
	{
		sFunctionHandler handler;
		handler.id = id;
		handler.type = type;
		handler.pFunc = pFunc;

		m_functionMap.insert(std::make_pair(id, handler));
	}

	void SetFunctionType(int type) { m_functionType = type; }

	template <class T, class A>
	bool Exec(T* pObj, A arg)
	{
		std::map<int, sFunctionHandler>::const_iterator it =
			m_functionMap.find(m_functionType);
		if (it == m_functionMap.end())
		{
			return false;
		}

		sFunctionHandler handler = (*it).second;

		FUNCTION_PTR pFunc = handler.pFunc;

		bool bResult = true;

		switch (handler.type)
		{
		case FUNCTIONTYPE_VOID:
			(pObj->*(void (T::*)())pFunc)();
			break;
		case FUNCTIONTYPE_BOOL:
			bResult = (pObj->*(bool (T::*)())(void (T::*)())pFunc)();
			break;
		case FUNCTIONTYPE_ARG:
			(pObj->*(void (T::*)(A))(void (T::*)())pFunc)(arg);
			break;
		case FUNCTIONTYPE_BOOL_ARG:
			bResult = (pObj->*(bool (T::*)(A))(void (T::*)())pFunc)(arg);
			break;
		}

		return bResult;
	}

protected:
	std::map<int, sFunctionHandler> m_functionMap;
	int m_functionType;
};

namespace _util
{

	template <class T>
	struct NewCreator
	{
		static T* CreateObject() { return new T; }
	};

	template <class T, class R>
	struct WMemFun
	{
		typedef R (T::*Func)();

		WMemFun(T& obj, Func func)
			: m_pObj(&obj), m_func(func)
		{
		}

		R operator()() { return (m_pObj->*m_func)(); }

		T* m_pObj;
		Func m_func;
	};

	template <class T, class R, class A>
	struct WMemFun1
	{
		typedef R (T::*Func)(A);

		WMemFun1(T& obj, Func func)
			: m_pObj(&obj), m_func(func)
		{
		}

		R operator()(A a) { return (m_pObj->*m_func)(a); }

		T* m_pObj;
		Func m_func;
	};

	template <class T, class R>
	WMemFun<T, R> MakeMemFun(T& obj, R (T::*func)())
	{
		return WMemFun<T, R>(obj, func);
	}

	template <class T, class R, class A>
	WMemFun1<T, R, A> MakeMemFun1(T& obj, R (T::*func)(A))
	{
		return WMemFun1<T, R, A>(obj, func);
	}

}

template <class C, class P>
bool FindIf(const C& c, P pred)
{
	typename C::const_iterator it = std::find_if(c.begin(), c.end(), pred);
	return it != c.end() ? true : false;
}

template <class T>
T wAbs(T value)
{
	return value >= 0 ? value : -value;
}

template <class T>
T SAFE_SUBVALUE(T& value, const T& min, T sub)
{
	if (value > min)
	{
		value -= sub;
		if (value < min)
			value = min;
	}
	return value;
}

template <class T>
T SAFE_ADDVALUE(T& value, const T& max, T add)
{
	if (value < max)
	{
		value += add;
		if (value > max)
			value = max;
	}
	return value;
}

template <class T, class U>
bool SAFE_RANGEVALUE(T& value, const U& min, const U& max, const U& newValue)
{
	if (value < min || value > max)
	{
		value = min;
		return false;
	}
	value = newValue;
	return true;
}

template <class T>
bool IS_INRANGE(const T& value, const T& min, const T& max)
{
	return value >= min && value <= max;
}

struct _FindByGuid
{
	_FindByGuid(unsigned long guid)
		: m_guid(guid)
	{
	}
	template <class T>
	bool operator()(const T& t) const
	{
		return m_guid == t.guid;
	}

	unsigned long m_guid;
};

struct _FindByGuid_Ptr
{
	_FindByGuid_Ptr(unsigned long guid)
		: m_guid(guid)
	{
	}
	template <class T>
	bool operator()(const T* p) const
	{
		return m_guid == p->id;
	}

	unsigned long m_guid;
};

struct _FindItem
{
	_FindItem(unsigned long typeId, unsigned long id)
		: m_typeId(typeId), m_id(id)
	{
	}
	template <class T>
	bool operator()(const T& t) const
	{
		return m_typeId == t.tid && m_id == t.guid;
	}

	unsigned long m_typeId;
	unsigned long m_id;
};

struct _FindByUid
{
	_FindByUid(unsigned long uid)
		: m_uid(uid)
	{
	}
	template <class T>
	bool operator()(const T& t) const
	{
		return m_uid == t.uid;
	}

	unsigned long m_uid;
};

template <class T>
struct less_guid_ptr
{
	bool operator()(const T a, const T b) const { return a->id < b->id; }
};

template <class C>
void back_insert_null(C& c, unsigned int n)
{
	while (n--)
		c.push_back(NULL);
}
