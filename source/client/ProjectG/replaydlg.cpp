#include "replaydlg.h"
#include "mousecursor.h"
#include "projectg.h"

IMPLEMENT_OBJECT(FrReplayControlDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrReplayControlDlg, FrForm)

ON_FRESH_VI("replay_play", FRCMD_INIT, FrReplayControlDlg::OnPlayInit)
ON_FRESH_VV("replay_play", FRCMD_LBUTTONUP, FrReplayControlDlg::OnPlayBtnUp)
ON_FRESH_VI("replay_before", FRCMD_INIT,
	FrReplayControlDlg::OnBeforeShotBtnInit)
ON_FRESH_VV("replay_before", FRCMD_LBUTTONUP,
	FrReplayControlDlg::OnBeforeShotBtnUp)
ON_FRESH_VI("replay_next", FRCMD_INIT, FrReplayControlDlg::OnNextShotBtnInit)
ON_FRESH_VV("replay_next", FRCMD_LBUTTONUP, FrReplayControlDlg::OnNextShotBtnUp)
ON_FRESH_VI("replay_exit", FRCMD_INIT, FrReplayControlDlg::OnExitBtnInit)
ON_FRESH_VV("replay_exit", FRCMD_LBUTTONUP, FrReplayControlDlg::OnExitBtnUp)
ON_FRESH_VI("replay_minimize", FRCMD_INIT,
	FrReplayControlDlg::OnMinimizeBtnInit)
ON_FRESH_VV("replay_minimize", FRCMD_LBUTTONUP,
	FrReplayControlDlg::OnMinimizeBtnUp)
ON_FRESH_VI("replay_record", FRCMD_INIT, FrReplayControlDlg::OnRecordBtnInit)
ON_FRESH_VV("replay_record", FRCMD_LBUTTONUP, FrReplayControlDlg::OnRecordBtnUp)

END_FRESH_MSGMAP()

void FrReplayControlDlg::Init()
{
}

FrReplayControlDlg::FrReplayControlDlg()
{
	m_pPlay = NULL;
	m_bPlay = true;
	m_alpha = 0.0f;
}

FrReplayControlDlg::~FrReplayControlDlg()
{
}

bool FrReplayControlDlg::OnInit()
{
	FrForm::OnInit();

	EnableDrag(false);
	return true;
}

void FrReplayControlDlg::OnPlayInit(int param)
{
	m_pPlay = DYNAMIC_CAST(FrButton, param);
}

void FrReplayControlDlg::TogglePlayButton()
{
	if (m_bPlay)
	{
		Doc()->m_replayState = 4;

		m_pPlay->SetButtonImg("replay_play_n", FrButton::NORMAL);
		m_pPlay->SetButtonImg("replay_btn_o_big", FrButton::OVER);
		m_pPlay->SetButtonImg("replay_play_n", FrButton::PRESSED);
		m_pPlay->SetButtonBelowImg("replay_play_n", FrButton::OVER);
	}
	else
	{
		Doc()->m_replayState = Doc()->m_replayShotIndex >= 2 ? 2 : 1;

		m_pPlay->SetButtonImg("replay_stop_n", FrButton::NORMAL);
		m_pPlay->SetButtonImg("replay_btn_o_big", FrButton::OVER);
		m_pPlay->SetButtonImg("replay_stop_n", FrButton::PRESSED);
		m_pPlay->SetButtonBelowImg("replay_stop_n", FrButton::OVER);
	}

	m_bPlay = !m_bPlay;
}

void FrReplayControlDlg::OnPlayBtnUp()
{
	TogglePlayButton();
}

void FrReplayControlDlg::OnBeforeShotBtnInit(int param)
{
	m_pBeforeShot = DYNAMIC_CAST(FrButton, param);

	m_pBeforeShot->Enable(false);
}

void FrReplayControlDlg::OnBeforeShotBtnUp()
{
	if (!m_bPlay)
	{
		TogglePlayButton();
	}

	Doc()->m_replayState = 11;
}

void FrReplayControlDlg::OnNextShotBtnInit(int param)
{
	m_pNextShot = DYNAMIC_CAST(FrButton, param);

	m_pNextShot->Enable(false);
}

void FrReplayControlDlg::OnNextShotBtnUp()
{
	if (Doc()->m_replayPlayType == 2)
	{
		if (!m_bPlay)
		{
			TogglePlayButton();
		}

		Doc()->m_replayState = 12;
	}
}

void FrReplayControlDlg::OnExitBtnInit(int param)
{
	m_pExit = DYNAMIC_CAST(FrButton, param);
}

void FrReplayControlDlg::OnExitBtnUp()
{
	Doc()->m_replayShotIndex = 0;
	Doc()->m_curChannel.Type = Doc()->m_replayTick;
	Doc()->m_replayTick = 0;

	Doc()->m_replayPlayType = 0;

	Doc()->m_replayState = 5;
	Close(true);
}

void FrReplayControlDlg::OnMinimizeBtnInit(int param)
{
	m_pMinimize = DYNAMIC_CAST(FrButton, param);
}

void FrReplayControlDlg::OnMinimizeBtnUp()
{
	CMouseCursor::Instance()->ForceMove(g_view->GetWidth() * 0.5f,
		g_view->GetHeight() * 0.5f, -1.0f);
	SetVisible(false);
}

void FrReplayControlDlg::OnRecordBtnInit(int param)
{
	m_pRecord = DYNAMIC_CAST(FrButton, param);

	m_pRecord->Enable(false);

	if (Doc()->m_replayState == 9)
		m_pRecord->Enable(false);
}

void FrReplayControlDlg::OnRecordBtnUp()
{
	Doc()->m_replayState = 6;
}

void FrReplayControlDlg::OnProc(const float delta)

{
	if (CMouseCursor::Instance()->InArea(m_rect))
	{
		m_alpha += delta * 2.0f;
		if (m_alpha > 1.0f)
			m_alpha = 1.0f;
	}
	else
	{
		m_alpha -= delta * 2.0f;
		if (m_alpha < 0.6f)
			m_alpha = 0.6f;
	}

	SetAlpha2ToChild(m_alpha);
}

void CReplayControlManager::Init()

{
	m_controlRect.x = g_view->GetWidth();
	m_controlRect.y = g_view->GetHeight() - 560.0f;
	m_controlRect.w = 335.0f;
	m_controlRect.h = 101.0f;

	m_ratio = 0.7f;
	m_reserved = 0;
}

CReplayControlManager::CReplayControlManager()
	: m_fadeType(0)
{
	m_controlRect.x = 10.0f;
	m_controlRect.y = 10.0f;
	m_controlRect.w = 335.0f;
	m_controlRect.h = 101.0f;

	m_ratio = 0.5f;
	m_reserved = 0;
}

CReplayControlManager::~CReplayControlManager()
{
}

bool CReplayControlManager::CalcReplayControlPos()
{
	WRect rect = GetControlRect();
	WVector mouse = g_input->GetMousePoint();

	float limit = g_view->GetWidth() - 340.0f;
	if (limit < mouse.x && mouse.x < g_view->GetWidth() && 0.0f < mouse.y &&
		mouse.y < 140.0f)
	{
		if (rect.x > limit)
			rect.x -= 15.0f;

		SetControlRect(rect);

		if (500.0f < rect.x)
			SetFadeType(2);

		return true;
	}
	else if (GetFadeType() == 2)
	{
		if (rect.x >= g_view->GetWidth())
			SetFadeType(0);
		else
			SetFadeType(2);

		rect.x += 15.0f;
		SetControlRect(rect);

		return true;
	}

	return false;
}
