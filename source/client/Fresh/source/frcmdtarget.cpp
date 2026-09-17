#include <string.h>
#include "frcmdtarget.h"

const sFRESH_MSGMAP* FrCmdTarget::GetMessageMap() const
{
	return 0;
}

bool FreshDispatchMsg(FrCmdTarget* pTarget, int var1, sFRESH_ENTRY* pEntry,
	sFRESH_HANDLER* pHandler)
{
	if (pHandler)
	{
		pHandler->pFn = pEntry->pFn;
		pHandler->eFt = pEntry->eFt;
		pHandler->pFresh = pTarget;
		return true;
	}

	uFreshFunctions mmf;
	mmf.pFn = pEntry->pFn;
	bool result = true;
	switch (pEntry->eFt)
	{
	case FrFuncVV:
		(pTarget->*mmf.pFn_FRESH_VV)();
		break;
	case FrFuncBV:
		result = (pTarget->*mmf.pFn_FRESH_BV)();
		break;
	case FrFuncVI:
		(pTarget->*mmf.pFn_FRESH_VI)(var1);
		break;
	case FrFuncBI:
		result = (pTarget->*mmf.pFn_FRESH_BI)(var1);
		break;
	}

	return result;
}

FrCmdTarget::FrCmdTarget()
{
}

FrCmdTarget::~FrCmdTarget()
{
}

sFRESH_ENTRY* FrCmdTarget::FindFreshEntry(sFRESH_ENTRY* pBegin,
	const char* pName, eFrCmd cmd)
{
	sFRESH_ENTRY* pFind = pBegin;
	while (pFind && pFind->pFn)
	{
		if (_stricmp(pFind->pName, pName) == 0 && pFind->cmd == cmd)
			return pFind;
		++pFind;
	}

	sFRESH_ENTRY* pDefault = pBegin;
	while (pDefault && pDefault->pFn)
	{
		if (*pDefault->pName == 0 && pDefault->cmd == cmd)
			return pDefault;
		++pDefault;
	}

	return 0;
}

bool FrCmdTarget::OnFreshMsg(const char* pName, eFrCmd cmd, int var1,
	sFRESH_HANDLER* pHandler)
{
	const sFRESH_MSGMAP* pMap = GetMessageMap();
	while (pMap && pMap->pEntry)
	{
		sFRESH_ENTRY* pEntry = FindFreshEntry(pMap->pEntry, pName, cmd);
		if (pEntry)
			return FreshDispatchMsg(this, var1, pEntry, pHandler);
		pMap = pMap->pBaseMsgMap;
	}

	return true;
}

sFRESH_MSGMAP FrCmdTarget::_MsgMap = { 0, 0 };
sFRESH_ENTRY FrCmdTarget::_MsgEntries[] = {
	{ 0, FRCMD_NONE, FrFuncNone, 0 }
};
