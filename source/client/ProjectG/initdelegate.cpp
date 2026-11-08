#include "initdelegate.h"

CInitDelegate::FUNC CInitDelegate::m_ReleaseFunctions[256] = { 0 };
CInitDelegate::FUNC CInitDelegate::m_InitFunctions[256] = { 0 };
int CInitDelegate::m_numInitFunctions = 0;
int CInitDelegate::m_numReleaseFunctions = 0;

void CInitDelegate::AddInitFunction(FUNC func)
{
	if (m_numInitFunctions < 256)
	{
		m_InitFunctions[m_numInitFunctions++] = func;
	}
}

void CInitDelegate::AddReleaseFunction(FUNC func)
{
	if (m_numReleaseFunctions < 256)
	{
		m_ReleaseFunctions[m_numReleaseFunctions++] = func;
	}
}

void CInitDelegate::ExecuteInitFunctions()
{
	for (int i = 0; i < m_numInitFunctions; ++i)
	{
		m_InitFunctions[i]();
	}
}

void CInitDelegate::ExecuteReleaseFunctions()
{
	for (int i = 0; i < m_numReleaseFunctions; ++i)
	{
		m_ReleaseFunctions[i]();
	}
}
