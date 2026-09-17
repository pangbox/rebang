#pragma once

class WRTTI
{
public:
	WRTTI(const char* name, const WRTTI* baseRTTI);

	const char* m_pName;
	const WRTTI* m_pBaseRTTI;
};
