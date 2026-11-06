#include "stdafx.h"

#include "gui_control.h"
#include "fredit.h"
#include "objectfactory.h"
#include "actor.h"
#include "club.h"
#include "fontinfo.h"
#include "wresrcmng.h"
#include "wview.h"
#include "mousecursor.h"
#include "simpleoverlay.h"
#include "golfdoc.h"
#include "mathconsts.h"
#include "shareddoc.h"
#include "chatmsg.h"
#include "frbutton.h"
#include "fresh.h"
extern Fresh* g_pFresh;

#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "reportdlg.h"
#include "../../shared/classdefine.h"
#include "packet.h"
#include "taskmanager.h"
#include "capturedbg.h"
#include "inputmanager.h"
#include "quitwindow.h"
#include "autotarget.h"
#include "golfball.h"
#include "wind.h"
#include "../../shared/sharedtables.h"
#include "s5/utilities.h"
#include "skinnedtheme.h"
#include "s5/classicserver/classicserver.h"
#include "commonutil.h"
#include "fxinterpreter.h"
#include "wfont.h"
#include "clientsetting.h"
#include "powergauge.h"
#include "quadtree.h"
#include "ground_info.h"
#include "helpmsg.h"
#include "headicon.h"
#include "itemwindow.h"
#include "wpuppet.h"
#include "projectg.h"
#include "soundmanager.h"
#include "user_info.h"
#include "trainingoptiondlg.h"

extern WMatrix g_camera;

namespace S5
{
	bool IsUsableCalipers();
}

float Random(float min, float max, int seed);

IMPLEMENT_ACTOR(CScreen, IActor)

CScreen::CScreen(void)
	: m_pWindPuppet(NULL), m_overlays(0x20, 0x20), m_pFont(NULL)
{
	m_pFontOverlay = NULL;
	m_pBase = NULL;
	m_pWindInfo = NULL;
	m_pRemainTimeInfo = NULL;
	m_pClubTypeInfo = NULL;
	m_pMiniScore = NULL;
	m_pAuxMenuUI = NULL;
	m_pAccuracyInfo = NULL;
	m_pPangInfo = NULL;
	m_pPowerInfo = NULL;

	m_pCammodeUI = NULL;
	m_pSimpleUI21C = NULL;
	m_pSimpleUI220 = NULL;
	m_pTutoTitleUI = NULL;

	m_pTitleFont = NULL;
	m_pMissionUI = NULL;

	for (int i = 0; i < 12; i++)
	{
		m_pTutorialUIs[i] = NULL;
	}
	m_missionTitleTime = 0;
	m_missionTitleAlpha = 1.0f;
	m_pCommandArrowUI = NULL;
	m_pTutoEscMsgUI = NULL;
	m_pCalipers = NULL;
	m_pHolePoseSkins = NULL;
	m_pExtraHoleInfo = NULL;
	m_clubBoundingBox = -1;
	m_pGuildInfo = NULL;
	m_dispPriority = (eDispPriority)4;
	AddSkipList(0x400);
}

void CScreen::OnDestroy(void)
{
	if (m_pFontOverlay)
	{
		delete m_pFontOverlay;
		m_pFontOverlay = NULL;
	}

	if (m_pBase)
	{
		delete m_pBase;
		m_pBase = NULL;
	}

	if (m_pWindInfo)
	{
		delete m_pWindInfo;
		m_pWindInfo = NULL;
	}

	if (m_pRemainTimeInfo)
	{
		delete m_pRemainTimeInfo;
		m_pRemainTimeInfo = NULL;
	}

	if (m_pClubTypeInfo)
	{
		delete m_pClubTypeInfo;
		m_pClubTypeInfo = NULL;
	}

	if (m_pMiniScore)
	{
		delete m_pMiniScore;
		m_pMiniScore = NULL;
	}

	if (m_pAuxMenuUI)
	{
		delete m_pAuxMenuUI;
		m_pAuxMenuUI = NULL;
	}

	if (m_pAccuracyInfo)
	{
		delete m_pAccuracyInfo;
		m_pAccuracyInfo = NULL;
	}

	if (m_pPangInfo)
	{
		delete m_pPangInfo;
		m_pPangInfo = NULL;
	}

	if (m_pPowerInfo)
	{
		delete m_pPowerInfo;
		m_pPowerInfo = NULL;
	}

	if (m_pCammodeUI)
	{
		delete m_pCammodeUI;
		m_pCammodeUI = NULL;
	}

	if (m_pSimpleUI21C)
	{
		delete m_pSimpleUI21C;
		m_pSimpleUI21C = NULL;
	}

	if (m_pSimpleUI220)
	{
		delete m_pSimpleUI220;
		m_pSimpleUI220 = NULL;
	}

	if (m_pTutoTitleUI)
	{
		delete m_pTutoTitleUI;
		m_pTutoTitleUI = NULL;
	}

	for (int i = 0; i < 12; i++)
	{
		if (m_pTutorialUIs[i])
		{
			delete m_pTutorialUIs[i];
			m_pTutorialUIs[i] = NULL;
		}
	}

	if (m_pMissionUI)
	{
		delete m_pMissionUI;
		m_pMissionUI = NULL;
	}

	if (m_pTitleFont)
	{
		m_pTitleFont->Erase();
		if (g_resrcmng && m_pTitleFont)
		{
			g_resrcmng->Release(m_pTitleFont);
			m_pTitleFont = 0;
		}
	}

	if (m_pCommandArrowUI)
	{
		delete m_pCommandArrowUI;
		m_pCommandArrowUI = NULL;
	}

	if (m_pCalipers)
	{
		delete m_pCalipers;
		m_pCalipers = NULL;
	}

	if (m_pHolePoseSkins)
	{
		delete m_pHolePoseSkins;
		m_pHolePoseSkins = NULL;
	}

	if (m_pExtraHoleInfo)
	{
		delete m_pExtraHoleInfo;
		m_pExtraHoleInfo = NULL;
	}

	if (m_pTutoEscMsgUI)
	{
		delete m_pTutoEscMsgUI;
		m_pTutoEscMsgUI = NULL;
	}

	if (m_pGuildInfo)
	{
		delete m_pGuildInfo;
		m_pGuildInfo = NULL;
	}

	delete WSingleton<CHelpMsg>::Instance();

	if (g_resrcmng && m_pWindPuppet)
	{
		g_resrcmng->Release(m_pWindPuppet, false);
		m_pWindPuppet = 0;
	}

	for (sOverlay* pOverlay = m_overlays.Start(); pOverlay;
		pOverlay = m_overlays.Next())
	{
		delete pOverlay;
	}

	m_overlays.Reset();

	GetFxInterpreter()->DetachFx(m_clubBoundingBox);
}

void CScreen::OnPreLoadInit(void)
{
	m_unknown3E1 = true;
	m_unknown3E3 = false;
	m_unknown3E2 = false;
}

void CScreen::OnLoad(void)
{
	AddOverlay("\xb9\xd9\xc5\xc1\xbf\xf8", "[left_back.jpg", 0);
	AddOverlay("\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3\xbf\xf8", "[left_time.jpg", 0);
	AddOverlay("\xc1\xa4\xba\xb8\xc7\xa5\xbd\xc3\xbf\xf8", "[left_ht.jpg", 0);
	AddOverlay("\xba\xbc\xbb\xf3\xc5\xc2", "[ball_con.jpg", 0);
	AddOverlay("\xbf\xde\xc2\xca\xb0\xd4\xc0\xcc\xc1\xf6", "middle_mblue.jpg",
		0);
	AddOverlay("\xc6\xc4\xbf\xf6\xb0\xd4\xc0\xcc\xc1\xf6", "middle_lblue.jpg",
		0);
	AddOverlay("\xc3\xbc\xb7\xc2\xb0\xd4\xc0\xcc\xc1\xf6", "middle_dblue.jpg",
		0);
	AddOverlay("\xb4\xab\xb1\xdd_\xb5\xe5\xb6\xf3\xc0\xcc\xba\xea",
		"[middle.jpg", 0);
	AddOverlay(
		"\xb4\xab\xb1\xdd_\xb5\xe5\xb6\xf3\xc0\xcc\xba\xea_\xb0\xc5\xb2\xd9\xb7\xce",
		"[middle.jpg", 0);
	AddOverlay("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3"
			   "10",
		"[middle-p.jpg", 0);
	AddOverlay("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3"
			   "15",
		"[middle-p15.jpg", 0);
	AddOverlay("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3"
			   "20",
		"[middle-p20.jpg", 0);
	AddOverlay("\xc0\xd4\xc3\xbc\xb9\xd9", "middle_bar.jpg", 0);
	AddOverlay("\xb0\xd4\xc0\xcc\xc1\xf6\xc1\xa4\xc1\xf6", "[left_play_0.jpg",
		0x80000000);
	AddOverlay("\xb0\xd4\xc0\xcc\xc1\xf6\xc5\xac\xb8\xaf", "[left_play_1.jpg",
		0x80000000);
	AddOverlay("\xb0\xd4\xc0\xcc\xc1\xf6\xc0\xcc\xb5\xbf", "[left_play_4.jpg",
		0x80000000);
	AddOverlay("\xc5\xb8\xc0\xcc\xb9\xd6\xbf\xb5\xbf\xaa", "middle_rblue.jpg",
		0);
	AddOverlay("\xbd\xba\xc7\xc9\xc6\xf7\xc0\xce\xc6\xae",
		"[left_ballpoint.jpg", 0);
	AddOverlay("\xbd\xba\xc7\xc9\xc6\xf7\xc0\xce\xc6\xae_Red",
		"[left_ballpoint_red.png", 0);

	if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
	{
		AddOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xbe\xe0",
			"classic_green.tga", 0);
		AddOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xc1\xdf",
			"classic_yellow.tga", 0);
		AddOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xb0\xad",
			"classic_red.tga", 0);
	}

	_ISkinnedTheme* pTheme = S5::THEME::GetCurrentSkinnedTheme();
	AddOverlay("\xb9\xd9\xb6\xf7\xc5\xab\xc3\xa2",
		pTheme->GetWindOutterCircleImgName(), 0);
	AddOverlay("\xb9\xd9\xb6\xf7\xc0\xdb\xc0\xba\xc3\xa2",
		pTheme->GetWindInnerCircleImgName(), 0);

	AddOverlay("\xbd\xc3\xc0\xdb\xc1\xa1", "middle_blue.jpg", 0);

	AddOverlay("\xc8\xa6\xc4\xc5\xb1\xb8\xb0\xa3", "hole_position.jpg", 0);
	AddOverlay("\xc5\xac\xb7\xb4\xb9\xd9", "{middle_club.png", 0);
	AddOverlay("\xc5\xbe\xba\xe4\xb5\xb5\xbf\xf2\xb8\xbb", "[camera_top.jpg",
		0);

	AddOverlay("\xc1\xb6\xc1\xbe\xba\xe4\xb5\xb5\xbf\xf2\xb8\xbb",
		"[camera_free.jpg", 0);
	AddOverlay("\xb3\xab\xc7\xcf\xba\xe4\xb5\xb5\xbf\xf2\xb8\xbb",
		"[camera_target.jpg", 0);

	AddOverlay("\xc5\xac\xb8\xaf\xc7\xa5\xbd\xc3", "[click.png", 0);

	if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 10)
	{
		AddOverlay("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3_border",
			"icon_time.png", 0);
		AddOverlay("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3_base",
			"icon_time_02.png", 0);
		AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1\xc1\xa4\xba\xb8",
			"[ground_text_02.jpg", 0);

		AddOverlay("\xb9\xcc\xbc\xc7\xb9\xda\xbd\xba\xc7\xc1\xb7\xb9\xc0\xd3"
				   "1",
			"treasurebox_frm1.tga", 0);
		AddOverlay("\xb9\xcc\xbc\xc7\xb9\xda\xbd\xba\xc7\xc1\xb7\xb9\xc0\xd3"
				   "2",
			"treasurebox_frm2.png", 0);

		switch (GetCurMap())
		{
		case 2:
		case 3:
		case 11:
		case 16:
		case 19:
			AddOverlay("\xc6\xae\xb7\xb9\xc1\xae\xb9\xda\xbd\xba",
				"treasurebox_a.png", 0);
			break;
		case 0:
		case 1:
		case 6:
		case 7:
			AddOverlay("\xc6\xae\xb7\xb9\xc1\xae\xb9\xda\xbd\xba",
				"treasurebox_b.png", 0);
			break;
		case 10:
		case 13:
		case 15:
		case 17:
			AddOverlay("\xc6\xae\xb7\xb9\xc1\xae\xb9\xda\xbd\xba",
				"treasurebox_c.png", 0);
			break;
		case 4:
		case 5:
		case 8:
		case 9:
		case 14:
			AddOverlay("\xc6\xae\xb7\xb9\xc1\xae\xb9\xda\xbd\xba",
				"treasurebox_d.png", 0);
			break;
		}
	}
	else
	{
		AddOverlay("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3",
			"timer.png", 0);
	}

	m_pCammodeUI = new CSimpleUI("[camera_mode.jpg");
	m_pCammodeUI->Add("FORWARD_OVER", WRect(0.0f, 0.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("FORWARD_CLICK", WRect(0.0f, 85.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("BACKWARD_OVER", WRect(85.0f, 0.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("BACKWARD_CLICK", WRect(85.0f, 85.0f, 85.0f, 85.0f),
		false);
	m_pCammodeUI->Add("LEFT_OVER", WRect(0.0f, 170.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("LEFT_CLICK", WRect(0.0f, 255.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("RIGHT_OVER", WRect(85.0f, 170.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("RIGHT_CLICK", WRect(85.0f, 255.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("UP_OVER", WRect(0.0f, 340.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("UP_CLICK", WRect(0.0f, 425.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("DOWN_OVER", WRect(85.0f, 340.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("DOWN_CLICK", WRect(85.0f, 425.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("NORMAL", WRect(170.0f, 0.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("INACTIVE", WRect(170.0f, 85.0f, 85.0f, 85.0f), false);

	m_pSimpleUI21C = new CSimpleUI("[club_line.jpg");
	m_pSimpleUI21C->Add("1W", WRect(0.0f, 0.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("2W", WRect(0.0f, 44.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("3W", WRect(0.0f, 88.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("2I", WRect(0.0f, 132.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("3I", WRect(0.0f, 176.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("4I", WRect(0.0f, 220.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("5I", WRect(0.0f, 264.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("6I", WRect(256.0f, 264.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("7I", WRect(0.0f, 326.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("8I", WRect(256.0f, 326.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("9I", WRect(0.0f, 388.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("PW", WRect(256.0f, 388.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("SW", WRect(0.0f, 450.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("1PT", WRect(256.0f, 450.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("2PT", WRect(256.0f, 450.0f, 256.0f, 62.0f), false);

	m_pCommandArrowUI = new CSimpleUI("keys.png");
	m_pCommandArrowUI->Add("DOWN_S", WRect(0.0f, 0.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("LEFT_S", WRect(20.0f, 0.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("RIGHT_S", WRect(40.0f, 0.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("UP_S", WRect(0.0f, 20.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("DOWN", WRect(20.0f, 20.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("LEFT", WRect(40.0f, 20.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("RIGHT", WRect(0.0f, 40.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("UP", WRect(20.0f, 40.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("INVALID", WRect(40.0f, 40.0f, 20.0f, 20.0f), false);

	m_pSimpleUI220 = new CSimpleUI("[icon01.jpg");
	m_pSimpleUI220->Add("\xc7\xf6\xc0\xe7\xc5\xac\xb7\xb4",
		WRect(288.0f, 0.0f, 48.0f, 48.0f), false);

	if (GOLFDOC()->m_tutorialMode < 15)
	{
		CSimpleUI** ppTutorialUI = m_pTutorialUIs;

		bool wideTitle[12] = { true, true, false, true, true, true, false,
			false, false, true, false, false };

		for (int i = 0; i < 12; i++)
		{
			CSimpleUI* pUI =
				new CSimpleUI(MakeStr("quest_message_%d.png", i + 1));

			*ppTutorialUI = pUI;
			if (wideTitle[i] == true)
			{
				pUI->Add("MissionTitle", WRect(0.0f, 0.0f, 256.0f, 16.0f),
					false);
			}
			else
			{
				pUI->Add("MissionTitle", WRect(0.0f, 0.0f, 512.0f, 16.0f),
					false);
			}
			ppTutorialUI++;
		}
	}

	m_remainTimeY = WSingleton<COption>::Instance()->gGetBar_Y();

	m_pWindPuppet = g_resrcmng->GetPuppet("wind.pet", false, true, false);

	m_pFontOverlay = new CFontOverlay;
	m_pBase = new CBase;
	m_pPangInfo = new CPangInfo;
	m_pMiniScore = new CMiniScore;
	m_pWindInfo = new CWindInfo;
	m_pClubTypeInfo = new CClubTypeInfo;
	m_pAccuracyInfo = new CAccuracyInfo;
	m_pPowerInfo = new CPowerInfo;

	if (GetGameType() == 7)
	{
		m_pHolePoseSkins = new CHolePoseSkins;
		m_pExtraHoleInfo = new CExtraHoleInfo;
	}

	if (GOLFDOC()->m_tutorialMode == 15)
	{
		AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_normal", "[left_ic_00.jpg",
			0x80000000);
		AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_item", "[left_ic_i.jpg",
			0x80000000);
		AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_chat", "[left_ic_c.jpg",
			0x80000000);
		AddOverlay("\xbd\xc3\xb0\xe8", "[left_clock.jpg", 0);
		AddOverlay("\xbf\xac\xbd\xc0\xb8\xf0\xb5\xe5\xbf\xc9\xbc\xc7",
			"train_option_small.png", 0);

		m_pAuxMenuUI = new CAuxMenuUI;
		m_pRemainTimeInfo = new CRemainTimeInfo;
		m_pCalipers = new CCalipers;

		new CHelpMsg;
	}
	else
	{
		if (GOLFDOC()->m_tutorialMode == 5 || GOLFDOC()->m_tutorialMode == 9)
		{
			AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_normal", "[left_ic_00.jpg",
				0x80000000);
			AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_item", "[left_ic_i.jpg",
				0x80000000);
			AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_chat", "[left_ic_c.jpg",
				0x80000000);

			m_pAuxMenuUI = new CAuxMenuUI;
			new CHelpMsg;
		}

		AddOverlay("\xc1\xb6\xc1\xd8\xbf\xb5\xbf\xaa", "{sight.png", 0);
		AddOverlay("\xc7\xd9\xbd\xc9\xc6\xf7\xc0\xce\xc6\xae",
			"check_point.png", 0);
		AddOverlay("\xc7\xd9\xbd\xc9\xc6\xf7\xc0\xce\xc6\xaesmall",
			"check_point_small.png", 0);

		if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 12)
		{
			m_tutorialNo = 0;
			m_pTutoTitleUI = new CSimpleUI("[tuto_opentext3.png");
			for (int i = 0; i <= 14; i++)
			{
				m_pTutoTitleUI->Add(
					MakeStr("\xc5\xb8\xc0\xcc\xc6\xb2_%d", i + 1),
					WRect(0.0f, (float)i * 28.0f, 128.0f, 28.0f), true);
			}
		}
		else
		{
			AddOverlay("\xb9\xcc\xbc\xc7\xc1\xa4\xba\xb8_0", "[mission_00.jpg",
				0);
		}

		m_tutoMsgBlinkPeriod = 0.25f;
		m_tutoMsgShown = 0;
		m_tutoMsgBlinkCount = 2;
		m_tutoMsgBlinkTime = 0;
		m_missionMarkBlinkPeriod = 0.25f;
		m_missionMarkBlinkCount = 3;
		m_tutoTitleState = 2;
		m_tutoTitleTime = 0;
		m_tutoTitleAlpha = 0;
		m_missionTitleState = 2;
		m_missionTitleTime = 0;

		m_pTutoEscMsgUI = new CSimpleUI("[tuto_esc_msg2.png");
		m_pTutoEscMsgUI->Add("MSG0", WRect(0.0f, 0.0f, 256.0f, 16.0f), false);
		m_pTutoEscMsgUI->Add("MSG1", WRect(0.0f, 16.0f, 256.0f, 16.0f), false);
	}

	m_pFont = WSingleton<CChatMsg>::Instance()->GetMaskedFont();

	for (int i = 0; i < 15; i++)
	{
		if (i < GOLFDOC()->m_tutorialMode)
		{
			m_missionMarkShown[i] = true;
		}
		else
		{
			m_missionMarkShown[i] = false;
		}
	}

	if (GOLFDOC()->m_tutorialMode >= 15)
	{
		return;
	}

	if (m_pFontOverlay)
	{
		m_pFontOverlay->FixHoleInfo();
	}
}

void CScreen::OnInit(void)
{
	m_bannerTime = 0;

	m_hitBarCount = 10;
	m_hitBarFrom = 0;
	m_hitBarTime = 0;
	m_selectedIndex = -1;
	m_turnStarted = false;
	m_bannerGameMode = 0;

	m_accumValue = 0;
	m_fadeValue = 0;
	m_blinkTime = 0;
	m_hudVisible = true;
	m_clubAngle = g_PI * 2;
	m_clubSpinDir = 0;
	m_clubSwing = 0;
	m_playerCount = 0;

	m_msgColorTime = 0;
	m_msgColor = 0;

	m_cammodeAlpha = 0;

	m_drawFlags.Enable(2);

	unsigned long clubSetTypeId = WSingleton<CSharedDoc>::Instance()
									  ->m_userInfo[GOLFDOC()->m_currentPlayer]
									  .clubInfo.tid;
	sClubSet* pClubSet = GolfClub().FindClubSet(clubSetTypeId);
	WPuppet* pPuppet = NULL;
	if (pClubSet)
	{
		pPuppet = pClubSet->uiPuppet[GolfClub().GetType()];
	}
	m_clubBoundingBox = GetFxInterpreter()->ParseBoundingBoxScript_fx(pPuppet,
		FxInterpreter::eSF_FX);

	if (m_pTitleFont == NULL)
	{
		tagWTITLEFONT info;
		memset(&info, 0, sizeof(info));
		static char* s_windFontFile = "[font_wind.jpg";
		info.filename = &s_windFontFile;
		info.fontw = 16;
		info.fonth = 16;
		info.numPages = 1;
		info.texw = 128;
		info.texh = 64;
		info.pCharSet = "1234567890ym%-.?/:";
		m_pTitleFont = g_resrcmng->GetTitleFont();
		m_pTitleFont->Create(&info);
	}

	if (m_pMissionUI != NULL)
	{
		return;
	}

	m_pMissionUI = new CSimpleUI("text_mission.png");
	m_pMissionUI->Add("\xb9\xcc\xbc\xc7\xbe\xf8\xc0\xbd",
		WRect(91.0f, 10.0f, 232.0f, 100.0f), false);
	m_pMissionUI->Add("\xb9\xcc\xbc\xc7\xc0\xd6\xc0\xbd",
		WRect(50.0f, 130.0f, 256.0f, 130.0f), false);
}

void CScreen::OnProcess(float elapsed)
{
	if (GOLFDOC()->m_tutorialMode < 0xF)
	{
		if (m_missionMarkBlinkCount < 3)
		{
			m_missionMarkBlinkTime += elapsed;
			if (m_missionMarkBlinkTime > m_missionMarkBlinkPeriod)
			{
				m_missionMarkBlinkTime -= m_missionMarkBlinkPeriod;
				m_missionMarkShown[GOLFDOC()->m_tutorialMode] =
					!m_missionMarkShown[GOLFDOC()->m_tutorialMode];
				if (m_missionMarkShown[GOLFDOC()->m_tutorialMode])
				{
					m_missionMarkBlinkCount++;
				}
			}
		}

		if (m_tutoMsgBlinkCount < 2)
		{
			m_tutoMsgBlinkTime += elapsed;
			if (m_tutoMsgBlinkTime > m_tutoMsgBlinkPeriod)
			{
				m_tutoMsgBlinkTime -= m_tutoMsgBlinkPeriod;
				m_tutoMsgShown = 1 - m_tutoMsgShown;
				if (m_tutoMsgShown)
				{
					m_tutoMsgBlinkCount++;
				}
			}
		}

		WVector2D start;
		start.x = g_view->GetWidth() * 0.5f - 50.0f;
		start.y = g_view->GetHeight() * 0.3f;
		WVector2D end(g_view->GetWidth() - 250.0f, 2.0f);

		switch (m_tutoTitleState)
		{
		case 0:
			m_tutoTitleTime += elapsed;
			if (m_tutoTitleTime > 1.0f)
			{
				m_tutoTitleTime = 0;
				m_tutoTitleAlpha = 0xFF;
				m_tutoTitleState = 1;
			}
			else
			{
				m_tutoTitleAlpha = (unsigned char)(m_tutoTitleTime * 255.0f);
			}
			m_tutoTitleX = start.x;
			m_tutoTitleY = start.y;
			break;
		case 1:
		{
			m_tutoTitleTime += elapsed;
			if (m_tutoTitleTime > 1.0f)
			{
				m_tutoTitleTime = 1.0f;
				m_tutoTitleState = 2;
				IActor* pTutorial = GetActor("Tutorial");
				if (pTutorial)
				{
					pTutorial << MsgObject(0, 0x148, 0, 0, 0, 0, 0);
				}
			}
			WVector2D pos =
				(1.0f - m_tutoTitleTime) * start + m_tutoTitleTime * end;
			m_tutoTitleX = pos.x;
			m_tutoTitleY = pos.y;
			break;
		}
		}
	}

	if (GOLFDOC()->m_tutorialMode >= 0 && GOLFDOC()->m_tutorialMode <= 0xE)
	{
		if (GOLFDOC()->m_tutorialMode != 6 && GOLFDOC()->m_tutorialMode != 7)
		{
			WVector2D start;
			start.x = ((g_view->GetWidth() * 0.5f));
			start.y = g_view->GetHeight() * 0.33333334f;
			WVector2D end((float)((int)g_view->GetWidth() / 2), 10.0f);

			switch (m_missionTitleState)
			{
			case 0:
				m_missionTitleAlpha = 1.2f;
				m_missionTitleTime += elapsed;
				if (m_missionTitleTime > 1.0f)
				{
					m_missionTitleTime = 0;
					m_missionTitleState = 1;
				}
				m_missionTitleX = start.x;
				m_missionTitleY = start.y;
				break;
			case 1:
			{
				m_missionTitleTime += elapsed;
				m_missionTitleAlpha -= elapsed * 0.2f;
				if (m_missionTitleTime > 1.0f)
				{
					m_missionTitleTime = 1.0f;
					m_missionTitleAlpha = 1.0f;
					m_missionTitleState = 2;
				}
				WVector2D pos = (1.0f - m_missionTitleTime) * start +
					m_missionTitleTime * end;
				m_missionTitleX = pos.x;
				m_missionTitleY = pos.y;
				break;
			}
			case 2:
				m_missionTitleAlpha = 1.0f;
				break;
			}
		}
	}

	if (WSingleton<CCapturedBg>::Instance()->IsCaptured())
	{
		if (GOLFDOC()->m_shotPhase == 2)
		{
			m_hitBarTime += elapsed * 100.0f;
			if (m_hitBarTime > 20.0f)
			{
				m_hitBarTime -= 40.0f;
			}
		}
	}
	else
	{
		Wind().Process(elapsed);

		if (m_pBase)
		{
			m_pBase->Process(elapsed);
		}

		if (m_pFontOverlay)
		{
			m_pFontOverlay->Process(elapsed);
		}

		if (m_pPangInfo)
		{
			m_pPangInfo->Process(elapsed);
		}

		if (m_pGuildInfo)
		{
			m_pGuildInfo->Process(elapsed);
		}

		float alpha = elapsed * 7.0f + m_cammodeAlpha;
		if (alpha > 1.0f)
		{
			alpha = 1.0f;
		}
		m_cammodeAlpha = alpha;

		if (GolfClub().TypeUpdated())
		{
			GetFxInterpreter()->DetachFx(m_clubBoundingBox);
			WPuppet* pPuppet =
				GolfClub()
					.FindClubSet(WSingleton<CSharedDoc>::Instance()
							->m_userInfo[GOLFDOC()->m_currentPlayer]
							.clubInfo.tid)
					->uiPuppet[GolfClub().GetType()];
			m_clubBoundingBox = GetFxInterpreter()->ParseBoundingBoxScript_fx(
				pPuppet, FxInterpreter::eSF_FX);
		}

		if (GolfClub().Updated())
		{
			m_clubAngle = 0;
			m_clubSpinDir = 1;
			m_clubSwing = 0;
		}
		else
		{
			switch (m_clubSpinDir)
			{
			case -1:
				m_clubAngle -= elapsed * 21.99115f;
				if (m_clubAngle < g_PI * 2)
				{
					m_clubAngle = g_PI * 2;
					m_clubSpinDir = 0;
				}
				break;
			case 0:
				m_clubSwing += elapsed * 2.0f;
				break;
			case 1:
				m_clubAngle += elapsed * 21.99115f;
				if (m_clubAngle > 6.8067842f)
				{
					m_clubAngle = 6.8067842f;
					m_clubSpinDir = -1;
				}
				break;
			}
		}
	}

	if (m_pCalipers)
	{
		if (IsMyTurn(false) &&
			PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000009)
		{
			m_pCalipers->Process(elapsed);
		}
		else if (S5::IsUsableCalipers() == true)
		{
			if (WSingleton<CSharedDoc>::Instance()->m_gameMode != 1 &&
				WSingleton<CSharedDoc>::Instance()->m_gameMode != 2)
			{
				m_pCalipers->Process(elapsed);
			}
		}
	}

	if (m_pAccuracyInfo)
	{
		m_pAccuracyInfo->Process(elapsed);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_golfGame.shotTimeLimit)
	{
		if (GOLFDOC()->m_shotPhase <= 0 && IsMyTurn(false))
		{
			if (GOLFDOC()->m_remainTime < 10000)
			{
				m_blinkTime += elapsed;
				if (m_blinkTime > 0.25f)
				{
					m_blinkTime -= 0.25f;
					m_hudVisible = !m_hudVisible;
				}
			}
			else
			{
				m_hudVisible = true;
			}
		}
		else
		{
			m_hudVisible = true;
			m_blinkTime = 0;
		}
	}

	if (m_pMiniScore && GOLFDOC()->m_tutorialMode >= 7)
	{
		m_pMiniScore->Process(elapsed);
	}

	if (GOLFDOC()->GameMode() & 0x40300)
	{
		if (m_bannerGameMode != GOLFDOC()->GameMode())
		{
			m_bannerGameMode = GOLFDOC()->GameMode();
			m_bannerTime = -255.0f;
		}
		else
		{
			float time = elapsed * 512.0f + m_bannerTime;
			if (time > 255.0f)
			{
				time = 255.0f;
			}
			m_bannerTime = time;
		}
	}

	if (GolfBall().m_groundType == 2)
	{
		CGolfDoc* pDoc = GOLFDOC();
		WVector cupPos = pDoc->GetHoleData().pin;
		float range = Max(0.0f,
			0.69813168f -
				Abs(CalcDeltaAngle(cupPos - GolfBall().m_pos,
					pDoc->GetCameraData().mat.za)));
		unsigned char alpha = (unsigned char)(128.0f * (range * 1.4323945f));

		if (GolfClub().PowerShotType() != 0)
		{
			m_hitBarRangeColor = (alpha << 24) | 0xFFF600;
		}
		else
		{
			m_hitBarRangeColor = (alpha << 24) | 0xFF96;
		}
	}

	if (m_turnStarted)
	{
		m_turnStartTime += elapsed;
		if (m_turnStartTime > 5.0f)
		{
			m_turnStarted = false;
			CMouseCursor::Instance()->Enable(0x21, true);
		}
	}

	if (!WSingleton<CQuitWindow>::Instance()->IsActive())
	{
		switch (CMouseCursor::Instance()->GetArea())
		{
		case 7:
			if (g_input->GetButton(LEFT_BUTTON) == 2 &&
				CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON) &&
				GOLFDOC()->m_tutorialMode == 0xF)
			{
				if (WSingleton<CChatMsg>::Instance()->IsOpened())
				{
					WSingleton<CChatMsg>::Instance()->Close(false, true);
				}
				else
				{
					WSingleton<CChatMsg>::Instance()->Open(false, true);
				}
				SendMsg(this, "Item", 0x195, 0, 0, 0, 0);
			}
			break;
		case 5:
		{
			int state;
			SendMsg(this, "Item", 0x199, (intptr_t)&state, 0, 0, 0);
			if (state == 3)
			{
				SendMsg(this, "Player", 0x199, (intptr_t)&state, 0, 0, 0);
				if (state != 3)
				{
					if (m_pCalipers)
					{
						m_pCalipers->VisiblePowerBar(1);
					}

					if (g_input->GetButton(LEFT_BUTTON) == 1 &&
						m_drawFlags.GetFlag(2) && IsMyTurn(false) &&
						GOLFDOC()->m_shotPhase == 0 && !GOLFDOC()->m_bPause &&
						!WSingleton<CChatMsg>::Instance()->IsActive() &&
						GOLFDOC()->m_tutorialMode >= 2 && GolfClub().BarIs(2))
					{
						SendMsg(this, "GolfRule", 0x76, 0, 0, 0, 0);
					}
				}
			}
			break;
		}
		case 10:
		case 11:
		case 12:
		case 13:
		{
			bool busy;
			SendMsg(this, "Score", 0x139, (intptr_t)&busy, 0, 0, 0);
			if (!busy)
			{
				if (g_input->GetButton(RIGHT_BUTTON))
				{
					IActor* pGolfRule = GetActor("GolfRule");
					if (pGolfRule)
					{
						if (!WSingleton<CChatMsg>::Instance()->IsOpened())
						{
							WSingleton<CChatMsg>::Instance()->Open(true, true);
						}
						unsigned char index =
							CMouseCursor::Instance()->GetArea() - 10;
						pGolfRule << MsgObject(0, 0x37,
							(intptr_t)WSingleton<CSharedDoc>::Instance()
								->m_userInfo[index]
								.info.sNick,
							0, 0, 0, 0);
					}
				}

				if (g_input->GetButton(LEFT_BUTTON) == 2 &&
					CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON) &&
					OnlinePlay())
				{
					unsigned char index =
						CMouseCursor::Instance()->GetArea() - 10;
					if (PLAYER(index)->state != 3)
					{
						g_audio->PlaySfx("play_menu_open", 0, 0, 0, 0, 0.5f,
							200.0f);
						CMouseCursor::Instance()->SetCamMode(false);

						std::map<unsigned long, sBriefUserInfo>::iterator it =
							WSingleton<CSharedDoc>::Instance()
								->m_briefUserInfoMap.find(PLAYER(index)->oid);
						if (it !=
							WSingleton<CSharedDoc>::Instance()
								->m_briefUserInfoMap.end())
						{
							WSingleton<CUserInfo>::Instance()->SetInfo(
								it->second.dwUid, PLAYER(index)->oid, true,
								true, false, false, it->second.sNick);
						}
					}
				}
			}
			break;
		}
		case 31:
			if (g_input->GetButton(LEFT_BUTTON) == 2 &&
				CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON))
			{
				IActor* pTutorial = GetActor("Tutorial");
				if (pTutorial)
				{
					pTutorial << MsgObject(0, 0x14B, 0, 0, 0, 0, 0);
				}
			}
			break;
		case 32:
			if (g_input->GetButton(LEFT_BUTTON) == 2 &&
				CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON))
			{
				IActor* pTutorial = GetActor("Tutorial");
				if (pTutorial)
				{
					pTutorial << MsgObject(0, 0x14C, 0, 0, 0, 0, 0);
				}
			}
			break;
		case 33:
			if (g_input->GetButton(LEFT_BUTTON) == 2 &&
				CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON))
			{
				if (GOLFDOC()->m_tutorialMode < 0xF && !m_turnStarted)
				{
					m_turnStarted = true;
					m_turnStartTime = 0;
					CMouseCursor::Instance()->Enable(0x21, false);
				}
			}
			break;
		case 36:
			if (g_input->GetButton(LEFT_BUTTON) == 2 &&
				CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON))
			{
				CMouseCursor::Instance()->Enable(0x24, false);
				FrTrainingOptionDlg* pDlg = CreateForm<FrTrainingOptionDlg>(
					g_pFresh->GetManager(), this, "training", 0);
				pDlg->Open(
					(FRESH_PFN_RESULT)&CScreen::OnTrainingOptionDlgResult, 7);
			}
			break;
		}
	}

	if (WSingleton<CHelpMsg>::Instance())
	{
		WSingleton<CHelpMsg>::Instance()->Process(elapsed);
	}

	if (WSingleton<CHeadIcon>::Instance())
	{
		WSingleton<CHeadIcon>::Instance()->Process(elapsed);
	}

	if (IsMyTurn(true))
	{
		if (GolfClub().GetType() == 3)
		{
			GOLFDOC()->GetBar().center = s_barStart;
		}
		else
		{
			float offset = 0;
			SendMsg(this, "Player", 0x113, (intptr_t)&offset, 0, 0, 0);

			if (IsLocalContent((localContentType_t)0x2F))
			{
				if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
				{
					offset = (float)floor(0.0);
				}
				else
				{
					float half = offset * 0.5f;
					offset = (float)floor((double)(half * g_RADTODEG));
				}
			}
			else
			{
				float half = offset * 0.5f;
				offset = (float)floor((double)(half * g_RADTODEG));
			}

			if (Abs(offset) < 2.5f)
			{
				offset = 0;
			}
			else if (offset > 0.0f)
			{
				offset -= 2.0f;
			}
			else
			{
				offset += 2.0f;
			}

			float pangYaArea =
				WSingleton<CPowerGauge>::Instance()->GetPangYaArea();
			GOLFDOC()->GetBar().center =
				Between(pangYaArea - GolfClub().GetNarrowAccuracy(), offset,
					GolfClub().GetNarrowAccuracy() - pangYaArea) +
				s_barStart;

			if (GolfClub().GetNarrowAccuracy() >
					Abs(GOLFDOC()->GetBar().gaugeStep -
						GOLFDOC()->GetBar().center) &&
				GOLFDOC()->m_shotPhase == 2)
			{
				for (int i = 0; i < 3; i++)
				{
					if (Wabs(m_narrowFlicker[i] - 255.0f) < g_EPSILON)
					{
						m_narrowFlicker[i] = Random(225.0f, 255.0f, -1);
					}
				}
			}
			else
			{
				m_narrowFlicker[0] = 255.0f;
				m_narrowFlicker[1] = 255.0f;
				m_narrowFlicker[2] = 255.0f;
			}
		}
	}

	if (m_pHolePoseSkins)
	{
		m_pHolePoseSkins->Process(elapsed);
	}

	float fade = m_fadeValue - elapsed;
	m_fadeValue = 0.0f < fade ? fade : 0.0f;
}

bool CScreen::OnTrainingOptionDlgResult(int ret, FrForm* pForm)
{
	CMouseCursor::Instance()->Enable(0x24, true);
	return true;
}

void CScreen::OnDisplay(void)
{
	if (!m_unknown3E1)
	{
		return;
	}

	WRect rect;
	m_remainTimeY = WSingleton<COption>::Instance()->gGetBar_Y();
	if (!WSingleton<CCapturedBg>::Instance()->IsCaptured())
	{
		rect = WRect(12.0f, m_remainTimeY - 64.0f, 128.0f, 64.0f);
		if (GOLFDOC()->m_tutorialMode == 15)
		{
			if (WSingleton<CChatMsg>::Instance()->IsActive())
			{
				DrawOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_chat", rect, 0xFF, 0);
			}
			else if (CItemWindow::IsClosed())
			{
				DrawOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_normal", rect, 0xFF, 0);
			}
			else
			{
				DrawOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_item", rect, 0xFF, 0);
			}
		}

		DrawWindInfo();
		DrawCammodeInfo();
		DrawClubInfo();

		if (IsMassGame() &&
			WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType != 10)
		{
			DrawOverlay("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3",
				WRect(g_view->GetWidth() * 0.5f - 87.0f,
					g_view->GetHeight() - 32.0f, 256.0f, 32.0f),
				0xFF, 0);

			IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(
				WSingleton<CSharedDoc>::Instance()->m_roomType);

			if (pMatch)
			{
				if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType ==
					5)
				{
					char text[64];
					sprintf(text, "%s %s", pMatch->Name, "\xc6\xc0\xc0\xfc");
					float width = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view,
						(g_view->GetWidth() - width) * 0.5f - 25.0f,
						g_view->GetHeight() - 16.0f, text, 0, 0xFFFFFFFF, 0);
				}
				else
				{
					char text[80] = "";
					if (WSingleton<CSharedDoc>::Instance()
							->m_roomInfo.realGameType == 14)
					{
						const char* name = "\xbc\xc5\xc7\xc3\xc4\xda\xbd\xba";
						strcpy(text, name);
					}
					else
					{
						strcpy(text, pMatch->Name);
					}
					float width = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view,
						(g_view->GetWidth() - width) * 0.5f - 25.0f,
						g_view->GetHeight() - 16.0f, text, 0, 0xFFFFFFFF, 0);
				}
			}
			else if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType ==
				6)
			{
				float width = m_pFont->GetTextWidth(g_view,
					"\xb1\xe6\xb5\xe5 \xb4\xeb\xc0\xfc");
				m_pFont->Print(g_view,
					(g_view->GetWidth() - width) * 0.5f - 25.0f,
					g_view->GetHeight() - 16.0f,
					"\xb1\xe6\xb5\xe5 \xb4\xeb\xc0\xfc", 0, 0xFFFFFFFF, 0);
			}

			if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameTimeLimit +
					WSingleton<CSharedDoc>::Instance()->m_approachStartTime >
				g_CurrentTime)
			{
				unsigned long sec = (WSingleton<CSharedDoc>::Instance()
											->m_golfGame.gameTimeLimit +
										WSingleton<CSharedDoc>::Instance()
											->m_approachStartTime -
										g_CurrentTime) /
					1000;
				m_pFont->Print(g_view, g_view->GetWidth() * 0.5f + 39.0f,
					g_view->GetHeight() - 16.0f,
					MakeStr("%02d:%02d", sec / 60, sec % 60), 0,
					sec < 180 ? 0xFFFF0000 : 0xFFFFFFFF, 0);
			}
			else
			{
				m_pFont->Print(g_view, g_view->GetWidth() * 0.5f + 39.0f,
					g_view->GetHeight() - 16.0f, "00:00", 0, 0xFFFF0000, 0);
			}
			m_pFont->Flush(g_view);
		}
		else if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 6)
		{
			DrawOverlay("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3",
				WRect(g_view->GetWidth() * 0.5f - 87.0f,
					g_view->GetHeight() - 32.0f, 256.0f, 32.0f),
				0xFF, 0);

			std::string title("\xb1\xe6\xb5\xe5 \xb4\xeb\xc0\xfc");

			float width = m_pFont->GetTextWidth(g_view, title.c_str());
			m_pFont->Print(g_view, (g_view->GetWidth() - width) * 0.5f - 25.0f,
				g_view->GetHeight() - 16.0f, title.c_str(), 0, 0xFFFFFFFF, 0);

			unsigned long start = 0;
			unsigned long limit =
				WSingleton<CSharedDoc>::Instance()->m_golfGame.gameTimeLimit;
			start = WSingleton<CSharedDoc>::Instance()->m_approachStartTime;
			if (limit + start > g_CurrentTime)
			{
				unsigned long sec = (limit + start - g_CurrentTime) / 1000;
				m_pFont->Print(g_view, g_view->GetWidth() * 0.5f + 39.0f,
					g_view->GetHeight() - 16.0f,
					MakeStr("%02d:%02d", sec / 60, sec % 60), 0,
					sec < 180 ? 0xFFFF0000 : 0xFFFFFFFF, 0);
			}
			else
			{
				m_pFont->Print(g_view, g_view->GetWidth() * 0.5f + 39.0f,
					g_view->GetHeight() - 16.0f, "00:00", 0, 0xFFFF0000, 0);
			}

			m_pFont->Flush(g_view);
		}
	}

	DrawApproachInfo();
	if (m_missionMarkShown[1])
	{
		DrawOverlay("\xb9\xd9\xc5\xc1\xbf\xf8",
			WRect(-6.0f, m_remainTimeY - 60.0f, 128.0f, 128.0f), 0xFF, 0);
	}

	if (m_hudVisible && m_missionMarkShown[1])
	{
		DrawRemainTime();
	}

	if (m_drawFlags.GetFlag(1) && GOLFDOC()->m_tutorialMode >= 2 &&
		m_missionMarkShown[2] && (!GOLFDOC()->m_bNoStat || IsMyTurn(true)))
	{
		DrawHitBarInfo();
	}

	if (!(GOLFDOC()->GameMode() & 0x10))
	{
		if (GOLFDOC()->m_tutorialMode == 15 || GOLFDOC()->m_tutorialMode == 5)
		{
			SendMsg(this, "Item", 0x198, 0, 0, 0, 0);
		}

		WSingleton<CChatMsg>::Instance()->PrintInScreen();
		WSingleton<CChatMsg>::Instance()->DrawChatWindow();
		WSingleton<CChatMsg>::Instance()->PrintInChatWindow();
	}

	if (m_missionMarkShown[1])
	{
		DrawClubBallUI();
	}

	if (GOLFDOC()->m_tutorialMode >= 12)
	{
		sOverlay* pOverlay = m_overlays.Find("\xbd\xc3\xb0\xe8");
		if (pOverlay && pOverlay->m_pOverlay)
		{
			pOverlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
				WRect(64.0f, m_remainTimeY + 40.0f - 16.0f, 32.0f, 32.0f), 0,
				0xFFFFFFFF, 0.0f, 0);
		}

		if (m_pRemainTimeInfo)
		{
			m_pRemainTimeInfo->Display();
		}

		if (GolfClub().GetType() != 3)
		{
			float x, y;
			int color;
			SendMsg(this, "Spin", 0x167, (intptr_t)&x, (intptr_t)&y,
				(intptr_t)&color, 0);
			DrawOverlay2("\xbd\xba\xc7\xc9\xc6\xf7\xc0\xce\xc6\xae",
				WRect(x - 16.0f, y - 16.0f, 32.0f, 32.0f), color, 0);

			if (m_unknown3E3 == true)
			{
				SendMsg(this, "Spin", 0x168, (intptr_t)&x, (intptr_t)&y,
					(intptr_t)&color, 0);
				DrawOverlay2("\xbd\xba\xc7\xc9\xc6\xf7\xc0\xce\xc6\xae_Red",
					WRect(x - 16.0f, y - 16.0f, 32.0f, 32.0f), color, 0);
			}
		}
	}

	if (!WSingleton<CCapturedBg>::Instance()->IsCaptured())
	{
		if (m_pFontOverlay)
		{
			m_pFontOverlay->Display();
		}

		if (m_pMiniScore && (m_missionMarkShown[7] || m_missionMarkShown[3]) &&
			!WSingleton<CProjectG>::Instance()->HidePI())
		{
			m_pMiniScore->Display();
		}

		if (GOLFDOC()->m_tutorialMode == 15)
		{
			if (m_bannerTime > 0.0f)
			{
				switch (GOLFDOC()->GameMode())
				{
				case 0x200:
					DrawOverlay("\xc5\xbe\xba\xe4\xb5\xb5\xbf\xf2\xb8\xbb",
						WRect(0.0f, 29.0f, 256.0f, 128.0f),
						(unsigned char)m_bannerTime, 0);
					break;
				case 0x40000:
					DrawOverlay(
						"\xb3\xab\xc7\xcf\xba\xe4\xb5\xb5\xbf\xf2\xb8\xbb",
						WRect(0.0f, 29.0f, 256.0f, 64.0f),
						(unsigned char)m_bannerTime, 0);
					break;
				case 0x100:
					DrawOverlay(
						"\xc1\xb6\xc1\xbe\xba\xe4\xb5\xb5\xbf\xf2\xb8\xbb",
						WRect(0.0f, 29.0f, 256.0f, 128.0f),
						(unsigned char)m_bannerTime, 0);
					break;
				}
			}
		}

		if (GOLFDOC()->GameMode() != 0x10)
		{
			if (WSingleton<CHelpMsg>::Instance())
			{
				WSingleton<CHelpMsg>::Instance()->Display();
			}
		}

		if (m_pPangInfo && !WSingleton<CProjectG>::Instance()->HidePI())
		{
			m_pPangInfo->Display();
		}

		if (WSingleton<CSharedDoc>::Instance()->m_gameMode == 2)
		{
			DrawOverlay("\xbf\xac\xbd\xc0\xb8\xf0\xb5\xe5\xbf\xc9\xbc\xc7",
				WRect(0.0f, 245.0f, 32.0f, 32.0f), 0xFF, 0);
		}
	}

	DrawCommandArrow();

	if (m_pAccuracyInfo)
	{
		m_pAccuracyInfo->Display();
	}

	if (m_pHolePoseSkins)
	{
		m_pHolePoseSkins->Display();
	}

	if (m_pExtraHoleInfo)
	{
		m_pExtraHoleInfo->Display();
	}

	if (m_pGuildInfo)
	{
		m_pGuildInfo->Display();
	}

	if (m_playerCount > 0 && (GOLFDOC()->GameMode() & 2) &&
		GOLFDOC()->IsTeeShotReady())
	{
		char text[32];
		sprintf(text,
			"\xbe\xc6\xc1\xf7 %d\xb8\xed\xc0\xc7 \xc0\xaf\xc0\xfa\xb0\xa1 \xc1\xd8\xba\xf1\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			m_playerCount);
		float width = m_pFont->GetTextWidth(g_view, text);
		m_pFont->Print(g_view, (g_view->GetWidth() - width) * 0.5f, 200.0f,
			text, 0, 0xFF000000, 0);
	}

	DrawTutorialUI();
}

void CScreen::AddOverlay(const char* name, const char* file, int flags)
{
	sOverlay* overlay = new sOverlay;
	overlay->m_pOverlay = g_resrcmng->GetOverlay(file, flags);
	if (overlay->m_pOverlay)
	{
		strcpy(overlay->m_name, name);
		m_overlays.AddItem(overlay, overlay->m_name, false);
	}
	else
	{
		delete overlay;
	}
}

void CScreen::DrawOverlay(const char* name, const WRect& rect,
	unsigned char alpha, int flags)
{
	if (!alpha)
	{
		return;
	}

	sOverlay* overlay = m_overlays.Find(name);
	if (!overlay || !overlay->m_pOverlay)
	{
		return;
	}

	overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect,
		flags, (alpha << 24) | 0xFFFFFF, 0.0f, 0);
}

void CScreen::DrawOverlay2(const char* name, const WRect& rect, int color,
	int flags)
{
	if (!(color & 0xFF000000))
	{
		return;
	}

	sOverlay* overlay = m_overlays.Find(name);
	if (!overlay || !overlay->m_pOverlay)
	{
		return;
	}

	overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect,
		flags, color, 0.0f, 0);
}

void CScreen::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0x1D0:
		if (msg.param1 == 1)
		{
			GOLFDOC()->m_bShowDropPos = true;
		}
		else
		{
			GOLFDOC()->m_bShowDropPos = false;
		}
		break;
	case 0x1CF:
		m_playerCount = (unsigned char)msg.param1;
		break;
	case 0x1CB:
		if (m_pAuxMenuUI)
		{
			m_pAuxMenuUI->SetUnplayable();
		}
		break;
	case 0x1CC:
		if (m_pAuxMenuUI)
		{
			m_pAuxMenuUI->SetPause();
		}
		break;
	case 0x1CD:
		if (m_pAuxMenuUI)
		{
			m_pAuxMenuUI->OpenChatRecord();
		}
		break;
	case 0x1C9:
		if (m_pExtraHoleInfo)
		{
			m_pExtraHoleInfo->SetShotTime(GOLFDOC()->m_shotTime);
		}
		break;
	case 0x1CA:
		if (m_pExtraHoleInfo)
		{
			m_pExtraHoleInfo->UpdateLeader();
		}
		break;
	case 0x1B9:
		if (m_pFontOverlay)
		{
			m_pFontOverlay->ResetTimeOutInfo();
		}
		break;
	case 0x2718:
		if (m_pBase)
		{
			m_accumValue = *(float*)msg.param1;
			m_pBase->SetAngle(m_accumValue);
		}
		break;
	case 0x1A1:
		if (m_pBase)
		{
			float* delta = (float*)msg.param1;
			m_accumValue += *delta;
			m_pBase->SetAngle(m_accumValue);
		}
		break;
	case 0x1A3:
		if (m_pBase)
		{
			WVector normal = GetPVS().GetMeshNormal(GolfBall().m_meshIndex);
			m_pBase->SetNormal(normal);
		}

		if (m_pFontOverlay)
		{
			m_pFontOverlay->ResetHoleCupInfo();
		}
		m_hitBarRangeColor = 0x8000FF96;
		m_ballStateCut = GetPVS().GetMaxPowerOnGround();
		if (m_ballStateCut < 1.0f)
		{
			m_ballStateCut -= 0.05f;
		}
		break;
	case 0x20C:
	{
		WPolySoup::sPoint* pPoint =
			GOLFDOC()->m_pPolySoup->FindPoint((const char*)msg.param1);
		GolfBall().m_pos =
			GetPVS().GetGroundPoint(pPoint->pos, false, NULL, false) +
			WVector(0.0f, GolfBall().m_radius, 0.0f);
		GolfBall().m_meshIndex = GetPVS().GetMeshIndex(GolfBall().m_pos, false);
		WVector normal = GetPVS().GetMeshNormal(GolfBall().m_meshIndex);
		m_pBase->SetNormal(normal);
		break;
	}
	case 0x1A4:
		if (m_pFontOverlay)
		{
			m_pFontOverlay->SetActive(CFontOverlay::INFO_HOLECUP,
				msg.param1 != 0);
		}
		break;
	case 0x1A5:
		if (m_pFontOverlay)
		{
			m_pFontOverlay->SetActive(CFontOverlay::INFO_FLY, msg.param1 != 0);
		}
		break;
	case 0x1A7:
		m_drawFlags.Enable(1);
		break;
	case 0x1A8:
		m_drawFlags.Disable(1);
		break;
	case 0x1A6:
		m_fadeValue = 1.0f;
		break;
	case 0x1A9:
		m_remainTimeColor = msg.param1;
		m_remainTimeArc = *(float*)msg.param2;
		if (m_pRemainTimeInfo)
		{
			m_pRemainTimeInfo->SetColor(m_remainTimeColor);
		}
		break;
	case 0x1AA:
		m_hitBarColor = ULblend(0xFF016A96, 0xFFC81E42,
			float2int(*(float*)msg.param1 * 255.0f));
		break;
	case 0x1AC:
		if (m_pFontOverlay)
		{
			m_pFontOverlay->SetAccuracy();
		}
		break;
	case 0x1AB:
		if (m_pAccuracyInfo)
		{
			m_pAccuracyInfo->SetPower();
		}
		break;
	case 0x1AD:
	{
		float* alpha = (float*)msg.param1;
		m_msgColorTime = 0xFFFFFF - ((int)(*alpha * -255.0f) << 24);
		alpha = (float*)msg.param2;
		m_msgColor = 0xFFFFFF - ((int)(*alpha * -255.0f) << 24);
		break;
	}
	case 0x1AE:
		m_hitBarFrom = msg.param1;
		m_hitBarCount = msg.param2;
		if (msg.param3)
		{
			m_remainTimeY = COption::Instance()->gGetBar_Y();
			float maxY, minY;
			maxY = minY = m_hitMarks[0].y = ((float*)msg.param3)[0];
			int i;
			for (i = 1; i < m_hitBarCount + 1; i++)
			{
				m_hitMarks[i].y = ((float*)msg.param3)[i];
				if (m_hitMarks[i].y < minY)
				{
					minY = m_hitMarks[i].y;
				}

				if (m_hitMarks[i].y > maxY)
				{
					maxY = m_hitMarks[i].y;
				}
			}
			float center = (maxY - minY) * 0.5f + minY;
			float scale = (maxY - minY) > 1.6f ? 1.6f / (maxY - minY) : 1.0f;
			m_hitMarks[0].y = (m_remainTimeY - 1.0f) -
				(m_hitMarks[0].y - center) * scale * 5.0f;
			for (i = 1; i < m_hitBarCount + 1; i++)
			{
				m_hitMarks[i].y = (m_remainTimeY - 1.0f) -
					(m_hitMarks[i].y - center) * scale * 5.0f;
				float offset = Between(-s_barTextY,
					m_hitMarks[i].y - m_hitMarks[0].y, s_barTextY);
				if (offset > 0.0f)
				{
					int c = (int)(255.0f - offset * 31.875f);
					m_hitMarks[i].color = (c << 16) | c | 0xFF00E500;
				}
				else
				{
					int c = (int)(offset * 31.875f + 255.0f);
					m_hitMarks[i].color = ((c | 0xFFFFFF00) << 8) | c;
				}
			}
		}
		break;
	case 0x1A0:
		m_hitBarTime = 0.0f;
		if (CHelpMsg::Instance())
		{
			CHelpMsg::Instance()->OnChangeTurn();
		}

		if (m_pPangInfo)
		{
			m_pPangInfo->ChangeTurn();
		}

		if (m_pBase)
		{
			m_pBase->ChangeTurn();
		}

		if (m_pAuxMenuUI)
		{
			m_pAuxMenuUI->ChangeTurn();
		}

		if (m_pCalipers)
		{
			m_pCalipers->ResetBar();
		}
		break;
	case 0x1AF:
		m_drawFlags.Enable(2);
		break;
	case 0x1B0:
		m_drawFlags.Disable(2);
		break;
	case 0x1B1:
		m_cammodeAlpha = 0.0f;
		break;
	case 0x1B2:
		if (m_pFontOverlay)
		{
			m_pFontOverlay->ResetHoleInfo();
		}
		break;
	case 0x1B4:
		if (m_pPangInfo)
		{
			m_pPangInfo->PreCalc(msg.param1 != 0);
		}
		break;
	case 0x1B5:
		if (m_pPangInfo)
		{
			m_pPangInfo->Restore();
		}
		break;
	case 0x1B6:
		if (m_pPangInfo)
		{
			m_pPangInfo->Skip();
		}
		break;
	case 0x1B7:
		if (m_pPangInfo)
		{
			switch (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType)
			{
			case 1:
				m_pPangInfo->PreCalcTeamHoleInBonus();
				break;
			case 3:
				m_pPangInfo->PreCalcMatchHoleInBonus();
				break;
			case 7:
				m_pPangInfo->ShowMyPang();
				break;
			}
			SendMsg(this, "Score", 0x132, 0, 0, 0, 0);
		}
		break;
	case 0x1B8:
		if (m_pPangInfo)
		{
			*(bool*)msg.param1 = m_pPangInfo->IsFinalized();
		}
		break;
	case 0x4C:
		if (m_pBase)
		{
			m_pBase->ChangeBall(msg.param1);
		}
		break;
	case 0x1BA:
		if (msg.param1)
		{
			m_tutoMsgShown = 0;
			m_tutoMsgBlinkTime = 0.0f;
			m_selectedIndex = GOLFDOC()->m_tutorialMode;
			if (GOLFDOC()->m_tutorialMode < 8 &&
				WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 12)
			{
				m_turnStarted = true;
				m_turnStartTime = 0;
				CMouseCursor::Instance()->Enable(0x21, false);
			}
		}
		else
		{
			m_tutoMsgShown = 0;
		}
		break;
	case 0x208:
		m_missionTitleState = msg.param1 != 0 ? 0 : 3;
		break;
	case 0x1BB:
		if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 11)
		{
			IActor* pTutorial = GetActor("Tutorial");
			if (pTutorial)
			{
				pTutorial << MsgObject(0, 0x148, 0, 0, 0, 0, 0);
			}
		}
		else
		{
			m_tutoTitleState = 0;
			m_tutoTitleTime = 0.0f;
			m_tutoTitleAlpha = 0;
		}
		m_tutorialNo = msg.param1;
		break;
	case 0x1BC:
	{
		switch (msg.param1)
		{
		case 0:
			m_missionMarkShown[GOLFDOC()->m_tutorialMode] = false;
			m_missionMarkBlinkTime = 0.0f;
			m_missionMarkBlinkCount = 0;
			break;
		case 1:
			m_missionMarkShown[GOLFDOC()->m_tutorialMode] = false;
			m_missionMarkBlinkCount = 3;
			break;
		case 2:
			m_missionMarkShown[GOLFDOC()->m_tutorialMode] = true;
			break;
		}

		for (int i = 0; i < 15; i++)
		{
			if (i < GOLFDOC()->m_tutorialMode)
			{
				m_missionMarkShown[i] = true;
			}
		}
		break;
	}
	case 0x271B:
		m_remainTimeY = COption::Instance()->gGetBar_Y();
		if (m_pBase)
		{
			m_pBase->ResetUiPosition();
		}

		if (m_pPangInfo)
		{
			m_pPangInfo->ResetUiPosition();
		}

		if (m_pFontOverlay)
		{
			m_pFontOverlay->ResetUiPosition();
		}

		if (m_pMiniScore)
		{
			m_pMiniScore->ResetUiPosition();
		}
		SendMsg(this, "Grid", 0x1D4, 0, 0, 0, 0);
		CHelpMsg::Instance()->ResetUiPosition();
		break;
	case 0x1BD:
		if (CMiniScore* pMiniScore = m_pMiniScore)
		{
			pMiniScore->m_state = 0;
			pMiniScore->m_stateTime = 1.0f;
			pMiniScore->m_alpha = 0xFF;
		}
		break;
	case 0x1BE:
		if (m_pFontOverlay)
		{
			m_pFontOverlay->FlickerHoleInfo();
		}
		break;
	case 0x1BF:
		if (m_pAuxMenuUI)
		{
			m_pAuxMenuUI->SetActive(true, msg.param1);
		}
		break;
	case 0x1C0:
		if (m_pAuxMenuUI)
		{
			m_pAuxMenuUI->m_index = -1;
		}
		break;
	case 0x1C1:
		if (m_pBase)
		{
			*(WVector*)msg.param1 = m_pBase->GetBaseMatrix().ya;
		}
		break;
	case 0x1C2:
		*(float*)msg.param1 = m_ballStateCut;
		break;
	case 0x190:
		m_unknown3E1 = msg.param1 == 1;
		break;
	case 0x1C3:
		if (m_pMiniScore)
		{
			m_pMiniScore->Show();
		}
		break;
	case 0x1C4:
		if (m_pMiniScore)
		{
			m_pMiniScore->Hide();
		}
		break;
	case 0x1B3:
		if (m_pFontOverlay)
		{
			m_pFontOverlay->FixHoleInfo();
		}
		break;
	case 0x1C5:
		if (m_pFontOverlay)
		{
			m_pFontOverlay->FixHoleCupInfo();
		}
		break;
	case 0x1C6:
		if (m_pFontOverlay)
		{
			m_pFontOverlay->GetHoleInfoPos((float*)msg.param1,
				(float*)msg.param2);
		}
		break;
	case 0x49:
		if (m_pMiniScore)
		{
			m_pMiniScore->ChangeCharacter();
		}
		break;
	case 0x64:
		if (m_pAuxMenuUI)
		{
			FrChatFuncDlg* pChatFuncDlg = m_pAuxMenuUI->m_pChatFuncDlg;
			if (pChatFuncDlg)
			{
				pChatFuncDlg->AddChatMsg((const char*)msg.param1, msg.param2);
			}
		}
		break;
	case 0x1C7:
		if (m_pHolePoseSkins)
		{
			m_pHolePoseSkins->SetPose(msg.param1, msg.param2);
		}
		break;
	case 0x1C8:
		if (m_pMiniScore)
		{
			if (m_pMiniScore->m_pShotCountInfo)
			{
				m_pMiniScore->m_pShotCountInfo->StartFloating();
			}
		}
		break;
	case 0x1CE:
		if (m_pGuildInfo)
		{
			m_pGuildInfo->SetScore(msg.param1, msg.param2);
		}
		break;
	case 0x202:
		if (IsLocalContent((localContentType_t)0x2F))
		{
			PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;
			WVector up = WVector::UNIT_POS_Y;
			m_pBase->SetNormal(up);
			m_hitBarRangeColor = 0x8000FF96;
			m_ballStateCut = GetPVS().GetMaxPowerOnGround();
			if (m_ballStateCut < 1.0f)
			{
				m_ballStateCut -= 0.05f;
			}
			GolfClub().SetAccuracy();
			CFxSequence* pSeq =
				CFx::Instance()->OpenSequence("safetee_ball.seq", true);
			SendMsg(this, "Player", 0x203, 1, 0, 0, 0);
			if (pSeq)
			{
				pSeq->m_pos = GolfBall().m_pos;
			}
		}
		break;
	case 0xA0:
		if (m_pCalipers)
		{
			m_pCalipers->Reset();
		}
		break;
	case 0xA1:
		if (m_pCalipers)
		{
			float* limit = (float*)msg.param1;
			m_pCalipers->SetCalipersLimit(*limit);
		}
		break;
	case 0x20E:
		if (GOLFDOC()->m_tutorialMode == 11)
		{
			m_unknown3E2 = true;
		}
		break;
	case 0x1F0:
	{
		float intensity = (float)msg.param1;
		if (!(intensity >= 10.0f))
		{
			Wind().SetIntensity(intensity);
		}
		break;
	}
	case 0x1F1:
		Wind().SetDirection((float)msg.param1 * g_DEGTORAD);
		GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		break;
	case 0x1D1:
		m_unknown3E3 = msg.param1 == 1;
		break;
	}
}

CMiniScore::CMiniScore(void)
	: m_state(1), m_alpha(0)
{
	m_pOverlay04 = NULL;
	m_pOverlay1C = NULL;
	m_pOverlay20 = NULL;
	m_stateTime = 0;
	for (unsigned char i = 0; i < 4; i++)
	{
		m_pCharBitmap[i] = NULL;
	}

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
	{
		if (PLAYER(i)->state == 3)
		{
			continue;
		}

		CPartTidList& tidList = GOLFDOC()->GetPartTidList(i);

		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(tidList.m_charTid);

		if (pChar)
		{
			m_pCharBitmap[i] =
				g_pFresh->GetManager()->GetBitmap("ITEMS", pChar->c.Icon);
		}
	}

	m_pVsIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_vs");

	m_pOverlay20 = g_resrcmng->GetOverlay("[nplayer.jpg", 0);
	m_pOverlay1C = g_resrcmng->GetOverlay("info_hp.jpg", 0);

	m_pBestRecord[0] = g_resrcmng->GetOverlay("[best_record_l.png", 0);
	m_pBestRecord[1] = g_resrcmng->GetOverlay("[best_record_r.png", 0);

	if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 1)
	{
		m_pOverlay04 = g_resrcmng->GetOverlay("[info_2p.jpg", 0);
	}
	else if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 7)
	{
		m_pOverlay04 = g_resrcmng->GetOverlay("[skins_info_1p.jpg", 0);
	}
	else
	{
		m_pOverlay04 = g_resrcmng->GetOverlay("[info_1p.jpg", 0);
	}

	m_pFont = WSingleton<CChatMsg>::Instance()->GetMaskedFont();

	m_pShotCountInfo = new CShotCountInfo;

	m_pSimpleUI = new CSimpleUI("[hole_out.jpg");
	m_pSimpleUI->Add("HoleOut", WRect(0.0f, 0.0f, 128.0f, 32.0f), false);
	m_pSimpleUI->Add("GiveUp", WRect(0.0f, 32.0f, 128.0f, 32.0f), false);
	m_pSimpleUI->SetAngle(-0.52359879f);

	m_iconUV = WRect(0.0f, 0.0f, 0.46875f, 1.0f);
	m_iconRect.w = 120.0f;
	m_iconRect.h =
		(WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 7 ? 128.0f
																	  : 64.0f) *
		(WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 1 ? 2.0f
																	  : 1.0f);

	m_offsetY = 0;
	m_posX = g_view->GetWidth() - 120.0f;
	m_gameMode = 0;
}

CMiniScore::~CMiniScore(void)
{
	if (m_pOverlay04 != NULL)
	{
		g_resrcmng->Release(m_pOverlay04);
		m_pOverlay04 = NULL;
	}

	if (m_pOverlay1C != NULL)
	{
		g_resrcmng->Release(m_pOverlay1C);
		m_pOverlay1C = NULL;
	}

	if (m_pOverlay20 != NULL)
	{
		g_resrcmng->Release(m_pOverlay20);
		m_pOverlay20 = NULL;
	}

	if (m_pBestRecord[0] != NULL)
	{
		g_resrcmng->Release(m_pBestRecord[0]);
		m_pBestRecord[0] = NULL;
	}

	if (m_pBestRecord[1] != NULL)
	{
		g_resrcmng->Release(m_pBestRecord[1]);
		m_pBestRecord[0] = NULL;
	}

	if (m_pShotCountInfo)
	{
		delete m_pShotCountInfo;
		m_pShotCountInfo = NULL;
	}

	if (m_pSimpleUI)
	{
		delete m_pSimpleUI;
		m_pSimpleUI = NULL;
	}
}

void CMiniScore::ChangeCharacter(void)
{
	switch (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType)
	{
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
	{
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(
			WSingleton<CSharedDoc>::Instance()->m_userInfo[0].charInfo.tid);
		if (pChar)
		{
			m_pCharBitmap[0] =
				g_pFresh->GetManager()->GetBitmap("ITEMS", pChar->c.Icon);
		}
		else
		{
			m_pCharBitmap[0] = NULL;
		}
		break;
	}
	default:
		for (unsigned char i = 0; i < GetPlayerNum(); i++)
		{
			IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(
				WSingleton<CSharedDoc>::Instance()->m_userInfo[i].charInfo.tid);

			if (pChar)
			{
				m_pCharBitmap[i] =
					g_pFresh->GetManager()->GetBitmap("ITEMS", pChar->c.Icon);
			}
			else
			{
				m_pCharBitmap[i] = NULL;
			}
		}
		break;
	}
}

WRect CMiniScore::GetIconRect(int flags)
{
	WRect rect;
	rect.x = (flags & 2) ? g_view->GetWidth() - 120.0f : 0.0f;
	rect.y = (flags & 1) ? 124.0f : 30.0f;
	rect.w = m_iconRect.w;
	rect.h = m_iconRect.h;
	return rect;
}

void CMiniScore::Process(float elapsed)
{
	if (m_gameMode != GOLFDOC()->GameMode())
	{
		if ((m_gameMode & ~0x38) &&
			((GOLFDOC()->GameMode() & 8) ||
				(WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 7 &&
					(GOLFDOC()->GameMode() & 0x1000))))
		{
			m_state = 2;
			m_stateTime = 0;
			m_offsetY = 0;
			m_posX = g_view->GetWidth() - 120.0f;
			m_alpha = 0;
		}
		else if ((GOLFDOC()->GameMode() & ~0x3A) && (m_gameMode & 0x2A))
		{
			m_state = 3;
			m_stateTime = 0;
			m_alpha = 255;
		}

		m_gameMode = GOLFDOC()->GameMode();
	}

	if (m_state == 0)
	{
		if (m_pShotCountInfo)
		{
			m_pShotCountInfo->Process(elapsed);
		}
	}

	if (m_state == 0 || m_state == 1)
	{
		return;
	}

	if (m_state == 2)
	{
		m_stateTime += elapsed * 2.0f;
		if (m_stateTime > 1.0f)
		{
			m_state = 0;
			m_stateTime = 1.0f;
		}

		m_alpha = (unsigned char)(m_stateTime * 255.0f);
	}
	else
	{
		m_stateTime += elapsed;
		if (m_stateTime > 1.0f)
		{
			m_state = 1;
			m_stateTime = 1.0f;
		}

		m_offsetY -= m_stateTime * 30.0f;
		m_posX += m_stateTime * 30.0f;
	}
}

void CMiniScore::Flicker(void)
{
	m_state = 0;
	m_stateTime = 1.0f;
	m_alpha = 255;
}

void CMiniScore::Display(void)
{
	if (IsLocalContent((localContentType_t)0x4F) &&
		WSingleton<CSharedDoc>::Instance()->m_playingMode == 2)
	{
		GOLFDOC()->GetPlayer(0)->state = 0;
		Flicker();
	}

	if (m_state == 1)
	{
		return;
	}

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (!pDevice)
	{
		return;
	}

	bool bScoreOpen = false;
	WRect rc;
	WRect uv;
	int fill;
	unsigned char j;
	{
		MsgObject msg(0, 0x139, (intptr_t)&bScoreOpen, 0, 0, 0, 0);
		AfxGetTask()->GetActor("Score") << msg;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 1)
	{
		m_iconRect.y = 30.0f;
		for (unsigned char team = 0; team < 2; team++)
		{
			if (team)
			{
				uv.x = 0.5f;
				m_iconRect.x = m_posX;
				m_charRect.x = m_posX;
			}
			else
			{
				uv.x = 0.0f;
				m_iconRect.x = m_offsetY;
				m_charRect.x = m_offsetY + 120.0f - 46.0f;
			}
			m_charRect.y = m_iconRect.y + 14.0f;

			uv.y = 0.0f;
			uv.w = m_iconUV.w;
			uv.h = m_iconUV.h;

			unsigned long color =
				(team == PLAYER(GOLFDOC()->m_currentPlayer)->team &&
					!bScoreOpen)
				? m_alpha << 24
				: (m_alpha / 2) << 24;

			if (m_pOverlay04)
			{
				m_pOverlay04->Render(g_view, uv, m_iconRect, 0,
					color | 0xFFFFFF, 0.0f, 0);
			}

			fill = 0;
			for (j = 0; j < GOLFDOC()->GetPlayerNum(); j++)
			{
				if (PLAYER(j)->state == 3)
				{
					continue;
				}

				if (PLAYER(j)->team != team)
				{
					continue;
				}

				color = ((j == GOLFDOC()->m_currentPlayer && !bScoreOpen)
								? m_alpha << 24
								: (m_alpha / 2) << 24) |
					0xFFFFFF;

				if (m_pCharBitmap[j])
				{
					m_charRect.w = (float)m_pCharBitmap[j]->Width();
					m_charRect.h = (float)m_pCharBitmap[j]->Height();

					pDevice->DrawTexture(m_pCharBitmap[j], m_charRect, color,
						0);
				}

				rc.x = team ? m_posX + 5.0f : m_offsetY + 5.0f;
				rc.y = fill++ * 112.0f + m_iconRect.y + 2.0f;
				if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
				{
					g_pFresh->GetManager()->PrintText(WPoint(rc.x, rc.y), 0,
						WSingleton<CSharedDoc>::Instance()
							->m_userInfo[j]
							.info.sNick,
						-1.0f, color);
				}

				if (m_pVsIcon &&
					(bool)((WSingleton<CSharedDoc>::Instance()
								   ->m_userInfo[j]
								   .info.dwIdentity >>
							   7) &
						1))
				{
					WRect rect(m_iconRect.x + 98.0f,
						(j % 2) * 116.0f + (m_iconRect.y - 2.0f),
						(float)m_pVsIcon->Width(), (float)m_pVsIcon->Height());
					pDevice->DrawTexture(m_pVsIcon, rect, color, 0);
				}

				if (WSingleton<CMouseCursor>::Instance()->GetArea() == j + 10)
				{
					char text[32];
					rc.x = team ? m_posX - 256.0f + 6.0f
								: m_offsetY + 120.0f - 6.0f;
					rc.y = m_charRect.y;

					if (m_pBestRecord[team])
					{
						m_pBestRecord[team]->Render(g_view,
							WRect(0.0f, 0.0f, 1.0f, 1.0f),
							WRect(rc.x, rc.y, 256.0f, 128.0f), 0, color, 0.0f,
							0);
					}

					rc.x += team ? 110.0f : 15.0f;

					m_pFont->Print(g_view, rc.x, rc.y + 20.0f,
						"\xc8\xb9\xb5\xe6\xc6\xce :", 0, color & 0xFF000000, 0);
					m_pFont->Print(g_view, rc.x, rc.y + 37.0f,
						"\xc3\xd6\xb4\xeb\xba\xf1\xb0\xc5\xb8\xae :", 0,
						color & 0xFF000000, 0);
					m_pFont->Print(g_view, rc.x, rc.y + 54.0f,
						"\xc3\xd6\xb4\xeb\xc4\xa8\xc0\xce\xb0\xc5\xb8\xae :", 0,
						color & 0xFF000000, 0);
					m_pFont->Print(g_view, rc.x, rc.y + 71.0f,
						"\xc3\xd6\xb4\xeb\xc6\xdb\xc6\xc3\xb0\xc5\xb8\xae :", 0,
						color & 0xFF000000, 0);

					rc.x += 135.0f;
					sprintf(text, "%dp", (int)PLAYER(j)->totalPang);
					rc.w = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view, rc.x - rc.w, rc.y + 20.0f, text, 0,
						color & 0xFF000000, 0);

					sprintf(text, "%.2fy",
						WSingleton<CSharedDoc>::Instance()
							->m_holeStatistics[j]
							.fLongest);
					rc.w = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view, rc.x - rc.w, rc.y + 37.0f, text, 0,
						color & 0xFF000000, 0);

					sprintf(text, "%.2fy",
						WSingleton<CSharedDoc>::Instance()
							->m_holeStatistics[j]
							.fLongestChipIn);
					rc.w = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view, rc.x - rc.w, rc.y + 54.0f, text, 0,
						color & 0xFF000000, 0);

					sprintf(text, "%.2fy",
						WSingleton<CSharedDoc>::Instance()
							->m_holeStatistics[j]
							.fLongestPuttIn);
					rc.w = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view, rc.x - rc.w, rc.y + 71.0f, text, 0,
						color & 0xFF000000, 0);
				}

				m_charRect.y += 49.0f;
			}

			if (GOLFDOC()->GetTeam(team)->gauge > 0.0f)
			{
				rc.y = m_iconRect.y + 61.0f;
				rc.h = 4.0f;

				unsigned long barAlpha =
					(team == PLAYER(GOLFDOC()->m_currentPlayer)->team &&
						!bScoreOpen)
					? m_alpha << 24
					: (m_alpha / 2) << 24;
				unsigned char count =
					(unsigned char)(WSingleton<CSharedDoc>::Instance()
										->GetComboGaugeLimit(team) *
						0.030303031f);

				for (j = 0; j < count; j++)
				{
					fill = (int)Min(33.0f,
						GOLFDOC()->GetTeam(team)->gauge - j * 33.0f);
					if (fill <= 0)
					{
						break;
					}

					unsigned char slot;
					unsigned long rgb;
					if (j >= 3)
					{
						slot = Min(j - 3, 0);
						rgb = fill == 33.0f ? 0xFF : 0xC2FF;
					}
					else
					{
						slot = j;
						rgb = fill == 33.0f ? 0xFF0000 : 0xFFC200;
					}

					unsigned long barColor = rgb | barAlpha;
					rc.w = fill * 0.60606062f;

					if (team & 1)
					{
						rc.x = (slot * 21.0f + 1.0f) + (m_posX + 46.0f) + 5.0f;
					}
					else
					{
						rc.x = m_offsetY + 120.0f - 46.0f - 5.0f - rc.w -
							(slot * 21.0f + 1.0f);
					}

					if (m_pOverlay1C)
					{
						m_pOverlay1C->Render(g_view,
							WRect(0.0f, 0.0f, 1.0f, 1.0f), rc, 0, barColor,
							0.0f, 0);
					}
				}
			}

			color = ((team == PLAYER(GOLFDOC()->m_currentPlayer)->team &&
						 !bScoreOpen)
							? m_alpha << 24
							: (m_alpha / 2) << 24) |
				0xFFFFFF;
			if (team)
			{
				rc.x = (m_posX + 46.0f) + 56.0f;
			}
			else
			{
				rc.x = m_offsetY + 56.0f;
			}
			if (m_pShotCountInfo)
			{
				m_pShotCountInfo->SetColor(color);
				m_pShotCountInfo->Display(rc.x, m_iconRect.y + 69.0f, team);
			}

			m_pFont->Flush(g_view);

			float stateX = !team ? (m_offsetY + 35.0f) : (m_posX + 82.0f);

			if (m_pSimpleUI)
			{
				unsigned char alpha = (unsigned char)(m_alpha * 0.5f);

				m_pSimpleUI->SetColor((alpha << 24) | 0xFFFFFF);

				if (GOLFDOC()->GetTeam(team)->state == 1)
				{
					if (TEAM(team)->stroke[GOLFDOC()->m_currentHole - 1] >
						(unsigned char)(GOLFDOC()->GetHoleData().par + 4))
					{
						m_pSimpleUI->Render("GiveUp", stateX, 80.0f, 1.0f,
							0x22);
					}
					else if (PLAYER(team)->stroke[GOLFDOC()->m_currentHole -
								 1] > (unsigned char)0)
					{
						m_pSimpleUI->Render("HoleOut", stateX, 80.0f, 1.0f,
							0x22);
					}
				}
				else if (team == PLAYER(GOLFDOC()->m_currentPlayer)->team &&
					PLAYER(GOLFDOC()->m_currentPlayer)->state == 0)
				{
					if (GOLFDOC()->GameMode() & 0x1000)
					{
						m_pSimpleUI->Render("HoleOut", stateX, 80.0f, 1.0f,
							0x22);
					}
					else if (GOLFDOC()->GameMode() & 0x80000)
					{
						m_pSimpleUI->Render("GiveUp", stateX, 80.0f, 1.0f,
							0x22);
					}
				}
			}
		}
	}
	else if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 7)
	{
		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 3)
			{
				continue;
			}

			m_iconRect.y = (i & 1) ? 124.0f : 30.0f;

			if (i & 2)
			{
				uv.x = 0.5f;
				m_iconRect.x = m_posX;

				m_charRect.x = m_posX;
				m_charRect.y = m_iconRect.y + 14.0f;
			}
			else
			{
				uv.x = 0.0f;
				m_iconRect.x = m_offsetY;

				m_charRect.x = m_offsetY + 120.0f - 46.0f;
				m_charRect.y = m_iconRect.y + 14.0f;
			}

			unsigned long color =
				((i == GOLFDOC()->m_currentPlayer && !bScoreOpen)
						? m_alpha << 24
						: (m_alpha / 2) << 24) |
				0xFFFFFF;

			uv.y = 0.0f;
			uv.w = m_iconUV.w;
			uv.h = m_iconUV.h;

			if (m_pOverlay04)
			{
				m_pOverlay04->Render(g_view, uv, m_iconRect, 0, color, 0.0f, 0);
			}

			if (m_pCharBitmap[i])
			{
				m_charRect.w = (float)m_pCharBitmap[i]->Width();
				m_charRect.h = (float)m_pCharBitmap[i]->Height();

				pDevice->DrawTexture(m_pCharBitmap[i], m_charRect, color, 0);
			}

			rc.x = (i & 2) ? m_posX + 25.0f : m_offsetY + 25.0f;
			rc.y = m_iconRect.y + 2.0f;

			if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
			{
				g_pFresh->GetManager()->PrintText(WPoint(rc.x, rc.y), 0,
					WSingleton<CSharedDoc>::Instance()
						->m_userInfo[i]
						.info.sNick,
					84.0f, color);
			}

			if (m_pVsIcon &&
				(bool)((WSingleton<CSharedDoc>::Instance()
							   ->m_userInfo[i]
							   .info.dwIdentity >>
						   7) &
					1))
			{
				WRect rect(m_iconRect.x + 98.0f, m_iconRect.y - 2.0f,
					(float)m_pVsIcon->Width(), (float)m_pVsIcon->Height());
				pDevice->DrawTexture(m_pVsIcon, rect, color, 0);
			}

			if (PLAYER(i)->gauge > 0.0f)
			{
				rc.y = m_iconRect.y + 21.0f;
				rc.h = 4.0f;

				unsigned long barAlpha =
					(i == GOLFDOC()->m_currentPlayer && !bScoreOpen)
					? m_alpha << 24
					: (m_alpha / 2) << 24;
				unsigned char count =
					(unsigned char)(WSingleton<CSharedDoc>::Instance()
										->GetComboGaugeLimit(i) *
						0.030303031f);

				for (j = 0; j < count; j++)
				{
					fill = (int)Min(33.0f, PLAYER(i)->gauge - j * 33.0f);
					if (fill <= 0)
					{
						break;
					}

					unsigned char slot;
					unsigned long rgb;
					if (j >= 3)
					{
						slot = Min(j - 3, 0);
						rgb = fill == 33.0f ? 0xFF : 0xC2FF;
					}
					else
					{
						slot = j;
						rgb = fill == 33.0f ? 0xFF0000 : 0xFFC200;
					}

					unsigned long barColor = rgb | barAlpha;
					rc.w = fill * 0.60606062f;

					if (i & 2)
					{
						rc.x = (slot * 21.0f + 1.0f) + (m_posX + 46.0f) + 5.0f;
					}
					else
					{
						rc.x = m_offsetY + 120.0f - 46.0f - 5.0f - rc.w -
							(slot * 21.0f + 1.0f);
					}

					if (m_pOverlay1C)
					{
						m_pOverlay1C->Render(g_view,
							WRect(0.0f, 0.0f, 1.0f, 1.0f), rc, 0, barColor,
							0.0f, 0);
					}
				}
			}

			color = ((i == GOLFDOC()->m_currentPlayer && !bScoreOpen)
							? m_alpha << 24
							: (m_alpha / 2) << 24) |
				PLAYER(i)->color;

			uv.x = (i & 1) ? 0.5f : 0.0f;
			uv.y = (i & 2) ? 0.25f : 0.0f;
			uv.w = 0.5f;
			uv.h = 0.25f;
			rc.x = (i & 2) ? m_posX - 4.0f : m_offsetY - 4.0f;
			rc.y = m_iconRect.y - 3.0f;
			rc.w = 32.0f;
			rc.h = 16.0f;

			if (m_pOverlay20)
			{
				m_pOverlay20->Render(g_view, uv, rc, 0, color, 0.0f, 0);
			}

			color = ((i == GOLFDOC()->m_currentPlayer && !bScoreOpen)
							? m_alpha << 24
							: (m_alpha / 2) << 24) |
				0xFFFFFF;
			if (i & 2)
			{
				rc.x = (m_posX + 46.0f) + 56.0f;
			}
			else
			{
				rc.x = m_offsetY + 56.0f;
			}
			if (m_pShotCountInfo)
			{
				m_pShotCountInfo->SetColor(color);
				m_pShotCountInfo->Display(rc.x, m_iconRect.y + 29.0f, i);
			}

			m_pFont->Flush(g_view);

			float x = (i & 2) ? m_posX + 62.0f : m_offsetY + 51.0f;
			float y = (i & 1) ? 157.0f : 62.0f;

			if (m_pSimpleUI)
			{
				unsigned char alpha = (unsigned char)(m_alpha * 0.5f);

				m_pSimpleUI->SetColor((alpha << 24) | 0xFFFFFF);
				if (PLAYER(i)->state == 1)
				{
					if (PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1] >
						(unsigned char)(GOLFDOC()->GetHoleData().par + 4))
					{
						m_pSimpleUI->Render("GiveUp", x, y, 1.0f, 0x22);
					}
					else if (PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1] >
						(unsigned char)0)
					{
						m_pSimpleUI->Render("HoleOut", x, y, 1.0f, 0x22);
					}
				}
				else if (i == GOLFDOC()->m_currentPlayer &&
					PLAYER(i)->state == 0)
				{
					if (GOLFDOC()->GameMode() & 0x1000)
					{
						m_pSimpleUI->Render("HoleOut", x, y, 1.0f, 0x22);
					}
					else if (GOLFDOC()->GameMode() & 0x80000)
					{
						m_pSimpleUI->Render("GiveUp", x, y, 1.0f, 0x22);
					}
				}
			}
		}
	}
	else
	{
		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 3)
			{
				continue;
			}

			m_iconRect.y = (i & 1) ? 124.0f : 30.0f;

			if (i & 2)
			{
				uv.x = 0.5f;
				m_iconRect.x = m_posX;

				m_charRect.x = m_posX;
				m_charRect.y = m_iconRect.y + 14.0f;
			}
			else
			{
				uv.x = 0.0f;
				m_iconRect.x = m_offsetY;

				m_charRect.x = m_offsetY + 120.0f - 46.0f;
				m_charRect.y = m_iconRect.y + 14.0f;
			}

			unsigned long color =
				((i == GOLFDOC()->m_currentPlayer && !bScoreOpen)
						? m_alpha << 24
						: (m_alpha / 2) << 24) |
				0xFFFFFF;

			uv.y = 0.0f;
			uv.w = m_iconUV.w;
			uv.h = m_iconUV.h;

			if (m_pOverlay04)
			{
				m_pOverlay04->Render(g_view, uv, m_iconRect, 0, color, 0.0f, 0);
			}

			if (m_pCharBitmap[i])
			{
				m_charRect.w = (float)m_pCharBitmap[i]->Width();
				m_charRect.h = (float)m_pCharBitmap[i]->Height();

				pDevice->DrawTexture(m_pCharBitmap[i], m_charRect, color, 0);
			}

			rc.x = (i & 2) ? m_posX + 25.0f : m_offsetY + 25.0f;
			rc.y = m_iconRect.y + 2.0f;

			if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
			{
				g_pFresh->GetManager()->PrintText(WPoint(rc.x, rc.y), 0,
					WSingleton<CSharedDoc>::Instance()
						->m_userInfo[i]
						.info.sNick,
					84.0f, color);
			}

			if (m_pVsIcon &&
				(bool)((WSingleton<CSharedDoc>::Instance()
							   ->m_userInfo[i]
							   .info.dwIdentity >>
						   7) &
					1))
			{
				WRect rect(m_iconRect.x + 98.0f, m_iconRect.y - 2.0f,
					(float)m_pVsIcon->Width(), (float)m_pVsIcon->Height());
				pDevice->DrawTexture(m_pVsIcon, rect, color, 0);
			}

			if (PLAYER(i)->gauge > 0.0f)
			{
				rc.y = m_iconRect.y + 21.0f;
				rc.h = 4.0f;

				unsigned long barAlpha =
					(i == GOLFDOC()->m_currentPlayer && !bScoreOpen)
					? m_alpha << 24
					: (m_alpha / 2) << 24;
				unsigned char count =
					(unsigned char)(WSingleton<CSharedDoc>::Instance()
										->GetComboGaugeLimit(i) *
						0.030303031f);

				for (j = 0; j < count; j++)
				{
					fill = (int)Min(33.0f, PLAYER(i)->gauge - j * 33.0f);
					if (fill <= 0)
					{
						break;
					}

					unsigned char slot;
					unsigned long rgb;
					if (j >= 3)
					{
						slot = Min(j - 3, 0);
						rgb = fill == 33.0f ? 0xFF : 0xC2FF;
					}
					else
					{
						slot = j;
						rgb = fill == 33.0f ? 0xFF0000 : 0xFFC200;
					}

					unsigned long barColor = rgb | barAlpha;
					rc.w = fill * 0.60606062f;

					if (i & 2)
					{
						rc.x = (slot * 21.0f + 1.0f) + (m_posX + 46.0f) + 5.0f;
					}
					else
					{
						rc.x = m_offsetY + 120.0f - 46.0f - 5.0f - rc.w -
							(slot * 21.0f + 1.0f);
					}

					if (m_pOverlay1C)
					{
						m_pOverlay1C->Render(g_view,
							WRect(0.0f, 0.0f, 1.0f, 1.0f), rc, 0, barColor,
							0.0f, 0);
					}
				}
			}

			if (i == MyIndex())
			{
				int hasExtra = 0;
				float extraGauge = 0.0f;
				MsgObject msg(0, 0x129, (intptr_t)&hasExtra,
					(intptr_t)&extraGauge, 0, 0, 0);
				AfxGetTask()->GetActor("Player") << msg;

				if (hasExtra)
				{
					rc.y = m_iconRect.y + 21.0f;
					rc.h = 4.0f;

					for (j = 0; j < 3; j++)
					{
						fill = (int)(33.0f > extraGauge - j * 33.0f
								? extraGauge - j * 33.0f
								: 33.0f);
						if (fill <= 0)
						{
							break;
						}

						unsigned long barColor;
						if (fill == 33.0f)
						{
							barColor = ((i == GOLFDOC()->m_currentPlayer &&
											!bScoreOpen)
											   ? m_alpha << 24
											   : (m_alpha / 2) << 24) |
								0xFF0000;
						}
						else
						{
							barColor = ((i == GOLFDOC()->m_currentPlayer &&
											!bScoreOpen)
											   ? m_alpha << 24
											   : (m_alpha / 2) << 24) |
								0xFFC200;
						}

						rc.w = fill * 0.60606062f;
						if (i & 2)
						{
							rc.x = (j * 21.0f + 1.0f) + (m_posX + 46.0f) + 5.0f;
						}
						else
						{
							rc.x = m_offsetY + 120.0f - 46.0f - 5.0f - rc.w -
								(j * 21.0f + 1.0f);
						}

						if (m_pOverlay1C)
						{
							m_pOverlay1C->Render(g_view,
								WRect(0.0f, 0.0f, 1.0f, 1.0f), rc, 0, barColor,
								0.0f, 0);
						}
					}
				}
			}

			color = ((i == GOLFDOC()->m_currentPlayer && !bScoreOpen)
							? m_alpha << 24
							: (m_alpha / 2) << 24) |
				PLAYER(i)->color;

			uv.x = (i & 1) ? 0.5f : 0.0f;
			uv.y = (i & 2) ? 0.25f : 0.0f;
			uv.w = 0.5f;
			uv.h = 0.25f;
			rc.x = (i & 2) ? m_posX - 4.0f : m_offsetY - 4.0f;
			rc.y = m_iconRect.y - 3.0f;
			rc.w = 32.0f;
			rc.h = 16.0f;

			if (m_pOverlay20)
			{
				m_pOverlay20->Render(g_view, uv, rc, 0, color, 0.0f, 0);
			}

			color = ((i == GOLFDOC()->m_currentPlayer && !bScoreOpen)
							? m_alpha << 24
							: (m_alpha / 2) << 24) |
				0xFFFFFF;
			if (i & 2)
			{
				rc.x = m_posX + 46.0f;
			}
			else
			{
				rc.x = m_offsetY;
			}
			rc.x += 56.0f;
			if (m_pShotCountInfo)
			{
				m_pShotCountInfo->SetColor(color);
				m_pShotCountInfo->Display(rc.x, m_iconRect.y + 29.0f, i);
			}

			if (WSingleton<CMouseCursor>::Instance()->GetArea() == i + 10)
			{
				if (IsLocalContent((localContentType_t)0x4F) &&
					WSingleton<CSharedDoc>::Instance()->m_playingMode == 2)
				{
					break;
				}

				char text[32];
				rc.x = (i & 2) ? m_posX - 256.0f + 6.0f
							   : m_offsetY + 120.0f - 6.0f;
				rc.y = m_iconRect.y;

				m_pBestRecord[(i >> 1) & 1]->Render(g_view,
					WRect(0.0f, 0.0f, 1.0f, 1.0f),
					WRect(rc.x, rc.y, 256.0f, 128.0f), 0, color, 0.0f, 0);

				rc.x += (i & 2) ? 110.0f : 15.0f;
				color &= 0xFF000000;

				m_pFont->Print(g_view, rc.x, rc.y + 20.0f,
					"\xc8\xb9\xb5\xe6\xc6\xce :", 0, color, 0);
				switch (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType)
				{
				case 4:
				case 5:
				case 6:
				case 9:
				case 10:

					m_pFont->Print(g_view, rc.x, rc.y + 37.0f,
						"\xb4\xa9\xc0\xfb\xba\xf1\xb0\xc5\xb8\xae :", 0, color,
						0);
					break;
				default:

					m_pFont->Print(g_view, rc.x, rc.y + 37.0f,
						"\xc3\xd6\xb4\xeb\xba\xf1\xb0\xc5\xb8\xae :", 0, color,
						0);
					break;
				}

				m_pFont->Print(g_view, rc.x, rc.y + 54.0f,
					"\xc3\xd6\xb4\xeb\xc4\xa8\xc0\xce\xb0\xc5\xb8\xae :", 0,
					color, 0);

				m_pFont->Print(g_view, rc.x, rc.y + 71.0f,
					"\xc3\xd6\xb4\xeb\xc6\xdb\xc6\xc3\xb0\xc5\xb8\xae :", 0,
					color, 0);

				rc.x += 135.0f;
				sprintf(text, "%dp", (int)PLAYER(i)->totalPang);
				rc.w = m_pFont->GetTextWidth(g_view, text);
				m_pFont->Print(g_view, rc.x - rc.w, rc.y + 20.0f, text, 0,
					color, 0);

				switch (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType)
				{
				case 4:
				case 5:
				case 6:
				case 9:
				case 10:
					sprintf(text, "%dy",
						WSingleton<CSharedDoc>::Instance()
							->m_holeStatistics[i]
							.dwDistance);
					break;
				default:
					sprintf(text, "%.2fy",
						WSingleton<CSharedDoc>::Instance()
							->m_holeStatistics[i]
							.fLongest);
					break;
				}
				rc.w = m_pFont->GetTextWidth(g_view, text);
				m_pFont->Print(g_view, rc.x - rc.w, rc.y + 37.0f, text, 0,
					color, 0);

				sprintf(text, "%.2fy",
					WSingleton<CSharedDoc>::Instance()
						->m_holeStatistics[i]
						.fLongestChipIn);
				rc.w = m_pFont->GetTextWidth(g_view, text);
				m_pFont->Print(g_view, rc.x - rc.w, rc.y + 54.0f, text, 0,
					color, 0);

				sprintf(text, "%.2fy",
					WSingleton<CSharedDoc>::Instance()
						->m_holeStatistics[i]
						.fLongestPuttIn);
				rc.w = m_pFont->GetTextWidth(g_view, text);
				m_pFont->Print(g_view, rc.x - rc.w, rc.y + 71.0f, text, 0,
					color, 0);
			}

			m_pFont->Flush(g_view);

			float x = (i & 2) ? m_posX + 62.0f : m_offsetY + 51.0f;
			float y = (i & 1) ? 157.0f : 62.0f;

			if (m_pSimpleUI)
			{
				unsigned char alpha = (unsigned char)(m_alpha * 0.5f);

				m_pSimpleUI->SetColor((alpha << 24) | 0xFFFFFF);
				if (PLAYER(i)->state == 1)
				{
					if (PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1] >
						(unsigned char)(GOLFDOC()->GetHoleData().par + 4))
					{
						m_pSimpleUI->Render("GiveUp", x, y, 1.0f, 0x22);
					}
					else if (PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1] >
						(unsigned char)0)
					{
						m_pSimpleUI->Render("HoleOut", x, y, 1.0f, 0x22);
					}
				}
				else if (i == GOLFDOC()->m_currentPlayer &&
					PLAYER(i)->state == 0)
				{
					if (GOLFDOC()->GameMode() & 0x1000)
					{
						m_pSimpleUI->Render("HoleOut", x, y, 1.0f, 0x22);
					}
					else if (GOLFDOC()->GameMode() & 0x80000)
					{
						m_pSimpleUI->Render("GiveUp", x, y, 1.0f, 0x22);
					}
				}
			}
		}
	}

	if (!WSingleton<CHeadIcon>::Instance())
	{
		return;
	}

	WSingleton<CHeadIcon>::Instance()->DisplayBesideBoard(m_offsetY + 120.0f,
		m_posX);
}

void CMiniScore::ResetUiPosition(void)
{
	if (m_state == 0 || m_state == 2)
	{
		m_posX = g_view->GetWidth() - 120.0f;
	}
}

void CMiniScore::Show(void)
{
	m_state = 2;
	m_stateTime = 0;
	m_offsetY = 0;
	m_posX = g_view->GetWidth() - 120.0f;
	m_alpha = 0;
}

void CMiniScore::Hide(void)
{
	m_state = 1;
	m_stateTime = 1.0f;
	m_offsetY = -120.0f;
	m_posX = g_view->GetWidth() - 120.0f + 120.0f;
	m_alpha = 255;
}

void CMiniScore::StartFloating(void)
{
	if (m_pShotCountInfo)
	{
		m_pShotCountInfo->StartFloating();
	}
}

CAuxMenuUI::CAuxMenuUI(void)
	: m_pBase(NULL),
	  m_pDropDlg(NULL),
	  m_pDropFailDlg(NULL),
	  m_pChatFuncDlg(NULL)
{
	m_pBase = g_resrcmng->GetOverlay("[option_base.png", 0x80000000);
	m_pItemOverlay[0] = g_resrcmng->GetOverlay("[option_black.png", 0x80000000);
	m_pItemOverlay[1] = g_resrcmng->GetOverlay("[option_white.png", 0x80000000);

	m_active = 0;
	m_fade = 0;
	m_unknown28 = 2;
}

CAuxMenuUI::~CAuxMenuUI(void)
{
	if (g_resrcmng && m_pBase)
	{
		g_resrcmng->Release(m_pBase);
		m_pBase = 0;
	}

	if (g_resrcmng && m_pItemOverlay[0])
	{
		g_resrcmng->Release(m_pItemOverlay[0]);
		m_pItemOverlay[0] = 0;
	}

	if (g_resrcmng && m_pItemOverlay[1])
	{
		g_resrcmng->Release(m_pItemOverlay[1]);
		m_pItemOverlay[1] = 0;
	}
}

void CAuxMenuUI::Process(float elapsed)
{
	m_hoverIndex = WSingleton<CMouseCursor>::Instance()->GetArea() - 0x27;

	switch (m_active)
	{
	case 0:
		if (WSingleton<CMouseCursor>::Instance()->GetArea() == 0x26 &&
			g_input->GetButton(LEFT_BUTTON) == 2)
		{
			m_active = 1;
		}
		break;
	case 1:
		m_fade += elapsed * 5.0f;
		if (m_fade > 1.0f)
		{
			m_fade = 1.0f;
			m_active = 2;

			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 0, true);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 1, true);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 2, true);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 3, true);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 4, true);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 5, true);
		}
		break;
	case 2:
		if (m_hoverIndex < 0 || m_hoverIndex > 5)
		{
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 0, false);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 1, false);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 2, false);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 3, false);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 4, false);
			WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 5, false);
			m_active = 3;
		}
		else if (g_input->GetButton(LEFT_BUTTON) == 2)
		{
			switch (m_hoverIndex)
			{
			case 0:
				if (WSingleton<CQuitWindow>::Instance())
				{
					WSingleton<CQuitWindow>::Instance()->CreateOptionWindow();
				}
				break;
			case 1:
				AfxGetTask()->GetActor("GuideBook")
					<< MsgObject(0, 0x12A, 0, 0, 0, 0, 0);
				break;
			case 2:
				SetUnplayable();
				break;
			case 3:
				SetPause();
				break;
			case 4:
				OpenChatRecord();
				break;
			}
		}
		break;
	case 3:
		m_fade -= elapsed * 5.0f;
		if (m_fade < 0.0f)
		{
			m_fade = 0;
			m_active = 0;
		}
		break;
	}
}

void CAuxMenuUI::SetUnplayable(void)
{
	if (GOLFDOC()->m_bDropRequest != false || GalleryMode() || !IsMyTurn(false))
	{
		return;
	}

	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 2, false);

	bool overlapped = GetPVS().OverlappedWithObject(GolfBall().m_pos, 0);

	if (!overlapped &&
		(GolfBall().m_groundType == 0 || GolfBall().m_groundType == 1 ||
			GolfBall().m_groundType == 2))
	{
		if (m_pDropFailDlg != NULL)
		{
			return;
		}

		m_pDropFailDlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

		switch (GolfBall().m_groundType)
		{
		case 0:

			m_pDropFailDlg->SetMessage(
				"\xc6\xbc\xbc\xa6\xc0\xa7\xc4\xa1\xbf\xa1\xbc\xad\xb4\xc2 \xbe\xf0\xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xba\xed\xc0\xbb \xbc\xb1\xbe\xf0 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case 1:

			m_pDropFailDlg->SetMessage(
				"\xc6\xe4\xbe\xee\xbf\xfe\xc0\xcc\xbf\xa1\xbc\xad\xb4\xc2 \xbe\xf0\xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xba\xed\xc0\xbb \xbc\xb1\xbe\xf0 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case 2:

			m_pDropFailDlg->SetMessage(
				"\xb1\xd7\xb8\xb0\xbf\xa1\xbc\xad\xb4\xc2 \xbe\xf0\xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xba\xed\xc0\xbb \xbc\xb1\xbe\xf0 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		}

		m_pDropFailDlg->Open((FRESH_PFN_RESULT)&CAuxMenuUI::OnDropFailDlgResult,
			4);
		m_pDropFailDlg->SetTimeLimit(3.0f);
	}
	else
	{
		GOLFDOC()->m_bDropRequest = true;

		if (GOLFDOC()->GameMode() & ~0x200)
		{
			AfxGetTask()->GetActor("GolfRule")
				<< MsgObject(0, 0x78, 0x200, 0, 0, 0, 0);
		}

		g_camera.pivot =
			GetPVS().GetGroundPoint(GolfBall().m_pos, false, NULL, false);
		g_camera.pivot.y += 192.0f;
	}
}

void CAuxMenuUI::SetPause(void)
{
	switch (GetGameType())
	{
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		return;
	}

	if (GetShotTimeLimit() == 0)
	{
		return;
	}

	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 3, false);

	if (m_unknown28 == 0)
	{
		AfxGetTask()->GetActor("GolfRule") << MsgObject(0, 0x23,
			(intptr_t)"\xb0\xd4\xc0\xd3\xc1\xa4\xc1\xf6\xb4\xc2 \xc7\xd1 \xb0\xd4\xc0\xd3\xbf\xa1 2\xb9\xf8\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	m_unknown28--;

	if (OnlinePlay())
	{
		WSendPacket packet((enumClientPacket)0x30);
		packet.Encode1(1);
		packet.Send(TO_GAME);
	}
	else
	{
		AfxGetTask()->GetActor("GolfRule") << MsgObject(0, 0x84, 1,
			(intptr_t)"\xc7\xf6\xc0\xe7 \xb0\xd4\xc0\xd3\xc0\xcc \xc0\xe1\xbd\xc3 \xc1\xdf\xb4\xdc\xb5\xc8 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.\n\xb0\xd4\xc0\xd3\xc0\xbb \xb0\xe8\xbc\xd3\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad\xb4\xc2 \xc8\xae\xc0\xce\xc0\xbb \xb4\xad\xb7\xaf\xc1\xd6\xbc\xbc\xbf\xe4.",
			0, 0, 0);
	}
}

void CAuxMenuUI::OpenChatRecord(void)
{
	if (!OnlinePlay())
	{
		return;
	}

	if (m_pChatFuncDlg != NULL)
	{
		return;
	}

	m_pChatFuncDlg =
		CreateForm<FrChatFuncDlg>(g_pFresh->GetManager(), this, "chat_func", 0);
	m_pChatFuncDlg->Open((FRESH_PFN_RESULT)&CAuxMenuUI::OnChatFuncDlgResult, 5);
}

bool CAuxMenuUI::OnDropDlgResult(int ret, FrForm* pForm)
{
	if (ret == 1)
	{
		if (OnlinePlay())
		{
			WSendPacket packet((enumClientPacket)0x19);
			WVector pos = GOLFDOC()->m_pAutoTarget->GetDropPos();
			packet.EncodeBuffer(&pos, sizeof(WVector));
			packet.Send(TO_GAME);
		}
		else
		{
			GolfBall().m_pos = GOLFDOC()->m_pAutoTarget->GetDropPos();
			AfxGetTask()->GetActor("GolfRule")
				<< MsgObject(0, 0x8D, 0, 0, 0, 0, 0);
		}
	}

	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 2, true);

	m_pDropDlg = NULL;
	return true;
}

bool CAuxMenuUI::OnDropFailDlgResult(int ret, FrForm* pForm)
{
	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 2, true);

	m_pDropFailDlg = NULL;
	return true;
}

void CAuxMenuUI::Display(void)
{
	if (!CMouseCursor::Instance()->IsVisible())
	{
		return;
	}

	unsigned long color;
	if (m_active == 0)
	{
		switch (CMouseCursor::Instance()->GetArea())
		{
		case 0x25:
			color = 0x80FFFFFF;
			break;
		case 0x26:
			color = 0xFFFFFFFF;
			break;
		default:
			color = 0x80FFFFFF;
			break;
		}
	}
	else
	{
		color = 0xFFFFFFFF;
	}

	if (m_pBase)
	{
		m_pBase->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
			WRect(g_view->GetWidth() - 128.0f + (1.0f - m_fade) * 79.0f,
				g_view->GetHeight() * 0.5f - 66.0f, 128.0f, 256.0f),
			0x2000000, color, 0.0f, 0);
	}

	for (int i = 0; i < 6; i++)
	{
		if (m_pItemOverlay[i == m_hoverIndex || i == m_index])
		{
			m_pItemOverlay[i == m_hoverIndex || i == m_index]->Render(g_view,
				WRect(0.0f, i * 0.0859375f, 1.0f, 0.0859375f),
				WRect(g_view->GetWidth() - m_fade * 79.0f,
					g_view->GetHeight() * 0.5f + (i - 3) * 22.0f, 128.0f,
					22.0f),
				0, 0xFFFFFFFF, 0.0f, 0);
		}
	}
}

bool CAuxMenuUI::OnChatFuncDlgResult(int ret, FrForm* pForm)
{
	m_pChatFuncDlg = NULL;
	return true;
}

void CAuxMenuUI::SetActive(bool active, int index)
{
	if (active)
	{
		m_index = index;
		m_active = 1;

		WSingleton<CMouseCursor>::Instance()->Enable(0x27 + index, true);
		WSingleton<CMouseCursor>::Instance()->ForceMove(g_view->GetWidth() -
				50.0f,
			((index - 3) + 0.5f) * 22.0f + g_view->GetHeight() * 0.5f, -1.0f);
	}
	else
	{
		m_index = -1;
	}
}

void CAuxMenuUI::ChangeTurn(void)
{
	m_index = -1;
	m_active = 0;
	m_fade = 0;

	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 0, false);
	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 1, false);
	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 2, false);
	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 3, false);
	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 4, false);
	WSingleton<CMouseCursor>::Instance()->Enable(0x27 + 5, false);
}

CDrivingDistInfo::CDrivingDistInfo(void)
	: m_pPlayerOverlay(NULL),
	  m_pTopOverlay(NULL),
	  m_unknown08(true),
	  m_unknown09(false),
	  m_bestPlayer(0xFF),
	  m_unknown0B(0xFF),
	  m_bestColor(0xFFFFFFFF),
	  m_myColor(0xFFFFFFFF),
	  m_shownDistance(0),
	  m_bestDistance(0),
	  m_myDistance(0)
{
	m_pPlayerOverlay = g_resrcmng->GetOverlay("[nplayer.jpg", 0);
	m_pTopOverlay = g_resrcmng->GetOverlay("[top_drv.jpg", 0);
}

CDrivingDistInfo::~CDrivingDistInfo(void)
{
	if (g_resrcmng && m_pPlayerOverlay)
	{
		g_resrcmng->Release(m_pPlayerOverlay);
		m_pPlayerOverlay = 0;
	}

	if (g_resrcmng && m_pTopOverlay)
	{
		g_resrcmng->Release(m_pTopOverlay);
		m_pTopOverlay = 0;
	}
}

void CDrivingDistInfo::OnChangeTurn(void)
{
	m_unknown08 = true;

	m_myDistance = WSingleton<CSharedDoc>::Instance()
					   ->m_holeStatistics[MyIndex()]
					   .fLongest;

	if (Wabs(m_bestDistance) < g_EPSILON)
	{
		return;
	}

	m_bestPlayer = m_unknown0B = 0;
	m_bestDistance =
		WSingleton<CSharedDoc>::Instance()->m_holeStatistics[0].fLongest;

	for (unsigned char i = 1; i < GOLFDOC()->GetPlayerNum(); i++)
	{
		if (m_bestDistance <
			WSingleton<CSharedDoc>::Instance()->m_holeStatistics[i].fLongest)
		{
			m_bestPlayer = m_unknown0B = i;
			m_bestDistance = WSingleton<CSharedDoc>::Instance()
								 ->m_holeStatistics[i]
								 .fLongest;
		}
	}

	m_shownDistance = m_bestDistance;
}

void CDrivingDistInfo::Process(float elapsed)
{
	if (!m_unknown08)
	{
		return;
	}

	static float s_blinkTime = 0;
	s_blinkTime += elapsed;

	if (s_blinkTime > (m_unknown09 ? 1.5f : 2.0f))
	{
		s_blinkTime = 0;
		m_unknown09 = !m_unknown09;
	}

	m_bestColor = m_myColor = 0xFFFFFFFF;

	if (GolfBall().m_state == 0 && GolfBall().m_bOB)
	{
		if (m_bestDistance > m_shownDistance)
		{
			if (m_bestDistance - m_shownDistance < 0.05f)
			{
				m_bestPlayer = m_unknown0B;
				m_bestDistance = m_shownDistance;
				m_bestColor = 0xFFFFFFFF;
			}
			else
			{
				m_bestDistance -=
					(m_bestDistance - m_shownDistance) * elapsed * 5.0f;
				m_bestColor = 0xFFFF0000;
			}
		}

		if (GOLFDOC()->m_currentPlayer == MyIndex())
		{
			const float& target =
				WSingleton<CSharedDoc>::Instance()
					->m_holeStatistics[GOLFDOC()->m_currentPlayer]
					.fLongest;
			if (m_myDistance > target)
			{
				if (m_myDistance - target < 0.05f)
				{
					m_myDistance = target;
					m_bestColor = 0xFFFFFFFF;
				}
				else
				{
					m_myDistance -= (m_myDistance - target) * elapsed * 5.0f;
					m_myColor = 0xFFFF0000;
				}
			}
		}
	}
	else if (GOLFDOC()->GameMode() == 0x80)
	{
		if (WisEqual(GolfBall().PreData(0)->vel, WVector::ZERO, 1e-5f))
		{
			m_unknown08 = false;
			return;
		}

		WVector d;
		d = GolfBall().m_pos - GolfBall().PreData(0)->pos;
		float dist = sqrtf(d.x * d.x + d.z * d.z) * 0.3125f;

		if (m_bestDistance <= dist)
		{
			m_bestPlayer = GOLFDOC()->m_currentPlayer;
			m_bestDistance = dist;
			m_bestColor = 0xFF00FF97;
		}

		if (GOLFDOC()->m_currentPlayer == MyIndex() && m_myDistance <= dist)
		{
			m_myDistance = dist;
			m_myColor = 0xFF00FF97;
		}
	}
}

void CDrivingDistInfo::Display(WFont* pFont)
{
	char text[32];
	WRect rect;
	WRect uv;

	float y = WSingleton<COption>::Instance()->gGetBar_Y();

	WOverlay::DrawBox(g_view, WRect(27.0f, y + 4.0f, 62.0f, 1.0f), 0,
		0xFFEEEEEE, 0.001f);

	if (GOLFDOC()->GameMode() != 0x80 && m_unknown09)
	{
		uv.y = 0.0f;
		uv.x = 0.0f;
		uv.h = 1.0f;
		uv.w = 1.0f;
		rect.x = 27.0f;
		rect.y = y - 28.0f;
		rect.h = 64.0f;
		rect.w = 64.0f;

		m_pTopOverlay->Render(g_view, uv, rect, 0, 0xFFFFFFFF, 0.0f, 0);
	}
	else
	{
		sprintf(text, "%.2fy", m_bestDistance);
		float width = pFont->GetTextWidth(g_view, text);
		float x;
		x = 57.0f - (int)(width * 0.5f);
		pFont->Print(g_view, x, y - 9.0f, text, 0, m_bestColor, 0);

		sprintf(text, "%.2fy", m_myDistance);
		width = pFont->GetTextWidth(g_view, text);
		x = 57.0f - (int)(width * 0.5f);
		pFont->Print(g_view, x, y + 7.0f, text, 0, m_myColor, 0);

		uv.w = 0.5f;
		uv.h = 0.25f;
		rect.x = 41.0f;
		rect.w = 32.0f;
		rect.h = 16.0f;
		if (m_bestPlayer != 0xFF)
		{
			uv.x = (m_bestPlayer & 1) ? 0.5f : 0.0f;
			uv.y = (m_bestPlayer & 2) ? 0.25f : 0.0f;
			rect.y = y - 25.0f;
			if (m_pPlayerOverlay)
			{
				m_pPlayerOverlay->SetClippingArea(&rect);
				m_pPlayerOverlay->Render(g_view, uv, rect, 0,
					PLAYER(m_bestPlayer)->color | 0xFF000000, 0.0f, 0);
			}
		}

		uv.x = (MyIndex() & 1) ? 0.5f : 0.0f;
		uv.y = (MyIndex() & 2) ? 0.25f : 0.0f;
		rect.y = y + 17.0f;
		if (m_pPlayerOverlay)
		{
			m_pPlayerOverlay->SetClippingArea(&rect);
			m_pPlayerOverlay->Render(g_view, uv, rect, 0,
				PLAYER(MyIndex())->color | 0xFF000000, 0.0f, 0);
		}

		pFont->Flush(g_view);
	}
}

IMPLEMENT_OBJECT(FrChatFuncDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrChatFuncDlg, FrForm)
ON_FRESH_VI("chatview", FRCMD_INIT, FrChatFuncDlg::OnChatViewInit)
ON_FRESH_VI("hidechat", FRCMD_INIT, FrChatFuncDlg::OnHideChatInit)
ON_FRESH_VI("showchat", FRCMD_INIT, FrChatFuncDlg::OnShowChatInit)
ON_FRESH_VV("hidechat", FRCMD_LBUTTONUP, FrChatFuncDlg::OnBtnHideChatUp)
ON_FRESH_VV("showchat", FRCMD_LBUTTONUP, FrChatFuncDlg::OnBtnShowChatUp)
ON_FRESH_VV("savechat", FRCMD_LBUTTONUP, FrChatFuncDlg::OnBtnSaveChatUp)
ON_FRESH_VV("accuse", FRCMD_LBUTTONUP, FrChatFuncDlg::OnBtnAccuseUp)
ON_FRESH_VV("closedlg", FRCMD_LBUTTONUP, FrChatFuncDlg::OnBtnCloseDlgUp)
END_FRESH_MSGMAP()

FrChatFuncDlg::FrChatFuncDlg(void)
	: m_pUnknown110(NULL),
	  m_pBtnShowChat(NULL),
	  m_pBtnHideChat(NULL),
	  m_pUnknown11C(NULL),
	  m_pUnknown120(NULL),
	  m_pChatView(NULL)
{
}

FrChatFuncDlg::~FrChatFuncDlg(void)
{
}

void FrChatFuncDlg::OnBtnShowChatUp(void)
{
	if (m_pBtnHideChat)
	{
		m_pBtnHideChat->SetVisible(true);
	}

	if (m_pBtnShowChat)
	{
		m_pBtnShowChat->SetVisible(false);
	}

	GOLFDOC()->ShowChat(true);

	WSingleton<CChatMsg>::Instance()->AddChatMsg(
		"\xb0\xd4\xc0\xd3\xb3\xbb \xb4\xeb\xc8\xad\xc3\xa2\xc0\xbb \xc8\xb0\xbc\xba\xc8\xad\xc7\xd5\xb4\xcf\xb4\xd9.",
		0xFFFF7878, true, false);
}

void FrChatFuncDlg::OnBtnHideChatUp(void)
{
	if (m_pBtnHideChat)
	{
		m_pBtnHideChat->SetVisible(false);
	}

	if (m_pBtnShowChat)
	{
		m_pBtnShowChat->SetVisible(true);
	}

	GOLFDOC()->ShowChat(false);

	WSingleton<CChatMsg>::Instance()->AddChatMsg(
		"\xb0\xd4\xc0\xd3\xb3\xbb \xb4\xeb\xc8\xad\xc3\xa2\xc0\xbb \xbc\xfb\xb1\xe9\xb4\xcf\xb4\xd9.",
		0xFFFF7878, true, false);
}

void FrChatFuncDlg::OnBtnSaveChatUp(void)
{
	int index =
		WSingleton<CSharedDoc>::Instance()->m_chatManager.SaveChatHistory(
			GOLFDOC()->GetSaveChatNum(), 1);
	GOLFDOC()->SetSaveChatNum(index);
}

bool FrChatFuncDlg::OnReportDlgResult(int ret, FrForm* pForm)
{
	Close(FrOK, true);

	m_pReportDlg = NULL;
	return true;
}

void FrChatFuncDlg::OnBtnAccuseUp(void)
{
	m_pReportDlg =
		CreateForm<FrReportDlg>(g_pFresh->GetManager(), this, "report", 0);
	if (!m_pReportDlg)
	{
		return;
	}

	std::string msg;

	msg =
		"1. \xb4\xeb\xc8\xad\xc3\xa2\xc0\xc7 \xc1\xf6\xb3\xad \xb3\xbb\xbf\xeb\xb5\xb5 \xb8\xf0\xb5\xce \xbc\xad\xb9\xf6\xbf\xa1 \xb1\xe2\xb7\xcf\xb5\xcb\xb4\xcf\xb4\xd9.\n";

	msg +=
		"2. \xc1\xb6\xc0\xdb, \xc7\xe3\xc0\xa7, \xb9\xdd\xba\xb9\xbd\xc5\xb0\xed\xb4\xc2 \xc3\xb3\xb9\xfa\xc0\xbb \xb9\xde\xbd\xc0\xb4\xcf\xb4\xd9.\n";

	msg +=
		"3. \xbd\xc5\xb0\xed\xc7\xcf\xbd\xc5 \xb3\xbb\xbf\xeb\xc0\xba 48\xbd\xc3\xb0\xa3 \xc0\xcc\xb3\xbb\xbf\xa1 \xc3\xb3\xb8\xae\xb5\xcb\xb4\xcf\xb4\xd9.";

	m_pReportDlg->SetMessage(msg.c_str(), false);
	m_pReportDlg->Open((FRESH_PFN_RESULT)&FrChatFuncDlg::OnReportDlgResult, 3);
}

void FrChatFuncDlg::OnBtnCloseDlgUp(void)
{
	Close(FrOK, true);
}

void FrChatFuncDlg::OnHideChatInit(intptr_t param)
{
	m_pBtnHideChat = DYNAMIC_CAST(FrButton, param);
	if (!m_pBtnHideChat)
	{
		return;
	}

	if (GOLFDOC()->IsShowChat())
	{
		m_pBtnHideChat->SetVisible(true);
	}
	else
	{
		m_pBtnHideChat->SetVisible(false);
	}
}

void FrChatFuncDlg::OnShowChatInit(intptr_t param)
{
	m_pBtnShowChat = DYNAMIC_CAST(FrButton, param);
	if (!m_pBtnShowChat)
	{
		return;
	}

	if (GOLFDOC()->IsShowChat())
	{
		m_pBtnShowChat->SetVisible(false);
	}
	else
	{
		m_pBtnShowChat->SetVisible(true);
	}
}

void FrChatFuncDlg::OnChatViewInit(intptr_t param)
{
	m_pChatView = DYNAMIC_CAST(FrEdit, param);
	if (!m_pChatView)
	{
		return;
	}

	std::list<sChatLine>::iterator it;
	for (it = CSharedDoc::Instance()->m_chatLineList.begin();
		it != CSharedDoc::Instance()->m_chatLineList.end(); ++it)
	{
		sChatLine& line = *it;
		m_pChatView->AddLine(line.text.c_str(),
			WSingleton<CChatMsg>::Instance()->ConvertToUIColor(line.color),
			false);
	}
}

void FrChatFuncDlg::AddChatMsg(const char* msg, int color)
{
	if (m_pChatView)
	{
		m_pChatView->AddLine(msg, color, false);
	}
}

void CScreen::DrawClubInfo(void)
{
	if (!m_missionMarkShown[1] ||
		WSingleton<CCapturedBg>::Instance()->IsCaptured())
	{
		return;
	}

	sClubSet* clubSet = GolfClub().FindClubSet(CSharedDoc::Instance()
			->m_userInfo[GOLFDOC()->m_currentPlayer]
			.clubInfo.tid);

	float clipNear = g_view->GetClipNearValue();
	if (!(0.0f < clipNear))
	{
		clipNear = 0.0f;
	}

	float clipFar = g_view->GetClipFarValue();
	WRect rect(0.0f, m_remainTimeY - 170.0f, s_barStart, 150.0f);
	WVector2D center;
	center.y = m_remainTimeY - 36.0f;
	WMatrix cam = clubSet->uiCamera[GolfClub().GetType()];
	float fov = g_view->GetFOV();

	g_view->SetFOV(
		g_DEGTORAD * (g_view->GetHeight() / g_view->GetWidth() * 85.0f));
	g_view->SetClip(0.1f, 100.0f, false);

	float scale;
	if (WSingleton<COption>::Instance()->vIsWideMode())
	{
		scale = 800.0f / g_view->GetWidth();
	}
	else
	{
		scale =
			(600.0f / g_view->GetHeight() + 800.0f / g_view->GetWidth()) * 0.5f;
	}

	cam.za *= scale;
	float offset = (m_clubAngle * 0.15915494f - 1.0f) * 50.0f;
	center.x = 24.0f + offset;
	center.y -= offset;

	g_view->SetCamera(cam);
	g_view->SetClippingArea(rect);
	g_view->SetScreenCenter(center.x, center.y);
	g_view->UpdateCamera();

	WPuppet* puppet = clubSet->uiPuppet[GolfClub().GetType()];
	if (puppet)
	{
		puppet->ApplyBones(NULL,
			RotMat((float)sin((double)m_clubSwing) * 0.08726646f - m_clubAngle,
				0));
		puppet->SetAlpha(
			(unsigned char)Between(0.0f, m_clubAngle * 40.584511f, 255.0f));
		puppet->Transform(NULL, 1.0f, 0);
		puppet->UpdateLightSource(&clubSet->uiLight[GolfClub().GetType()],
			false, NULL);
		puppet->Render(g_view, true, 1.0f, false, true, false);
	}

	g_view->Flush(1);

	g_view->SetCamera(g_camera);
	g_view->SetClip(clipNear, clipFar, false);
	g_view->SetFOV(fov);
	g_view->ResetClippingArea();
	g_view->SetScreenCenter(g_view->GetWidth() * 0.5f,
		g_view->GetHeight() * 0.5f);
	g_view->UpdateCamera();
}

void CScreen::DrawCammodeInfo(void)
{
	if (!WSingleton<CMouseCursor>::Instance()->IsCamMode())
	{
		return;
	}

	char text[16];
	char state[8];

	if (g_input->GetButton(LEFT_BUTTON))
	{
		strcpy(state, "CLICK");
	}
	else
	{
		strcpy(state, "OVER");
	}

	switch (WSingleton<CMouseCursor>::Instance()->GetArea())
	{
	case 0x17:
		strcpy(text, MakeStr("LEFT_%s", state));
		break;
	case 0x18:
		strcpy(text, MakeStr("RIGHT_%s", state));
		break;
	case 0x19:
		strcpy(text, MakeStr("UP_%s", state));
		break;
	case 0x1A:
		strcpy(text, MakeStr("DOWN_%s", state));
		break;
	case 0x1B:
		if (GOLFDOC()->GameMode() & 0x8000)
		{
			strcpy(text, "INACTIVE");
		}
		else
		{
			strcpy(text, MakeStr("FORWARD_%s", state));
		}
		break;
	case 0x1C:
		if (GOLFDOC()->GameMode() & 0x8000)
		{
			strcpy(text, "INACTIVE");
		}
		else
		{
			strcpy(text, MakeStr("BACKWARD_%s", state));
		}
		break;
	default:
		if (GOLFDOC()->GameMode() == 0x8000)
		{
			strcpy(text, "INACTIVE");
		}
		else
		{
			strcpy(text, "NORMAL");
		}
		break;
	}

	if (!m_pCammodeUI)
	{
		return;
	}

	m_pCammodeUI->SetColor((WSingleton<CMouseCursor>::Instance()->IsVisible() ||
							   m_cammodeAlpha < 1.0f)
			? 0xFFFFFFFF
			: 0x80FFFFFF);
	m_pCammodeUI->Render(text, g_view->GetWidth() * 0.65f,
		g_view->GetHeight() - 150.0f, m_cammodeAlpha, 0x22);
}

void CScreen::DrawWindInfo(void)
{
	bool visible;

	SendMsg(NULL, "ShotReview", -1, (intptr_t)"GET_VISIBLE_STATE",
		(intptr_t)&visible, 0, 0);

	if (GolfClub().GetType() == 3)
	{
		if (!visible)
		{
			return;
		}
	}

	if (GOLFDOC()->m_tutorialMode != 15 &&
		(GOLFDOC()->m_tutorialMode != 11 || !m_unknown3E2))
	{
		return;
	}

	DrawOverlay2("\xb9\xd9\xb6\xf7\xc5\xab\xc3\xa2",
		WRect(g_view->GetWidth() - 114.0f, m_remainTimeY - 60.0f, 128.0f,
			128.0f),
		0xFFFFFFFF, 0);
	Wind().Display(m_pWindPuppet, g_view->GetWidth() - 50.0f,
		m_remainTimeY + 5.0f);
	DrawOverlay2("\xb9\xd9\xb6\xf7\xc0\xdb\xc0\xba\xc3\xa2",
		WRect(g_view->GetWidth() - 52.0f, m_remainTimeY + 1.0f, 64.0f, 64.0f),
		0xFFFFFFFF, 0);

	if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
	{
		S5::CLASSICSRV::eWINDPOWERSTEP step =
			S5::CLASSICSRV::GetWindPowerStep(Wind().GetGlobalIntensity());

		WRect rect(g_view->GetWidth() - 34.0f, m_remainTimeY + 19.0f, 32.0f,
			32.0f);

		switch (step)
		{
		case S5::CLASSICSRV::WINDPOWERSTEP_WEAK:
			DrawOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xbe\xe0", rect, 0xFF,
				0);
			break;
		case S5::CLASSICSRV::WINDPOWERSTEP_MIDDLE:
			DrawOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xc1\xdf", rect, 0xFF,
				0);
			break;
		case S5::CLASSICSRV::WINDPOWERSTEP_STRONG:
			DrawOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xb0\xad", rect, 0xFF,
				0);
			break;
		}
	}
	else
	{
		if (m_pWindInfo)
		{
			m_pWindInfo->Display();
		}
	}
}

void CScreen::DrawApproachInfo(void)
{
	if (CSharedDoc::Instance()->m_golfGame.gameType != 10 ||
		CSharedDoc::Instance()->m_approachStartTime == 0)
	{
		return;
	}

	DrawOverlay("\xb9\xcc\xbc\xc7\xb9\xda\xbd\xba\xc7\xc1\xb7\xb9\xc0\xd3"
				"1",
		WRect(g_view->GetWidth() - 270.0f, 46.0f, 256.0f, 32.0f), 255, 0);
	DrawOverlay("\xb9\xcc\xbc\xc7\xb9\xda\xbd\xba\xc7\xc1\xb7\xb9\xc0\xd3"
				"2",
		WRect(g_view->GetWidth() - 270.0f, 70.0f, 256.0f, 256.0f), 255, 0);
	if (GOLFDOC()->GetApproachMissionType() == 0)
	{
		m_pMissionUI->Render("\xb9\xcc\xbc\xc7\xbe\xf8\xc0\xbd",
			g_view->GetWidth() - 182.0f, 95.0f, 1.0f, 0);
	}
	else
	{
		m_pMissionUI->Render("\xb9\xcc\xbc\xc7\xc0\xd6\xc0\xbd",
			g_view->GetWidth() - 220.0f, 75.0f, 1.0f, 0);
	}

	if (GOLFDOC()->GetApproachMissionType() != 0)
	{
		char prizeText[64] = "\0";
		switch (GOLFDOC()->GetApproachWinnerType())
		{
		case 1:

			sprintf(prizeText,
				"\xa1\xd8 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8\xbd\xc3 \xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4 %d\xb0\xb3 \xb4\xf5.",
				GOLFDOC()->GetApproachPrizeType());
			break;
		case 2:

			sprintf(prizeText,
				"\xa1\xd8 \xbc\xba\xb0\xf8\xc7\xd1 \xc0\xaf\xc0\xfa\xbf\xa1\xb0\xd4 %d\xb0\xb3 \xb4\xf5.",
				GOLFDOC()->GetApproachPrizeType());
			break;
		case 3:
			sprintf(prizeText,
				"\xa1\xd8 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8\xbd\xc3 \xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4 %d\xb0\xb3 \xb4\xf5.",
				GOLFDOC()->GetApproachPrizeType());
			break;
		}

		WPoint textPos(g_view->GetWidth() - 205.0f, 138.0f);

		std::string desc(GOLFDOC()->GetApproachMissionDesc());
		std::string lines[4] = { "\0" };
		lines[3] = prizeText;

		if (desc.length() >= 33)
		{
			int pos = 0;
			int len = 33;
			for (int i = 0; i < 3; i++)
			{
				if (desc.length() < (unsigned int)(pos + 33))
				{
					lines[i] = desc.substr(pos, desc.length());
					break;
				}

				for (; len > 0; len--)
				{
					if (desc[pos + len] == ' ')
					{
						break;
					}
				}

				lines[i] = desc.substr(pos, len);
				pos += len + 1;
				len = 33;
			}
		}
		else
		{
			lines[0] = desc;
		}

		for (int i = 0; i < 4; i++)
		{
			m_pFont->Print(g_view, textPos.x, i * 17.0f + textPos.y,
				lines[i].c_str(), 0, 0xFFFFFFFF, NULL);
		}
	}

	m_pFont->Flush(g_view);

	DrawOverlay("\xc6\xae\xb7\xb9\xc1\xae\xb9\xda\xbd\xba",
		WRect(g_view->GetWidth() - 60.0f, 47.0f, 32.0f, 32.0f), 255, 0);

	unsigned long prize = GOLFDOC()->GetApproachTotalPrize();

	char prizeNum[4] = "";
	itoa(prize, prizeNum, 10);

	float offset = 65.0f;
	if (prize > 9)
	{
		offset = 75.0f;
	}

	m_pTitleFont->Print(g_view, g_view->GetWidth() - offset, 49.0f, prizeNum, 0,
		0xFFFFFFFF, NULL);

	unsigned long remain;
	if (CSharedDoc::Instance()->m_approachStartTime == 0)
	{
		remain = CSharedDoc::Instance()->m_golfGame.gameTimeLimit;
	}
	else if (CSharedDoc::Instance()->m_golfGame.gameTimeLimit +
			CSharedDoc::Instance()->m_approachStartTime <
		g_CurrentTime)
	{
		remain = 0;
	}
	else
	{
		remain = CSharedDoc::Instance()->m_golfGame.gameTimeLimit +
			CSharedDoc::Instance()->m_approachStartTime - g_CurrentTime;
	}

	char timeText[16];
	sprintf(timeText, "%d.%03d", remain / 1000, remain % 1000);

	unsigned long color;
	if (remain > 20000)
	{
		color = 0xFF000000;
	}
	else
	{
		color = remain > 10000 ? 0xFFFF8000 : 0xFFFF0000;
	}

	DrawOverlay2("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3_base",
		WRect(17.0f, 150.0f, 128.0f, 64.0f), color, 0);
	DrawOverlay("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3_border",
		WRect(17.0f, 150.0f, 128.0f, 64.0f), 255, 0);

	float width = m_pFont->GetTextWidth(g_view, timeText);
	m_pFont->Print(g_view, 75.0f - width, 161.0f, timeText, 0, 0xFFFFFFFF,
		NULL);

	if (GOLFDOC()->GameMode() & 0x8C0)
	{
		DrawOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1\xc1\xa4\xba\xb8",
			WRect(10.0f, 200.0f, 64.0f, 64.0f), 255, 0);
	}

	m_pFont->Flush(g_view);
}

void CScreen::DrawHitBarInfo(void)
{
	bool bigBar = false;
	float y = WSingleton<COption>::Instance()->gGetBar_Y();
	float h = 9.0f;

	if (GolfClub().GetType() == 3)
	{
		y -= 10.0f;
		bigBar = true;
		h = 19.0f;
	}

	unsigned long color = (GOLFDOC()->GetUIAlpha() << 24) | 0xFFFFFF;
	DrawOverlay2("\xc3\xbc\xb7\xc2\xb0\xd4\xc0\xcc\xc1\xf6",
		WRect(s_barStart, y, s_powerBarWidth, h), m_hitBarColor & color, 0);

	if (!bigBar)
	{
		DrawOverlay("\xbf\xde\xc2\xca\xb0\xd4\xc0\xcc\xc1\xf6",
			WRect(s_barGaugeStart, y, s_barStart - s_barGaugeStart, h),
			GOLFDOC()->GetUIAlpha(), 0);
	}

	if (GolfBall().m_groundType == 2 && m_hitBarFrom < m_hitBarCount)
	{
		float unit = s_powerBarWidth / (float)m_hitBarCount;
		DrawOverlay2("\xc8\xa6\xc4\xc5\xb1\xb8\xb0\xa3",
			WRect((float)m_hitBarFrom * unit + s_barStart, y, unit, h),
			m_hitBarRangeColor & color, 0);
	}

	if (bigBar && GolfBall().m_groundType == 2)
	{
		float step = 360.0f / (float)m_hitBarCount;

		for (int i = 1; i < m_hitBarCount + 1; i++)
		{
			WOverlay::DrawBox(g_view,
				WRect(((float)i - 0.5f) * step + s_barStart - 1.0f,
					m_hitMarks[i].y, 3.0f, 3.0f),
				0, m_hitMarks[i].color & color, 0.001f);
		}
	}

	sOverlay* overlay;
	if (GOLFDOC()->m_shotPhase < 2)
	{
		WRect rect(s_barStart, y, GOLFDOC()->GetBar().gaugeStep - s_barStart,
			h);
		overlay = m_overlays.Find("\xc6\xc4\xbf\xf6\xb0\xd4\xc0\xcc\xc1\xf6");
		if (overlay && overlay->m_pOverlay)
		{
			overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
				rect, 0, 0xFFFFFFFF, 0.0f, 0);
		}
	}
	else
	{
		WRect rect(s_barStart, y, GOLFDOC()->GetBar().power - s_barStart, h);
		overlay = m_overlays.Find("\xc6\xc4\xbf\xf6\xb0\xd4\xc0\xcc\xc1\xf6");
		if (overlay && overlay->m_pOverlay)
		{
			overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
				rect, 0, 0xFFFFFFFF, 0.0f, 0);
		}
	}

	WRect src;
	src.x = 90.0f;
	src.w = 479.0f;
	if (bigBar)
	{
		if (GolfBall().m_groundType == 2)
		{
			overlay = m_overlays.Find(
				MakeStr("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3%d", m_hitBarCount));
		}
		else
		{
			overlay = m_overlays.Find("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3"
									  "10");
		}

		src.y = m_remainTimeY - 17.0f;
		src.h = 64.0f;
	}
	else
	{
		DrawOverlay("\xc5\xb8\xc0\xcc\xb9\xd6\xbf\xb5\xbf\xaa",
			WRect(s_barStart - GolfClub().GetNarrowAccuracy(), y,
				GolfClub().GetNarrowAccuracy() * 2.0f, h),
			GOLFDOC()->GetUIAlpha(), 0);

		overlay = m_overlays.Find(
			"\xb4\xab\xb1\xdd_\xb5\xe5\xb6\xf3\xc0\xcc\xba\xea");

		src.y = m_remainTimeY - 6.0f;
		src.h = 32.0f;
	}

	if (overlay && overlay->m_pOverlay)
	{
		overlay->m_pOverlay->Render(g_view,
			WRect(0.064453125f, 0.0f, 0.935546875f, 1.0f), src, 0, color, 0.0f,
			0);
	}

	float pangYaArea = WSingleton<CPowerGauge>::Instance()->GetPangYaArea();

	if (IsLocalContent((localContentType_t)0x6E))
	{
		float offset = 0.0f;
		float width = pangYaArea * 2.0f;
		if ((int)width % 2 == 1)
		{
			offset = 0.5f;
		}

		DrawOverlay("\xbd\xc3\xc0\xdb\xc1\xa1",
			WRect(GOLFDOC()->GetBar().center - pangYaArea - offset, y, width,
				h),
			GOLFDOC()->GetUIAlpha(), 0);
	}
	else
	{
		DrawOverlay("\xbd\xc3\xc0\xdb\xc1\xa1",
			WRect(GOLFDOC()->GetBar().center - pangYaArea, y, pangYaArea * 2.0f,
				h),
			GOLFDOC()->GetUIAlpha(), 0);
	}

	if (bigBar)
	{
		if (IsLocalContent((localContentType_t)0x4F) &&
			WSingleton<CSharedDoc>::Instance()->m_playingMode == 2)
		{
			if (WSingleton<CSharedDoc>::Instance()->m_replayState != 3)
			{
				GOLFDOC()->GetBar().gaugeStep = s_barStart;
			}
			else
			{
				GOLFDOC()->GetBar().gaugeStep =
					WSingleton<CSharedDoc>::Instance()->m_replayPosition;
			}
		}

		DrawOverlay("\xc0\xd4\xc3\xbc\xb9\xd9",
			WRect(GOLFDOC()->GetBar().gaugeStep - 4.0f, m_remainTimeY - 12.0f,
				8.0f, 24.0f),
			GOLFDOC()->GetUIAlpha(), 0);
	}
	else
	{
		DrawOverlay("\xc0\xd4\xc3\xbc\xb9\xd9",
			WRect(GOLFDOC()->GetBar().gaugeStep - 4.0f, m_remainTimeY - 4.0f,
				8.0f, 16.0f),
			GOLFDOC()->GetUIAlpha(), 0);
	}

	if (GOLFDOC()->m_shotPhase == 0)
	{
		overlay = m_overlays.Find("\xb0\xd4\xc0\xcc\xc1\xf6\xc1\xa4\xc1\xf6");
	}
	else if (GOLFDOC()->m_shotPhase == 1)
	{
		overlay = m_overlays.Find("\xb0\xd4\xc0\xcc\xc1\xf6\xc5\xac\xb8\xaf");
	}
	else
	{
		overlay = m_overlays.Find("\xb0\xd4\xc0\xcc\xc1\xf6\xc0\xcc\xb5\xbf");
	}

	if (overlay && overlay->m_pOverlay)
	{
		overlay->m_pOverlay->Render(g_view, WRect(0.125f, 0.0f, 0.875f, 1.0f),
			WRect(92.0f, m_remainTimeY + 13.0f, 56.0f, 32.0f), 0, color, 0.0f,
			0);
	}

	if (m_pPowerInfo)
	{
		WVector spot;
		SendMsg(this, "Spot", 0xF4, (intptr_t)&spot, 0, 0, 0);

		m_pPowerInfo->SetColor(color);
		m_pPowerInfo->SetPos(spot);
		m_pPowerInfo->Display();
	}

	if (m_pCalipers)
	{
		bool usable = S5::IsUsableCalipers();

		if (IsMyTurn(false) && !usable)
		{
			bool impact = true;

			if (PLAYER(GOLFDOC()->m_currentPlayer)->item != 0x18000009)
			{
				if (GOLFDOC()->m_shotPhase == 2 || GOLFDOC()->m_shotPhase == 3)
				{
					impact = false;
					goto drawRange;
				}
			}
			else if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000009)
			{
				m_pCalipers->Display();
				if (GOLFDOC()->m_shotPhase == 2 || GOLFDOC()->m_shotPhase == 3)
				{
drawRange:
					float pos = 0.0f;
					m_pCalipers->DrawShotPowerRange(pos, false);
					m_pCalipers->DrawLimitLine(pos, impact);
				}
			}
		}
		else if (usable == true)
		{
			if (WSingleton<CSharedDoc>::Instance()->m_gameMode != 1 &&
				WSingleton<CSharedDoc>::Instance()->m_gameMode != 2)
			{
				m_pCalipers->Display();
			}

			if (!m_pCalipers->IsVisiblePowerBar() &&
				(GOLFDOC()->m_shotPhase == 2 || GOLFDOC()->m_shotPhase == 3))
			{
				if (IsMyTurn(false))
				{
					float pos = 0.0f;
					m_pCalipers->DrawShotPowerRange(pos, false);
					m_pCalipers->DrawLimitLine(pos, false);
				}
			}
		}
	}

	if (GolfClub().GetType() == 3 || GalleryMode())
	{
		return;
	}

	if (GOLFDOC()->m_shotPhase == 1 &&
		s_powerBarWidth * 0.5f + s_barStart < GOLFDOC()->GetBar().gaugeStep)
	{
		DrawOverlay("\xc5\xac\xb8\xaf\xc7\xa5\xbd\xc3",
			WRect(GOLFDOC()->GetBar().gaugeStep - 23.0f, m_remainTimeY - 51.0f,
				64.0f, 64.0f),
			0xFF, 0);
	}
	else if (GOLFDOC()->m_shotPhase == 2)
	{
		DrawOverlay("\xc5\xac\xb8\xaf\xc7\xa5\xbd\xc3",
			WRect(117.0f, m_remainTimeY - 71.0f + Abs(m_hitBarTime), 64.0f,
				64.0f),
			0xFF, 0);
	}
}

void CScreen::DrawRemainTime(void)
{
	sOverlay* overlay =
		m_overlays.Find("\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3\xbf\xf8");

	if (WSingleton<CCapturedBg>::Instance()->IsCaptured())
	{
		if (overlay && overlay->m_pOverlay)
		{
			overlay->m_pOverlay->ArcClipRender(g_view,
				WRect(0.0f, 0.0f, 1.0f, 1.0f),
				WRect(11.5f, m_remainTimeY + 4.0f - 46.5f, 93.0f, 93.0f),
				m_remainTimeArc, 5.0614548f, 0, m_remainTimeColor);
		}
	}
	else
	{
		m_remainTimeArcShown = m_remainTimeArc;
		if (overlay && overlay->m_pOverlay)
		{
			overlay->m_pOverlay->ArcClipRender(g_view,
				WRect(0.0f, 0.0f, 1.0f, 1.0f),
				WRect(11.5f, m_remainTimeY + 4.0f - 46.5f, 93.0f, 93.0f),
				m_remainTimeArc, 5.0614548f, 0, m_remainTimeColor);
		}
	}
}

void CScreen::DrawClubBallUI(void)
{
	if (IsMyTurn(false))
	{
		sOverlay* overlay = m_overlays.Find("\xc5\xac\xb7\xb4\xb9\xd9");

		WRect clip;
		clip.x = 58.0f;
		clip.y = m_remainTimeY - 6.0f;
		clip.w = GolfClub().GetBarWidth();
		clip.h = 32.0f;

		unsigned long color = (GolfClub().GetBarAlpha() << 24) | 0xFFFFFF;

		if (overlay && overlay->m_pOverlay)
		{
			overlay->m_pOverlay->SetClippingArea(&clip);
			overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
				WRect(clip.x - 456.0f + clip.w, clip.y, 512.0f, 32.0f), 0,
				color, 0.0f, 0);
		}

		if (GolfClub().BarIs(0) || GolfClub().BarIs(3))
		{
			if (m_pSimpleUI21C)
			{
				m_pSimpleUI21C->SetColor(color);
				m_pSimpleUI21C->Render(GolfClub().GetName(), 85.0f,
					m_remainTimeY + 5.0f, 1.0f, 0x40);
			}

			if (m_pSimpleUI220)
			{
				m_pSimpleUI220->SetColor(color);

				m_pSimpleUI220->Render("\xc7\xf6\xc0\xe7\xc5\xac\xb7\xb4",
					494.0f -
						Min<unsigned char>(GolfClub().GetIndex(), 13) * 26.0f,
					m_remainTimeY + 5.0f, 1.0f, 0x22);
			}
		}
	}

	DrawOverlay("\xc1\xa4\xba\xb8\xc7\xa5\xbd\xc3\xbf\xf8",
		WRect(5.0f, m_remainTimeY - 52.0f, 128.0f, 128.0f), 255, 0);

	if (m_pBase)
	{
		m_pBase->Display();
	}

	if (GolfBall().m_playFrame < 0)
	{
		unsigned long color;
		switch (GolfBall().m_groundType)
		{
		case 0:
		case 1:
		case 2:
		case 6:
		case 7:
			color = 0;
			break;
		case 3:
			color = 0x80E5CA77;
			break;
		case 5:
			color = 0x80FFFFFF;
			break;
		default:
			color = 0x8078E577;
			break;
		}

		if (color)
		{
			sOverlay* overlay = m_overlays.Find("\xba\xbc\xbb\xf3\xc5\xc2");
			if (overlay && overlay->m_pOverlay)
			{
				const float& cut = m_ballStateCut;
				overlay->m_pOverlay->Render(g_view,
					WRect(0.0f, cut, 1.0f, 1.0f - cut),
					WRect(26.0f, m_remainTimeY - 28.0f + cut * 64.0f, 64.0f,
						64.0f * (1.0f - cut)),
					0, color, 0.0f, 0);
			}
		}
	}

	if (m_pClubTypeInfo)
	{
		m_pClubTypeInfo->Display();
	}
}

void CScreen::DrawCommandArrow(void)
{
	if (!(GOLFDOC()->GameMode() & 0x60))
	{
		return;
	}

	float y = 200.0f;
	std::string name;

	std::list<unsigned long>::iterator it = GOLFDOC()->m_commandList.begin();
	while (it != GOLFDOC()->m_commandList.end())
	{
		if (*it == 0x10)
		{
			++it;
			continue;
		}

		switch (*it)
		{
		case 1:
			name = "UP";
			break;
		case 2:
			name = "DOWN";
			break;
		case 4:
			name = "LEFT";
			break;
		case 8:
			name = "RIGHT";
			break;
		case 0x40:
			name = "INVALID";
			break;
		case 0x20:
			name = "INVALID";
			y -= 22.0f;
			break;
		}

		++it;
		if (it != GOLFDOC()->m_commandList.end() && *it == 0x10)
		{
			name += "_S";
		}

		if (m_pCommandArrowUI)
		{
			m_pCommandArrowUI->Render(name.c_str(), y, m_remainTimeY - 30.0f,
				1.0f, 0);
		}
		y += 22.0f;
	}
}

void CScreen::DrawTutorialUI(void)
{
	WRect rect;

	if (GOLFDOC()->m_tutorialMode == 15)
	{
		return;
	}

	if (!WSingleton<CCapturedBg>::Instance()->IsCaptured())
	{
		if (GOLFDOC()->m_tutorialMode == 0 && m_tutoMsgShown)
		{
			DrawOverlay("\xc1\xb6\xc1\xd8\xbf\xb5\xbf\xaa",
				WRect(g_view->GetWidth() * 0.5f - 64.0f,
					g_view->GetHeight() * 0.5f - 64.0f, 128.0f, 128.0f),
				0xFF, 0);
		}

		if ((WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 0xB ||
				WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType ==
					0xC) &&
			m_pTutoEscMsgUI)
		{
			m_pTutoEscMsgUI->Render("MSG0", 150.0f, m_remainTimeY + 33.0f, 1.0f,
				0);
			m_pTutoEscMsgUI->Render("MSG1", 406.0f, m_remainTimeY + 33.0f, 1.0f,
				0);
		}

		if (GOLFDOC()->m_tutorialMode == 5)
		{
			if (CItemWindow::IsClosed())
			{
				DrawOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_normal", rect, 0xFF, 0);
			}
			else
			{
				DrawOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_item", rect, 0xFF, 0);
			}
		}

		if (m_tutoMsgShown)
		{
			if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 0xC)
			{
				DrawOverlay(
					MakeStr(
						"\xc4\xf9\xbd\xba\xc6\xae\xb9\xcc\xbc\xc7\xc1\xa4\xba\xb8%d",
						GOLFDOC()->m_tutorialMode),
					WRect(g_view->GetWidth() - 256.0f, 26.0f, 256.0f, 256.0f),
					0xFF, 0);
			}
			else
			{
				DrawOverlay(
					"\xc4\xf9\xbd\xba\xc6\xae\xb9\xcc\xbc\xc7\xc1\xa4\xba\xb8"
					"0",
					WRect(g_view->GetWidth() - 256.0f, 26.0f, 256.0f, 128.0f),
					0xFF, 0);
			}
		}

		if (m_pTutoTitleUI)
		{
			m_pTutoTitleUI->SetColor((m_tutoTitleAlpha << 24) | 0xFFFFFF);

			m_pTutoTitleUI->Render(
				MakeStr("\xc5\xb8\xc0\xcc\xc6\xb2_%d", m_tutorialNo + 1),
				m_tutoTitleX + 20.0f, m_tutoTitleY, 1.0f, 0);
		}
	}

	int index;
	switch (GOLFDOC()->m_tutorialMode)
	{
	case 1:
		index = 0;
		break;
	case 2:
		index = 1;
		break;
	case 3:
		index = 2;
		break;
	case 4:
		index = 3;
		break;
	case 5:
		index = 4;
		break;
	case 8:
		index = 5;
		break;
	case 9:
		index = 6;
		break;
	case 10:
		index = 7;
		break;
	case 11:
		index = 8;
		break;
	case 12:
		index = 9;
		break;
	case 13:
		index = 10;
		break;
	case 14:
		index = 11;
		break;
	default:
		return;
	}

	if (m_missionTitleState == 3)
	{
		return;
	}

	if (index == 6)
	{
		m_pTutorialUIs[6]->Render("MissionTitle", m_missionTitleX,
			m_missionTitleY + 20.0f, m_missionTitleAlpha, 2);
	}
	else
	{
		m_pTutorialUIs[index]->Render("MissionTitle", m_missionTitleX,
			m_missionTitleY, m_missionTitleAlpha, 2);
	}
}
