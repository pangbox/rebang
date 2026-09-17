#include "minatl.h"
#include "rtti.h"

WRTTI::WRTTI(const char* name, const WRTTI* baseRTTI)
	: m_pName(name), m_pBaseRTTI(baseRTTI)
{
}
