#pragma once

enum eFrCmd
{
	FRCMD_NONE,
	FRCMD_INIT,
	FRCMD_MOUSEMOVE,
	FRCMD_LBUTTONDOWN,
	FRCMD_LBUTTONUP,
	FRCMD_RBUTTONDOWN,
	FRCMD_RBUTTONUP,
	FRCMD_DBLCLICK,
	FRCMD_OWNERDRAW,
	FRCMD_ENTERKEY,
	FRCMD_LOSTKEYFOCUS,
	FRCMD_HOVERON,
	FRCMD_HOVEROFF,
	FRCMD_FINISH,
	FRCMD_DESTROY
};

enum eFrFuncType
{
	FrFuncNone,
	FrFuncVV,
	FrFuncBV,
	FrFuncBI,
	FrFuncVI,
	FrFuncBI2,
	FrFuncBI3
};

class FrCmdTarget;
typedef void (FrCmdTarget::*FRESH_PFN)();
typedef bool (FrCmdTarget::*FRESH_PFN_BV)();
typedef void (FrCmdTarget::*FRESH_PFN_VI)(int);
typedef bool (FrCmdTarget::*FRESH_PFN_BI)(int);

union uFreshFunctions
{
	FRESH_PFN pFn;
	FRESH_PFN pFn_FRESH;
	FRESH_PFN pFn_FRESH_VV;
	FRESH_PFN_BV pFn_FRESH_BV;
	FRESH_PFN_VI pFn_FRESH_VI;
	FRESH_PFN_BI pFn_FRESH_BI;
};

struct sFRESH_ENTRY
{
	char* pName;
	eFrCmd cmd;
	eFrFuncType eFt;
	FRESH_PFN pFn;
};

struct sFRESH_HANDLER
{
	FrCmdTarget* pFresh;
	eFrFuncType eFt;
	FRESH_PFN pFn;
};

struct sFRESH_MSGMAP
{
	sFRESH_MSGMAP* pBaseMsgMap;
	sFRESH_ENTRY* pEntry;
};

class FrCmdTarget
{
public:
	FrCmdTarget();
	virtual ~FrCmdTarget();
	bool OnFreshMsg(const char* pName, eFrCmd cmd, int var1,
		sFRESH_HANDLER* pHandler);

protected:
	virtual const sFRESH_MSGMAP* GetMessageMap() const;
	sFRESH_ENTRY* FindFreshEntry(sFRESH_ENTRY* pBegin, const char* pName,
		eFrCmd cmd);

	static sFRESH_MSGMAP _MsgMap;
	static sFRESH_ENTRY _MsgEntries[];
};
