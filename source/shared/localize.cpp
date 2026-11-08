#include <bitset>

#include "localize.h"

static __int64 g_localCountryCode;
static std::bitset<MAX_LOCAL_CONTENTS> s_localContentSet;

#include "localize_kor.h"

bool IsLocalCountry(__int64 code)
{
	return (g_localCountryCode & code) ? true : false;
}

bool IsLocalContent(localContentType_t code)
{
	if (code >= MAX_LOCAL_CONTENTS)
		return false;

	return s_localContentSet[code];
}

int GetLocalCountry()
{
	return (int)g_localCountryCode;
}
