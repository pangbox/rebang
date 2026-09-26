#pragma once

template <class T>
class WSingleton
{
public:
	static T* Instance(void) { return m_pInstance; }
	static bool IsInstantiated(void) { return m_pInstance ? true : false; }

protected:
	static T* m_pInstance;
};
