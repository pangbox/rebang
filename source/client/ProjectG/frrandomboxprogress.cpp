#include "frrandomboxprogress.h"
#include "frgaugebar.h"
#include "frstatic.h"
#include "actor.h"
IMPLEMENT_OBJECT(FrRandomBoxProgress, FrForm)

BEGIN_FRESH_MSGMAP(FrRandomBoxProgress, FrForm)

ON_FRESH_VI("waitbar", FRCMD_INIT, FrRandomBoxProgress::OnProgressInit)
ON_FRESH_VI("cancle", FRCMD_LBUTTONUP, FrRandomBoxProgress::OnCancelUp)
ON_FRESH_VI("explane", FRCMD_INIT, FrRandomBoxProgress::OnExplaneStatic_Init)

END_FRESH_MSGMAP()

FrRandomBoxProgress::FrRandomBoxProgress()
{
	m_pProgress = NULL;
	m_elapsedTime = 0;
	m_totalTime = 0;
	m_pExplaneStatic = NULL;
}

FrRandomBoxProgress::~FrRandomBoxProgress()
{
}

void FrRandomBoxProgress::SetExplane(const char* text)
{
	if (text == NULL)
		return;

	if (m_pExplaneStatic == NULL)
		return;

	m_pExplaneStatic->Enable(true);
	m_pExplaneStatic->SetVisible(true);
	m_pExplaneStatic->SetCaption(text);
}

void FrRandomBoxProgress::OnProgressInit(int param)
{
	m_pProgress = DYNAMIC_CAST(FrGaugeBar, param);
	m_pProgress->SetRange(0, 100, 0);
}

void FrRandomBoxProgress::OnCancelUp(int param)
{
	FrForm::OnCancel();
}

void FrRandomBoxProgress::OnExplaneStatic_Init(int param)
{
	m_pExplaneStatic = DYNAMIC_CAST(FrStatic, param);
	if (m_pExplaneStatic)
	{
		m_pExplaneStatic->SetVisible(false);
		m_pExplaneStatic->Enable(false);
	}
}

bool FrRandomBoxProgress::OnInit()
{
	const char* explane = m_pExplaneStatic->GetCaption();
	if (strlen(explane))
	{
		FrGaugeBar* pProgress = m_pProgress;
		WRect rect = pProgress->GetRect();

		rect.y += 15;
		pProgress->SetRect(rect);
	}

	srand(GetTickCount());
	m_totalTime = rand() % 4 + 4;

	return true;
}

void FrRandomBoxProgress::OnProc(const float dt)
{
	m_elapsedTime += dt;

	if (m_elapsedTime > m_totalTime)
	{
		m_pProgress->SetPos(100);

		AfxGetTask()->GetMainActor() << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		WSendPacket send((enumClientPacket)0xf1);
		send.Encode4(m_boxTid);
		send.Send(TO_GAME);

		m_elapsedTime = 0;
		m_totalTime = 0;

		FrForm::OnCancel();
	}
	else
	{
		int pos = (int)((m_elapsedTime / m_totalTime) * 100.0f);
		if (pos < 100)
			m_pProgress->SetPos(pos);
	}
}
