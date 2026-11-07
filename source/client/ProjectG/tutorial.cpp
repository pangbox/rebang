#include "minatl.h"
#include "tutorial.h"
#include "quizdlg.h"
#include "inputmanager.h"
#include "mousecursor.h"
#include "chatmsg.h"
#include "capturedbg.h"
#include "golfball.h"
#include "club.h"
#include "wind.h"
#include "quadtree.h"
#include "projectg.h"
#include "westpak.h"
#include "../../shared/localize.h"
#include "../../shared/sharedtables.h"

extern Fresh* g_pFresh;
extern WView* g_view;
extern WResourceManager* g_resrcmng;
extern WMatrix g_camera;
extern CInputManager* g_input;
extern bool g_bQuit;

char* MakeStr(const char* format, ...);
bool SetCurMap(unsigned char map);

IMPLEMENT_ACTOR(CTutorial, CTaskMain)

struct sTutorialInfo
{
	int code;
	unsigned long typeId;
	int count;
};

static const sTutorialInfo s_tutorialInfo[20] = {
	{ 0x1,     0x1a00000f, 3   },
	{ 0x2,     0x18000007, 3   },
	{ 0x4,     0x18000005, 3   },
	{ 0x8,     0x18000008, 3   },
	{ 0x10,    0x1a000010, 500 },
	{ 0x20,    0x18000004, 3   },
	{ 0x40,    0x1a000010, 500 },
	{ 0x80,    0x00000000, 1   },
	{ 0x100,   0x1a00000f, 3   },
	{ 0x200,   0x18000028, 1   },
	{ 0x400,   0x18000006, 1   },
	{ 0x800,   0x18000007, 5   },
	{ 0x1000,  0x18000000, 4   },
	{ 0x2000,  0x18000001, 4   },
	{ 0x8000,  0x18000005, 3   },
	{ 0x10000, 0x18000005, 3   },
	{ 0x20000, 0x18000005, 3   },
	{ 0x40000, 0x18000005, 3   },
	{ 0x80000, 0x18000005, 3   },
	{ 0x80000, 0x18000005, 3   },
};

static const char s_buttonName[3][8] = { "ok", "yes", "keep" };

static const char s_caddieFace[9][32] = {
	"dummy",
	"pipin_facial_smile01",
	"pipin_facial_smile03",
	"pipin_facial_play",
	"pipin_facial_cry",
	"pipin_facial_difficulty",
	"pipin_facial_angry",
	"pipin_facial_surprise",
	"pipin_facial_embarras",
};

static const char s_missionImage[14][32] = {
	"[mission01.png",
	"[mission02.png",
	"[mission03.png",
	"[mission04.png",
	"[mission05.png",
	"[mission06.png",
	"[mission07.png",
	"[mission08.png",
	"[mission09.png",
	"[mission10.png",
	"[mission11.png",
	"[mission12.png",
	"[mission13.png",
	"[mission14.png",
};

static const int s_missionImageOffset[14] = { 44, 73, 161, 44, 155, 100, 101,
	155, 143, 100, 100, 100, 100, 93 };
AnimatedOverlay::AnimatedOverlay()
{
	m_srcRect = WRect(0.0f, 0.0f, 1.0f, 1.0f);
	m_rect = WRect(0.0f, 0.0f, 1.0f, 1.0f);
	m_interval = 0;

	m_elapsed = 0;
	m_frame = 0;
	m_frameNum = 0;
	m_bActive = false;
	m_bRender = false;
	m_bLoop = false;

	m_fade = 0;
	m_alpha = 255.0f;

	m_pOverlay = NULL;

	m_column = 0;
}

AnimatedOverlay::~AnimatedOverlay()
{
	for (std::vector<WOverlay*>::iterator it = m_overlayList.begin();
		it != m_overlayList.end(); ++it)
	{
		if (g_resrcmng && *it)
		{
			g_resrcmng->Release(*it);
			*it = NULL;
		}
	}
}

void AnimatedOverlay::Process(float delta)
{
	if (!m_bRender && !m_bActive)
		return;

	if (m_frameNum < 1)
		return;

	if (m_fade == 0 && m_interval > 0.0f)
	{
		m_elapsed += delta;
		if (m_elapsed > m_interval)
		{
			if (m_frameNum == 1)
			{
				m_bRender = !m_bRender;
			}

			m_elapsed -= m_interval;
			m_frame++;

			if (!m_bActive || !m_bLoop)
			{
				if (m_frame == m_frameNum)
				{
					m_frame = 0;
					m_elapsed = 0;
					m_interval = 0;
					m_bRender = m_bActive;
				}
			}
			else
			{
				m_frame %= m_frameNum;
			}
		}
	}
	else if (m_fade != 0 || m_interval == 0.0f)
	{
		if (m_fade != 0)
		{
			m_elapsed += delta * 3.0f;
			float alpha =
				(float)((1 - m_fade) * 128.0f) + m_fade * m_elapsed * 255.0f;
			m_alpha = alpha;
			if (alpha < 0.0f || alpha > 255.0f)
			{
				m_alpha = Between<float>(0.0f, alpha, 255.0f);

				m_fade = 0;
				m_elapsed = 0;
				m_bRender = m_bActive;
			}
		}
	}

	GetNextOverlay();
}

void AnimatedOverlay::Activate(bool bActivate, bool bFade, bool bLoop)
{
	m_bActive = bActivate;
	m_bLoop = bLoop;
	if (bActivate)
	{
		m_frame = 0;
		m_elapsed = 0;
		m_alpha = 255.0f;
		m_bRender = bActivate;
	}

	if (bFade)
	{
		m_fade = bActivate ? 1 : -1;
	}
}

void AnimatedOverlay::Render()
{
	if (!m_bRender)
		return;

	if (!m_pOverlay)
		return;

	m_pOverlay->Render(g_view, m_srcRect, m_rect, 0,
		((int)m_alpha << 24) | 0xffffff, 0, 0);
}

void AnimatedOverlay::SetSourceOverlay(const char* name, int column, int row,
	int mode, float width, float height)
{
	if (!m_overlayList.empty())
	{
		std::vector<WOverlay*>::iterator it = m_overlayList.begin();
		if (g_resrcmng && *it)
		{
			g_resrcmng->Release(*it);
			*it = NULL;
		}
		m_overlayList.erase(it);
	}

	WOverlay* pOverlay = g_resrcmng->GetOverlay(name, 0);

	if (pOverlay == NULL)
		return;

	m_overlayList.push_back(pOverlay);

	m_pOverlay = pOverlay;

	m_column = column;
	m_frameNum = column * row;

	if (mode == 1)
	{
		m_srcRect.w = width / m_pOverlay->GetWidth();
		m_srcRect.h = height / m_pOverlay->GetHeight();
	}
	else
	{
		m_srcRect.w = 1.0f / column;
		m_srcRect.h = 1.0f / row;
	}
}

void AnimatedOverlay::GetNextOverlay()
{
	int x = m_frame % m_column;
	int y = m_frame / m_column;
	m_srcRect.x = x * m_srcRect.w;
	m_srcRect.y = y * m_srcRect.h;
}

CTutorial::CTutorial()
{
	m_pTipForm = NULL;
	m_pResultForm = NULL;
	m_pReviewForm = NULL;

	m_pCaddieForm = NULL;
	m_pSelectButton = NULL;
	m_completeCode = 0;

	m_pMissionForm = NULL;
	m_pMissionImage = NULL;

	m_pQuizForm = NULL;
	m_quizType = 0;

	m_bKeepBlockInput = false;
	m_bKeyPress = false;

	memset(m_itemTypeId, 0, sizeof(m_itemTypeId));

	m_dispPriority = DISP_PRIORITY_4;
	AddSkipList(0x10);

	m_bBlockInput = false;
}

CTutorial::~CTutorial()
{
	if (m_bBlockInput)
	{
		FreeInput();
	}
}

void CTutorial::OnInit()
{
	if (GOLFDOC()->m_tutorialMode == 15)
		return;

	for (int i = 0; i < 15; i++)
		m_bComplete[i] = false;

	Wind().CheckSpecialWind();

	m_tutorialIndex = -1;
	m_bSkipped = false;
	m_skipCount = 0;
	m_pResultForm = NULL;
	m_failCount = 0;
	m_hitCount = 0;
	m_bRepeatTip = false;
	m_repeatTipTime = -1;
	m_clearScale = 1.5f;
	m_reserved = 0;

	m_bReserved = false;
	m_gaugePos = 0;

	m_bPractice = false;

	m_tipRect.x = (g_view->GetWidth() - 370.0f) * 0.5f;
	m_tipRect.y = 50.0f;
	m_tipRect.w = 370.0f;
	m_tipRect.h = 125.0f;

	IsPractice();
}

void CTutorial::OnLoad()
{
	if (GOLFDOC()->m_tutorialMode == 15)
		return;

	cFile* pFile = g_resrcmng->GetCFile("tutorial_text.txt", -1);

	if (pFile)
	{
		int size = pFile->Length();
		char* pBuffer = new char[size + 1];
		memset(pBuffer, 0, size + 1);
		pFile->Read(pBuffer, size);

		char* p = pBuffer;
		int id;
		char text[1024];

		while (p && strlen(p))
		{
			sscanf(p, "%d", &id);
			p = GetLine(p, text);

			m_textMap[id] = text;
		}

		CloseCFile(pFile);

		delete[] pBuffer;
	}

	GOLFDOC()->m_target.pet =
		g_resrcmng->GetPuppet("camera_target.pet", false, true, false);

	OnImageLoad();
}

void CTutorial::OnProcess(float delta)
{
	if (GOLFDOC()->m_tutorialMode == 15)
		return;

	if (m_repeatKey[0])
	{
		g_input->SetFakeInput(m_repeatKey, m_bKeyPress);
		m_bKeyPress = false;
	}

	if (m_delay >= 0)
	{
		m_delay -= (int)(delta * 1000.0f);
		if (m_delay < 0)
		{
			if (m_pCaddieForm && !m_pSelectButton)
			{
				m_pCaddieForm->Close(true);
			}
			else
			{
				PopMacro();
			}
		}
	}

	if (m_state == 1 && !m_pMissionForm)
	{
		if (!CCapturedBg::Instance()->IsCaptured())
		{
			OpenMissionForm();
		}
	}

	if (m_state == 1 && m_bRepeatTip)
	{
		m_repeatTipTime += (int)(delta * -1000.0f);
		if (m_repeatTipTime < 0)
		{
			m_repeatTipTime = 24000;

			if (GOLFDOC()->m_gameMode & 0x208)
			{
				Again_Msg(false);
			}
		}
		else if (m_repeatTipTime < 15000)
		{
			Again_Msg(true);
		}
	}

	for (int i = 0; i < 26; i++)
	{
		m_overlay[i].Process(delta);
	}

	m_clearOverlay.Process(delta);

	if (m_clearOverlay.IsActivate())
	{
		if (m_clearOverlay.m_rect.y > 20.0f)
			m_clearOverlay.m_rect.y -= 2.0f;

		if (m_clearScale > 1.0f)
			m_clearScale -= 0.004f;

		m_clearOverlay.m_rect.w = m_clearScale * 262.0f;
		m_clearOverlay.m_rect.h = m_clearScale * 44.0f;

		m_clearOverlay.m_rect.x =
			(g_view->GetWidth() - m_clearOverlay.m_rect.w) * 0.5f;
	}

	for (int j = 0; j < 3; j++)
	{
		m_chanceOverlay[j].Process(delta);
	}

	switch (m_state)
	{
	case 1:
	{
		if (GOLFDOC()->m_tutorialMode != 12)
		{
			if (CheckMissionComplete())
				MissionComplate();
		}
	}
	break;

	case 4:
	{
		m_state = 5;
		BlockInput();

		OpenMissionForm();
		OpenQuizForm(m_quizType);
	}
	break;

	case 6:
	{
		if (IsLocalContent(S3_TUTORIAL_RENEWAL))
		{
			if (m_pMissionForm)
			{
				m_pMissionForm->Close(true);
			}

			if (m_bCorrectAnswer)
			{
				MissionComplate();
			}
			else
			{
				HandleMsg(MsgObject(NULL, 0x154, 0, 0, 0, 0, 0));
			}
		}
	}
	break;
	}
}

void CTutorial::OnDisplay()
{
	if (!IsLocalContent(S3_TUTORIAL_RENEWAL))
		return;

	for (int i = 0; i < 26; i++)
		m_overlay[i].Render();

	m_clearOverlay.Render();

	ChanceOverlay();
	for (int j = 0; j < 3; j++)
		m_chanceOverlay[j].Render();

	if (CCapturedBg::Instance()->IsCaptured())
		return;

	if (!m_pMissionImage)
		return;

	m_pMissionImage->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
		WRect(g_view->GetWidth() - m_pMissionImage->GetWidth() - 10.0f, 40.0f,
			m_pMissionImage->GetWidth(), m_pMissionImage->GetHeight()),
		0, 0xffffffff, 0, 0);
}

void CTutorial::OnDestroy()
{
	if (Doc()->m_golfGame.gameType == 11 || Doc()->m_golfGame.gameType == 12)
	{
		SetCurMap(0);
		Doc()->m_golfGame.gameType = 0;
		strcpy(Doc()->m_golfGame.gameTypeName, Doc()->m_gameTypeInfo[0].name);

		g_input->FreeKey();
		g_input->FreeMouse();

		ClearMacro();
		m_textMap.clear();

		if (g_resrcmng && GOLFDOC()->m_target.pet)
		{
			g_resrcmng->Release(GOLFDOC()->m_target.pet, false);
			GOLFDOC()->m_target.pet = NULL;
		}
		if (g_resrcmng && m_pMissionImage)
		{
			g_resrcmng->Release(m_pMissionImage);
			m_pMissionImage = NULL;
		}
	}
}

void CTutorial::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0x148:
		PopMacro();
		break;

	case 0x14b:
		if (!m_pResultForm && !m_bComplete[GOLFDOC()->m_tutorialMode])
		{
			m_bSkipped = true;
		}
		SkipMission();
		break;

	case 0x149:
		if (m_state == 1)
		{
			if (GOLFDOC()->m_tutorialMode == 12)
				CheckMissionComplete();
			else
				MissionComplate();
		}
		else
		{
			PopMacro();
		}
		break;

	case 0x154:
		if (IsLocalContent(S3_TUTORIAL_RENEWAL))
		{
			if (m_pReviewForm)
			{
				m_pReviewForm->Close(FrCANCEL, true);
			}

			BlockInput();

			g_input->BlockKey("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9");

			if (m_state == 1 || m_state == 6)
			{
				m_caddieFace = 4;
				m_selectButton = 2;

				m_state = 7;
				switch (GOLFDOC()->m_tutorialMode)
				{
				case 2:
					m_caddieFace = 1;
					OpenTipCaddieForm(m_textMap[307].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 4:
					if (m_failCount >= 2)
					{
						m_failCount++;
						OpenTipCaddieForm(m_textMap[409].c_str(),
							(FRESH_PFN_RESULT)&CTutorial::
								OnTipCaddieNoneResult);
						OpenMissionFailedForm();
					}
					else
					{
						m_state = 1;
						m_failCount++;
						FreeInput();
						SendMsg(this, "Screen", 442, 1, 0, 0, 0);
					}
					break;

				case 3:
					OpenTipCaddieForm(m_textMap[608].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 5:
					OpenTipCaddieForm(m_textMap[810].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 8:
					SendMsg(this, "Letter", 396, 0, 0, 0, 0);
					OpenTipCaddieForm(m_textMap[907].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 6:
					OpenTipCaddieForm(m_textMap[710].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 7:
					OpenTipCaddieForm(m_textMap[510].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 9:
					OpenTipCaddieForm(m_textMap[1107].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 10:
					OpenTipCaddieForm(m_textMap[1208].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 12:
					while (!m_macroQueue.empty())
					{
						m_macroQueue.pop();
					}
					PushMacroHitPointMission();
					m_hitCount = 0;
					SendMsg(this, "GolfRule", 0x89,
						(int)"\xbd\xc3\xc0\xdb\xc1\xa1",
						(int)"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1\x33", 0, 0);
					PopMacro();

					OpenTipCaddieForm(m_textMap[1309].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 11:
					OpenTipCaddieForm(m_textMap[1010].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 13:
					OpenTipCaddieForm(m_textMap[1413].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;

				case 14:
					OpenTipCaddieForm(m_textMap[1513].c_str(),
						(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					break;
				}
			}
			else
			{
				SendMsg(this, "Screen", 442, 1, 0, 0, 0);
			}
		}
		break;

	case 0x14c:
		ReviewTutorial();
		break;

	case 0x14a:
	{
		float* pos = (float*)msg.param1;
		m_screenX = pos[0];
		m_screenY = pos[1];
	}
	break;

	case 0x152:
		*(unsigned long**)msg.param1 = m_itemTypeId;
		break;

	case 0x155:
		m_gaugePos = (float)msg.param1;

		if (m_gaugePos < GOLFDOC()->m_tutorialPower)
		{
			GOLFDOC()->m_bar.gauge = (float)GOLFDOC()->m_tutorialPower;
			SendMsg(NULL, "GolfRule", 162, 0, 0, 0, 0);
		}
		break;

	case 0x156:
		PopSpecial();
		break;

	case 0x20f:
		switch (msg.param1)
		{
		case 0:
			m_bKeepBlockInput = true;
			g_input->BlockMouse(LEFT_BUTTON);
			BlockInput();
			break;

		case 1:
			m_bKeepBlockInput = false;
			g_input->FreeMouse(LEFT_BUTTON);
			break;

		case 2:
		{
			float gauge = *(float*)msg.param2;
			GOLFDOC()->m_bar.gauge = gauge;
			if (msg.param3 == 1)
			{
				GOLFDOC()->m_bar.dir = 1;
			}
			else if (msg.param3 == 2)
			{
				GOLFDOC()->m_bar.gaugeStep = gauge;
			}
		}
		break;
		}
		break;
	}
}

bool CTutorial::CheckMissionComplete()
{
	switch (GOLFDOC()->m_tutorialMode)
	{
	case 0:
		return CheckCameraTarget();
	case 1:
		return CheckClubTarget();
	case 12:
		m_hitCount++;

		if (m_hitCount == 4)
			MissionComplate();

		BlockInput();
		CMouseCursor::Instance()->SetActive(true);

		if (m_pTipForm)
			m_pTipForm->Close(FrCANCEL, true);

		PopMacro();
		return true;
	}

	return false;
}

void CTutorial::BlockInput()
{
	g_input->BlockKey();

	g_input->FreeKey("ESCAPE");
	g_input->FreeKey("ENTER");
	g_input->FreeKey("PADENTER");
	g_input->FreeKey("FUNC_11");

	CMouseCursor::Instance()->Enable(5, false);

	g_input->BlockMouse(MIDDLE_BUTTON);
	g_input->BlockMouse(RIGHT_BUTTON);

	m_bBlockInput = true;
}

void CTutorial::FreeInput()
{
	g_input->FreeKey();

	g_input->FreeMouse();
	g_input->FreeMouse(LEFT_BUTTON);
	g_input->FreeMouse(MIDDLE_BUTTON);
	g_input->FreeMouse(RIGHT_BUTTON);

	CMouseCursor::Instance()->Enable(5, true);

	m_bBlockInput = false;
}

char* CTutorial::GetLine(char* src, char* dst)
{
	while (*src != '"' && *src)
		src++;

	if (*src == 0)
		return NULL;

	for (src++; *src && *src != '"';)
	{
		if (*src == '\\' && src[1] == 'n')
		{
			*dst++ = '\n';
			src += 2;
		}
		else
		{
			*dst++ = *src++;
		}
	}

	if (*src == 0)
		return NULL;

	src++;
	*dst = 0;

	return src;
}

void CTutorial::IsPractice()
{
	if (CTask* task = AfxGetPreservedTask())
	{
		IActor* actor = task->GetActor("TutorialMain");
		if (actor)
		{
			int a, b, mode, c;
			actor << MsgObject(NULL, 335, (int)&a, (int)&b, (int)&mode,
				(int)&m_tutorialIndex, (int)&c);
			GOLFDOC()->m_tutorialMode = mode;

			m_completeCode = s_tutorialInfo[m_tutorialIndex].code;
		}
	}

	m_pResultForm = NULL;
	CMouseCursor::Instance()->Store();

	for (int i = 0; i < 26; i++)
		m_overlay[i].Activate(false, true, true);

	BlockInput();

	SendMsg(this, "Screen", 444, 2, 0, 0, 0);
	InitTutorialMode(GOLFDOC()->m_tutorialMode);
}

void CTutorial::ReviewTutorial()
{
	if (m_pReviewForm)
		return;

	BlockInput();

	CMouseCursor::Instance()->SetActive(true);
	if (m_pTipForm)
		m_pTipForm->Close(FrCANCEL, true);

	m_state = 3;

	m_pReviewForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
	m_pReviewForm->SetMessage(
		"\xbc\xb3\xb8\xed\xc0\xbb \xb4\xd9\xbd\xc3 \xb5\xe9\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		false);
	m_pReviewForm->EnableDrag(false);
	m_pReviewForm->Open((FRESH_PFN_RESULT)&CTutorial::OnReviewFormResult,
		WPoint((g_view->GetWidth() - m_pReviewForm->GetRect().w) * 0.5f, 80.0f),
		FrENTER);
	m_pReviewForm->MoveCursor("ok");
}

void CTutorial::MoveNextStep()
{
	if (!IsLocalContent(S3_TUTORIAL_RENEWAL))
	{
		if (m_pResultForm)
			return;

		if (m_pMissionForm)
			m_pMissionForm->Close(FrNONE, true);

		if (!m_bSkipped)
			m_bComplete[GOLFDOC()->m_tutorialMode] = true;

		BlockInput();

		CMouseCursor::Instance()->SetActive(true);
		if (m_pTipForm)
			m_pTipForm->Close(FrCANCEL, true);

		m_state = 3;
	}

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		CProjectG::Instance()->FromGolfToLobbySettings();

		int bAllClear = m_completeCode == 0x80;

		if (CTask* task = AfxGetPreservedTask())
		{
			IActor* actor = task->GetActor("TutorialMain");
			if (actor)
				actor << MsgObject(NULL, 336, m_tutorialIndex, m_completeCode,
					1, bAllClear, 0);
		}

		CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
		CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0,
			(int)"TUTORIAL_MAIN", 0, 0, 0);
	}

	if (GOLFDOC()->m_tutorialMode < 15)
	{
		m_pResultForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tutorial", NULL);
		m_pResultForm->SetMessage(
			"\xb9\xcc\xbc\xc7\xc0\xbb \xbf\xcf\xb7\xe1\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. \n\n\xc3\xe0\xc7\xcf\xc7\xd5\xb4\xcf\xb4\xd9! ",
			false);
	}
	else
	{
		m_pResultForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tutorial", NULL);
		m_pResultForm->SetMessage(
			"\xc6\xce\xbe\xdf\xb9\xe8\xbf\xec\xb1\xe2\xb8\xa6 \xb8\xb6\xc4\xa1\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. \xc3\xe0\xc7\xcf\xc7\xd5\xb4\xcf\xb4\xd9.",
			false);
	}

	if (m_pResultForm)
	{
		m_pResultForm->EnableDrag(false);
		m_pResultForm->Open((FRESH_PFN_RESULT)&CTutorial::OnCompleteFormResult,
			WPoint((g_view->GetWidth() - m_pResultForm->GetRect().w) * 0.5f,
				80.0f),
			FrENTER);
		m_pResultForm->MoveCursor("ok");
	}
}

void CTutorial::MissionComplate()
{
	if (m_pResultForm)
		return;

	if (m_pMissionForm)
		m_pMissionForm->Close(FrNONE, true);

	if (!m_bSkipped)
		m_bComplete[GOLFDOC()->m_tutorialMode] = true;

	BlockInput();

	CMouseCursor::Instance()->SetActive(true);
	if (m_pTipForm)
		m_pTipForm->Close(FrCANCEL, true);

	m_state = 3;

	m_clearOverlay.m_rect.w = 393.0f;
	m_clearOverlay.m_rect.h = 66.0f;
	m_clearOverlay.m_rect.x = (g_view->GetWidth() - 393.0f) * 0.5f;
	m_clearOverlay.m_rect.y = (g_view->GetHeight() - 66.0f) * 0.5f;

	m_clearOverlay.m_interval = 0.0f;
	m_clearOverlay.Activate(true, true, true);

	m_bRepeatTip = false;

	PopMacro();
}

void CTutorial::PrepareMission()
{
	m_failCount = 0;

	if (GOLFDOC()->m_tutorialMode == 3)
	{
		GOLFDOC()->GetPlayer(0)->gauge = 33.0f;
	}
	else if (GOLFDOC()->m_tutorialMode == 5)
	{
		m_itemTypeId[0] = 0x18000004;
		m_itemTypeId[1] = 0x18000004;
		m_itemTypeId[2] = 0x18000004;
		GOLFDOC()->GetPlayer(0)->itemNum = 3;
	}
	else if (GOLFDOC()->m_tutorialMode == 8)
	{
		GOLFDOC()->GetPlayer(0)->totalPang = 0;
		GOLFDOC()->GetPlayer(0)->bunker = 0;
		sPlayerData* player = GOLFDOC()->GetPlayer(0);
		player->stroke[GOLFDOC()->m_currentHole - 1] = 1;
		SendMsg(this, "GolfRule", 0x89, (int)"\xbd\xc3\xc0\xdb\xc1\xa1", 0, 0,
			0);
	}

	switch (GOLFDOC()->m_tutorialMode)
	{
	case 2:
	case 3:
	case 5:
	case 11:
	case 13:
	case 14:
		SendMsg(this, "GolfRule", 0x89, (int)"\xbd\xc3\xc0\xdb\xc1\xa1",
			(int)"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1\x33", 0, 0);
		break;
	}
}

void CTutorial::SkipMission()
{
	if (m_pResultForm)
		return;

	BlockInput();

	CMouseCursor::Instance()->SetActive(true);
	if (m_pTipForm)
		m_pTipForm->Close(FrCANCEL, true);

	m_state = 3;

	m_pResultForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
	m_pResultForm->SetMessage(
		"\xc7\xf6\xc0\xe7 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce \xc4\xf9\xbd\xba\xc6\xae\xb8\xa6 \xc6\xf7\xb1\xe2\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		false);
	m_pResultForm->EnableDrag(false);
	m_pResultForm->Open((FRESH_PFN_RESULT)&CTutorial::OnSkipMissionFormResult,
		WPoint((g_view->GetWidth() - m_pResultForm->GetRect().w) * 0.5f, 80.0f),
		FrENTER);
	m_pResultForm->MoveCursor("ok");
}

void CTutorial::OnImageLoad()
{
	char buf[256];

	m_overlay[0].SetSourceOverlay("tuto_point.tga", 4, 1, 0, 1.0f, 1.0f);
	m_clearOverlay.SetSourceOverlay("text_questclear.png", 1, 1, 0, 1.0f, 1.0f);
	m_chanceOverlay[0].SetSourceOverlay("tutorial_icon_chance.png", 1, 1, 0,
		1.0f, 1.0f);
	m_chanceOverlay[1].SetSourceOverlay("tutorial_icon_chance.png", 1, 1, 0,
		1.0f, 1.0f);
	m_chanceOverlay[2].SetSourceOverlay("tutorial_icon_chance.png", 1, 1, 0,
		1.0f, 1.0f);

	for (int i = 1; i < 26; i++)
	{
		if (i < 20)
			sprintf(buf, "tutorial_keyanmaion%02d.png", i);
		else
			sprintf(buf, "tutorial_ground%02d.png", i - 20);

		switch (i)
		{
		case 1:
			m_overlay[1].SetSourceOverlay(buf, 3, 1, 1, 128.0f, 88.0f);
			break;
		case 3:
			m_overlay[3].SetSourceOverlay(buf, 2, 1, 1, 128.0f, 88.0f);
			break;
		case 4:
			m_overlay[4].SetSourceOverlay(buf, 3, 1, 1, 136.0f, 88.0f);
			break;
		case 5:
			m_overlay[5].SetSourceOverlay(buf, 2, 1, 1, 136.0f, 88.0f);
			break;
		case 0:
		case 2:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 16:
		case 17:
		case 18:
		case 19:
			m_overlay[i].SetSourceOverlay(buf, 2, 1, 1, 128.0f, 88.0f);
			break;
		case 12:
			m_overlay[12].SetSourceOverlay(buf, 2, 4, 1, 136.0f, 105.0f);
			break;
		case 13:
		case 14:
			m_overlay[i].SetSourceOverlay(buf, 2, 1, 1, 136.0f, 105.0f);
			break;
		case 15:
			m_overlay[15].SetSourceOverlay(buf, 2, 2, 1, 136.0f, 105.0f);
			break;
		case 20:
		case 21:
		case 22:
			m_overlay[i].SetSourceOverlay(buf, 1, 1, 0, 1.0f, 1.0f);
			break;
		case 23:
			m_overlay[23].SetSourceOverlay(buf, 2, 1, 1, 128.0f, 88.0f);
			break;
		case 24:
			sprintf(buf, "tu_icon_click.tga");
			m_overlay[24].SetSourceOverlay(buf, 1, 1, 0, 1.0f, 1.0f);
			break;
		case 25:
			sprintf(buf, "tu_arrow_click.tga");
			m_overlay[25].SetSourceOverlay(buf, 1, 1, 0, 1.0f, 1.0f);
			break;
		}
	}

	m_overlay[23].SetSourceOverlay("tutorial_preview.png", 1, 1, 0, 1.0f, 1.0f);
}

void CTutorial::Again_Msg(bool bAgain)
{
	if (m_state == 1)
	{
		if (bAgain)
		{
			if (m_pCaddieForm)
				m_pCaddieForm->Close(true);
		}
		else
		{
			m_caddieFace = 1;
			m_selectButton = 0;

			int textIndex = 5;
			switch (GOLFDOC()->m_tutorialMode)
			{
			case 1:
				textIndex = 213;
				break;
			case 2:
				textIndex = 308;
				break;
			case 4:
				textIndex = 410;
				break;
			case 3:
				textIndex = 607;
				break;
			case 5:
				textIndex = 809;
				break;
			case 9:
				textIndex = 1107;
				break;
			case 10:
				textIndex = 1209;
				break;
			case 11:
				textIndex = 1011;
				break;
			case 12:
				textIndex = 1309;
				break;
			case 13:
				textIndex = 1414;
				break;
			case 14:
				textIndex = 1514;
				break;
			}

			if (!m_pCaddieForm)
				OpenTipCaddieForm(m_textMap[textIndex].c_str(),
					(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieNoneResult);

			if (m_pCaddieForm)
				m_pCaddieForm->EnableFlag(FrENTER);

			if (m_pSelectButton)
				m_pSelectButton->Enable(true);

			CMouseCursor::Instance()->ForceMove(g_view->GetWidth() * 0.5f,
				g_view->GetHeight() * 0.5f, -1.0f);
		}
	}
}

void CTutorial::ChanceOverlay()
{
	if (m_state == 1 && GOLFDOC()->m_tutorialMode == 4)
	{
		for (int i = 0; i < 3; i++)
		{
			if (i < 3 - m_failCount)
			{
				m_chanceOverlay[i].m_rect.x =
					g_view->GetWidth() - ((i + 1) * 62.0f) - 20.0f;
				m_chanceOverlay[i].m_rect.y = 160.0f;
				m_chanceOverlay[i].m_rect.w = 58.0f;
				m_chanceOverlay[i].m_rect.h = 53.0f;
				m_chanceOverlay[i].Activate(true, true, true);
			}
			else
			{
				m_chanceOverlay[i].Activate(false, true, true);
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
			m_chanceOverlay[i].Activate(false, true, true);
	}
}

void CTutorial::PriviewOverlay()
{
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 23, (g_view->GetWidth() - 128.0f) * 0.5f,
			(g_view->GetHeight() * 0.25f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 23, 128.0, 64.0, 0.0), 0, 1);
	PushMacro(MACRO_NONE, 0, 2000, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 23), 500, 1);
}

bool CTutorial::OnTipFormResult(int result, FrForm* form)
{
	m_pTipForm = NULL;
	if (result != FrCANCEL)
	{
		if (m_bKeepBlockInput)
		{
			g_input->BlockMouse(LEFT_BUTTON);
		}
		else
		{
			FreeInput();
		}

		CMouseCursor::Instance()->Store();

		if (m_cameraTargetCount == 6)
		{
			m_cameraTargetCount = 0;
			MissionComplate();
		}
		else
		{
			PopMacro();
		}
	}

	return true;
}

bool CTutorial::OnCompleteFormResult(int result, FrForm* form)
{
	m_pResultForm = NULL;
	CMouseCursor::Instance()->Store();

	if (!IsLocalContent(S3_TUTORIAL_RENEWAL))
	{
		if (Doc()->m_golfGame.gameType == 11 || GOLFDOC()->m_tutorialMode == 7)
		{
			if (WNetworkSystem::Instance()->IsConnected(
					WNetworkSystem::NET_GAME))
			{
				CProjectG::Instance()->FromGolfToLobbySettings();

				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"TOPPAGE", 0, 0, 0);
			}
			else
			{
				g_bQuit = true;
			}
		}
		else
		{
			if (m_bSkipped && result == FrOK)
				m_skipCount += result;

			m_bSkipped = false;
		}

		SendMsg(this, "Screen", 444, result == FrOK ? 2 : 1, 0, 0, 0);
		InitTutorialMode(GOLFDOC()->m_tutorialMode + (result == FrOK));
	}
	else
	{
		if (result != FrNONE &&
			!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
			g_bQuit = true;

		return true;
	}

	return true;
}

bool CTutorial::OnReviewFormResult(int result, FrForm* form)
{
	m_pReviewForm = NULL;

	CMouseCursor::Instance()->Store();

	if (result == FrOK)
	{
		SendMsg(this, "Screen", 520, 0, 0, 0, 0);
		SendMsg(this, "Screen", 444, 1, 0, 0, 0);
		InitTutorialMode(GOLFDOC()->m_tutorialMode);
	}
	else if (result == FrCANCEL)
	{
		if (GOLFDOC()->m_tutorialMode == 6 || GOLFDOC()->m_tutorialMode == 7)
		{
			m_state = 4;
			return true;
		}

		FreeInput();

		m_state = 1;

		switch (GOLFDOC()->m_tutorialMode)
		{
		case 2:
		case 3:
		case 4:
		case 5:
		case 8:
		case 10:
		case 11:
		case 13:
		case 14:
			break;
		default:
			OpenMissionForm();
			SendMsg(this, "Screen", 444, 2, 0, 0, 0);
			SendMsg(this, "Screen", 442, 1, 0, 0, 0);
			break;
		}
	}

	return true;
}

bool CTutorial::OnSkipMissionFormResult(int result, FrForm* form)
{
	m_pResultForm = NULL;

	CMouseCursor::Instance()->Store();

	if (m_bSkipped == true && result == FrOK)
	{
		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		{
			CProjectG::Instance()->FromGolfToLobbySettings();
			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0,
				(int)"TUTORIAL_MAIN", 0, 0, 0);
		}
	}
	else
	{
		if (GOLFDOC()->m_tutorialMode == 6 || GOLFDOC()->m_tutorialMode == 7)
		{
			m_state = 4;
			return true;
		}

		m_state = 1;
		FreeInput();
		OpenMissionForm();
		SendMsg(this, "Screen", 444, 2, 0, 0, 0);
		SendMsg(this, "Screen", 442, 1, 0, 0, 0);
	}

	return true;
}

bool CTutorial::OnPracticeFormResult(int result, FrForm* form)
{
	m_pResultForm = NULL;
	CMouseCursor::Instance()->Store();

	if (result == FrOK)
	{
		m_bPractice = true;
	}

	for (int i = 0; i < 26; i++)
	{
		m_overlay[i].Activate(false, true, true);
	}

	BlockInput();

	InitTutorialMode(GOLFDOC()->m_tutorialMode);

	return true;
}

void CTutorial::OpenTipCaddieForm(const char* text,
	bool (FrCmdTarget::*handler)(int, FrForm*))
{
	if (m_pReviewForm)
	{
		m_pReviewForm->Close(FrCANCEL, true);
	}

	if (m_pCaddieForm || !g_view)
		return;

	std::string str(text);
	if (str.size() <= 75)
		m_pCaddieForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip_imageBox");
	else if (str.size() <= 100)
		m_pCaddieForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip_MimageBox");
	else
		m_pCaddieForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip_BimageBox");

	m_pCaddieForm->SetMessage(text, false);
	m_pCaddieForm->EnableDrag(false);

	FrArea* pFace =
		DYNAMIC_CAST(FrArea, m_pCaddieForm->FindChildByName("pipin_face"));
	if (pFace)
	{
		pFace->SetBgImg(s_caddieFace[m_caddieFace]);
	}

	for (unsigned int i = 0; i < 3; i++)
	{
		FrButton* pButton = DYNAMIC_CAST(FrButton,
			m_pCaddieForm->FindChildByName(s_buttonName[i]));
		if (pButton)
		{
			bool bSelect = i == m_selectButton;
			if (bSelect)
			{
				m_pSelectButton = pButton;
			}
			pButton->SetVisible(bSelect);
			pButton->Enable(false);
		}
	}

	m_tipRect.x = (g_view->GetWidth() - m_pCaddieForm->GetRect().w) * 0.5f;
	m_tipRect.y = 50.0f;
	m_tipRect.w = m_pCaddieForm->GetRect().w;
	m_tipRect.h = m_pCaddieForm->GetRect().h;
	m_pCaddieForm->SetKeyEvent(true);
	m_pCaddieForm->Open(handler,
		WPoint((g_view->GetWidth() - m_pCaddieForm->GetRect().w) * 0.5f, 50.0f),
		4);
	m_pCaddieForm->MoveCursor(s_buttonName[m_selectButton]);
}

bool CTutorial::OnTipCaddieFormResult(int result, FrForm* form)
{
	m_pCaddieForm = NULL;
	m_pSelectButton = NULL;
	if (result == FrCANCEL)
		return true;

	if (result == FrNONE)
		return true;

	CMouseCursor::Instance()->Store();

	if (m_cameraTargetCount == 6)
	{
		m_cameraTargetCount = 0;
		MissionComplate();
	}
	else
	{
		PopMacro();
	}

	return true;
}

bool CTutorial::OnTipCaddieReopenQuizResult(int result, FrForm* form)
{
	m_pCaddieForm = NULL;
	m_pSelectButton = NULL;
	m_state = 4;
	return true;
}

bool CTutorial::OnTipCaddieNoneResult(int result, FrForm* form)
{
	m_pCaddieForm = NULL;
	m_pSelectButton = NULL;

	return true;
}

void CTutorial::OpenMissionForm()
{
	m_caddieFace = 1;
	m_selectButton = 2;

	if (!m_pMissionImage)
	{
		if (m_tutorialIndex >= 0 &&
			m_tutorialIndex <
				sizeof(s_missionImage) / sizeof(s_missionImage[0]))
		{
			m_pMissionImage =
				g_resrcmng->GetOverlay(s_missionImage[m_tutorialIndex], 0);
		}
	}

	if (!m_pMissionForm)
	{
		m_pMissionForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tuto_mission2");

		float x = g_view->GetWidth() - m_pMissionForm->GetRect().w - 10.0f;
		float y = 25.0f;
		if (m_pMissionImage)
		{
			x = g_view->GetWidth() - m_pMissionImage->GetWidth() + 20.0f;
			y = m_pMissionImage->GetHeight() + 25.0f -
				s_missionImageOffset[m_tutorialIndex];
		}

		m_pMissionForm->SetFixed(true);
		m_pMissionForm->SetKeyEvent(true);

		m_pMissionForm->Open((FRESH_PFN_RESULT)&CTutorial::OnMissionFormResult,
			WPoint(x, y), 6);
	}

	CMouseCursor::Instance()->ForceMove(g_view->GetWidth() * 0.5f,
		g_view->GetHeight() * 0.5f, -1.0f);
}

bool CTutorial::OnMissionFormResult(int result, FrForm* form)
{
	if (m_pCaddieForm)
	{
		m_pCaddieForm->Close(true);
	}

	if (m_pQuizForm)
	{
		m_pQuizForm->Close(true);
	}

	if (g_resrcmng && m_pMissionImage)
	{
		g_resrcmng->Release(m_pMissionImage);
		m_pMissionImage = NULL;
	}

	m_pMissionForm = NULL;
	if (result == FrOK)
	{
		HandleMsg(MsgObject(NULL, 332, 0, 0, 0, 0, 0));
	}
	else if (result == FrCANCEL)
	{
		HandleMsg(MsgObject(NULL, 331, 0, 0, 0, 0, 0));
	}

	return true;
}

void CTutorial::OpenQuizForm(int type)
{
	if (m_pQuizForm)
		return;

	if (type == 1)
		m_pQuizForm = CreateForm<FrQuizDlg>(g_pFresh->GetManager(), this,
			"tuto_quiz_ground");
	else
		m_pQuizForm = CreateForm<FrQuizDlg>(g_pFresh->GetManager(), this,
			"tuto_quiz_score");

	m_pQuizForm->SetupQuiz(type);

	m_caddieFace = 1;
	m_selectButton = 2;

	m_pQuizForm->SetKeyEvent(true);
	m_pQuizForm->SetFixed(true);
	m_pQuizForm->EnableDrag(false);
	m_pQuizForm->Open((FRESH_PFN_RESULT)&CTutorial::OnQuizFormResult,
		WPoint((g_view->GetWidth() - m_pQuizForm->GetRect().w) * 0.5f,
			(g_view->GetHeight() - m_pQuizForm->GetRect().h) * 0.5f + 10.0f),
		3);
	m_pQuizForm->MoveCursor("cases");

	m_state = 5;
	m_bCorrectAnswer = false;
}

bool CTutorial::OnQuizFormResult(int result, FrForm* form)
{
	if (m_pCaddieForm)
	{
		m_pCaddieForm->Close(true);
	}

	if (m_pQuizForm)
	{
		m_bCorrectAnswer = m_pQuizForm->IsCorrectAnswer();
	}

	m_pQuizForm = NULL;
	m_state = 6;

	if (result == FrCANCEL)
	{
		m_pMissionForm->Close(FrCANCEL, true);
	}

	return true;
}

void CTutorial::OpenMissionFailedForm()
{
	if (m_pResultForm)
		return;

	m_pResultForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");
	m_pResultForm->SetMessage(
		"\xb4\xd9\xbd\xc3 \xb5\xb5\xc0\xfc\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?\n\n(\xbe\xc6\xb4\xcf\xbf\xc0\xb8\xa6 \xb4\xa9\xb8\xa3\xb8\xe9 \xb9\xcc\xbc\xc7\xc0\xbb \xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed\n\xb9\xdb\xc0\xb8\xb7\xce \xb3\xaa\xb0\xa9\xb4\xcf\xb4\xd9)",
		false);
	m_pResultForm->EnableDrag(false);
	m_pResultForm->Open((FRESH_PFN_RESULT)&CTutorial::OnMissionFailedFormResult,
		WPoint((g_view->GetWidth() - m_pResultForm->GetRect().w) * 0.5f,
			180.0f),
		2);
	m_pResultForm->MoveCursor("ok");
}

bool CTutorial::OnMissionFailedFormResult(int result, FrForm* form)
{
	m_pResultForm = NULL;
	CMouseCursor::Instance()->Store();

	if (m_pCaddieForm)
	{
		m_pCaddieForm->Close(true);
	}

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		if (result == FrOK)
		{
			m_state = 1;

			if (GOLFDOC()->m_tutorialMode == 7 ||
				GOLFDOC()->m_tutorialMode == 6)
			{
				m_state = 4;
				return true;
			}

			PrepareMission();
			OpenMissionForm();
			SendMsg(this, "Screen", 444, 2, 0, 0, 0);
			SendMsg(this, "Screen", 442, 1, 0, 0, 0);
			FreeInput();
		}
		else if (result == FrCANCEL)
		{
			m_bRepeatTip = false;

			CProjectG::Instance()->FromGolfToLobbySettings();

			CTask* pTask = AfxGetPreservedTask();
			if (pTask)
			{
				IActor* pActor = pTask->GetActor("TutorialMain");
				if (pActor)
					pActor << MsgObject(NULL, 336, m_tutorialIndex,
						m_completeCode, 0, 0, 0);
			}

			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0,
				(int)"TUTORIAL_MAIN", 0, 0, 0);

			FreeInput();
		}
	}
	else
	{
		SendMsg(this, "Screen", 442, 1, 0, 0, 0);
		FreeInput();
	}

	return true;
}

void CTutorial::PushMacroCamera()
{
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "100", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 444), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "101", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d 1", 472,
			GetPoint("\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
					 "1_1")),
		0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d", 471,
			"\xc4\xb3\xb5\xf0"
			"1_1"),
		0, 1);
	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"1_1",
		0, 1);
	PushMacro(MACRO_MISSION, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "102", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d 1", 472,
			GetPoint("\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
					 "1_2")),
		0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d", 471,
			"\xc4\xb3\xb5\xf0"
			"1_2"),
		0, 1);
	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"1_2",
		-1, 1);
	PushMacro(MACRO_CADDIE_FACE, "4 ok", 0, 1);
	PushMacro(MACRO_TIP, "103", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d 1", 472,
			GetPoint("\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
					 "1_3")),
		0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d", 471,
			"\xc4\xb3\xb5\xf0"
			"1_3"),
		0, 1);
	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"1_3",
		-1, 1);
	PushMacro(MACRO_CADDIE_FACE, "5 ok", 0, 1);
	PushMacro(MACRO_TIP, "104", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d 1", 472,
			GetPoint("\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
					 "1_4")),
		0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d", 471,
			"\xc4\xb3\xb5\xf0"
			"1_4"),
		0, 1);
	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"1_4",
		-1, 1);
	PushMacro(MACRO_CADDIE_FACE, "6 ok", 0, 1);
	PushMacro(MACRO_TIP, "105", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d 1", 472,
			GetPoint("\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
					 "1_5")),
		0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d", 471,
			"\xc4\xb3\xb5\xf0"
			"1_5"),
		0, 1);
	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"1_5",
		-1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "106", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d 1", 472,
			GetPoint("\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
					 "1_6")),
		0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d", 471,
			"\xc4\xb3\xb5\xf0"
			"1_6"),
		0, 1);
	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"1_6",
		-1, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "107", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
}

void CTutorial::PushMacroClub()
{
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "200", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "201", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "202", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "203", -1, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Screen %d %d %d", 454, &m_screenX, &m_screenY), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW_SCREEN,
		MakeStr("%d %.1f %.1f %.1f", 0, 64.0f, 64.0f, 1.0f), 0, 1);
	PushMacro(MACRO_NONE, NULL, 1500, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 30, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "204", -1, 1);
	float w = g_view->GetWidth();
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, (double)((w * 0.5f) - 32.0f),
			(double)((g_view->GetHeight() * 0.48333332f) - 32.0f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 64.0f, 64.0f, 1.0f), 0, 1);
	PushMacro(MACRO_NONE, NULL, 1500, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "205", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "206", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 10,
			(double)((m_tipRect.x + m_tipRect.w) - 128.0f),
			(double)(m_tipRect.y + m_tipRect.h + 20.0f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 10, 128.0f, 88.0f, 0.5f), 0, 1);
	PushMacro(MACRO_KEY, "PAGE_DOWN", 200, 5);
	PushMacro(MACRO_NONE, NULL, 1000, 1);
	PushMacro(MACRO_KEY, "PAGE_UP", 200, 5);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 10), 300, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "207", -1, 1);
	PushMacro(MACRO_NONE, NULL, 30, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 444), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "208", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 9, (double)(m_tipRect.x + m_tipRect.w + 20.0f),
			m_tipRect.y + m_tipRect.h - 88.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 9, 128.0f, 88.0f, 0.5f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_KEY, "NUM_0", 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "209", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 1, m_tipRect.x + m_tipRect.w - 128.0f,
			m_tipRect.y + m_tipRect.h + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 1, 128.0f, 88.0f, 0.5f), 200, 1);
	PushMacro(MACRO_NONE, NULL, 1500, 1);
	PushMacro(MACRO_REPEAT_KEY, "LSHIFT", 0, 1);
	PushMacro(MACRO_KEY, "\xbb\xf3", 1, 50);
	PushMacro(MACRO_NONE, NULL, 800, 1);
	PushMacro(MACRO_KEY, "\xc7\xcf", 1, 50);
	PushMacro(MACRO_STOP_REPEAT_KEY, NULL, 500, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 9), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 1), 300, 1);
	PushMacro(MACRO_KEY, "NUM_0", 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "210", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xc1\xc2", 250, 1);
	PushMacro(MACRO_STOP_REPEAT_KEY, NULL, 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "211", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d %d", 518, 1), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("Caddie %d %d", 471,
			"\xc4\xb3\xb5\xf0"
			"1_6"),
		0, 1);
	PushMacro(MACRO_TARGET, "\xb3\xa1\xc1\xa1", 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, NULL, 20000, 1);
	PushMacro(MACRO_NONE, NULL, 2000, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "212", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_NEXT_STEP, NULL, -1, 1);
}

void CTutorial::PushMacroShot()
{
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "300", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"3",
		0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "301", -1, 1);
	PushMacro(MACRO_NONE, NULL, 1500, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 444), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "302", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 3, (double)(m_tipRect.w + m_tipRect.x - 128.0f),
			(double)(m_tipRect.h + m_tipRect.y + 20.0f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 3, 128.0f, 88.0f, 0.5f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d", 135, (int)s_barEnd - 10,
			(int)s_barStart),
		-1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d %d", 138, &m_screenX), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 1", 136), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "303", -1, 1);
	PushMacro(MACRO_OVERLAY_SHOW_SCREEN,
		MakeStr("%d %.1f %.1f %.1f", 0, 64.0f, 64.0f, 1.0f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 136), 0, 1);
	PushMacro(MACRO_NONE, NULL, -1, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_KEY_PRESS, NULL, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 1", 136), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "304", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 110.0f, g_view->GetHeight() - 80.0f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 64.0f, 64.0f, 0.5f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 3), 0, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 136), 0, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "305", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, NULL, 20000, 1);
	PushMacro(MACRO_NONE, NULL, 2000, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "306", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_NEXT_STEP, NULL, -1, 1);
}

void CTutorial::PushMacroGreen()
{
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "400", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "401", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "402", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 4, m_tipRect.w + m_tipRect.x - 128.0f,
			m_tipRect.h + m_tipRect.y + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 4, 128.0f, 88.0f, 0.5f), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "403", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 4), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Grid %d 1", 135), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 1 %d", 135, 350), -1, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_KEY_PRESS, NULL, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Grid %d 0", 135), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "404", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "405", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Grid %d 1", 519), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "406", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbf\xec", 295, 1);
	PushMacro(MACRO_STOP_REPEAT_KEY, NULL, 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Grid %d 0", 519), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Grid %d 1", 135), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 1 %d", 135, 350), -1, 1);
	PushMacro(MACRO_KEY_PRESS, NULL, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Grid %d 0", 135), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "407", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbf\xec", 100, 1);
	PushMacro(MACRO_STOP_REPEAT_KEY, NULL, 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, NULL, 20000, 1);
	PushMacro(MACRO_NONE, NULL, 2000, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "408", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_NEXT_STEP, NULL, -1, 1);
}

void CTutorial::PushMacroGauge()
{
	PLAYER(GOLFDOC()->m_currentPlayer)->gauge = 30.0f;
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "600", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "601", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 445), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 444), 0, 1);
	PushMacro(MACRO_OVERLAY_MOVE, MakeStr("%d %.1f %.1f", 0, 3.0f, 45.0f), 0,
		1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 76.0f, 22.0f, 0.5f), 0, 1);
	PushMacro(MACRO_NONE, NULL, 500, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "602", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d", 135, (int)s_barEnd - 30,
			(int)s_barStart),
		-1, 1);
	PushMacro(MACRO_KEY_PRESS, NULL, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);
	PushMacro(MACRO_KEY_PRESS, NULL, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1,
		1);
	PushMacro(MACRO_STOP_REPEAT_KEY, NULL, 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "603", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "604", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 5, m_tipRect.w + m_tipRect.x - 128.0f,
			m_tipRect.h + m_tipRect.y + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 5, 128.0f, 88.0f, 0.5f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 5), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 0, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Player %d", 285), 2000, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d", 135, (int)s_barEnd - 5, (int)s_barStart),
		-1, 1);
	PushMacro(MACRO_KEY_PRESS, NULL, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);
	PushMacro(MACRO_KEY_PRESS, NULL, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1,
		1);
	PushMacro(MACRO_STOP_REPEAT_KEY, NULL, 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "605", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, NULL, 20000, 1);
	PushMacro(MACRO_NONE, NULL, 2000, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "606", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_NEXT_STEP, NULL, -1, 1);
}

void CTutorial::PushMacroGround()
{
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "700", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "701", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "702", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 9, (double)(m_tipRect.x + m_tipRect.w - 128.0f),
			(double)(m_tipRect.y + m_tipRect.h + 30.0f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 9, 128.0f, 88.0f, 0.5f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 9), 300, 1);
	PushMacro(MACRO_KEY, "NUM_0", 500, 1);
	PushMacro(MACRO_MOVE_CURSOR,
		MakeStr("%f %f", (double)(g_view->GetWidth() * 0.5f - 50.0f),
			g_view->GetHeight() * 0.5f - 30.0f),
		500, 1);
	PushMacro(MACRO_BUTTON, "0", 500, 1);
	PushMacro(MACRO_MOVE_CURSOR,
		MakeStr("%f %f", g_view->GetWidth() * 0.5f + 50.0f,
			g_view->GetHeight() * 0.5f + 50.0f),
		500, 1);
	PushMacro(MACRO_BUTTON, "0", 500, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "703", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 22, m_tipRect.x + m_tipRect.w - 128.0f,
			m_tipRect.y + m_tipRect.h + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 22, 256.0f, 128.0f, 0.0f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 22), 300, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "704", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 20, m_tipRect.x + m_tipRect.w - 128.0f,
			m_tipRect.y + m_tipRect.h + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 20, 256.0f, 128.0f, 0.0f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 20), 300, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "705", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 21, m_tipRect.x + m_tipRect.w - 128.0f,
			m_tipRect.y + m_tipRect.h + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 21, 256.0f, 128.0f, 0.0f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 21), 300, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "706", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 21, m_tipRect.x + m_tipRect.w - 128.0f,
			m_tipRect.y + m_tipRect.h + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 21, 256.0f, 128.0f, 0.0f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 21), 300, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "707", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_KEY, "NUM_0", 500, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "708", -1, 1);
	PushMacro(MACRO_QUIZ, "1", -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "709", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_NEXT_STEP, NULL, -1, 1);
}

void CTutorial::PushMacroItemUse()
{
	m_itemTypeId[0] = 0x18000004;
	m_itemTypeId[1] = 0x18000004;
	m_itemTypeId[2] = 0x18000004;
	PLAYER(0)->itemNum = 3;
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "800", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "801", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "802", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 6, m_tipRect.w + m_tipRect.x - 128.0f,
			m_tipRect.h + m_tipRect.y + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 6, 128.0f, 88.0f, 0.5f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 6), 300, 1);
	PushMacro(MACRO_KEY, "ITEMSLOT", 0, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "803", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 7, 80.0f, g_view->GetHeight() - 208.0f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 7, 128.0f, 88.0f, 0.5f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 7), 300, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_KEY, "\xbf\xec", 500, 1);
	PushMacro(MACRO_KEY, "\xbf\xec", 500, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "804", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 8, 80.0f, g_view->GetHeight() - 208.0f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 8, 128.0f, 88.0f, 0.5f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 8), 300, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 2000, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "805", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d", 135, (int)s_barEnd - 30,
			(int)s_barStart),
		-1, 1);
	PushMacro(MACRO_KEY_PRESS, NULL, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);
	PushMacro(MACRO_KEY_PRESS, NULL, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);
	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1,
		1);
	PushMacro(MACRO_STOP_REPEAT_KEY, NULL, 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "806", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "807", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, NULL, 20000, 1);
	PushMacro(MACRO_NONE, NULL, 2000, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "808", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_NEXT_STEP, NULL, -1, 1);
}

void CTutorial::PushMacroScore()
{
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "500", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "501", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 446), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "502", -1, 1);
	PushMacro(MACRO_OVERLAY_MOVE, MakeStr("%d %.1f %.1f", 0, 165.0f, 8.0f), 0,
		1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 50.0f, 20.0f, 1.0f), 0, 1);
	PushMacro(MACRO_NONE, NULL, 300, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "503", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "504", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "505", -1, 1);
	PushMacro(MACRO_NONE, NULL, 500, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "506", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 445), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 444), 0, 1);
	PushMacro(MACRO_OVERLAY_MOVE, MakeStr("%d %.1f %.1f", 0, 2.0f, 55.0f), 0,
		1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 70.0f, 38.0f, 0.5f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "507", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "508", -1, 1);
	PushMacro(MACRO_QUIZ, "0", -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "509", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_NEXT_STEP, NULL, -1, 1);
}

void CTutorial::PushMacroHoleIn()
{
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "900", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "901", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "902", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 446), 0, 1);
	PushMacro(MACRO_OVERLAY_MOVE, MakeStr("%d %.1f %.1f", 0, 94.0f, 8.0f), 0,
		1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 130.0f, 20.0f, 1.0f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "903", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 445), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 444), 0, 1);
	PushMacro(MACRO_OVERLAY_MOVE, MakeStr("%d %.1f %.1f", 0, 4.0f, 56.0f), 0,
		1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 65.0f, 40.0f, 1.0f), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 0, 1);
	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "905", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, NULL, -1, 1);
	PushMacro(MACRO_NONE, NULL, 2000, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "906", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, NULL, -1, 1);
	PushMacro(MACRO_NEXT_STEP, NULL, -1, 1);
}

void CTutorial::PushMacroWindReading()
{
	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"3",
		0, 1);
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1000", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "1001", -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 526), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, (double)(g_view->GetWidth() - 105.0f),
			(double)(g_view->GetHeight() - 105.0f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_NONE, 0, 500, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "1002", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1003", -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 496, 1), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 497, 0), 0, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, g_view->GetWidth() - 90.0f,
			(double)(g_view->GetHeight() - 90.0f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 80.0, 80.0, 0.5), 0, 1);
	PushMacro(MACRO_NONE, 0, 500, 1);

	for (int i = 0; i <= 360; i += 5)
		PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 497, i), 100, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1004", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, g_view->GetWidth() - 45.0f,
			g_view->GetHeight() - 45.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 40.0, 40.0, 0.5), 0, 1);

	for (int i = 0; i < 9; i++)
		PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 496, i + 1), 600, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1005", -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 497, 90), 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1006", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d", 135, (int)s_barEnd - 30,
			(int)s_barStart),
		-1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);

	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1,
		1);

	PushMacro(MACRO_STOP_REPEAT_KEY, 0, 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1007", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1008", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, 0, 20000, 1);

	PushMacro(MACRO_NONE, 0, 2000, 1);

	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1009", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_NEXT_STEP, 0, -1, 1);
}

void CTutorial::PushMacroBallMove()
{
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1100", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "1101", -1, 1);

	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbb\xf3", 800, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xc1\xc2", 800, 1);
	PushMacro(MACRO_STOP_REPEAT_KEY, 0, 0, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "1102", -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 525), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, g_view->GetWidth() - 400.0f, 1.0), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 40.0, 30.0, 0.5), 0, 1);
	PushMacro(MACRO_NONE, 0, 500, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_CURSOR_TO_SLOT, MakeStr("unplayable %d", 586), 0, 1);
	PushMacro(MACRO_BLOCK_MOUSE, "0", 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 24, g_view->GetWidth() - 395.0f, 35.0), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 24, 73.0, 33.0, 0.0), 300, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 24, 80.3f, 36.3f, 0.0), 100, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 24, 73.0, 33.0, 0.0), 1000, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 24), 0, 1);

	PushMacro(MACRO_FREE_MOUSE, 0, 0, 1);

	PushMacro(MACRO_BUTTON, "0", 0, 1);
	PushMacro(MACRO_BLOCK_MOUSE, "0", 800, 1);

	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1103", -1, 1);

	PushMacro(MACRO_NONE, 0, 500, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 464, 0), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 464, 1), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 464, 0), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 464, 1), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 464, 0), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 464, 1), 300, 1);

	PushMacro(MACRO_FREE_MOUSE, 0, 0, 1);
	PushMacro(MACRO_BLOCK_MOUSE, "2", 0, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_BLOCK_MOUSE, "0", 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1104", -1, 1);
	PushMacro(MACRO_FREE_MOUSE, 0, 0, 1);
	PushMacro(MACRO_BLOCK_MOUSE, "2", 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 11, m_tipRect.x + m_tipRect.w - 128.0f,
			m_tipRect.y + m_tipRect.h + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 11, 128.0, 88.0, 0.5), 500, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 11), 100, 1);

	PushMacro(MACRO_SET_CURSOR,
		MakeStr("%f %f",
			(double)(g_view->GetWidth() - g_view->GetWidth() * 0.125f),
			(double)(g_view->GetHeight() * 0.5f + 30.0f)),
		0, 1);

	PushMacro(MACRO_BLOCK_MOUSE, "0", 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 24,
			(double)(g_view->GetWidth() - g_view->GetWidth() * 0.125f),
			(double)(g_view->GetHeight() * 0.5f + 45.0f)),
		0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 25,
			g_view->GetWidth() - g_view->GetWidth() * 0.125f - 245.0f,
			g_view->GetHeight() * 0.5f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 25, 195.0, 106.0, 0.0), 0, 1);

	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 24, 73.0, 33.0, 0.0), 300, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 24, 80.3f, 36.3f, 0.0), 100, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 24, 73.0, 33.0, 0.0), 1000, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 24), 0, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 25), 0, 1);

	PushMacro(MACRO_FREE_MOUSE, 0, 100, 1);

	PushMacro(MACRO_REPEAT_KEY, "LCONTROL", 100, 1);
	PushMacro(MACRO_BUTTON, "0", 100, 1);
	PushMacro(MACRO_BUTTON, "0", 300, 1);
	PushMacro(MACRO_STOP_REPEAT_KEY, 0, 500, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1105", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_NONE, 0, 500, 1);

	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d %d", 137, "\xbd\xc3\xc0\xdb\xc1\xa1_Unplayable",
			0),
		0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 525), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, 0, 20000, 1);

	PushMacro(MACRO_NONE, 0, 2000, 1);

	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1106", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_NEXT_STEP, 0, -1, 1);

	PushMacro(MACRO_NEXT_STEP, 0, -1, 1);
}

void CTutorial::PushMacroBallSlope()
{
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1200", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"3",
		0, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1201", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1202", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "1203", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 5.0, (double)(g_view->GetHeight() - 105.0f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_NONE, 0, 1000, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_NONE, 0, 500, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d %d", 137, "Slope_Right", 0),
		0, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1204", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_NONE, 0, 1000, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d", 135, (int)s_barEnd - 30,
			(int)s_barStart + 3),
		-1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);

	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1,
		1);

	PushMacro(MACRO_STOP_REPEAT_KEY, 0, 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 0, 1);

	PushMacro(MACRO_NONE, 0, 500, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d %d", 137, "Slope_Left", 0), 0,
		1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1205", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_NONE, 0, 1000, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d", 135, (int)s_barEnd - 30,
			(int)s_barStart - 5),
		-1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);

	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1,
		1);

	PushMacro(MACRO_STOP_REPEAT_KEY, 0, 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1206", -1, 1);

	PushMacro(MACRO_NONE, 0, 500, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Player %d %d %d %d %d", 343, 5, -1, -1),
		0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d %d", 137, "Slope_Right", 0),
		300, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Player %d %d %d", 284, 0x10000000, 1), 0,
		1);
	PushMacro(MACRO_MISSION, 0, 20000, 1);

	PushMacro(MACRO_NONE, 0, 2000, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1207", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_NEXT_STEP, 0, -1, 1);
}

void CTutorial::PushMacroHitPoint()
{
	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1300", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "1301", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1302", -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 465, 1), 500, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 465, 0), 500, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 465, 1), 500, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 465, 0), 500, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 465, 1), 500, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 5.0, (double)(g_view->GetHeight() - 105.0f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_NONE, 0, 1000, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1303", -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d", 362), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 12,
			(double)(m_tipRect.x + m_tipRect.w - 128.0f),
			(double)(m_tipRect.y + m_tipRect.h + 20.0f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 12, 137.0, 105.0, 1.0), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 363, 2, 3, 1), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 1), 0, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 12), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "1304", -1, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 363, 1, 0, 0), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 0), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 465, 0), 500, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, 0, 0), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, 0, -20), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 13, m_tipRect.x + m_tipRect.w - 128.0f,
			m_tipRect.y + m_tipRect.h + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 13, 137.0, 105.0, 1.0), 0, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 13), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "7 ok", 0, 1);
	PushMacro(MACRO_TIP, "1305", -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, 0, 0), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, 0, 20), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 14, m_tipRect.x + m_tipRect.w - 128.0f,
			m_tipRect.y + m_tipRect.h + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 14, 137.0, 105.0, 1.0), 0, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 14), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1306", -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 0), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 15, m_tipRect.x + m_tipRect.w - 128.0f,
			m_tipRect.y + m_tipRect.h + 20.0f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 15, 137.0, 105.0, 1.0), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, -20, 0), 1000,
		1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, 20, 0), 1000,
		1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 15), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1307", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, 0, 0), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);

	PushMacroHitPointMission();
}

void CTutorial::PushMacroHitPointMission()
{
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 465, 1), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 2, 0, -20), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 2), 0, 1);
	PushMacro(MACRO_MISSION, 0, 20000, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 1), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 2, -20, 0), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 0), 1000, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 2), 0, 1);
	PushMacro(MACRO_MISSION, 0, 20000, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 1), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 2, 0, 20), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 0), 1000, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 2), 0, 1);
	PushMacro(MACRO_MISSION, 0, 20000, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 1), 300, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 2, 20, 0), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 0), 1000, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 2), 0, 1);
	PushMacro(MACRO_MISSION, 0, 20000, 1);

	PushMacro(MACRO_NONE, 0, 2000, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1308", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_NEXT_STEP, 0, -1, 1);
}

void CTutorial::PushMacroPowerSpin()
{
	PushMacro(MACRO_TARGET,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
		"3",
		0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 424), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1400", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1401", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1402", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 423), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 3), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, 0, -20), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1403", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 13, (double)(m_tipRect.x + m_tipRect.w - 128.f),
			(double)(m_tipRect.y + m_tipRect.h + 20.f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 13, 137.f, 105.f, 1.f), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 5.f, (double)(g_view->GetHeight() - 105.f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.f, 103.f, 0.5f), 0, 1);
	PushMacro(MACRO_NONE, 0, 1000, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 13), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_POWER, "310", 0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d ", 135, (int)s_barEnd - 10,
			(int)s_barStart),
		-1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);

	PushMacro(MACRO_SPECIAL, 0, 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1404", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_REPEAT_KEY, "\xbb\xf3", 0, 1);

	PushMacro(MACRO_POWER, "142", 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 162), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_SPECIAL, 0, 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 1", 136), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 110.f,
			(double)(g_view->GetHeight() - 115.f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 123.f, 45.f, 1.f), 0, 1);
	PushMacro(MACRO_NONE, 0, 300, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 16, (double)(m_tipRect.x + m_tipRect.w - 128.f),
			(double)(m_tipRect.y + m_tipRect.h + 20.f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 16, 137.f, 105.f, 1.f), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 50, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1405", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 16), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1406", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 110.f,
			(double)(g_view->GetHeight() - 120.f)),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 120.f, 103.f, 1.f), 0, 1);
	PushMacro(MACRO_NONE, 0, 100, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_POWER, "0", 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 162), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 136), -1, 1);
	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);

	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 300,
		1);

	PushMacro(MACRO_STOP_REPEAT_KEY, 0, -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 100, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 3), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, 0, 20), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1407", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 14, m_tipRect.x + m_tipRect.w - 128.f,
			m_tipRect.y + m_tipRect.h + 20.f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 14, 137.f, 105.f, 1.f), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 5.f, g_view->GetHeight() - 105.f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.f, 103.f, 0.5f), 0, 1);
	PushMacro(MACRO_NONE, 0, 1000, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 14), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_POWER, "310", 0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d ", 135, (int)s_barEnd - 10,
			(int)s_barStart),
		-1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);

	PushMacro(MACRO_SPECIAL, 0, 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1408", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_REPEAT_KEY, "\xc7\xcf", 0, 1);

	PushMacro(MACRO_POWER, "142", 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 162), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_SPECIAL, 0, 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 1", 136), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 110.f, g_view->GetHeight() - 115.f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 123.f, 45.f, 1.f), 0, 1);
	PushMacro(MACRO_NONE, 0, 300, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 17, m_tipRect.x + m_tipRect.w - 128.f,
			m_tipRect.y + m_tipRect.h + 20.f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 17, 137.f, 105.f, 1.f), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 50, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1409", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 17), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1410", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 110.f, g_view->GetHeight() - 120.f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 120.f, 103.f, 1.f), 0, 1);
	PushMacro(MACRO_NONE, 0, 100, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_POWER, "0", 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 162), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 136), -1, 1);
	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);

	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 300,
		1);

	PushMacro(MACRO_STOP_REPEAT_KEY, 0, -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 1000, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 4), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1411", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_STOP_REPEAT_KEY, 0, 0, 1);
	PushMacro(MACRO_NONE, 0, 500, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Player %d %d %d %d %d", 343, -1, 28, -1),
		0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, 0, 20000, 1);

	PushMacro(MACRO_NONE, 0, 2000, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1412", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_NEXT_STEP, 0, -1, 1);
}

void CTutorial::PushMacroPowerCurve()
{
	PushMacro(MACRO_TARGET, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1_curve", 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 424), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "1 ok", 0, 1);
	PushMacro(MACRO_TIP, "1500", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1501", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1502", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_BLOCK_INPUT, "0", 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Player %d %d %d %d %d", 343, -1, -1, 7),
		0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d", 423), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 3), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, 7, 0), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1503", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 15, m_tipRect.x + m_tipRect.w - 128.f,
			m_tipRect.y + m_tipRect.h + 20.f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 15, 137.f, 105.f, 1.f), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 5.f, g_view->GetHeight() - 105.f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.f, 103.f, 0.5f), 0, 1);
	PushMacro(MACRO_NONE, 0, 1000, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 15), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_POWER, "310", 0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d ", 135, (int)s_barEnd - 10,
			(int)s_barStart),
		-1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);

	PushMacro(MACRO_SPECIAL, 0, 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1504", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_REPEAT_KEY, "\xbf\xec", 0, 1);

	PushMacro(MACRO_POWER, "142", 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 162), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_SPECIAL, 0, 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 1", 136), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 110.f, g_view->GetHeight() - 115.f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 123.f, 45.f, 1.f), 0, 1);
	PushMacro(MACRO_NONE, 0, 300, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 18, m_tipRect.x + m_tipRect.w - 128.f,
			m_tipRect.y + m_tipRect.h + 20.f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 18, 137.f, 105.f, 1.f), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 50, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1505", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 18), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1506", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 110.f, g_view->GetHeight() - 120.f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 120.f, 103.f, 1.f), 0, 1);
	PushMacro(MACRO_NONE, 0, 100, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_POWER, "0", 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 162), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 136), -1, 1);
	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);

	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 300,
		1);

	PushMacro(MACRO_STOP_REPEAT_KEY, 0, -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 100, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 3), 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d %d %d", 357, 1, -7, 0), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1507", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 15, m_tipRect.x + m_tipRect.w - 128.f,
			m_tipRect.y + m_tipRect.h + 20.f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 15, 137.f, 105.f, 1.f), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 5.f, g_view->GetHeight() - 105.f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 103.f, 103.f, 0.5f), 0, 1);
	PushMacro(MACRO_NONE, 0, 1000, 1);

	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 15), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_POWER, "310", 0, 1);
	PushMacro(MACRO_SENDMSG,
		MakeStr("GolfRule %d 1 %d %d ", 135, (int)s_barEnd - 10,
			(int)s_barStart),
		-1, 1);

	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", -1, 1);

	PushMacro(MACRO_SPECIAL, 0, 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_CADDIE_FACE, "2 ok", 0, 1);
	PushMacro(MACRO_TIP, "1508", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_REPEAT_KEY, "\xc1\xc2", 0, 1);

	PushMacro(MACRO_POWER, "142", 0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 162), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_SPECIAL, 0, 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 1", 136), 0, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 110.f, g_view->GetHeight() - 115.f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 123.f, 45.f, 1.f), 0, 1);
	PushMacro(MACRO_NONE, 0, 300, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 19, m_tipRect.x + m_tipRect.w - 128.f,
			m_tipRect.y + m_tipRect.h + 20.f),
		0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 19, 137.f, 105.f, 1.f), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 50, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1509", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 19), 300, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1510", -1, 1);

	PushMacro(MACRO_OVERLAY_MOVE,
		MakeStr("%d %.1f %.1f", 0, 110.f, g_view->GetHeight() - 120.f), 0, 1);
	PushMacro(MACRO_OVERLAY_SHOW,
		MakeStr("%d %.1f %.1f %.1f", 0, 120.f, 103.f, 1.f), 0, 1);
	PushMacro(MACRO_NONE, 0, 100, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_OVERLAY_HIDE, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_POWER, "0", 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 162), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d", 163), 0, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 136), -1, 1);
	PushMacro(MACRO_KEY_PRESS, 0, 0, 1);
	PushMacro(MACRO_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 1, 1);

	PushMacro(MACRO_SHOW_CURSOR, "0", 0, 1);
	PushMacro(MACRO_REPEAT_KEY, "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 300,
		1);

	PushMacro(MACRO_STOP_REPEAT_KEY, 0, -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("GolfRule %d 0", 135), 1000, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1511", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Spin %d %d", 361, 4), 0, 1);

	PushMacro(MACRO_STOP_REPEAT_KEY, 0, 0, 1);
	PushMacro(MACRO_NONE, 0, 500, 1);

	PushMacro(MACRO_SENDMSG, MakeStr("Player %d %d %d %d %d", 343, -1, 28, 7),
		0, 1);
	PushMacro(MACRO_SENDMSG, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro(MACRO_MISSION, 0, 20000, 1);

	PushMacro(MACRO_NONE, 0, 2000, 1);

	PushMacro(MACRO_CADDIE_FACE, "3 ok", 0, 1);
	PushMacro(MACRO_TIP, "1512", -1, 1);
	PushMacro(MACRO_ENABLE_SELECT, 0, -1, 1);
	PushMacro(MACRO_NEXT_STEP, 0, -1, 1);
}

void CTutorial::InitTutorialMode(int mode)
{
	char startPoint[32];
	char dropPoint[32];

	ClearMacro();

	GetPVS().ResetHoleData();

	m_caddieFace = 1;
	m_selectButton = 0;
	m_bKeepBlockInput = false;
	m_bKeyPress = false;
	m_state = 7;
	m_delay = -1;
	m_repeatKey[0] = 0;
	startPoint[0] = 0;
	dropPoint[0] = 0;
	m_bRepeatTip = false;
	m_failCount = 0;
	m_hitCount = 0;
	PLAYER(0)->totalPang = 0;
	m_clearScale = 1.5f;
	PLAYER(0)->bunker = 0;
	PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] = 1;
	GOLFDOC()->m_target.bShow = false;

	memset(m_itemTypeId, 0, sizeof(m_itemTypeId));
	PLAYER(0)->itemNum = 0;
	PLAYER(0)->item = 0;
	PLAYER(0)->gauge = 0.0f;

	SendMsg(this, "Screen", 520, 0, 0, 0, 0);
	PriviewOverlay();

	GOLFDOC()->m_tutorialMode = mode;

	switch (GOLFDOC()->m_tutorialMode)
	{
	case 0:
		m_cameraTargetCount = 0;
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		PushMacroCamera();
		break;

	case 1:
		strcpy(startPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"2");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"1_6");
		PushMacroClub();
		break;

	case 2:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroShot();
		break;

	case 6:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroGround();
		break;

	case 3:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroGauge();
		break;

	case 4:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1_\xb1\xd7\xb8\xb0");
		PushMacroGreen();
		break;

	case 5:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroItemUse();
		break;

	case 7:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroScore();
		break;

	case 8:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroHoleIn();
		break;

	case 11:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		Wind().Init(7, 0xcc, 0, 1);
		Wind().SetIntensity(0);
		Wind().SetActive(true);
		PushMacroWindReading();
		break;

	case 9:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1_Unplayable");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroBallMove();
		break;

	case 10:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroBallSlope();
		break;

	case 12:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroHitPoint();
		break;

	case 13:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint,
			"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1"
			"3");
		PushMacroPowerSpin();
		break;

	case 14:
		strcpy(startPoint, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(dropPoint, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1_curve");
		PushMacroPowerCurve();
		break;
	}

	BlockInput();
	GOLFDOC()->m_target.bShow = false;
	CMouseCursor::Instance()->SetActive(false);

	SendMsg(this, "Screen", 442, 0, 0, 0, 0);
	SendMsg(this, "Screen", 443, m_tutorialIndex + 1, 0, 0, 0);

	SendMsg(this, "GolfRule", 137, (int)startPoint,
		dropPoint[0] ? (int)dropPoint : 0, 0, 0);

	CChatMsg::Instance()->ClearHistory();
}

bool CTutorial::CheckCameraTarget()
{
	WVector dir = GOLFDOC()->m_target.pos - g_camera.pivot;
	dir.y += 6.0f;
	float dot =
		g_camera.za.x * dir.x + g_camera.za.y * dir.y + g_camera.za.z * dir.z;

	if (Wabs(dot) < g_EPSILON)
		return false;

	if (dot > 96.0f && dot < 288.0f &&
		Abs(acosf(Between(-1.0f,
			dot / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z),
			1.0f))) < 3.0f * g_DEGTORAD)
	{
		m_cameraTargetCount++;
		PopMacro();
	}
	return false;
}

bool CTutorial::CheckClubTarget()
{
	WVector pos = GolfBall().m_pos +
		(GolfClub().GetRange() * 3.2f) * GOLFDOC()->m_cameraData.mat.za;
	WVector diff = GOLFDOC()->m_target.pos - pos;

	if (sqrtf(diff.x * diff.x + diff.z * diff.z) < 16.0f)
		return true;

	return false;
}

WVector* CTutorial::GetPoint(const char* name)
{
	return &GOLFDOC()->m_pPolySoup->FindPoint(name)->pos;
}

void CTutorial::PushMacro(eMacroType type, const char* param, int delay,
	int count)
{
	sMacro macro;
	macro.type = type;
	macro.delay = delay;
	if (param == NULL)
	{
		macro.param[0] = 0;
	}
	else
	{
		strcpy(macro.param, param);
	}

	for (int i = 0; i < count; i++)
	{
		m_macroQueue.push(macro);
	}
}

void CTutorial::PopSpecial()
{
	if (m_macroQueue.empty())
		return;

	if (m_macroQueue.front().type == MACRO_SPECIAL)
	{
		m_state = 7;
		m_delay = m_macroQueue.front().delay;
		m_macroQueue.pop();

		if (m_macroQueue.empty())
			return;

		if (m_delay == 0)
			PopMacro();
	}
}

void CTutorial::PopMacro()
{
	if (m_macroQueue.empty() || m_pResultForm)
		return;

	m_state = 7;
	m_delay = m_macroQueue.front().delay;

	if (m_macroQueue.front().type == MACRO_SPECIAL)
		return;

	switch (m_macroQueue.front().type)
	{
	case MACRO_KEY:
		g_input->SetFakeInput(m_macroQueue.front().param, m_bKeyPress);
		m_bKeyPress = false;
		break;

	case MACRO_REPEAT_KEY:
		strcpy(m_repeatKey, m_macroQueue.front().param);
		g_input->SetFakeInput(m_repeatKey, m_bKeyPress);
		m_bKeyPress = false;
		break;

	case MACRO_STOP_REPEAT_KEY:
		m_repeatKey[0] = 0;
		break;

	case MACRO_BUTTON:
	{
		eButton button;
		sscanf(m_macroQueue.front().param, "%d", &button);
		g_input->SetFakeInput(button);
		break;
	}

	case MACRO_MOVE_CURSOR:
	{
		float x, y;
		sscanf(m_macroQueue.front().param, "%f %f", &x, &y);
		CMouseCursor::Instance()->ForceMove(x, y, m_delay * 0.001f);
		break;
	}

	case MACRO_SET_CURSOR:
	{
		float x, y;
		sscanf(m_macroQueue.front().param, "%f %f", &x, &y);
		CMouseCursor::Instance()->ForceMove(x, y, -1.0f);
		break;
	}

	case MACRO_CURSOR_TO_SLOT:
	{
		char name[32] = "";
		sscanf(m_macroQueue.front().param, "%s", name);
		FrWnd* pWnd = NULL;
		SendMsg(this, "GolfRule", 586, (int)name, (int)&pWnd, 0, 0);
		if (pWnd)
		{
			CMouseCursor::Instance()->ForceMove(pWnd->GetRect().x + 5.0f,
				pWnd->GetRect().y + 5.0f, -1.0f);
		}
		break;
	}

	case MACRO_MISSION:
		if (m_delay > 0)
		{
			m_bRepeatTip = true;
			m_repeatTipTime = m_delay + 4000;
			m_delay = -1;
		}
		m_state = 1;
		SendMsg(this, "Screen", 442, 1, 0, 0, 0);
		PrepareMission();
		OpenMissionForm();
		break;

	case MACRO_TIP:
	{
		m_state = 0;
		CMouseCursor::Instance()->SetActive(true);
		int index;
		sscanf(m_macroQueue.front().param, "%d", &index);
		if (IsLocalContent(S3_TUTORIAL_RENEWAL))
		{
			m_delay = 0;
			OpenTipCaddieForm(m_textMap[index].c_str(),
				(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieFormResult);
			break;
		}
		m_pTipForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip", NULL);
		m_pTipForm->SetMessage(m_textMap[index].c_str(), false);
		m_pTipForm->EnableDrag(false);
		m_pTipForm->Open((FRESH_PFN_RESULT)&CTutorial::OnTipFormResult,
			WPoint((g_view->GetWidth() - m_pTipForm->GetRect().w) * 0.5f,
				80.0f),
			FrENTER | FrMODALESS);
		m_pTipForm->MoveCursor("ok");

		FrWnd* pWnd = m_pTipForm->FindChildByName("harrow");
		if (pWnd)
			pWnd->SetVisible(false);

		pWnd = m_pTipForm->FindChildByName("varrow");
		if (pWnd)
			pWnd->SetVisible(false);
		break;
	}

	case MACRO_TIP_CADDIE:
	{
		m_state = 0;
		CMouseCursor::Instance()->SetActive(true);
		std::string text;
		int index;
		float x, y, ax, ay;
		sscanf(m_macroQueue.front().param, "%d %f %f %f %f", &index, &x, &y,
			&ax, &ay);
		m_delay = 0;
		OpenTipCaddieForm(m_textMap[index].c_str(),
			(FRESH_PFN_RESULT)&CTutorial::OnTipCaddieFormResult);
		break;
	}

	case MACRO_TIP_ARROW:
	{
		m_state = 0;
		CMouseCursor::Instance()->SetActive(true);
		int index;
		float x, y, ax, ay;
		sscanf(m_macroQueue.front().param, "%d %f %f %f %f", &index, &x, &y,
			&ax, &ay);
		m_pTipForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip", NULL);
		m_pTipForm->SetMessage(m_textMap[index].c_str(), false);
		m_pTipForm->EnableDrag(false);
		m_pTipForm->Open((FRESH_PFN_RESULT)&CTutorial::OnTipFormResult,
			WPoint(x, y), FrENTER | FrMODALESS);
		m_pTipForm->MoveCursor("ok");

		FrWnd* pWnd = m_pTipForm->FindChildByName("harrow");
		if (pWnd)
			pWnd->SetClientRect(WRect(ax, ay, 19.0f, 14.0f));

		pWnd = m_pTipForm->FindChildByName("varrow");
		if (pWnd)
			pWnd->SetVisible(false);
		break;
	}

	case MACRO_WAIT_INPUT:
		m_state = 0;
		CMouseCursor::Instance()->SetActive(true);
		break;

	case MACRO_TIP_IMAGE:
	{
		m_state = 0;
		CMouseCursor::Instance()->SetActive(true);
		int index, type;
		float width, height;
		sscanf(m_macroQueue.front().param, "%d %f %f %d", &index, &width,
			&height, &type);
		m_pTipForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip_img", NULL);
		m_pTipForm->EnableDrag(false);
		m_pTipForm->SetRect(
			WRect((g_view->GetWidth() - width) * 0.5f, 200.0f, width, height));

		FrWnd* pOk = m_pTipForm->FindChildByName("ok");
		if (pOk)
		{
			WRect rect;
			m_pTipForm->GetClientRect(rect);
			pOk->MoveWindow(
				WPoint(rect.x + rect.w - 51.0f, rect.y + rect.h - 25.0f));
		}

		switch (type)
		{
		case 0:
		{
			FrWnd* pWnd = m_pTipForm->FindChildByName("bong");
			if (pWnd)
				pWnd->SetVisible(false);

			pWnd = m_pTipForm->FindChildByName("message");
			if (pWnd)
				pWnd->SetClientRect(WRect(10.0f, 30.0f, 273.0f, 102.0f));
			break;
		}

		case 1:
		{
			FrWnd* pWnd = m_pTipForm->FindChildByName("score");
			if (pWnd)
				pWnd->SetVisible(false);

			pWnd = m_pTipForm->FindChildByName("message");
			if (pWnd)
				pWnd->SetClientRect(WRect(65.0f, 55.0f, 218.0f, 80.0f));
			break;
		}
		}

		m_pTipForm->SetMessage(m_textMap[index].c_str(), false);
		m_pTipForm->Open((FRESH_PFN_RESULT)&CTutorial::OnTipFormResult,
			FrENTER | FrMODALESS);
		m_pTipForm->MoveCursor("ok");
		break;
	}

	case MACRO_CADDIE_FACE:
	{
		char button[16];
		sscanf(m_macroQueue.front().param, "%d %s", &m_caddieFace, button);

		if (m_caddieFace >= sizeof(s_caddieFace) / sizeof(s_caddieFace[0]))
			m_caddieFace = 1;

		m_selectButton = 0;
		for (unsigned int i = 0;
			i < sizeof(s_buttonName) / sizeof(s_buttonName[0]); i++)
		{
			if (stricmp(s_buttonName[i], button) == 0)
			{
				m_selectButton = i;
				break;
			}
		}
		break;
	}

	case MACRO_ENABLE_SELECT:
		if (m_pSelectButton && m_pCaddieForm)
		{
			m_pSelectButton->Enable(true);
			m_pCaddieForm->EnableFlag(FrENTER);

			if (m_bKeepBlockInput)
				g_input->FreeMouse(LEFT_BUTTON);
		}
		break;

	case MACRO_OVERLAY_SHOW:
	{
		int index = 0;
		float width, height, interval;
		sscanf(m_macroQueue.front().param, "%d %f %f %f", &index, &width,
			&height, &interval);
		m_overlay[index].m_rect.w = width;
		m_overlay[index].m_rect.h = height;
		m_overlay[index].m_interval = interval;
		m_overlay[index].Activate(true, true, true);
		break;
	}

	case MACRO_OVERLAY_SHOW_SCREEN:
	{
		int index = 0;
		float width, height, interval;
		sscanf(m_macroQueue.front().param, "%d %f %f %f", &index, &width,
			&height, &interval);
		m_overlay[index].m_rect.w = width;
		m_overlay[index].m_rect.h = height;
		m_overlay[index].m_interval = interval;
		m_overlay[index].m_rect.x = m_screenX - width * 0.5f;
		m_overlay[index].m_rect.y = m_screenY - height * 0.5f;
		m_overlay[index].Activate(true, true, true);
		break;
	}

	case MACRO_OVERLAY_HIDE:
	{
		int index = 0;
		sscanf(m_macroQueue.front().param, "%d", &index);
		m_overlay[index].Activate(false, true, true);
		break;
	}

	case MACRO_OVERLAY_MOVE:
	{
		int index = 0;
		float x, y;
		sscanf(m_macroQueue.front().param, "%d %f %f", &index, &x, &y);
		m_overlay[index].m_rect.x = x;
		m_overlay[index].m_rect.y = y;
		break;
	}

	case MACRO_QUIZ:
		m_state = 4;
		sscanf(m_macroQueue.front().param, "%d", &m_quizType);
		break;

	case MACRO_NEXT_STEP:
		MoveNextStep();
		break;

	case MACRO_TARGET:
	{
		m_state = 1;
		GOLFDOC()->m_target.bShow = true;
		WPolySoup::sPoint* pPoint =
			GOLFDOC()->m_pPolySoup->FindPoint(m_macroQueue.front().param);
		GOLFDOC()->m_target.pos =
			GetPVS().GetGroundPoint(pPoint->pos, false, NULL, true);
		break;
	}

	case MACRO_SENDMSG:
	{
		char name[32];
		int type;
		int param[3] = { 0, 0, 0 };
		sscanf(m_macroQueue.front().param, "%s %d %d %d %d", name, &type,
			&param[0], &param[1], &param[2]);
		SendMsg(this, name, type, param[0], param[1], param[2], 0);
		break;
	}

	case MACRO_SHOW_CURSOR:
	{
		int show;
		sscanf(m_macroQueue.front().param, "%d", &show);
		CMouseCursor::Instance()->SetActive(show != 0);
		break;
	}

	case MACRO_BLOCK_INPUT:
	{
		int block;
		sscanf(m_macroQueue.front().param, "%d", &block);
		if (block)
		{
			g_input->BlockMouse(LEFT_BUTTON);
			g_input->BlockMouse();
		}
		BlockInput();
		break;
	}

	case MACRO_FREE_INPUT:
		FreeInput();
		break;

	case MACRO_BLOCK_MOUSE:
	{
		int button;
		sscanf(m_macroQueue.front().param, "%d", &button);
		switch (button)
		{
		case 0:
			g_input->BlockMouse();
			break;

		case 1:
			g_input->BlockMouse(LEFT_BUTTON);
			break;

		case 2:
			g_input->BlockMouse(RIGHT_BUTTON);
			break;
		}
		break;
	}

	case MACRO_FREE_MOUSE:
		g_input->FreeMouse();
		break;

	case MACRO_POWER:
		sscanf(m_macroQueue.front().param, "%d", &GOLFDOC()->m_tutorialPower);
		break;

	case MACRO_KEY_PRESS:
		m_bKeyPress = true;
		break;
	}

	m_macroQueue.pop();

	if (GOLFDOC()->m_tutorialMode == 7 && m_macroQueue.empty())
		return;

	if (m_state == 0)
		BlockInput();
	else if (m_state == 1)
		FreeInput();
	else if (m_state == 5)
		BlockInput();

	if (m_delay == 0)
		PopMacro();
}

void CTutorial::ClearMacro()
{
	while (!m_macroQueue.empty())
	{
		m_macroQueue.pop();
	}
}
