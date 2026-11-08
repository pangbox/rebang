#include "ghosttask.h"
#include "actor.h"
#include "fresh.h"
#include "taskmanager.h"

extern Fresh* g_pFresh;
extern int g_iRecvPacketType;
IMPLEMENT_OBJECT(CGhostTask, CTask)

CGhostTask::CGhostTask()
{
	IActor::m_defaultSkipLayer = 0;
	IActor::m_pLayer = NULL;
}

void CGhostTask::Load()
{
	SetProgressBar(false);
	SetTip(false);

	CTask::Load();
}

void CGhostTask::Init(const char* wallPaper)
{
	CTask::Init("ghost_main_bg.jpg");

	SetWhisper(false);
}

void CGhostTask::Register()
{
	SetMainActor(AddActor("CGhostMain", "Ghost", 0, ""));
}

void CGhostTask::Process(float delta)
{
	CTask::Process(delta);
	g_pFresh->OnProcess(delta);
}

void CGhostTask::Display()
{
	CTask::Display();
	g_pFresh->OnDisplay(true);
}

int CGhostTask::OnPacket(WReceivedPacket& packet)
{
	unsigned short type = packet.Decode2();

	g_iRecvPacketType = type;

	switch (type)
	{
	case 0x132:
	{
		packet.DecodeStr();
		packet.Decode1();
		break;
	}

	default:
		return OnPacketCommon(packet);
	}

	return 1;
}
