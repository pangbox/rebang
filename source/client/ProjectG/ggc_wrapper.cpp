#include "ggc_wrapper.h"
#include "npgamelib.h"
#include "ggc_helper.h"
#include "projectg.h"
#include "actor.h"

CNPGameLib* g_pGameGuard = NULL;

BOOL _GGC_Init()
{
	if (!g_pGameGuard)
	{
		g_pGameGuard = new CNPGameLib("PangyaTest");
	}

	DWORD dwResult = g_pGameGuard->Init();

	if (dwResult == NPGAMEMON_SUCCESS)
		return TRUE;

	return GGC_GetHelper()->StackError(dwResult, true);
}

void _GGC_End()
{
	if (g_pGameGuard)
	{
		delete g_pGameGuard;
		g_pGameGuard = NULL;
	}
}

void _GGC_SetHwnd(HWND hWnd)
{
	if (g_pGameGuard)
		g_pGameGuard->SetHwnd(hWnd);
}

void _GGC_SendUserID(const char* id)
{
	if (g_pGameGuard)
		g_pGameGuard->Send(id);
}

BOOL _GGC_CheckGameMon()
{
	if (g_pGameGuard)
		return g_pGameGuard->Check() == NPGAMEMON_SUCCESS;

	return TRUE;
}

void _GGC_CSAuth2(WReceivedPacket& packet)
{
	if (!g_pGameGuard)
		return;

	GG_AUTH_DATA authData;
	packet.DecodeBuffer(&authData, sizeof(authData));

	GG_AUTH_DATA errorData = { 1000, 0, 0, 0 };

	if (memcmp(&authData, &errorData, sizeof(GG_AUTH_DATA)) == 0)
		CProjectG::Instance()->GameGuardLog("CSAuth2 requested with error.");
	else
		CProjectG::Instance()->GameGuardLog("CSAuth2 requested.");

	g_pGameGuard->Auth2(&authData);
}

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	if (dwMsg == NPGAMEMON_CHECK_CSAUTH2)
	{
		GG_AUTH_DATA authData = *(PGG_AUTH_DATA)dwArg;
		AfxGetTask()->SendMsgToMainActor(
			MsgObject(NULL, 619, (int)&authData, 0, 0, 0, 0));

		return TRUE;
	}

	return GGC_GetHelper()->StackError(dwMsg, false);
}
