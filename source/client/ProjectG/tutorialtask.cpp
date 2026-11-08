#include "tutorialtask.h"
#include "actor.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "clientsetting.h"
#include "mousecursor.h"
#include "projectg.h"

extern Fresh* g_pFresh;
extern int g_iRecvPacketType;

class CCapturedBg;
class CSceneManager;

CGolfDoc* m_pGolfDoc;
CSceneManager* m_pSceneManger;
CCapturedBg* m_pCapturedBg;

IMPLEMENT_OBJECT(CTutorialTask, CTask)

CTutorialTask::CTutorialTask()
{
	IActor::m_defaultSkipLayer = 0;
	IActor::m_pLayer = NULL;
}

CTutorialTask::~CTutorialTask()
{
	Doc()->m_gameMode = 0;
}

void CTutorialTask::Init(const char* wallPaper)
{
	CTask::Init("tutorial_background.jpg");

	SetWhisper(false);
}

void CTutorialTask::Register()
{
	SetMainActor(AddActor("CTutorialMain", "TutorialMain", 0, ""));
}

void CTutorialTask::Load()
{
	SetProgressBar(false);
	SetTip(false);

	CTask::Load();
}

void CTutorialTask::PreserveBack(const char* name)
{
	CTask::PreserveBack(name);
}

void CTutorialTask::RestorePreserved()
{
	CTask::RestorePreserved();

	g_pFresh->GetManager()->GetDesktop()->SetViewFocus(true);

	COption::Instance()->vApplyLobbyScreenSize();
	CMouseCursor::Instance()->SetActive(true);

	g_pFresh->GetManager()->GetDesktop()->Init(g_view->GetWidth(),
		g_view->GetHeight());
	g_pFresh->GetManager()->GetDesktop()->SetWallPaper(
		"tutorial_background.jpg", true);
}

void CTutorialTask::Destroy()
{
	CTask::Destroy();
}

void CTutorialTask::Process(float delta)
{
	CTask::Process(delta);
	g_pFresh->OnProcess(delta);
}

void CTutorialTask::Display()
{
	if (!Doc()->m_bBackgroundVideo)
		CTask::Display();

	g_pFresh->OnDisplay(Doc()->m_bBackgroundVideo != 0);
}

int CTutorialTask::OnPacket(WReceivedPacket& packet)
{
	unsigned short type = packet.Decode2();

	g_iRecvPacketType = type;

	switch (type)
	{
	case 0x11b:
	{
		unsigned char index = packet.Decode1();
		unsigned char mode = packet.Decode1();

		if (index < 3)
		{
			unsigned long complete = packet.Decode4();
			Doc()->m_tutorialComplete[index] = complete;
		}
		else
		{
			packet.DecodeBuffer(Doc()->m_tutorialComplete,
				sizeof(Doc()->m_tutorialComplete));
		}

		CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0x151, index,
			mode, 0, 0);
		break;
	}

	case 0x11c:
	{
		unsigned char step = packet.Decode1();
		CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0x153, step, 0,
			0, 0);
		break;
	}

	default:

		return OnPacketCommon(packet);
	}

	return 1;
}
