#pragma once

template <class T>
class WSingleton
{
public:
	static T* Instance(void) { return m_pInstance; }

protected:
	static T* m_pInstance;
};
