#include "minatl.h"
ILFILLTU_GOLFRULE
#include "voiceitem.h"
#include "fxsequence.h"
#include "golfball.h"
#include "quadtree.h"
#include "club.h"
#include "mousecursor.h"
#include "emoticondlg.h"
#include "wind.h"
#include "headicon.h"
#include "helpmsg.h"
#include "autotarget.h"
#include "capturedbg.h"
#include "rival.h"
#include "camerawork.h"
#include "randomhouse.h"
#include "netresourcemanager.h"
#include "clientsetting.h"
#include "slicestr.h"
#include "onelineboard.h"
#include "rankinginfo.h"
#include "shufflebonus.h"
#include "messengerdlg.h"
#include "../../shared/encryption.h"
#include "replaydlg.h"
#include "effectmanager.h"
#include "powergauge.h"
#include "treasure.h"
#include "cardsystem.h"
#include "logininfo.h"
#include "simpleoverlay.h"
#include "golfrulebase.h"
#include "approachresultdlg.h"
#include "chatmsg.h"
#include "onelinereqdlg.h"
#include "reportdlg.h"
#include "optiondlg.h"
#include "../../shared/sharedtables.h"
#include "s5/utilities.h"
#include "gatewayactor.h"
#include "mathconsts.h"
#include "fx.h"
#include "shadow.h"
#include "quitwindow.h"
#include "itemwindow.h"
#include "user_info.h"
#include "frcombobox.h"
#include "frdesktop.h"
#include "kbdmanualdlg.h"
#include "bongdarishopdlg.h"
#include "giftalarmdlg.h"
#include "registry.h"
#include "codetinker.h"
#include "s5/classicserver/classicserver.h"
#include "../../shared/s5/sharedutilities.h"
#include <io.h>

extern "C" __declspec(dllimport) int __cdecl mkdir(const char*);
extern Fresh* g_pFresh;
extern bool g_bQuit;
extern int g_iSendFailType;
extern int g_iRecvDisconCode;
extern int g_iSendPacketType;
extern int g_iRecvPacketType;
bool IsMyTurn(bool bCheck);
void RequestUccInfo(sUserInfo& info);
std::string GetMyComputerName();
extern const char* ball_stop_replay_data_name;

ILFILLTU_GOLFRULE3

const char* g_bgmPutting[] = {
	"putting.mp3",
	"putting2.mp3",
};

const char* g_plSound[] = {
	"shot_best_timing",
	"shot_best_timing",
	"shot_good_timing",
	"shot_normal_timing",
	"shot_bad_timing",
	"\xb0\xb6\xb7\xaf\xb8\xae_\xb9\xda\xbc\xf6",
	"\xc6\xce\xbe\xdf",
	"pangya_mike",
};

const char* g_szCommonCharMotion[] = {
	"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
	"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
	"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
};

const char* g_szLuciaCharMotion[] = {
	"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
	"\xb4\xf5\xba\xed\xba\xb8\xb1\xe2\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
	"\xb4\xf5\xba\xed\xba\xb8\xb1\xe2\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
};

sLadderItem g_ladderItem[] = {
	{ 0,  15 },
	{ 1,  13 },
	{ 10, 18 },
	{ 11, 12 },
	{ 4,  18 },
	{ 12, 24 },
};

#include "frcombobox.h"
#include "user_info.h"
#include "quitwindow.h"

extern Fresh* g_pFresh;

static int GetShotEffect(bool bLocal, bool bPowerShot, bool bSpecial,
	bool bExtra)
{
	int index = 0;

	if (bPowerShot)
		index = 4;
	else if (bSpecial)
		index = 2;

	if (bExtra)
		index += 1;

	if (index >= 5)
		index = 5;

	if (bLocal)
	{
		int effect[6] = { 12, 3, 14, 3, 3, 3 };
		return effect[index];
	}
	else
	{
		int effect[6] = { 4, 3, 4, 3, 3, 3 };
		return effect[index];
	}
}

static const char* GetCharMotion(int score, unsigned long charType, int stroke)
{
	int index = 0;
	if (stroke != 1)
	{
		if (score < -3 || score > 3)
			index = 7;
		else
			index = min(score + 4, 7);
	}

	return charType == 8 ? g_szLuciaCharMotion[index]
						 : g_szCommonCharMotion[index];
}

namespace
{
	CGolfBall::eBallState GetInitBallState()
	{
		unsigned char clubType = GolfClub().GetType();

		if (clubType == 3)
			return CGolfBall::BALL_ROLL;

		unsigned long charType =
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tid &
			0x3ffffff;

		if (GOLFDOC()->GetBar().grade < 2)
		{
			bool bOver =
				GOLFDOC()->GetBar().impact > GOLFDOC()->GetBar().center;
			if (clubType == 2)
				return bOver ? CGolfBall::BALL_ROLL : CGolfBall::BALL_BOUNCE;

			if (clubType == 0)
			{
				if (bOver || charType == 1 || charType == 5 || charType == 6)
					return CGolfBall::BALL_STOP;

				return CGolfBall::BALL_ROLL;
			}

			if (clubType == 1)
			{
				if (bOver)
					return CGolfBall::BALL_ROLL;

				if (charType == 5 || charType == 6)
					return CGolfBall::BALL_STOP;
			}
		}

		return CGolfBall::BALL_BOUNCE;
	}
}

namespace
{
	int GetBarClickTiming()
	{
		CGolfDoc* pDoc = GOLFDOC();
		switch (pDoc->GetBar().grade)
		{
		case 3:
			return pDoc->GetBar().Delta() > 0.0f ? 1 : -1;

		case 2:
			return pDoc->GetBar().impact > s_barStart ? 2 : -2;

		case 0:
		case 1:
			return pDoc->GetBar().Delta() > 0.0f ? 3 : -3;
		}

		return 0;
	}
}

namespace
{
	float GetBarClickPowerFactor()
	{
		if (GolfClub().GetType() != 3)
		{
			CGolfDoc* pDoc = GOLFDOC();
			switch (pDoc->GetBar().grade)
			{
			case 4:
				return sqrtf(GetPVS().GetMaxPowerOnGround());

			case 3:
			{
				float diff = pDoc->GetBar().Delta();
				float ratio;
				if (diff > 0.0f)
					ratio = diff /
						(GolfClub().GetNarrowAccuracy() + s_barStart -
							pDoc->GetBar().center);
				else
					ratio = diff /
						(pDoc->GetBar().center -
							(s_barStart - GolfClub().GetNarrowAccuracy()));

				return sqrtf(
					GetPVS().GetMaxPowerOnGround() * (1.0f - Abs(ratio)) +
					GetPVS().GetMinPowerOnGround() * Abs(ratio));
			}

			case 2:
			{
				float ratio = (Abs(pDoc->GetBar().impact - s_barStart) -
								  GolfClub().GetNarrowAccuracy()) /
					(GolfClub().GetWideAccuracy() -
						GolfClub().GetNarrowAccuracy());
				return sqrtf(GetPVS().GetMinPowerOnGround() * (1.0f - ratio) +
					GetPVS().GetMinPowerOnGround() * ratio);
			}
			}
		}

		return 1.0f;
	}
}

namespace
{
	struct BallSpeedFactor
	{
		BallSpeedFactor(unsigned long charType, const CGolfBall& ball,
			float angle);

		float pitch;
		float angle;
		float curve;
		float spin;
		WVector vel;
	};

	struct sDetermineWhetherToEnableFiledItem
	{
		~sDetermineWhetherToEnableFiledItem();
	};
}

namespace
{
	BallSpeedFactor::BallSpeedFactor(unsigned long charType,
		const CGolfBall& ball, float angle)
		: pitch(0.0f),
		  angle(0.0f),
		  curve(ball.m_initCurve),
		  spin(ball.m_initSpin)
	{
		vel.Reset();
		AfxGetTask()->GetMainActor()->SendMsg(NULL, "Player", 0x110,
			(int)&this->angle, 0, 0, 0);
		this->angle += angle;

		if (GolfClub().GetType() == 3)
			return;

		if (GOLFDOC()->GetBar().grade == 4)
			pitch -= curve * (20.0f * g_DEGTORAD);

		if (GOLFDOC()->GetBar().grade == 3)
		{
			CGolfDoc* pDoc = GOLFDOC();
			float diff = pDoc->GetBar().Delta();
			float ratio;
			if (diff > 0.0f)
				ratio = diff /
					(GolfClub().GetNarrowAccuracy() + s_barStart -
						pDoc->GetBar().center);
			else
				ratio = diff /
					(pDoc->GetBar().center -
						(s_barStart - GolfClub().GetNarrowAccuracy()));

			pitch -= curve * (20.0f * g_DEGTORAD);

			if (!IsLocalContent((localContentType_t)0x50) ||
				ball.m_initSpecial == 0)
				curve -= ratio * 0.2f;
		}

		if (GOLFDOC()->GetBar().grade == 2)
		{
			float ratio = (Abs(GOLFDOC()->GetBar().impact - s_barStart) -
							  GolfClub().GetNarrowAccuracy()) /
				(GolfClub().GetWideAccuracy() - GolfClub().GetNarrowAccuracy());

			if (!IsLocalContent((localContentType_t)0x50) ||
				ball.m_initSpecial == 0)
			{
				curve = 0.2f;
				spin = 0.0f;
			}

			if (GOLFDOC()->GetBar().impact > s_barStart)
			{
				curve *= -1.0f;
				pitch -= ratio * (10.0f * g_DEGTORAD);
			}
			else
			{
				pitch += ratio * (10.0f * g_DEGTORAD);
			}
		}

		if (GOLFDOC()->GetBar().grade == 0 || GOLFDOC()->GetBar().grade == 1)
		{
			curve = 0.0f;
			spin = 0.0f;

			switch (GolfClub().GetType())
			{
			case 0:
				if (GOLFDOC()->GetBar().impact > GOLFDOC()->GetBar().center)
				{
					vel.Reset();
				}
				else if (charType == 0x4000001 || charType == 0x4000006 ||
					charType == 0x4000005)
				{
					vel.Reset();
				}
				else
				{
					vel = WVector(0.0f, 0.0f, 15.0f);
					pitch += 40.0f * g_DEGTORAD;
				}
				break;

			case 1:
				if (GOLFDOC()->GetBar().impact > GOLFDOC()->GetBar().center)
					vel = WVector(0.0f, 0.0f, 20.0f);
				else
					vel = RotMat(80.0f * g_DEGTORAD, 0).za * 50.0f;

				if (charType == 0x4000006 || charType == 0x4000005)
					vel.Reset();
				break;

			case 2:
				if (GOLFDOC()->GetBar().impact > GOLFDOC()->GetBar().center)
					vel = WVector(0.0f, 0.0f, 5.0f);
				else
					vel = RotMat(80.0f * g_DEGTORAD, 0).za * 20.0f;
				break;
			}
		}

		CGolfDoc* pDoc = GOLFDOC();
		if (pDoc->GetBar().grade > 1)
		{
			if (pDoc->GetBar().grade == 4 && pDoc->GetBar().bFullPower &&
				GolfClub().PowerShotType() != 0 &&
				!(ball.m_initSpecial & 0x70) &&
				IsLocalContent((localContentType_t)0x2f) &&
				PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
				return;

			WVector slope;
			AfxGetTask()->GetMainActor()->SendMsg(NULL, "Screen", 0x1c1,
				(int)&slope, 0, 0, 0);

			if (ball.m_initSpecial & 0x40)
				curve -= slope.x * 0.15f;
			else
				curve -= slope.x * 0.5f;
		}
	}
}

namespace
{
	sDetermineWhetherToEnableFiledItem::~sDetermineWhetherToEnableFiledItem()
	{
		int count = 0;
		AfxGetTask()->GetActor("GroundItemMan")
			<< MsgObject(NULL, 0x27c, (int)&count, 0, 0, 0, 0);

		if (count <= 0)
			return;

		unsigned char gameType = Doc()->m_golfGame.gameType;
		unsigned char stroke = gameType == GAME_TYPE_TEAM
			? TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
				  ->stroke[GOLFDOC()->m_currentHole - 1]
			: PLAYER(GOLFDOC()->m_currentPlayer)
				  ->stroke[GOLFDOC()->m_currentHole - 1];

		int bEnable = TRUE;
		int diff = stroke - GOLFDOC()->GetHoleData().par;

		if (diff >= 0)
		{
			bEnable = FALSE;

			if (diff == 0)
			{
				CChatMsg::Instance()->AddChatMsg(
					"(\xbe\xcb\xb8\xb2) \xc0\xcc\xba\xec\xc6\xc4\xb0\xa1 \xb5\xc7\xbe\xee \xc7\xca\xb5\xe5 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbb\xe7\xb6\xf3\xc1\xfd\xb4\xcf\xb4\xd9.",
					0xffff7878, false, false);
			}
		}

		AfxGetTask()->GetActor("GroundItemMan")
			<< MsgObject(NULL, 0x27b, bEnable, 0, 0, 0, 0);
	}
}

IMPLEMENT_ACTOR(CGolfRule, CTaskMain)
BEGIN_FRESH_MSGMAP(CGolfRule, CTaskMain)
ON_FRESH_VI("MAIN.chatview", FRCMD_INIT, CGolfRule::OnChatViewInit)
ON_FRESH_VI("MAIN.emoticon", FRCMD_INIT, CGolfRule::OnEmoticonInit)
ON_FRESH_VV("MAIN.emoticon", FRCMD_LBUTTONUP, CGolfRule::OnEmoticonBtnUp)
ON_FRESH_VI("MAIN.language", FRCMD_INIT, CGolfRule::OnLanguageInit)
ON_FRESH_VI("MAIN.report", FRCMD_INIT, CGolfRule::OnReportInit)
ON_FRESH_VV("MAIN.report", FRCMD_LBUTTONUP, CGolfRule::OnReportBtnUp)
ON_FRESH_VI("MAIN.chattarget", FRCMD_INIT, CGolfRule::OnChatTargetInit)
ON_FRESH_VI("MAIN.chattarget", FRCMD_LBUTTONDOWN,
	CGolfRule::OnChatTargetBtnDown)
ON_FRESH_BI("MAIN.chattarget", FRCMD_ENTERKEY, CGolfRule::OnChatTargetEnterKey)
ON_FRESH_VI("MAIN.chatinput", FRCMD_INIT, CGolfRule::OnChatInputInit)
ON_FRESH_BI("MAIN.chatinput", FRCMD_ENTERKEY, CGolfRule::OnChatInputEnterKey)
ON_FRESH_VI("MAIN.spcard", FRCMD_INIT, CGolfRule::OnSpecialCardInit)
ON_FRESH_VV("MAIN.spcard", FRCMD_LBUTTONUP, CGolfRule::OnSpecialCardBtnUp)
ON_FRESH_VI("MAIN.spcard", FRCMD_HOVERON, CGolfRule::OnTopBarHoverOn)
ON_FRESH_VI("MAIN.spcard", FRCMD_HOVEROFF, CGolfRule::OnTopBarHoverOff)
ON_FRESH_VI("MAIN.myinfo", FRCMD_INIT, CGolfRule::OnMyInfoInit)
ON_FRESH_VV("MAIN.myinfo", FRCMD_LBUTTONUP, CGolfRule::OnMyInfoBtnUp)
ON_FRESH_VI("MAIN.myinfo", FRCMD_HOVERON, CGolfRule::OnTopBarHoverOn)
ON_FRESH_VI("MAIN.myinfo", FRCMD_HOVEROFF, CGolfRule::OnTopBarHoverOff)
ON_FRESH_VI("MAIN.option", FRCMD_INIT, CGolfRule::OnOptionInit)
ON_FRESH_VV("MAIN.option", FRCMD_LBUTTONUP, CGolfRule::OnOptionBtnUp)
ON_FRESH_VI("MAIN.option", FRCMD_HOVERON, CGolfRule::OnTopBarHoverOn)
ON_FRESH_VI("MAIN.option", FRCMD_HOVEROFF, CGolfRule::OnTopBarHoverOff)
ON_FRESH_VI("MAIN.caddiebook", FRCMD_INIT, CGolfRule::OnCaddieBookInit)
ON_FRESH_VV("MAIN.caddiebook", FRCMD_LBUTTONUP, CGolfRule::OnCaddieBookBtnUp)
ON_FRESH_VI("MAIN.unplayable", FRCMD_INIT, CGolfRule::OnUnplayableInit)
ON_FRESH_VV("MAIN.unplayable", FRCMD_LBUTTONUP, CGolfRule::OnUnplayableBtnUp)
ON_FRESH_VI("MAIN.pause", FRCMD_INIT, CGolfRule::OnPauseInit)
ON_FRESH_VV("MAIN.pause", FRCMD_LBUTTONUP, CGolfRule::OnPauseBtnUp)
ON_FRESH_VI("MAIN.record", FRCMD_INIT, CGolfRule::OnChatRecordInit)
ON_FRESH_VV("MAIN.record", FRCMD_LBUTTONUP, CGolfRule::OnChatRecordBtnUp)
ON_FRESH_VI("MAIN.onelineview", FRCMD_INIT, CGolfRule::OnOnelineViewInit)
ON_FRESH_VI("MAIN.onelineview", FRCMD_OWNERDRAW,
	CGolfRule::OnOnelineViewOwnerDraw)
ON_FRESH_VI("MAIN.onelinebtn", FRCMD_INIT, CGolfRule::OnOnelineBtnInit)
ON_FRESH_VV("MAIN.onelinebtn", FRCMD_LBUTTONUP, CGolfRule::OnOnelineBtnUp)
ON_FRESH_VI("MAIN.tooltip", FRCMD_OWNERDRAW, CTaskMain::OnBarTooltipOwnerDraw)

ON_FRESH_VI("MAIN.messenger", FRCMD_INIT, CGolfRule::OnMessengerBtnInit)
ON_FRESH_VV("MAIN.messenger", FRCMD_LBUTTONUP, CGolfRule::OnMessengerBtnUp)
ON_FRESH_VI("MAIN.messenger", FRCMD_HOVERON, CGolfRule::OnTopBarHoverOn)
ON_FRESH_VI("MAIN.messenger", FRCMD_HOVEROFF, CGolfRule::OnTopBarHoverOff)
ON_FRESH_VI("MAIN.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("MAIN.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("MAIN.messenger_desc", FRCMD_INIT,
	CGolfRule::OnMessengerBubbleEditInit)
ON_FRESH_VI("MAIN.messenger_alarm", FRCMD_INIT,
	CGolfRule::OnMessenger_AlarmBtnInit)
ON_FRESH_VV("MAIN.messenger_alarm", FRCMD_LBUTTONUP,
	CGolfRule::OnMessenger_AlarmBtnUp)
ON_FRESH_VI("MAIN.gift_alarm", FRCMD_INIT,
	CGolfRule::OnMessenger_GiftAlarmBtnInit)
ON_FRESH_VV("MAIN.gift_alarm", FRCMD_LBUTTONUP,
	CGolfRule::OnMessenger_GiftAlarmBtnUp)
ON_FRESH_VI("MAIN.replay_status", FRCMD_INIT, CGolfRule::OnReplayStatusInit)

ON_FRESH_VI("MAIN.predict_normal", FRCMD_INIT,
	CGolfRule::OnRadioPredictNormalShotInit)
ON_FRESH_VI("MAIN.predict_tomahwak", FRCMD_INIT,
	CGolfRule::OnRadioPredictTomahwakShotInit)
ON_FRESH_VI("MAIN.predict_cobra", FRCMD_INIT,
	CGolfRule::OnRadioPredictCobraShotInit)
ON_FRESH_VI("MAIN.predict_spike", FRCMD_INIT,
	CGolfRule::OnRadioPredictSpikeShotInit)
ON_FRESH_VV("MAIN.predict_normal", FRCMD_LBUTTONDOWN,
	CGolfRule::OnRadioPredictNormalShotDown)
ON_FRESH_VV("MAIN.predict_tomahwak", FRCMD_LBUTTONDOWN,
	CGolfRule::OnRadioPredictTomahwakShotDown)
ON_FRESH_VV("MAIN.predict_cobra", FRCMD_LBUTTONDOWN,
	CGolfRule::OnRadioPredictCobraShotDown)
ON_FRESH_VV("MAIN.predict_spike", FRCMD_LBUTTONDOWN,
	CGolfRule::OnRadioPredictSpikeShotDown)
ON_FRESH_VI("MAIN.predict_normal", FRCMD_OWNERDRAW,
	CGolfRule::OnRadioPredictNormalShotDraw)
ON_FRESH_VI("MAIN.predict_tomahwak", FRCMD_OWNERDRAW,
	CGolfRule::OnRadioPredictTomahwakShotDraw)
ON_FRESH_VI("MAIN.predict_cobra", FRCMD_OWNERDRAW,
	CGolfRule::OnRadioPredictCobraShotDraw)
ON_FRESH_VI("MAIN.predict_spike", FRCMD_OWNERDRAW,
	CGolfRule::OnRadioPredictSpikeShotDraw)
END_FRESH_MSGMAP()

int g_iRecvPacketGR;
int g_iSendPacketGR;
void CGolfRule::OnChatViewInit(int id)
{
	m_pChatView = DYNAMIC_CAST(FrEdit, (FrWnd*)id);
	if (m_pChatView)
	{
		m_pChatView->HidePrivacy(true);

		WRect rect = m_pChatView->GetRect();
		int lines = (COption::Instance()->gIsChatLineEntended() ||
						Doc()->m_roomInfo.nUserLimit >= 100)
			? 10
			: 5;

		rect.h = lines * 19.0f + 8.0f;
		rect.y = COption::Instance()->gGetBar_Y() - 100.0f - rect.h;
		m_pChatView->SetRect(rect);
		m_pChatView->GetScrollBar()->Resize(lines, 1, false);
		m_pChatView->GetScrollBar()->SetVisible(false);
		HandleMsg(MsgObject(NULL, 99, 0, 0, 0, 0, 0));
	}
}

void CGolfRule::OnEmoticonInit(int id)
{
	m_pEmoticonBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pEmoticonBtn)
	{
		m_pEmoticonBtn->SetVisible(false);

		WPoint pos;
		pos.x = m_pEmoticonBtn->GetRect().x;
		pos.y = COption::Instance()->gGetBar_Y() + 16.0f;
		m_pEmoticonBtn->MoveWindow(pos);
	}
}

void CGolfRule::OnEmoticonBtnUp()
{
	if (!m_pEmoticonDlg)
	{
		m_pEmoticonDlg =
			CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon");
		if (m_pEmoticonDlg)
		{
			WPoint pos(0.0f, 0.0f);

			if (m_pEmoticonBtn)
			{
				pos.x = m_pEmoticonBtn->GetRect().x - 200.0f;
				pos.y = m_pEmoticonBtn->GetRect().y - 260.0f;
			}

			if (!g_pFresh->GetManager()->IsKeyExclusive())
				g_pFresh->GetManager()->SetExclusiveKey(true);

			m_pEmoticonDlg->Open((FRESH_PFN_RESULT)&CGolfRule::OnEmoticonResult,
				pos, 3);
		}
	}
	else
	{
		m_pEmoticonDlg->Close(FrCANCEL, true);
	}
}

void CGolfRule::OnLanguageInit(int id)
{
	m_pLanguage = DYNAMIC_CAST(FrArea, (FrWnd*)id);
	if (m_pLanguage)
	{
		m_pLanguage->SetVisible(false);

		if (IsLocalCountry(1) && g_ime->IsAlphaNumericMode())
			m_pLanguage->SetBgImg("chat_eng");
		else
			m_pLanguage->SetBgImg("chat_kor");

		WPoint pos;
		pos.x = m_pLanguage->GetRect().x;
		pos.y = COption::Instance()->gGetBar_Y() + 16.0f;
		m_pLanguage->MoveWindow(pos);
	}
}

void CGolfRule::OnReportInit(int id)
{
	m_pReportBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pReportBtn)
	{
		m_pReportBtn->SetVisible(false);

		WPoint pos;
		pos.x = m_pReportBtn->GetRect().x;
		pos.y = COption::Instance()->gGetBar_Y() + 16.0f;
		m_pReportBtn->MoveWindow(pos);
	}
}

void CGolfRule::OnReportBtnUp()
{
	if (GOLFDOC()->m_bStopRequest)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		pForm->SetMessage(
			"\xc7\xf6\xc0\xe7 \xb0\xd4\xc0\xd3\xbf\xa1 \xb4\xeb\xc7\xd8\xbc\xad \xc0\xcc\xb9\xcc \xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa2\xbc\xf6\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 0);
		return;
	}

	if (!m_pReportDlg)
	{
		m_pReportDlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");
		m_pReportDlg->SetMessage(
			"\xbd\xc5\xb0\xed\xb8\xa6 \xc7\xcf\xbd\xc3\xb8\xe9 \xc7\xf6\xc0\xe7\xb0\xd4\xc0\xd3\xc0\xc7 \xb4\xeb\xc8\xad \xb3\xbb\xbf\xeb\xc0\xcc \xb8\xf0\xb5\xce \xbf\xee\xbf\xb5\xc0\xda\xbf\xa1\xb0\xd4 \xbd\xc5\xb0\xed\xb0\xa1 \xb5\xcb\xb4\xcf\xb4\xd9.\x0a\x0a\xb4\xeb\xc8\xad \xb3\xbb\xbf\xeb \xc1\xdf \xc3\xb3\xb9\xfa\xc7\xd8\xb4\xe7\xbb\xe7\xc7\xd7\xc0\xcc \xc0\xd6\xb4\xc2 \x5c"
			"c0xffff0000\x5c"
			"c\xb8\xf0\xb5\xe7 \xbe\xc6\xc0\xcc\xb5\xf0\x5c"
			"c0xff000000\x5c"
			"c\xb4\xc2 \xba\xed\xb7\xb0 \xc3\xb3\xb8\xae\xb8\xa6 \xb4\xe7\xc7\xcf\xb0\xd4 \xb5\xcb\xb4\xcf\xb4\xd9.\x0a\x0a\xbd\xc5\xb0\xed \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		m_pReportDlg->Open((FRESH_PFN_RESULT)&CGolfRule::OnReportResult, 0);
	}
}

void CGolfRule::OnChatTargetInit(int id)
{
	m_pChatTarget = DYNAMIC_CAST(FrComboBox, (FrWnd*)id);
	if (m_pChatTarget)
	{
		m_pChatTarget->SetVisible(false);
		Doc()->InitReservedChatList(m_pChatTarget,
			(Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM ||
				Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
				? (eReservedChatPartner)3
				: (eReservedChatPartner)2);

		WPoint pos;
		pos.x = m_pChatTarget->GetRect().x;
		pos.y = COption::Instance()->gGetBar_Y() + 14.0f;
		m_pChatTarget->MoveWindow(pos);
	}
}

void CGolfRule::OnChatTargetBtnDown(int id)
{
	if (m_pChatInput)
		m_pChatInput->SetKeyFocus(true);
}

bool CGolfRule::OnChatTargetEnterKey(int id)
{
	if (m_pChatTarget && !strlen(m_pChatTarget->GetLine(1, false)))
		m_pChatTarget->SetLine(1,
			K2L_Compatibility("\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4"), 0, false, 0);

	if (m_pChatInput)
		m_pChatInput->SetKeyFocus(true);

	return false;
}

void CGolfRule::OnChatInputInit(int id)
{
	m_pChatInput = DYNAMIC_CAST(FrEdit, (FrWnd*)id);
	if (m_pChatInput)
	{
		m_pChatInput->SetVisible(false);

		WPoint pos;
		pos.x = m_pChatInput->GetRect().x;
		pos.y = COption::Instance()->gGetBar_Y() + 14.0f;
		m_pChatInput->MoveWindow(pos);
	}
}

bool CGolfRule::OnChatInputEnterKey(int id)
{
	if (strlen((const char*)id))
	{
		if (!GOLFDOC()->m_bShowChat)
		{
			CChatMsg::Instance()->AddChatMsg(
				"\xc1\xf6\xb1\xdd\xc0\xba \xb4\xeb\xc8\xad\xc3\xa2\xc0\xbb \xbc\xfb\xb1\xe4 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xffff7878, true, false);
			return true;
		}

		if (!Doc()->UTIL_SendChatMessage(m_pChatTarget, (const char*)id, false))
		{
			m_pChatTarget->SetLine(1, "\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4", 0,
				false, 0);
			CChatMsg::Instance()->AddChatMsg(
				"\xb1\xd3\xb8\xbb \xbb\xf3\xb4\xeb\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \x22\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4\x22\xb7\xce \xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9.",
				0xffff7878, true, false);
		}

		if (OnlinePlay())
		{
			CHeadIcon::Instance()->SendIcon(0xffff);
			return true;
		}

		std::string target = m_pChatTarget->GetLine(1, false);
		if (target == "\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4")
		{
			std::string msg =
				Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].info.sNick;
			msg += " : \x08";
			msg += (const char*)id;

			if (Doc()->m_gameMode == 5)
			{
				char buf[1024] = { 0 };
				strncpy(buf, (const char*)id,
					strlen((const char*)id) < 1024 ? strlen((const char*)id)
												   : 1023);
				if (Doc()->m_chatManager.Filtering(buf))
				{
					CChatMsg::Instance()->AddChatMsg(
						std::string(
							"(\xbe\xcb\xb8\xb2)\xbf\xe5\xbc\xb3\xc0\xba \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."),
						0xffff8000, false, false);
				}
				else
				{
					CChatMsg::Instance()->AddChatMsg(msg, 0xffffcc00, false,
						false);
				}
			}
			else
			{
				CChatMsg::Instance()->AddChatMsg(msg, 0xffffcc00, false, false);
			}
		}
	}
	else
	{
		CChatMsg::Instance()->Close(true, true);
	}

	return true;
}

void CGolfRule::OnSpecialCardInit(int id)
{
	m_pOverBarSpecialCard = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pOverBarSpecialCard)
	{
		m_pOverBarSpecialCard->SetVisible(false);
		m_pOverBarSpecialCard->EnableHover(true, 0.0f);
		m_pOverBarSpecialCard->EnableBlink(
			CCardManager::Instance()->ShowSPCardAlarm());

		WPoint pos;
		pos.x = g_view->GetWidth() - 106.0f;
		pos.y = m_pOverBarSpecialCard->GetRect().y;
		m_pOverBarSpecialCard->MoveWindow(pos);
	}
}

void CGolfRule::OnSpecialCardBtnUp()
{
	OnOverBar_SpecialCardUp();
}

void CGolfRule::OnMyInfoInit(int id)
{
	m_pMyInfoBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pMyInfoBtn)
	{
		m_pMyInfoBtn->SetVisible(false);
		m_pMyInfoBtn->EnableHover(true, 0.0f);

		WPoint pos;
		pos.x = g_view->GetWidth() - 54.0f;
		pos.y = m_pMyInfoBtn->GetRect().y;
		m_pMyInfoBtn->MoveWindow(pos);
	}
}

void CGolfRule::OnMyInfoBtnUp()
{
	if (CUserInfo::IsInstantiated())
	{
		CUserInfo::Instance()->SetInfo(MyUID(), MyGuid(false), true, true,
			false, false, std::string(Doc()->m_myInfo.info.sNick));
	}

	OnBarHoverOff(0);
}

void CGolfRule::OnOptionInit(int id)
{
	m_pOptionBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pOptionBtn)
	{
		m_pOptionBtn->SetVisible(false);
		m_pOptionBtn->EnableHover(true, 0.0f);

		WPoint pos;
		pos.x = g_view->GetWidth() - 29.0f;
		pos.y = m_pMyInfoBtn->GetRect().y;
		m_pOptionBtn->MoveWindow(pos);
	}
}

void CGolfRule::OnOptionBtnUp()
{
	if (CQuitWindow::IsInstantiated())
	{
		CQuitWindow::Instance()->CreateOptionWindow();
	}

	OnBarHoverOff(0);
}

void CGolfRule::OnCaddieBookInit(int id)
{
	m_pCaddieBookBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pCaddieBookBtn)
	{
		m_pCaddieBookBtn->SetVisible(false);

		WPoint pos;
		pos.x = g_view->GetWidth() - 423.0f;
		pos.y = m_pCaddieBookBtn->GetRect().y;
		m_pCaddieBookBtn->MoveWindow(pos);

		ResetUiPos_Board();
	}
}

void CGolfRule::OnCaddieBookBtnUp()
{
	SendMsg(this, "GuideBook", 0x12a, 0, 0, 0, 0);
}

void CGolfRule::OnUnplayableInit(int id)
{
	m_pUnplayableBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pUnplayableBtn)
	{
		m_pUnplayableBtn->SetVisible(false);

		WPoint pos;
		pos.x = g_view->GetWidth() - 398.0f;
		pos.y = m_pUnplayableBtn->GetRect().y;
		m_pUnplayableBtn->MoveWindow(pos);
	}
}

void CGolfRule::OnUnplayableBtnUp()
{
	if (GOLFDOC()->m_bNoStat)
	{
		if (CHelpMsg::IsInstantiated())
			CHelpMsg::Instance()->SetHelpMsg(0x17, NULL);
		return;
	}

	if (GOLFDOC()->m_gameMode & 0x48108)
		SendMsg(this, "Screen", 0x1cb, 0, 0, 0, 0);
}

void CGolfRule::OnPauseInit(int id)
{
	m_pPauseBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pPauseBtn)
	{
		m_pPauseBtn->SetVisible(false);

		WPoint pos;
		pos.x = g_view->GetWidth() - 373.0f;
		pos.y = m_pPauseBtn->GetRect().y;
		m_pPauseBtn->MoveWindow(pos);
	}
}

void CGolfRule::OnPauseBtnUp()
{
	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_APPROACH:
		if (CHelpMsg::IsInstantiated())
			CHelpMsg::Instance()->SetHelpMsg(0x18, NULL);
		break;

	default:
		if (GOLFDOC()->m_gameMode & 0x48108)
			SendMsg(this, "Screen", 0x1cc, 0, 0, 0, 0);
		break;
	}
}

void CGolfRule::OnChatRecordInit(int id)
{
	m_pChatRecordBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pChatRecordBtn)
	{
		m_pChatRecordBtn->SetVisible(false);

		WPoint pos;
		pos.x = g_view->GetWidth() - 348.0f;
		pos.y = m_pChatRecordBtn->GetRect().y;
		m_pChatRecordBtn->MoveWindow(pos);
	}
}

void CGolfRule::OnChatRecordBtnUp()
{
	SendMsg(this, "Screen", 0x1cd, 0, 0, 0, 0);
}

void CGolfRule::OnOnelineViewInit(int id)
{
	m_pOnelineView = DYNAMIC_CAST(FrArea, (FrWnd*)id);
	m_pOnelineView->SetVisible(false);

	COneLineBoard::Instance()->SetRect(m_pOnelineView->GetRect());
	COneLineBoard::Instance()->LoadBg();
}

void CGolfRule::OnOnelineViewOwnerDraw(int id)
{
	COneLineBoard::Instance()->Display();
}

void CGolfRule::OnOnelineBtnInit(int id)
{
	m_pOnelineBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);

	WRect rect = m_pOnelineBtn->GetRect();
	rect.x = g_view->GetWidth() - 135.0f;
	m_pOnelineBtn->SetRect(rect);

	m_pOnelineBtn->EnableHover(true, 0.0f);

	m_pOnelineBtn->SetVisible(false);
	COneLineBoard::Instance()->ShowBg(false);
}

bool CGolfRule::OnOnelineReqChkOutDlgResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		WSendPacket packet((enumClientPacket)0x66);
		packet.EncodeStr(COneLineBoard::Instance()->GetMessageA());
		packet.Send((eSendTo)0);
	}

	return true;
}

bool CGolfRule::OnOnelineReqDlgResult(int result, FrForm* form)
{
	if (!CChatMsg::Instance()->IsOpened())
		g_pFresh->GetManager()->SetExclusiveKey(false);

	if (result == FrOK)
	{
		char* msg = (char*)((FrOnelineReqDlg*)form)->GetInputString();

		if (Doc()->m_chatManager.Filtering(msg))
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetMessage(
				"\xc0\xfb\xb4\xe7\xc7\xcf\xc1\xf6\xbe\xca\xc0\xba \xb4\xdc\xbe\xee\xb0\xa1 \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
			m_pOnelineReqDlg = NULL;
			return true;
		}

		if (msg[0] == 0)
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetMessage(
				"\xba\xb8\xb3\xbe \xb3\xbb\xbf\xeb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
			m_pOnelineReqDlg = NULL;
			return true;
		}

		COneLineBoard::Instance()->SetMessage(msg);

		WSendPacket packet((enumClientPacket)0x67);
		packet.Send((eSendTo)0);
	}

	m_pOnelineReqDlg = NULL;
	return true;
}

void CGolfRule::OnOnelineBtnUp()
{
	if (Doc()->IsControlServerService(0x20000))
	{
		this << MsgObject(this, 35,
			(int)"\xc0\xfc\xb1\xa4\xc6\xc7 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	m_pOnelineReqDlg = CreateForm<FrOnelineReqDlg>(g_pFresh->GetManager(), this,
		"oneline_req");
	if (m_pOnelineReqDlg)
	{
		m_pOnelineReqDlg->Open(
			(FRESH_PFN_RESULT)&CGolfRule::OnOnelineReqDlgResult, 0x11);

		if (!g_pFresh->GetManager()->IsKeyExclusive())
			g_pFresh->GetManager()->SetExclusiveKey(true);

		m_pOnelineReqDlg->SetIconRect(m_pOnelineBtn->GetRect());
	}
}

void CGolfRule::OnTopBarHoverOn(int id)
{
	m_hoverBtn = (int)DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_hoverBtn)
	{
		if (m_hoverBtn == (int)m_pMessengerBtn)
			CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_MSN);
		else if (m_hoverBtn == (int)m_pMyInfoBtn)
			CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_MYINFO);
		else if (m_hoverBtn == (int)m_pOptionBtn)
			CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_OPTION);
		else if (m_hoverBtn == (int)m_pOverBarSpecialCard)
			CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_SPECIAL_CARD);
	}
}

void CGolfRule::OnTopBarHoverOff(int id)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (!pButton || (int)pButton == m_hoverBtn)
	{
		m_hoverBtn = 0;
		CIconToolTip::GetInstance()->ClearToolTip();
	}
}

void CGolfRule::InitRadioPredictShotButton(FrButton** button, int id, float x)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)id);

	if (pButton)
	{
		pButton->MoveWindow(
			WPoint(g_view->GetWidth() - 180.0f, g_view->GetHeight() - x));
		pButton->SetPushDelay(0.1f);
	}

	*button = pButton;
}

void CGolfRule::SetRadioPredictShotPress(FrButton* button)
{
	if (m_pPredictNormalBtn)
		m_pPredictNormalBtn->SetStatus(FrButton::NORMAL);
	if (m_pPredictTomahawkBtn)
		m_pPredictTomahawkBtn->SetStatus(FrButton::NORMAL);
	if (m_pPredictCobraBtn)
		m_pPredictCobraBtn->SetStatus(FrButton::NORMAL);
	if (m_pPredictSpikeBtn)
		m_pPredictSpikeBtn->SetStatus(FrButton::NORMAL);

	if (button)
		button->SetStatus(FrButton::PRESSED);

	int type = 0;
	if (button == m_pPredictTomahawkBtn)
		type = 1;
	if (button == m_pPredictCobraBtn)
		type = 2;
	if (button == m_pPredictSpikeBtn)
		type = 3;
	SendMsg(this, "ShotPreview", -1, (int)"CHANGE_PREDICT_TYPE", type, 0, 0);
}

void CGolfRule::UpdateRadioPredictShotButtonStatus()
{
	bool bVisible = GolfClub().GetType() != 3;

	if (m_pPredictNormalBtn)
		m_pPredictNormalBtn->SetVisible(bVisible);
	if (m_pPredictTomahawkBtn)
		m_pPredictTomahawkBtn->SetVisible(bVisible);
	if (m_pPredictCobraBtn)
		m_pPredictCobraBtn->SetVisible(bVisible);
	if (m_pPredictSpikeBtn)
		m_pPredictSpikeBtn->SetVisible(bVisible);
}

void CGolfRule::OnRadioPredictNormalShotInit(int id)
{
	InitRadioPredictShotButton(&m_pPredictNormalBtn, id, 100.0f);
	if (m_pPredictNormalBtn)
		m_pPredictNormalBtn->SetStatus(FrButton::PRESSED);
}

void CGolfRule::OnRadioPredictTomahwakShotInit(int id)
{
	InitRadioPredictShotButton(&m_pPredictTomahawkBtn, id, 80.0f);
}

void CGolfRule::OnRadioPredictCobraShotInit(int id)
{
	InitRadioPredictShotButton(&m_pPredictCobraBtn, id, 60.0f);
}

void CGolfRule::OnRadioPredictSpikeShotInit(int id)
{
	InitRadioPredictShotButton(&m_pPredictSpikeBtn, id, 40.0f);
}

void CGolfRule::OnRadioPredictNormalShotDown()
{
	if (m_pPredictNormalBtn &&
		m_pPredictNormalBtn->GetStatus() != FrButton::PRESSED)
		SetRadioPredictShotPress(m_pPredictNormalBtn);
}

void CGolfRule::OnRadioPredictTomahwakShotDown()
{
	if (m_pPredictTomahawkBtn)
	{
		if (GolfClub().PowerShotType() != 0)
			SetRadioPredictShotPress(m_pPredictTomahawkBtn);
		else
			m_pPredictTomahawkBtn->SetStatus(FrButton::NORMAL);
	}
}

void CGolfRule::OnRadioPredictCobraShotDown()
{
	if (m_pPredictCobraBtn)
	{
		if (GolfClub().PowerShotType() != 0)
			SetRadioPredictShotPress(m_pPredictCobraBtn);
		else
			m_pPredictCobraBtn->SetStatus(FrButton::NORMAL);
	}
}

void CGolfRule::OnRadioPredictSpikeShotDown()
{
	if (m_pPredictSpikeBtn)
	{
		if (GolfClub().PowerShotType() != 0)
			SetRadioPredictShotPress(m_pPredictSpikeBtn);
		else
			m_pPredictSpikeBtn->SetStatus(FrButton::NORMAL);
	}
}

void CGolfRule::OnRadioPredictNormalShotDraw(int id)
{
	FrButton* pButton = m_pPredictNormalBtn;
	if (pButton)
	{
		pButton->MoveWindow(
			WPoint(g_view->GetWidth() - 180.0f, g_view->GetHeight() - 100.0f));

		WRect rect = pButton->GetRect();
		float x = rect.x + rect.w - 35.0f;
		if (pButton->GetStatus() == FrButton::PRESSED)
			m_pPredictUI->SetColor(0xffffffff);
		else
			m_pPredictUI->SetColor(0x80808080);

		m_pPredictUI->Render("Pangya", x, rect.y, 1.0f, 0);
	}
}

void CGolfRule::OnRadioPredictTomahwakShotDraw(int id)
{
	FrButton* pButton = m_pPredictTomahawkBtn;
	if (pButton)
	{
		if (GolfClub().PowerShotType() == 0)
			SetRadioPredictShotPress(m_pPredictNormalBtn);

		pButton->MoveWindow(
			WPoint(g_view->GetWidth() - 180.0f, g_view->GetHeight() - 80.0f));
		pButton->Enable(GolfClub().PowerShotType() != 0);

		WRect rect = pButton->GetRect();
		float x = rect.x + rect.w - 23.0f;
		if (pButton->GetStatus() == FrButton::PRESSED)
			m_pPredictUI->SetColor(0xffffffff);
		else
			m_pPredictUI->SetColor(0x80808080);

		m_pPredictUI->Render("Tomahawk", x, rect.y, 1.0f, 0);
	}
}

void CGolfRule::OnRadioPredictCobraShotDraw(int id)
{
	FrButton* pButton = m_pPredictCobraBtn;
	if (pButton)
	{
		if (GolfClub().PowerShotType() == 0)
			SetRadioPredictShotPress(m_pPredictNormalBtn);

		pButton->MoveWindow(
			WPoint(g_view->GetWidth() - 180.0f, g_view->GetHeight() - 60.0f));
		pButton->Enable(GolfClub().PowerShotType() != 0);

		WRect rect = pButton->GetRect();
		float x = rect.x + rect.w - 40.0f;
		if (pButton->GetStatus() == FrButton::PRESSED)
			m_pPredictUI->SetColor(0xffffffff);
		else
			m_pPredictUI->SetColor(0x80808080);

		m_pPredictUI->Render("Cobra", x, rect.y, 1.0f, 0);
	}
}

void CGolfRule::OnRadioPredictSpikeShotDraw(int id)
{
	FrButton* pButton = m_pPredictSpikeBtn;
	if (pButton)
	{
		if (GolfClub().PowerShotType() == 0)
			SetRadioPredictShotPress(m_pPredictNormalBtn);

		pButton->MoveWindow(
			WPoint(g_view->GetWidth() - 180.0f, g_view->GetHeight() - 40.0f));
		pButton->Enable(GolfClub().PowerShotType() != 0);

		WRect rect = pButton->GetRect();
		float x = rect.x + rect.w - 40.0f;
		if (pButton->GetStatus() == FrButton::PRESSED)
			m_pPredictUI->SetColor(0xffffffff);
		else
			m_pPredictUI->SetColor(0x80808080);

		m_pPredictUI->Render("Spike", x, rect.y, 1.0f, 0);
	}
}

CGolfRule::CGolfRule()
{
	m_dispPriority = DISP_PRIORITY_0;
	m_pChatView = NULL;
	m_pEmoticonBtn = NULL;
	m_pLanguage = NULL;
	m_pReportBtn = NULL;
	m_pChatTarget = NULL;
	m_pChatInput = NULL;
	m_pEmoticonDlg = NULL;
	m_pReportDlg = NULL;
	m_pMyInfoBtn = NULL;
	m_pOptionBtn = NULL;
	m_pCaddieBookBtn = NULL;
	m_pUnplayableBtn = NULL;
	m_pPauseBtn = NULL;
	m_pChatRecordBtn = NULL;
	m_pOnelineView = NULL;
	m_pOnelineBtn = NULL;
	m_pForgetDlg = NULL;
	m_bForgetDlg = false;
	m_forgetDlgTime = 0.0f;
	m_forgetItemSlot = 0x1800002d;
	m_pPlayerInfoDlg = NULL;
	m_pOnelineReqDlg = NULL;
	m_pApproachDlg = NULL;
	m_pApproachEndDlg = NULL;
	m_bClosedBonusBall = false;
	memset(m_closedBonusBall, 0, sizeof(m_closedBonusBall));

	m_pCurrentRule = NULL;

	if (IsLocalContent((localContentType_t)0x50))
	{
		m_command = m_spinCommand = 0;
		m_specialShot = 0;
	}

	if (IsLocalContent((localContentType_t)0x4f))
	{
		m_pReplayDlg = NULL;
		m_pRecordAviDlg = NULL;

		CReplayControlManager::Instance()->Init();
		m_recordPang = 0;
		m_recordBonusPang = 0;
	}

	Doc()->SetGoStopMode(false);

	m_pPredictUI = new CSimpleUI("[impact.png");
	m_pPredictUI->Add("Pangya", WRect(0.0f, 18.0f, 128.0f, 18.0f), false);
	m_pPredictUI->Add("Tomahawk", WRect(0.0f, 72.0f, 128.0f, 18.0f), false);
	m_pPredictUI->Add("Cobra", WRect(0.0f, 126.0f, 128.0f, 18.0f), false);
	m_pPredictUI->Add("Spike", WRect(0.0f, 144.0f, 128.0f, 18.0f), false);

	m_pPredictNormalBtn = NULL;
	m_pPredictCobraBtn = NULL;
	m_pPredictSpikeBtn = NULL;
	m_pPredictTomahawkBtn = NULL;
	m_topViewMode = 0;
}

CGolfRule::~CGolfRule()
{
	if (m_pPredictUI)
	{
		delete m_pPredictUI;
		m_pPredictUI = NULL;
	}

	if (m_pCurrentRule)
	{
		delete m_pCurrentRule;
		m_pCurrentRule = NULL;
	}
}
#pragma pack(push, 1)
struct sMapCheckInfo
{
	int Version;
	int CRC;
	unsigned char Par;
	float HoleStartPos[2];
	float HoleCupPos[2];
};
#pragma pack(pop)

void CGolfRule::SendHoleData()
{
	sMapCheckInfo info;
	float teeX = GOLFDOC()->GetHoleData().tee.x;
	float teeZ = GOLFDOC()->GetHoleData().tee.z;
	float pinX = GOLFDOC()->GetHoleData().pin.x;
	float pinZ = GOLFDOC()->GetHoleData().pin.z;
	unsigned char par = GOLFDOC()->GetHoleData().par;
	info.HoleStartPos[0] = teeX;
	info.HoleStartPos[1] = teeZ;
	info.HoleCupPos[0] = pinX;
	info.HoleCupPos[1] = pinZ;
	info.Par = par;
	info.CRC = GOLFDOC()->GetHoleData().checkSum;
	info.Version = 0;

	WSendPacket packet(0x1a);
	packet.Encode1(GOLFDOC()->m_currentHole);
	packet.EncodeBuffer(&info, sizeof(info));
	packet.Send((eSendTo)0);
}

void CGolfRule::OnPreLoadInit()
{
	CTaskMain::OnPreLoadInit();

	for (unsigned char i = 0; i < 12; i++)
	{
		Doc()->m_awardItem[i].uid = 0xffffffff;
		Doc()->m_awardItem[i].pang = 0;
	}

	Doc()->m_myGamePang = 0;

	m_shotData.pvsFrame = 0;

	GOLFDOC()->MakeHoleData();

	if (!WNetworkSystem::Instance()->IsConnected((WNetworkSystem::eNetUnit)0))
	{
		SetGameType(COption::Instance()->dGetGameType());

		GOLFDOC()->m_currentHole = COption::Instance()->dGetStartHole();
	}
	else
	{
		if (Doc()->m_startHole > 0)
			GOLFDOC()->m_currentHole = Doc()->m_startHole;
		else
			GOLFDOC()->m_currentHole = GetStartHole();
	}

	GOLFDOC()->m_tutorialMode = 15;

	g_view->SetFOV(55.0f * g_DEGTORAD);

	if (GetCurMap() == 13)
		g_view->SetClip(0.1f, 26214.4f, true);
	else
		g_view->SetClip(0.1f, 1920.0f, true);

	g_camera.Reset();
	GOLFDOC()->m_cameraData.mat.Reset();

	m_shotFlag = 0;
	m_bUnused3c9 = false;
	m_bModal = false;
	m_bNetMsgWait = false;

	m_bGaugePause = false;
	GOLFDOC()->m_bLockGauge = false;

	if (IsLocalContent((localContentType_t)0x58))
	{
		CTHunter::Instance()->SetTreasurePoint(0);
	}

	m_netMsgSend = NULL;
	m_netMsgRecv = NULL;
	m_netMsgSendTime = 0;
	m_netMsgRecvTime = 0;
	m_netMsgWaitLine = -1;
	m_bShowUI = true;
	m_bgm = 0;
	m_reservedSpecial = 0;
	m_observerTime = 0;
	m_shotSync = 0;
	m_debugCameraSpeed = -1.0f;
	m_debugCameraRadius = 100.0f;

	m_captureStep = 0;
	m_captureScale = 1;
	m_captureMode = 0;

	m_bAutoShot = false;
	m_bRemoteClick = false;
	m_nextPlayer = 0;
	m_pPauseDlg = NULL;
	m_pGoStopDlg = NULL;
	m_pCancelDropDlg = NULL;
	m_bShotBlock = false;
	m_shotBlockTime = 0.0f;

	m_bChatTyping = false;
	m_bDisconnect = false;

	CMouseCursor::Instance()->SetAutoHide(true);

	new CCameraWork;
	new CRival;
	new CRandomHouse;

	char path[512];
	strcpy(path, g_executeDirectory);
	sprintf(path, "%s/data/Sound/bgm", path);

	CheckUserBGM(path);

	Doc()->m_pangRate = 1.0f;
	if (!GalleryMode())
	{
		ITEM_LIST_IT it;
		it = std::find(Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(),
			0x1a000001);
		if (it == Doc()->m_myItemList.end())
		{
			it = std::find(Doc()->m_myItemList.begin(),
				Doc()->m_myItemList.end(), 0x1a000002);
		}

		if (it != Doc()->m_myItemList.end())
		{
			if (IsPcBang())
				Doc()->m_pangRate *= 2.5f;
			else
				Doc()->m_pangRate *= 2.0f;
		}
		else
		{
			if (IsPcBang())
				Doc()->m_pangRate *= 1.2f;
		}

		if (IsLocalContent((localContentType_t)0x51))
		{
			CCardManager::Instance()->SetPlayerIndex(0xff);
			Doc()->m_pangRate *=
				CCardManager::Instance()->GetCardPeriodPangRate() * 0.01f +
				1.0f;
		}

		it = std::find(Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(),
			0x1a000005);
		if (it != Doc()->m_myItemList.end())
			Doc()->m_pangRate *= 4.0f;

		it = std::find(Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(),
			0x1a000009);
		if (it != Doc()->m_myItemList.end())
			Doc()->m_pangRate *= 1.2f;

		it = std::find(Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(),
			0x1a00000c);
		if (it != Doc()->m_myItemList.end())
			Doc()->m_pangRate *= 1.2f;

		if (IsLocalContent((localContentType_t)3))
		{
			sMascotInfo* pMascotInfo = Doc()->GetMyEquippedMascotInfo();
			if (pMascotInfo)
			{
				IFF_STRUCT::sMascot* pMascot =
					ItemManager()->FindMascot(pMascotInfo->tid);
				if (pMascot)
				{
					if (pMascot->PangUp > 100)
						Doc()->m_pangRate *= (pMascot->PangUp * 0.01f);
				}
			}
		}

		if (IsLocalContent((localContentType_t)0x39))
		{
			Doc()->m_pangRate *=
				Doc()->GetMapEventPangRate(GetCurMap()) * 0.01f;
		}
	}

	Doc()->m_bFastForward = false;
	if (!GalleryMode())
	{
		ITEM_LIST_IT it;
		it = std::find(Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(),
			0x1a000011);
		if (it != Doc()->m_myItemList.end())
			Doc()->m_bFastForward = true;
	}

	for (unsigned char i = 0; i < 8; i++)
	{
		g_audio->LoadSfx(g_plSound[i], NULL, 0, NULL, NULL, 0.5f, 200.0f);
	}

	m_bSkinsNextHole = false;
	m_coinTime = 0.0f;
	m_coinCount = 0;
	m_coinNum = 0;

	for (unsigned char hole = 0; hole < Doc()->m_golfGame.holes; hole++)
	{
		unsigned long pang = GetHolePang(hole + 1);
		GOLFDOC()->GetHoleData(Doc()->m_holeOrder[hole]).basePang = pang;
		GOLFDOC()->GetHoleData(Doc()->m_holeOrder[hole]).pang = pang;
	}
}

void CGolfRule::OnLoad()
{
	CTaskMain::OnLoad();

	OnReload();

	CTaskMain::OnInit();

	g_pFresh->OpenLayout("MAIN", this, true);
}

void CGolfRule::OnInit()
{
	if (GetCurMapProp())
		GetPVS().SetSlopeFactor(GetCurMapProp()->Slope);

	CSceneManager::Instance()->UpdateWind();

	m_bTrajectoryFx = false;
	CalcUIOffset(false);

	SendMsg(this, "Score", 0x12f, 0, 0, 0, 0);

	const char* name = s_gatewayActor[1];
	m_pReplayDlg = NULL;
	m_pRecordAviDlg = NULL;
	m_bPlayerPreviewReady = false;
	m_bWaitForStart = false;

	IActor* pActor = GetActor(name);
	pActor << MsgObject(NULL, 0x26f, 12, 0, 0, 0, 0);
}

void CGolfRule::OnReload()
{
	SetCurrentGolfRule();

	if (!m_pCurrentRule)
	{
		ReturnLobby(false);
		return;
	}

	Doc()->ClearGameVars();
	GOLFDOC()->ClearShotResultMsg();

	if (Doc()->GetPlayingMode() == 1)
	{
		if (IsLocalContent((localContentType_t)0x4f))
			Doc()->m_shotDataList.clear();

		for (unsigned char i = 0; i < Doc()->m_golfGame.holes; i++)
		{
			GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).playerNum = 0;
			GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).result = 3;

			if (Doc()->m_roomInfo.realGameType == 14)
				SetCurMap(Doc()->m_pinIndex[i]);

			if (!GOLFDOC()->m_pPolySoup->LoadMapCheckData(
					Doc()->m_holeOrder[i]))
			{
				if (WNetworkSystem::Instance()->IsConnected(
						(WNetworkSystem::eNetUnit)0))
				{
					g_bQuit = true;
					return;
				}
			}

			GetCurMap();
		}
	}
	else if (IsLocalContent((localContentType_t)0x4f) &&
		Doc()->GetPlayingMode() == 2)
	{
		Doc()->m_golfGame.shotTimeLimit = 0;
		COption::Instance()->dSetNumPlayers(1);
		GOLFDOC()->MakePlayer(1);

		if (ShotDataLoad(Doc()->GetFileName()) == true)
		{
			if (Doc()->m_replayPlayType == 1)
			{
				PLAYER(0)->bonusPang = 0;
				PLAYER(0)->totalPang = 0;
			}
			else
			{
				Doc()->m_replayShotIndex = 0;
			}

			GOLFDOC()->m_pPolySoup->LoadMapCheckData(m_shotData.hole);
		}
	}

	SetPlayer();
	SetGrade();

	if ((Doc()->m_myInfo.info.dwIdentity & 0xe) == 0xe)
	{
		RecordPlayerList();

		for (RIVAL_LIST_IT it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
		{
			(*it).observerState = 0;
		}
	}

	ToggleGameUI();

	CRandomHouse::Instance()->SetRandomSeed(g_CurrentTime);
	CCameraWork::Instance()->Load();

	CChatMsg::Instance()->ClearHistory();
	CMouseCursor::Instance()->Reset(true);
	CMouseCursor::Instance()->SetMode(1);
	COneLineBoard::Instance()->Clear();
	CMessengerInfo::Instance()->SetGameFocus(true);
}

void CGolfRule::SetPlayer()
{
	if (HackingManager::Instance())
		HackingManager::Instance()->ClearCrypticList();

	if (m_pCurrentRule)
	{
		m_pCurrentRule->SetPlayer();
	}
}

void CGolfRule::SetGrade()
{
	if (m_pCurrentRule)
	{
		m_pCurrentRule->SetGrade();
	}
}

void CGolfRule::SetReady()
{
	if (Doc()->GetPlayingMode() == 2)
	{
		ChangeGameMode(0x10000);
		return;
	}

	if (m_pCurrentRule)
	{
		m_pCurrentRule->SetReady();
	}
}

void CGolfRule::RecordPlayerList()
{
	mkdir("history");
	mkdir("history/ongamenet");

	SYSTEMTIME st;
	GetLocalTime(&st);

	char buf[256] = { 0 };
	FILE* fp =
		fopen(MakeStr("history/ongamenet/player(%02d\xbd\xc3%02d\xba\xd0).txt",
				  st.wHour, st.wMinute),
			"wt");

	if (fp)
	{
		sprintf(buf, "\xbe\xc6\xc0\xcc\xb5\xf0 : \xb4\xeb\xc8\xad\xb8\xed\n");
		fwrite(buf, strlen(buf), 1, fp);

		sprintf(buf,
			"------------------------------------------------------------\n");
		fwrite(buf, strlen(buf), 1, fp);

		switch (Doc()->m_golfGame.gameType)
		{
		case GAME_TYPE_30S:
		case GAME_TYPE_30S_TEAM:
		case GAME_TYPE_GUILD_MATCH:
		case GAME_TYPE_APPROACH:
		case GAME_TYPE_NEW_APPROACH:
			for (RIVAL_LIST_IT it = Doc()->m_rivalList.begin();
				it != Doc()->m_rivalList.end(); ++it)
			{
				sprintf(buf, "%s\n", (*it).nickname);
				fwrite(buf, strlen(buf), 1, fp);
			}
			break;

		default:
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
			{
				sprintf(buf, "%s\n", Doc()->m_userInfo[i].info.sNick);
				fwrite(buf, strlen(buf), 1, fp);
			}
			break;
		}

		fclose(fp);
	}
}

void CGolfRule::SetVisibleUpperBtns(bool bVisible)
{
	if (m_pChatView)
		m_pChatView->SetVisible(bVisible);

	if (m_pOnelineBtn)
		m_pOnelineBtn->SetVisible(bVisible);

	if (m_pMyInfoBtn)
		m_pMyInfoBtn->SetVisible(bVisible);

	if (m_pOptionBtn)
		m_pOptionBtn->SetVisible(bVisible);

	if (m_pMessengerBtn)
		m_pMessengerBtn->SetVisible(bVisible);
}

void CGolfRule::AddPlayer(unsigned char index, const char* name,
	unsigned long uid, unsigned long oid, unsigned long caddie)
{
	if (m_pCurrentRule)
	{
		if (WNetworkSystem::Instance()->IsConnected(
				(WNetworkSystem::eNetUnit)0))
		{
			m_pCurrentRule->AddPlayer(index, name, uid, oid, caddie);
		}
		else
		{
			m_pCurrentRule->AddPlayerOffline(index, name, oid, caddie);
		}
	}

	if (OnlinePlay() && HackingManager::Instance())
	{
		GolfBall().AddCrypticValue();
		GolfClub().AddCrypticValue();

		for (unsigned char i = 0; i < 18; i++)
		{
			HackingManager::Instance()->AddCrypticByteValue(
				&PLAYER(index)->stroke[i]);
			HackingManager::Instance()->AddCrypticByteValue(
				&PLAYER(index)->putt[i]);
			HackingManager::Instance()->AddCrypticIntValue(
				&PLAYER(index)->pang[i]);
		}

		HackingManager::Instance()->AddCrypticIntValue(
			&PLAYER(index)->totalPang);
		HackingManager::Instance()->AddCrypticIntValue(
			&PLAYER(index)->bonusPang);
		HackingManager::Instance()->AddCrypticFloatValue(&PLAYER(index)->gauge);
		HackingManager::Instance()->AddCrypticIntValue(&PLAYER(index)->power);
		HackingManager::Instance()->AddCrypticIntValue(&PLAYER(index)->control);
		HackingManager::Instance()->AddCrypticIntValue(
			&PLAYER(index)->accuracy);
		HackingManager::Instance()->AddCrypticIntValue(&PLAYER(index)->spin);
		HackingManager::Instance()->AddCrypticIntValue(&PLAYER(index)->curve);
	}
}

void CGolfRule::OnDestroy()
{
	CTaskMain::OnDestroy();

	if (CCameraWork::IsInstantiated())
		delete CCameraWork::Instance();
	if (CRival::IsInstantiated())
		delete CRival::Instance();
	if (CRandomHouse::IsInstantiated())
		delete CRandomHouse::Instance();
	if (CCapturedBg::IsInstantiated())
		delete CCapturedBg::Instance();

	while (!m_backUpList.empty())
		m_backUpList.pop();
}

void CGolfRule::OnDisplay()
{
	if ((GOLFDOC()->m_gameMode & 0x20) && CCapturedBg::Instance())
		CCapturedBg::Instance()->Render();

	if (!(GOLFDOC()->m_gameMode & 0xffdddddf) && !m_captureMode)
		return;

	int index;
	float height = g_camera.pivot.y -
		GetPVS().GetGroundPoint(g_camera.pivot, true, &index, false).y;

	if (index == -1)
		height = g_camera.pivot.y - GetPVS().GetLowest();

	float nearClip;
	if (GOLFDOC()->m_gameMode == 0x80 ||
		g_view->InFrustum(WSphere(GolfBall().m_pos, GolfBall().GetRadius())))
	{
		float dist = (g_camera.pivot - GolfBall().m_pos).Magnitude();
		nearClip = height > dist ? dist : height;
	}
	else
	{
		nearClip = height;
	}

	Waabb& aabb = GOLFDOC()->m_pPolySoup->FindModel(NULL)->aabb;

	if (GOLFDOC()->m_gameMode & 0x200)
	{
		g_view->SetClip(3.2f, 3840.0f, true);
	}
	else
	{
		float farClip;
		if (GetCurMap() == 13)
			farClip = 8192.0f;
		else
			farClip = 600.0f;

		float size = aabb.max.x - aabb.min.x;

		size = size < aabb.max.y - aabb.min.y ? aabb.max.y - aabb.min.y : size;
		size = size < aabb.max.z - aabb.min.z ? aabb.max.z - aabb.min.z : size;

		size += 160.0f;
		farClip = farClip * 3.2f < size ? size : farClip * 3.2f;

		float clip =
			nearClip < 0.1f ? 0.1f : (nearClip > 1.0f ? 1.0f : nearClip);

		g_view->SetClip(clip, farClip, true);
	}

	g_view->UpdateCamera();
}

void CGolfRule::OnProcess(float delta)
{
	UpdateRadioPredictShotButtonStatus();

	if (g_input->GetDown("FUNC_8", true))
	{
		mkdir("report");
		std::list<sShotData> backup(Doc()->m_shotDataList);
		int playType = Doc()->m_replayPlayType;
		Doc()->m_replayPlayType = 2;

		SYSTEMTIME st;
		GetLocalTime(&st);

		const char* filename =
			MakeStr("report/pangya_report_repaly.%04d%02d%02d-%02d%02d%02d.sav",
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		ShotDataSave(filename, false);
		Doc()->m_shotDataList = backup;
		Doc()->m_replayPlayType = playType;
	}

	if (IsLocalContent((localContentType_t)0x4f))
	{
		if (Doc()->GetPlayingMode() == 2)
		{
			ReplayDlgRun();

			g_input->BlockKey("\xc1\xc2");
			g_input->BlockKey("\xbf\xec");
			g_input->BlockKey("\xbb\xf3");
			g_input->BlockKey("\xc7\xcf");

			g_input->BlockKey("W");
			g_input->BlockKey("A");
			g_input->BlockKey("S");
			g_input->BlockKey("D");

			g_input->BlockKey("ENTER");
		}
		else
		{
			if (m_pReplayStatus)
				m_pReplayStatus->SetVisible(false);
		}
	}

	if (m_pOnelineBtn)
		m_pOnelineBtn->SetAlpha(COneLineBoard::Instance()->GetFadeFactor());

	if (!(GOLFDOC()->m_gameMode & 0x30) &&
		CCapturedBg::Instance()->IsCaptured())
	{
		CCapturedBg::Instance()->Reset();
	}

	if (m_bShotBlock)
	{
		m_shotBlockTime += delta;
		if (m_shotBlockTime > 5.0f)
			m_bShotBlock = false;
	}

	ProcessObserver();

	if (GalleryMode() &&
		(GOLFDOC()->m_shotPhase == 1 || GOLFDOC()->m_shotPhase == 2))
		DoPowerGauge(delta);

	if (GOLFDOC()->m_gameMode != 0x200 && g_input->GetDown("FUNC_12", true))
	{
		if (m_pKbdManualDlg)
		{
			m_pKbdManualDlg->Close(true);
		}
		else
		{
			m_pKbdManualDlg = CreateForm<FrKbdManualDlg>(g_pFresh->GetManager(),
				this, "kbd_manual", NULL);
			if (m_pKbdManualDlg)
			{
				m_pKbdManualDlg->Open(
					(FRESH_PFN_RESULT)&CGolfRule::OnKbdManualDlgResult, 1);
				m_pKbdManualDlg->EnableDrag(false);
			}
		}
	}

	if (!CQuitWindow::Instance()->IsActive())
	{
		if (((g_input->Get("LCONTROL", true) ||
				 g_input->Get("RCONTROL", true)) &&
				g_input->GetDown("HIDE", true)) ||
			(!m_bShowUI &&
				(g_input->GetDown("ENTER", true) ||
					g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
						true))))
		{
			m_bShowUI = !m_bShowUI;
			ToggleGameUI();
		}
	}
	else
	{
		if (!m_bShowUI)
		{
			m_bShowUI = true;
			ToggleGameUI();
		}
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("EMOTICON", false))
	{
		if (!RANKINGDLG() && !CMessengerInfo::Instance()->IsVisible())
			OnEmoticonBtnUp();
	}

	if (m_pLanguage)
	{
		static bool s_bAlphaNumeric = true;

		if (IsLocalCountry(1) && g_ime->IsAlphaNumericMode() != s_bAlphaNumeric)
		{
			s_bAlphaNumeric = g_ime->IsAlphaNumericMode();

			if (s_bAlphaNumeric)
				m_pLanguage->SetBgImg("chat_eng");
			else
				m_pLanguage->SetBgImg("chat_kor");
		}
	}

	if (m_pChatInput && m_pChatTarget)
	{
		if (!CChatMsg::Instance()->IsClosed())
		{
			if (g_input->GetDown("PAGE_UP", true))
			{
				if (m_pChatInput->HasKeyFocus() &&
					Doc()->m_chatHistory.size() > 0)
				{
					STRING_LIST_IT it = std::find(Doc()->m_chatHistory.begin(),
						Doc()->m_chatHistory.end(),
						m_pChatInput->GetLine(1, false));
					if (it != Doc()->m_chatHistory.end())
					{
						if (it != Doc()->m_chatHistory.begin())
						{
							--it;
							CChatMsg::Instance()->SetChatText((*it).c_str(),
								false);
						}
					}
					else
					{
						CChatMsg::Instance()->SetChatText(
							Doc()->m_chatHistory.back().c_str(), false);
					}
				}
				else if (m_pChatTarget->HasKeyFocus() &&
					Doc()->m_reservedChatList.size() > 0)
				{
					CChatMsg::Instance()->SetChatText(
						Doc()->GetPrevChatTarget(
							m_pChatTarget->GetLine(1, false)),
						false);
				}
			}
			else if (g_input->GetDown("PAGE_DOWN", true))
			{
				if (m_pChatInput->HasKeyFocus() &&
					Doc()->m_chatHistory.size() > 0)
				{
					STRING_LIST_IT it = std::find(Doc()->m_chatHistory.begin(),
						Doc()->m_chatHistory.end(),
						m_pChatInput->GetLine(1, false));
					if (it != Doc()->m_chatHistory.end())
					{
						it++;
						if (it != Doc()->m_chatHistory.end())
							CChatMsg::Instance()->SetChatText((*it).c_str(),
								false);
						else
							CChatMsg::Instance()->SetChatText("", false);
					}
				}
				else if (m_pChatTarget->HasKeyFocus() &&
					Doc()->m_reservedChatList.size() > 0)
				{
					CChatMsg::Instance()->SetChatText(
						Doc()->GetNextChatTarget(
							m_pChatTarget->GetLine(1, false)),
						false);
				}
			}
		}

		if (m_pChatInput->IsVisible() && g_input->GetDown("ENTER", true) &&
			!CMessengerInfo::Instance()->IsVisible())
		{
			m_pChatInput->SetKeyFocus(true);
		}

		if (OnlinePlay())
		{
			const char* text = m_pChatInput->GetLine(1, false);
			if (text && strlen(text) > 0)
			{
				if (!m_bChatTyping)
				{
					m_bChatTyping = true;
					CHeadIcon::Instance()->SendIcon(1);
				}

				if (strlen(m_pChatTarget->GetLine(1, false)) == 0 ||
					!strcmp(m_pChatTarget->GetLine(1, false),
						"\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4"))
					m_pChatInput->SetFontColor(0xffffffff);
				else if (strlen(m_pChatTarget->GetLine(1, false)) == 0 ||
					!strcmp(m_pChatTarget->GetLine(1, false),
						"\xb1\xe6\xb5\xe5\xbf\xa1\xb0\xd4"))
					m_pChatInput->SetFontColor(0xff5bffb0);
				else
					m_pChatInput->SetFontColor(0xffcf67ff);
			}
			else
			{
				if (m_bChatTyping == true)
				{
					m_bChatTyping = false;
					CHeadIcon::Instance()->SendIcon(0xffff);
				}
			}
		}
	}

	if (m_bNetMsgWait)
	{
		m_netMsgWaitTime -= delta;
		if (m_netMsgWaitTime < 0.0f)
		{
			unsigned long elapsed = GetTickCount() - m_netMsgSendTime;
			unsigned long limit =
				(m_netMsgSend && !strcmpi(m_netMsgSend, "DoScoreBoard"))
				? 40000
				: 20000;

			if (elapsed > limit)
			{
				CProjectG::Instance()->ReportError(1, "%s_%d_%d_%d_%d(%d_%d)",
					m_netMsgSend ? m_netMsgSend : "UnknownLock",
					g_iSendPacketGR, g_iRecvPacketGR, g_iSendFailType,
					WSAGetLastError(), Doc()->m_roomInfo.roomGuid,
					MyGuid(false));

				g_iSendPacketGR = 0;
				g_iRecvPacketGR = 0;
				g_iRecvDisconCode = 0;

				SendReportError(1);
				SetNetMsgWait(false, __LINE__, "", 0.0f);

				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", NULL);
				pForm->SetMessage(
					"\xbc\xad\xb9\xf6\xbf\xcd\xc0\xc7 \xbf\xac\xb0\xe1\xc0\xcc \xb2\xf7\xb0\xe5\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open(
					(FRESH_PFN_RESULT)&CGolfRule::ReturnToLobbyDlgResult,
					0x203);
			}
		}
	}

	if (m_bDisconnect)
	{
		m_bDisconnect = false;
		SetNetMsgWait(false, __LINE__, "", 0.0f);

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xc6\xc4\xc0\xcf\xc0\xcc \xbc\xd5\xbb\xf3\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
			false);
		pForm->Open((FRESH_PFN_RESULT)&CGolfRule::ReturnToLobbyDlgResult, 3);
	}

	if (GOLFDOC()->m_gameMode != 0x20 && !m_captureMode)
		GetPVS().PreProcess(delta, false);

	if (GOLFDOC()->m_gameMode & 0x48308)
	{
		if (!m_bShotBlock)
			GolfClub().Process(delta);
	}

	if (GetGameType() == GAME_TYPE_NEW_APPROACH)
	{
		bool bMode = false;
		bool bStarted = false;
		bool bTimeOver = false;

		if (!(GOLFDOC()->m_gameMode & 0x800cc0))
			bMode = true;

		if (Doc()->m_approachStartTime > 0)
			bStarted = true;

		unsigned long limit = GetGameTimeLimit() + Doc()->m_approachStartTime;
		if (g_CurrentTime > limit)
			bTimeOver = true;

		if (bMode && bStarted && bTimeOver)
		{
			GolfBall().m_playFrame = 0;

			SendShotResult();

			if (CCapturedBg::Instance()->IsCaptured())
			{
				CCapturedBg::Instance()->Reset();
			}

			SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Player", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Mascot", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Bubble", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1a4, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1a5, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1a8, 0, 0, 0, 0);

			ChangeGameMode(0x800);
		}
	}

	if ((GOLFDOC()->m_gameMode & 8) && GolfClub().PowerShotType() == 0 &&
		(g_input->Get("LALT", true) || g_input->Get("RALT", true)))
		return;

	g_input->GetDown("SKIP", true);

	if (g_input->GetDown("FASTFORWARD", true) && m_bCanSkip &&
		(GOLFDOC()->m_gameMode & 0xc0))
	{
		if (OnlinePlay() && IsMyTurn(false))
		{
			if (IsPcBang() || Doc()->m_bFastForward)
			{
				m_bCanSkip = false;
				float speed = 3.0f;
				WSendPacket packet((enumClientPacket)0x65);
				packet.EncodeBuffer(&speed, 4);
				packet.Send((eSendTo)0);
			}
		}
		else
		{
			m_bCanSkip = false;
		}
	}

	if (Doc()->GetPlayingMode() == 1)
	{
		if (OnlinePlay() || Doc()->m_gameMode == 5)
		{
			if (!WisEqual(CProjectG::Instance()->GetGameSpeed(), 1.0f,
					g_EPSILON) &&
				(GOLFDOC()->m_gameMode & 0xfffff73f))
				CProjectG::Instance()->SetGameSpeed(1.0f);
		}
	}

	if (IsLocalContent((localContentType_t)0x4f))
	{
		if (Doc()->m_replayPlayType == 4 && g_input->GetDown("REPLAY", true) &&
			IsMyTurn(false) && GOLFDOC()->m_tutorialMode == 15 &&
			Doc()->m_gameMode == 0)
		{
			if (GetCurMap() != 7)
			{
				Doc()->m_replayPlayType = 2;
				ReplayTapeCheckAndUse();
			}
		}
	}

	if (IsLocalContent((localContentType_t)0x4f) &&
		Doc()->GetPlayingMode() == 2)
	{
		if (Doc()->GetReplayState() == 7 && m_pReplayDlg)
			ReplayBoxClose();
	}

	m_processTime += delta;
	if (m_pGoStopDlg)
	{
		char buf[50];
		m_goStopTime += delta;
		sprintf(buf,
			"%d\xc3\xca \xc8\xc4\xbf\xa1 \xc0\xda\xb5\xbf\xc0\xb8\xb7\xce \xb0\xd4\xc0\xd3\xbf\xa1\xbc\xad \xb3\xaa\xb0\xa9\xb4\xcf\xb4\xd9. %d\xc3\xca",
			10.0f, (int)m_goStopTime);
		m_pGoStopDlg->SetDesc(buf);
	}

	switch (GOLFDOC()->m_gameMode)
	{
	case 0x2:
		DoPlayerPreview(delta);
		break;
	case 0x4:
		DoCloseupCaddie(delta);
		break;
	case 0x8:
		DoStand(delta);
		break;
	case 0x10:
		DoGetReadyToCaptureBG(delta);
		break;
	case 0x20:
		DoPowerGauge(delta);
		break;
	case 0x100:
		DoNavigateCamera(delta);
		break;
	case 0x200:
		DoTopViewCamera(delta);
		break;
	case 0x400:
		DoDebugCamera(delta);
		break;
	case 0x800:
		DoToDefaultCamera(delta);
		break;
	case 0x40:
		DoDrive(delta);
		break;
	case 0x80:
		DoFlyBall(delta);
		break;
	case 0x1000:
		DoHoleInPose(delta);
		break;
	case 0x2000:
		DoScoreBoard(delta);
		break;
	case 0x4000:
		DoTimeOut(delta);
		break;
	case 0x8000:
		DoFixedCamera(delta);
		break;
	case 0x20000:
		DoGameOver(delta);
		break;
	case 0x1:
		DoWaitForStart(delta);
		break;
	case 0x40000:
		DoSpotCamera(delta);
		break;
	case 0x80000:
		DoDespairPose(delta);
		break;
	case 0x200000:
		DoCoursePreview(delta);
		break;
	case 0x800000:
		DoApproachNextHole(delta);
		break;
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("CANCEL", false))
	{
		if (GOLFDOC()->IsShowChat())
		{
			CChatMsg::Instance()->AddChatMsg(
				"\xb0\xd4\xc0\xd3\xb3\xbb \xb4\xeb\xc8\xad\xc3\xa2\xc0\xbb \xbc\xfb\xb1\xe9\xb4\xcf\xb4\xd9.",
				0xffff7878, true, false);
			GOLFDOC()->ShowChat(false);
		}
		else
		{
			CChatMsg::Instance()->AddChatMsg(
				"\xb0\xd4\xc0\xd3\xb3\xbb \xb4\xeb\xc8\xad\xc3\xa2\xc0\xbb \xc8\xb0\xbc\xba\xc8\xad\xc7\xd5\xb4\xcf\xb4\xd9.",
				0xffff7878, true, false);
			GOLFDOC()->ShowChat(true);
		}
	}

	if (m_coinTime != 0.0f)
	{
		m_coinTime -= delta;

		if (m_coinTime < 0.0f)
		{
			m_coinTime = 0.0f;
			m_coinCount--;

			if (m_coinCount > 0)
				m_coinTime = 0.1f;

			SendMsg(this, "Effect", 0x17d, m_coinNum, 0, 0, 0);
		}
	}

	CMouseCursor::Instance()->SetCursor(0);
}

extern Fresh* g_pFresh;

void CGolfRule::DoFixedCamera(float delta)
{
	if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true))
	{
		CMouseCursor::Instance()->Store();
		ChangeGameMode(8);
	}
	else
	{
		if (TopViewClick())
			ChangeGameMode(0x200);
	}
}

void CGolfRule::DoSpotCamera(float delta)
{
	if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true))
	{
		CMouseCursor::Instance()->Store();
		ChangeGameMode(8);
	}
	else
	{
		if (TopViewClick())
			ChangeGameMode(0x200);
	}
}

bool CGolfRule::CheckDropOut()
{
	if (m_pCurrentRule)
	{
		int num = m_pCurrentRule->GetNumTimeout();
		if (num >= 3)
		{
			if (!OnlinePlay())
				SendMsg(this, "GolfRule", 0x82,
					PLAYER(GOLFDOC()->m_currentPlayer)->oid, 0, 0, 0);
			return true;
		}
	}
	return false;
}

bool CGolfRule::CheckGiveUp()
{
	if (m_pCurrentRule)
		return m_pCurrentRule->CheckGiveUp();
	return false;
}

void CGolfRule::DoDespairPose(float delta)
{
	if (m_processTime < m_timer)
	{
		CCameraWork::Instance()->Process(delta, 0.0f);
		return;
	}

	if (OnlinePlay() && !m_bSendFinishShot)
	{
		m_bSendFinishShot = true;

		if (!IsMassGame())
			SetNetMsgWait(true, 3290, "DoDespairPose", 20.0f);

		if (Doc()->GetGoStopMode() == true &&
			GOLFDOC()->GetHoleData().playerNum <= 2)
		{
			Doc()->SetGoStopMode(false);
		}
		else
		{
			WSendPacket packet((enumClientPacket)0x1c);
			EncodePacket((enumClientPacket)0x1c, packet);
			packet.Send(TO_GAME);
		}

		CheckNetMsgWait_Send("DoDespairPose");
	}

	if (m_bModal || m_bNetMsgWait)
		return;

	if (PLAYER(GOLFDOC()->m_currentPlayer)->oid == MyGuid(true))
	{
		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		{
			if (GOLFDOC()->m_tutorialMode == 8)
			{
				IActor* pActor = GetActor("Tutorial");
				if (!pActor)
					return;

				pActor << MsgObject(NULL, 0x154, 0, 0, 0, 0, 0);
				return;
			}
			else if (GOLFDOC()->m_tutorialMode == 7)
			{
				ReturnLobby(true);
				return;
			}
			else if (GOLFDOC()->m_tutorialMode == 15 && Doc()->m_gameMode == 0)
			{
				if (PLAYER(GOLFDOC()->m_currentPlayer)->state == 3)
					return;

				if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
				{
					if (PLAYER(PLAYER(GOLFDOC()->m_currentPlayer)->teammate)
									->timeout +
								PLAYER(GOLFDOC()->m_currentPlayer)->timeout >=
							3 ||
						PLAYER(PLAYER(GOLFDOC()->m_currentPlayer)->teammate)
									->giveUp +
								PLAYER(GOLFDOC()->m_currentPlayer)->giveUp >=
							Max(3, (GetHoles() + 1) / 2))
						return;
				}
				else
				{
					if (PLAYER(GOLFDOC()->m_currentPlayer)->timeout >= 3 ||
						PLAYER(GOLFDOC()->m_currentPlayer)->giveUp >=
							Max(3, (GetHoles() + 1) / 2))
						return;
				}
			}
		}
		else
		{
			ChangeGameMode(0x2000);
			return;
		}
	}

	if (CanMoveToNextHole())
	{
		if (GOLFDOC()->m_tutorialMode == 8)
		{
			IActor* pActor = GetActor("Tutorial");
			if (!pActor)
				return;

			pActor << MsgObject(NULL, 0x14b, 0, 0, 0, 0, 0);
			return;
		}
		else if (GOLFDOC()->m_tutorialMode == 7)
		{
			ReturnLobby(false);
			return;
		}

		if (!OnlinePlay() || IsMassGame())
			ChangeGameMode(0x2000);
	}
	else
	{
		SendMsg(this, "Score", 0x12f, 0, 0, 0, 0);
		if (g_pFresh)
			g_pFresh->GetManager()->CloseForm(false);
		ChangeGameMode(0x800);
	}
}

void CGolfRule::DoTimeOut(float delta)
{
	if (m_processTime > 3.0f)
	{
		if (OnlinePlay() && !m_bSendFinishShot)
		{
			m_bSendFinishShot = true;

			if (!IsMassGame())
				SetNetMsgWait(true, 3448, "DoTimeOut", 20.0f);

			WSendPacket packet((enumClientPacket)0x1c);
			EncodePacket((enumClientPacket)0x1c, packet);
			packet.Send(TO_GAME);

			CheckNetMsgWait_Send("DoTimeOut");
		}

		if (m_bModal || m_bNetMsgWait || m_pGoStopDlg)
			return;

		m_bSendFinishShot = false;

		if (m_nextPlayer == GOLFDOC()->m_currentPlayer)
		{
			PLAYER(GOLFDOC()->m_currentPlayer)
				->stroke[GOLFDOC()->m_currentHole - 1] += 1;
			if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
				TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
					->stroke[GOLFDOC()->m_currentHole - 1] += 1;

			SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Clock", 0x1e9, 0, 0, 0, 0);

			SendMsg(this, "Player", 0x10f, 0, 0, 0, 0);
			SendMsg(this, "Caddie", 0x2716, (int)"\xb1\xe2\xba\xbb", 0, 0, 0);
			SendMsg(this, "Caddie", 0x1db, 0, 0, 0, 0);
			ChangeGameMode(8);
		}
		else
		{
			GOLFDOC()->m_currentPlayer = m_nextPlayer;
			PLAYER(GOLFDOC()->m_currentPlayer)
				->stroke[GOLFDOC()->m_currentHole - 1] += 1;
			if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
				TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
					->stroke[GOLFDOC()->m_currentHole - 1] += 1;

			SetEnvironmentVariableA();
			SetTurnChangeEvent();
			ReadyForNextTurn();
		}
	}
}

void CGolfRule::DoGameOver(float delta)
{
	if (m_processTime < m_timer)
	{
		CCameraWork::Instance()->Process(delta, 0.0f);
		return;
	}

	if (m_processTime < 10.0f)
		return;

	if (Doc()->m_golfGame.gameType <= GAME_TYPE_TEAM)
	{
		if (OnlinePlay())
		{
			if (!CTHunter::Instance()->IsReturnLobby())
				return;
		}
	}

	if (m_bModal || m_bNetMsgWait)
		return;

	if (OnlinePlay())
	{
		SetNetMsgWait(true, 3539, "DoGameOver", 20.0f);

		this << MsgObject(NULL, 0x8f, 1, 0, 0, 0, 0);

		CheckNetMsgWait_Send("DoGameOver");
		return;
	}

	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		return;

	switch (Doc()->m_gameMode)
	{
	case 1:
		SetGameOver(true);
		CProjectG::Instance()->FromGolfToLobbySettings();
		CTaskManager::Instance()->ChangeTask("CFamilyTask", "", false);
		PostMsg(this, "Family", 0, (int)"FAMILY", 0, 0, 0);
		break;

	case 2:
	{
		m_bModal = true;
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
			"notify_yesno", NULL);
		pForm->Open((FRESH_PFN_RESULT)&CGolfRule::ContinueDlgResult, 0);
		pForm->SetMessage(
			"\xbf\xac\xbd\xc0\xc0\xbb \xb0\xe8\xbc\xd3 \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		pForm->EnableDrag(false);
		break;
	}

	case 5:
		SetGameOver(true);
		CProjectG::Instance()->FromGolfToLobbySettings();
		CTaskManager::Instance()->ChangeTask("CGhostTask", "",
			CTaskManager::Instance()->GetPreservedTask() != NULL);
		CTaskManager::Instance()->PostMsg(NULL, "Ghost", 0, (int)"GHOST", 0, 0,
			0);
		break;
	}
}

bool CGolfRule::HaveTeeShots()
{
	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		if (!TEAM(1 - m_turn)->stroke[GOLFDOC()->m_currentHole - 1])
			return true;
		return false;
	}

	for (unsigned char i = 0; i < GetPlayerNum(); i++)
	{
		if (PLAYER(i)->state == 0 &&
			PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1] == (unsigned char)0)
			return true;
	}

	return false;
}

void CGolfRule::RecordPlayerData(unsigned char player)
{
	if (PLAYER(player)->state == 3)
		return;

	PLAYER(player)->item = 0;

	if (PLAYER(player)->holeNum == GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
	{
		if (!GOLFDOC()->m_bNetGameEnd)
		{
			PLAYER(player)->totalStroke +=
				PLAYER(player)->stroke[GOLFDOC()->m_currentHole - 1];
			if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
				PLAYER(player)->score = TEAM(PLAYER(player)->team)->score;
			else if (Doc()->m_golfGame.gameType != GAME_TYPE_MATCH &&
				Doc()->m_golfGame.gameType != GAME_TYPE_SKINS)
				PLAYER(player)->score +=
					(unsigned char)PLAYER(player)
						->stroke[GOLFDOC()->m_currentHole - 1] -
					GOLFDOC()->GetHoleData().par;
		}
		PLAYER(player)->holeNum++;
	}

	PLAYER(player)->state = 1;
}

void CGolfRule::RecordTeamData(unsigned char team)
{
	TEAM(team)->totalStroke += TEAM(team)->stroke[GOLFDOC()->m_currentHole - 1];
	TEAM(team)->state = 1;
}

void CGolfRule::ResetRank()
{
	unsigned char i, j;

	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_SKINS:
		for (i = 0; i < GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 3)
				continue;

			PLAYER(i)->rank = 1;
			PLAYER(i)->order = 1;

			for (j = 0; j < GetPlayerNum(); j++)
			{
				if (i == j || PLAYER(j)->state == 3)
					continue;

				if ((int)PLAYER(j)->skinsSumPang > (int)PLAYER(i)->skinsSumPang)
				{
					PLAYER(i)->rank++;
					PLAYER(i)->order++;
				}
				else if ((int)PLAYER(j)->skinsSumPang ==
						(int)PLAYER(i)->skinsSumPang &&
					PLAYER(i)->index > PLAYER(j)->index)
				{
					PLAYER(i)->order++;
				}
			}
		}
		break;

	case GAME_TYPE_GUILD_MATCH:
		if (GOLFDOC()->m_currentHole >= 1 && GOLFDOC()->m_currentHole <= 18)
		{
			sRivalData* pRival0 = GUILDMATCHUP(0);
			sRivalData* pRival1 = GUILDMATCHUP(1);

			for (int i = 0; i < GOLFDOC()->m_currentHole; i++)
			{
				if (pRival0->holeStroke[i] == 0 || pRival1->holeStroke[i] == 0)
					break;

				if (pRival0->holeStroke[i] < pRival1->holeStroke[i])
				{
					GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).result = 0;
				}
				else if (pRival0->holeStroke[i] > pRival1->holeStroke[i])
				{
					GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).result = 1;
				}
				else
				{
					GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).result = 2;
				}
			}
		}
		break;

	case GAME_TYPE_MATCH:
		if (GOLFDOC()->m_currentHole >= 1 && GOLFDOC()->m_currentHole <= 18)
		{
			if (PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] <
				PLAYER(1)->stroke[GOLFDOC()->m_currentHole - 1])
			{
				GOLFDOC()->GetHoleData().result = 0;

				if (!GOLFDOC()->m_bNetGameEnd)
					PLAYER(0)->score++;
			}
			else if (PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] >
				PLAYER(1)->stroke[GOLFDOC()->m_currentHole - 1])
			{
				GOLFDOC()->GetHoleData().result = 1;

				if (!GOLFDOC()->m_bNetGameEnd)
					PLAYER(1)->score++;
			}
			else
				GOLFDOC()->GetHoleData().result = 2;

			for (i = 0; i < GetPlayerNum(); i++)
			{
				if (PLAYER(i)->state == 3)
					continue;

				PLAYER(i)->order = 1;
				PLAYER(i)->rank = 1;

				for (j = 0; j < GetPlayerNum(); j++)
				{
					if (i == j || PLAYER(j)->state == 3)
						continue;

					if (PLAYER(i)->score < PLAYER(j)->score ||
						(PLAYER(i)->score == PLAYER(j)->score &&
							PLAYER(i)->totalPang < PLAYER(j)->totalPang))
					{
						PLAYER(i)->order++;
						PLAYER(i)->rank++;
					}

					if (PLAYER(i)->score == PLAYER(j)->score &&
						PLAYER(i)->totalPang == PLAYER(j)->totalPang &&
						PLAYER(i)->index > PLAYER(j)->index)
						PLAYER(i)->order++;
				}
			}
		}
		break;

	case GAME_TYPE_STROKE:
		for (i = 0; i < GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 3)
				continue;

			PLAYER(i)->order = 1;
			PLAYER(i)->rank = 1;

			for (j = 0; j < GetPlayerNum(); j++)
			{
				if (i == j || PLAYER(j)->state == 3)
					continue;

				if (PLAYER(i)->score > PLAYER(j)->score ||
					(PLAYER(i)->score == PLAYER(j)->score &&
						PLAYER(i)->totalPang < PLAYER(j)->totalPang))
				{
					PLAYER(i)->order++;
					PLAYER(i)->rank++;
				}

				if (PLAYER(i)->score == PLAYER(j)->score &&
					PLAYER(i)->totalPang == PLAYER(j)->totalPang &&
					PLAYER(i)->index > PLAYER(j)->index)
					PLAYER(i)->order++;
			}
		}
		break;

	case GAME_TYPE_TEAM:
		if (GOLFDOC()->m_currentHole >= 1 && GOLFDOC()->m_currentHole <= 18)
		{
			if (TEAM(0)->state == 3 || TEAM(1)->state == 3)
			{
				unsigned char winner = TEAM(0)->state == 3;

				GOLFDOC()->GetHoleData().result = winner;

				if (!GOLFDOC()->m_bNetGameEnd)
					PLAYER(winner)->score++;
			}
			else
			{
				if (TEAM(0)->stroke[GOLFDOC()->m_currentHole - 1] <
					TEAM(1)->stroke[GOLFDOC()->m_currentHole - 1])
				{
					GOLFDOC()->GetHoleData().result = 0;

					if (!GOLFDOC()->m_bNetGameEnd)
						TEAM(0)->score++;
				}
				else if (TEAM(0)->stroke[GOLFDOC()->m_currentHole - 1] >
					TEAM(1)->stroke[GOLFDOC()->m_currentHole - 1])
				{
					GOLFDOC()->GetHoleData().result = 1;

					if (!GOLFDOC()->m_bNetGameEnd)
						TEAM(1)->score++;
				}
				else
					GOLFDOC()->GetHoleData().result = 2;
			}
		}
		break;
	}

	SendMsg(this, "Score", 0x131, 0, 0, 0, 0);

	if (OnlinePlay())
		return;

	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_STROKE:
		for (i = 0; i < GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 3)
			{
				GOLFDOC()->GetHoleData().rank[i] = GetPlayerNum();
				continue;
			}

			GOLFDOC()->GetHoleData().rank[i] = 1;

			for (j = 0; j < GetPlayerNum(); j++)
			{
				if (i == j || PLAYER(j)->state == 3)
					continue;

				if (PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1] >
						PLAYER(j)->stroke[GOLFDOC()->m_currentHole - 1] ||
					(PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1] ==
							PLAYER(j)->stroke[GOLFDOC()->m_currentHole - 1] &&
						PLAYER(i)->teeOrder > PLAYER(j)->teeOrder))
					GOLFDOC()->GetHoleData().rank[i]++;
			}
		}

		for (i = 0; i < GetPlayerNum(); i++)
			PLAYER(i)->teeOrder = GOLFDOC()->GetHoleData().rank[i] - 1;
		break;

	case GAME_TYPE_TEAM:
		if (TEAM(m_turn)->stroke[GOLFDOC()->m_currentHole - 1] >
			TEAM(1 - m_turn)->stroke[GOLFDOC()->m_currentHole - 1])
			m_turn = 1 - m_turn;

		for (unsigned char t = 0; t < 2; t++)
		{
			TEAM(t)->teeShooter = 1 - TEAM(t)->teeShooter;
			TEAM(t)->turn = TEAM(t)->teeShooter;
		}
		break;
	}
}

bool CGolfRule::CanMoveToNextHole()
{
	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_STROKE:
		for (int i = 0; i < GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 0 ||
				(PLAYER(i)->state == 2 &&
					!PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1]))
				return false;
		}

		ResetRank();
		return true;

	case GAME_TYPE_MATCH:
	case GAME_TYPE_SKINS:
		if (PLAYER(0)->state == 1 && PLAYER(1)->state == 1)
			return true;

		if (PLAYER(0)->state == 1)
		{
			if (PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] <=
				PLAYER(1)->stroke[GOLFDOC()->m_currentHole - 1])
				return true;
		}
		else if (PLAYER(1)->state == 1)
		{
			if (PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] >=
				PLAYER(1)->stroke[GOLFDOC()->m_currentHole - 1])
				return true;
		}
		return false;

	case GAME_TYPE_TEAM:
	{
		bool bEnd[2];
		bEnd[0] = TEAM(0)->state == 1;
		bEnd[1] = TEAM(1)->state == 1;

		if (bEnd[0] && bEnd[1])
			return true;

		unsigned char stroke0 = TEAM(0)->stroke[GOLFDOC()->m_currentHole - 1];
		unsigned char stroke1 = TEAM(1)->stroke[GOLFDOC()->m_currentHole - 1];

		if (bEnd[0])
		{
			if (stroke0 <= stroke1)
				return true;
		}
		else if (bEnd[1])
		{
			if (stroke0 >= stroke1)
				return true;
		}
		break;
	}

	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		return PLAYER(GOLFDOC()->m_currentPlayer)->state == 1;
	}

	return false;
}

void CGolfRule::RecordGiveUp(unsigned char player)
{
	if (player >= GetPlayerNum())
		return;

	unsigned char maxStroke = GOLFDOC()->GetHoleData().par + 4;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
		TEAM(PLAYER(player)->team)->stroke[GOLFDOC()->m_currentHole - 1] =
			maxStroke + 1;
	else
		PLAYER(player)->stroke[GOLFDOC()->m_currentHole - 1] = maxStroke + 1;

	PLAYER(player)->giveUp++;
}

void CGolfRule::RecordScoreByPlayer(unsigned char player)
{
	if (player >= GetPlayerNum())
		return;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
		RecordTeamData(PLAYER(player)->team);

	RecordPlayerData(player);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM &&
		PLAYER(player)->teammate != 0xff)
		RecordPlayerData(PLAYER(player)->teammate);

	SendHoleStat();
}

void CGolfRule::RecordScoreByTeam(unsigned char team)
{
	if (Doc()->m_golfGame.gameType != GAME_TYPE_TEAM || team >= 2)
		return;

	for (int i = 0; i < 4; i++)
	{
		if (PLAYER(i)->team == team)
		{
			RecordScoreByPlayer(i);
			break;
		}
	}
}

void CGolfRule::ResetData(const WVector& pos)
{
	unsigned char playerNum = 0;

	for (unsigned char i = 0; i < GetPlayerNum(); i++)
	{
		if (PLAYER(i)->state == 3)
			continue;

		PLAYER(i)->event.Reset();
		PLAYER(i)->flag = 0;
		PLAYER(i)->startPos = pos;
		PLAYER(i)->pos = pos;
		PLAYER(i)->timeout = 0;
		PLAYER(i)->holeInPose = 0;
		PLAYER(i)->holeInCollision = 0;
		PLAYER(i)->bunker =
			(Doc()->GetPlayingMode() == 2) ? m_shotData.bunker : 0;

		if (GOLFDOC()->m_skinsHolePang == 0)
			PLAYER(i)->skinsHolePang = 0;

		playerNum++;
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		for (unsigned char t = 0; t < 2; t++)
		{
			TEAM(t)->pos = pos;
			TEAM(t)->holeInPose = 0;
			TEAM(t)->holeInCollision = 0;
			TEAM(t)->bunker = 0;
		}
	}

	GOLFDOC()->GetHoleData().playerNum = playerNum;

	m_bShotResult = false;
	m_bSendFinishShot = false;
	m_bBallStopped = false;
	m_bTurnSetup = false;
	m_bCanSkip = false;

	while (!m_backUpList.empty())
		m_backUpList.pop();

	SendMsg(this, "Ball", 0x1eb, 0, 0, 0, 0);
}

unsigned long CGolfRule::GetHolePang(unsigned char hole)
{
	float rate;
	unsigned long type = Doc()->m_curChannel.Type;

	if (type & 0x800)
		rate = 1.0f;
	else if (type & 0x1000)
		rate = 2.5f;
	else if (type & 0x2000)
		rate = 5.0f;
	else
		rate = 1.0f;

	int step = (hole - 1) / 3;

	float pang = rate * (step * 0.5f + 1.0f) * 20.0f;
	if (hole == GetHoles())
		pang *= 2;

	return (unsigned long)pang;
}

void CGolfRule::DoScoreBoard(float delta)
{
	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
	{
		bool bClosed = false;
		SendMsg(this, "Score", 0x138, (int)&bClosed, 0, 0, 0);

		if (!bClosed)
			return;
		break;
	}

	default:
		if (m_processTime < 3.0f)
			return;
		break;
	}

	if (OnlinePlay())
	{
		if (IsGameOver() || GOLFDOC()->m_bNetGameEnd)
		{
			if (IsMassGame())
			{
				if (!GOLFDOC()->GetStatUpdated())
					UpdateShotResultStat();

				ReturnLobby(false);
			}
			else
			{
				ChangeGameMode(0x20000);
			}
			return;
		}

		if (m_bModal || m_bNetMsgWait)
			return;

		GolfBall().SetHoleIn(0);

		if (GetHoleIndex(GOLFDOC()->m_currentHole) == GetHoles())
		{
			GOLFDOC()->m_currentHole = 1;
			GOLFDOC()->m_bNoStat = true;
		}
		else
		{
			if (Doc()->m_roomInfo.realGameType == 14)
				SetCurMap(
					Doc()->m_pinIndex[GetHoleIndex(GOLFDOC()->m_currentHole)]);

			GOLFDOC()->m_currentHole =
				Doc()->m_holeOrder[GetHoleIndex(GOLFDOC()->m_currentHole)];
		}

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(GOLFDOC()->m_currentHole);

		SetNetMsgWait(true, 4217, "DoScoreBoard", 40.0f);

		SendHoleData();
		CheckNetMsgWait_Send("DoScoreBoard");

		SendMsg(this, "GolfBg", 0x140, 0, 0, 0, 0);
		CheckNetMsgWait_Send("DoScoreBoard");

		if (Doc()->m_golfGame.gameType == GAME_TYPE_STROKE ||
			Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
			Doc()->m_golfGame.gameType == GAME_TYPE_30S ||
			Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM ||
			Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
			SendMsg(this, "Score", 0x12f, 2, 0, 0, 0);
	}
	else
	{
		if (IsGameOver())
		{
			ChangeGameMode(0x20000);
			return;
		}

		if (GetHoleIndex(GOLFDOC()->m_currentHole) == GetHoles())
		{
			GOLFDOC()->m_currentHole = 1;
			GOLFDOC()->m_bNoStat = true;
			GOLFDOC()->m_pPolySoup->LoadMapCheckData(GOLFDOC()->m_currentHole);
		}
		else
		{
			GOLFDOC()->m_currentHole =
				Doc()->m_holeOrder[GetHoleIndex(GOLFDOC()->m_currentHole)];
		}

		SendMsg(this, "GolfBg", 0x140, 0, 0, 0, 0);

		if (Doc()->m_golfGame.gameType == GAME_TYPE_STROKE ||
			Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
			Doc()->m_golfGame.gameType == GAME_TYPE_30S ||
			Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM ||
			Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
			SendMsg(this, "Score", 0x12f, 2, 0, 0, 0);
	}
}

void CGolfRule::DoHoleInPose(float delta)
{
	if (m_processTime < m_timer)
	{
		CCameraWork::Instance()->Process(delta, 0.0f);
		return;
	}

	if (OnlinePlay() && !m_bSendFinishShot && Doc()->GetPlayingMode() == 1)
	{
		m_bSendFinishShot = true;

		if (!IsMassGame())
			SetNetMsgWait(true, 4292, "DoHoleInPose", 20.0f);

		WSendPacket packet((enumClientPacket)0x1c);
		EncodePacket((enumClientPacket)0x1c, packet);
		packet.Send(TO_GAME);

		CheckNetMsgWait_Send("DoHoleInPose");
	}

	if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->GetPlayingMode() == 2)
	{
		m_bModal = false;
		m_bNetMsgWait = false;

		if (Doc()->GetReplayState() == 7)
		{
			Doc()->SetReplayState(9);
			g_input->SetFakeInput("FUNC_11", false);
		}
	}

	if (m_bModal || m_bNetMsgWait)
		return;

	if (GOLFDOC()->m_tutorialMode < 15)
	{
		SendMsg(this, "GolfRule", 0x88, 1, 0, 0, 0);

		IActor* pActor = GetActor("Tutorial");
		if (!pActor)
			return;

		if (GOLFDOC()->m_tutorialMode == 8)
		{
			if (PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] <
				(unsigned char)5)
				pActor << MsgObject(NULL, 0x149, 0, 0, 0, 0, 0);
			else
				pActor << MsgObject(NULL, 0x154, 0, 0, 0, 0, 0);
		}
		else if (GOLFDOC()->m_tutorialMode == 12)
		{
			pActor << MsgObject(NULL, 0x154, 0, 0, 0, 0, 0);
		}
		else
		{
			pActor << MsgObject(NULL, 0x148, 0, 0, 0, 0, 0);
		}
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		if (m_bSkinsNextHole)
			ChangeGameMode(0x2000);
	}
	else if (CanMoveToNextHole())
	{
		if (!OnlinePlay() || IsMassGame())
			ChangeGameMode(0x2000);
	}
	else
	{
		if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->GetPlayingMode() == 2)
			return;

		SendMsg(this, "Score", 0x12f, 0, 0, 0, 0);
		if (g_pFresh)
			g_pFresh->GetManager()->CloseForm(false);
		ChangeGameMode(0x800);
	}
}

void CGolfRule::DoWaitForStart(float delta)
{
	FlyBy(delta);

	m_bWaitForStart = true;

	if (GOLFDOC()->m_currentHole != Doc()->m_holeOrder[0])
		ChangeGameMode(2);

	if (Doc()->m_slotList.size() == 1)
		ReturnLobby(false);
}

void CGolfRule::DoPlayerPreview(float delta)
{
	CCameraWork::Instance()->Process(delta, 0.0f);

	if (CCameraWork::Instance()->Finished())
	{
		if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->GetPlayingMode() == 2)
		{
			ChangeGameMode(8);
			if (!GOLFDOC()->IsTeeShotReady())
				GOLFDOC()->SetTeeShotReady(true);
			return;
		}

		if (OnlinePlay())
		{
			if (!GOLFDOC()->IsTeeShotReady())
			{
				GOLFDOC()->SetTeeShotReady(true);

				unsigned char gameType = Doc()->m_golfGame.gameType;
				if (gameType == GAME_TYPE_NEW_APPROACH || !IsMassGame())
					SetNetMsgWait(true, 4450, "DoPlayerPreview", 20.0f);

				WSendPacket packet((enumClientPacket)0x34);
				packet.Send(TO_GAME);

				CheckNetMsgWait_Send("DoPlayerPreview");
			}
		}

		if ((Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH ||
				!IsMassGame()) &&
			(m_bModal || m_bNetMsgWait || m_pGoStopDlg))
			return;

		if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
			Doc()->m_approachStartTime = g_CurrentTime;

		SetTurnChangeEvent();
		ReadyForNextTurn();

		if (GOLFDOC()->m_currentHole == Doc()->m_holeOrder[GetHoles() - 1])
			SendMsg(this, "Score", 0x135, 0, 0, 0, 0);

		CVoiceItem::Instance()->PlaySoundA(
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid,
			CVoiceItem::WAVESTATE_20, -1);
	}
}

void CGolfRule::DoCloseupCaddie(float delta)
{
	if (m_processTime < 3.0f)
	{
		CCameraWork::Instance()->Process(delta, 0.0f);
		return;
	}

	CMouseCursor::Instance()->Reset(true);
	SendMsg(this, "Clock", 0x1e9, 0, 0, 0, 0);
	g_audio->PlaySfx("play_my_turn");
	ChangeGameMode(8);
}

void CGolfRule::ReadyForNextTurn()
{
	CMouseCursor::Instance()->Reset(true);

	SendMsg(this, "Clock", 0x1e9, 0, 0, 0, 0);

	ChangeTurn();

	GOLFDOC()->MakeDefaultCamera(true);
	WPuppet* pBall = CSceneManager::Instance()->GetPuppet("\xb0\xf8");

	if (pBall && pBall->GetRootBone())
		pBall->SetAlpha(0xff);
	g_audio->StopSfx("shot_dist_count");
	g_audio->PlaySfx("play_my_turn");
	CCameraWork::Instance()->StopBallSound();
	ChangeGameMode(8);
}

void CGolfRule::DoDrive(float delta)
{
	unsigned char powerShot = GolfClub().PowerShotType();

	if (CCameraWork::Instance()->GetCameraType() >= 200 ||
		(powerShot == 2 && m_bDrivePose) || (powerShot == 3 && m_bDrivePose))
		CCameraWork::Instance()->Process(delta, 0.0f);

	if (m_bModal || m_bNetMsgWait)
		return;

	if (!m_bDrivePose)
	{
		if (!(m_shotFlag & 2))
		{
			if (!g_input->Get("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true))
				CCameraWork::Instance()->Unlock();
		}

		if (m_processTime > 1.0f)
		{
			m_bDrivePose = true;
			SendMsg(this, "Player", 0x10e, 0, 0, 0, 0);

			if ((powerShot == 2 || powerShot == 3) && GolfClub().GetType() == 0)
			{
				WVector pos;
				AfxGetTask()->GetActor("Player")
					<< MsgObject(NULL, 0x127, (int)&pos, 0, 0, 0, 0);

				char* name = MakeStr("\xbf\xec\xb5\xe5\xc6\xc4\xbf\xf6\xbc\xa6"
									 "2_%02d",
					rand() % 4 + 1);
				m_timer = CCameraWork::Instance()->LoadCamera(
					CCameraWork::CAMMODE_DRIVE, name, pos);

				m_bClubCameraEnd = false;
			}
		}
	}
	else
	{
		if (m_processTime > 11.0f)
			ChangeGameMode(0x80);
	}
}

static const int s_clearBonus[20] = { 20, 50, 55, 80, 65, 24, 50, 70, 40, 55,
	40, 20, 0, 80, 20, 20, 40, 40, 0, 40 };

void CGolfRule::HandleMsg(const MsgObject& msg)
{
	CGolfBall& ball = GolfBall();

	switch (msg.message)
	{
	case 0x24a:
	{
		FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			(const char*)msg.param1);
		if (pWnd == NULL)
			return;

		FrWnd** ppWnd = (FrWnd**)msg.param2;
		*ppWnd = pWnd;
	}
	break;

	case 0x21e:
		if (!IsLocalContent(S4_TREASURE_HUNTER))
			return;

		m_processTime = 0;
		break;

	case 0x29:
	{
		SetReady();

		if (!IsLocalContent(S3_BONGDARISHOP))
			return;
		if (!m_bClosedBonusBall)
			return;

		m_bClosedBonusBall = false;

		FrClosedBonusBallResultDlg* pDlg =
			CreateForm<FrClosedBonusBallResultDlg>(g_pFresh->GetManager(), this,
				"closed_bonusball");
		if (pDlg == NULL)
			return;

		pDlg->Open(NULL, 3);
		pDlg->SetClosedBonusBall(m_closedBonusBall);
		pDlg->SetFixed(true);
	}
	break;

	case 0x9d:
		m_debugCameraRadius = (float)msg.param1;
		break;

	case 0x9b:
		m_debugCameraSpeed = (float)msg.param1;
		break;

	case 0x9a:
		m_bSpecialPuttPose = true;
		break;

	case 0x271a:
		OnPreLoadInit();
		break;

	case 0x99:
		m_timer = 0;

		GOLFDOC()->SetTeeShotReady(false);
		GolfBall().Reset(
			GetTeeShotPos() + WVector(0.0f, GolfBall().GetRadius(), 0.0f));
		GolfBall().m_meshIndex = GetPVS().GetMeshIndex(GolfBall().m_pos, false);
		ResetData(GolfBall().m_pos);
		break;

	case 0x98:
	{
		if (m_pChatView == NULL)
			return;

		int lines = COption::Instance()->gIsChatLineEntended() ? 10 : 5;

		WPoint pos;
		pos.x = m_pChatView->GetRect().x;
		pos.y =
			COption::Instance()->gGetBar_Y() - 100.0f - (lines * 19.0f + 8.0f);
		m_pChatView->MoveWindow(pos);

		m_pChatView->GetScrollBar()->Resize(lines, 1, false);

		if (COption::Instance()->gIsChatLineEntended())
		{
			for (int i = 0; i < 5; i++)
			{
				const char* text = m_pChatView->GetLine(i + 1, false);
				if (text)
					m_pChatView->SetLine(i + 6, text, 0, false, 0);

				m_pChatView->SetLine(i + 1, "", 0, false, 0);
			}
		}
		else
		{
			for (int i = 0; i < 5; i++)
			{
				const char* text = m_pChatView->GetLine(i + 6, false);
				if (text)
					m_pChatView->SetLine(i + 1, text, 0, false, 0);

				m_pChatView->SetLine(i + 6, "", 0, false, 0);
			}
		}
	}
	break;

	case 0x69:
		if (GOLFDOC()->m_gameMode == 0x1000)
		{
			int diff;
			if (Doc()->m_golfGame.gameType != 1)
				diff = (unsigned char)PLAYER(GOLFDOC()->m_currentPlayer)
						   ->stroke[GOLFDOC()->m_currentHole - 1] -
					GOLFDOC()->GetHoleData().par;
			else
				diff = (unsigned char)TEAM(
						   PLAYER(GOLFDOC()->m_currentPlayer)->team)
						   ->stroke[GOLFDOC()->m_currentHole - 1] -
					GOLFDOC()->GetHoleData().par;
			if (diff <= 0)
				SetConstantBGM("bgm_under_par.mp3", true, true);
			else
				SetConstantBGM("bgm_over_par.mp3", true, true);
		}
		else if (GOLFDOC()->m_gameMode == 0x2000)
		{
			SetConstantBGM("bgm_scoreboard.mp3", false, true);
		}
		else if (GOLFDOC()->m_gameMode == 0x20000)
		{
			SetConstantBGM("bgm_scoreboard.mp3", true, true);
		}
		else
		{
			PlayBGM();
		}
		break;

	case 0x271b:
	{
		WPoint pos;

		if (m_pChatView)
		{
			pos.x = m_pChatView->GetRect().x;
			pos.y = COption::Instance()->gGetBar_Y() - 100.0f -
				((COption::Instance()->gIsChatLineEntended() ? 10 : 5) * 19.0f +
					8.0f);
			m_pChatView->MoveWindow(pos);
		}

		if (m_pChatTarget)
		{
			pos.x = m_pChatTarget->GetRect().x;
			pos.y = COption::Instance()->gGetBar_Y() + 14.0f;
			m_pChatTarget->MoveWindow(pos);
		}

		if (m_pChatInput)
		{
			pos.x = m_pChatInput->GetRect().x;
			pos.y = COption::Instance()->gGetBar_Y() + 14.0f;
			m_pChatInput->MoveWindow(pos);
		}

		if (m_pEmoticonBtn)
		{
			pos.x = m_pEmoticonBtn->GetRect().x;
			pos.y = COption::Instance()->gGetBar_Y() + 16.0f;
			m_pEmoticonBtn->MoveWindow(pos);
		}

		if (m_pLanguage)
		{
			pos.x = m_pLanguage->GetRect().x;
			pos.y = COption::Instance()->gGetBar_Y() + 16.0f;
			m_pLanguage->MoveWindow(pos);
		}

		if (m_pReportBtn)
		{
			pos.x = m_pReportBtn->GetRect().x;
			pos.y = COption::Instance()->gGetBar_Y() + 16.0f;
			m_pReportBtn->MoveWindow(pos);
		}

		if (m_pOverBarSpecialCard)
		{
			pos.x = g_view->GetWidth() - 106.0f;
			pos.y = m_pOverBarSpecialCard->GetRect().y;
			m_pOverBarSpecialCard->MoveWindow(pos);
		}

		if (m_pMyInfoBtn)
		{
			pos.x = g_view->GetWidth() - 54.0f;
			pos.y = m_pMyInfoBtn->GetRect().y;
			m_pMyInfoBtn->MoveWindow(pos);
		}

		if (m_pMessengerBtn)
		{
			pos.x = g_view->GetWidth() - 81.0f;
			pos.y = m_pMessengerBtn->GetRect().y;
			m_pMessengerBtn->MoveWindow(pos);
			CMessengerInfo::Instance()->m_alarmRect =
				m_pMessengerBtn->GetRect();
		}

		if (m_pMessengerBubbleEdit)
		{
			pos.x = g_view->GetWidth() - 167.0f;
			pos.y = m_pMessengerBubbleEdit->GetRect().y;
			m_pMessengerBubbleEdit->MoveWindow(pos);
		}

		if (m_pMessengerAlarmBtn)
		{
			pos.x = g_view->GetWidth() - 110.0f;
			pos.y = m_pMessengerAlarmBtn->GetRect().y;
			m_pMessengerAlarmBtn->MoveWindow(pos);
		}

		if (m_pMessengerGiftAlarmBtn)
		{
			pos.x = g_view->GetWidth() - 75.0f;
			pos.y = m_pMessengerGiftAlarmBtn->GetRect().y;
			m_pMessengerGiftAlarmBtn->MoveWindow(pos);
		}

		if (m_pOptionBtn)
		{
			pos.x = g_view->GetWidth() - 29.0f;
			pos.y = m_pMyInfoBtn->GetRect().y;
			m_pOptionBtn->MoveWindow(pos);
		}

		if (m_pCaddieBookBtn)
		{
			pos.x = g_view->GetWidth() - 423.0f;
			pos.y = m_pCaddieBookBtn->GetRect().y;
			m_pCaddieBookBtn->MoveWindow(pos);
		}

		if (m_pUnplayableBtn)
		{
			pos.x = g_view->GetWidth() - 398.0f;
			pos.y = m_pUnplayableBtn->GetRect().y;
			m_pUnplayableBtn->MoveWindow(pos);
		}

		if (m_pPauseBtn)
		{
			pos.x = g_view->GetWidth() - 373.0f;
			pos.y = m_pPauseBtn->GetRect().y;
			m_pPauseBtn->MoveWindow(pos);
		}

		if (m_pChatRecordBtn)
		{
			pos.x = g_view->GetWidth() - 348.0f;
			pos.y = m_pChatRecordBtn->GetRect().y;
			m_pChatRecordBtn->MoveWindow(pos);
		}

		ResetUiPos_Board();
	}
	break;

	case 0x8b:
		if (GOLFDOC()->GetBar().grade > 2)
		{
			*(float*)msg.param1 = GOLFDOC()->GetBar().Persent();
		}
		else
		{
			*(float*)msg.param1 = 0.01f;
		}
		break;

	case 0x8c:
		m_captureScale = msg.param1;
		break;

	case 0x8a:
		*(WVector2D*)msg.param1 = WVector2D(GOLFDOC()->GetBar().gaugeStep,
			COption::Instance()->gGetBar_Y());
		break;

	case 0xa2:
		if (m_bGaugePause == true)
		{
			m_bGaugePause = false;
			m_gaugeTime = m_gaugePauseTime;
		}
		else
		{
			m_bGaugePause = true;
		}
		break;

	case 0x89:
	{
		PLAYER(0)->state = 0;

		WPolySoup::sPoint* pPoint =
			GOLFDOC()->m_pPolySoup->FindPoint((const char*)msg.param1);
		GolfBall().m_pos =
			GetPVS().GetGroundPoint(pPoint->pos, false, NULL, false) +
			WVector(0.0f, GolfBall().GetRadius(), 0.0f);
		GolfBall().m_meshIndex = GetPVS().GetMeshIndex(GolfBall().m_pos, false);

		GolfBall().m_groundType =
			GetPVS().GetGroundType(GolfBall().m_pos, true, NULL);

		PLAYER(GOLFDOC()->m_currentPlayer)->pos = GolfBall().m_pos;
		if (msg.param2)
		{
			m_target =
				GOLFDOC()->m_pPolySoup->FindPoint((const char*)msg.param2)->pos;
		}
		else
		{
			m_target = GOLFDOC()->m_pAutoTarget->GetPoint(NULL);
		}

		ChangeTurn();

		SendMsg(this, "Screen", 0x1a3, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 0x18c, 0, 0, 0, 0);
		CFx::Instance()->ClearAll(true);

		if (GOLFDOC()->m_tutorialMode < 0xf && GOLFDOC()->m_tutorialMode != 0xb)
		{
			Wind().SetIntensity(0);
			Wind().SetDirection(0);
		}

		GOLFDOC()->m_pPolySoup->UpdateBgFxWind();

		SendMsg(this, "GolfRule", 0x88, 0, 0, 0, 0);

		SetEnvironmentVariableA();
		GOLFDOC()->MakeDefaultCamera(true);

		ChangeGameMode(8);
	}
	break;

	case 0x206:
		if (!IsLocalContent(S3_TUTORIAL_RENEWAL))
			return;

		GolfClub().SetClub((unsigned char)msg.param1);
		break;

	case 0x20d:
		if (!IsLocalContent(S3_TUTORIAL_RENEWAL))
			return;

		ToggleCommandBtns(true);
		break;

	case 0x88:
		m_bModal = msg.param1 != 0;
		break;

	case 0x7a:
	{
		if ((Doc()->m_myInfo.info.dwIdentity & 0xe) == 0xe)
		{
			CSceneManager::Instance()->SetVisible(
				MakeStr("PLAYER_%d", GOLFDOC()->m_currentPlayer), true);
			CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", true);
			CSceneManager::Instance()->SetVisible("\xc4\xb3\xb5\xf0", true);
			CShadowManager::Instance()->Enable(
				COption::Instance()->vIsShadowEnabled());

			SendMsg(this, "Score", 0x13b, 0, 0, 0, 0);
		}

		if (GalleryMode())
			SendMsg(this, "ItemWindow", 0x195, 0, 0, 0, 0);

		GOLFDOC()->m_shotPhase = 3;

		g_audio->StopSfx("\xc5\xb8\xc0\xd3");

		CMouseCursor::Instance()->SetCamMode(false);

		SendMsg(this, "Camera", 0x19b, 0, 0, 0, 0);
		SendMsg(this, "Letter", 0x18c, 0, 0, 0, 0);

		if (Doc()->m_golfGame.gameType != 3)
			SendMsg(this, "Screen", 0x1a7, 0, 0, 0, 0);
		GOLFDOC()->GetUIAlpha() = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();

		sShotProtocol& shot = GOLFDOC()->m_shotInfo;

		ball.m_initCurve = shot.curve;
		ball.m_initSpin = shot.spin;
		ball.m_initSpecial = shot.speical;
		GOLFDOC()->GetBar().power = shot.maxClick;
		GOLFDOC()->GetBar().impact = shot.minClick;
		GOLFDOC()->GetBar().gaugeStep = shot.minClick;
		GOLFDOC()->GetBar().gauge = shot.minClick;
		GOLFDOC()->GetBar().grade = shot.accuracy;
		GOLFDOC()->GetBar().bFullPower =
			WisEqual(GOLFDOC()->GetBar().power, s_barEnd, 1e-5f) != 0;
		GOLFDOC()->GetBar().center = shot.center;
		GOLFDOC()->m_shotTime = shot.shotTime;
		if (GetShotTimeLimit())
		{
			if (GOLFDOC()->m_shotTime < GetShotTimeLimit())
				GOLFDOC()->m_remainTime =
					GetShotTimeLimit() - GOLFDOC()->m_shotTime;
			else
				GOLFDOC()->m_remainTime = 0;
		}

		GOLFDOC()->m_cameraData.angle = shot.angle;
		GetPVS().SetFrame(shot.frame);
		GolfClub().SetClub(shot.clubIndex);
		GolfClub().SetRandomPower(shot.randomPower);

		m_shotSync = shot.randomAngle;

		if (Doc()->m_golfGame.gameType != 3)
		{
			SendMsg(this, "Spin", 0x165, 0, (int)GolfBall().m_initCurve,
				(int)GolfBall().m_initSpin, 0);
		}

		SetAngle(GOLFDOC()->m_cameraData.angle);
		SendMsg(this, "Player", 0x2718, (int)&GOLFDOC()->m_cameraData.angle, 0,
			0, 0);

		if (GOLFDOC()->m_bNoStat)
		{
			SendMsg(this, "Screen", 0x1c9, 0, 0, 0, 0);
		}

		m_target = ball.m_pos + RotMat(GOLFDOC()->m_cameraData.angle, 1).za;
		GOLFDOC()->MakeDefaultCamera(true);
		ChangeGameMode(0x40);
	}
	break;

	case 0x7b:
		if (GOLFDOC()->m_gameMode == 0x200000)
			return;

		switch (GOLFDOC()->m_gaugeInfo.phase)
		{
		case 1:
		{
			g_audio->StopSfx("\xc5\xb8\xc0\xd3");

			GOLFDOC()->m_shotPhase = 1;

			unsigned long mode = GOLFDOC()->m_gameMode;
			ChangeGameMode(0x20);
			GOLFDOC()->m_gameMode = mode;
		}
		break;

		case 2:
			if (GOLFDOC()->m_shotPhase == 1)
			{
				m_bRemoteClick = true;
				GOLFDOC()->GetBar().gauge = GOLFDOC()->m_gaugeInfo.pos;
				GOLFDOC()->GetBar().gaugeStep = GOLFDOC()->m_gaugeInfo.pos;
			}
			break;
		}
		break;

	case 0x74:
		m_holeInPos = *(WVector*)msg.param1;
		break;

	case 0x9e:
		m_bShotBlock = msg.param1 == 1;
		if (m_bShotBlock)
			m_shotBlockTime = 0;
		break;

	case 0x76:
		if (GOLFDOC()->m_gameMode == 8)
		{
			if (m_bShotBlock)
				return;

			if (!m_bShotEnable)
			{
				g_audio->PlaySfx("play_unplayable");
				return;
			}

			ChangeGameMode(0x10);
		}
		else if (GOLFDOC()->m_gameMode & 0x300)
		{
			g_camera = GOLFDOC()->GetDefaultCamera();
			CMouseCursor::Instance()->Store();
			ChangeGameMode(8);
		}
		break;

	case 0x77:
		m_bShotEnable = msg.param1 != 0;
		break;

	case 0x78:
		ChangeGameMode(msg.param1);
		break;

	case 0xa4:
		if (GOLFDOC()->m_gameMode == 0x80)
		{
			GolfBall().m_state = CGolfBall::BALL_STOP;
			return;
		}

		GolfBall().m_state = CGolfBall::BALL_STOP;
		GOLFDOC()->m_gameMode = 0x80;

		if (Doc()->m_replayPlayType == 1)
		{
			PLAYER(0)->bonusPang = 0;
			PLAYER(0)->totalPang = 0;
		}
		break;

	case 0x81:
	{
		m_bNetMsgWait = false;
		m_netMsgRecv = "PM_NET_GAMESYNC_START";
		m_netMsgRecvTime = GetTickCount();
		g_iRecvPacketGR = g_iRecvPacketType;

		GOLFDOC()->m_currentPlayer = (unsigned char)msg.param1;
		SendMsg(this, "Score", 0x130, 0, 0, 0, 0);

		GOLFDOC()->InitShotResultMsg();

		if (OnlinePlay() == 1 &&
			((!GOLFDOC()->m_bNoStat &&
				 GOLFDOC()->m_currentHole == GetStartHole()) ||
				Doc()->m_golfGame.gameType == 0xa))
		{
			ChangeGameMode(2);
		}
		else
		{
			ChangeGameMode(1);
		}

		if (IsLocalContent(S3_MAP_EVENT))
		{
			if (Doc()->IsMapEventActive(GetCurMap()) &&
				GetStartHole() == GOLFDOC()->m_currentHole)
				CChatMsg::Instance()->AddChatMsg(
					"(\xbe\xcb\xb8\xb2)\xb8\xca\xc0\xcc\xba\xa5\xc6\xae\xb0\xa1 "
					"\xc1\xf8\xc7\xe0\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.\xb0\xd4"
					"\xc0\xd3\xbf\xcf\xc1\xd6\xbd\xc3 \xba\xb8\xb3\xca\xbd\xba"
					"\xb0\xa1 \xc1\xd6\xbe\xee\xc1\xfd\xb4\xcf\xb4\xd9.",
					0xffff7878, true, false);
		}

		CShuffleBonus::Instance()->ShowNoticeMsg();
	}
	break;

	case 0x243:
		HoleComboGaugeSetting();
		break;

	case 0x7c:
		GOLFDOC()->m_cameraData.angle =
			CalcDeltaAngle(m_target - GolfBall().m_pos, WVector::UNIT_POS_Z);
		GOLFDOC()->MakeDefaultCamera(true);

		SetAngle(GOLFDOC()->m_cameraData.angle);
		SendMsg(this, "Player", 0x2718, (int)&GOLFDOC()->m_cameraData.angle, 0,
			0, 0);

		if (IsMassGame())
			m_nextPlayer = 0;
		else
			m_nextPlayer = Doc()->GetIndex(msg.param1);

		ChangeGameMode(0x4000);
		break;

	case 0x85:
		QuickSave((const char*)msg.param1);
		break;

	case 0x86:
		QuickLoad((char*)msg.param1);
		break;

	case 0x7d:
		m_bNetMsgWait = false;
		m_netMsgRecv = "PM_NET_NEXT_TURN";
		m_netMsgRecvTime = GetTickCount();
		g_iRecvPacketGR = g_iRecvPacketType;

		if (IsMassGame())
			m_nextPlayer = 0;
		else
			m_nextPlayer = Doc()->GetIndex(msg.param1);

		ChangeGameMode(0x800);

		GolfClub().SetCalcRandomPower(false);
		break;

	case 0xa6:
	{
		if (Doc()->m_golfGame.gameType != 0xa)
			return;
		if (m_pApproachDlg)
			return;

		Doc()->m_approachStartTime = 0;

		m_pApproachDlg = CreateForm<FrApproachResultDlg>(g_pFresh->GetManager(),
			this, "approach_result");
		m_pApproachDlg->SetApproachEnd(true);
		m_pApproachDlg->EnableDrag(false);

		WRect rect = m_pApproachDlg->GetRect();
		m_pApproachDlg->Open((FRESH_PFN_RESULT)&CGolfRule::OnApproachDlgResult,
			WPoint((g_view->GetWidth() * 0.5f) - (rect.w * 0.5f), 35.0f), 0);
		m_pApproachDlg->SetTimeLimit(8.0f);

		rect.w += 30.0f;
		m_pApproachDlg->SetRect(rect);

		CProjectG::Instance()->SetGameSpeed(1.0f);
	}
	break;

	case 0x80:
		if (!GOLFDOC()->IsNetGameEnd())
		{
			GOLFDOC()->NetGameEnd();
			GOLFDOC()->RefreshPlayerNum();

			for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
			{
				if (PLAYER(i)->state == 3)
					continue;

				PLAYER(i)->rank = GOLFDOC()->m_gameResult[i].rank;
				PLAYER(i)->score = GOLFDOC()->m_gameResult[i].score;
				PLAYER(i)->totalStroke = GOLFDOC()->m_gameResult[i].totalStroke;

				PLAYER(i)->totalPang = (int)GOLFDOC()->m_gameResult[i].pang;
				PLAYER(i)->bonusPang =
					(int)GOLFDOC()->m_gameResult[i].bonusPang;

				if (Doc()->m_golfGame.gameType == 1)
				{
					TEAM(PLAYER(i)->team)->totalPang = PLAYER(i)->totalPang;
					TEAM(PLAYER(i)->team)->bonusPang = PLAYER(i)->bonusPang;
				}
				else if (Doc()->m_golfGame.gameType == 7)
				{
					PLAYER(i)->skinsTotalPang =
						GOLFDOC()->m_gameResult[i].skinsPang;
				}
			}

			m_bNetMsgWait = false;
			m_netMsgRecv = "PM_NET_GAME_END";
			m_netMsgRecvTime = GetTickCount();
			g_iRecvPacketGR = g_iRecvPacketType;

			g_view->SetFOV(55.0f * g_DEGTORAD);
			SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1a8, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1a5, 0, 0, 0, 0);
			SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
			SendMsg(this, "Trajectory", 0x18c, 0, 0, 0, 0);
			SendMsg(this, "Trajectory", 0x160, 0, 0, 0, 0);

			CFx::Instance()->ClearAll(true);

			if (Doc()->m_golfGame.gameType == 7)
			{
				if (GOLFDOC()->m_skinsDonator == MyGuid(false) &&
					PLAYER(MyIndex())->skinsTotalPang > 0)
				{
					__int64 donation =
						(int)(PLAYER(MyIndex())->skinsTotalPang * 0.05f);
					PLAYER(MyIndex())->skinsTotalPang -= donation;
					CChatMsg::Instance()->AddChatMsg(
						MakeStr("\xbe\xcb\xb8\xb2 : \xc6\xce\xb9\xe8\xc6\xb2 "
								"\xc8\xb9\xb5\xe6\xc6\xce\xbf\xa1\xbc\xad 5%%(%"
								"I64d\xc6\xce)\xb8\xa6 \xb1\xe2\xba\xce "
								"\xc7\xd5\xb4\xcf\xb4\xd9.",
							donation),
						0xffff7878, false, false);
				}

				CalcSkinsHolePang();
			}

			ChangeGameMode(0x2000);
		}

		if (m_pGoStopDlg)
			m_pGoStopDlg->Close(FrNONE, true);

		if (m_pCancelDropDlg)
			m_pCancelDropDlg->Close(FrCANCEL, true);

		if (CTHunter::IsInstantiated())
			CTHunter::Instance()->SetReturnLobby(true);

		SetGameOver(true);
		break;

	case 0x7e:
		m_bNetMsgWait = false;
		m_netMsgRecv = "PM_NET_NEXT_HOLE";
		m_netMsgRecvTime = GetTickCount();
		g_iRecvPacketGR = g_iRecvPacketType;

		switch (Doc()->m_golfGame.gameType)
		{
		case 7:
			m_bSkinsNextHole = true;
			ChangeGameMode(0x1000);
			CalcSkinsHolePang();
			break;

		case 0xa:
			if (m_pApproachDlg == NULL)
			{
				Doc()->m_approachStartTime = 0;

				m_pApproachDlg = CreateForm<FrApproachResultDlg>(
					g_pFresh->GetManager(), this, "approach_result");
				m_pApproachDlg->EnableDrag(false);

				WRect rect = m_pApproachDlg->GetRect();
				m_pApproachDlg->Open(
					(FRESH_PFN_RESULT)&CGolfRule::OnApproachDlgResult,
					WPoint((g_view->GetWidth() * 0.5f) - (rect.w * 0.5f),
						35.0f),
					0);
				m_pApproachDlg->SetTimeLimit(8.0f);

				rect.w += 30.0f;
				m_pApproachDlg->SetRect(rect);

				CProjectG::Instance()->SetGameSpeed(1.0f);
			}
			break;

		default:
			ChangeGameMode(0x2000);
			break;
		}

		GOLFDOC()->ClearShotResultMsg();
		GolfClub().SetCalcRandomPower(false);
		break;

	case 0x79:
		if (GOLFDOC()->m_gameMode & 0x40)
		{
			if (OnlinePlay())
			{
				SendMsg(this, "Cutin", 0x78, 0x10, 0, 0, 0);
			}
			else
			{
				m_bNetMsgWait = false;
				m_netMsgRecv = "PM_NET_SHOT_RESULT";
				m_netMsgRecvTime = GetTickCount();
				g_iRecvPacketGR = g_iRecvPacketType;
			}

			m_shotFlag &=
				GOLFDOC()->m_shotResult[GOLFDOC()->m_currentPlayer].ruleFlag;

			if (Doc()->m_golfGame.gameType == 0 &&
				(GOLFDOC()->m_shotResult[GOLFDOC()->m_currentPlayer].shotFlag &
					0x2000))
			{
				if (S5::CLASSICSRV::IsClassicServer(
						S5::UTIL::GetServerProperty()))
				{
					unsigned long bonus = 0;
					for (unsigned char i = 0; i < GetHoles(); i++)
						bonus = S5::CLASSICSRV::GetClearBonus(
							GOLFDOC()
								->GetHoleData(Doc()->m_holeOrder[i])
								.playerNum,
							GetCurMap());

					GOLFDOC()
						->m_shotResult[GOLFDOC()->m_currentPlayer]
						.bonusPang += bonus;
				}
				else
				{
					unsigned char birdie = 0;
					unsigned char eagle = 0;
					unsigned char albatross = 0;
					for (unsigned char i = 0; i < GetHoles(); i++)
					{
						switch (GOLFDOC()
								->GetHoleData(Doc()->m_holeOrder[i])
								.playerNum)
						{
						case 2:
							birdie++;
							break;
						case 3:
							eagle++;
							break;
						case 4:
							albatross++;
							break;
						}
					}

					GOLFDOC()
						->m_shotResult[GOLFDOC()->m_currentPlayer]
						.bonusPang += (albatross * 3 + eagle * 2 + birdie) *
						s_clearBonus[GetCurMap()];
				}
			}

			m_bShotResult = true;
			m_bCanSkip = GolfBall().m_groundType != 2;

			CorrectShotResult();

			SendMsg(this, "Player", 0x77, 0, 0, 0, 0);
		}
		break;

	case 0x87:
	{
		switch (msg.param1)
		{
		case 0:
			m_bAutoShot = false;
			break;

		case 1:
			m_bAutoShot = true;
			g_input->SetFakeInput("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
				false);
			m_autoShotPos[0] = (float)msg.param2;
			m_autoShotPos[1] = (float)msg.param3;
			break;

		case 2:
			m_bAutoShot = true;
			g_input->SetFakeInput("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
				false);
			m_autoShotPos[0] = (float)msg.param2;
			m_autoShotPos[1] = (float)msg.param3;
			break;
		}

		IActor* pTutorial = GetActor("Tutorial");
		if (m_bAutoShot)
		{
			if (pTutorial == NULL)
				return;

			pTutorial << MsgObject(NULL, 0x20f, 0, 0, 0, 0, 0);
		}
		else
		{
			if (pTutorial == NULL)
				return;

			pTutorial << MsgObject(NULL, 0x20f, 1, 0, 0, 0, 0);
		}
	}
	break;

	case 0xa3:
		GOLFDOC()->m_bLockGauge = !GOLFDOC()->m_bLockGauge;
		break;

	case 0x82:
		if (GOLFDOC()->m_bNetGameEnd)
			return;

		PLAYER(Doc()->GetIndex(msg.param1))->state = 3;

		if (Doc()->m_golfGame.gameType == 1)
		{
			bool bAllOut = true;
			unsigned char team = PLAYER(Doc()->GetIndex(msg.param1))->team;

			for (int i = 0; i < GOLFDOC()->m_playerNum; i++)
			{
				if (team == PLAYER(i)->team && PLAYER(i)->state != 3)
					bAllOut = false;
			}

			if (bAllOut)
				TEAM(team)->state = 3;
		}

		GOLFDOC()->RefreshPlayerNum();
		PLAYER(Doc()->GetIndex(msg.param1))->rank =
			PLAYER(Doc()->GetIndex(msg.param1))->order =
				GOLFDOC()->GetCurrentPlayerNum() + 1;

		ResetRank();

		if (Doc()->m_golfGame.gameType == 0)
		{
			GOLFDOC()->m_maxGrade = 0;

			for (int i = 0; i < GOLFDOC()->m_playerNum; i++)
			{
				if (PLAYER(i)->state == 3)
					continue;

				unsigned char grade;
				if (Doc()->m_userInfo[i].stat.Level < 6)
					grade = 1;
				else
					grade = (Doc()->m_userInfo[i].stat.Level - 1) / 5 + 1;

				if (GOLFDOC()->m_maxGrade < grade)
					GOLFDOC()->m_maxGrade = grade;
			}
		}
		else if (Doc()->m_golfGame.gameType == 7)
		{
			int index = Doc()->GetIndex(msg.param1);
			int hole =
				GOLFDOC()->m_bNoStat ? GetEndHole() : GOLFDOC()->m_currentHole;

			__int64 pang = Doc()->m_userInfo[index].stat.i64Pang <
					GOLFDOC()->m_skinsHolePang
				? Doc()->m_userInfo[index].stat.i64Pang
				: GOLFDOC()->m_skinsHolePang;

			PLAYER(index)->skinsPang[hole - 1] -= (int)pang;
			PLAYER(index)->skinsSumPang -= pang;

			if (PLAYER(index)->skinsSumPang > 0)
				PLAYER(index)->skinsSumPang = 0;

			PLAYER(index)->skinsTotalPang = PLAYER(index)->skinsSumPang;
			GOLFDOC()->m_skinsForfeitPang += pang;
			PLAYER(index)->stroke[hole - 1] = 0xff;
		}
		break;

	case 0x83:
		if (m_bShotResult)
			Skip();
		break;

	case 0x8d:
		Drop(*(WVector*)msg.param1);
		break;

	case 0xa7:
		ForcedMoveBall(*(WVector*)msg.param1);
		break;

	case 0x8e:
		Warp(*(WVector*)msg.param1);
		break;

	case 0x8f:
		if (GOLFDOC()->m_tutorialMode < 0xf)
		{
			if (msg.param1 == 0 && Doc()->m_golfGame.gameType == 0xc)
			{
				WSendPacket packet((enumClientPacket)0x2e);
				packet.Send(TO_GAME);
			}

			ReturnLobby(false);
		}
		else if (GetGameType() == 3)
		{
			WSendPacket packet((enumClientPacket)0xf);
			packet.Encode1(0);
			packet.Encode2(0xffff);
			packet.Encode8(0);
			packet.Encode8(0);
			packet.Send(TO_GAME);
		}
		else if (msg.param1 == 1)
		{
			ReturnLobby(false);
		}
		else if (msg.param1 == 2)
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
				(FrCmdTarget*)this, "notify");
			pForm->SetMessage(
				"\xbb\xf3\xb4\xeb\xb0\xa1 \xb0\xd4\xc0\xd3\xbf\xa1"
				"\xbc\xad \xb3\xaa\xb0\xac\xbd\xc0\xb4\xcf\xb4\xd9",
				false);
			pForm->Open((FRESH_PFN_RESULT)&CGolfRule::OnReturnLobbyResult, 3);
		}
		break;

	case 0x90:
	{
		unsigned char index = (unsigned char)msg.param1;
		if (Doc()->m_userInfo[index].info.dwIdentity & 1)
		{
			Doc()->m_userInfo[index].info.dwIdentity &= ~1;
			SendMsg(this, "Letter", 0x18c, (int)"AI", 0, 0, 0);
			return;
		}

		Doc()->m_userInfo[index].info.dwIdentity |= 1;
		SendMsg(this, "Letter", 0x18d, (int)"AI", 0, 0, 0);
		ChangeGameMode(8);
		SetEnvironmentVariableA();
	}
	break;

	case 0x91:
		m_reservedSpecial |= msg.param1;
		break;

	case 0x92:
		memcpy(m_backUp.item, Doc()->m_userInfo[0].userEquip.tidItemSlot,
			sizeof(m_backUp.item));
		break;

	case 0x93:
		SendShotResult();
		break;

	case 0x84:
		if (msg.param1)
		{
			if (m_pPauseDlg)
				return;

			m_pPauseDlg =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			m_pPauseDlg->SetMessage((const char*)msg.param2, false);
			m_pPauseDlg->Open((FRESH_PFN_RESULT)&CGolfRule::PauseDlgResult, 0);
			m_pPauseDlg->EnableDrag(false);

			GOLFDOC()->SetPause(true);
			CMouseCursor::Instance()->Enable(0x2a, false);
		}
		else
		{
			GOLFDOC()->SetPause(false);

			if (m_pPauseDlg)
				m_pPauseDlg->Close(FrNONE, true);
		}
		break;

	case 0x94:
		m_bNetMsgWait = false;
		m_netMsgRecv = "PM_NET_TEESHOT_READY_ACK";
		m_netMsgRecvTime = GetTickCount();
		g_iRecvPacketGR = g_iRecvPacketType;

		GOLFDOC()->SetApproachReloaded(false);
		break;

	case 0x95:
	{
		GOLFDOC()->m_bStopDlg = true;

		if (m_pGoStopDlg == NULL)
		{
			m_goStopTime = 0;
			m_pGoStopDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify_yesno");
			m_pGoStopDlg->Open((FRESH_PFN_RESULT)&CGolfRule::GoStopDlgResult,
				0);
			m_pGoStopDlg->EnableDrag(false);
		}

		int score;
		SendMsg(this, "Score", 0x13a, (int)&score, 0, 0, 0);

		char text[1024];
		sprintf(text,
			"\xbb\xf3\xb4\xeb\xb0\xa1 \xc5\xf0\xc0\xe5\xc7\xd8\xbc\xad \xb4\xf5 \xc0\xcc\xbb\xf3\xc0\xc7 \xb4\xeb\xc0\xfc\xc0\xba \xba\xd2\xb0\xa1\xb4\xc9\xc7\xcf\xb3\xaa, \xb0\xb3\xc0\xce \xb1\xe2\xb7\xcf \xbc\xf6\xb8\xb3\xc0\xbb \xc0\xa7\xc7\xd8\xbc\xad \xc7\xf6\xc0\xe7\xb1\xee\xc1\xf6 \xbe\xf2\xc0\xba\n"
			"\\c0xffff0000\\c\xb0\xe6\xc7\xe8\xc4\xa1 %d\\c0xff000000\\c\xbf\xcd \\c0xffff0000\\c%d \xc6\xce\\c0xff000000\\c\xb8\xb8\xc0\xbb \xb9\xde\xb0\xed \xb0\xd4\xc0\xd3\xc0\xbb \xb0\xe8\xbc\xd3 \xc1\xf8\xc7\xe0 \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\n"
			"\xb1\xd7\xb8\xae\xb0\xed, \xb0\xd4\xc0\xd3\xc0\xbb \xbf\xcf\xc1\xd6\xc7\xcf\xb8\xe9 \xc5\xac\xb8\xae\xbe\xee \xba\xb8\xb3\xca\xbd\xba \xc6\xce\xc0\xbb \xb9\xde\xc0\xbb \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\n"
			"\xb0\xe8\xbc\xd3 \xc1\xf8\xc7\xe0 \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			score, (int)PLAYER(MyIndex())->totalPang);
		m_pGoStopDlg->SetMessage(text, false);
		m_pGoStopDlg->SetTimeLimit(10.0f);
	}
	break;

	case 0x62:
		if (m_pEmoticonBtn)
			m_pEmoticonBtn->SetVisible(msg.param1 != 0);

		if (m_pLanguage)
			m_pLanguage->SetVisible(msg.param1 != 0);

		if (m_pReportBtn)
			m_pReportBtn->SetVisible(msg.param1 != 0);

		if (m_pChatTarget)
			m_pChatTarget->SetVisible(msg.param1 != 0);

		if (m_pChatInput)
		{
			if (msg.param1)
			{
				m_pChatInput->SetVisible(true);
				m_pChatInput->SetKeyFocus(true);
			}
			else
			{
				m_pChatInput->SetVisible(false);
			}

			g_pFresh->GetManager()->SetExclusiveKey(msg.param1 != 0);
		}
		break;

	case 0x3f:
		*(const char**)msg.param1 =
			m_pChatInput ? m_pChatInput->GetLine(1, false) : NULL;
		break;

	case 0x37:
		if (Doc()->AddWhisperPartner(m_pChatTarget, (const char*)msg.param1))
			m_pChatTarget->SetLine(1, (const char*)msg.param1, 0, false, 0);

		if (CUserInfo::Instance()->IsVisible())
			CUserInfo::Instance()->Close();
		if (m_pChatInput)
			m_pChatInput->SetKeyFocus(true);
		break;

	case 0x65:
		if (Doc()->m_recentWhisperList.size() > 0)
			m_pChatTarget->SetLine(1, Doc()->m_recentWhisperList.back().c_str(),
				0, false, 0);
		break;

	case 0x38:
		Doc()->AddWhisperPartner(m_pChatTarget, (const char*)msg.param1);
		break;

	case 0x47:
		Doc()->InitRecentWhisperList(m_pChatTarget);
		break;

	case 0x64:
	{
		if (m_pChatView == NULL)
			return;

		int lines = m_pChatView->GetScrollBar()->GetRowCapacity();
		for (int i = 1; i < lines; i++)
		{
			const char* text = m_pChatView->GetLine(i + 1, false);
			if (text)
				m_pChatView->SetLine(i, text, 0, false,
					m_pChatView->GetAnimTime(i + 1));
		}

		m_pChatView->SetLine(lines, (const char*)msg.param1, msg.param2, false,
			timeGetTime());
	}
	break;

	case 0x96:
	{
		if (m_pChatView == NULL)
			return;

		int lines = m_pChatView->GetScrollBar()->GetRowCapacity();
		for (int i = 0; i < lines; i++)
		{
			const char* text = m_pChatView->GetLine(i + 1, false);
			if (text && *text)
			{
				m_pChatView->SetLine(i + 1, "", 0, false, 0);
				break;
			}
		}
	}
	break;

	case 0x63:
	{
		if (m_pChatView == NULL)
			return;

		int lines = m_pChatView->GetScrollBar()->GetRowCapacity();
		for (int i = 0; i < lines; i++)
			m_pChatView->SetLine(i + 1, "", 0, false, 0);
	}
	break;

	case 0x97:
		if (IsLocalContent(S4_REPLAY_SYSTEM))
		{
			if (Doc()->m_replayPlayType != 2)
				CHelpMsg::Instance()->CheckReplayAvailable();
		}
		else
		{
			Replay(false);
			CHelpMsg::Instance()->CheckReplayAvailable();
		}
		break;

	case 0x9c:
		m_bNetMsgWait = false;
		m_netMsgRecv = "PM_NET_CANCEL_SHOT";
		m_netMsgRecvTime = GetTickCount();
		g_iRecvPacketGR = g_iRecvPacketType;

		Cancel();
		break;

	case 0x9f:
		if (msg.param1 == Doc()->m_myInfo.info.dwGuid)
			ChangeGameMode(0x2000);
		break;

	case 0x5e:
	{
		unsigned long uid = msg.param1;
		if (Doc()->m_teamPlayerGuid[0] != uid &&
			Doc()->m_teamPlayerGuid[1] != uid)
			return;

		sRivalData* pRival1 = GUILDMATCHUP(0);
		sRivalData* pRival2 = GUILDMATCHUP(1);
		if (pRival1 == NULL || pRival2 == NULL)
			return;

		int hole = msg.param2;
		if (pRival1->holeStroke[hole - 1] == 0 ||
			pRival2->holeStroke[hole - 1] == 0)
			return;

		char text[1024];
		if (pRival1->holeStroke[hole - 1] < pRival2->holeStroke[hole - 1])
			sprintf(text,
				"%s\xb4\xd4\xc0\xcc %d\xc8\xa6\xbf\xa1\xbc\xad "
				"\xbd\xc2\xb8\xae\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
				pRival1->nickname, hole);
		else if (pRival1->holeStroke[hole - 1] > pRival2->holeStroke[hole - 1])
			sprintf(text,
				"%s\xb4\xd4\xc0\xba %d\xc8\xa6\xbf\xa1\xbc\xad "
				"\xc6\xd0\xb9\xe8\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
				pRival1->nickname, hole);
		else
			sprintf(text,
				"%d\xc8\xa6\xc0\xba \xb9\xab\xbd\xc2\xba\xce\xb0\xa1 "
				"\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
				hole);

		CChatMsg::Instance()->AddChatMsg(text, 0xffff7878, true, false);

		if (pRival1->holeStroke[hole - 1] < pRival2->holeStroke[hole - 1])
			GOLFDOC()->GetHoleData(Doc()->m_holeOrder[hole - 1]).result = 0;
		else if (pRival1->holeStroke[hole - 1] > pRival2->holeStroke[hole - 1])
			GOLFDOC()->GetHoleData(Doc()->m_holeOrder[hole - 1]).result = 1;
		else
			GOLFDOC()->GetHoleData(Doc()->m_holeOrder[hole - 1]).result = 2;
	}
	break;

	case 0xb1:
	{
		sBonusBall* pBonusBall = (sBonusBall*)msg.param1;
		if (pBonusBall == NULL)
			return;

		m_bClosedBonusBall = true;
		memcpy(m_closedBonusBall, pBonusBall, sizeof(m_closedBonusBall));
	}
	break;

	case 0x1f9:
	{
		if (CQuitWindow::Instance()->IsActive())
			return;

		bool bShow =
			!CProjectG::Instance()->HideGUI() && m_bShowUI ? true : false;
		SendMsg(this, "Screen", 0x190, bShow ? 1 : 0, 0, 0, 0);
		SendMsg(this, "Letter", 0x190, bShow ? 1 : 0, 0, 0, 0);

		if (g_pFresh)
			g_pFresh->Toggle(bShow);

		CChatMsg::Instance()->Toggle(
			!CProjectG::Instance()->HidePI() && m_bShowUI ? true : false);

		CMouseCursor::Instance()->Toggle(!CProjectG::Instance()->HideGUI() &&
					!CProjectG::Instance()->HidePI() && m_bShowUI
				? true
				: false);
	}
	break;

	case 0x24d:
	{
		bool bWait = false;
		if (msg.param1 == 1)
			bWait = true;

		SetNetMsgWait(bWait, 6103, (char*)msg.param2, 20.0f);
	}
	break;

	default:
		HandleMsgCommon(msg);
		break;
	}
}

void CGolfRule::DispatchMatchItems()
{
	unsigned char interval = GetHoles() / 9;
	unsigned char hole = GetHoleIndex(GOLFDOC()->m_currentHole);

	if (interval == 0)
		interval = 1;

	if (hole > 0 && hole % interval == 0 && Doc()->m_golfGame.gameType == 3)
	{
		unsigned long typeId = 0;
		unsigned long sum = 0;
		unsigned long i = 0;
		unsigned long rate =
			Doc()->m_holeRandom[(unsigned char)(hole / interval)] % 100;

		for (; i < 6; i++)
		{
			sum += g_ladderItem[i].rate;
			if (sum > rate)
			{
				typeId = g_ladderItem[i].typeIndex | 0x18000000;
				break;
			}
		}

		int slots = 10;
		if (IsLocalContent(S3_MASCOT))
			slots = 8;

		for (int i = 0; i < GetPlayerNum(); i++)
		{
			if (GOLFDOC()->GetPlayer(i)->state == 3)
				continue;

			for (int k = 0; k < slots; k++)
			{
				if (Doc()->m_userInfo[i].userEquip.tidItemSlot[k] == 0x1800000d)
				{
					Doc()->m_userInfo[i].userEquip.tidItemSlot[k] = typeId;
					break;
				}
			}
		}
	}
}

bool IsMyTurn(bool bTeam);
extern Fresh* g_pFresh;

void CGolfRule::ChangeGameMode(unsigned long mode)
{
	unsigned long prevMode = GOLFDOC()->GameMode();
	CGolfDoc* pGolfDoc = GOLFDOC();

	if (pGolfDoc->GameMode() == 0x200 && pGolfDoc->m_bDropRequest)
	{
		if (mode == 0x4000)
		{
			GOLFDOC()->m_bDropRequest = false;

			if (m_pCancelDropDlg)
				m_pCancelDropDlg->Close(FrNONE, false);
		}
		else
		{
			if (m_pCancelDropDlg)
				return;

			m_pCancelDropDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify_yesno", NULL);
			m_pCancelDropDlg->Open(
				(FRESH_PFN_RESULT)&CGolfRule::CancelDropDlgResult, 0);
			m_pCancelDropDlg->SetMessage(
				"\xb0\xf8 \xbf\xc5\xb1\xe2\xb1\xe2 \xb8\xf0\xb5\xe5\xb8\xa6 \xc3\xeb\xbc\xd2\xc7\xd5\xb4\xcf\xb4\xd9.",
				false);
			m_pCancelDropDlg->EnableDrag(false);
			return;
		}
	}

	GOLFDOC()->GameMode() = mode;
	m_processTime = 0.0f;

	CGolfBall* pBall = &GolfBall();

	switch (mode)
	{
	case 0x200000:
		GOLFDOC()->m_shotPhase = 0;
		m_timer = Random(0.0f, 36.0f);
		m_cameraTarget = (GetHoleCupPos() + GetTeeShotPos()) * 0.5f;
		g_view->SetClip(32.0f, 3200.0f, true);

		CSceneManager::Instance()->SetVisible(
			MakeStr("PLAYER_%d", GOLFDOC()->m_currentPlayer), false);
		CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", false);
		CSceneManager::Instance()->SetVisible("\xc4\xb3\xb5\xf0", false);
		CShadowManager::Instance()->Enable(false);

		SendMsg(this, "Score", 0x13b, 1, 0, 0, 0);
		break;

	case 0x40000:
		GOLFDOC()->GetUIAlpha() = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();
		break;

	case 1:
	{
		IActor* pGateway = GetActor(s_gatewayActor[1]);
		pGateway << MsgObject(NULL, 0x26f, 12, 0, 0, 0, 0);

		GetPVS().ResetHoleData();
		SendMsg(this, "Letter", 0x18c, 0, 0, 0, 0);
		SendMsg(this, "Score", 0x12e, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);

		m_timer = 0.0f;

		GOLFDOC()->SetTeeShotReady(false);
		pBall->Reset(GetTeeShotPos() + WVector(0.0f, pBall->GetRadius(), 0.0f));
		pBall->m_meshIndex = GetPVS().GetMeshIndex(pBall->m_pos, false);
		ResetData(pBall->m_pos);

		CTHunter::Instance()->SetReceiveGiftPacket(false);
		Doc()->m_bTreasureResult = false;
		Doc()->m_bTreasureAward = false;

		if (OnlinePlay() && GOLFDOC()->m_currentHole == GetStartHole() &&
			(GetGameType() == 10 || !GOLFDOC()->m_bNoStat))
		{
			SendMsg(this, "GolfBg", 0x1e5, (int)&m_flyByPos, 0, 0, 0);
		}
		else
		{
			ChangeGameMode(2);
		}
	}
	break;

	case 0x10000:
		if (GetCurMapProp() && GetCurMapProp()->AmbientSnd[0])
			g_audio->PlaySfx(GetCurMapProp()->AmbientSnd, NULL, 1, NULL, NULL,
				0.5f, 200.0f);

		SendMsg(this, "GolfBg", 0x142, m_shotData.weather, 0, 0, 0);

		Wind().Init(0xff, 0xff, 0, 1);
		Wind().SetWind(m_shotData.wind);
		GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		CSceneManager::Instance()->UpdateWind();

		GetPVS().ResetHoleData();

		m_shotData.pos =
			GetPVS().GetGroundPoint(m_shotData.pos, false, NULL, false);
		m_shotData.pos.y += pBall->GetRadius();

		pBall->Reset(m_shotData.pos);
		pBall->m_meshIndex = GetPVS().GetMeshIndex(pBall->m_pos, false);
		ResetData(pBall->m_pos);

		PLAYER(0)->bunker = m_shotData.bunker;

		GetPVS().SetFrame(0);
		SetEnvironmentVariableA();

		{
			float fade = m_shotData.clubPower ? 1.0f : 0.0f;
			SendMsg(this, "Screen", 0x1aa, (int)&fade, 0, 0, 0);
		}
		SendMsg(this, "Screen", 0x1b3, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1c5, 0, 0, 0, 0);
		SendMsg(this, "Logo", 0x18b, 0, 0, 0, 0);
		SendMsg(this, "Item", 0x19b, 0, 0, 0, 0);

		ChangeTurn();
		GOLFDOC()->MakeDefaultCamera(false);

		CShadowManager::Instance()->SetActive(true);

		GOLFDOC()->m_currentPlayer = 0;

		if (Doc()->GetReplayPlayType() == 2 ||
			(Doc()->GetReplayPlayType() == 1 && Doc()->m_replayShotIndex == 0))
			ChangeGameMode(2);
		else
			ChangeGameMode(8);
		break;

	case 0x100000:
	{
		char path[512];
		strcpy(path, g_executeDirectory);
		sprintf(path, "%s/data/Sound/bgm", path);

		CheckUserBGM(path);
		m_bgm = rand() % m_bgmList.size();

		g_audio->StopSfx(NULL);

		switch (GetWeather())
		{
		case 2:
			if (GetCurMap() != 9 && !IsCurMapSnow())
				g_audio->PlaySfx("\xba\xf1", NULL, 1, "AmbientSound");
			break;

		default:
			if (GetCurMapProp() && GetCurMapProp()->AmbientSnd[0])
				g_audio->PlaySfx(GetCurMapProp()->AmbientSnd, NULL, 1,
					"AmbientSound");
			else
				g_audio->StopSfx("AmbientSound");
			break;
		}

		GOLFDOC()->m_shotPhase = 0;

		GetPVS().ResetHoleData();
		pBall->Reset(PLAYER(GOLFDOC()->m_currentPlayer)->pos);
		pBall->m_meshIndex = GetPVS().GetMeshIndex(pBall->m_pos, false);

		m_bShotResult = false;
		m_bSendFinishShot = false;
		m_bBallStopped = false;
		m_bTurnSetup = false;
		m_bCanSkip = false;

		SendMsg(this, "Ball", 0x1eb, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a3, 0, 0, 0, 0);
		SendMsg(this, "Clock", 0x1e9, 0, 0, 0, 0);
		SendMsg(this, "GolfBg", 0x13d, GetWeather(), 0, 0, 0);

		m_target = pBall->m_pos + RotMat(GOLFDOC()->m_cameraData.angle, 1).za;

		SetAngle(GOLFDOC()->m_cameraData.angle);
		SendMsg(this, "Player", 0xe2, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x13, 0, 0, 0, 0);

		SendMsg(this, "Spot", 0xf5, 0, 0, 0, 0);

		{
			float fade = GolfClub().PowerShotType() ? 1.0f : 0.0f;
			SendMsg(this, "Screen", 0x1aa, (int)&fade, 0, 0, 0);
		}
		SendMsg(this, "Screen", 0x1b3, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1c5, 0, 0, 0, 0);
		SendMsg(this, "Logo", 0x18b, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 0x18c, 0, 0, 0, 0);
		SendMsg(this, "Letter", 0x18c, 0, 0, 0, 0);

		SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x17f, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x178, 0, 0, 0, 0);

		SendMsg(this, "Score", 0x12f, 0, 0, 0, 0);

		ChangeTurn();
		GOLFDOC()->MakeDefaultCamera(false);
		GOLFDOC()->m_commandList.clear();

		WPuppet* pPuppet = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
		if (pPuppet && pPuppet->GetRootBone())
			pPuppet->SetAlpha(0xff);

		CShadowManager::Instance()->SetActive(true);
		CFx::Instance()->ClearAll(true);

		ChangeGameMode(8);

		Wind().MakeWindList();
		Wind().CheckSpecialWind();
		GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
	}
	break;

	case 2:
		if (Doc()->GetPlayingMode() == (eReplayModeType)1)
		{
			if (prevMode == 0x400000)
			{
				m_bPlayerPreviewReady = true;
			}

			if (!m_bPlayerPreviewReady)
			{
				ChangeGameMode(0x400000);
				return;
			}

			Doc()->m_shotDataList.clear();

			m_recordPang = PLAYER(MyIndex())->totalPang;
			m_recordBonusPang = PLAYER(MyIndex())->bonusPang;
		}
		else if (Doc()->GetPlayingMode() == (eReplayModeType)2)
		{
			PLAYER(0)->bonusPang = 0;
			PLAYER(0)->totalPang = 0;

			Doc()->SetReplayState(1);

			Doc()->m_replayShotIndex = 1;
		}

		switch (GetWeather())
		{
		case 2:
			if (GetCurMap() != 9 && !IsCurMapSnow())
				g_audio->PlaySfx("\xba\xf1", NULL, 1, "AmbientSound");
			break;

		case 1:
			if (GetCurMap() != 9)
				g_audio->PlaySfx(MakeStr("\xb9\xf8\xb0\xb3\xc1\xd8\xba\xf1%d",
					rand() % 2 + 1));

		default:
			if (GetCurMapProp() && GetCurMapProp()->AmbientSnd[0])
				g_audio->PlaySfx(GetCurMapProp()->AmbientSnd, NULL, 1,
					"AmbientSound");
			else
				g_audio->StopSfx("AmbientSound");
			break;
		}

		m_bgm = rand() % m_bgmList.size();

		SendMsg(this, "GolfBg", 0x13d, GetWeather(), 0, 0, 0);
		SendMsg(this, "Effect", 0x17a, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x178, 0, 0, 0, 0);

		if (!OnlinePlay())
		{
			this << MsgObject(NULL, 0x243, 0, 0, 0, 0, 0);
		}

		for (int i = 0; i < GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 3)
				continue;

			if (!GOLFDOC()->m_bNoStat)
				Doc()->m_holeStatistics[i].dwHole++;

			PLAYER(i)->state = 0;
			PLAYER(i)->pos =
				GetTeeShotPos() + WVector(0.0f, GolfBall().GetRadius(), 0.0f);
		}

		GOLFDOC()->m_currentPlayer = GetFirstTurn();

		GetPVS().SetFrame(0);
		SetEnvironmentVariableA();

		if (IsOthersTurn(false))
			SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);

		SendMsg(this, "Caddie", 0x163, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1b2, 0, 0, 0, 0);
		SendMsg(this, "Logo", 0x18b, 0, 0, 0, 0);
		SendMsg(this, "NPC", 0x18a, 0, 0, 0, 0);

		if ((IsMassGame() && GOLFDOC()->m_currentHole != GetStartHole()) ||
			GOLFDOC()->m_bNoStat)
		{
			if (!GOLFDOC()->m_bNoStat)
				SendMsg(this, "Player", 0x2716,
					(int)"\xb5\xee\xc0\xe5\xb8\xf0\xbc\xc7", -3, 1, 0);

			SendMsg(this, "Player", 0x125, 1, 0, 0, 0);
		}
		else
		{
			SendMsg(this, "Player", 0x2716,
				(int)"\xb5\xee\xc0\xe5\xb8\xf0\xbc\xc7", 0, 0, 0);
		}

		ChangeTurn();
		GOLFDOC()->MakeDefaultCamera(false);

		CCameraWork::Instance()->SelectCamera(0xcb);

		PLAYER(GOLFDOC()->m_currentPlayer)
			->stroke[GOLFDOC()->m_currentHole - 1] = 1;

		if (GetGameType() == 1)
		{
			for (int i = 0; i < 2; i++)
				TEAM(i)->state = 0;

			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
				->stroke[GOLFDOC()->m_currentHole - 1] = 1;
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->shooter =
				GOLFDOC()->m_currentPlayer;
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->club =
				GolfClub().GetType();
		}
		else if (GetGameType() == 7)
		{
			if (!GOLFDOC()->m_bNoStat)
			{
				if (GOLFDOC()->m_skinsHolePang)
				{
					GOLFDOC()->m_skinsCarryPang +=
						GetHolePang(GetHoleIndex(GOLFDOC()->m_currentHole));

					GOLFDOC()->m_skinsMultiple =
						min(GOLFDOC()->m_skinsMultiple * 2, 8);

					GOLFDOC()->m_skinsHolePang = GOLFDOC()->m_skinsMultiple *
						GOLFDOC()->m_skinsCarryPang;
					GOLFDOC()
						->GetHoleData(GetHoleIndex(GOLFDOC()->m_currentHole))
						.pang = GOLFDOC()->m_skinsHolePang;
				}
				else
				{
					GOLFDOC()->m_skinsCarryPang =
						GetHolePang(GetHoleIndex(GOLFDOC()->m_currentHole));
					GOLFDOC()->m_skinsHolePang =
						GetHolePang(GetHoleIndex(GOLFDOC()->m_currentHole));
					GOLFDOC()->m_skinsMultiple = 1;
				}

				GOLFDOC()->m_skinsCoursePang = 0;

				for (int i = 0; i < GetHoles(); i++)
				{
					int hole = Doc()->m_holeOrder[i];

					if (i < GetHoleIndex(GOLFDOC()->m_currentHole) &&
						GOLFDOC()->GetHoleData(hole).pang)
					{
						__int64 pang = GOLFDOC()->GetHoleData(hole).pang;
						int skinsPang = PLAYER(MyIndex())->skinsPang[hole - 1];

						if (skinsPang != -1)
							GOLFDOC()->m_skinsCoursePang += pang;
					}
					else
					{
						GOLFDOC()->m_skinsCoursePang +=
							GOLFDOC()->GetHoleData(hole).basePang;
					}
				}
			}
		}

		CShadowManager::Instance()->SetActive(true);

		Wind().MakeWindList();
		Wind().CheckSpecialWind();
		if (GOLFDOC()->m_pPolySoup)
			GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		break;

	case 4:
	{
		g_view->SetFOV(55.0f * g_DEGTORAD);

		SendMsg(this, "Screen", 0x1a4, 1, 0, 0, 0);
		SendMsg(this, "Caddie", 0x2716, (int)"\xc2\xf9\xbd\xba", 0, 0, 0);

		if (GetGameType() != 1)
			SendMsg(this, "Caddie", 0x1dc, 0x22, 0, 0, 0);

		WVector caddiePos;
		GetGameType();
		SendMsg(this, "Caddie", 0xf4, (int)&caddiePos, 0, 0, 0);
		CCameraWork::Instance()->SetCaddieCamera(g_camera, caddiePos);
	}
	break;
		GetGameType();

	case 8:
		if (IsLocalContent((localContentType_t)0x4f))
		{
			if (Doc()->GetPlayingMode() == (eReplayModeType)1)
			{
				Doc()->SetReplayPlayType(0);

				m_recordGauge = GetGameType() == 1
					? TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->gauge
					: PLAYER(GOLFDOC()->m_currentPlayer)->gauge;
			}
			else if (Doc()->GetPlayingMode() == (eReplayModeType)2)
			{
				SetEnvironmentVariableA();
			}
		}
		GetGameType();
		GetGameType();

		if (IsLocalContent((localContentType_t)0x51))
		{
			CCardManager::Instance()->SetPlayerIndex(
				GOLFDOC()->m_currentPlayer);
			GetGameType();
			CCardManager::Instance()->CalcCardPeriodAndStatus();
		}

		_ImeReset();

		if (GOLFDOC()->m_tutorialMode == 15)
		{
			if (m_pOverBarSpecialCard)
				m_pOverBarSpecialCard->SetVisible(true);

			if (m_pMyInfoBtn)
				m_pMyInfoBtn->SetVisible(true);

			if (m_pOptionBtn)
				m_pOptionBtn->SetVisible(true);

			ToggleCommandBtns(true);

			if (m_pOnelineBtn)
				m_pOnelineBtn->SetVisible(true);

			if (m_pOnelineView)
				m_pOnelineView->SetVisible(true);

			COneLineBoard::Instance()->ShowBg(true);

			if (m_pMessengerBtn)
				m_pMessengerBtn->SetVisible(true);

			if (Doc()->m_bMessengerAlarm && m_pMessengerAlarmBtn)
			{
				m_pMessengerAlarmBtn->SetTopmost(true);
				m_pMessengerAlarmBtn->SetVisible(true);
			}

			if (Doc()->m_bRefreshCamera && m_pMessengerGiftAlarmBtn)
			{
				m_pMessengerGiftAlarmBtn->SetTopmost(true);
				m_pMessengerGiftAlarmBtn->SetVisible(true);
			}
		}
		else if (GOLFDOC()->m_tutorialMode == 9)
		{
			ToggleCommandBtns(true);
		}
		else
		{
			if (Doc()->m_bRefreshCamera && m_pMessengerGiftAlarmBtn)
			{
				m_pMessengerGiftAlarmBtn->SetTopmost(false);
				m_pMessengerGiftAlarmBtn->SetVisible(false);
			}
		}

		m_bTurnSetup = true;

		if (pBall->m_bOB)
		{
			pBall->m_bOB = false;
			GOLFDOC()->MakeDefaultCamera(true);
			pBall->m_water = CGolfBall::WATER_NONE;
		}
		else
		{
			g_camera = GOLFDOC()->GetDefaultCamera();
		}

		if (IsMyTurn(true) ||
			(IsLocalContent((localContentType_t)0x4f) &&
				Doc()->GetPlayingMode() == (eReplayModeType)2))
		{
			SendMsg(this, "Spin", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1a7, 0, 0, 0, 0);
			GOLFDOC()->GetUIAlpha() = 0xff;
			CChatMsg::Instance()->ReplaceBackupUiAlpha();
		}
		else
		{
			SendMsg(this, "Screen", 0x1a8, 0, 0, 0, 0);
		}

		if (!(Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].info.dwIdentity &
				1))
			SendMsg(this, "Camera", 0x19a, 0, 0, 0, 0);

		SendMsg(this, "Camera", 0xee, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a4, 1, 0, 0, 0);
		SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);

		g_view->SetFOV(55.0f * g_DEGTORAD);

		if (Doc()->GetReplayState() == 1)
		{
			Doc()->SetReplayState(0);
			g_input->SetFakeInput("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
				false);
		}
		else if (Doc()->GetReplayState() == 7)
		{
			g_input->SetFakeInput("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
				false);
		}
		break;
	case 0x10:
		g_audio->StopSfx("\xc5\xb8\xc0\xd3");

		CMouseCursor::Instance()->SetActive(false);
		CMouseCursor::Instance()->SetCamMode(false);

		SendMsg(this, "Score", 0x12f, 0, 0, 0, 0);
		if (g_pFresh)
			g_pFresh->GetManager()->CloseForm(false);

		SendMsg(this, "Camera", 0x19b, 0, 0, 0, 0);
		SendMsg(this, "Letter", 0x18c, 0, 0, 0, 0);

		GOLFDOC()->m_shotPhase++;

		SendMsg(this, "Spin", 0x161, (int)&GolfBall().m_initCurve,
			(int)&GolfBall().m_initSpin, 0, 0);
		SendMsg(this, "Spot", 0xfa, (int)&m_shotSync, 0, 0, 0);

		GolfBall().m_initSpecial = 0;
		GOLFDOC()->m_commandList.clear();
		m_commandTime = 0;

		if (OnlinePlay())
		{
			CGolfDoc::sGaugeInfo info;
			info.phase = 1;
			info.pos = Bar().center;

			WSendPacket packet((enumClientPacket)0x14);
			packet.EncodeBuffer(&info, sizeof(info));
			packet.Send((eSendTo)0);
		}
		break;

	case 0x20:
		if (m_pChatView && CCapturedBg::Instance()->IsCaptured())
			m_pChatView->SetVisible(false);

		Bar().dir = 1;

		m_bTurnSetup = false;
		m_gaugeTime = 0;
		m_gaugeStartTime = 0;
		m_gaugePauseTime = 0;

		if (IsMyTurn(false))
		{
			timeBeginPeriod(1);
			CFx::Instance()->SetActive(false);
		}
		break;
	case 0x100:
		GOLFDOC()->GetUIAlpha() = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();
		break;

	case 0x400:
		m_debugCameraMat = g_camera;
		m_cameraPos = g_camera.pivot;
		m_cameraTarget = m_cameraPos + g_camera.za;
		SendMsg(this, "Camera", 0x19b, 0, 0, 0, 0);

		if (m_pOverBarSpecialCard)
			m_pOverBarSpecialCard->SetVisible(false);

		if (m_pMyInfoBtn)
			m_pMyInfoBtn->SetVisible(false);

		if (m_pOptionBtn)
			m_pOptionBtn->SetVisible(false);

		ToggleCommandBtns(false);

		if (m_pMessengerBtn)
			m_pMessengerBtn->SetVisible(false);

		if (m_pMessengerAlarmBtn)
		{
			m_pMessengerAlarmBtn->SetVisible(false);
			m_pMessengerAlarmBtn->SetTopmost(false);
		}

		if (m_pMessengerGiftAlarmBtn)
		{
			m_pMessengerGiftAlarmBtn->SetVisible(false);
			m_pMessengerGiftAlarmBtn->SetTopmost(false);
		}
		break;

	case 0x800:
		if (GOLFDOC()->IsNetGameEnd())
			break;

		if (CCameraWork::Instance()->GetCameraType() == 0x15)
			CCameraWork::Instance()->UninstallCamera();

		m_timer = 0.0f;

		if (PLAYER(GOLFDOC()->m_currentPlayer)->state == 3)
		{
			m_bSendFinishShot = true;
			break;
		}

		if (IsLocalContent((localContentType_t)0x30))
			AfxGetTask()->GetActor("Screen")
				<< MsgObject(NULL, 0xa0, 0, 0, 0, 0, 0);
		break;
		GetGameType();

	case 0x200:
		if (m_pOverBarSpecialCard)
			m_pOverBarSpecialCard->SetVisible(false);

		if (m_pMyInfoBtn)
			m_pMyInfoBtn->SetVisible(false);

		if (m_pOptionBtn)
			m_pOptionBtn->SetVisible(false);

		ToggleCommandBtns(false);

		if (m_pMessengerBtn)
			m_pMessengerBtn->SetVisible(false);

		if (m_pMessengerAlarmBtn)
			m_pMessengerAlarmBtn->SetVisible(false);

		if (m_pMessengerGiftAlarmBtn)
		{
			m_pMessengerGiftAlarmBtn->SetVisible(false);
			m_pMessengerGiftAlarmBtn->SetTopmost(false);
		}

		GOLFDOC()->GetUIAlpha() = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();

		g_view->SetFOV(1.04719758f);

		g_view->SetClip(3.2f, 3200.0f, true);

		g_camera = GOLFDOC()->MakeTopViewCameraMatrix(false);
		SendMsg(this, "Camera", 0xf1, 0, 0, 0, 0);
		CSceneManager::Instance()->UpdateFog(false);

		if (m_pKbdManualDlg)
			m_pKbdManualDlg->Close(true);
		break;
	case 0x40:
	{
		GetGameType();
		GetGameType();
		GetGameType();
		if (CCapturedBg::IsInstantiated())
			CCapturedBg::Instance()->Reset();

		if (GolfClub().GetType() != 3)
		{
			int timing = GetBarClickTiming();
			SendMsg(this, "Screen", 0x1ac, 0, 0, 0, 0);
			SendMsg(this, "Caddie", 0x1df, timing, 0, 0, 0);

			if (GolfClub().PowerShotType())
			{
				g_audio->PlaySfx("powershot_ball_effect");
				SendMsg(this, "Effect", 0x17e, 1, 0, 0, 0);
			}
			else
			{
				SendMsg(this, "Player", 0x10f, 0, 0, 0, 0);
			}
		}

		SendMsg(this, "Clock", 0x1ea, 0, 0, 0, 0);
		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);

		SendMsg(this, "ShotReview", -1, (int)"SAVE_SPOT_INDICATOR", 0, 0, 0);
		GetGameType();

		if (GolfClub().GetType() != 3 && Bar().grade == 4)
			Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].dwPangya++;

		PLAYER(GOLFDOC()->m_currentPlayer)->shotTime += GOLFDOC()->m_shotTime;

		if (IsLocalContent((localContentType_t)0x4f) &&
			Doc()->GetPlayingMode() == (eReplayModeType)2)
			GolfBall().m_initSpecial = m_shotData.special;

		float persent = Bar().Persent();
		float sq = sqrtf(persent);
		float power = GolfClub().GetPower() * sq;
		power *= GolfBall().GetPowerFactor();
		power *= GetBarClickPowerFactor();

		CGolfBall::sExpected* pData = pBall->PreDataToWrite(0);
		pData->event = 0;
		pData->pos = pBall->m_pos;
		pData->state = GetInitBallState();

		unsigned long charTid =
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tid;
		BallSpeedFactor factor(charTid, GolfBall(), m_shotSync);
		pData->vel = factor.vel;
		pBall->m_initCurve = factor.curve;
		pBall->m_initSpin = factor.spin;
		float pitch = factor.pitch;
		float angle = factor.angle;

		if (IsLocalContent((localContentType_t)0x4f) &&
			Doc()->GetPlayingMode() == (eReplayModeType)2)
		{
			pBall->m_initCurve = m_shotData.curve;
			angle = m_shotData.angle;
			pitch = m_shotData.deviation;
		}

		if (Bar().grade > 1)
		{
			pData->vel = (power * (Abs(pBall->m_initCurve) * 0.1f + 1.0f)) *
				RotMat(GolfClub().GetLie(), 0).za * RotMat(angle + pitch, 1);
		}
		GetGameType();

		switch (Doc()->GetPlayingMode())
		{
		case 1:
			if (!WNetworkSystem::Instance()->IsConnected(
					WNetworkSystem::NET_GAME) ||
				IsMyTurn(false))
			{
				memset(&m_shotData, 0, sizeof(m_shotData));
				m_shotData.hole = GOLFDOC()->m_currentHole;
				m_shotData.wind = Wind().GetGlobalWind();
				m_shotData.weather = GetWeather();
				m_shotData.stroke = PLAYER(GOLFDOC()->m_currentPlayer)
										->stroke[GOLFDOC()->m_currentHole - 1];
				m_shotData.angle = angle;
				m_shotData.pvsFrame = GetPVS().GetFrame();
				m_shotData.pos = pBall->m_pos;
				m_shotData.bar = Bar();
				m_shotData.curve = pBall->m_initCurve;
				m_shotData.spin = pBall->m_initSpin;
				m_shotData.special = pBall->m_initSpecial;
				m_shotData.clubPower = GolfClub().PowerShotType();
				m_shotData.bunker = PLAYER(GOLFDOC()->m_currentPlayer)->bunker;
				strcpy(m_shotData.club, GolfClub().GetName());
				m_shotData.item = PLAYER(GOLFDOC()->m_currentPlayer)->item;
				m_shotData.deviation = pitch;

				m_shotData.holeStroke = GetGameType() == 1
					? TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
						  ->stroke[GOLFDOC()->m_currentHole - 1]
					: PLAYER(GOLFDOC()->m_currentPlayer)
						  ->stroke[GOLFDOC()->m_currentHole - 1];

				m_shotData.pang =
					PLAYER(GOLFDOC()->m_currentPlayer)->totalPang -
					m_recordPang;
				m_shotData.bonusPang =
					PLAYER(GOLFDOC()->m_currentPlayer)->bonusPang -
					m_recordBonusPang;
				m_shotData.gauge = m_recordGauge;

				Doc()->m_shotDataList.push_back(m_shotData);
			}
			break;

		case 2:
			GolfBall().m_initCurve = m_shotData.curve;
			GolfBall().m_initSpin = m_shotData.spin;
			GolfBall().m_initSpecial = m_shotData.special;

			GetPVS().SetFrame(m_shotData.pvsFrame);
			break;
		}

		float flyTime;
		GetGameType();
		GetGameType();
		GetGameType();
		GetGameType();
		SendMsg(this, "Player", 0x126, (int)&flyTime, 0, 0, 0);

		if (!OnlinePlay() || IsMassGame())
		{
			SendMsg(this, "Player", 0x77, 0, 0, 0, 0);
			m_bShotResult = true;
			m_bCanSkip = pBall->m_groundType != 2;
		}
		else
		{
			if (IsLocalContent((localContentType_t)0x4f) &&
				Doc()->GetPlayingMode() == (eReplayModeType)2)
				SendMsg(this, "Player", 0x77, 0, 0, 0, 0);
		}

		m_shotPvsFrame = GetPVS().GetFrame();
		m_replayPvsFrame = m_shotPvsFrame + 50;
		m_flyPvsFrame = m_replayPvsFrame - (int)(flyTime * -50.0f);
		GetPVS().SetFrame(m_flyPvsFrame);

		pBall->m_vel = pData->vel;
		pBall->m_state = pData->state;

		pBall->m_special = pBall->m_initSpecial;

		m_bClubCameraEnd = true;
		m_bDrivePose = false;
		m_replayCamera = 0;
		pBall->m_water = CGolfBall::WATER_NONE;

		pBall->SetHoleIn(0);
		PLAYER(GOLFDOC()->m_currentPlayer)->unusedWord = 0xffff;
		PLAYER(GOLFDOC()->m_currentPlayer)->shotFlag = 0;
		if (GolfBall().m_state != CGolfBall::BALL_STOP)
			PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;

		GetPVS().ResetVariable();

		pBall->ClearBuffer();

		pBall->ResetVectorSlideCount();
		pBall->m_renderNum = 0;
		pBall->m_bounceFrame = -1;
		pBall->m_topFrame = -1;
		pBall->m_nearObjFrame = -1;
		pBall->m_holeFrame = -1;
		pBall->m_holeOutFrame = -1;
		pBall->m_greenFrame = -1;
		pBall->m_cupFrame = -1;
		pBall->m_afterBurnerFrame = -1;
		pBall->m_rollFrame = pBall->m_state == CGolfBall::BALL_ROLL ? 0 : -1;
		pBall->m_stayCount = 0;
		pBall->m_standGround = 0;
		pBall->m_prevColTriIndex = -1;
		pBall->m_prevColObjIndex = -1;
		pBall->m_colTriIndex = -1;
		pBall->m_colObjIndex = -1;
		pBall->m_meshIndex = -1;
		pBall->m_cobraFrame = -1;
		pBall->m_tomahawkFrame = -1;
		pBall->m_afterBurner.Reset();
		pBall->m_spikeFrame = -1;
		pBall->m_spikeDiveFrame = -1;
		pBall->m_spikeEndFrame = -1;
		pBall->m_vectorSlideFrame = -1;

		unsigned char groundType = pBall->m_groundType;

		if (Bar().grade == 1)
		{
			pBall->m_spin = 0.0f;
			pBall->m_curve = 0.0f;
		}
		else
		{
			if (IsLocalContent((localContentType_t)0x4f) &&
				Doc()->GetPlayingMode() == (eReplayModeType)2)
				GolfClub().SetApproachMode();

			if (GolfClub().GetApproachMode() == 4)
			{
				float spinAdd =
					Doc()->GetAuxPartProperty(GOLFDOC()->m_currentPlayer, 0) *
					0.5f;
				if (IsLocalContent((localContentType_t)0x70))
				{
					float buff = (float)Doc()->GetBuffValue(
						GOLFDOC()->m_currentPlayer, 0x10);
					if (GolfClub().PowerShotType() != 0)
						buff += (float)Doc()->GetBuffValue(
							GOLFDOC()->m_currentPlayer, 0x11);

					spinAdd += buff;
				}
				else if (IsLocalContent((localContentType_t)0x51))
				{
					CCardManager::Instance()->SetPlayerIndex(
						GOLFDOC()->m_currentPlayer);
					float buff = CCardManager::Instance()->GetCardRangeUp();
					if (GolfClub().PowerShotType() != 0)
						buff += CCardManager::Instance()->GetCardPowerRangeUp();

					spinAdd += buff;
				}

				pBall->m_spin =
					(Max(0.0f,
						 ((float)PLAYER(GOLFDOC()->m_currentPlayer)->power +
							 spinAdd) *
							 0.00588235306f) +
						1.5f) *
					persent * persent;
			}
			else
			{
				pBall->m_spin = 0.0f;
			}

			pBall->m_curve = persent * pBall->m_initCurve;
		}

		while (pBall->m_state != CGolfBall::BALL_STOP)
		{
			CGolfBall::sExpected* pExp = pBall->NewPreData();
			pExp->sound[0] = 0;
			pExp->event = 0;
			pExp->effect = 0;

			GetPVS().PreProcess(0.02f, true);
			GetPVS().BallProcess(0.02f);

			if (GolfClub().GetApproachMode() == 4)
			{
				if (pBall->m_special & 0x20)
				{
					if (pBall->m_cobraFrame < 0)
						pBall->m_pos.y = Max(pBall->m_pos.y,
							GetPVS().GetBasePosY(pBall->m_pos,
								pBall->GetRadius() * 2.0f));
					else
						pBall->m_spin -= 0.02f;
				}
				else if (pBall->m_special & 0x40)
				{
					if (pBall->m_spikeFrame < 0)
						pBall->m_pos.y = Max(pBall->m_pos.y,
							GetPVS().GetBasePosY(pBall->m_pos,
								pBall->GetRadius() * 2.0f));
					else
						pBall->m_spin -= 0.02f;
				}
				else
				{
					if (pBall->m_bounceFrame < 0 &&
						GetPVS().GetObjColNum() == 0)
						pBall->m_spin -=
							(0.5f - pBall->m_initSpin * 0.1f) * 0.02f;
					else
						pBall->m_spin -= 0.002f;
				}
			}

			if (pBall->m_renderNum >= 3000 ||
				(pBall->m_renderNum >= 1500 &&
					(pBall->m_vel.Magnitude() < 0.1f ||
						GetPVS().GetObjColNum() >= 10 ||
						GetPVS().GetMeshIndex(pBall->m_pos, false) == -1)))
			{
				pBall->m_vel.Reset();
				pBall->m_pos =
					GetPVS().GetGroundPoint(pBall->m_pos, false, NULL, false);
				pBall->m_pos.y += pBall->GetRadius();

				if (IsMyTurn(false) && pBall->m_state == CGolfBall::BALL_HOLE &&
					Doc()->GetPlayingMode() != (eReplayModeType)2)
					CollectBallStopData();

				pBall->m_state = CGolfBall::BALL_STOP;
			}

			pExp->pos = pBall->m_pos;
			pExp->vel = pBall->m_vel;
			pExp->rot.x = pBall->m_rot.x;
			pExp->rot.y = pBall->m_rot.y;
			pExp->state = pBall->m_state;

			if (pBall->m_state == CGolfBall::BALL_STOP)
				pBall->m_special &= ~0x70;

			if (pBall->m_state == CGolfBall::BALL_BOUNCE &&
				pBall->m_vel.y < 0.0f && pBall->m_topFrame < 0)
				pBall->m_topFrame = pBall->m_renderNum;

			if (pBall->m_tomahawkFrame < 0 && (pBall->m_initSpecial & 0x10) &&
				!(pBall->m_special & 0x10))
				pBall->m_tomahawkFrame = pBall->m_renderNum;

			if (pBall->m_special & 0x20)
			{
				if (pBall->m_renderNum == 1)
				{
					pBall->m_vel.y = 0.0f;
					pBall->m_vel = power * pBall->m_vel.Normalize();
				}

				WVector diff = pBall->m_pos - pBall->PreData(0)->pos;
				diff.y = 0.0f;

				if (pBall->m_cobraFrame < 0 &&
					diff.Magnitude() >
						(GolfClub().GetRange() * sq - 100.0f) * 3.2f)
				{
					pBall->m_cobraFrame = pBall->m_renderNum;
					pBall->m_vel.Normalize();

					switch (GolfClub().GetID())
					{
					case 0:
						pBall->m_vel *= 74.0f;
						break;

					case 1:
						pBall->m_vel *= 76.0f;
						break;

					case 2:
						pBall->m_vel *= 80.0f;
						break;
					}

					pBall->m_vel *= sq;
					pBall->m_spin = 2.5f;
				}
			}

			if (pBall->m_spikeEndFrame < 0 && (pBall->m_initSpecial & 0x40) &&
				!(pBall->m_special & 0x40) && pBall->m_spikeDiveFrame > 0)
				pBall->m_spikeEndFrame = pBall->m_renderNum;

			if (pBall->m_special & 0x40)
			{
				if (pBall->m_renderNum == 1)
				{
					pBall->m_vel.y = 0.0f;
					pBall->m_vel = power * pBall->m_vel.Normalize();
				}

				WVector diff = pBall->m_pos - pBall->PreData(0)->pos;
				diff.y = 0.0f;

				if (pBall->m_spikeFrame < 0 && diff.Magnitude() > 0.0f)
				{
					pBall->m_spikeFrame = pBall->m_renderNum;
					pBall->m_vel.Normalize();
					pBall->m_vel *= 72.5f;
					GetGameType();
					GetGameType();
					GetGameType();
					pBall->m_vel *= sq * 2.0f;
					pBall->m_spin = 3.1f;
				}

				if (pBall->m_topFrame >= 0 &&
					pBall->m_renderNum > pBall->m_topFrame + 60 &&
					pBall->m_spikeDiveFrame < 0)
				{
					pBall->m_spikeDiveFrame = pBall->m_renderNum;

					switch (GolfClub().GetID())
					{
					case 0:
						pBall->m_vel.y = -8.0f -
							(344.0f - min(GolfClub().GetPower() * sq, 344.0f)) *
								0.191964284f;
						break;

					case 1:
						pBall->m_vel.y = -10.2999992f -
							(306.0f - min(GolfClub().GetPower() * sq, 306.0f)) *
								0.195238099f;
						break;

					case 2:
						pBall->m_vel.y = -10.7999992f -
							(273.0f - min(GolfClub().GetPower() * sq, 273.0f)) *
								0.202000007f;
						break;
					}
					GetGameType();

					pBall->m_vel *= sq * 7.0f;
					pBall->m_spin = pBall->m_initSpin;
				}
			}

			if (pExp->event == 6 && pBall->m_vectorSlideFrame == -1)
				pBall->m_vectorSlideFrame = pBall->m_renderNum;
		}

		pBall->m_groundType = groundType;
		pBall->m_special = pBall->m_initSpecial;

		if (OnlinePlay())
		{
			if (IsMassGame() && Doc()->GetPlayingMode() == (eReplayModeType)1)
			{
				if (GolfClub().GetType() != 3 &&
					PLAYER(GOLFDOC()->m_currentPlayer)->oid ==
						Doc()->m_myInfo.info.dwGuid &&
					GolfBall().m_groundType != 2)
				{
					if (GolfBall().IsHoleIn())
					{
						SetFlagCutin(true);
					}
					else
					{
						WVector dist;
						dist = GetHoleCupPos() - GolfBall().m_pos;
						dist.y = 0.0f;
						GetGameType();
						GetGameType();
						if (dist.Magnitude() < 15.0f)
							SetFlagCutin(true);
					}
				}

				if (Doc()->GetPlayingMode() == (eReplayModeType)2)
					SetFlagCutin(false);

				int cutinMode = Doc()->m_gameMode;
				if (GOLFDOC()->m_tutorialMode < 15)
					cutinMode = 6;

				if (GetFlagCutin())
				{
					GetGameType();
					GetGameType();
					GetGameType();
					if (Doc()->m_cutinList.size() > 0)
					{
						std::list<sItemInfo>::iterator it;
						for (it = Doc()->m_cutinList.begin();
							it != Doc()->m_cutinList.end(); it++)
						{
							if (S5::ISCutinItem((*it).tid))
							{
								IFF_STRUCT::S5::sCutinInformation* pInfo =
									ItemManager()->FindCutinInformation(
										(*it).tid);
								if (pInfo && (pInfo->Condition & 8))
								{
									SendMsg(this, "Cutin", 0x78, 1, 0, 0, 0);
									S5::UTIL::SendCutInPacket(MyUID(), 8,
										cutinMode,
										Doc()
											->m_userInfo[GOLFDOC()
													->m_currentPlayer]
											.charInfo.tid);
									break;
								}
							}
							GetGameType();

							SetFlagCutin(false);
						}
					}
					else
					{
						SetFlagCutin(false);
					}
				}
			}
		}

		pBall->DbgTraceOutput(GolfClub().GetName(), GolfClub().GetRange());

		SendMsg(this, "NPC", 0x186, 0, 0, 0, 0);

		if (GetPVS().GetObjColNum() > 0)
			pBall->m_nearObjFrame = -1;

		GetPVS().SetFrame(m_shotPvsFrame - 1);
		GetPVS().PreProcess(0.02f, true);

		pBall->m_totalTime = pBall->m_renderNum * 0.02f;
		pBall->m_curve = pBall->m_initCurve;
		pBall->m_spin = pBall->m_initSpin;

		if (!pBall->m_bOB)
			pBall->m_bOB = GOLFDOC()->m_pPolySoup->IsOutOfBound(pBall->m_pos);

		if (Doc()->GetPlayingMode() == (eReplayModeType)1)
			AutoReplay();

		SendMsg(this, "Player", 0x115, (int)pBall->PreData(pBall->m_renderNum),
			0, 0, 0);

		m_backUp.pos = PLAYER(0)->pos;
		m_backUp.angle = angle;
		m_backUp.bOB = pBall->m_bOB;
		m_backUp.bunker = PLAYER(0)->bunker;
		m_backUp.gauge = PLAYER(0)->gauge;
		m_backUp.totalPang = PLAYER(0)->totalPang;
		m_backUp.bonusPang = PLAYER(0)->bonusPang;
		m_backUp.pang = PLAYER(0)->pang[GOLFDOC()->m_currentHole - 1];
		m_backUp.stroke = PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1];
		m_backUp.flag = PLAYER(0)->flag;
		strcpy(m_backUp.club, GolfClub().GetName());

		m_backUpList.push(m_backUp);

		if (pBall->m_bOB)
		{
			PLAYER(GOLFDOC()->m_currentPlayer)->pos = pBall->m_water
				? GetPVS().GetPenaltyPos()
				: pBall->PreData(0)->pos;
			PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;
		}
		else
		{
			if (GolfBall().IsHoleIn() || GolfBall().m_standGround > 0)
			{
				PLAYER(GOLFDOC()->m_currentPlayer)->pos =
					pBall->PreData(pBall->m_renderNum)->pos;

				unsigned char ground = GetPVS().GetGroundType(
					PLAYER(GOLFDOC()->m_currentPlayer)->pos, true, NULL);
				if (ground <= 7 || ground == 11)
					PLAYER(GOLFDOC()->m_currentPlayer)->startPos =
						PLAYER(GOLFDOC()->m_currentPlayer)->pos;
			}
			else
			{
				GOLFDOC()->m_pAutoTarget->CalcDropPos(
					pBall->PreData(pBall->m_renderNum)->pos);
				PLAYER(GOLFDOC()->m_currentPlayer)->pos =
					GOLFDOC()->m_pAutoTarget->GetDropPos();
				PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;
			}
		}

		if (!pBall->m_bOB &&
			GetPVS().IsOverWater(PLAYER(GOLFDOC()->m_currentPlayer)->pos))
		{
			pBall->m_bOB = true;
			pBall->m_water = CGolfBall::WATER_NONE;

			PLAYER(GOLFDOC()->m_currentPlayer)->pos = pBall->PreData(0)->pos;
			PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;
		}

		if (GetGameType() == 10)
			PLAYER(GOLFDOC()->m_currentPlayer)->pos =
				pBall->PreData(pBall->m_renderNum)->pos;

		if (GetGameType() == 1)
		{
			if (PLAYER(GOLFDOC()->m_currentPlayer)->teammate != 0xff)
			{
				PLAYER(PLAYER(GOLFDOC()->m_currentPlayer)->teammate)->pos =
					PLAYER(GOLFDOC()->m_currentPlayer)->pos;
				PLAYER(PLAYER(GOLFDOC()->m_currentPlayer)->teammate)->bunker =
					PLAYER(GOLFDOC()->m_currentPlayer)->bunker;
			}

			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->pos =
				PLAYER(GOLFDOC()->m_currentPlayer)->pos;
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->bunker =
				PLAYER(GOLFDOC()->m_currentPlayer)->bunker;
		}

		if (GOLFDOC()->m_tutorialMode == 15 || GOLFDOC()->m_tutorialMode == 8)
			SendMsg(this, "Screen", 0x1b4, 0, 0, 0, 0);

		m_bForgetDlg = false;
		m_forgetDlgTime = 0.0f;

		if (pBall->m_bOB && OnlinePlay() && IsMassGame() &&
			GetGameType() != 10 && CanCancelShot())
			m_bForgetDlg = true;

		if (!m_bForgetDlg)
			SendShotResult();

		pBall->m_pos = pData->pos;
		pBall->m_vel = pData->vel;
		pBall->m_meshIndex = GetPVS().GetMeshIndex(pBall->m_pos, false);
		GetGameType();

		if (GolfClub().GetType() == 3)
		{
			if (pBall->m_groundType == 2)
				PLAYER(GOLFDOC()->m_currentPlayer)
					->putt[GOLFDOC()->m_currentHole - 1] += 1;

			Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].dwPutt++;
		}
		else
		{
			Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].dwDrive++;
		}

		if (GolfClub().GetType() != 3)
		{
			int combo = 0;
			float auxProp =
				Doc()->GetAuxPartProperty(GOLFDOC()->m_currentPlayer, 4);

			switch (Bar().grade)
			{
			case 4:
			{
				bool bItem = false;
				GetGameType();
				if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x1800000a ||
					PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000007 ||
					PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000002 ||
					PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000012)
					bItem = true;

				if (IsLocalContent((localContentType_t)0x4b) &&
					auxProp == 11.0f)
				{
					bool bSpecial = false;
					if ((GolfBall().m_initSpecial & 3) ||
						(GolfBall().m_initSpecial & 0xc))
						bSpecial = true;

					combo = GetShotEffect(true, GolfClub().PowerShotType() != 0,
						bSpecial, bItem);
				}
				else if (GolfClub().PowerShotType() == 0 && !bItem)
				{
					if (GolfBall().m_initSpecial & 3)
						combo = (auxProp == 9.0f) + 14;
					else if (GolfBall().m_initSpecial & 0xc)
						combo = (auxProp == 9.0f) + 14;
					else
						combo = (auxProp == 9.0f) + 12;
				}

				if (GolfClub().PowerShotType() == 0 && !bItem)
				{
					if (IsLocalContent((localContentType_t)0x51))
					{
						CCardManager::Instance()->SetPlayerIndex(
							GOLFDOC()->m_currentPlayer);
						combo +=
							CCardManager::Instance()->GetCardComboGaugeUp();
					}

					if (!IsMassGame() && GetGameType() != 3)
					{
						unsigned char grade = 0;
						if (GetGameType() == 0 || GetGameType() == 7)
						{
							if (Doc()
									->m_userInfo[GOLFDOC()->m_currentPlayer]
									.stat.Level < 6)
								grade = 1;
							else
								grade =
									(Doc()->m_userInfo[GOLFDOC()
														   ->m_currentPlayer]
											.stat.Level -
										1) /
										5 +
									1;
						}
						else if (GetGameType() == 1)
						{
							if (Doc()
									->m_userInfo[GOLFDOC()->m_currentPlayer]
									.stat.Level < 6)
								grade = 1;
							else
								grade =
									(Doc()->m_userInfo[GOLFDOC()
														   ->m_currentPlayer]
											.stat.Level -
										1) /
										5 +
									1;

							if (PLAYER(GOLFDOC()->m_currentPlayer)->teammate !=
								0xff)
							{
								if (Doc()
										->m_userInfo[PLAYER(
											GOLFDOC()->m_currentPlayer)
												->teammate]
										.stat.Level < 6)
									grade += 1;
							}
							else
							{
								grade +=
									(Doc()->m_userInfo[GOLFDOC()
														   ->m_currentPlayer]
											.stat.Level -
										1) /
										5 +
									1;
							}

							grade /= 2;
						}

						if (grade < GOLFDOC()->m_maxGrade)
							combo += (GOLFDOC()->m_maxGrade - grade) * 2;
					}
				}
			}
			break;

			case 3:
			{
				bool bItem = false;
				GetGameType();
				if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x1800000a ||
					PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000007 ||
					PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000002 ||
					PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000012)
					bItem = true;

				if (IsLocalContent((localContentType_t)0x4b) &&
					auxProp == 11.0f)
					combo = GetShotEffect(false,
						GolfClub().PowerShotType() != 0, false, bItem);
				else if (GolfClub().PowerShotType() == 0)
					combo = (auxProp == 9.0f ? 3 : 0) + 4;
			}
			break;

			case 2:
				if (GolfClub().PowerShotType() == 0)
					combo = -4;
				break;

			case 0:
			case 1:
				if (GolfClub().PowerShotType() == 0)
					combo = -17;
				break;
			}
			GetGameType();
			GetGameType();
			GetGameType();

			if ((GetGameType() == 3 || GetGameType() == 7) &&
				PLAYER(GOLFDOC()->m_currentPlayer)
						->stroke[GOLFDOC()->m_currentHole - 1] >
					GOLFDOC()->GetHoleData().par &&
				combo > 0)
				combo = 0;

			if (GolfClub().PowerShotType() &&
				PLAYER(GOLFDOC()->m_currentPlayer)->item != 0x18000004 &&
				PLAYER(GOLFDOC()->m_currentPlayer)->item != 0x18000010 &&
				PLAYER(GOLFDOC()->m_currentPlayer)->item != 0x18000022 &&
				PLAYER(GOLFDOC()->m_currentPlayer)->item != 0x18000027)
			{
				if (IsLocalContent((localContentType_t)0x31))
				{
					if (GolfClub().PowerShotType() == 1)
						combo -= 33;
					else if (GolfClub().PowerShotType() == 2 ||
						GolfClub().PowerShotType() == 3)
						combo -= 66;
				}
				else
				{
					if (GolfClub().PowerShotType() == 1)
						combo -= 33;
					else if (GolfClub().PowerShotType() == 2)
						combo -= 66;
				}
			}

			if (Doc()->m_curChannel.Type & 8)
				combo = 0;

			if ((GOLFDOC()->m_tutorialMode == 15 ||
					GOLFDOC()->m_tutorialMode == 3) &&
				combo)
			{
				float gauge;
				GetGameType();
				if (combo > 0)
					gauge = Doc()->GetAuxPartProperty(
								GOLFDOC()->m_currentPlayer, 2) *
						combo;
				else
					gauge = (float)combo;

				if (GetGameType() == 1)
				{
					TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
						->gauge = Between(0.0f,
						TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->gauge +
							gauge,
						Doc()->GetComboGaugeLimit(
							PLAYER(GOLFDOC()->m_currentPlayer)->team));
				}
				else
				{
					PLAYER(GOLFDOC()->m_currentPlayer)->gauge = Between(0.0f,
						PLAYER(GOLFDOC()->m_currentPlayer)->gauge + gauge,
						Doc()->GetComboGaugeLimit(GOLFDOC()->m_currentPlayer));
				}
			}
		}

		m_nextPlayer = GetNextTurn();

		m_bSendFinishShot = false;
		m_bTurnSetup = true;
		m_bSpecialPuttPose = false;

		if (!(m_shotFlag & 2))
		{
			ReactionForShot();
			CCameraWork::Instance()->SetReplayCount(0);

			if (IsLocalContent((localContentType_t)0x31))
			{
				if (!((GolfClub().PowerShotType() == 2 ||
						  GolfClub().PowerShotType() == 3) &&
						GolfClub().GetType() == 0))
					CCameraWork::Instance()->SelectCamera(-1);
			}
			else
			{
				if (!(GolfClub().PowerShotType() == 2 &&
						GolfClub().GetType() == 0))
					CCameraWork::Instance()->SelectCamera(-1);
			}
		}

		CMouseCursor::Instance()->SetActive(true);

		if (GetGameType() == 10)
			SendMsg(this, "Screen", 0x1a5, 1, 0, 0, 0);
	}
	break;
	case 0x80:
	{
		m_pCameraArea = NULL;

		m_bRainbow = false;

		SendMsg(this, "Effect", 0x17f, 0, 0, 0, 0);

		if (m_replayCamera == 0)
		{
			SendMsg(this, "Screen", 0x1a8, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1a4, 0, 0, 0, 0);
			SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);
		}

		GOLFDOC()->m_commandList.clear();

		const CGolfBall::sExpected* pData = pBall->PreData(0);

		pBall->m_playFrame = 0;
		pBall->m_state = pData->state;
		pBall->m_rot.x = 0.0f;
		pBall->m_rot.y = CalcDeltaAngle(WVector::UNIT_POS_Z, pBall->m_vel);

		m_timer = 0.0f;
		GetPVS().SetFrame(m_flyPvsFrame);

		if (pBall->m_rollFrame > 0)
			SendMsg(this, "Ball", 0x1ec, 0, 0, 0, 0);

		if (!(m_shotFlag & 2) || m_replayCamera == 2)
		{
			SendMsg(this, "Screen", 0x1a5, 1, 0, 0, 0);

			if (!g_audio->IsPlaying("shot_dist_count"))
				g_audio->PlaySfx("shot_dist_count", NULL, 1, "shot_dist_count");
		}

		if (pBall->m_state != CGolfBall::BALL_BOUNCE)
		{
			if (pBall->m_state == CGolfBall::BALL_ROLL)
				SendMsg(this, "Trajectory", 0x15d, 0, 0, 0, 0);
			break;
		}

		float persent = Bar().Persent();

		unsigned char ground =
			GetPVS().GetGroundType(pBall->m_pos, false, NULL);
		if (GolfClub().GetType() != 3 && persent > 0.8f && ground != 1 &&
			ground != 6)
		{
			static int s_shotImpact = -1;
			if (!CFx::Instance()->FindControl(s_shotImpact))
				s_shotImpact = CFx::Instance()->MakeShotImpact(*pBall);
		}

		CFx::Instance()->MakeClubImpact(*pBall,
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid);

		WVector dist = pBall->PreData(pBall->m_renderNum)->pos - pData->pos;
		dist.y = 0.0f;

		m_flySound[0] = 0;

		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
				0x14000005 ||
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
				0x14000080)
		{
			if (dist.Magnitude() > 320.0f)
				strcpy(m_flySound, "wb_fly");
		}
		else
		{
			if (dist.Magnitude() > 640.0f)
				strcpy(m_flySound,
					"\xb0\xf8\xb3\xaf\xbe\xc6\xb0\xa1\xb1\xe2"
					"1");
			else if (dist.Magnitude() > 480.0f)
				strcpy(m_flySound,
					"\xb0\xf8\xb3\xaf\xbe\xc6\xb0\xa1\xb1\xe2"
					"2");
			else if (dist.Magnitude() > 320.0f)
				strcpy(m_flySound,
					"\xb0\xf8\xb3\xaf\xbe\xc6\xb0\xa1\xb1\xe2"
					"3");
		}

		if (m_flySound[0])
		{
			g_audio->StopSfx(m_flySound);
			g_audio->PlaySfx(m_flySound, NULL, 0, m_flySound);
		}

		unsigned char powerShot = GolfClub().PowerShotType();
		switch (GolfClub().GetType())
		{
		case 0:
		case 1:
			if (persent > 0.8f)
			{
				if (pBall->m_initSpecial & 0x70)
				{
					if (GolfClub().PowerShotType() == 2 ||
						GolfClub().PowerShotType() == 3)
					{
						CVoiceItem::Instance()->PlaySoundA(
							Doc()
								->m_userInfo[GOLFDOC()->m_currentPlayer]
								.clubInfo.tid,
							(CVoiceItem::eWaveState)8, -1);
					}
					else if (m_replayCamera == 2 || IsMassGame() ||
						GetCurMap() == 7)
					{
						CVoiceItem::Instance()->PlaySoundA(
							Doc()
								->m_userInfo[GOLFDOC()->m_currentPlayer]
								.clubInfo.tid,
							(CVoiceItem::eWaveState)4, -1);
					}

					if (pBall->m_initSpecial & 0x10)
					{
						bool bPower = powerShot == 2 || powerShot == 3;
						CFx::Instance()->MakeTomahawk(*pBall, bPower);
					}
					else if (pBall->m_initSpecial & 0x20)
					{
						CFx::Instance()->MakeCobra(*pBall);
						if (powerShot == 2 || powerShot == 3)
							AfxGetTask()->GetActor("Trajectory")
								<< MsgObject(NULL, 0x15e, 0, 0, 0, 0, 0);
					}
					else if (pBall->m_initSpecial & 0x40)
					{
						bool bPower = powerShot == 2 || powerShot == 3;
						CFx::Instance()->MakeSpike(*pBall, bPower);
					}
				}
				else
				{
					if (Bar().grade == 4)
					{
						if (GolfClub().PowerShotType())
						{
							SendMsg(this, "Trajectory", 0x158, 0, 0, 0, 0);
							SendMsg(this, "Effect", 0x181, 0, 0, 0, 0);
							g_audio->StopSfx("\xc7\xfd\xbc\xba");
							g_audio->PlaySfx(
								"\xb3\xaf\xbe\xc6\xb0\xa1\xb1\xe2_\xc7\xfd\xbc\xba",
								NULL, 0, "\xc7\xfd\xbc\xba");
						}
						else if (GOLFDOC()->m_tutorialMode > 9 &&
							GOLFDOC()->m_tutorialMode < 15)
						{
							SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
						}
						else
						{
							SendMsg(this, "Effect", 0x180, 0, 0, 0, 0);
							g_audio->StopSfx("\xba\xb0\xb0\xa1\xb7\xe7");
							g_audio->PlaySfx(
								"\xb3\xaf\xbe\xc6\xb0\xa1\xb1\xe2_\xba\xb0\xb0\xa1\xb7\xe7",
								NULL, 0, "\xba\xb0\xb0\xa1\xb7\xe7");
						}

						if (!(m_shotFlag & 2) || m_replayCamera == 2)
						{
							float scale = 2.0f;
							char logo[64] = "";
							sprintf(logo, "\xc6\xce\xbe\xdf%02d",
								Doc()->GetIndexPangyaLogoImage());

							SendMsg(this, "Letter", 0x18d, (int)logo,
								(int)&scale, 0, 0);
							SendMsg(this, "Gallery", 0xff, 0, 0, 0, 0);
							CSoundItemManager::Instance()
								->PlayCurrentSoundItem();
						}
					}
					else
					{
						if (Wind().GetGlobalWind().Magnitude() > 4.0f)
						{
							SendMsg(this, "Trajectory", 0x15b, 0, 0, 0, 0);
							g_audio->StopSfx("\xb9\xd9\xb6\xf7");
							g_audio->PlaySfx(
								"\xb3\xaf\xbe\xc6\xb0\xa1\xb1\xe2_\xb9\xd9\xb6\xf7",
								NULL, 0, "\xb9\xd9\xb6\xf7");
						}
						else
						{
							SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
						}
					}

					if (GolfClub().PowerShotType())
					{
						if (GolfClub().PowerShotType() == 1)
							SendMsg(this, "Effect", 0x16c, 0, 0, 0, 0);
						else if (powerShot == 2 || powerShot == 3)
							CFx::Instance()->MakePowerShot2(*pBall);

						CCameraWork::Instance()->Quake();
						g_audio->PlaySfx(
							"\xc6\xc4\xbf\xf6\xbc\xa6_\xc0\xcc\xc6\xe5\xc6\xae");
					}
				}

				if (Bar().grade > 2)
					SendMsg(this, "Effect", 0x16e, 0, 0, 0, 0);
			}
			else
			{
				if (persent > 0.2f)
					SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
			}

			SendMsg(this, "Ball", 0x1ef, 0, 0, 0, 0);
			break;

		case 2:
			if (persent > 0.8f)
			{
				if (pBall->m_initSpecial & 0x10)
				{
					bool bPower = powerShot == 2 || powerShot == 3;
					CFx::Instance()->MakeTomahawk(*pBall, bPower);
					break;
				}

				if (Bar().grade == 4)
				{
					if (!(m_shotFlag & 2) || m_replayCamera == 2)
					{
						float scale = 2.0f;
						char logo[64] = "";
						sprintf(logo, "\xc6\xce\xbe\xdf%02d",
							Doc()->GetIndexPangyaLogoImage());

						SendMsg(this, "Letter", 0x18d, (int)logo, (int)&scale,
							0, 0);
						SendMsg(this, "Gallery", 0xff, 0, 0, 0, 0);
						CSoundItemManager::Instance()->PlayCurrentSoundItem();
					}
				}
				else
				{
					SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
				}

				if (Bar().grade > 2)
					SendMsg(this, "Effect", 0x16e, 0, 0, 0, 0);
			}
			else
			{
				if (persent > 0.2f)
					SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
			}
			break;

		case 3:
			SendMsg(this, "Trajectory", 0x15d, 0, 0, 0, 0);
			break;
		}

		if ((pBall->m_initSpecial & 3) && pBall->m_afterBurnerFrame > 0)
			SendMsg(this, "Trajectory", 0x15a, 0, 0, 0, 0);
	}
	break;
	case 0x2000:
	{
		if (IsLocalContent((localContentType_t)0x4f))
			Doc()->SetReplayPlayType(0);

		ResetRank();

		m_bTurnSetup = false;
		m_bSkinsNextHole = false;

		SetConstantBGM("bgm_scoreboard.mp3", false, true);
		g_audio->StopSfx(NULL);

		if (!GOLFDOC()->IsNetGameEnd())
			DispatchMatchItems();

		SendMsg(this, "Score", 0x12f, 1, 0, 0, 0);

		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);

		if (!OnlinePlay() || IsMassGame())
			SetGameOver(GOLFDOC()->m_currentHole == GetEndHole());

		int count = 0;
		GetActor(s_gatewayActor[1]) << MsgObject(NULL, 0x26f, 6, 0, 0, 0, 0);
		GetActor(s_gatewayActor[1])
			<< MsgObject(NULL, 0x26f, 4, (int)&count, 0, 0, 0);
		if (count <= 0)
			GetActor(s_gatewayActor[1])
				<< MsgObject(NULL, 0x26f, 3, 0, 0, 0, 0);

		AfxGetTask()->GetActor("Screen")
			<< MsgObject(NULL, 0xa0, 0, 0, 0, 0, 0);
	}
	break;

	case 0x400000:
	{
		if (m_bWaitForStart)
			break;

		WVector pos = GetTeeShotPos();

		pos.y = GetPVS().GetGroundPoint(pos, false, NULL, false).y + 100.0f;

		WVector dir = GetHoleCupPos() - pos;
		dir.Normalize();

		g_camera = MakeCameraMatrix(pos, dir);
	}
	break;

	case 0x1000:
		if (IsLocalContent((localContentType_t)0x4f))
		{
			if (Doc()->GetPlayingMode() == (eReplayModeType)1)
			{
				Doc()->SetReplayPlayType(4);
			}
			else if (Doc()->GetPlayingMode() == (eReplayModeType)2 &&
				Doc()->GetReplayState() != 9 && Doc()->GetReplayState() != 7)
			{
				Doc()->m_replayShotIndex++;
				Doc()->SetReplayState(0);
			}
		}

		m_bShotResult = false;
		m_bBallStopped = false;
		m_bCanSkip = false;

		SendMsg(this, "Letter", 0x18c, 0, 0, 0, 0);
		if (!IsLocalContent((localContentType_t)0x4f) ||
			Doc()->GetPlayingMode() != (eReplayModeType)2)
			SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);

		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a8, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a5, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 0x18c, 0, 0, 0, 0);
		CFx::Instance()->ClearAll(true);

		if (GetGameType() == 1)
		{
			if (!SetHoleInPose_Team())
			{
				ChangeGameMode(0x800);
				return;
			}
		}
		else if (GetGameType() == 3)
		{
			if (!SetHoleInPose_Match())
			{
				ChangeGameMode(0x800);
				return;
			}
		}
		else if (GetGameType() == 7)
		{
			if (m_bSkinsNextHole)
			{
				SetHoleInPose_Skins();
			}
			else
			{
				ChangeGameMode(0x800);
				return;
			}
		}
		else
		{
			SetHoleInPose(false);
		}

		g_view->SetFOV(55.0f * g_DEGTORAD);
		break;

	case 0x4000:
	{
		if (IsLocalContent((localContentType_t)0x2f) &&
			PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
			SendMsg(this, "Player", 0x203, 0, 0, 0, 0);

		PLAYER(GOLFDOC()->m_currentPlayer)->shotTime += GOLFDOC()->m_shotTime;
		PLAYER(GOLFDOC()->m_currentPlayer)->item = 0;

		if (GetGameType() == 1)
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->gauge = Max(0.0f,
				TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->gauge + -30.0f);
		else
			PLAYER(GOLFDOC()->m_currentPlayer)->gauge =
				Max(0.0f, PLAYER(GOLFDOC()->m_currentPlayer)->gauge + -30.0f);

		g_audio->StopSfx("\xc5\xb8\xc0\xd3");
		g_audio->PlaySfx("\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6", NULL, 0,
			"\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6");

		PLAYER(GOLFDOC()->m_currentPlayer)->timeout++;
		Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].dwTimeOut++;

		float fade = 0.0f;
		GolfClub().SetPowerShot(0);
		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1aa, (int)&fade, 0, 0, 0);
		SendMsg(this, "Clock", 0x271a, 0, 0, 0, 0);
		SendMsg(this, "GuideLine", 0x1a2, 0, 0, 0, 0);

		if (IsMyTurn(true))
		{
			UIAlpha() = 0xff;
			CChatMsg::Instance()->ReplaceBackupUiAlpha();
		}

		m_nextPlayer = GetNextTurn();

		if (CheckDropOut() || CheckGiveUp())
		{
			RecordGiveUp(GOLFDOC()->m_currentPlayer);

			if (!CheckDropOut())
				RecordScoreByPlayer(GOLFDOC()->m_currentPlayer);

			if (GetGameType() == 1)
				ChangeGameMode(0x1000);
			else
				ChangeGameMode(0x80000);
		}
		else
		{
			if (!GOLFDOC()->m_bNoStat)
			{
				float scale = 1.5f;
				SendMsg(this, "Letter", 0x18d,
					(int)"\xc5\xb8\xc0\xd3\xbe\xc6\xbf\xf4", (int)&scale, 0, 0);
				SendMsg(this, "Screen", 0x1b9, 0, 0, 0, 0);
			}

			g_camera = GOLFDOC()->GetDefaultCamera();

			SendMsg(this, "Player", 0x2716,
				(int)"\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
				0, 0, 0);
			SendMsg(this, "Caddie", 0x2716, (int)"\xba\xa1\xc4\xbf", 0, 0, 0);
		}
	}
	break;

	case 0x20000:
	{
		CCapturedBg::Instance()->Reset();
		ResetRank();

		SendMsg(this, "Score", 0x12e, 1, 0, 0, 0);
		SendMsg(this, "GolfBg", 0x1e7, 0, 0, 0, 0);

		if (CSceneManager::IsInstantiated())
			SendMsg(this, "GolfBg", 0x142, 0, 0, 0, 0);

		WPolySoup::sCamera* pCamera = GOLFDOC()->m_pPolySoup->FindCamera(
			"\xbd\xc3\xbb\xf3\xbd\xc4\xc4\xab\xb8\xde\xb6\xf3");
		if (pCamera)
		{
			g_camera = pCamera->mat;
			g_view->SetFOV(pCamera->fov * g_DEGTORAD);
		}
		else
		{
			g_camera.Reset();
			g_view->SetFOV(55.0f * g_DEGTORAD);
		}

		SendMsg(this, "Score", 0x130, 0, 0, 0, 0);
		SendMsg(this, "Player", 0x119, 0, 0, 0, 0);

		SetConstantBGM("bgm_scoreboard.mp3", true, true);

		char camera[64];
		if (GetGameType() == 1)
		{
			if (!OnlinePlay())
			{
				if (TEAM(0)->score > TEAM(1)->score)
					GOLFDOC()->SetTeamGameWinner((eTeamName)0);
				else if (TEAM(0)->score < TEAM(1)->score)
					GOLFDOC()->SetTeamGameWinner((eTeamName)1);
				else if (TEAM(0)->totalPang > TEAM(1)->totalPang)
					GOLFDOC()->SetTeamGameWinner((eTeamName)0);
				else if (TEAM(0)->totalPang < TEAM(1)->totalPang)
					GOLFDOC()->SetTeamGameWinner((eTeamName)1);
			}

			strcpy(camera, "\xbd\xc3\xbb\xf3\xbd\xc4_\xc4\xbf\xc7\xc3");
		}
		else
		{
			strcpy(camera, "\xbd\xc3\xbb\xf3\xbd\xc4_\xb0\xb3\xc0\xce");
		}

		m_timer = CCameraWork::Instance()->LoadCamera((CCameraWork::eCamMode)1,
			camera, WVector::ZERO);

		if (OnlinePlay() && !IsMassGame() && !Doc()->m_bSentHoleStat &&
			!GalleryMode())
		{
			Doc()->m_bSentHoleStat = true;

			WSendPacket packet((enumClientPacket)6);
			Doc()->m_holeStatistics[MyIndex()].dwShotTime =
				PLAYER(MyIndex())->shotTime / 1000;
			packet.EncodeBuffer(&Doc()->m_holeStatistics[MyIndex()],
				sizeof(sPangYaUserStatistics));
			packet.Send((eSendTo)0);
		}
	}
	break;

	case 0x80000:
		g_view->SetFOV(55.0f * g_DEGTORAD);

		if (GetGameType() != 1)
		{
			if (GOLFDOC()->m_tutorialMode == 15 ||
				GOLFDOC()->m_tutorialMode == 8)
			{
				SendMsg(this, "Screen", 0x1b4, 1, 0, 0, 0);
				SendMsg(this, "Score", 0x132, 0, 0, 0, 0);
			}
		}

		SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a8, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a5, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 0x18c, 0, 0, 0, 0);
		CFx::Instance()->ClearAll(true);

		SetHoleInPose(false);

		m_nextPlayer = GetNextTurn();
		break;

	case 0x800000:
		if (IsLocalContent((localContentType_t)0x4f))
			Doc()->SetReplayPlayType(0);

		m_bTurnSetup = false;
		ResetRank();

		SetConstantBGM("bgm_scoreboard.mp3", false, true);

		g_audio->StopSfx(NULL);

		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Player", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Mascot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Bubble", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a4, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a5, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a8, 0, 0, 0, 0);

		if (IsLocalContent((localContentType_t)0x30))
			AfxGetTask()->GetActor("Screen")
				<< MsgObject(NULL, 0xa0, 0, 0, 0, 0, 0);
		break;
	}
}

static char* MakePoseName(const char* name)
{
	return MakeStr("%s_%02d", name, rand() % 4 + 1);
}

void CGolfRule::SetHoleInPose(bool bHoleIn)
{
	if (!bHoleIn)
	{
		WVector& pos = m_holeInPos;
		float angle = g_PI;
		WVector caddiePos = pos;

		SendMsg(this, "Player", 10009, (int)&pos, 0, 0, 0);
		SendMsg(this, "Player", 10008, (int)&angle, 0, 0, 0);
		SendMsg(this, "Player", 276, 0, 0, 0, 0);

		angle = g_PI * 1.75f;
		caddiePos = GetPVS().GetGroundPoint(
			WVector(pos.x + 10.0f, pos.y, pos.z + 10.0f), false, NULL, false);
		SendMsg(this, "Caddie", 473, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 482, (int)&angle, 0, 0, 0);
		SendMsg(this, "Caddie", 10009, (int)&caddiePos, 0, 0, 0);
	}

	char name[3][32];
	int diff;

	if (Doc()->m_golfGame.gameType != GAME_TYPE_TEAM)
		diff = (unsigned char)PLAYER(GOLFDOC()->m_currentPlayer)
				   ->stroke[GOLFDOC()->m_currentHole - 1] -
			GOLFDOC()->GetHoleData().par;
	else
		diff = (unsigned char)TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
				   ->stroke[GOLFDOC()->m_currentHole - 1] -
			GOLFDOC()->GetHoleData().par;

	int stroke = (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
		? TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
			  ->stroke[GOLFDOC()->m_currentHole - 1]
		: PLAYER(GOLFDOC()->m_currentPlayer)
			  ->stroke[GOLFDOC()->m_currentHole - 1];

	if (IsLocalContent(S4_RANDOM_CHAR_MOTION))
	{
		unsigned long charType =
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tid &
			0x3ffffff;
		strcpy(name[0], GetCharMotion(diff, charType, stroke));

		if (stroke == 1)
		{
			strcpy(name[1], "\xc8\xa6\xc0\xce\xbf\xf8");
			strcpy(name[2], "\xc8\xa6\xc0\xce\xbf\xf8");
		}
		else
		{
			switch (diff)
			{
			case -3:
				strcpy(name[1], "\xc8\xa6\xc0\xce\xbf\xf8");
				strcpy(name[2], "\xbe\xcb\xb9\xd9\xc6\xae\xb7\xce\xbd\xba");
				break;
			case -2:
				strcpy(name[1], "\xc8\xa6\xc0\xce");
				strcpy(name[2], "\xc0\xcc\xb1\xdb");
				break;
			case -1:
				strcpy(name[1], "\xc8\xa6\xc0\xce");
				strcpy(name[2], "\xb9\xf6\xb5\xf0");
				break;
			case 0:
				strcpy(name[1], "\xc8\xa6\xc0\xce");
				strcpy(name[2], "\xc6\xc4");
				break;
			case 1:
				strcpy(name[1], "\xbd\xc7\xb0\xdd");
				strcpy(name[2], "\xba\xb8\xb1\xe2");
				break;
			case 2:
				strcpy(name[1], "\xbd\xc7\xb0\xdd");
				strcpy(name[2], "\xb4\xf5\xba\xed\xba\xb8\xb1\xe2");
				break;
			case 3:
				strcpy(name[1], "\xbd\xc7\xb0\xdd");
				strcpy(name[2], "\xc6\xae\xb8\xae\xc7\xc3\xba\xb8\xb1\xe2");
				break;
			default:
				strcpy(name[1], "\xbd\xc7\xb0\xdd");
				if (GolfBall().IsHoleIn())
					sprintf(name[2], "+%d", diff);
				else
					strcpy(name[2], "\xc6\xf7\xb1\xe2");
				break;
			}
		}
	}
	else if (stroke == 1)
	{
		strcpy(name[0],
			"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
		strcpy(name[1], "\xc8\xa6\xc0\xce\xbf\xf8");
		strcpy(name[2], "\xc8\xa6\xc0\xce\xbf\xf8");
	}
	else
	{
		switch (diff)
		{
		case -3:
			strcpy(name[0],
				"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
			strcpy(name[1], "\xc8\xa6\xc0\xce\xbf\xf8");
			strcpy(name[2], "\xbe\xcb\xb9\xd9\xc6\xae\xb7\xce\xbd\xba");
			break;
		case -2:
			strcpy(name[0], "\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
			strcpy(name[1], "\xc8\xa6\xc0\xce");
			strcpy(name[2], "\xc0\xcc\xb1\xdb");
			break;
		case -1:
			strcpy(name[0], "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
			strcpy(name[1], "\xc8\xa6\xc0\xce");
			strcpy(name[2], "\xb9\xf6\xb5\xf0");
			break;
		case 0:
			strcpy(name[0],
				"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
			strcpy(name[1], "\xc8\xa6\xc0\xce");
			strcpy(name[2], "\xc6\xc4");
			break;
		case 1:
			strcpy(name[0],
				"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee");
			strcpy(name[1], "\xbd\xc7\xb0\xdd");
			strcpy(name[2], "\xba\xb8\xb1\xe2");
			break;
		case 2:
			strcpy(name[0],
				"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee");
			strcpy(name[1], "\xbd\xc7\xb0\xdd");
			strcpy(name[2], "\xb4\xf5\xba\xed\xba\xb8\xb1\xe2");
			break;
		case 3:
			strcpy(name[0],
				"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee");
			strcpy(name[1], "\xbd\xc7\xb0\xdd");
			strcpy(name[2], "\xc6\xae\xb8\xae\xc7\xc3\xba\xb8\xb1\xe2");
			break;
		default:
			strcpy(name[0],
				"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee");
			strcpy(name[1], "\xbd\xc7\xb0\xdd");
			if (GolfBall().IsHoleIn())
				sprintf(name[2], "+%d", diff);
			else
				strcpy(name[2], "\xc6\xf7\xb1\xe2");
			break;
		}
	}

	if (diff <= 0)
		SetConstantBGM("bgm_under_par.mp3", true, true);
	else
		SetConstantBGM("bgm_over_par.mp3", true, true);

	if (!bHoleIn)
	{
		SendMsg(this, "Player", 10006, (int)name[0], 0, 0, 0);
		SendMsg(this, "Player", 10007, (int)"", 0, 0, 0);
		SendMsg(this, "Caddie", 10006, (int)name[1], 0, 0, 0);

		char cam[64] = { 0 };
		bool bItem = false;
		char* motion = NULL;

		SendMsg(this, "Player", 10005, (int)&motion, 0, 0, 0);

		if (strstr(motion, "_item"))
			bItem = true;

		char* camName;
		if (!(Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tid &
				0x3ffffff))
			camName = MakeStr("%s_%02d", motion, rand() % 2 + 3);
		else
			camName = MakePoseName(motion);

		strncpy(cam, camName, 63);

		WVector& pos = m_holeInPos;
		m_timer = CCameraWork::Instance()->LoadCamera(
			CCameraWork::CAMMODE_HOLEIN, cam, pos);

		if (bItem == true)
			CCameraWork::Instance()->SetAbsAnim();

		if (m_timer < 0.0f)
		{
			WVector& p = m_holeInPos;
			WVector src, dest;
			src = WVector(pos.x, pos.y + 6.4f, pos.z - 32.0f);
			dest = WVector(p.x, p.y + 6.4f, p.z);
			g_camera = MakeCameraMatrix(src, dest);
			m_timer = 3.5f;
		}
	}

	SendMsg(this, "Letter", 397, (int)name[2], 0, 0, 0);
}

bool CGolfRule::SetHoleInPose_Match()
{
	if (Doc()->m_golfGame.gameType != GAME_TYPE_MATCH)
		return true;

	bool bHoleIn0 = PLAYER(0)->state == 1;
	bool bHoleIn1 = PLAYER(1)->state == 1;

	if (!bHoleIn0 && !bHoleIn1)
		return false;

	if (bHoleIn0 ^ bHoleIn1)
	{
		if (bHoleIn0)
		{
			if (PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] >
				PLAYER(1)->stroke[GOLFDOC()->m_currentHole - 1])
			{
				if (PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] !=
						PLAYER(1)->stroke[GOLFDOC()->m_currentHole - 1] +
							(unsigned char)1 ||
					PLAYER(0)->score <= PLAYER(1)->score + GetHoles() -
							GetHoleIndex(GOLFDOC()->m_currentHole))
					return false;
			}

			PLAYER(1)->stroke[GOLFDOC()->m_currentHole - 1] += 1;
			RecordScoreByPlayer(1);
		}
		else
		{
			if (PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] <
				PLAYER(1)->stroke[GOLFDOC()->m_currentHole - 1])
			{
				if (PLAYER(1)->stroke[GOLFDOC()->m_currentHole - 1] !=
						PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] +
							(unsigned char)1 ||
					PLAYER(1)->score <= PLAYER(0)->score + GetHoles() -
							GetHoleIndex(GOLFDOC()->m_currentHole))
					return false;
			}

			PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] += 1;
			RecordScoreByPlayer(0);
		}
	}

	bool bHoleIn[2];
	bHoleIn[0] = PLAYER(0)->state == 1;
	bHoleIn[1] = PLAYER(1)->state == 1;

	char name[3][32];
	int result;

	if (bHoleIn[0] && bHoleIn[1])
	{
		if (PLAYER(MyIndex())->stroke[GOLFDOC()->m_currentHole - 1] >
			PLAYER(1 - MyIndex())->stroke[GOLFDOC()->m_currentHole - 1])
		{
			result = -1;
		}
		else
		{
			if (PLAYER(MyIndex())->stroke[GOLFDOC()->m_currentHole - 1] ==
				PLAYER(1 - MyIndex())->stroke[GOLFDOC()->m_currentHole - 1])
				result = 0;
			else
				result = 1;
		}
	}
	else if (bHoleIn[MyIndex()])
		result = 1;
	else
		result = -1;

	if (result == -1)
	{
		strcpy(name[0],
			"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee");
		strcpy(name[1], "\xbd\xc7\xb0\xdd");
		strcpy(name[2], "lose");
	}
	else
	{
		if (PLAYER(MyIndex())->stroke[GOLFDOC()->m_currentHole - 1] ==
			(unsigned char)1)
		{
			strcpy(name[0],
				"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
			strcpy(name[1], "\xc8\xa6\xc0\xce\xbf\xf8");
		}
		else
		{
			int diff = (unsigned char)PLAYER(MyIndex())
						   ->stroke[GOLFDOC()->m_currentHole - 1] -
				GOLFDOC()->GetHoleData().par;

			switch (diff)
			{
			case -3:
				strcpy(name[0],
					"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
				strcpy(name[1], "\xc8\xa6\xc0\xce\xbf\xf8");
				break;
			case -2:
				strcpy(name[0],
					"\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
				strcpy(name[1], "\xc8\xa6\xc0\xce");
				break;
			case -1:
				strcpy(name[0],
					"\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
				strcpy(name[1], "\xc8\xa6\xc0\xce");
				break;
			default:
				strcpy(name[0],
					"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
				strcpy(name[1], "\xc8\xa6\xc0\xce");
				break;
			}
		}

		if (result == 0)
			strcpy(name[2], "draw");
		else
			strcpy(name[2], "win");
	}

	WVector& pos = m_holeInPos;
	SendMsg(this, "Player", 289, (int)&pos, (int)name[0], 0, 0);
	SendMsg(this, "Caddie", 289,
		(int)&GetPVS().GetGroundPoint(
			WVector(pos.x + 10.0f, pos.y, pos.z + 10.0f), false, NULL, false),
		(int)name[1], 0, 0);

	m_timer = CCameraWork::Instance()->LoadCamera(CCameraWork::CAMMODE_HOLEIN,
		name[0], pos);

	if (m_timer < 0.0f)
	{
		WVector& p = m_holeInPos;
		WVector src, dest;
		src = WVector(pos.x, pos.y + 6.4f, pos.z - 32.0f);
		dest = WVector(p.x, p.y + 6.4f, p.z);
		g_camera = MakeCameraMatrix(src, dest);
		m_timer = 3.5f;
	}

	SendMsg(this, "Letter", 397, (int)name[2], 0, 0, 0);
	SendMsg(this, "Screen", 439, 0, 0, 0, 0);

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME) &&
		(result == 0 || result == 1))
	{
		WSendPacket packet((enumClientPacket)0x52);
		packet.Encode1(PLAYER(MyIndex())->holeInPose);
		packet.Encode1(PLAYER(MyIndex())->holeInCollision);
		packet.Send(TO_GAME);
	}

	return true;
}

bool CGolfRule::SetHoleInPose_Team()
{
	if (GetGameType() != GAME_TYPE_TEAM)
		return true;

	unsigned char team = PLAYER(MyIndex())->team;

	bool bHoleIn0 = TEAM(0)->state == 1;
	bool bHoleIn1 = TEAM(1)->state == 1;

	if (!bHoleIn0 && !bHoleIn1)
		return false;

	if (bHoleIn0 ^ bHoleIn1)
	{
		if (bHoleIn0)
		{
			if (TEAM(0)->stroke[GOLFDOC()->m_currentHole - 1] >
				TEAM(1)->stroke[GOLFDOC()->m_currentHole - 1])
			{
				if (TEAM(0)->stroke[GOLFDOC()->m_currentHole - 1] !=
						TEAM(1)->stroke[GOLFDOC()->m_currentHole - 1] +
							(unsigned char)1 ||
					TEAM(0)->score <= TEAM(1)->score + GetHoles() -
							GetHoleIndex(GOLFDOC()->m_currentHole))
					return false;
			}

			TEAM(1)->stroke[GOLFDOC()->m_currentHole - 1] += 1;
			RecordScoreByTeam(1);
		}
		else
		{
			if (TEAM(0)->stroke[GOLFDOC()->m_currentHole - 1] <
				TEAM(1)->stroke[GOLFDOC()->m_currentHole - 1])
			{
				if (TEAM(1)->stroke[GOLFDOC()->m_currentHole - 1] !=
						TEAM(0)->stroke[GOLFDOC()->m_currentHole - 1] +
							(unsigned char)1 ||
					TEAM(1)->score <= TEAM(0)->score + GetHoles() -
							GetHoleIndex(GOLFDOC()->m_currentHole))
					return false;
			}

			TEAM(0)->stroke[GOLFDOC()->m_currentHole - 1] += 1;
			RecordScoreByTeam(0);
		}
	}

	SendMsg(this, "Caddie", 356, 0, 0, 0, 0);

	bool bHoleIn[2];
	bHoleIn[0] = TEAM(0)->state == 1;
	bHoleIn[1] = TEAM(1)->state == 1;

	char name[4][32];
	int result;

	if (bHoleIn[0] && bHoleIn[1])
	{
		if (TEAM(team)->stroke[GOLFDOC()->m_currentHole - 1] >
			TEAM(1 - team)->stroke[GOLFDOC()->m_currentHole - 1])
		{
			result = -1;
		}
		else
		{
			if (TEAM(team)->stroke[GOLFDOC()->m_currentHole - 1] ==
				TEAM(1 - team)->stroke[GOLFDOC()->m_currentHole - 1])
				result = 0;
			else
				result = 1;
		}
	}
	else if (bHoleIn[team])
		result = 1;
	else
		result = -1;

	if (result == -1)
	{
		strcpy(name[0],
			"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee");
		strcpy(name[1], "3\xb5\xee\xb8\xf0\xbc\xc7");
		strcpy(name[2],
			"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee");
		strcpy(name[3], "lose");
	}
	else
	{
		if (TEAM(team)->stroke[GOLFDOC()->m_currentHole - 1] ==
			(unsigned char)1)
		{
			strcpy(name[0],
				"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
		}
		else
		{
			int diff = (unsigned char)TEAM(team)
						   ->stroke[GOLFDOC()->m_currentHole - 1] -
				GOLFDOC()->GetHoleData().par;

			switch (diff)
			{
			case -3:
				strcpy(name[0],
					"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
				break;
			case -2:
				strcpy(name[0],
					"\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
				break;
			case -1:
				strcpy(name[0],
					"\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
				break;
			default:
				strcpy(name[0],
					"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
				break;
			}
		}

		if (result == 0)
			strcpy(name[1], "2\xb5\xee\xb8\xf0\xbc\xc7");
		else
			strcpy(name[1], "1\xb5\xee\xb8\xf0\xbc\xc7");

		strcpy(name[2], name[0]);

		if (result == 0)
			strcpy(name[3], "draw");
		else
			strcpy(name[3], "win");
	}

	int bOther = TEAM(team)->shooter != MyIndex();
	WVector& pos = m_holeInPos;

	if (!bOther)
		SendMsg(this, "Player", 288, (int)name[0], (int)name[1], (int)&pos, 0);
	else
		SendMsg(this, "Player", 288, (int)name[1], (int)name[0], (int)&pos, 0);

	m_timer = CCameraWork::Instance()->LoadCamera(CCameraWork::CAMMODE_HOLEIN,
		name[2], pos);

	if (m_timer < 0.0f)
	{
		WVector& p = m_holeInPos;
		WVector src, dest;
		src = WVector(pos.x, pos.y + 6.4f, pos.z - 32.0f);
		dest = WVector(p.x, p.y + 6.4f, p.z);
		g_camera = MakeCameraMatrix(src, dest);
		m_timer = 3.5f;
	}

	SendMsg(this, "Letter", 397, (int)name[3], 0, 0, 0);
	SendMsg(this, "Screen", 439, 0, 0, 0, 0);

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME) &&
		(result == 0 || result == 1))
	{
		WSendPacket packet((enumClientPacket)0x35);
		packet.Encode1(TEAM(PLAYER(MyIndex())->team)->holeInPose);
		packet.Encode1(TEAM(PLAYER(MyIndex())->team)->holeInCollision);
		packet.Send(TO_GAME);
	}

	return true;
}

__int64 CGolfRule::CalcWinnersPang(unsigned long oid)
{
	__int64 pang = 0;

	for (int i = 0; i < GetPlayerNum(); i++)
	{
		if (PLAYER(i)->state == 3)
			continue;

		if (PLAYER(i)->oid == oid)
			continue;

		pang +=
			min(GOLFDOC()->m_skinsHolePang, Doc()->m_userInfo[i].stat.i64Pang);
	}

	return GOLFDOC()->m_skinsForfeitPang + pang;
}

void CGolfRule::CalcSkinsHolePang()
{
	unsigned long winner = GOLFDOC()->m_skinsWinner;

	if (winner == 0xffffffff)
	{
		for (int i = 0; i < GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 3)
				continue;

			PLAYER(i)->skinsPang[GOLFDOC()->m_currentHole - 1] = -1;
			PLAYER(i)->skinsHolePang = 0;

			if (PLAYER(i)->state == 0)
				RecordScoreByPlayer(i);
		}

		if (GOLFDOC()->m_bNoStat)
			return;

		GOLFDOC()->m_skinsCarryNum++;

		GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole).carry = 1;

		if (GOLFDOC()->m_currentHole == Doc()->m_holeOrder[GetHoles() - 1])
			return;

		GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole).pang =
			GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole).basePang;

		__int64 pang = GOLFDOC()->m_skinsCarryPang +
			GetHolePang(GetHoleIndex(GOLFDOC()->m_currentHole + 1));
		unsigned long multiple =
			min((unsigned long)(GOLFDOC()->m_skinsMultiple * 2), 8);

		GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole + 1).carry = 1;
		GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole + 1).pang =
			pang * multiple;
	}
	else
	{
		__int64 winPang = CalcWinnersPang(winner);

		for (int i = 0; i < GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 3)
				continue;

			int hole = GOLFDOC()->m_bNoStat ? Doc()->m_holeOrder[GetHoles() - 1]
											: GOLFDOC()->m_currentHole;

			if (PLAYER(i)->oid != winner)
			{
				PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1] += 1;

				__int64 pang = min(GOLFDOC()->m_skinsHolePang,
					Doc()->m_userInfo[i].stat.i64Pang);
				PLAYER(i)->skinsHolePang = -(int)pang;
				PLAYER(i)->skinsPang[hole - 1] = -(int)pang;
				PLAYER(i)->skinsSumPang -= pang;
				Doc()->m_userInfo[i].stat.i64Pang -= pang;
			}
			else
			{
				PLAYER(i)->score += 1;
				PLAYER(i)->skinsHolePang = winPang;
				PLAYER(i)->skinsPang[hole - 1] = (int)winPang;
				PLAYER(i)->skinsSumPang += winPang;
				Doc()->m_userInfo[i].stat.i64Pang += winPang;
			}

			if (PLAYER(i)->state == 0)
				RecordScoreByPlayer(i);
		}

		GOLFDOC()->m_skinsCarryNum = 0;
		GOLFDOC()->m_skinsHolePang = 0;
		GOLFDOC()->m_skinsForfeitPang = 0;
	}
}

bool CGolfRule::SetHoleInPose_Skins()
{
	if (Doc()->m_golfGame.gameType != GAME_TYPE_SKINS)
		return true;

	char name[3][32];
	WVector& pos = m_holeInPos;
	int bLoop = 1;
	int result;
	unsigned long winner = GOLFDOC()->m_skinsWinner;

	if (winner == 0xffffffff)
		result = 0;
	else if (winner == PLAYER(MyIndex())->oid)
		result = 1;
	else
		result = -1;

	if (result == -1)
	{
		if (GOLFDOC()->m_skinsCarryNum < 3)
		{
			strcpy(name[0],
				"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee");
		}
		else
		{
			strcpy(name[0], "3\xb5\xee\xb8\xf0\xbc\xc7");
			bLoop = 0;
		}

		if (GOLFDOC()->m_skinsCarryNum < 3)
			strcpy(name[1], "\xbd\xc7\xb0\xdd");
		else
			strcpy(name[1], "\xba\xa1\xc4\xbf");

		strcpy(name[2],
			"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee");
	}
	else if (result == 1)
	{
		if (GOLFDOC()->m_skinsCarryNum < 2)
		{
			strcpy(name[0], "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
		}
		else if (GOLFDOC()->m_skinsCarryNum < 4)
		{
			strcpy(name[0], "1\xb5\xee\xb8\xf0\xbc\xc7");
			bLoop = 0;
		}
		else
		{
			strcpy(name[0], "\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
		}

		// HACK: bLoop test is dead, it only exists to prod the register allocator for the constant
		if (bLoop)
		{
			if (GOLFDOC()->m_skinsCarryNum < 3)
				strcpy(name[1], "\xc8\xa6\xc0\xce");
			else
				strcpy(name[1], "\xc8\xa6\xc0\xce\xbf\xf8");
		}
		else
		{
			if (GOLFDOC()->m_skinsCarryNum < 3)
				strcpy(name[1], "\xc8\xa6\xc0\xce");
			else
				strcpy(name[1], "\xc8\xa6\xc0\xce\xbf\xf8");
		}

		if (GOLFDOC()->m_skinsCarryNum < 3)
			strcpy(name[2], "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
		else
			strcpy(name[2], "\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
	}
	else
	{
		if (GOLFDOC()->m_skinsCarryNum < 3)
		{
			strcpy(name[0],
				"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
		}
		else
		{
			strcpy(name[0], "2\xb5\xee\xb8\xf0\xbc\xc7");
			bLoop = 0;
		}

		strcpy(name[1], "\xc8\xa6\xc0\xce");
		strcpy(name[2],
			"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee");
	}

	if (result == 1)
		SetConstantBGM("skins_win.mp3", true, true);
	else if (result == 0)
		SetConstantBGM("skins_draw.mp3", true, true);
	else
		SetConstantBGM("skins_lose.mp3", true, true);

	SendMsg(this, "Player", 290, (int)&pos, (int)name[0], bLoop, 0);
	SendMsg(this, "Caddie", 289,
		(int)&GetPVS().GetGroundPoint(
			WVector(pos.x + 10.0f, pos.y, pos.z + 10.0f), false, NULL, false),
		(int)name[1], 0, 0);

	m_timer = CCameraWork::Instance()->LoadCamera(CCameraWork::CAMMODE_HOLEIN,
		name[2], pos);

	if (m_timer < 0.0f)
	{
		WVector& p = m_holeInPos;
		WVector src, dest;
		src = WVector(pos.x, pos.y + 6.4f, pos.z - 32.0f);
		dest = WVector(p.x, p.y + 6.4f, p.z);
		g_camera = MakeCameraMatrix(src, dest);
		m_timer = 3.5f;
	}

	SendMsg(this, "Letter", 396, 0, 0, 0, 0);
	SendMsg(this, "Screen", 455, result, (int)CalcWinnersPang(MyGuid(false)), 0,
		0);
	SendMsg(this, "Screen", 456, 0, 0, 0, 0);
	SendMsg(this, "Screen", 439, 0, 0, 0, 0);

	return true;
}

void CGolfRule::DoFlyBall(float delta)
{
	if (m_bModal || m_bNetMsgWait)
		return;

	unsigned char powerShot = GolfClub().PowerShotType();

	if ((powerShot == 2 || powerShot == 3) && GolfClub().GetType() == 0 &&
		!m_bClubCameraEnd && CCameraWork::Instance()->Finished())
	{
		CCameraWork::Instance()->SelectCamera(-1);
		m_bClubCameraEnd = true;
	}

	if (!(m_shotFlag & 2) &&
		!g_input->Get("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true) &&
		m_replayCamera == 0)
	{
		CCameraWork::Instance()->Unlock();
	}

	WPolySoup* pPolySoup = GOLFDOC()->m_pPolySoup;
	CCameraWork* pCamera = CCameraWork::Instance();

	WPolySoup::sArea* pArea = pPolySoup->GetAreaByPos(GolfBall().m_pos);

	if (pArea && m_pCameraArea != pArea)
	{
		char camName[64];
		WSliceStr slice(pArea->name);
		slice.Get("*cam", "%s", camName);

		WPolySoup::sCamera* pCam = pPolySoup->GetCameraByName(camName);

		if (pCam)
		{
			pCamera->SetInstalledCamera(pCam->mat.pivot);
		}

		m_pCameraArea = pArea;
	}
	else
	{
		pArea = pPolySoup->GetAreaByPos(GolfBall().m_pos);

		if (!pArea)
		{
			m_pCameraArea = NULL;
			if (pCamera->GetCameraType() == 21)
				pCamera->UninstallCamera();
		}
	}

	GolfBall();

	if (GolfBall().m_state != CGolfBall::BALL_STOP)
	{
		DoFlyBall_Flying(delta);
	}
	else
	{
		DoFlyBall_Stopped(delta);
	}
}

void CGolfRule::DoFlyBall_Flying(float delta)
{
	CGolfBall& ball = GolfBall();
	float t;

	m_timer += delta;

	if (OnlinePlay())
	{
		if (ball.m_playFrame >
				(ball.m_renderNum - ball.m_afterBurnerFrame) * 0.8f &&
			m_bFlagCutin && GOLFDOC()->m_gameMode == 15)
		{
			PostMsg(this, "GolfRule", 0x24d, 1, (int)"S5::mode_flyBall", 0, 0);
			SendMsg(this, "Cutin", 0x78, 0x20, 0, 0, 0);
			m_bFlagCutin = false;
		}
	}

	if (m_timer >= 0.02f)
	{
		int frame = m_timer * 50.0f;
		if (ball.m_playFrame + frame > ball.m_renderNum)
			frame = ball.m_renderNum - ball.m_playFrame;

		bool bFirst = true;
		for (int i = 0; i < frame; i++)
		{
			GetTask()->ProcessNetSyncActor(true, 0.02f,
				ball.m_playFrame + i + 1);

			const CGolfBall::sExpected* pData =
				ball.PreData(ball.m_playFrame + i + 1);

			if (pData->event > 0)
				SendMsg(this, "ShotReview", -1, (int)"RECORD_REAL_BOUNCE_BALL",
					(int)pData, 0, 0);

			if (pData->event)
			{
				if (pData->event == 6)
				{
					SendMsg(this, "MagicCarpetHelper", 0x280, 0, 0, 0, 0);
					CFx::Instance()->MakeBoosterWarp(ball, 6);
				}
			}

			if (bFirst)
			{
				bFirst = false;

				if (pData->event)
				{
					ball.m_special &= ~0x70;

					switch (pData->effect)
					{
					case 1:
						SendMsg(this, "Effect", 0x173, 0, 0, 0, 0);
						break;

					case 2:
						if (pData->vel.Magnitude() > 30.0f)
							SendMsg(this, "Effect", 0x174,
								ball.m_playFrame + i + 1, 0, 0, 0);
						break;

					case 3:
						SendMsg(this, "Effect", 0x171, 0, 0, 0, 0);
						break;

					case 4:
						SendMsg(this, "Effect", 0x176, 0, 0, 0, 0);
						break;
					}

					if (pData->event == 2)
						SendMsg(this, "Gallery", 0x102, 0, 0, 0, 0);

					if (pData->event == 5)
					{
						CFx::Instance()->MakeBoosterWarp(ball, 0);

						if (Doc()
								->m_userInfo[GOLFDOC()->m_currentPlayer]
								.userEquip.tidBall == 0x14000005)
							SendMsg(this, "Ball", 0x1ee, 0, 0, 0, 0);
					}

					if (ball.m_playFrame + i + 1 == ball.m_greenFrame)
						SendMsg(this, "Gallery", 0x101, 0, 0, 0, 0);

					if (m_flySound[0])
						g_audio->StopSfx(m_flySound);

					if (pData->sound[0] && !g_audio->IsPlaying(pData->sound))
					{
						WVector pos;
						pos = ball.m_pos - g_camera.pivot;
						g_audio->PlaySfx(pData->sound, &pos, 5, pData->sound,
							NULL, 0.5f, 200.0f);
					}
				}
			}

			if (ball.m_playFrame + i + 1 == ball.m_nearObjFrame)
				SendMsg(this, "Gallery", 0x10d, 0, 0, 0, 0);

			if (!(ball.m_holeIn == 1) &&
				ball.m_playFrame + i + 1 == ball.m_holeOutFrame)
				SendMsg(this, "Gallery", 0x109, 0, 0, 0, 0);

			int slide =
				ball.m_vectorSlideFrame < 80 ? ball.m_vectorSlideFrame : 80;
			if (ball.m_vectorSlideFrame - slide == ball.m_playFrame + i + 1)
			{
				SendMsg(this, "MagicCarpetHelper", 0x27e, 0, 0, 0, 0);
			}

			if (ball.m_playFrame + i + 1 == ball.m_rollFrame)
			{
				SendMsg(this, "MagicCarpetHelper", 0x27f, 0, 0, 0, 0);
			}

			if (ball.m_vectorSlideFrame == ball.m_playFrame + i + 1)
			{
				SendMsg(this, "MagicCarpetHelper", 0x27d, 0, 0, 0, 0);
			}
		}

		ball.m_playFrame = Min(ball.m_playFrame + frame, ball.m_renderNum);
		m_timer -= frame * 0.02f;

		if (ball.m_state == CGolfBall::BALL_ROLL &&
			GetPVS().GetGroundType(ball.PreData(ball.m_playFrame)->pos, true,
				NULL) == 2 &&
			GetPVS().GetGroundType(
				ball.PreData(Min(ball.m_playFrame + frame, ball.m_renderNum))
					->pos,
				true, NULL) != 2)
			SendMsg(this, "Gallery", 0x10a, 0, 0, 0, 0);

		if (ball.m_afterBurnerFrame > 0 &&
			ball.m_playFrame > ball.m_afterBurnerFrame &&
			ball.m_playFrame < ball.m_afterBurnerFrame + 50)
		{
			g_audio->PlaySfx("spin_super", NULL, 0, NULL, NULL, 0.5f, 200.0f);

			static int s_powerSpin = -1;
			if (!CFx::Instance()->FindControl(s_powerSpin))
			{
				s_powerSpin = CFx::Instance()->MakePowerSpin(ball);
			}
		}

		if (ball.m_playFrame == ball.m_renderNum)
		{
			if (ball.m_water)
			{
				if (GetCurMap() == 13)
				{
					CFxSequence* pSeq =
						CFx::Instance()->OpenSequence("hell_ob.seq", true);

					if (pSeq)
						pSeq->Pos() = GolfBall().m_pos;
				}
				else
					SendMsg(this, "Effect", 0x176, 0, 0, 0, 0);
			}

			m_timer = 0.0f;
			ball.m_vel.x = ball.m_vel.y = ball.m_vel.z = 0.0f;
			ball.m_state = CGolfBall::BALL_STOP;

			g_audio->StopSfx("shot_dist_count");

			SendMsg(this, "Screen", 0x1ca, 0, 0, 0, 0);

			SendMsg(this, "ShotReview", -1, (int)"SAVE_BALL_TRACE",
				(int)&GolfBall(), 0, 0);
		}

		if ((m_shotFlag & 2) && ball.m_playFrame > 0.3f / 0.02f)
		{
			m_replayCamera++;

			if (m_replayCamera == 3)
			{
				m_shotFlag &= ~2;
				m_replayCamera = 0;
				ReactionForShot();

				if (g_input->Get("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
						true))
					g_camera = GOLFDOC()->GetDefaultCamera();

				CCameraWork::Instance()->SelectCamera(-1);
			}
			else
			{
				SwingReplay();
			}
		}
	}

	if (GolfClub().GetType() != 3 &&
		(Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall &
			0x3ffffff) == 0x20)
	{
		if (!m_bRainbow && ball.m_playFrame > ball.m_topFrame)
		{
			CFxSequence* pSeq =
				CFx::Instance()->OpenSequence("rainbow_002.seq", true);

			if (pSeq)
				pSeq->Pos() = ball.m_pos;

			m_bRainbow = true;
		}
	}

	if (ball.m_playFrame < ball.m_renderNum && GOLFDOC()->m_gameMode == 0x80)
	{
		t = m_timer > 0.0f ? (0.02f - m_timer) * 50.0f : 1.0f;

		ball.m_pos = ball.PreData(ball.m_playFrame)->pos * t +
			ball.PreData(ball.m_playFrame + 1)->pos * (1.0f - t);
		ball.m_vel = ball.PreData(ball.m_playFrame)->vel * t +
			ball.PreData(ball.m_playFrame + 1)->vel * (1.0f - t);
		ball.m_rot = t * ball.PreData(ball.m_playFrame)->rot +
			ball.PreData(ball.m_playFrame + 1)->rot * (1.0f - t);
	}
	else
	{
		t = 0.0f;
		ball.m_pos = ball.PreData(ball.m_playFrame)->pos;
		ball.m_vel = ball.PreData(ball.m_playFrame)->vel;
		ball.m_rot = ball.PreData(ball.m_playFrame)->rot;
	}

	if (GolfClub().GetType() != 3 && GetCurMap() == 10 && !m_bTrajectoryFx &&
		ball.m_playFrame >= ball.m_rollFrame)
	{
		m_bTrajectoryFx = true;
		SendMsg(this, "Trajectory", 0x15d, 0, 0, 0, 0);
	}

	if (!(m_shotFlag & 2) || m_replayCamera != 0)
		CCameraWork::Instance()->Process(delta, 1.0f - t);
}

bool CGolfRule::GetShotCntData()
{
	int cnt = 1;
	int size = Doc()->m_shotDataList.size();
	volatile bool bRet;
	if (Doc()->m_replayPlayType == 2)
	{
		bRet = false;
	}
	else
	{
		if (Doc()->m_replayHoleStroke)
		{
			if (Doc()->m_gameMode)
			{
				bRet = false;
			}
			else
			{
				if (Doc()->m_replayPlayType)
				{
					bRet = false;
				}
				else
				{
					bRet = false;
				}
			}
		}
		else
		{
			if (Doc()->m_gameMode)
			{
				bRet = false;
			}
			else
			{
				if (Doc()->m_replayPlayType)
				{
					bRet = false;
				}
				else
				{
					bRet = false;
				}
			}
		}
	}

	if (Doc()->GetReplayState() == 14)
	{
		Doc()->m_replayShotIndex = 0;
		return true;
	}

	if (Doc()->m_replayShotIndex <= size)
	{
		if (Doc()->m_replayShotIndex != size)
		{
			Doc()->m_replayShotIndex++;
		}
	}
	else
	{
		Doc()->m_replayShotIndex = size;
	}

	bRet = true;

	std::list<sShotData>::iterator it = Doc()->m_shotDataList.begin();
	for (; it != Doc()->m_shotDataList.end(); it++)
	{
		if (cnt == Doc()->m_replayShotIndex)
		{
			memcpy(&m_shotData, &(*it), sizeof(sShotData));
			Wind().SetWind(m_shotData.wind);
			Doc()->m_replayHoleStroke = m_shotData.holeStroke;
			PLAYER(0)->gauge = m_shotData.gauge;

			GetPVS().SetFrame(m_shotData.pvsFrame);

			if (Doc()->m_replayShotIndex == 0)
			{
				PLAYER(0)->bonusPang = 0;
				PLAYER(0)->totalPang = 0;
			}
			else
			{
				PLAYER(0)->bonusPang = m_shotData.bonusPang;
				PLAYER(0)->totalPang = m_shotData.pang;
			}
			break;
		}
		cnt++;
	}

	return bRet;
}

extern Fresh* g_pFresh;

void CGolfRule::DoFlyBall_Stopped(float delta)
{
	m_bTrajectoryFx = false;

	CGolfBall& ball = GolfBall();

	m_bBallStopped = true;

	if (IsLocalContent((localContentType_t)0x4f) &&
		Doc()->GetPlayingMode() == 2)
	{
		if (Doc()->m_replayPlayType == 2)
		{
			bool bRet = GetShotCntData();

			if (bRet != true)
				return;

			if (Doc()->GetReplayState() == 7)
			{
				if (ball.m_holeIn == 1)
					Doc()->SetReplayState(8);
			}
			else if (ball.m_holeIn == 1 && Doc()->GetReplayState() != 11)
				Doc()->SetReplayState(13);
			else
				Doc()->SetReplayState(10);
		}
		else if (Doc()->m_replayPlayType == 1)
		{
			if (Doc()->GetReplayState() == 7)
				Doc()->SetReplayState(6);

			if (Doc()->GetReplayState() == 11 || Doc()->GetReplayState() == 12)
				Doc()->SetReplayState(10);
		}

		WPuppet* pBall = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
		if (pBall && pBall->GetRootBone())
			pBall->SetAlpha(0xff);

		CCameraWork::Instance()->StopBallSound();
		SendMsg(this, "Score", 0x132, 0, 0, 0, 0);
		g_audio->StopSfx("shot_dist_count");

		if (Doc()->GetReplayState() == 8)
		{
			Doc()->SetReplayState(7);
			ChangeGameMode(0x1000);
		}
		else if (Doc()->GetReplayState() == 13)
			ChangeGameMode(0x1000);
		else
			ChangeGameMode(0x800);

		m_bShotResult = true;
	}

	int stroke = Doc()->m_golfGame.gameType == GAME_TYPE_TEAM
		? TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
			  ->stroke[GOLFDOC()->m_currentHole - 1]
		: PLAYER(GOLFDOC()->m_currentPlayer)
			  ->stroke[GOLFDOC()->m_currentHole - 1];

	bool bTutorial = GOLFDOC()->m_tutorialMode >= 8 ||
		GOLFDOC()->m_tutorialMode == 4 || GOLFDOC()->m_tutorialMode == 10;

	if (ball.m_holeIn == 1 && bTutorial &&
		Doc()->m_golfGame.gameType != GAME_TYPE_NEW_APPROACH)
	{
		if (m_bShotResult)
		{
			m_bShotResult = false;

			WPuppet* pBall = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
			if (pBall && pBall->GetRootBone())
				pBall->SetAlpha(0xff);

			CCameraWork::Instance()->StopBallSound();
			SendMsg(this, "Score", 0x132, 0, 0, 0, 0);
			g_audio->StopSfx("shot_dist_count");

			if (m_bSpecialPuttPose)
				SendMsg(this, "Player", 0x2716,
					(int)"\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8", 0, 0, 0);

			if (IsLocalContent((localContentType_t)0x55))
			{
				unsigned long tidBall = Doc()->m_myInfo.userEquip.tidBall;

				if (tidBall)
					CAztecHoleinEffect::Instance()->PlayEffect(tidBall);
			}

			if (m_shotFlag & 1)
			{
				m_replayCamera++;
				m_bShotResult = true;

				switch (m_replayCamera)
				{
				case 1:
					CCameraWork::Instance()->Lock();

					if (IsMassGame() &&
						GOLFDOC()->m_currentHole == GetEndHole() &&
						!m_bSendFinishShot)
					{
						m_bSendFinishShot = true;
						WSendPacket packet((enumClientPacket)0x1c);
						EncodePacket((enumClientPacket)0x1c, packet);
						packet.Send((eSendTo)0);
					}

				case 2:
					HoleReplay();
					return;

				case 3:
					m_shotFlag &= ~1;
					CCameraWork::Instance()->Unlock();
					break;
				}
			}
			else
			{
				if (IsMassGame() && GOLFDOC()->m_currentHole == GetEndHole() &&
					!m_bSendFinishShot)
				{
					m_bSendFinishShot = true;
					WSendPacket packet((enumClientPacket)0x1c);
					EncodePacket((enumClientPacket)0x1c, packet);
					packet.Send((eSendTo)0);
				}

				SendMsg(this, "Effect", 0x179, 0, 0, 0, 0);

				int diff = stroke - GOLFDOC()->GetHoleData().par;

				GetWinningPrize(diff);

				if (diff <= 0)
				{
					int coin = diff < 0 ? 13 : 6;
					if ((bool)((Doc()->m_serverProperty >> 7) & 1))
						coin = S5::UTIL::GetHoleinCoinCount(coin, 2);

					if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS &&
						GOLFDOC()->m_skinsState == 2)
					{
						m_coinNum = 30;
						m_coinTime = 0.1f;
						m_coinCount = GOLFDOC()->m_skinsCarryNum;
					}
					else
						SendMsg(this, "Effect", 0x17d, coin, 0, 0, 0);
				}

				unsigned long state = 0;
				if (stroke == 1)
					state = 0x400;
				else
				{
					switch (diff)
					{
					case -3:
						state = 0x800;
						break;
					case -2:
						state = 0x1000;
						break;
					case -1:
						state = 0x2000;
						break;
					case 0:
						state = 0x4000;
						break;
					case 1:
						state = 0x8000;
						break;
					case 2:
						state = 0x10000;
						break;
					case 3:
						state = 0x20000;
						break;
					case 4:
						state = 0x40000;
						break;
					}
				}

				if (Doc()
						->m_userInfo[GOLFDOC()->m_currentPlayer]
						.userEquip.tidBall == 0x14000004)
				{
					if (diff < 0)
						g_audio->PlaySfx(MakeStr("t_Ex7(\xb9\xf6\xb5\xf0)-%d",
											 rand() % 4 + 1),
							NULL, 0, NULL, NULL, 0.5f, 200.0f);
					else if (diff == 0)
						g_audio->PlaySfx(
							MakeStr("t_Ex7(\xc6\xc4)-%d", rand() % 4 + 1), NULL,
							0, NULL, NULL, 0.5f, 200.0f);
					else if (diff > 0)
						g_audio->PlaySfx(
							MakeStr("t_Ex7(\xbf\xc0\xb9\xf6\xc6\xc4)-%d",
								rand() % 4 + 1),
							NULL, 0, NULL, NULL, 0.5f, 200.0f);
				}
				else if (!CVoiceItem::Instance()->PlaySoundA(
							 Doc()
								 ->m_userInfo[GOLFDOC()->m_currentPlayer]
								 .clubInfo.tid,
							 (CVoiceItem::eWaveState)state, -1))
				{
					if (diff < 0)
						g_audio->PlaySfx("Birdie", NULL, 0, NULL, NULL, 0.5f,
							200.0f);
					else if (diff == 0)
						g_audio->PlaySfx("par", NULL, 0, NULL, NULL, 0.5f,
							200.0f);
					else if (diff > 0)
						g_audio->PlaySfx("Bogey", NULL, 0, NULL, NULL, 0.5f,
							200.0f);
				}

				if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
					Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
					SetHoleInPose(true);
			}
		}

		m_timer += delta;

		if (m_timer >
			(Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ? 3.0f : 2.0f))
		{
			bool bNext;
			if (GOLFDOC()->m_tutorialMode == 15)
				SendMsg(this, "Screen", 0x1b8, (int)&bNext, 0, 0, 0);
			else
				bNext = true;

			if (bNext ||
				m_timer > (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ? 7.0f
																		: 6.0f))
			{
				if (m_shotFlag & 4)
				{
					m_shotFlag &= ~4;
					Replay(false);
					return;
				}

				m_timer = 0.0f;
				UpdateShotResultStat();

				if (GOLFDOC()->m_tutorialMode == 4)
				{
					if (!m_bAutoShot)
					{
						SendMsg(this, "GolfRule", 0x88, 1, 0, 0, 0);
						IActor* pTutorial = GetActor("Tutorial");
						if (pTutorial)
						{
							pTutorial << MsgObject(NULL, 0x149, 0, 0, 0, 0, 0);
						}
					}
					else
						ChangeGameMode(0x800);
				}
				else
				{
					if (IsLocalContent((localContentType_t)0x4f) &&
						Doc()->GetPlayingMode() == 2)
					{
					}
					else
						RecordScoreByPlayer(GOLFDOC()->m_currentPlayer);
					ChangeGameMode(0x1000);
				}
			}
		}
	}
	else
	{
		if (m_bShotResult)
		{
			m_bShotResult = false;

			WPuppet* pBall = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
			if (pBall && pBall->GetRootBone())
				pBall->SetAlpha(0xff);

			g_audio->StopSfx("shot_dist_count");
			SendMsg(this, "NPC", 0x183, 0, 0, 0, 0);

			if (m_bSpecialPuttPose)
				SendMsg(this, "Player", 0x2716,
					(int)"\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0", 0, 0, 0);

			WVector pin = GetHoleCupPos();
			const CGolfBall::sExpected* pStart = ball.PreData(0);
			WVector dist(pStart->pos.x - pin.x, 0.0f, pStart->pos.z - pin.z);

			if (!PLAYER(GOLFDOC()->m_currentPlayer)->event.GetFlag(1) &&
				!(GolfBall().m_holeIn == 1) && !m_bSpecialPuttPose &&
				GetPVS().GetGroundType(ball.PreData(0)->pos, true, NULL) == 2 &&
				(GOLFDOC()->m_remainTime < 10000 || dist.Magnitude() < 32.0f ||
					PLAYER(GOLFDOC()->m_currentPlayer)
							->putt[GOLFDOC()->m_currentHole - 1] >=
						(unsigned char)2))
			{
				PLAYER(GOLFDOC()->m_currentPlayer)->event.Enable(1);
				SendMsg(this, "Player", 0x2717,
					(int)"\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
					0, 0, 0);
			}

			dist = GetHoleCupPos() - ball.m_pos;
			float len = sqrtf(dist.z * dist.z + dist.x * dist.x);

			switch (GetPVS().GetGroundType(ball.m_pos, true, NULL))
			{
			case 3:
			case 14:
				SendMsg(this, "Gallery", 0x106, 0, 0, 0, 0);
				CVoiceItem::Instance()->PlaySoundA(
					Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid,
					(CVoiceItem::eWaveState)0x80, -1);
				break;

			default:
				if (stroke <= 2 &&
					GetPVS().GetGroundType(ball.m_pos, true, NULL) == 2)
				{
					if (len > 22.4f)
						SendMsg(this, "Gallery", 0x100, 0, 0, 0, 0);
					else
						SendMsg(this, "Gallery", 0x10b, 0, 0, 0, 0);
				}
				break;
			}

			if (GolfClub().GetType() == 3)
			{
				if (len < 3.2f && ball.m_holeFrame < 0)
					SendMsg(this, "Gallery", 0x104, 0, 0, 0, 0);
			}

			if (ball.m_bOB)
			{
				if (!GOLFDOC()->m_bReplay)
				{
					PLAYER(GOLFDOC()->m_currentPlayer)->flag++;
					PLAYER(GOLFDOC()->m_currentPlayer)
						->stroke[GOLFDOC()->m_currentHole - 1] += 1;
					if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
						TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
							->stroke[GOLFDOC()->m_currentHole - 1] += 1;
				}

				if (GolfBall().m_water == CGolfBall::WATER_POOL ||
					GolfBall().m_water == CGolfBall::WATER_RIVER)
				{
					SendMsg(this, "Letter", 0x18d,
						(int)"\xbf\xf6\xc5\xcd\xc7\xd8\xc0\xfa\xb5\xe5", 0, 0,
						0);
					g_audio->PlaySfx("ball_fall_into_water", NULL, 0, NULL,
						NULL, 0.5f, 200.0f);
					CVoiceItem::Instance()->PlaySoundA(
						Doc()
							->m_userInfo[GOLFDOC()->m_currentPlayer]
							.clubInfo.tid,
						(CVoiceItem::eWaveState)0x100, -1);
				}
				else
				{
					SendMsg(this, "Letter", 0x18d, (int)"OB", 0, 0, 0);
					if (GolfBall().m_water == CGolfBall::WATER_SEA)
					{
						g_audio->PlaySfx("ball_fall_into_water", NULL, 0, NULL,
							NULL, 0.5f, 200.0f);
						CVoiceItem::Instance()->PlaySoundA(
							Doc()
								->m_userInfo[GOLFDOC()->m_currentPlayer]
								.clubInfo.tid,
							(CVoiceItem::eWaveState)0x100, -1);
					}
					else
					{
						g_audio->PlaySfx("ball_OB_area", NULL, 0, NULL, NULL,
							0.5f, 200.0f);
						CVoiceItem::Instance()->PlaySoundA(
							Doc()
								->m_userInfo[GOLFDOC()->m_currentPlayer]
								.clubInfo.tid,
							(CVoiceItem::eWaveState)0x10, -1);
					}
				}

				SendMsg(this, "Gallery", 0x105, 0, 0, 0, 0);
			}
			else if (!ball.m_standGround)
			{
				m_shotFlag &= ~4;

				if (!GOLFDOC()->m_bReplay)
				{
					PLAYER(GOLFDOC()->m_currentPlayer)->flag++;
					PLAYER(GOLFDOC()->m_currentPlayer)
						->stroke[GOLFDOC()->m_currentHole - 1] += 1;
					if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
						TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
							->stroke[GOLFDOC()->m_currentHole - 1] += 1;
				}

				g_audio->PlaySfx("ball_OB_area", NULL, 0, NULL, NULL, 0.5f,
					200.0f);
				CVoiceItem::Instance()->PlaySoundA(
					Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid,
					(CVoiceItem::eWaveState)0x10, -1);

				if (Doc()->m_golfGame.gameType != GAME_TYPE_NEW_APPROACH)
					SendMsg(this, "Letter", 0x18d, (int)"Retry", 0, 0, 0);
			}
			else
			{
				PLAYER(GOLFDOC()->m_currentPlayer)->flag = 0;
			}

			if (m_pCurrentRule && m_pCurrentRule->CheckGiveUp() &&
				(Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
					Doc()->m_golfGame.gameType == GAME_TYPE_MATCH))
				SetHoleInPose(true);

			if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
			{
				Doc()->m_rivalList[MyIndex()].approachDistance =
					Doc()->m_rivalList[MyIndex()].approachResultDistance;
				SendMsg(this, "Score", 0x2a, 1, 0, 0, 0);
			}
		}

		if (ball.m_water)
		{
			if (ball.m_pos.y - delta * 3.2f >
				GetPVS().GetGroundPoint(ball.m_pos, true, NULL, false).y +
					ball.GetRadius())
				ball.m_pos.y -= delta * 3.2f;
		}

		m_timer += delta;

		if ((ball.m_bOB && m_timer > 2.0f) || (!ball.m_bOB && m_timer > 3.5f))
		{
			bool bNext;
			if (GOLFDOC()->m_tutorialMode == 15)
				SendMsg(this, "Screen", 0x1b8, (int)&bNext, 0, 0, 0);
			else
				bNext = true;

			if (bNext || m_timer > (ball.m_bOB ? 6.0f : 7.5f))
			{
				if (m_shotFlag & 4)
				{
					m_shotFlag &= ~4;
					Replay(false);
					return;
				}

				if (m_bForgetDlg)
				{
					if (!m_pForgetDlg)
					{
						m_pForgetDlg = CreateForm<FrForm>(
							g_pFresh->GetManager(), this, "flowerofforgetting");
						m_pForgetDlg->SetMessage(
							"\xb4\xe7\xbd\xc5\xc0\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 \xb8\xc1\xb0\xa2\xc8\xad\xb4\xc2 \\c0xffff0000\\c\xb3\xaa\xbb\xdb \xb1\xe2\xbe\xef\xc0\xbb \xc1\xf6\xbf\xef \xbc\xf6 \\c0xff000000\\c\xc0\xd6\xb4\xc2 \xb4\xc9\xb7\xc2\xc0\xcc \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \\c0xffff0000\\c\xb8\xc1\xb0\xa2\xc8\xad\\c0xff000000\\c\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 \xc0\xdf\xb8\xf8\xb5\xc8 \xbc\xa6\xc0\xbb \xb5\xc7\xb5\xb9\xb8\xae\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
							false);
						m_pForgetDlg->Open(
							(FRESH_PFN_RESULT)&CGolfRule::OnForgetDlgResult, 3);
						m_forgetDlgTime = 0.0f;
						SendMsg(this, "Item", 0x196, 0, 0, 0, 0);
					}

					m_forgetDlgTime +=
						delta / CProjectG::Instance()->GetGameSpeed();

					if (m_forgetDlgTime > 10.0f && m_pForgetDlg)
						m_pForgetDlg->Close(FrCANCEL, true);
					return;
				}

				UpdateShotResultStat();
				m_bBallStopped = false;
				m_timer = 0.0f;

				if (m_pCurrentRule && m_pCurrentRule->CheckGiveUp())
				{
					SendMsg(this, "GuideLine", 0x1a2, 0, 0, 0, 0);
					RecordGiveUp(GOLFDOC()->m_currentPlayer);
					RecordScoreByPlayer(GOLFDOC()->m_currentPlayer);

					if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
						Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
						ChangeGameMode(0x1000);
					else
						ChangeGameMode(0x80000);
				}
				else
				{
					if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
						Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
						ChangeGameMode(0x1000);
					else
						ChangeGameMode(0x800);
				}
			}
		}
	}

	CCameraWork::Instance()->Process(delta, 0.0f);
}

extern "C" _CRTIMP int __cdecl mkdir(const char* dirname);
extern int g_last_screen_width;
extern int g_last_screen_height;
void SaveBMP(const char* filename, const Bitmap* bitmap);

void CGolfRule::CaptureProcess()
{
	static bool bHeadIcon = true;
	static bool bShowUI = true;
	static int fov = 55;
	static int index = 1;
	char filename[MAX_PATH];

	if (m_captureMode == 0)
	{
		if ((g_input->Get("LCONTROL", true) ||
				g_input->Get("RCONTROL", true)) &&
			g_input->GetDown("FUNC_10", true) && GOLFDOC()->m_gameMode & 0x400)
		{
			mkdir("capture");
			mkdir("capture/cinematic");

			m_captureMode = 2;
			m_captureStep = 1;

			index = 1;
			fov = (int)ceilf(g_view->GetFOV() * 57.29578f);
			while (
				_access(MakeStr("capture/cinematic/fov(%d)_%03d_frame(1).bmp",
							fov, index),
					0) != -1)
				index++;

			SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Player", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x190, 0, 0, 0, 0);
			SendMsg(this, "Effect", 0x17b, 0, 0, 0, 0);

			CShadowManager::Instance()->SetActive(false);
			Wind().SetActive(false);
			CSceneManager::Instance()->SetLOD(false, false);
			GetTask()->SetPause(true);
		}

		if (!g_input->Get("LCONTROL", true) &&
			!g_input->Get("RCONTROL", true) &&
			g_input->GetDown("FUNC_10", true))
		{
			if (COption::Instance()->vGetTnLMode() == 0)
			{
				MessageBoxA(NULL,
					"HW TnL \xb0\xa1\xbc\xd3 \xb8\xf0\xb5\xe5\xbf\xa1\xbc\xad\xb8\xb8 \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
					"\xc1\xd6\xc0\xc7", MB_OK);
				return;
			}

			mkdir("capture");
			mkdir("capture/high quality");

			m_captureMode = 1;
			m_captureStep = 0;

			index = 1;
			while (_access(MakeStr("capture/high quality/shot%03d.bmp", index),
					   0) != -1)
				index++;

			bShowUI = m_bShowUI;
			m_bShowUI = false;
			ToggleGameUI();

			SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);

			SendMsg(this, "ShotPreview", -1, (int)"ENABLE_DISPLAY", 0, 0, 0);

			if (CHeadIcon::IsInstantiated())
			{
				bHeadIcon = CHeadIcon::Instance()->GetVisible();
				CHeadIcon::Instance()->Hide();
			}

			Wind().SetActive(false);
			CSceneManager::Instance()->SetLOD(false, false);
			GetTask()->SetPause(true);
		}

		return;
	}

	Bitmap* image = new Bitmap(g_last_screen_width, g_last_screen_height, 24);

	switch (m_captureMode)
	{
	case 2:
	{
		bool bFinish = false;
		sprintf(filename, "capture/cinematic/fov(%d)_%03d_frame(%d).bmp", fov,
			index, m_captureStep);

		if (g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURE_SCREEN, 0,
				(int)image) == 1)
		{
			SaveBMP(filename, image);

			if (m_captureStep * fov >= 360)
				bFinish = true;
			else
			{
				g_camera *= RotMat(g_view->GetFOV(), 1);
				g_camera.pivot = m_cameraPos;
			}
		}
		else
		{
			MessageBoxA(NULL,
				MakeStr(
					"%s \xbd\xba\xc5\xa9\xb8\xb0 \xc4\xb8\xc3\xc4\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					strrchr(filename, '/')),
				"\xb0\xe6\xb0\xed", MB_OK);
			bFinish = true;
		}

		if (bFinish)
		{
			SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Player", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Caddie", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Ball", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x190, 1, 0, 0, 0);
			SendMsg(this, "Effect", 0x17a, 0, 0, 0, 0);

			CShadowManager::Instance()->SetActive(true);
			Wind().SetActive(true);
			CSceneManager::Instance()->SetLOD(true, false);
			GetTask()->SetPause(false);

			m_captureMode = 0;

			m_cameraTarget = m_cameraPos + g_camera.za;

			m_bShowUI = bShowUI;
			ToggleGameUI();

			SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);

			SendMsg(this, "ShotPreview", -1, (int)"ENABLE_DISPLAY", 1, 0, 0);

			if (CHeadIcon::IsInstantiated() && bHeadIcon)
				CHeadIcon::Instance()->Show();

			m_captureStep = -1;
			break;
		}

		m_captureStep++;
		break;
	}

	case 1:
	{
		bool bFinish = false;
		if (m_captureStep == 0)
			sprintf(filename, "capture/high quality/shot%03d_origin.bmp",
				index);
		else
			sprintf(filename, "capture/high quality/shot%03d_%03d.bmp", index,
				m_captureStep);

		if (g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURE_SCREEN, 0,
				(int)image) == 1)
		{
			SaveBMP(filename, image);

			if (m_captureStep >= m_captureScale * m_captureScale)
				bFinish = true;
		}
		else
		{
			MessageBoxA(NULL,
				MakeStr(
					"%s \xbd\xba\xc5\xa9\xb8\xb0 \xc4\xb8\xc3\xc4\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					strrchr(filename, '/')),
				"\xb0\xe6\xb0\xed", MB_OK);
			bFinish = true;
		}

		if (bFinish)
		{
			m_bShowUI = bShowUI;
			ToggleGameUI();

			SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);

			if (CHeadIcon::IsInstantiated() && bHeadIcon)
				CHeadIcon::Instance()->Show();

			Wind().SetActive(true);
			CSceneManager::Instance()->SetLOD(true, false);
			GetTask()->SetPause(false);
			g_view->xScaleProjMat(0, 1);

			m_captureMode = 0;
			if (m_captureStep >= m_captureScale * m_captureScale)
				IntergrateFile(MakeStr("capture/high quality/shot%03d", index));

			for (int i = 1; i <= m_captureScale * m_captureScale; i++)
				DeleteFileA(MakeStr("capture/high quality/shot%03d_%03d.bmp",
					index, i));

			m_captureStep = -1;
			break;
		}

		m_captureStep++;
		g_view->xScaleProjMat(m_captureStep - 1, m_captureScale);
		break;
	}
	}

	delete image;
}

#pragma pack(push, 1)
struct sBmpFileHeader
{
	char type[2];
	unsigned long size;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned long offBits;
};
#pragma pack(pop)

void CGolfRule::IntergrateFile(const char* filename)
{
	sBmpFileHeader bf;
	BITMAPINFOHEADER bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = 0;
	bi.biHeight = 0;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	bf.type[0] = 'B';
	bf.type[1] = 'M';
	bf.size = sizeof(sBmpFileHeader);
	bf.reserved1 = 0;
	bf.reserved2 = 0;
	bf.offBits = sizeof(sBmpFileHeader) + sizeof(BITMAPINFOHEADER);

	FILE* fp = fopen(MakeStr("%s.bmp", filename), "wb");
	if (!fp)
		return;

	Bitmap** bitmaps = new Bitmap*[m_captureScale];

	bi.biWidth = (LONG)(m_captureScale * g_view->GetWidth());
	bi.biHeight = (LONG)(m_captureScale * g_view->GetHeight());

	fwrite(&bf, 1, sizeof(sBmpFileHeader), fp);
	fwrite(&bi, 1, sizeof(BITMAPINFOHEADER), fp);

	for (int y = 0; y < m_captureScale; y++)
	{
		for (int x = 0; x < m_captureScale; x++)
		{
			bitmaps[x] = g_resrcmng->LoadBMP(MakeStr("%s_%03d.bmp", filename,
				(m_captureScale - y - 1) * m_captureScale + x + 1));
			if (!bitmaps[x])
			{
				delete[] bitmaps;
				fclose(fp);
				return;
			}
		}

		for (int row = bitmaps[0]->bi->bmiHeader.biHeight - 1; row >= 0; row--)
		{
			for (int i = 0; i < m_captureScale; i++)
			{
				for (int j = 0; j < bitmaps[i]->bi->bmiHeader.biWidth; j++)
				{
					BYTE* p =
						bitmaps[i]->vram + bitmaps[i]->pitch * row + j * 3;
					fputc(p[0], fp);
					fputc(p[1], fp);
					fputc(p[2], fp);
				}
			}
		}

		for (int i = 0; i < m_captureScale; i++)
		{
			if (bitmaps[i])
				delete bitmaps[i];
		}
	}

	delete[] bitmaps;
	fclose(fp);
}

void CGolfRule::DoDebugCamera(float delta)
{
	static float forward;
	static float side;
	static float up;

	if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true))
	{
		CFxSequence* pSequence =
			CFx::Instance()->OpenSequence("bullet.seq", true);
		if (pSequence)
		{
			pSequence->Pos() =
				g_camera.pivot + g_camera.za * 10.0f - g_camera.ya * 3.0f;
			pSequence->m_rot = RotMat(atanf(0.0093749994f), 0) * g_camera;
		}
	}

	if (g_input->GetDown("PARTICLE_2", true))
	{
		CFxSpray* pSpray = CFx::Instance()->OpenSpray("bullet.spr");
		if (pSpray)
		{
			pSpray->Pos() = g_camera.pivot + g_camera.za * 10.0f;
			pSpray->Vel() = RotVec(pSpray->Vel(), g_camera);
		}
	}

	if (g_input->GetDown("\xb5\xf0\xb9\xf6\xb1\xd7\xc5\xb0", true))
	{
		GOLFDOC()->m_gameMode = m_debugCameraMode;
		g_camera = m_debugCameraMat;
		SendMsg(this, "Camera", 0x19a, 0, 0, 0, 0);
		forward = side = up = 0.0f;

		if (m_pOverBarSpecialCard)
			m_pOverBarSpecialCard->SetVisible(true);

		if (m_pMyInfoBtn)
			m_pMyInfoBtn->SetVisible(true);

		if (m_pOptionBtn)
			m_pOptionBtn->SetVisible(true);

		ToggleCommandBtns(true);

		if (m_pMessengerBtn)
			m_pMessengerBtn->SetVisible(true);

		if (Doc()->m_bMessengerAlarm && m_pMessengerAlarmBtn)
		{
			m_pMessengerAlarmBtn->SetTopmost(true);
			m_pMessengerAlarmBtn->SetVisible(true);
		}

		if (Doc()->m_bRefreshCamera && m_pMessengerGiftAlarmBtn)
		{
			m_pMessengerGiftAlarmBtn->SetTopmost(true);
			m_pMessengerGiftAlarmBtn->SetVisible(true);
		}
		return;
	}

	WVector mouse;
	mouse = g_input->GetMouseDelta();

	if (!WisEqual(mouse, WVector::ZERO, g_EPSILON))
	{
		m_cameraTarget +=
			(g_camera.xa * mouse.x - g_camera.ya * mouse.y) * 0.01f;
		m_cameraTarget =
			(m_cameraTarget - m_cameraPos).Normalize() + m_cameraPos;
	}

	float dt;
	if (Wabs(delta) < g_EPSILON)
		dt = 0.03f;
	else
		dt = delta;

	float speed = dt * 300.0f;

	float scale;
	if (g_input->Get("LALT", true))
		scale = 0.05f;
	else
		scale = 1.0f;

	if (g_input->Get('W') && !g_input->Get('S'))
		forward += scale * speed;
	else if (g_input->Get('S') && !g_input->Get('W'))
		forward -= scale * speed;
	else
		forward = 0;

	if (g_input->Get('A') && !g_input->Get('D'))
		side -= scale * speed;
	else if (g_input->Get('D') && !g_input->Get('A'))
		side += scale * speed;
	else
		side = 0;

	if (g_input->Get('Z') && !g_input->Get('X'))
		up += scale * speed;
	else if (g_input->Get('X') && !g_input->Get('Z'))
		up -= scale * speed;
	else
		up = 0;

	if (m_debugCameraSpeed > 0)
	{
		forward = Between(-m_debugCameraSpeed, forward, m_debugCameraSpeed);
		side = Between(-m_debugCameraSpeed, side, m_debugCameraSpeed);
		up = Between(-m_debugCameraSpeed, up, m_debugCameraSpeed);
	}

	if (g_input->Get("ROTATE_CW", true))
	{
		if (g_input->GetDown("ROTATE_CW", true) &&
			!g_input->GetDown("ROTATE_CCW", true))
		{
			WVector dir(g_camera.za.x, 0, g_camera.za.z);
			m_debugCameraCenter =
				g_camera.pivot + m_debugCameraRadius * dir.Normalize();
		}

		m_cameraPos = (g_camera.pivot - m_debugCameraCenter) *
				RotMat(-(m_debugCameraSpeed / m_debugCameraRadius * dt), 1) +
			m_debugCameraCenter;

		WVector dir;
		dir = m_debugCameraCenter - m_cameraPos;
		dir.y = m_debugCameraRadius /
			sqrtf(1.0f - g_camera.za.y * g_camera.za.y) * g_camera.za.y;
		m_cameraTarget = m_cameraPos + dir.Normalize();
	}

	if (g_input->Get("ROTATE_CCW", true))
	{
		if (g_input->GetDown("ROTATE_CCW", true) &&
			!g_input->GetDown("ROTATE_CW", true))
		{
			WVector dir(g_camera.za.x, 0, g_camera.za.z);
			m_debugCameraCenter =
				g_camera.pivot + m_debugCameraRadius * dir.Normalize();
		}

		m_cameraPos = (g_camera.pivot - m_debugCameraCenter) *
				RotMat(m_debugCameraSpeed / m_debugCameraRadius * dt, 1) +
			m_debugCameraCenter;

		WVector dir;
		dir = m_debugCameraCenter - m_cameraPos;
		dir.y = m_debugCameraRadius /
			sqrtf(1.0f - g_camera.za.y * g_camera.za.y) * g_camera.za.y;
		m_cameraTarget = m_cameraPos + dir.Normalize();
	}

	if (!WisZero(forward))
	{
		m_cameraPos += g_camera.za * forward * dt;
		m_cameraTarget += g_camera.za * forward * dt;
	}

	if (!WisZero(side))
	{
		m_cameraPos += g_camera.xa * side * dt;
		m_cameraTarget += g_camera.xa * side * dt;
	}

	if (!WisZero(up))
	{
		m_cameraPos.y += up * dt;
		m_cameraTarget.y += up * dt;
	}

	g_camera = MakeCameraMatrix(m_cameraPos, m_cameraTarget);
}

bool IsMyTurn(bool check);

void CGolfRule::DoStand(float delta)
{
	if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].info.dwIdentity & 1)
	{
		CRival::Instance()->Shot();
		HitShot();
		CRival::Instance()->SetResult();
		return;
	}

	if (TopViewClick())
	{
		if (Doc()->m_bMessengerAlarm)
		{
			if (m_pMessengerAlarmBtn)
			{
				m_pMessengerAlarmBtn->SetVisible(false);
				m_pMessengerAlarmBtn->SetTopmost(false);
			}
		}

		if (Doc()->m_bRefreshCamera)
		{
			if (m_pMessengerGiftAlarmBtn)
			{
				m_pMessengerGiftAlarmBtn->SetVisible(false);
				m_pMessengerGiftAlarmBtn->SetTopmost(false);
			}
		}

		ChangeGameMode(0x200);
		return;
	}

	if (Doc()->GetPlayingMode() != 2)
	{
		if (!IsMyTurn(false))
			return;
	}

	bool bEnable = !CQuitWindow::Instance()->IsActive();
	if (GOLFDOC()->m_tutorialMode < 15)
		bEnable = m_bAutoShot || bEnable;

	if (!g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true) &&
		g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true) &&
		bEnable && !GOLFDOC()->IsPause() && !CChatMsg::Instance()->IsActive() &&
		GOLFDOC()->m_tutorialMode >= 2 && GolfClub().BarIs(2))
	{
		if (m_bShotBlock)
			return;

		_ImeReset();

		if (!m_bShotEnable)
		{
			g_audio->PlaySfx("play_unplayable");
			return;
		}

		int result;
		SendMsg(this, "Item", 0x199, (int)&result, 0, 0, 0);
		if (result == 3)
		{
			SendMsg(this, "Player", 0x199, (int)&result, 0, 0, 0);
			if (result != 3)
			{
				switch (Doc()->GetPlayingMode())
				{
				case 1:
					ChangeGameMode(0x10);
					break;
				case 2:
					GOLFDOC()->m_bar = m_shotData.bar;
					SetEnvironmentVariableA();
					GOLFDOC()->MakeDefaultCamera(true);
					HitShot();
					break;
				}
			}
		}
	}
}

void CGolfRule::DoCoursePreview(float delta)
{
	if (g_input->GetDown("NUM_9", true) || g_input->GetDown("PAD_9", true))
	{
		CSceneManager::Instance()->SetVisible(
			MakeStr("PLAYER_%d", GOLFDOC()->m_currentPlayer), true);
		CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", true);
		CSceneManager::Instance()->SetVisible("\xc4\xb3\xb5\xf0", true);
		CShadowManager::Instance()->Enable(
			COption::Instance()->vIsShadowEnabled());
		SendMsg(this, "Score", 0x13b, 0, 0, 0, 0);
		ChangeGameMode(8);
		return;
	}

	g_camera = RotMat(-0.34906584f, 0) *
		RotMat((m_timer + m_processTime) * 0.17453292f, 1);
	WVector tee = GOLFDOC()->GetHoleData().tee;
	WVector pin = GOLFDOC()->GetHoleData().pin;
	g_camera.pivot = m_cameraTarget -
		g_camera.za * (((pin - tee).Magnitude() * 0.5f) + 640.0f);
}

void CGolfRule::DoGetReadyToCaptureBG(float delta)
{
	if (CCapturedBg::Instance()->IsCaptured())
	{
		if (COption::Instance()->dGetClickSystem())
		{
			if ((g_input->GetButton(LEFT_BUTTON) == 2) ^
				g_input->GetUp("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9"))
			{
				CancelShot();
				return;
			}
		}
		ChangeGameMode(0x20);
	}
}

void CGolfRule::DoPowerGauge(float delta)
{
	if (m_bGaugePause)
	{
		m_gaugePauseTime = timeGetTime();
		IActor* pTutorial = GetActor("Tutorial");
		if (pTutorial)
			pTutorial << MsgObject(NULL, 0x156, 0, 0, 0, 0, 0);
		return;
	}

	if (m_bModal || m_bNetMsgWait)
		return;

	bool bClick;
	if (COption::Instance()->dGetClickSystem())
	{
		bClick = (GOLFDOC()->m_shotPhase == 1 &&
					 ((g_input->GetButton(LEFT_BUTTON) == 2) ^
						 g_input->GetUp(
							 "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9"))) ||
			(GOLFDOC()->m_shotPhase == 2 &&
				(g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
					 true) ^
					(g_input->GetButton(LEFT_BUTTON) == 1)));
	}
	else
	{
		bClick =
			g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true) ^
			(g_input->GetButton(LEFT_BUTTON) == 1);
	}

	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
		bClick = false;

	if (IsMyTurn(false))
	{
		float accuracy =
			GolfClub().GetType() == 3 ? 10.0f : GolfClub().GetNarrowAccuracy();
		if (GOLFDOC()->m_shotPhase == 1 && bClick &&
			GOLFDOC()->GetBar().gauge < s_barStart + accuracy)
		{
			CancelShot();
			return;
		}

		if (m_bAutoShot &&
			!g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true))
			bClick = false;

		if (bClick)
		{
			if (OnlinePlay())
			{
				CGolfDoc::sGaugeInfo info;
				info.phase = GOLFDOC()->m_shotPhase + 1;
				info.pos = GOLFDOC()->GetBar().gaugeStep;

				if (GolfClub().GetType() == 3 && info.phase == 2)
					info.phase = 3;

				WSendPacket packet((enumClientPacket)0x14);
				packet.EncodeBuffer(&info, sizeof(info));
				packet.Send((eSendTo)0);
			}
		}
		else if (m_bAutoShot &&
			((GOLFDOC()->m_shotPhase == 1 &&
				 GOLFDOC()->GetBar().gauge - m_autoShotPos[0] > 0) ||
				(GOLFDOC()->m_shotPhase == 2 &&
					GOLFDOC()->GetBar().gauge - m_autoShotPos[1] < 0)))
		{
			IActor* pTutorial = GetActor("Tutorial");
			if (pTutorial)
			{
				unsigned char index = GOLFDOC()->m_shotPhase - 1;
				pTutorial << MsgObject(NULL, 0x20f, 2,
					(int)&m_autoShotPos[index], GOLFDOC()->m_shotPhase, 0, 0);
				pTutorial << MsgObject(NULL, 0x148, 0, 0, 0, 0, 0);

				if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
						true) == 1)
					bClick = true;

				if (m_bModal)
					return;
			}
		}

		if (GOLFDOC()->m_tutorialMode > 12)
			CommandBox();
	}
	else
	{
		if (m_bRemoteClick)
		{
			m_bRemoteClick = false;
			bClick = true;
		}
	}

	if (m_gaugeTime == 0)
	{
		m_gaugeTime = timeGetTime();

		if (m_gaugeStartTime == 0)
			m_gaugeStartTime = m_gaugeTime;
	}

	unsigned long now;
	float elapsed;
	if (bClick)
	{
		unsigned long eventTime;
		if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true))
			eventTime = g_input->GetEventTime(
				"\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9");
		else
			eventTime = g_input->GetEventTime(LEFT_BUTTON);

		now = timeGetTime();

		elapsed = (now - m_gaugeTime) * 0.001f;
	}
	else
	{
		now = timeGetTime();

		elapsed = (now - m_gaugeTime) * 0.001f;
	}

	m_gaugeTime = now;

	if (!IsMyTurn(false))
		elapsed *= 0.85f;

	if (GolfClub().GetType() == 3)
		PuttingGauge(elapsed, bClick);
	else
		DriveGauge(elapsed, bClick);

	if (GOLFDOC()->GetBar().dir == 1)
		GOLFDOC()->GetBar().gaugeStep = floorf(GOLFDOC()->GetBar().gauge);
	else
		GOLFDOC()->GetBar().gaugeStep = ceilf(GOLFDOC()->GetBar().gauge);
}

void CGolfRule::DoApproachNextHole(float delta)
{
	if (!OnlinePlay())
		return;

	if (GOLFDOC()->GetApproachReloaded() == true)
		return;

	SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
	SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
	SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);

	if (GetHoleIndex(GOLFDOC()->m_currentHole) == GetHoles())
	{
		GOLFDOC()->m_currentHole = 1;
		GOLFDOC()->m_bNoStat = true;
		GOLFDOC()->m_pPolySoup->LoadMapCheckData(GOLFDOC()->m_currentHole);
	}
	else
	{
		GOLFDOC()->m_currentHole =
			Doc()->m_holeOrder[GetHoleIndex(GOLFDOC()->m_currentHole)];
		GOLFDOC()->m_bNoStat = true;
		GOLFDOC()->m_pPolySoup->LoadMapCheckData(GOLFDOC()->m_currentHole);
	}

	m_bModal = false;
	m_bNetMsgWait = false;
	SendMsg(this, "GolfBg", 0x140, 0, 0, 0, 0);

	m_timer = 0;
	Doc()->m_approachStartTime = 0;

	GolfBall().SetHoleIn(0);

	SendMsg(this, "Player", 0x163, 0, 0, 0, 0);
	SendMsg(this, "Mascot", 0x163, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 0x163, 0, 0, 0, 0);

	SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
	SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
	SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);

	SendMsg(this, "Screen", 0x1a8, 0, 0, 0, 0);
	SendMsg(this, "Screen", 0x1a4, 0, 0, 0, 0);
	SendMsg(this, "Screen", 0x1a5, 0, 0, 0, 0);

	GOLFDOC()->RefreshPlayerNum();
	SendHoleData();

	AfxGetTask()->GetActor("GolfRule") << MsgObject(NULL, 0x99, 0, 0, 0, 0, 0);

	GOLFDOC()->SetApproachReloaded(true);
}

void CGolfRule::CommandBox()
{
	if (GOLFDOC()->m_shotPhase < 2 ||
		GOLFDOC()->GetBar().gauge > s_powerBarWidth * 0.5f + s_barStart ||
		GOLFDOC()->GetBar().power < s_powerBarWidth * 0.8f + s_barStart ||
		(GOLFDOC()->m_commandList.size() > 0 &&
			GOLFDOC()->m_commandList.back() & 0x70))
		return;

	int count = 0;
	unsigned long command = 0;

	if (g_input->Get("\xbb\xf3", true))
	{
		count++;
		command |= 1;
	}

	if (g_input->Get("\xc7\xcf", true))
	{
		count++;
		command |= 2;
	}

	if (g_input->Get("\xc1\xc2", true))
	{
		count++;
		command |= 4;
	}

	if (g_input->Get("\xbf\xec", true))
	{
		count++;
		command |= 8;
	}

	if (count == 0)
	{
		m_commandTime = 0;
		return;
	}

	if (count == 1)
	{
		if (m_commandTime > 0)
		{
			if (GOLFDOC()->m_tutorialMode <= 12 ||
				GOLFDOC()->m_tutorialMode >= 15)
			{
				if (timeGetTime() - m_commandTime > 100)
				{
					command = 0x10;
					GOLFDOC()->m_commandList.push_back(command);
				}
			}
			else
			{
				if (timeGetTime() - m_commandTime > 0)
				{
					command = 0x10;
					GOLFDOC()->m_commandList.push_back(command);
				}
			}
		}
		else
		{
			m_commandTime = timeGetTime();
			GOLFDOC()->m_commandList.push_back(command);
		}
	}
	else
	{
		command = 0x40;
		GOLFDOC()->m_commandList.push_back(command);
	}
}

void CGolfRule::ToggleCommandBtns(bool bEnable)
{
	if (m_pCaddieBookBtn)
		m_pCaddieBookBtn->SetVisible(bEnable);

	if (m_pUnplayableBtn)
		m_pUnplayableBtn->SetVisible(bEnable);

	if (m_pPauseBtn)
		m_pPauseBtn->SetVisible(bEnable);

	if (m_pChatRecordBtn)
		m_pChatRecordBtn->SetVisible(bEnable);
}

bool CGolfRule::GetBarCondition()
{
	GOLFDOC()->GetBar().impact = GOLFDOC()->GetBar().gaugeStep;

	GOLFDOC()->GetBar().dir = 0;

	float diff = Abs(GOLFDOC()->GetBar().impact - s_barStart);
	float area = CPowerGauge::Instance()->GetPangYaArea();

	bool bHalf = false;
	if (IsLocalContent(S4_NEW_PANGYAZONE_CALC))
	{
		if ((int)(area * 2) % 2 == 1)
		{
			area -= 0.5;
			bHalf = true;
		}
	}

	if (Abs(GOLFDOC()->GetBar().impact - GOLFDOC()->GetBar().center) <
		area + 0.5f)
	{
		GOLFDOC()->GetBar().grade = 4;
		g_audio->PlaySfx(g_plSound[1]);
	}
	else if (IsLocalContent(S4_NEW_PANGYAZONE_CALC) && bHalf == true &&
		(area + 1.5f) * -1.0f <
			GOLFDOC()->GetBar().impact - GOLFDOC()->GetBar().center &&
		GOLFDOC()->GetBar().impact - GOLFDOC()->GetBar().center < 0)
	{
		GOLFDOC()->GetBar().grade = 4;
		g_audio->PlaySfx(g_plSound[1]);
	}
	else if (diff < GolfClub().GetNarrowAccuracy() + 0.5f)
	{
		GOLFDOC()->GetBar().grade = 3;
		g_audio->PlaySfx(g_plSound[2]);
	}
	else if (diff < GolfClub().GetWideAccuracy() + 0.5f)
	{
		GOLFDOC()->GetBar().grade = 2;
		g_audio->PlaySfx(g_plSound[3]);
	}
	else
	{
		GOLFDOC()->GetBar().grade = 1;
		g_audio->PlaySfx(g_plSound[4]);
	}

	if ((GOLFDOC()->GetBar().grade == 4 || GOLFDOC()->GetBar().grade == 3 ||
			GOLFDOC()->GetBar().grade == 2) == true)
		return true;

	return false;
}

void CGolfRule::CalcInputCommand()
{
	int i = 0;
	m_command = m_spinCommand = 0;
	if (GOLFDOC()->m_commandList.size() > 0 &&
		!(GOLFDOC()->m_commandList.back() & 0x40))
	{
		std::list<unsigned long>::iterator it;
		for (it = GOLFDOC()->m_commandList.begin();
			it != GOLFDOC()->m_commandList.end(); it++)
		{
			if (i < 2)
			{
				if (m_command > 0 && *it != 0x10)
					m_command <<= 8;
				m_command |= *it;
			}
			else if (i > 1)
			{
				if (m_spinCommand > 0 && *it != 0x10)
					m_spinCommand <<= 8;
				m_spinCommand |= *it;
			}
			i++;
		}
	}
}

void CGolfRule::SetSpecialShot()
{
	m_specialShot = 0;

	switch (m_command)
	{
	case 0x11:
		if (GOLFDOC()->GetBar().grade == 4)
			GolfBall().m_initSpecial = 1;
		break;

	case 0x12:
		if (GOLFDOC()->GetBar().grade == 4)
			GolfBall().m_initSpecial = 2;
		break;

	case 0x14:
		if (GOLFDOC()->GetBar().grade == 4)
			GolfBall().m_initSpecial = 4;
		break;

	case 0x18:
		if (GOLFDOC()->GetBar().grade == 4)
			GolfBall().m_initSpecial = 8;
		break;

	case 0x102:
	case 0x112:
	case 0x1102:
	case 0x1112:
		m_specialShot = 0x10;
		if (GOLFDOC()->GetBar().grade == 4)
		{
			if (m_spinCommand == 0x11)
				GolfBall().m_initSpecial = 0x11;
			else if (m_spinCommand == 0x12)
				GolfBall().m_initSpecial = 0x12;
			else if (m_spinCommand == 0x14)
				GolfBall().m_initSpecial = 0x14;
			else if (m_spinCommand == 0x18)
				GolfBall().m_initSpecial = 0x18;
			else
				GolfBall().m_initSpecial = 0x10;
		}
		else
			GolfBall().m_initSpecial = 0x10;
		break;

	case 0x801:
	case 0x811:
	case 0x1801:
	case 0x1811:
		m_specialShot = 0x20;
		if (GOLFDOC()->GetBar().grade == 4)
		{
			if (m_spinCommand == 0x11)
				GolfBall().m_initSpecial = 0x21;
			else if (m_spinCommand == 0x12)
				GolfBall().m_initSpecial = 0x22;
			else if (m_spinCommand == 0x14)
				GolfBall().m_initSpecial = 0x24;
			else if (m_spinCommand == 0x18)
				GolfBall().m_initSpecial = 0x28;
			else
				GolfBall().m_initSpecial = 0x20;
		}
		else
			GolfBall().m_initSpecial = 0x20;
		break;

	case 0x802:
	case 0x812:
	case 0x1802:
	case 0x1812:
		m_specialShot = 0x40;
		if (GOLFDOC()->GetBar().grade == 4)
		{
			if (m_spinCommand == 0x11)
				GolfBall().m_initSpecial = 0x41;
			else if (m_spinCommand == 0x12)
				GolfBall().m_initSpecial = 0x42;
			else if (m_spinCommand == 0x14)
				GolfBall().m_initSpecial = 0x44;
			else if (m_spinCommand == 0x18)
				GolfBall().m_initSpecial = 0x48;
			else
				GolfBall().m_initSpecial = 0x40;
		}
		else
			GolfBall().m_initSpecial = 0x40;
		break;
	}
}

void CGolfRule::CheckSpecialShotSpinAndCurve()
{
	if (GolfBall().m_initSpecial)
	{
		if (GolfBall().m_initSpecial & 0x70)
		{
			if (GolfClub().PowerShotType() == 0 ||
				((GolfBall().m_initSpecial & 0x20) && GolfClub().GetType()))
				GolfBall().m_initSpecial = 0;

			if (GolfClub().PowerShotType() == 0 ||
				((GolfBall().m_initSpecial & 0x40) && GolfClub().GetType()))
				GolfBall().m_initSpecial = 0;
		}

		float curve, spin;
		SendMsg(this, "Spin", 0x162, (int)&curve, (int)&spin, 0, 0);

		if (GolfBall().m_initSpecial & 1)
		{
			if (spin > 0.0f || WisZero(spin))
				GolfBall().m_initSpecial = m_specialShot;
		}
		else if (GolfBall().m_initSpecial & 2)
		{
			if (spin < 0.0f || WisZero(spin))
				GolfBall().m_initSpecial = m_specialShot;
		}
		else if (GolfBall().m_initSpecial & 4)
		{
			if (!WisEqual(curve, -1.0f, g_EPSILON))
				GolfBall().m_initSpecial = m_specialShot;
		}
		else if (GolfBall().m_initSpecial & 8)
		{
			if (!WisEqual(curve, 1.0f, g_EPSILON))
				GolfBall().m_initSpecial = m_specialShot;
		}

		if (GolfBall().m_initSpecial & 4)
			GolfBall().m_initCurve = -1.5f;
		else if (GolfBall().m_initSpecial & 8)
			GolfBall().m_initCurve = 1.5f;
	}

	GolfBall().m_initSpecial |= m_reservedSpecial;
	m_reservedSpecial = 0;
}

void CGolfRule::DriveGauge(float delta, bool bAuto)
{
	std::list<unsigned long>::iterator it;

	float barSpeed = GolfClub().GetBarSpeed();
	float speed =
		barSpeed - (int)PLAYER(GOLFDOC()->m_currentPlayer)->control * 10.0f;

	unsigned char ground =
		GetPVS().GetGroundType(GolfBall().m_pos, false, NULL);

	if (PLAYER(GOLFDOC()->m_currentPlayer)->item != 0x18000028)
	{
		if (ground == 3 || ground == 14)
		{
			switch (GolfClub().GetType())
			{
			case 0:
				if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
					speed *= 1.7f;
				else
					speed *= 1.5f;
				break;

			case 1:
				if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
					speed *= 1.5f;
				else
					speed *= 1.3f;
				break;

			case 2:
				if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
					speed *= 1.3f;
				else
					speed *= 1.2f;
				break;
			}
		}
		else if (ground == 7)
		{
			switch (GolfClub().GetType())
			{
			case 0:
				if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
					speed *= 1.5f;
				else
					speed *= 1.3f;
				break;

			case 1:
				if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
					speed *= 1.3f;
				else
					speed *= 1.2f;
				break;

			case 2:
				if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
					speed *= 1.1f;
				break;
			}
		}
	}

	if (GOLFDOC()->GetBar().dir == 1)
	{
		if (COption::Instance()->dGetClickSystem())
		{
			float persent = Min((GOLFDOC()->GetBar().gauge - s_barGaugeStart) *
					0.0025316456f,
				0.5f);
			speed *= persent * persent * 2.0f + 0.5f;
		}
	}
	else if (GOLFDOC()->m_shotPhase > 1)
	{
		if (GOLFDOC()->GetBar().bFullPower)
		{
			speed *= 0.9375f;
		}
		else
		{
			float persent = GOLFDOC()->GetBar().Persent();
			if (persent < 0.5f)
			{
				speed *= 0.875f;
			}
			else
			{
				persent -= 0.5f;
				speed = (persent * persent + 1.75f) * speed * 0.5f;
			}
		}
	}

	if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x1800000b ||
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000008 ||
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000003 ||
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x1800000f ||
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000010 ||
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000011 ||
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000012 ||
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x1800001f)
		speed *= 0.5f;

	if (m_bAutoShot && GOLFDOC()->m_tutorialMode != 4)
		speed *= 0.5f;

	if (bAuto)
	{
		switch (GOLFDOC()->m_shotPhase)
		{
		case 1:
			GOLFDOC()->m_shotPhase++;

			if (GOLFDOC()->GetBar().calipers > s_barStart)
			{
				GOLFDOC()->GetBar().gaugeStep = GOLFDOC()->GetBar().calipers;
				GOLFDOC()->GetBar().gauge = GOLFDOC()->GetBar().calipers;

				GOLFDOC()->GetBar().dir =
					WisEqual(GOLFDOC()->GetBar().calipers, s_barEnd, g_EPSILON)
					? 0
					: -1;
			}

			GOLFDOC()->GetBar().power = GOLFDOC()->GetBar().gaugeStep;

			if (GOLFDOC()->GetBar().dir == 0)
			{
				GOLFDOC()->GetBar().dir = -1;
				g_audio->PlaySfx(g_plSound[0]);
				GOLFDOC()->GetBar().bFullPower = true;
				SendMsg(this, "Screen", 0x1ab, 0, 0, 0, 0);
			}
			else
			{
				g_audio->PlaySfx(g_plSound[2]);
			}
			break;

		case 2:
			if (GOLFDOC()->GetBar().dir == -1 &&
				GOLFDOC()->GetBar().gauge <
					Min(GOLFDOC()->GetBar().power, 240.0f))
			{
				GOLFDOC()->m_shotPhase++;

				if (IsLocalContent(S4_SPECIALSHOT_RESTRICTION_MODIFY))
				{
					bool bCondition = GetBarCondition();
					if (bCondition == true)
					{
						CalcInputCommand();
						SetSpecialShot();
						CheckSpecialShotSpinAndCurve();
						HitShot();
					}
					else
						HitShot();
					return;
				}

				GOLFDOC()->GetBar().impact = GOLFDOC()->GetBar().gaugeStep;
				GOLFDOC()->GetBar().dir = 0;

				float diff = Abs(GOLFDOC()->GetBar().impact - s_barStart);
				float area = CPowerGauge::Instance()->GetPangYaArea();

				if (Abs(GOLFDOC()->GetBar().impact -
						GOLFDOC()->GetBar().center) < area + 0.5f)
				{
					unsigned long command = 0;
					if (GOLFDOC()->m_commandList.size() > 0 &&
						!(GOLFDOC()->m_commandList.back() & 0x40))
					{
						for (it = GOLFDOC()->m_commandList.begin();
							it != GOLFDOC()->m_commandList.end(); it++)
						{
							if (command > 0 && *it != 0x10)
								command <<= 8;
							command |= *it;
						}
					}

					switch (command)
					{
					case 0x11:
						GolfBall().m_initSpecial = 1;
						break;

					case 0x12:
						GolfBall().m_initSpecial = 2;
						break;

					case 0x14:
						GolfBall().m_initSpecial = 4;
						break;

					case 0x18:
						GolfBall().m_initSpecial = 8;
						break;

					case 0x102:
						GolfBall().m_initSpecial = 0x10;
						break;

					case 0x801:
						GolfBall().m_initSpecial = 0x20;
						break;

					case 0x10211:
						GolfBall().m_initSpecial = 0x11;
						break;

					case 0x10212:
						GolfBall().m_initSpecial = 0x12;
						break;

					case 0x10214:
						GolfBall().m_initSpecial = 0x14;
						break;

					case 0x10218:
						GolfBall().m_initSpecial = 0x18;
						break;

					case 0x80111:
						GolfBall().m_initSpecial = 0x21;
						break;

					case 0x80112:
						GolfBall().m_initSpecial = 0x22;
						break;

					case 0x80114:
						GolfBall().m_initSpecial = 0x24;
						break;

					case 0x80118:
						GolfBall().m_initSpecial = 0x28;
						break;

					case 0x802:
						GolfBall().m_initSpecial = 0x40;
						break;

					case 0x80211:
						GolfBall().m_initSpecial = 0x41;
						break;

					case 0x80212:
						GolfBall().m_initSpecial = 0x42;
						break;

					case 0x80214:
						GolfBall().m_initSpecial = 0x44;
						break;

					case 0x80218:
						GolfBall().m_initSpecial = 0x48;
						break;
					}

					if (GolfBall().m_initSpecial != 0)
					{
						if (GolfBall().m_initSpecial & 0x70)
						{
							if (GolfClub().PowerShotType() == 0 ||
								((GolfBall().m_initSpecial & 0x20) &&
									GolfClub().GetType()))
								GolfBall().m_initSpecial = 0;

							if (GolfClub().PowerShotType() == 0 ||
								((GolfBall().m_initSpecial & 0x40) &&
									GolfClub().GetType()))
								GolfBall().m_initSpecial = 0;
						}

						float curve, spin;
						SendMsg(this, "Spin", 0x162, (int)&curve, (int)&spin, 0,
							0);

						if (GolfBall().m_initSpecial & 1)
						{
							if (spin > 0.0f || WisZero(spin))
								GolfBall().m_initSpecial = 0;
						}
						else if (GolfBall().m_initSpecial & 2)
						{
							if (spin < 0.0f || WisZero(spin))
								GolfBall().m_initSpecial = 0;
						}
						else if (GolfBall().m_initSpecial & 4)
						{
							if (!WisEqual(curve, -1.0f, g_EPSILON))
								GolfBall().m_initSpecial = 0;
						}
						else if (GolfBall().m_initSpecial & 8)
						{
							if (!WisEqual(curve, 1.0f, g_EPSILON))
								GolfBall().m_initSpecial = 0;
						}

						if (GolfBall().m_initSpecial & 4)
							GolfBall().m_initCurve = -1.5f;
						else if (GolfBall().m_initSpecial & 8)
							GolfBall().m_initCurve = 1.5f;
					}

					GolfBall().m_initSpecial |= m_reservedSpecial;
					m_reservedSpecial = 0;

					GOLFDOC()->GetBar().grade = 4;
					g_audio->PlaySfx(g_plSound[1]);
				}
				else if (diff < GolfClub().GetNarrowAccuracy() + 0.5f)
				{
					GOLFDOC()->GetBar().grade = 3;
					g_audio->PlaySfx(g_plSound[2]);
				}
				else if (diff < GolfClub().GetWideAccuracy() + 0.5f)
				{
					GOLFDOC()->GetBar().grade = 2;
					g_audio->PlaySfx(g_plSound[3]);
				}
				else
				{
					GOLFDOC()->GetBar().grade = 1;
					g_audio->PlaySfx(g_plSound[4]);
				}

				GolfBall().m_initSpecial |= m_reservedSpecial;
				m_reservedSpecial = 0;

				HitShot();
			}
			return;
		}
	}

	switch (GOLFDOC()->GetBar().dir)
	{
	case -1:
		GOLFDOC()->GetBar().gauge -= speed * delta;

		if (GOLFDOC()->GetBar().gauge < s_barGaugeStart)
			CancelShot();
		break;

	case 1:
		GOLFDOC()->GetBar().gauge += speed * delta;

		if (GOLFDOC()->GetBar().gauge > s_barEnd)
		{
			GOLFDOC()->GetBar().gauge = s_barEnd;
			m_timer = 0.0f;

			GOLFDOC()->GetBar().dir = 0;
		}
		break;

	case 0:
		m_timer += delta;
		if (GOLFDOC()->m_shotPhase > 0 && GOLFDOC()->m_shotPhase < 3)
		{
			if (m_timer >
				(COption::Instance()->GetOption()->gPowerGauge ? 0.03f : 0.02f))
				GOLFDOC()->GetBar().dir = -1;
		}
		break;
	}

	if (m_bAutoShot && GOLFDOC()->m_tutorialMode >= 13 &&
		GOLFDOC()->GetBar().dir == -1 && !GOLFDOC()->m_bLockGauge)
	{
		IActor* pActor = GetActor("Tutorial");
		if (pActor)
			pActor << MsgObject(NULL, 0x155, (int)GOLFDOC()->GetBar().gauge, 0,
				0, 0, 0);
	}
}

void CGolfRule::PuttingGauge(float delta, bool bAuto)
{
	float speed = GolfClub().GetBarSpeed();

	float persent = (GOLFDOC()->GetBar().gauge - s_barStart) / s_powerBarWidth;
	speed = speed * (persent * persent + 1.0f) * 0.5f;

	if (bAuto)
	{
		switch (GOLFDOC()->m_shotPhase)
		{
		case 1:
			if (GOLFDOC()->GetBar().calipers > s_barStart)
			{
				GOLFDOC()->GetBar().gaugeStep = GOLFDOC()->GetBar().calipers;
				GOLFDOC()->GetBar().gauge = GOLFDOC()->GetBar().calipers;
				if (WisEqual(GOLFDOC()->GetBar().calipers, s_barEnd, g_EPSILON))
					GOLFDOC()->GetBar().dir = 0;
			}

			if (GOLFDOC()->GetBar().dir == 0)
				g_audio->PlaySfx(g_plSound[0]);
			else
				g_audio->PlaySfx(g_plSound[3]);

			GOLFDOC()->m_shotPhase++;
			GOLFDOC()->GetBar().power = GOLFDOC()->GetBar().gaugeStep;

			GOLFDOC()->GetBar().dir = 0;
			GOLFDOC()->GetBar().grade = 4;

			HitShot();
			return;
		}
	}

	switch (GOLFDOC()->GetBar().dir)
	{
	case 1:
		GOLFDOC()->GetBar().gauge += speed * delta;

		if (GOLFDOC()->GetBar().gauge > s_barEnd)
		{
			GOLFDOC()->GetBar().gauge = s_barEnd;
			GOLFDOC()->GetBar().dir = 0;
			m_timer = 0.0f;
		}
		break;

	case 0:
		if (GOLFDOC()->m_shotPhase > 0)
		{
			m_timer += delta;

			if (m_timer >
				(COption::Instance()->GetOption()->gPowerGauge ? 0.03f : 0.02f))
				CancelShot();
		}
		break;
	}
}

void CGolfRule::DoNavigateCamera(float delta)
{
	if (StandViewClick())
	{
		CMouseCursor::Instance()->Store();
		ChangeGameMode(8);
		SendMsg(this, "Camera", 0xf0, 0, 0, 0, 0);
	}

	if (TopViewClick())
		ChangeGameMode(0x200);
}

void CGolfRule::DoTopViewCamera(float delta)
{
	if (TopViewClick())
	{
		if (m_topViewMode)
		{
			g_camera = GOLFDOC()->MakeTopViewCameraMatrix(false);
			return;
		}

		if (Doc()->m_bMessengerAlarm && m_pMessengerAlarmBtn)
		{
			m_pMessengerAlarmBtn->SetTopmost(true);
			m_pMessengerAlarmBtn->SetVisible(true);
		}

		if (Doc()->m_bRefreshCamera && m_pMessengerGiftAlarmBtn)
		{
			if (GOLFDOC()->m_tutorialMode == 0xf)
			{
				m_pMessengerGiftAlarmBtn->SetTopmost(true);
				m_pMessengerGiftAlarmBtn->SetVisible(true);
			}
			else
			{
				m_pMessengerGiftAlarmBtn->SetTopmost(false);
				m_pMessengerGiftAlarmBtn->SetVisible(false);
			}
		}

		ChangeGameMode(8);
		CSceneManager::Instance()->UpdateFog(false);
	}
}

bool CGolfRule::StandViewClick()
{
	if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true))
		return true;
	return false;
}

bool IsMyTurn(bool check);
extern Fresh* g_pFresh;

bool CGolfRule::TopViewClick()
{
	if (CQuitWindow::Instance()->IsActive() || GOLFDOC()->m_tutorialMode < 1 ||
		!CItemWindow::IsClosed() || CUserInfo::Instance()->IsVisible() ||
		(GOLFDOC()->m_tutorialMode == 0xf &&
			g_pFresh->GetManager()->HasValidWindow() && !m_pKbdManualDlg))
		return false;

	if (g_input->GetDown("PAD_0", true) || g_input->GetDown("NUM_0", true) ||
		(g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true) &&
			(GOLFDOC()->m_gameMode & 0x200)) ||
		(!CMouseCursor::Instance()->GetArea() &&
			g_input->GetButton(RIGHT_BUTTON) == 1 &&
			CMessengerInfo::Instance()->IsGameFocus() == true))
	{
		if ((Doc()->m_curChannel.Type & 0x80) && !IsMyTurn(true))
		{
			if (CHelpMsg::Instance())
				CHelpMsg::Instance()->SetHelpMsg(0x16, 0);
			return false;
		}

		int prevMode = m_topViewMode;
		if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
				true) &&
			(GOLFDOC()->m_gameMode & 0x200))
		{
			m_topViewMode = 0;
		}
		else if (g_input->Get("LCONTROL", true) ||
			g_input->Get("RCONTROL", true))
		{
			m_topViewMode = 3;
		}
		else if ((g_input->Get("LSHIFT", true) ||
					 g_input->Get("RSHIFT", true) || g_input->Get(1)) &&
			IsMyTurn(true))
		{
			m_topViewMode = 2;
		}
		else if (g_input->GetDown("PAD_0", true) ||
			g_input->GetDown("NUM_0", true))
		{
			m_topViewMode = 1;
		}
		else
		{
			m_topViewMode = 0;
		}

		if (m_topViewMode == prevMode)
		{
			m_topViewMode = 0;
		}
		else if (m_topViewMode == 1)
		{
			if (prevMode >= 2 && prevMode <= 3)
				m_topViewMode = 0;
		}

		return true;
	}

	return false;
}

void CGolfRule::UpdateShotResultStat()
{
	if (GOLFDOC()->m_bNoStat)
		return;

	if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->GetPlayingMode() == 2)
		return;

	CGolfBall& ball = GolfBall();
	const CGolfBall::sExpected* pStart = ball.PreData(0);

	WVector diff = PLAYER(GOLFDOC()->m_currentPlayer)->pos - pStart->pos;

	float distance;
	if (IsLocalContent(S3_SAFETEE_ITEM) &&
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
		distance = 0.0f;
	else
		distance = sqrtf(diff.x * diff.x + diff.z * diff.z) * 0.3125f;

	if (ball.IsHoleIn() || ball.m_standGround > 0)
	{
		Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].fLongest =
			Max(distance,
				Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].fLongest);
	}

	if (ball.m_bOB)
	{
		Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].dwOB++;
	}
	else
	{
		if (ball.IsHoleIn())
		{
			if (GolfClub().GetType() == 3)
			{
				Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].dwPuttIn++;
				Doc()
					->m_holeStatistics[GOLFDOC()->m_currentPlayer]
					.fLongestPuttIn = Max(distance,
					Doc()
						->m_holeStatistics[GOLFDOC()->m_currentPlayer]
						.fLongestPuttIn);
			}
			else
			{
				Doc()
					->m_holeStatistics[GOLFDOC()->m_currentPlayer]
					.fLongestChipIn = Max(distance,
					Doc()
						->m_holeStatistics[GOLFDOC()->m_currentPlayer]
						.fLongestChipIn);
			}

			Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].dwHoleIn++;

			if ((Doc()->m_golfGame.gameType != GAME_TYPE_TEAM &&
					PLAYER(GOLFDOC()->m_currentPlayer)
							->stroke[GOLFDOC()->m_currentHole - 1] ==
						(unsigned char)1) ||
				(Doc()->m_golfGame.gameType == GAME_TYPE_TEAM &&
					TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
							->stroke[GOLFDOC()->m_currentHole - 1] ==
						(unsigned char)1))
			{
				Doc()
					->m_holeStatistics[GOLFDOC()->m_currentPlayer]
					.wHoleInOne++;
			}
			else if (Doc()->m_golfGame.gameType != GAME_TYPE_TEAM &&
				(unsigned char)PLAYER(GOLFDOC()->m_currentPlayer)
							->stroke[GOLFDOC()->m_currentHole - 1] -
						GOLFDOC()->GetHoleData().par ==
					-3)
			{
				Doc()
					->m_holeStatistics[GOLFDOC()->m_currentPlayer]
					.wAlbatross++;
			}
		}

		unsigned char ground = GetPVS().GetGroundType(
			PLAYER(GOLFDOC()->m_currentPlayer)->pos, true, NULL);
		if (ground == 3)
		{
			Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].wBunker++;
		}
		else if (ground == 1 || ground == 2)
		{
			if (PLAYER(GOLFDOC()->m_currentPlayer)
					->stroke[GOLFDOC()->m_currentHole - 1] == (unsigned char)1)
				Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].dwFairway++;
		}

		Doc()->m_holeStatistics[GOLFDOC()->m_currentPlayer].dwDistance +=
			(int)(sqrtf((diff.x * diff.x + diff.z * diff.z)) * 0.3125f);
	}

	GOLFDOC()->SetStatUpdated(true);
}

void CGolfRule::SendHoleStat()
{
	if (!OnlinePlay() || Doc()->m_myInfo.info.IsIdentity(2))
		return;

	bool bSend;
	unsigned char player;

	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_STROKE:
	case GAME_TYPE_MATCH:
	case GAME_TYPE_SKINS:
		bSend = GOLFDOC()->m_currentPlayer == MyIndex();
		player = GOLFDOC()->m_currentPlayer;
		break;

	case GAME_TYPE_TEAM:
		bSend = GOLFDOC()->m_currentPlayer == MyIndex() ||
			(PLAYER(MyIndex())->teammate != 0xff &&
				GOLFDOC()->m_currentPlayer == PLAYER(MyIndex())->teammate);
		player = MyIndex();
		break;

	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		player = 0;
		bSend = true;
		break;

	default:
		return;
	}

	if (bSend)
	{
		WSendPacket packet((enumClientPacket)0x31);

		sPlayerData* pPlayer = GOLFDOC()->GetPlayer(player);
		Doc()->m_holeStatistics[player].dwShotTime = pPlayer->shotTime / 1000;
		packet.EncodeBuffer(&Doc()->m_holeStatistics[player],
			sizeof(sPangYaUserStatistics));
		packet.Send(TO_GAME);
	}
}

extern int g_iSendPacketType;
extern int g_iRecvPacketType;

void CGolfRule::SetNetMsgWait(bool bWait, int line, char* msg, float time)
{
	if (bWait)
	{
		m_bNetMsgWait = true;
		m_netMsgWaitLine = line;
		m_netMsgWaitTime = time;
		m_netMsgSend = msg;
		m_netMsgSendTime = GetTickCount();

		g_iSendPacketGR = g_iSendPacketType;
		g_iRecvPacketGR = 0;
		g_iRecvPacketType = 0;
	}
	else
	{
		m_bNetMsgWait = false;
		m_netMsgRecv = msg;
		m_netMsgRecvTime = GetTickCount();

		g_iRecvPacketGR = g_iRecvPacketType;
	}
}

void CGolfRule::CheckNetMsgWait_Send(char* msg)
{
	if (m_netMsgSend && !strcmpi(m_netMsgSend, msg))
		g_iSendPacketGR = g_iSendPacketType;
}

void CGolfRule::CheckNetMsgWait_Recv(char* msg)
{
	if (m_netMsgSend && !strcmpi(m_netMsgSend, msg))
		g_iRecvPacketGR = g_iRecvPacketType;
}

void CGolfRule::DoToDefaultCamera(float delta)
{
	if (OnlinePlay() && !m_bSendFinishShot && Doc()->GetPlayingMode() == 1)
	{
		m_bSendFinishShot = true;

		if (!IsMassGame())
			SetNetMsgWait(true, 12730, "DoToDefaultCamera", 20.0f);

		WSendPacket packet((enumClientPacket)0x1c);
		EncodePacket((enumClientPacket)0x1c, packet);
		packet.Send(TO_GAME);

		CheckNetMsgWait_Send("DoToDefaultCamera");
	}

	if (GetGameType() == GAME_TYPE_NEW_APPROACH && !m_bModal)
	{
		m_bModal = true;

		m_bShotResult = false;
		m_bSendFinishShot = false;
		m_bBallStopped = false;
		m_bCanSkip = false;

		SendMsg(this, "Effect", 375, 0, 0, 0, 0);
		SendMsg(this, "Effect", 376, 0, 0, 0, 0);
		SendMsg(this, "Letter", 396, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 396, 0, 0, 0, 0);
		CFx::Instance()->ClearAll(true);

		g_view->SetFOV(55.0f * g_DEGTORAD);

		if (IsLocalContent((localContentType_t)0x2f))
		{
			if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
				SendMsg(this, "Player", 515, 0, 0, 0, 0);
		}

		PLAYER(GOLFDOC()->m_currentPlayer)->item = 0;

		CCameraWork::Instance()->SelectCamera(208);

		SendMsg(this, "Player", 356, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 356, 0, 0, 0, 0);
		SendMsg(this, "Mascot", 356, 0, 0, 0, 0);
		SendMsg(this, "Bubble", 356, 0, 0, 0, 0);

		CHeadIcon::Instance()->Hide();

		CProjectG::Instance()->SetGameSpeed(1.0f);
	}
	else
	{
		CHeadIcon::Instance()->Show();
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
		CCameraWork::Instance()->Process(delta, 0.0f);
	else
		CCameraWork::Instance()->InputProcess(delta);

	if (m_bModal || m_bNetMsgWait || m_pGoStopDlg)
		return;

	if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->GetPlayingMode() == 2)
	{
		if (Doc()->m_replayState == 7)
		{
			g_input->SetFakeInput("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
				false);
		}
		else if (Doc()->m_replayState == 12)
		{
			if (!GolfBall().IsHoleIn())
			{
				Doc()->m_replayState = 0;
				g_input->SetFakeInput(
					"\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", false);
			}
		}
		else if (Doc()->m_replayState == 10)
		{
			Doc()->m_replayState = 0;
			g_input->SetFakeInput("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
				false);
		}
		else if (Doc()->m_replayState != 14)
		{
			return;
		}
	}

	m_bShotResult = false;
	m_bSendFinishShot = false;
	m_bBallStopped = false;
	m_bCanSkip = false;

	SendMsg(this, "Effect", 375, 0, 0, 0, 0);
	SendMsg(this, "Effect", 376, 0, 0, 0, 0);
	SendMsg(this, "Letter", 396, 0, 0, 0, 0);
	SendMsg(this, "Trajectory", 396, 0, 0, 0, 0);
	CFx::Instance()->ClearAll(true);

	g_view->SetFOV(55.0f * g_DEGTORAD);

	CGolfBall& ball = GolfBall();
	CGolfBall::sExpected* pFirst = ball.PreDataToWrite(0);

	if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->GetPlayingMode() == 2)
	{
		ball.m_playFrame = -1;
		pFirst->pos = m_shotData.pos;
		ball.m_pos = m_shotData.pos;
		ball.m_meshIndex = GetPVS().GetMeshIndex(ball.m_pos, false);

		PLAYER(0)->bunker = m_shotData.bunker;
		SendMsg(this, "Ball", 493, 0, 0, 0, 0);

		GOLFDOC()->MakeDefaultCamera(true);

		if (Doc()->m_replayShotIndex == 0)
			ChangeGameMode(2);
		else
			ChangeGameMode(8);
		return;
	}

	if (IsLocalContent((localContentType_t)0x2f))
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
			SendMsg(this, "Player", 515, 0, 0, 0, 0);
	}

	unsigned long item = PLAYER(GOLFDOC()->m_currentPlayer)->item;
	PLAYER(GOLFDOC()->m_currentPlayer)->item = 0;

	if (GOLFDOC()->m_tutorialMode < 15)
		SendMsg(this, "Screen", 437, 0, 0, 0, 0);

	ball.m_playFrame = -1;
	ball.m_renderNum = 0;
	ball.m_initCurve = 0;
	ball.m_initSpin = 0;
	ball.m_rot.Reset();

	if (GOLFDOC()->m_tutorialMode < 15 && GOLFDOC()->m_tutorialMode != 8)
	{
		if (m_bAutoShot)
		{
			IActor* pTutorial = GetActor("Tutorial");
			if (pTutorial)
				pTutorial << MsgObject(NULL, 328, 0, 0, 0, 0, 0);
		}
		else
		{
			WVector dist = GOLFDOC()->m_target.pos - ball.m_pos;
			dist.y = 0.0f;
			float length = dist.Magnitude();

			IActor* pTutorial = GetActor("Tutorial");
			if (!pTutorial)
				return;

			if (GOLFDOC()->m_tutorialMode == 2 ||
				GOLFDOC()->m_tutorialMode == 10 ||
				GOLFDOC()->m_tutorialMode == 11 ||
				GOLFDOC()->m_tutorialMode == 13 ||
				GOLFDOC()->m_tutorialMode == 14)
			{
				float scale = GOLFDOC()->m_tutorialMode == 2 ? 2.0f : 1.0f;

				if (length < 44.8f * scale)
				{
					SendMsg(this, "GolfRule", 136, 1, 0, 0, 0);

					switch (GOLFDOC()->m_tutorialMode)
					{
					case 2:
					case 10:
					case 11:
						pTutorial << MsgObject(NULL, 329, 0, 0, 0, 0, 0);
						return;

					case 13:
						if ((GolfBall().m_initSpecial & 1) ||
							(GolfBall().m_initSpecial & 2))
						{
							pTutorial << MsgObject(NULL, 329, 0, 0, 0, 0, 0);
							return;
						}
						break;

					case 14:
						if ((GolfBall().m_initSpecial & 4) ||
							(GolfBall().m_initSpecial & 8))
						{
							pTutorial << MsgObject(NULL, 329, 0, 0, 0, 0, 0);
							return;
						}
						break;
					}
				}
			}
			else if (GOLFDOC()->m_tutorialMode == 3)
			{
				if (GolfClub().PowerShotType() == 1 &&
					GOLFDOC()->m_bar.grade > 1)
				{
					SendMsg(this, "GolfRule", 136, 1, 0, 0, 0);
					pTutorial << MsgObject(NULL, 329, 0, 0, 0, 0, 0);
					return;
				}
			}
			else if (GOLFDOC()->m_tutorialMode == 5)
			{
				if (item == 0x18000004 && GOLFDOC()->m_bar.grade > 1)
				{
					SendMsg(this, "GolfRule", 136, 1, 0, 0, 0);
					pTutorial << MsgObject(NULL, 329, 0, 0, 0, 0, 0);
					return;
				}
			}
		}

		PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;
		PLAYER(GOLFDOC()->m_currentPlayer)->pos = ball.PreData(0)->pos;
		ball.m_pos = ball.PreData(0)->pos;
		ball.m_meshIndex = GetPVS().GetMeshIndex(ball.m_pos, false);

		bool bKeepAngle = GOLFDOC()->m_tutorialMode == 2 ||
			GOLFDOC()->m_tutorialMode == 6 || GOLFDOC()->m_tutorialMode == 4 ||
			GOLFDOC()->m_tutorialMode == 10;
		if (bKeepAngle)
		{
			SendMsg(this, "Screen", 419, 0, 0, 0, 0);
			GOLFDOC()->m_cameraData.angle = m_shotData.angle;
			GolfClub().SetPowerShot(0);
			SetAngle(GOLFDOC()->m_cameraData.angle);
			SendMsg(this, "Player", 226, 0, 0, 0, 0);

			if (GOLFDOC()->m_tutorialMode == 2)
				SendMsg(this, "Caddie", 19, 0, 0, 0, 0);

			float alpha = 0.0f;
			SendMsg(this, "Screen", 426, (int)&alpha, 0, 0, 0);
		}
		else
		{
			SetEnvironmentVariableA();
		}

		CMouseCursor::Instance()->Reset(true);

		ChangeTurn();
		ChangeGameMode(8);
	}
	else
	{
		SendMsg(this, "Clock", 10010, 0, 0, 0, 0);

		if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
		{
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->shooter =
				GOLFDOC()->m_currentPlayer;
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->club =
				GolfClub().GetType();
		}

		GOLFDOC()->m_currentPlayer = m_nextPlayer;
		PLAYER(GOLFDOC()->m_currentPlayer)
			->stroke[GOLFDOC()->m_currentHole - 1] += 1;
		pFirst->pos = PLAYER(GOLFDOC()->m_currentPlayer)->pos;
		ball.m_pos = PLAYER(GOLFDOC()->m_currentPlayer)->pos;
		ball.m_meshIndex = GetPVS().GetMeshIndex(ball.m_pos, false);

		if (IsLocalContent((localContentType_t)0x2f) &&
			PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
			PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;

		if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
				->stroke[GOLFDOC()->m_currentHole - 1] += 1;

		SetEnvironmentVariableA();

		if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
		{
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->shooter =
				GOLFDOC()->m_currentPlayer;
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->club =
				GolfClub().GetType();
		}

		if (SetTurnChangeEvent())
		{
			ChangeTurn();
			GOLFDOC()->MakeDefaultCamera(true);
			ChangeGameMode(4);

			if (IsMassGame())
			{
				SendMsg(this, "Score", 308,
					(unsigned char)PLAYER(GOLFDOC()->m_currentPlayer)
							->stroke[GOLFDOC()->m_currentHole - 1] -
						GOLFDOC()->GetHoleData().par,
					0, 0, 0);
			}
		}
		else
		{
			ReadyForNextTurn();
		}
	}

	SendMsg(this, "Screen", 421, 0, 0, 0, 0);

	if (m_bAutoShot)
		return;

	IActor* pTutorial = GetActor("Tutorial");
	if (!pTutorial)
		return;

	if (GOLFDOC()->m_tutorialMode == 8)
		return;

	pTutorial << MsgObject(NULL, 340, 0, 0, 0, 0, 0);
}

struct sLetterNumber
{
	int reserved;
	int number;
	float scale;
	unsigned long color;
};

bool CGolfRule::SetTurnChangeEvent()
{
	sDetermineWhetherToEnableFiledItem determine;

	CGolfBall& ball = GolfBall();
	bool bResult = false;
	float scale = 2.0f;

	if (OnlinePlay())
		m_bFlagCutin = false;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		if (ball.m_groundType == 2)
		{
			int team = PLAYER(GOLFDOC()->m_currentPlayer)->team;
			bool bWinning = false;
			bool bMatch = false;
			int diff = TEAM(team)->score - TEAM(1 - team)->score;

			if (TEAM(1 - team)->state == 1)
			{
				if (TEAM(team)->stroke[GOLFDOC()->m_currentHole - 1] <
					TEAM(1 - team)->stroke[GOLFDOC()->m_currentHole - 1])
					bWinning = true;
			}
			else
			{
				if (TEAM(team)->stroke[GOLFDOC()->m_currentHole - 1] <=
					TEAM(1 - team)->stroke[GOLFDOC()->m_currentHole - 1])
					bWinning = true;
			}

			if (bWinning)
			{
				if (diff + 1 >
					GetHoles() - GetHoleIndex(GOLFDOC()->m_currentHole))
					bMatch = true;
			}
			else
			{
				if (diff > GetHoles() - GetHoleIndex(GOLFDOC()->m_currentHole))
					bMatch = true;
			}

			if (bMatch || bWinning)
			{
				if (Doc()
						->m_userInfo[GOLFDOC()->m_currentPlayer]
						.caddieInfo.guid != 0)
					bResult = true;
			}

			if (IsMyTurn(true))
			{
				if (bMatch)
				{
					SendMsg(this, "Letter", 397, (int)"matchshot", (int)&scale,
						0, 0);
					return bResult;
				}

				if (bWinning)
				{
					SendMsg(this, "Letter", 397, (int)"winningshot",
						(int)&scale, 0, 0);
					return bResult;
				}
			}
		}
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
	{
		if (ball.m_groundType == 2)
		{
			bool bWinning = false;
			bool bMatch = false;
			int diff = PLAYER(GOLFDOC()->m_currentPlayer)->score -
				PLAYER(1 - GOLFDOC()->m_currentPlayer)->score;

			if (PLAYER(1 - GOLFDOC()->m_currentPlayer)->state == 1)
			{
				if (PLAYER(GOLFDOC()->m_currentPlayer)
						->stroke[GOLFDOC()->m_currentHole - 1] <
					PLAYER(1 - GOLFDOC()->m_currentPlayer)
						->stroke[GOLFDOC()->m_currentHole - 1])
					bWinning = true;
			}
			else
			{
				if (PLAYER(GOLFDOC()->m_currentPlayer)
						->stroke[GOLFDOC()->m_currentHole - 1] <=
					PLAYER(1 - GOLFDOC()->m_currentPlayer)
						->stroke[GOLFDOC()->m_currentHole - 1])
					bWinning = true;
			}

			if (bWinning)
			{
				if (diff + 1 >
					GetHoles() - GetHoleIndex(GOLFDOC()->m_currentHole))
					bMatch = true;
			}
			else
			{
				if (diff > GetHoles() - GetHoleIndex(GOLFDOC()->m_currentHole))
					bMatch = true;
			}

			if (bMatch || bWinning)
			{
				if (Doc()
						->m_userInfo[GOLFDOC()->m_currentPlayer]
						.caddieInfo.guid != 0)
					bResult = true;
			}

			if (IsMyTurn(true))
			{
				if (bMatch)
				{
					SendMsg(this, "Letter", 397, (int)"matchshot2", (int)&scale,
						0, 0);
					return bResult;
				}

				if (bWinning)
				{
					SendMsg(this, "Letter", 397, (int)"winningshot2",
						(int)&scale, 0, 0);
					return bResult;
				}
			}
		}
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		if (ball.m_groundType == 2)
		{
			if (GOLFDOC()->m_skinsState == 2 &&
				Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].caddieInfo.guid !=
					0)
				bResult = true;

			if (IsMyTurn(true))
			{
				if (GOLFDOC()->m_skinsState == 1)
				{
					SendMsg(this, "Letter", 397, (int)"matchshot3", (int)&scale,
						0, 0);
				}
				else if (GOLFDOC()->m_skinsState == 2)
				{
					SendMsg(this, "Letter", 397, (int)"winningshot3",
						(int)&scale, 0, 0);

					sLetterNumber number;
					number.number = (int)GOLFDOC()->m_skinsHolePang;
					number.scale = 0.8f;
					number.color = 0xffffde44;
					SendMsg(this, "Letter", 401, (int)"winningshot3",
						(int)&number, 0, 0);
				}
				else
				{
					SendMsg(this, "Letter", 397,
						(int)"\xc2\xf7\xb7\xca\xc7\xa5\xbd\xc3", (int)&scale, 0,
						0);
				}
			}
		}
		else if (IsMyTurn(true) && GOLFDOC()->m_bNoStat)
		{
			if (GolfClub().GetApproachMode() <= 2)
				SendMsg(this, "Letter", 397,
					(int)"\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
						 "30_\xbf\xac\xc0\xe5",
					(int)&scale, 0, 0);
			else if (GolfClub().GetApproachMode() <= 3)
				SendMsg(this, "Letter", 397,
					(int)"\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
						 "60_\xbf\xac\xc0\xe5",
					(int)&scale, 0, 0);
		}

		return bResult;
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
	{
		return false;
	}
	else
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)
					->stroke[GOLFDOC()->m_currentHole - 1] <
				GOLFDOC()->GetHoleData().par &&
			ball.m_groundType == 2)
		{
			bResult =
				Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].caddieInfo.tid !=
				0;

			if (IsMyTurn(true))
			{
				switch ((unsigned char)PLAYER(GOLFDOC()->m_currentPlayer)
							->stroke[GOLFDOC()->m_currentHole - 1] -
					GOLFDOC()->GetHoleData().par)
				{
				case -2:
					SendMsg(this, "Letter", 397,
						(int)"\xc0\xcc\xb1\xdb\xc2\xf9\xbd\xba", (int)&scale, 0,
						0);
					return bResult;

				case -1:
					SendMsg(this, "Letter", 397,
						(int)"\xb9\xf6\xb5\xf0\xc2\xf9\xbd\xba", (int)&scale, 0,
						0);
					return bResult;
				}
			}
		}
	}

	if (Doc()->GetPlayingMode() == 1)
	{
		if (IsMyTurn(true))
		{
			if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].info.dwIdentity &
				1)
			{
				SendMsg(this, "Letter", 397, (int)"AI", 0, 0, 0);
			}
			else
			{
				if ((ball.m_groundType == 2 ||
						GolfClub().GetApproachMode() == 4) &&
					GOLFDOC()->m_tutorialMode != 9)
					SendMsg(this, "Letter", 397,
						(int)"\xc2\xf7\xb7\xca\xc7\xa5\xbd\xc3", (int)&scale, 0,
						0);
				else if (GolfClub().GetApproachMode() <= 2)
					SendMsg(this, "Letter", 397,
						(int)"\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
							 "30",
						(int)&scale, 0, 0);
				else if (GolfClub().GetApproachMode() <= 3)
					SendMsg(this, "Letter", 397,
						(int)"\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
							 "60",
						(int)&scale, 0, 0);
			}

			SendMsg(this, "Score", 303, 0, 0, 0, 0);
			if (g_pFresh)
				g_pFresh->GetManager()->CloseForm(false);
		}
		else
		{
			SendMsg(this, "Spin", 356, 0, 0, 0, 0);
			SendMsg(this, "Letter", 397, (int)"Wait", 0, 0, 0);
		}
	}

	return bResult;
}

void CGolfRule::CorrectShotResult()
{
	if (PLAYER(GOLFDOC()->m_currentPlayer)->state == 3 ||
		GOLFDOC()->m_shotResult[GOLFDOC()->m_currentPlayer].result == 0)
	{
		CProjectG::Instance()->ReportError(1,
			"CorrectShotResult_%d_%d_%d(%d_%d)", g_iSendPacketGR,
			g_iRecvPacketGR, GOLFDOC()->m_gameMode, Doc()->m_roomInfo.roomGuid,
			Doc()->m_myInfo.info.dwGuid);
		return;
	}

	for (unsigned char i = 0; i < 3; i++)
		PLAYER(GOLFDOC()->m_currentPlayer)->pos.p[i] =
			GOLFDOC()->m_shotResult[GOLFDOC()->m_currentPlayer].pos[i];

	PLAYER(GOLFDOC()->m_currentPlayer)->bunker =
		GOLFDOC()->m_shotResult[GOLFDOC()->m_currentPlayer].bunker;

	{
		CGolfDoc* pDoc = GOLFDOC();
		CRYPTIC_SET_INT(PLAYER(pDoc->m_currentPlayer)->totalPang,
			pDoc->m_shotResult[pDoc->m_currentPlayer].pang);
	}
	{
		CGolfDoc* pDoc = GOLFDOC();
		CRYPTIC_SET_INT(PLAYER(pDoc->m_currentPlayer)->bonusPang,
			pDoc->m_shotResult[pDoc->m_currentPlayer].bonusPang);
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->teammate != 0xff)
		{
			PLAYER(PLAYER(GOLFDOC()->m_currentPlayer)->teammate)->pos =
				PLAYER(GOLFDOC()->m_currentPlayer)->pos;
			PLAYER(PLAYER(GOLFDOC()->m_currentPlayer)->teammate)->bunker =
				PLAYER(GOLFDOC()->m_currentPlayer)->bunker;
		}

		TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->pos =
			PLAYER(GOLFDOC()->m_currentPlayer)->pos;
		TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->bunker =
			PLAYER(GOLFDOC()->m_currentPlayer)->bunker;
	}

	switch (GOLFDOC()->m_shotResult[GOLFDOC()->m_currentPlayer].result)
	{
	case 3:
		GolfBall().m_bOB = true;
		GolfBall().SetHoleIn(FALSE);
		break;
	case 5:
		GolfBall().m_standGround = 0;
	case 2:
		GolfBall().SetHoleIn(FALSE);
		break;
	case 4:
		GolfBall().SetHoleIn(TRUE);
		break;
	default:
		CRYPTIC_SET_INT(GolfBall().m_holeIn, 0);
	}

	if (OnlinePlay())
	{
		if (GolfClub().GetType() != 3 && GolfClub().GetType() != 2 &&
			PLAYER(GOLFDOC()->m_currentPlayer)->oid ==
				Doc()->m_myInfo.info.dwGuid &&
			GolfBall().m_groundType != 2 && Doc()->GetPlayingMode() == 1)
		{
			if (GolfBall().IsHoleIn())
			{
				m_bFlagCutin = true;
			}
			else
			{
				WVector dist;
				dist = GetHoleCupPos() - GolfBall().m_pos;
				dist.y = 0.0f;
				if (dist.Magnitude() < 15.0f)
					m_bFlagCutin = true;
			}
		}

		int gameMode = Doc()->m_gameMode;
		if (GOLFDOC()->m_tutorialMode < 15)
			gameMode = 6;

		if (Doc()->GetPlayingMode() == 2)
			m_bFlagCutin = false;

		if (m_bFlagCutin)
		{
			if (Doc()->m_cutinList.size() > 0)
			{
				std::list<sItemInfo>::iterator it;
				for (it = Doc()->m_cutinList.begin();
					it != Doc()->m_cutinList.end(); it++)
				{
					if (S5::ISCutinItem((*it).tid))
					{
						IFF_STRUCT::S5::sCutinInformation* pInfo =
							ItemManager()->FindCutinInformation((*it).tid);
						if (pInfo && (pInfo->Condition & 8))
						{
							SendMsg(this, "Cutin", 120, 1, 0, 0, 0);
							S5::UTIL::SendCutInPacket(MyUID(), 8, gameMode,
								Doc()
									->m_userInfo[GOLFDOC()->m_currentPlayer]
									.charInfo.tid);
							return;
						}
					}
					m_bFlagCutin = false;
				}
			}
			else
			{
				m_bFlagCutin = false;
			}
		}
	}
}

void CGolfRule::SetEnvironmentVariableA()
{
	SendMsg(this, "Screen", 419, 0, 0, 0, 0);

	if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->GetPlayingMode() == 2)
	{
		GOLFDOC()->m_cameraData.angle = m_shotData.angle;
		m_target =
			GolfBall().m_pos + RotMat(GOLFDOC()->m_cameraData.angle, 1).za;
		GolfClub().SetPowerShot(m_shotData.clubPower);

		GolfClub().SelectBestClub();

		GolfClub().SetClub(m_shotData.club);
	}
	else
	{
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].info.dwIdentity & 1)
		{
			CRival::Instance()->SetVariable(GOLFDOC());
			m_target = CRival::Instance()->GetPos();
		}
		else
		{
			if (GOLFDOC()->m_tutorialMode == 2 ||
				GOLFDOC()->m_tutorialMode == 6)
				GolfClub().SelectBestClub(m_target);
			else
				GolfClub().SelectBestClub();

			if (GOLFDOC()->m_tutorialMode == 15 ||
				GOLFDOC()->m_tutorialMode == 8)
				m_target = GOLFDOC()->m_pAutoTarget->GetPoint(NULL);
		}

		GOLFDOC()->m_cameraData.angle =
			CalcDeltaAngle(m_target - GolfBall().m_pos, WVector::UNIT_POS_Z);
		GolfClub().SetPowerShot(0);
	}

	SetAngle(GOLFDOC()->m_cameraData.angle);
	SendMsg(this, "Player", 226, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 19, 0, 0, 0, 0);

	float alpha = 0.0f;
	if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->GetPlayingMode() == 2)
	{
		if (m_shotData.clubPower == 1 || m_shotData.clubPower == 2)
			alpha = 1.0f;
	}

	SendMsg(this, "Spot", 245, 0, 0, 0, 0);
	SendMsg(this, "Screen", 426, (int)&alpha, 0, 0, 0);
}

void CGolfRule::SetAngle(float angle)
{
	SendMsg(this, "Camera", 10008, (int)&angle, 0, 0, 0);
	SendMsg(this, "Spot", 10008, (int)&angle, 0, 0, 0);
	SendMsg(this, "Screen", 10008, (int)&angle, 0, 0, 0);
	SendMsg(this, "Grid", 10008, (int)&angle, 0, 0, 0);
	SendMsg(this, "Caddie", 10008, (int)&angle, 0, 0, 0);
}

WMatrix CGolfRule::BankCamera(float angle)
{
	return RotMat(angle, 2) * g_camera;
}

bool IsMyTurn(bool bCheck);

void CGolfRule::Replay(bool bAuto)
{
	CGolfBall& ball = GolfBall();
	const CGolfBall::sExpected* data = ball.PreData(0);

	m_bDrivePose = false;
	m_bShotResult = true;

	ball.m_playFrame = 0;
	ball.m_pos = data->pos;
	ball.m_vel = data->vel;
	ball.m_state = data->state;
	ball.m_special = ball.m_initSpecial;

	SendMsg(this, "ShotPreview", -1, (int)"PM_REPLAY", 0, 0, 0);

	SendMsg(this, "NPC", 389, 0, 0, 0, 0);
	SendMsg(this, "Screen", 437, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 19, 0, 0, 0, 0);

	ReactionForShot();

	if ((Doc()->m_curChannel.Type & 0x80) && IsOthersTurn(true))
		SendMsg(this, "Screen", 424, 0, 0, 0, 0);
	else
		SendMsg(this, "Screen", 423, 0, 0, 0, 0);

	SendMsg(this, "Screen", 452, 0, 0, 0, 0);
	SendMsg(this, "Trajectory", 396, 0, 0, 0, 0);
	SendMsg(this, "Letter", 396, 0, 0, 0, 0);
	SendMsg(this, "Effect", 375, 0, 0, 0, 0);
	SendMsg(this, "Effect", 376, 0, 0, 0, 0);
	CFx::Instance()->ClearAll(true);
	SendMsg(this, "Ball", 494, 0, 0, 0, 0);

	g_view->SetFOV(55.0f * g_DEGTORAD);
	g_camera = GOLFDOC()->GetDefaultCamera();
	g_audio->StopSfx("shot_dist_count");

	SendMsg(this, "Letter", 402, 0, 0, 0, 0);

	SendMsg(this, "Player", 271, 0, 0, 0, 0);
	m_processTime = 1.0f;
	GOLFDOC()->m_gameMode = 0x40;
	GetPVS().SetFrame(m_replayPvsFrame);

	CCameraWork::Instance()->ReplayCamera(bAuto);
	GOLFDOC()->m_bReplay = true;

	if (CHelpMsg::Instance())
		CHelpMsg::Instance()->OnChangeTurn();
}

void CGolfRule::Skip()
{
	m_bShotResult = false;
	m_bBallStopped = false;
	m_bCanSkip = false;

	SendMsg(this, "Screen", 438, 0, 0, 0, 0);
	SendMsg(this, "Score", 306, 0, 0, 0, 0);
	SendMsg(this, "NPC", 386, 0, 0, 0, 0);
	CGolfBall& ball = GolfBall();

	if (ball.m_bOB && OnlinePlay() && IsMassGame() && CanCancelShot())
		SendShotResult();

	if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
	{
		Doc()->m_rivalList[MyIndex()].approachDistance =
			Doc()->m_rivalList[MyIndex()].approachResultDistance;
		SendMsg(this, "Score", 42, 1, 0, 0, 0);
	}

	if (GOLFDOC()->m_gameMode == 0x40)
	{
		if (ball.m_rollFrame > 0)
			SendMsg(this, "Ball", 492, 0, 0, 0, 0);

		SendMsg(this, "Screen", 424, 0, 0, 0, 0);
		SendMsg(this, "Spin", 356, 0, 0, 0, 0);
	}

	WPuppet* puppet = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
	if (puppet && puppet->GetRootBone())
		puppet->SetAlpha(0xff);

	g_audio->StopSfx("shot_dist_count");
	CCameraWork::Instance()->StopBallSound();

	UpdateShotResultStat();

	if (ball.IsHoleIn())
	{
		RecordScoreByPlayer(GOLFDOC()->m_currentPlayer);
		ChangeGameMode(0x1000);

		AddPrize((unsigned char)(Doc()->m_golfGame.gameType == GAME_TYPE_TEAM
						 ? TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
							   ->stroke[GOLFDOC()->m_currentHole - 1]
						 : PLAYER(GOLFDOC()->m_currentPlayer)
							   ->stroke[GOLFDOC()->m_currentHole - 1]) -
			GOLFDOC()->GetHoleData().par);
		AddExtPrize();
		return;
	}

	if (ball.m_bOB)
	{
		PLAYER(GOLFDOC()->m_currentPlayer)->flag++;
		PLAYER(GOLFDOC()->m_currentPlayer)
			->stroke[GOLFDOC()->m_currentHole - 1] += 1;
		if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
				->stroke[GOLFDOC()->m_currentHole - 1] += 1;
	}
	else
	{
		if (!ball.IsHoleIn() && !ball.m_standGround)
		{
			PLAYER(GOLFDOC()->m_currentPlayer)
				->stroke[GOLFDOC()->m_currentHole - 1] += 1;
			if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
			{
				TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
					->stroke[GOLFDOC()->m_currentHole - 1] += 1;
			}
		}
		PLAYER(GOLFDOC()->m_currentPlayer)->flag = 0;
	}

	if (m_pCurrentRule && m_pCurrentRule->CheckGiveUp())
	{
		SendMsg(this, "GuideLine", 418, 0, 0, 0, 0);
		RecordGiveUp(GOLFDOC()->m_currentPlayer);
		RecordScoreByPlayer(GOLFDOC()->m_currentPlayer);
		if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
			ChangeGameMode(0x1000);
		else
			ChangeGameMode(0x80000);
		return;
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
		ChangeGameMode(0x1000);
	else
		ChangeGameMode(0x800);
}

void CGolfRule::Drop(const WVector& pos)
{
	if (GOLFDOC()->m_tutorialMode == 9)
	{
		IActor* tutorial = GetActor("Tutorial");
		if (tutorial)
			tutorial << MsgObject(NULL, 329, 0, 0, 0, 0, 0);
	}

	CGolfBall::sExpected* data = GolfBall().PreDataToWrite(0);

	PLAYER(GOLFDOC()->m_currentPlayer)->stroke[GOLFDOC()->m_currentHole - 1] +=
		1;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
		TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
			->stroke[GOLFDOC()->m_currentHole - 1] += 1;

	GolfBall().m_pos = pos;
	PLAYER(GOLFDOC()->m_currentPlayer)->pos = pos;
	PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;

	data->pos = pos;
	GolfBall().m_meshIndex = GetPVS().GetMeshIndex(pos, false);

	SendMsg(this, "Screen", 419, 0, 0, 0, 0);

	GolfClub().SelectBestClub();
	m_target = GOLFDOC()->m_pAutoTarget->GetPoint(NULL);
	GOLFDOC()->m_cameraData.angle =
		CalcDeltaAngle(m_target - pos, WVector::UNIT_POS_Z);
	GolfClub().SetPowerShot(0);

	SetAngle(GOLFDOC()->m_cameraData.angle);
	SendMsg(this, "Player", 226, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 19, 0, 0, 0, 0);

	float param = 0.0f;
	SendMsg(this, "Spot", 245, 0, 0, 0, 0);
	SendMsg(this, "Screen", 426, (int)&param, 0, 0, 0);
	SendMsg(this, "Clock", 10010, 0, 0, 0, 0);

	SetTurnChangeEvent();
	ReadyForNextTurn();
}

void CGolfRule::ForcedMoveBall(const WVector& pos)
{
	CGolfBall::sExpected* data = GolfBall().PreDataToWrite(0);

	GolfBall().m_pos = pos;
	PLAYER(GOLFDOC()->m_currentPlayer)->pos = pos;
	PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;

	data->pos = pos;
	GolfBall().m_meshIndex = GetPVS().GetMeshIndex(pos, false);

	SendMsg(this, "Screen", 419, 0, 0, 0, 0);

	GolfClub().SelectBestClub();
	m_target = GOLFDOC()->m_pAutoTarget->GetPoint(NULL);

	SendMsg(this, "Player", 226, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 19, 0, 0, 0, 0);

	float param = 0.0f;
	SendMsg(this, "Spot", 245, 0, 0, 0, 0);
	SendMsg(this, "Screen", 426, (int)&param, 0, 0, 0);
}

void CGolfRule::Cancel()
{
	if (m_backUpList.empty())
		return;

	CCameraWork::Instance()->Unlock();

	CGolfBall& ball = GolfBall();
	CGolfBall::sExpected* data = ball.PreDataToWrite(0);

	PLAYER(0)->pos = m_backUpList.top().pos;
	PLAYER(0)->bunker = m_backUpList.top().bunker;
	PLAYER(0)->gauge = m_backUpList.top().gauge;

	PLAYER(0)->totalPang = (int)m_backUpList.top().totalPang;
	PLAYER(0)->bonusPang = (int)m_backUpList.top().bonusPang;
	PLAYER(0)->pang[GOLFDOC()->m_currentHole - 1] =
		(int)m_backUpList.top().pang;

	PLAYER(0)->stroke[GOLFDOC()->m_currentHole - 1] = m_backUpList.top().stroke;
	PLAYER(0)->flag = m_backUpList.top().flag;
	PLAYER(0)->state = 0;

	PLAYER(0)->itemNum = 0;
	for (int i = 0; i < 10; i++)
	{
		if (Doc()->m_userInfo[0].userEquip.tidItemSlot[i])
			PLAYER(0)->itemNum++;
	}

	g_audio->StopSfx("shot_dist_count");

	data->pos = PLAYER(0)->pos;
	ball.m_pos = PLAYER(0)->pos;
	ball.m_meshIndex = GetPVS().GetMeshIndex(ball.m_pos, false);
	ball.m_playFrame = -1;
	GOLFDOC()->m_cameraData.angle = m_backUpList.top().angle;

	SendMsg(this, "Screen", 419, 0, 0, 0, 0);

	GolfClub().SetPowerShot(0);
	GolfClub().SelectBestClub();
	GolfClub().SetClub(m_backUpList.top().club);

	m_backUpList.pop();

	m_target = GolfBall().m_pos + RotMat(GOLFDOC()->m_cameraData.angle, 1).za;

	SetAngle(GOLFDOC()->m_cameraData.angle);
	SendMsg(this, "Player", 226, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 19, 0, 0, 0, 0);

	float param = 0.0f;
	SendMsg(this, "Spot", 245, 0, 0, 0, 0);
	SendMsg(this, "Screen", 426, (int)&param, 0, 0, 0);
	SendMsg(this, "Clock", 10010, 0, 0, 0, 0);

	SendMsg(this, "Effect", 375, 0, 0, 0, 0);
	SendMsg(this, "Trajectory", 396, 0, 0, 0, 0);
	CFx::Instance()->ClearAll(true);
	SendMsg(this, "Score", 306, 0, 0, 0, 0);

	SendMsg(this, "ShotPreview", -1, (int)"POP_BACK_ITEM", 0, 0, 0);
	SendMsg(this, "ShotReview", -1, (int)"POP_BACK_ITEM", 0, 0, 0);

	SendMsg(this, "MagicCarpetHelper", 639, 0, 0, 0, 0);

	SetTurnChangeEvent();
	ReadyForNextTurn();

	m_bCanSkip = false;
	m_bShotResult = false;
	m_bBallStopped = false;
	m_timer = 0;
}

void CGolfRule::Warp(const WVector& pos)
{
	int index = GetPVS().GetMeshIndex(pos, true);

	if (index >= 0 && !GOLFDOC()->m_pPolySoup->IsOutOfBound(pos))
	{
		PLAYER(GOLFDOC()->m_currentPlayer)->pos = pos;
		PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;
		GolfBall().m_pos = pos;
		GolfBall().m_meshIndex = index;

		SetEnvironmentVariable();
		SetTurnChangeEvent();
		ReadyForNextTurn();
	}
}

void CGolfRule::FlyBy(float delta)
{
	static unsigned int index = 0;
	WVector pos;
	WVector dir;

	m_timer += delta;

	if (m_timer > 5.0f)
	{
		index += 2;
		m_timer -= 5.0;
		if (index >= m_flyByPos.size() - 1)
		{
			index = 0;
			SendMsg(this, "GolfBg", 485, (int)&m_flyByPos, 0, 0, 0);
		}
	}

	float t = m_timer * 0.2f;
	float t2 = t * t;
	float it = 1.0f - t;
	float it2 = it * it;

	pos = it2 * m_flyByPos[index] + it * t * 2.0f * m_flyByPos[index + 1] +
		t2 * m_flyByPos[index + 2];

	unsigned int next = index + 2;
	if (next >= m_flyByPos.size() - 1)
		next = 0;

	dir = it2 * m_flyByPos[next] + it * t * 2.0f * m_flyByPos[next + 1] +
		t2 * m_flyByPos[next + 2];

	dir = (dir - pos).Normalize();
	dir.y = -0.1f;

	unsigned int half = (m_flyByPos.size() - 1) / 2;
	if (index == 0 || index == half - 4 || index == half)
		dir.y -= 0.8f - m_timer * 0.16f;
	else if (index == half - 6 || index == half - 2 ||
		index == m_flyByPos.size() - 3)
		dir.y -= m_timer * 0.16f;

	if (dir.y < -0.9f)
		dir.y = -0.9f;
	else if (dir.y > -0.1f)
		dir.y = -0.1f;

	g_camera = MakeCameraMatrix(pos, pos + dir);
}

unsigned char CGolfRule::GetFirstTurn()
{
	if (OnlinePlay())
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
			m_turn = PLAYER(GOLFDOC()->m_currentPlayer)->team;

		return GOLFDOC()->m_currentPlayer;
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		return TEAM(m_turn)->teeShooter + m_turn * 2;
	}

	unsigned char i;
	for (i = 0; i < 4; i++)
	{
		if (PLAYER(i)->teeOrder == 0)
			break;
	}

	return i;
}

unsigned char CGolfRule::GetNextTurn()
{
	if (OnlinePlay())
		return m_nextPlayer;

	unsigned char next = 0;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		if (HaveTeeShots())
		{
			next = TEAM(1 - m_turn)->teeShooter + (1 - m_turn) * 2;
		}
		else
		{
			WVector dist[2];
			dist[0] = GetHoleCupPos() - TEAM(0)->pos;
			dist[0].y = 0;
			dist[1] = GetHoleCupPos() - TEAM(1)->pos;
			dist[1].y = 0;

			int team = dist[0].Magnitude() > dist[1].Magnitude() ? 0 : 1;

			TEAM(team)->turn = 1 - TEAM(team)->turn;
			next = team * 2 + TEAM(team)->turn;
		}
	}
	else
	{
		if (GOLFDOC()->m_playerNum == 1)
			return 0;

		if (HaveTeeShots())
		{
			unsigned char order = PLAYER(GOLFDOC()->m_currentPlayer)->teeOrder;

			for (; next < GOLFDOC()->m_playerNum; next++)
			{
				if (PLAYER(next)->state != 3)
				{
					if (PLAYER(next)->teeOrder == order + 1)
					{
						if (PLAYER(next)->state == 0)
							break;

						order++;
						next = -1;
					}
				}
			}
		}
		else
		{
			for (; next < GOLFDOC()->m_playerNum; next++)
			{
				if (PLAYER(next)->state == 0)
					break;
			}

			if (next == GOLFDOC()->m_playerNum)
				return GOLFDOC()->m_currentPlayer;

			WVector diff = GetHoleCupPos() - PLAYER(next)->pos;
			float maxDist = sqrtf(diff.x * diff.x + diff.z * diff.z);

			for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; i++)
			{
				if (PLAYER(i)->state == 0 && i != next)
				{
					diff = GetHoleCupPos() - PLAYER(i)->pos;
					float dist = sqrtf(diff.x * diff.x + diff.z * diff.z);
					if (dist > maxDist)
					{
						maxDist = dist;
						next = i;
					}
				}
			}
		}
	}

	return next;
}

void CGolfRule::AllReplay()
{
	m_shotFlag &= ~4;
	Replay(false);
}

void CGolfRule::HoleReplay()
{
	CGolfBall& ball = GolfBall();

	ball.m_playFrame = ball.m_renderNum - 50;
	ball.m_pos = ball.PreData(ball.m_playFrame)->pos;
	ball.m_vel = ball.PreData(ball.m_playFrame)->vel;
	ball.m_state = ball.PreData(ball.m_playFrame)->state;
	GetPVS().SetFrame(GetPVS().GetFrame() - 50);

	WVector dir;
	dir = GetHoleCupPos() - ball.PreData(0)->pos;
	dir.y = 0;
	dir.Normalize();

	WVector pos;
	switch (m_replayCamera)
	{
	case 1:
		pos = GetHoleCupPos() + dir * 16.0f * RotMat(g_PI / 4, 1);
		pos.y += 9.6f;
		g_camera = MakeCameraMatrix(pos, GetHoleCupPos());
		CCameraWork::Instance()->SetCamera(g_camera);
		break;
	case 2:
		pos = GetHoleCupPos() + dir * 9.6f * RotMat(-g_PI / 4, 1);
		g_camera = MakeCameraMatrix(pos, GetHoleCupPos());
		g_camera.pivot.y += 1.0f;
		CCameraWork::Instance()->SetCamera(g_camera);
		break;
	}
}

void CGolfRule::SwingReplay()
{
	CGolfBall& ball = GolfBall();
	const CGolfBall::sExpected* data = ball.PreData(0);

	SendMsg(this, "Effect", 375, 0, 0, 0, 0);
	SendMsg(this, "Effect", 376, 0, 0, 0, 0);
	SendMsg(this, "Trajectory", 396, 0, 0, 0, 0);
	CFx::Instance()->ClearAll(true);

	if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
		0x14000005)
		SendMsg(this, "Ball", 494, 0, 0, 0, 0);

	SendMsg(this, "Player", 271, 0, 0, 0, 0);
	m_bDrivePose = false;
	m_processTime = 1.0f;
	GOLFDOC()->m_gameMode = 0x40;
	GetPVS().SetFrame(m_replayPvsFrame);

	ball.m_playFrame = 0;
	ball.m_pos = data->pos;
	ball.m_vel = data->vel;
	ball.m_state = data->state;

	CCameraWork::Instance()->SetReplayCount(m_replayCamera);
	CCameraWork::Instance()->SelectCamera(206);
}

void CGolfRule::AutoReplay()
{
	m_shotFlag = 0;

	if (IsMassGame())
		return;

	if (GetCurMap() == 7)
		return;

	if (GOLFDOC()->m_tutorialMode < 15 && GOLFDOC()->m_tutorialMode != 8)
		return;

	WVector diff = GetHoleCupPos() - GolfBall().PreData(0)->pos;
	float dist = sqrtf(diff.x * diff.x + diff.z * diff.z);

	if (GolfBall().m_initSpecial & 0x70)
		m_shotFlag |= 2;

	if (GolfClub().PowerShotType() && GOLFDOC()->GetBar().grade == 4 &&
		GOLFDOC()->GetBar().bFullPower)
		m_shotFlag |= 2;

	if (GolfBall().IsHoleIn())
	{
		if (GolfClub().GetType() == 3 && dist > 64.0f)
			m_shotFlag |= 4;

		if (dist > 320.0f)
			m_shotFlag |= 4;

		if (!(m_shotFlag & 4))
		{
			if (dist > 121.6f && dist < 320.0f)
				m_shotFlag |= 1;

			if (GolfBall().m_special & 3)
				m_shotFlag |= 1;
		}
	}

	if (GolfBall().m_bOB)
		m_shotFlag &= ~4;

	char shotType = GolfClub().PowerShotType();

	if (shotType == 2 || shotType == 3 || GolfBall().m_renderNum < 15.000001f)
		m_shotFlag &= ~2;
}

void CGolfRule::SaveShot(const char* filename)
{
	if (m_shotData.pvsFrame == 0)
		return;

	if (filename == NULL)
	{
		mkdir("save");
		int i = 0;
		do
		{
			sprintf(m_fileName, "save/pangya_%03d.sav", i++);
		} while (_access(m_fileName, 0) != -1);
	}
	else
		strcpy(m_fileName, filename);

	FILE* fp = fopen(m_fileName, "wb");
	if (fp == NULL)
		return;

	char header[32] = { 0 };
	SYSTEMTIME time;
	GetLocalTime(&time);
	unsigned char index = IsMassGame() ? 0 : MyIndex();
	CPartTidList& tidList = GOLFDOC()->GetPartTidList(index);
	int playType = Doc()->m_replayPlayType;

	fwrite(header, 32, 1, fp);
	unsigned char map = GetCurMap();
	fwrite(&map, 1, 1, fp);
	fwrite(&time, sizeof(SYSTEMTIME), 1, fp);
	fwrite(&playType, 4, 1, fp);
	fwrite(&Doc()->m_userInfo[index], sizeof(sUserInfo), 1, fp);
	fwrite(&tidList, sizeof(CPartTidList), 1, fp);

	fwrite(GOLFDOC()->GetPlayer(index), sizeof(sPlayerData), 1, fp);
	fwrite(&m_shotData, sizeof(sShotData), 1, fp);
	fwrite(&Doc()->m_courseMap[GetHoleIndex(m_shotData.hole) - 1], 1, 1, fp);
	fwrite(&Doc()->m_curChannel.Type, 4, 1, fp);

	fclose(fp);

	CChatMsg::Instance()->AddChatMsg(
		MakeStr(
			"%s\xb4\xd4\xc0\xc7 %d\xc8\xa6 %d\xb9\xf8\xc2\xb0 \xbc\xa6 \xc1\xa4\xba\xb8\xb0\xa1 '\xb1\xe2\xb7\xcf\xb5\xc8 \xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc\xc5\xd7\xc0\xcc\xc7\xc1'\xbf\xa1 \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			MyNick(), m_shotData.hole, m_shotData.stroke),
		0xffff7878, true, false);
}

void CGolfRule::LoadShot(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		MessageBoxA(NULL,
			MakeStr(
				"\xbc\xbc\xc0\xcc\xba\xea \xc6\xc4\xc0\xcf(%s)\xc0\xbb \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				filename),
			"\xb0\xe6\xb0\xed", MB_OK);
		g_bQuit = true;
		return;
	}

	char header[32] = { 0 };
	unsigned char map = 0;
	SYSTEMTIME time;
	CPartTidList tidList;
	int playType = 0;

	fread(header, 32, 1, fp);
	fread(&map, 1, 1, fp);
	fread(&time, sizeof(SYSTEMTIME), 1, fp);
	fread(&playType, 4, 1, fp);
	Doc()->m_replayPlayType = playType;
	fread(&Doc()->m_userInfo[0], sizeof(sUserInfo), 1, fp);
	fread(&tidList, sizeof(CPartTidList), 1, fp);

	fread(GOLFDOC()->GetPlayer(0), sizeof(sPlayerData), 1, fp);
	fread(&m_shotData, sizeof(sShotData), 1, fp);
	fread(&Doc()->m_courseMap[GetHoleIndex(m_shotData.hole) - 1], 1, 1, fp);
	fread(&Doc()->m_curChannel.Type, 4, 1, fp);

	fclose(fp);

	SetCurMap(map);
	GOLFDOC()->m_currentHole = m_shotData.hole;
	Doc()->m_holeOrder[0] = m_shotData.hole;
	GolfClub().SetClub(m_shotData.club);
	GOLFDOC()->SetPartTidList(0, tidList.m_charTid, tidList.m_hairColor,
		tidList.m_shirtsColor, tidList.m_tid, tidList.m_uid);

	SetGameType(GAME_TYPE_STROKE);

	Doc()->m_replayPosition = m_shotData.bar.gaugeStep;

	Doc()->m_replayPlayType = playType;

	if (IsLocalContent(S4_UCC))
	{
		RequestUccInfo(Doc()->m_userInfo[0]);
	}
}

int CGolfRule::ShotDataSave(const char* filename, bool bAppend)
{
	if (m_shotData.pvsFrame == 0)
		return 0;

	if (filename == NULL)
	{
		mkdir("save");

		int i = 0;
		do
		{
			if (i >= 100)
				return -1;

			sprintf(m_fileName, "save/pangya_%03d.sav", i++);
		} while (_access(m_fileName, 0) != -1);
	}
	else
		strcpy(m_fileName, filename);

	FILE* fp = fopen(m_fileName, "wb");
	if (fp == NULL)
		return 0;

	unsigned char map = GetCurMap();
	SYSTEMTIME time;
	GetLocalTime(&time);
	unsigned char index = IsMassGame() ? 0 : MyIndex();
	CPartTidList& tidList = GOLFDOC()->GetPartTidList(index);
	int shotCount = Doc()->m_shotDataList.size();
	int playType = Doc()->m_replayPlayType;
	unsigned char course =
		Doc()->m_courseMap[GetHoleIndex(GOLFDOC()->m_currentHole) - 1];

	unsigned long uid = 0xffffffff;
	if (!bAppend)
	{
		uid = MyUID();
	}

	fwrite(&Doc()->m_replayVersion, 4, 1, fp);
	fwrite(&uid, 4, 1, fp);
	fwrite(&map, 1, 1, fp);
	fwrite(&time, sizeof(SYSTEMTIME), 1, fp);
	fwrite(&playType, 4, 1, fp);

	if (bAppend)
	{
		char id[22];
		char nick[22];
		strcpy(id, Doc()->m_userInfo[index].info.sID);
		strcpy(nick, Doc()->m_userInfo[index].info.sNick);
		unsigned long userUid = Doc()->m_userInfo[index].info.dwUID;
		sprintf(Doc()->m_userInfo[index].info.sID, "NOID");
		sprintf(Doc()->m_userInfo[index].info.sNick, "NONICKNAME");
		Doc()->m_userInfo[index].info.dwUID = 0xffffffff;

		fwrite(&Doc()->m_userInfo[index], sizeof(sUserInfo), 1, fp);

		strcpy(Doc()->m_userInfo[index].info.sID, id);
		strcpy(Doc()->m_userInfo[index].info.sNick, nick);
		Doc()->m_userInfo[index].info.dwUID = userUid;
	}
	else
	{
		fwrite(&Doc()->m_userInfo[index], sizeof(sUserInfo), 1, fp);
	}

	fwrite(&tidList, sizeof(CPartTidList), 1, fp);
	fwrite(&course, 1, 1, fp);
	fwrite(GOLFDOC()->GetPlayer(index), sizeof(sPlayerData), 1, fp);

	fwrite(&Doc()->m_curChannel.Type, 4, 1, fp);

	if (playType == 1)
	{
		fwrite(&m_shotData, sizeof(sShotData), 1, fp);
	}
	else if (playType == 2)
	{
		fwrite(&shotCount, 4, 1, fp);

		for (SHOTDATA_LIST_IT it = Doc()->m_shotDataList.begin();
			it != Doc()->m_shotDataList.end(); it++)
		{
			fwrite(&(*it), sizeof(sShotData), 1, fp);
		}

		Doc()->m_shotDataList.clear();
	}

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		int cardCount = Doc()->m_cardAbilityList[0].size();
		fwrite(&cardCount, 4, 1, fp);
		if (cardCount > 0)
		{
			for (CARDABILITY_LIST_IT it = Doc()->m_cardAbilityList[0].begin();
				it != Doc()->m_cardAbilityList[0].end(); it++)
			{
				fwrite(&(*it), sizeof(sSCardAvilityPeriodInfo), 1, fp);
			}
		}
	}

	fclose(fp);

	return 1;
}

bool CGolfRule::ShotDataLoad(const char* filename)
{
	int len = strlen(filename);
	FILE* fp;
	if (len == 0)
		fp = fopen("D:/work/ProjectG_S4/bin/save/pangya_003.sav", "rb");
	else
		fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		g_bQuit = true;
		return false;
	}

	unsigned long version = 0;
	unsigned char map = 0;
	SYSTEMTIME time;
	CPartTidList tidList;
	int shotCount = 0;
	unsigned char course = 0;
	int playType = 0;
	unsigned long uid = 0xffffffff;
	sShotData shotData;

	memset(&m_shotData, 0, sizeof(sShotData));
	Doc()->m_shotDataList.clear();
	Doc()->m_replayShotIndex = 0;

	fread(&version, 4, 1, fp);
	fread(&uid, 4, 1, fp);
	fread(&map, 1, 1, fp);
	fread(&time, sizeof(SYSTEMTIME), 1, fp);
	fread(&playType, 4, 1, fp);
	Doc()->m_replayPlayType = playType;

	fread(&Doc()->m_userInfo[0], sizeof(sUserInfo), 1, fp);
	fread(&tidList, sizeof(CPartTidList), 1, fp);
	fread(&course, 1, 1, fp);
	fread(GOLFDOC()->GetPlayer(0), sizeof(sPlayerData), 1, fp);

	fread(&Doc()->m_curChannel.Type, 4, 1, fp);

	if (playType == 1)
	{
		fread(&m_shotData, sizeof(sShotData), 1, fp);
		Doc()->m_replayShotIndex = m_shotData.holeStroke;
	}
	else if (playType == 2)
	{
		fread(&shotCount, 4, 1, fp);
		for (int i = 0; i < shotCount; i++)
		{
			fread(&shotData, sizeof(sShotData), 1, fp);
			if (i == 0)
			{
				memcpy(&m_shotData, &shotData, sizeof(sShotData));
				Doc()->m_replayShotIndex = 0;
				Doc()->m_replayHoleStroke = m_shotData.holeStroke;
				GOLFDOC()->GetPlayer(0)->gauge = m_shotData.gauge;
				GetPVS().SetFrame(m_shotData.pvsFrame);
			}

			Doc()->m_shotDataList.push_back(shotData);
			memset(&shotData, 0, sizeof(sShotData));
		}
	}

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		int cardCount = 0;
		fread(&cardCount, 4, 1, fp);
		if (cardCount > 0)
		{
			Doc()->m_cardAbilityList[2].clear();

			for (int j = 0; j < cardCount; j++)
			{
				sSCardAvilityPeriodInfo info;
				fread(&info, sizeof(sSCardAvilityPeriodInfo), 1, fp);
				Doc()->m_cardAbilityList[2].push_back(info);
			}
		}

		CCardManager::Instance()->m_charInfo[2] = Doc()->m_userInfo[0].charInfo;
		CCardManager::Instance()->CalcCardPeriodAndStatus();
		CCardManager::Instance()->SetPlayerIndex(2);
	}

	fclose(fp);

	SetCurMap(map);
	Doc()->m_holeOrder[0] = m_shotData.hole;
	Doc()->m_courseMap[GetHoleIndex(m_shotData.hole) - 1] = course;
	GOLFDOC()->m_currentHole = m_shotData.hole;

	GOLFDOC()->SetPartTidList(0, tidList.m_charTid, tidList.m_hairColor,
		tidList.m_shirtsColor, tidList.m_tid, tidList.m_uid);

	SetGameType(GAME_TYPE_STROKE);

	Doc()->m_replayPosition = m_shotData.bar.gaugeStep;

	Doc()->m_replayPlayType = playType;

	if (IsLocalContent(S4_UCC))
	{
		RequestUccInfo(Doc()->m_userInfo[0]);
	}

	return true;
}

void CGolfRule::SaveGame(int part)
{
	if (part == 0)
	{
		mkdir("save");
		int i = 0;
		do
		{
			sprintf(m_fileName, "save/save_%03d.dat", i++);
		} while (_access(m_fileName, 0) != -1);
	}

	FILE* fp = fopen(m_fileName, "ab");
	if (fp == NULL)
		return;

	switch (part)
	{
	case 0:
	{
		unsigned char map = GetCurMap();
		fwrite(&map, 1, 1, fp);

		int hole = GetStartHole();
		fwrite(&hole, 4, 1, fp);

		unsigned char playerNum = GetPlayerNum();
		fwrite(&playerNum, 1, 1, fp);
		fwrite(PLAYER(0), sizeof(sPlayerData), playerNum, fp);
		break;
	}
	case 1:
		fwrite(&Wind().GetGlobalWind(), sizeof(WVector), 1, fp);
		break;
	case 2:
		fwrite(&m_shotData, sizeof(sShotData), 1, fp);
		break;
	}

	fclose(fp);
}

void CGolfRule::LoadGame(int part)
{
	static long offset = 0;

	FILE* fp = fopen("save/save_000.dat", "rb");
	if (fp == NULL)
	{
		g_bQuit = true;
		return;
	}

	fseek(fp, offset, SEEK_SET);

	switch (part)
	{
	case 0:
	{
		unsigned char map;
		fread(&map, 1, 1, fp);
		SetCurMap(map);

		unsigned char playerNum = GetPlayerNum();
		fread(&GOLFDOC()->m_currentHole, 1, 1, fp);
		fread(&playerNum, 1, 1, fp);
		fread(PLAYER(0), sizeof(sPlayerData), playerNum, fp);
		break;
	}
	case 1:
		fread(&m_wind, sizeof(WVector), 1, fp);
		Wind().SetWind(m_wind);
		break;
	case 2:
		fread(&m_shotData, sizeof(sShotData), 1, fp);
		break;
	}

	offset = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);

	fclose(fp);
}

void CGolfRule::QuickSave(const char* filename)
{
	FILE* fp = fopen(MakeStr("%s.sav", filename), "wb");

	unsigned char map = GetCurMap();
	fwrite(&map, 1, 1, fp);
	fwrite(&GOLFDOC()->m_currentHole, 1, 1, fp);

	unsigned char playerNum = GetPlayerNum();
	fwrite(&playerNum, 1, 1, fp);
	fwrite(PLAYER(0), sizeof(sPlayerData), playerNum, fp);
	fwrite(&GOLFDOC()->m_currentPlayer, 1, 1, fp);

	fclose(fp);
}

void CGolfRule::QuickLoad(char* filename)
{
	FILE* fp = fopen(MakeStr("%s.sav", filename), "rb");
	if (fp == NULL)
		return;

	unsigned char map;
	fread(&map, 1, 1, fp);
	unsigned char hole;
	fread(&hole, 1, 1, fp);

	unsigned char playerNum = GetPlayerNum();
	fread(&playerNum, 1, 1, fp);
	fread(PLAYER(0), sizeof(sPlayerData), playerNum, fp);
	fread(&GOLFDOC()->m_currentPlayer, 1, 1, fp);

	fclose(fp);

	if (map != GetCurMap())
	{
		SetCurMap(map);
		GOLFDOC()->m_currentHole = hole;
		SendMsg(this, "GolfBg", 320, 0, 0, 0, 0);
		SendMsg(this, "GolfBg", 318, 0, 0, 0, 0);
	}
	else if (hole != GOLFDOC()->m_currentHole)
	{
		GOLFDOC()->m_currentHole = hole;
		SendMsg(this, "GolfBg", 320, 0, 0, 0, 0);
	}

	SendMsg(this, "Player", 278, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 278, 0, 0, 0, 0);

	SetEnvironmentVariableA();
	GOLFDOC()->MakeDefaultCamera(true);
	ChangeGameMode(8);

	delete[] filename;
}

void CGolfRule::HitShot()
{
	if (Doc()->GetPlayingMode() != 2)
	{
		if (!IsMyTurn(false))
			return;
	}

	timeEndPeriod(1);
	CFx::Instance()->SetActive(true);

	if (IsLocalContent(S4_SPECIALSHOT_RESTRICTION_MODIFY))
	{
		if (GolfBall().m_initSpecial > 0 && GOLFDOC()->GetBar().grade != 4 &&
			GolfClub().GetType() != 3)
		{
			float curve = (float)(rand() % 3 + 2);
			if (rand() % 2 == 0)
				curve *= -1.0f;
			GolfBall().m_initSpin = curve * 0.1f;

			float spin = (float)(rand() % 3 + 2);
			if (rand() % 2 == 0)
				spin *= -1.0f;
			GolfBall().m_initCurve = spin * 0.1f;

			SendMsg(this, "Spin", 357, 0, (int)GolfBall().m_initCurve,
				(int)GolfBall().m_initSpin, 0);
		}
	}

	if (Doc()->GetPlayingMode() == 1)
	{
		if (OnlinePlay())
		{
			short type = 4;
			int gameMode = Doc()->m_gameMode;
			if (GOLFDOC()->m_tutorialMode < 15)
				gameMode = 6;

			if (GolfClub().PowerShotType() && GolfBall().m_initSpecial)
			{
				if ((GolfBall().m_initSpecial & 0x70) &&
					GOLFDOC()->GetBar().grade == 4)
				{
					if (GolfClub().PowerShotType() == 2 ||
						(GolfClub().PowerShotType() == 3 &&
							GolfClub().GetType() == 0))
						type = 2;
					else
						type = 1;
				}

				if (Doc()->m_cutinList.size() > 0)
				{
					for (ITEM_LIST_IT it = Doc()->m_cutinList.begin();
						it != Doc()->m_cutinList.end(); it++)
					{
						if (S5::ISCutinItem((*it).tid))
						{
							IFF_STRUCT::S5::sCutinInformation* pInfo =
								ItemManager()->FindCutinInformation((*it).tid);
							if (pInfo && (pInfo->Condition & type))
							{
								S5::UTIL::SendCutInPacket(
									Doc()->m_myInfo.info.dwUID, type, gameMode,
									Doc()
										->m_userInfo[GOLFDOC()->m_currentPlayer]
										.charInfo.tid);
								goto CUTIN_SENT;
							}
						}
					}
				}

				S5::UTIL::SendCutInPacket(Doc()->m_myInfo.info.dwUID, 0,
					gameMode,
					Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tid);
			}
			else
				S5::UTIL::SendCutInPacket(Doc()->m_myInfo.info.dwUID, 0,
					gameMode,
					Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tid);
		}
	}
CUTIN_SENT:

	if (!OnlinePlay())
	{
		WSendPacket packet((enumClientPacket)0);
		SendMsg(this, "GroundItemMan", 632, (int)&packet, 0, 0, 0);
	}

	if (OnlinePlay() && Doc()->GetPlayingMode() == 1)
	{
		sShotProtocol shot;
		shot.curve = GolfBall().m_initCurve;
		shot.spin = GolfBall().m_initSpin;
		shot.speical = GolfBall().m_initSpecial;
		shot.minClick = GOLFDOC()->GetBar().impact;
		shot.maxClick = GOLFDOC()->GetBar().power;
		shot.accuracy = GOLFDOC()->GetBar().grade;
		shot.center = GOLFDOC()->GetBar().center;

		if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
		{
			shot.shotTime =
				Doc()->m_approachStartTime - g_CurrentTime + GetGameTimeLimit();
		}
		else
		{
			shot.shotTime = GOLFDOC()->m_shotTime;
		}

		shot.frame = GetPVS().GetFrame();
		SendMsg(this, "Player", 272, (int)&shot.angle, 0, 0, 0);
		shot.clubIndex = GolfClub().GetIndex();
		shot.randomPower = GolfClub().GetRandomPower();
		GolfClub().SetCalcRandomPower(false);
		shot.randomAngle = m_shotSync;

		if (IsLocalContent(S4_HACKINGTOOL))
		{
			sItemInfo* pBall = NULL;
			for (ITEM_LIST_IT it = Doc()->m_ballList.begin();
				it != Doc()->m_ballList.end(); it++)
			{
				if ((*it).tid == Doc()->m_myInfo.userEquip.tidBall)
				{
					pBall = &(*it);
					break;
				}
			}

			if (pBall == NULL)
			{
				Doc()->m_myInfo.userEquip.tidBall = 0x14000000;
			}
		}

		WSendPacket packet(0x12);
		AddSomeInfo(packet);
		packet.EncodeBuffer(&shot, sizeof(sShotProtocol));
		SendMsg(this, "GroundItemMan", 632, (int)&packet, 0, 0, 0);
		packet.Send(TO_GAME);

		if (OnlinePlay())
			SendMsg(this, "Cutin", 120, 0, 0, 0, 0);

		if (GOLFDOC()->m_commandList.size() > 0)
		{
			WSendPacket command((enumClientPacket)0x42);
			command.Encode1(GOLFDOC()->m_commandList.size());
			for (std::list<unsigned long>::iterator it =
					 GOLFDOC()->m_commandList.begin();
				it != GOLFDOC()->m_commandList.end(); it++)
				command.Encode4(*it);
			command.Send(TO_GAME);

			if (OnlinePlay())
				GetActor("Cutin") << MsgObject(NULL, 120, 8, 0, 0, 0, 0);
		}

		if (OnlinePlay())
		{
			if (IsMassGame())
			{
				SendMsg(this, "Cutin", 120, 16, 0, 0, 0);
				SetNetMsgWait(true, 14966, "modeDriveMotion_MassGame", 20.0f);
				ChangeGameMode(0x40);
			}
			else
				SetNetMsgWait(true, 14970, "HitShot", 20.0f);
		}
		else
		{
			if (IsMassGame())
				ChangeGameMode(0x40);
			else
				SetNetMsgWait(true, 14977, "HitShot", 20.0f);
		}
	}
	else
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
		{
			Doc()->m_rivalList[MyIndex()].approachTime =
				Doc()->m_approachStartTime - g_CurrentTime + GetGameTimeLimit();
		}

		if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->GetPlayingMode() == 2)
		{
			if (Doc()->GetReplayState() != 7)
				Doc()->SetReplayState(3);
			GolfBall().m_initCurve = m_shotData.curve;
			GolfBall().m_initSpin = m_shotData.spin;
			SendMsg(this, "Spin", 357, 0, (int)GolfBall().m_initCurve,
				(int)GolfBall().m_initSpin, 0);
		}

		if (OnlinePlay())
		{
			if (GOLFDOC()->m_tutorialMode == 15 && Doc()->GetPlayingMode() != 2)
			{
				SendMsg(this, "Cutin", 120, 16, 0, 0, 0);
				SetNetMsgWait(true, 15001, "modeDriveMotion_MassGame", 20.0f);
			}
		}

		ChangeGameMode(0x40);
	}

	if (m_pChatView)
		m_pChatView->SetVisible(true);
}

void CGolfRule::ReactionForShot()
{
	SendMsg(this, "Caddie", 470, 0, 0, 0, 0);

	WVector diff = GolfBall().PreData(GolfBall().m_renderNum)->pos -
		GolfBall().PreData(0)->pos;

	if (!GolfBall().m_bOB && sqrtf(diff.z * diff.z + diff.x * diff.x) > 416.0f)
		SendMsg(this, "Caddie", 471, 0, 0, 0, 0);
	else
		CCameraWork::Instance()->SetCaddieChase(false);
}

void CGolfRule::ChangeTurn()
{
	SendMsg(this, "Spin", 10010, 0, 0, 0, 0);
	SendMsg(this, "Grid", 355, 0, 0, 0, 0);
	SendMsg(this, "Ball", 355, 0, 0, 0, 0);
	SendMsg(this, "Ball", 491, 0, 0, 0, 0);
	SendMsg(this, "Ball", 493, 0, 0, 0, 0);
	SendMsg(this, "Screen", 416, 0, 0, 0, 0);
	SendMsg(this, "Item", 416, 0, 0, 0, 0);
	SendMsg(this, "Ball", 416, 0, 0, 0, 0);
	SendMsg(this, "Score", 416, 0, 0, 0, 0);
	SendMsg(this, "GuideLine", 416, 0, 0, 0, 0);
	SendMsg(this, "NPC", 386, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 19, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 483,
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].caddieInfo.tid,
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].caddieInfo.tidPart, 0, 0);

	GOLFDOC()->m_shotPhase = 0;
	GOLFDOC()->GetBar().Reset();
	GOLFDOC()->GetBar().gauge =
		GolfClub().GetType() == 3 ? s_barStart : s_barGaugeStart;
	GOLFDOC()->GetBar().gaugeStep = GOLFDOC()->GetBar().gauge;

	GolfBall().SetHoleIn(FALSE);

	GetPVS().CalcGreenPlane();
	GolfClub().SetAccuracy();

	PlayBGM();

	GOLFDOC()->m_bReplay = false;
}

void CGolfRule::GetWinningPrize(int rank)
{
	AddPrize(rank);

	AddExtPrize();
}

void CGolfRule::AddPrize(int prize)
{
	unsigned long typeId;
	switch (prize)
	{
	case -3:
		typeId = PLAYER(GOLFDOC()->m_currentPlayer)->prize.albatross;
		break;
	case -2:
		typeId = PLAYER(GOLFDOC()->m_currentPlayer)->prize.eagle;
		break;
	case -1:
		typeId = PLAYER(GOLFDOC()->m_currentPlayer)->prize.birdie;
		break;
	case 0:
		typeId = PLAYER(GOLFDOC()->m_currentPlayer)->prize.par;
		break;
	case 1:
		typeId = PLAYER(GOLFDOC()->m_currentPlayer)->prize.bogey;
		break;
	default:
		typeId = 0;
		break;
	}

	if (GOLFDOC()->GetHoleData().par + prize == 1)
		typeId = PLAYER(GOLFDOC()->m_currentPlayer)->prize.holeInOne;

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
	if (pItem)
	{
		SendMsg(this, "Caddie", 478, 35, 0, 0, 0);

		CFxSpray* pSpray = CFx::Instance()->OpenSpray("prize.spr");
		if (pSpray)
		{
			WVector pos = GolfBall().m_pos;
			pSpray->Pos() = pos;
		}
	}

	for (int i = 0; i < 6; i++)
		((unsigned long*)&PLAYER(GOLFDOC()->m_currentPlayer)->prize)[i] = 0;
}

void CGolfRule::AddExtPrize()
{
	bool bEffect = true;

	PLAYER(GOLFDOC()->m_currentPlayer);

	PLAYER(GOLFDOC()->m_currentPlayer);

	for (int i = 0; i < PLAYER(GOLFDOC()->m_currentPlayer)->extPrizeNum; i++)
	{
		int type = PLAYER(GOLFDOC()->m_currentPlayer)->extPrize[i].type;
		// HACK!
		IsNoEffectPrize(type);
		if (type >= 3 && type <= 5)
			continue;

		unsigned long typeId =
			PLAYER(GOLFDOC()->m_currentPlayer)->extPrize[i].typeId;

		if (typeId == 0)
			continue;

		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
		if (pItem == NULL)
			continue;

		const char* effect = NULL;
		switch (typeId & 0xfc000000)
		{
		case 0x74000000:
		{
			IFF_STRUCT::sQuestDrop* pDrop = ItemManager()->FindQuestDrop(
				PLAYER(GOLFDOC()->m_currentPlayer)->extPrize[i].typeId);
			if (pDrop == NULL)
				continue;
			effect = pDrop->dropFx;
			if (effect == NULL)
				continue;
		}
		break;
		default:
			effect = "thanxgiving.seq";
			if (typeId == 0x1a0000aa)
			{
				effect = "coin_medal01.seq";
			}
			else if (typeId == 0x1a0000ab)
			{
				effect = "coin_medal02.seq";
			}
			else if (typeId == 0x1a0000ac)
			{
				effect = "coin_medal03.seq";
			}
			else if (typeId == 0x1a000010)
			{
				int count =
					PLAYER(GOLFDOC()->m_currentPlayer)->extPrize[i].quantity;
				if (count <= 10)
					count = 10;
				else
					count = count <= 100 ? 20 : 30;

				SendMsg(this, "Effect", 381, count, 0, 0, 0);

				bEffect = false;
			}
			else if (typeId == 0x1a000143)
			{
				effect = "4god_Bluedragon.seq";
				bEffect = false;
			}
			else if (typeId == 0x1a000144)
			{
				effect = "4god_tiger.seq";
				bEffect = false;
			}
			else if (typeId == 0x1a000145)
			{
				effect = "4god_FireBird.seq";
				bEffect = false;
			}
			else if (typeId == 0x1a000146)
			{
				effect = "4god_Turtle.seq";
				bEffect = false;
			}
			break;
		}

		char* ext = strrchr(effect, '.');
		if (ext && strlen(ext) >= 4)
		{
			GolfBall();

			if (stricmp(ext + 1, "spr") == 0)
			{
				CFxSpray* pSpray = CFx::Instance()->OpenSpray(effect);
				if (pSpray)
					pSpray->Pos() = GetHoleCupPos();
			}
			else
			{
				CFxSequence* pSeq = CFx::Instance()->OpenSequence(effect, true);
				if (pSeq)
					pSeq->Pos() = GetHoleCupPos();
			}

			if (bEffect)
			{
				char name[128] = "xmas_prize.spr";

				CCodeTinker tinker;
				const char* dropName = tinker.GetHoleDropEffectName(typeId);
				if (dropName)
					strcpy(name, dropName);

				CFxSpray* pSpray = CFx::Instance()->OpenSpray(name);
				if (pSpray)
				{
					WVector pos = GolfBall().m_pos;
					pSpray->Pos() = pos;
				}
			}

			SendMsg(this, "Caddie", 478, 35, 0, 0, 0);

			if (IsMyTurn(true))
			{
				char text[128] = { 0 };

				if (typeId == 0x1a000010)
					sprintf(text, "%d %s",
						PLAYER(GOLFDOC()->m_currentPlayer)
							->extPrize[i]
							.quantity,
						pItem->Name);
				else
					sprintf(text, "%s", pItem->Name);

				CChatMsg::Instance()->AddChatMsg(
					MakeStr("\xc3\xe0! %s \xc8\xb9\xb5\xe6", text), 0xffff7878,
					false, false);
			}
		}
	}

	PLAYER(GOLFDOC()->m_currentPlayer)->extPrizeNum = 0;
}

void CGolfRule::CancelShot()
{
	timeEndPeriod(1);
	CFx::Instance()->SetActive(true);

	if (OnlinePlay() && IsMyTurn(false))
	{
		CGolfDoc::sGaugeInfo info = { 0, GOLFDOC()->m_bar.center };

		WSendPacket packet((enumClientPacket)0x14);
		packet.EncodeBuffer(&info, sizeof(info));
		packet.Send(TO_GAME);
	}

	m_bRemoteClick = false;
	GOLFDOC()->m_shotPhase = 0;
	GOLFDOC()->m_bar.gauge =
		GolfClub().GetType() == 3 ? s_barStart : s_barGaugeStart;
	GOLFDOC()->m_bar.gaugeStep = GOLFDOC()->m_bar.gauge;
	GOLFDOC()->m_bar.power = s_barStart;
	GOLFDOC()->m_bar.dir = 0;
	GOLFDOC()->m_bar.bFullPower = false;

	CMouseCursor::Instance()->SetActive(true);

	if (CCapturedBg::Instance())
		CCapturedBg::Instance()->Reset();

	SendMsg(this, "Camera", 410, 0, 0, 0, 0);
	GOLFDOC()->m_gameMode = 8;

	if (m_pChatView)
		m_pChatView->SetVisible(true);
}

void CGolfRule::SaveErrorFile()
{
	if (IsMyTurn(false))
	{
		mkdir("error");
		int i = 0;
		do
		{
			sprintf(m_fileName, "error/%s_%03d.rep",
				GetMyComputerName().c_str(), i++);
		} while (_access(m_fileName, 0) != -1);

		SaveShot(m_fileName);
	}
}

void CGolfRule::ReturnLobby(bool bForce)
{
	if (Doc()->m_golfGame.gameType != GAME_TYPE_NEW_APPROACH)
	{
		SetGameOver(true);

		if (CTHunter::IsInstantiated())
			CTHunter::Instance()->SetReturnLobby(true);
	}

	if (OnlinePlay())
	{
		if (m_pCurrentRule)
		{
			if (IsMassGame())
				Doc()->m_holeStatistics[0].dwShotTime =
					PLAYER(0)->shotTime / 1000;

			CProjectG::Instance()->FromGolfToLobbySettings();
		}

		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);

		if (bForce)
		{
			PostMsg(this, "Lobby", 0, (int)"ROOMLIST", 0, 0, 0);
		}
		else
		{
			if (IsMassGame())
			{
				if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
					PostMsg(this, "Lobby", 0, (int)"GAMEROOM_EXT", 0, 0, 0);
				else
					PostMsg(this, "Lobby", 0, (int)"GAMEROOM_EXTRES", 0, 0, 0);
			}
			else
				PostMsg(this, "Lobby", 0, (int)"GAMEROOM", 0, 0, 0);

			PostMsg(this, "Lobby", 47, 0, 0, 0, 0);
			PostMsg(this, "Lobby", 15, 0, 0, 0, 0);
			PostMsg(this, "Lobby", 14, 0, 0, 0, 0);

			if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
			{
				if (Doc()->m_holeStatistics[0].dwExp != -1)
					PostMsg(this, "Lobby", 45, 0, 0, 0, 0);
			}
		}

		PostMsg(this, "Lobby", 26, 0, 0, 0, 0);

		if (CSceneManager::Instance())
			SendMsg(this, "GolfBg", 322, 0, 0, 0, 0);
	}
	else
	{
		if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		{
			g_bQuit = true;
			return;
		}

		if (m_pCurrentRule)
			CProjectG::Instance()->FromGolfToLobbySettings();

		if (Doc()->m_golfGame.gameType == GAME_TYPE_TUTORIAL_BASIC ||
			Doc()->m_golfGame.gameType == GAME_TYPE_TUTORIAL_ADV)
		{
			Doc()->m_gameMode = 0;
			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0,
				(int)"TUTORIAL_MAIN", 0, 0, 0);
		}
		else if (Doc()->m_gameMode == 1)
		{
			CTaskManager::Instance()->ChangeTask("CFamilyTask", "", false);
			PostMsg(this, "Family", 0, (int)"FAMILY", 0, 0, 0);
		}
		else if (Doc()->m_gameMode == 5)
		{
			CTaskManager::Instance()->ChangeTask("CGhostTask", "",
				CTaskManager::Instance()->GetPreservedTask() != NULL);
			CTaskManager::Instance()->PostMsg(NULL, "Ghost", 0, (int)"GHOST", 0,
				0, 0);
		}
	}
}

bool CGolfRule::ReturnToLobbyDlgResult(int result, FrForm* form)
{
	if (IsWebLogin())
	{
		g_bQuit = true;
	}
	else
	{
		if (PLAYER(MyIndex()))
			Doc()->m_myGamePang = PLAYER(MyIndex())->totalPang;
		else
			Doc()->m_myGamePang = 0;

		ReturnLobby(true);
	}

	return true;
}

void CGolfRule::ToggleGameUI()
{
	SendMsg(this, "Screen", 400, m_bShowUI ? 1 : 0, 0, 0, 0);
	SendMsg(this, "Letter", 400, m_bShowUI ? 1 : 0, 0, 0, 0);
	SendMsg(this, "VideoRecorder", 400, m_bShowUI ? 1 : 0, 0, 0, 0);

	if (g_pFresh)
		g_pFresh->Toggle(m_bShowUI);

	CMouseCursor::Instance()->Toggle(m_bShowUI);
	CChatMsg::Instance()->Toggle(m_bShowUI);
}

bool CGolfRule::CheckAllHoleOut()
{
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
	{
		if (PLAYER(i)->state == 0)
			return false;
	}

	return true;
}

bool CGolfRule::PauseDlgResult(int result, FrForm* form)
{
	CMouseCursor::Instance()->Enable(CMouseCursor::AREA_AUXMENU_PAUSE, true);

	m_pPauseDlg = NULL;

	if (OnlinePlay())
	{
		if (result)
		{
			WSendPacket packet((enumClientPacket)0x30);
			packet.Encode1(0);
			packet.Send(TO_GAME);
		}
	}
	else
	{
		GOLFDOC()->SetPause(false);
	}

	return true;
}

bool CGolfRule::ContinueDlgResult(int result, FrForm* form)
{
	m_bModal = false;

	if (result == 1)
	{
		PLAYER(0)->state = 2;
		PLAYER(0)->gauge = 0.0f;
		PLAYER(0)->totalStroke = 0;
		PLAYER(0)->shotTime = 0;
		PLAYER(0)->rank = 1;
		PLAYER(0)->gauge = 0.0f;
		PLAYER(0)->score = 0;
		PLAYER(0)->totalPang = 0;
		PLAYER(0)->bonusPang = 0;

		PLAYER(0)->holeNum = 0;

		for (int i = 0; i < 18; i++)
		{
			PLAYER(0)->stroke[i] = 0;
			PLAYER(0)->putt[i] = 0;
			PLAYER(0)->pang[i] = 0;
		}

		if (Doc()->m_gameMode == 2)
		{
			memcpy(Doc()->m_userInfo[0].userEquip.tidItemSlot,
				Doc()->m_myInfo.userEquip.tidItemSlot,
				sizeof(Doc()->m_userInfo[0].userEquip.tidItemSlot));

			PLAYER(0)->itemNum = Doc()->CollapseItemSlot(0);
		}

		GOLFDOC()->m_currentHole = Doc()->m_holeOrder[0];
		SendMsg(this, "GolfBg", 320, 0, 0, 0, 0);
	}
	else if (result == 2)
	{
		ReturnLobby(false);
	}

	return true;
}

bool CGolfRule::CancelDropDlgResult(int result, FrForm* form)
{
	m_pCancelDropDlg = NULL;

	if (result == 1)
	{
		GOLFDOC()->m_bDropRequest = false;
		ChangeGameMode(8);
	}

	return true;
}

bool CGolfRule::GoStopDlgResult(int result, FrForm* form)
{
	m_pGoStopDlg = NULL;

	if (result)
	{
		WSendPacket packet((enumClientPacket)0x36);
		packet.Encode1(result == 1);
		packet.Send(TO_GAME);

		GOLFDOC()->m_bStopDlg = false;
		m_bSendFinishShot = true;
		SetNetMsgWait(true, 15851, "GoStopDlgResult", 20.0f);
	}

	return true;
}

bool CGolfRule::OnEmoticonResult(int result, FrForm* form)
{
	if (!CChatMsg::Instance()->IsOpened())
		g_pFresh->GetManager()->SetExclusiveKey(false);

	if (m_pEmoticonDlg && result == 1)
	{
		FrEmoticonDlg* pDlg = DYNAMIC_CAST(FrEmoticonDlg, form);
		if (pDlg)
		{
			const char* emoticon = pDlg->GetSelectedIcon();
			if (emoticon)
			{
				if (m_pChatInput && m_pChatInput->IsVisible())
				{
					const char* front = m_pChatInput->GetEditText_Front();
					const char* comp = m_pChatInput->GetEditText_Comp();
					const char* end = m_pChatInput->GetEditText_End();

					if (CChatMsg::Instance()->GetMaskedFont()->GetTextWidth(
							g_view,
							MakeStr("%s%s%s%s", front, comp, emoticon, end)) <
						m_pChatInput->GetWidthLimit())
						m_pChatInput->SetLine(1,
							MakeStr("%s%s%s%s", front, comp, emoticon, end), 0,
							0, 0);

					m_pChatInput->SetKeyFocus(true);
				}
				else
				{
					Doc()->m_chatManager.SendChatMessage(emoticon, NULL, false);

					if (!OnlinePlay())
					{
						std::string msg =
							Doc()
								->m_userInfo[GOLFDOC()->m_currentPlayer]
								.info.sNick;
						msg += " : \x08";
						msg += emoticon;
						CChatMsg::Instance()->AddChatMsg(msg, 0xffffcc00, false,
							false);
					}
				}
			}
		}
	}

	m_pEmoticonDlg = NULL;
	return true;
}

bool CGolfRule::OnReportResult(int result, FrForm* form)
{
	if (result == 1)
	{
		GOLFDOC()->SetReport(true);

		WSendPacket packet((enumClientPacket)0x3a);
		packet.Send(TO_GAME);
	}

	m_pReportDlg = NULL;
	return true;
}

bool CGolfRule::OnReturnLobbyResult(int result, FrForm* form)
{
	ReturnLobby(false);
	return true;
}

bool CGolfRule::OnForgetDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		SendMsg(this, "Item", 414, m_forgetItemSlot, 0, 0, 0);
		SetNetMsgWait(true, 15933, "PM_NET_CANCEL_SHOT", 20.0f);
	}
	else if (result == 2)
	{
		SendShotResult();
		SendMsg(this, "Item", 405, 0, 0, 0, 0);
		SendMsg(this, "Screen", 431, 0, 0, 0, 0);
	}

	m_bForgetDlg = false;
	m_forgetDlgTime = 0.0f;
	m_pForgetDlg = NULL;

	return true;
}

bool CGolfRule::OnPlayerInfoDlgResult(int result, FrForm* form)
{
	m_pPlayerInfoDlg = NULL;

	return true;
}

bool CGolfRule::OnApproachEndDlgResult(int result, FrForm* form)
{
	if (m_pApproachEndDlg == NULL)
		return true;

	if (!Doc()->m_bSentHoleStat)
	{
		Doc()->m_bSentHoleStat = true;
	}

	if (!GOLFDOC()->m_bNetGameEnd)
	{
		SendMsg(this, "Ball", 356, 0, 0, 0, 0);
		SendMsg(this, "Spot", 356, 0, 0, 0, 0);
		SendMsg(this, "Grid", 356, 0, 0, 0, 0);
		SendMsg(this, "Screen", 424, 0, 0, 0, 0);
		SendMsg(this, "Screen", 421, 0, 0, 0, 0);
		SendMsg(this, "Effect", 375, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 396, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 352, 0, 0, 0, 0);

		CFx::Instance()->ClearAll(true);

		ChangeGameMode(0x2000);
	}

	if (CTHunter::IsInstantiated())
	{
		CTHunter::Instance()->SetReturnLobby(true);
	}

	SetGameOver(true);

	m_pApproachEndDlg = NULL;

	return true;
}

bool CGolfRule::OnApproachDlgResult(int result, FrForm* form)
{
	if (m_pApproachDlg == NULL)
		return true;

	if (m_pApproachDlg->GetApproachEnd())
	{
		if (m_pApproachEndDlg == NULL)
		{
			m_pApproachEndDlg = CreateForm<FrApproachEndDlg>(
				g_pFresh->GetManager(), this, "approach_end", NULL);
			m_pApproachEndDlg->EnableDrag(false);

			WRect rect = m_pApproachEndDlg->GetRect();
			float x = g_view->GetWidth() * 0.5f - (rect.w * 0.5f);

			m_pApproachEndDlg->Open(
				(FRESH_PFN_RESULT)&CGolfRule::OnApproachEndDlgResult,
				WPoint(x, 35.0f), 0);
			m_pApproachEndDlg->SetTimeLimit(15.0f);

			m_pApproachEndDlg->SetCaption(
				"\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1 \xc3\xd6\xc1\xbe\xb0\xe1\xb0\xfa");
		}
	}
	else
	{
		ChangeGameMode(0x800000);
	}

	m_pApproachDlg = NULL;

	return true;
}

void CGolfRule::CheckUserBGM(const char* path)
{
	m_bgmList.clear();

	switch (GetCurMap())
	{
	case 4:
	case 5:
		m_bgmList.push_back("shiny.mp3");
		m_bgmList.push_back("bunny.mp3");
		break;

	case 7:
		m_bgmList.push_back("navy_blue.mp3");
		m_bgmList.push_back("rising_sun.mp3");
		break;

	case 8:
		m_bgmList.push_back("happy flight.mp3");
		m_bgmList.push_back("crystal waver.mp3");
		break;

	case 14:
		m_bgmList.push_back("crystal lake.mp3");
		m_bgmList.push_back("fade into white.mp3");
		break;

	case 9:
		m_bgmList.push_back("snowscape.mp3");
		m_bgmList.push_back("winter_ride.mp3");
		break;

	case 10:
		m_bgmList.push_back("somewhere.mp3");
		m_bgmList.push_back("nowhere.mp3");
		break;

	case 13:
		m_bgmList.push_back("volcano.mp3");
		m_bgmList.push_back("vermilion.mp3");
		break;

	case 15:
		m_bgmList.push_back("the mystery of the lost seaway.mp3");
		m_bgmList.push_back("voyage the sky.mp3");
		break;

	case 16:
		m_bgmList.push_back("eastern_valley.mp3");
		m_bgmList.push_back("river.mp3");
		break;

	case 17:
		m_bgmList.push_back("volcano.mp3");
		m_bgmList.push_back("river.mp3");
		break;

	case 19:
		m_bgmList.push_back("SecretWish.mp3");
		m_bgmList.push_back("ADayInTheWizCity.mp3");
		break;

	default:
		m_bgmList.push_back("samba3.mp3");
		m_bgmList.push_back("daydream.mp3");
		m_bgmList.push_back("frog.mp3");
		m_bgmList.push_back("spring.mp3");
		break;
	}
}

bool CGolfRule::CanCancelShot()
{
	for (int i = 0; i < 10; i++)
	{
		if (Doc()->m_userInfo[0].userEquip.tidItemSlot[i] == 0x1800000e)
		{
			m_forgetItemSlot = i;
			return true;
		}
	}

	return false;
}

void CGolfRule::SendShotResult()
{
	if (OnlinePlay())
	{
		WSendPacket packet((enumClientPacket)0x1b);
		sShotResult result;

		CGolfBall& ball = GolfBall();

		result.oid = PLAYER(GOLFDOC()->m_currentPlayer)->oid;

		for (int i = 0; i < 3; i++)
			result.pos[i] = PLAYER(GOLFDOC()->m_currentPlayer)->pos.p[i];

		result.bunker = PLAYER(GOLFDOC()->m_currentPlayer)->bunker;

		if (ball.m_bOB)
			result.result = 3;
		else
		{
			if (ball.IsHoleIn())
			{
				result.result = 4;
			}
			else if (ball.m_standGround)
			{
				result.result = 2;
			}
			else
			{
				result.result = 5;
			}
		}

		result.ruleFlag = (unsigned char)m_shotFlag;

		result.pang = PLAYER(GOLFDOC()->m_currentPlayer)->totalPang;
		result.bonusPang = PLAYER(GOLFDOC()->m_currentPlayer)->bonusPang;

		result.shotFlag = PLAYER(GOLFDOC()->m_currentPlayer)->shotFlag;
		result.traceNum = ball.m_renderNum;

		result.actionFlag = 0;

		if (ball.m_special & 0x10)
			result.actionFlag = 2;
		else if (ball.m_special & 0x40)
			result.actionFlag = 4;
		else if (ball.m_special & 0x20)
			result.actionFlag = 8;

		if (ball.m_special & 1)
			result.actionFlag |= 0x10;
		else if (ball.m_special & 2)
			result.actionFlag |= 0x20;

		if (ball.m_special & 4)
			result.actionFlag |= 0x40;
		else if (ball.m_special & 8)
			result.actionFlag |= 0x80;

		if (IsLocalContent(S3_SHOTRESULT_ENCRYPT))
			CEncryption::Instance()->Encrypt(&result,
				Doc()->m_roomInfo.RoomKey);

		packet.EncodeBuffer(&result, sizeof(result));

		packet.EncodeBuffer(&Doc()->m_roomInfo.RoomKey, sizeof(EncryptKey));

		packet.Send(TO_GAME);
	}
}

void CGolfRule::SendReportError(unsigned char error)
{
	WSendPacket packet(0x33);

	if (m_netMsgSend == NULL)
		m_netMsgSend = "UnknownLock";

	if (m_netMsgRecv == NULL)
		m_netMsgRecv = "UnknownUnlock";

	std::string msg;
	char buf[128];
	msg = m_netMsgSend;
	sprintf(buf, "(%d) / ", m_netMsgSendTime);
	msg += buf;
	msg += m_netMsgRecv;
	sprintf(buf, "(%d)", m_netMsgRecvTime);
	msg += buf;
	sprintf(buf, "/ ET(%d)", GetTickCount() - m_netMsgSendTime);
	msg += buf;

	if (error == 3)
	{
		sprintf(buf, "[Pang Hacker]");
		msg += buf;
	}

	packet.Encode1(error);
	packet.EncodeStr(msg);
	packet.Send(TO_GAME);
}

void CGolfRule::ResetUiPos_Board()
{
	WRect rect = m_pOnelineBtn->GetRect();
	rect.x = g_view->GetWidth() - s_barStart;
	m_pOnelineBtn->SetRect(rect);

	rect = m_pOnelineView->GetRect();
	rect.x = g_view->GetWidth() - 317.0f;
	m_pOnelineView->SetRect(rect);

	COneLineBoard::Instance()->SetRect(m_pOnelineView->GetRect());
}

void CGolfRule::PlayBGM()
{
	if (GolfBall().m_groundType == 2)
		SetConstantBGM(g_bgmPutting[rand() % 2], false, true);
	else
		SetConstantBGM(m_bgmList[m_bgm].c_str(), false, true);
}

int CGolfRule::HoleReplay2()
{
	return 0;
}

void CGolfRule::OnMessengerBtnInit(int id)
{
	m_pMessengerBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pMessengerBtn)
	{
		if (!Doc()->m_bMessengerInit)
		{
			m_pMessengerBtn->EnableBlink(false);
		}

		m_pMessengerBtn->SetVisible(false);
		m_pMessengerBtn->EnableHover(true, 0);

		WPoint pos;
		pos.x = g_view->GetWidth() - 81.0f;
		pos.y = m_pMessengerBtn->GetRect().y;
		m_pMessengerBtn->MoveWindow(pos);

		CMessengerInfo::Instance()->m_alarmRect = m_pMessengerBtn->GetRect();
	}
}

void CGolfRule::OnMessengerBtnUp()
{
	CMessengerInfo::Instance()->Open(true);

	OnBarHoverOff(0);
}

void CGolfRule::OnMessengerBubbleEditInit(int id)
{
	m_pMessengerBubbleEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)id);
	if (m_pMessengerBubbleEdit)
	{
		m_pMessengerBubbleEdit->SetVisible(false);

		WPoint pos;
		pos.x = g_view->GetWidth() - 167.0f;
		pos.y = m_pMessengerBubbleEdit->GetRect().y;
		m_pMessengerBubbleEdit->MoveWindow(pos);
	}
}

void CGolfRule::OnMessenger_AlarmBtnInit(int id)
{
	m_pMessengerAlarmBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pMessengerAlarmBtn)
	{
		m_pMessengerAlarmBtn->SetTopmost(true);

		if (m_pMessengerAlarmBtn && !Doc()->m_bMessengerAlarm)
		{
			m_pMessengerAlarmBtn->SetVisible(false);
			m_pMessengerAlarmBtn->SetTopmost(false);
		}

		WPoint pos;
		pos.x = g_view->GetWidth() - 110.0f;
		pos.y = m_pMessengerAlarmBtn->GetRect().y;
		m_pMessengerAlarmBtn->MoveWindow(pos);
	}
}

void CGolfRule::OnMessenger_AlarmBtnUp()
{
	CMessengerInfo::Instance()->AlarmBtnUp(0xffffffff);
}

void CGolfRule::OnMessenger_GiftAlarmBtnInit(int id)
{
	m_pMessengerGiftAlarmBtn = DYNAMIC_CAST(FrButton, (FrWnd*)id);
	if (m_pMessengerGiftAlarmBtn)
	{
		m_pMessengerGiftAlarmBtn->SetVisible(false);
		m_pMessengerGiftAlarmBtn->SetTopmost(false);

		WPoint pos;
		pos.x = g_view->GetWidth() - 75.0f;
		pos.y = m_pMessengerGiftAlarmBtn->GetRect().y;
		m_pMessengerGiftAlarmBtn->MoveWindow(pos);
	}
}

void CGolfRule::OnMessenger_GiftAlarmBtnUp()
{
	m_pMessengerGiftAlarmBtn->SetVisible(false);
	m_pMessengerGiftAlarmBtn->SetTopmost(false);

	FrGiftAlarmDlg* pDlg =
		CreateForm<FrGiftAlarmDlg>(g_pFresh->GetManager(), this, "gift_alarm");
	pDlg->Open((FRESH_PFN_RESULT)&CGolfRule::OnNotifyDlgResult, 0x11);
	pDlg->SetTopmost(true);
	pDlg->FindNextTopFocus(false);
	pDlg->SetIconRect(m_pMessengerGiftAlarmBtn->GetRect());
}

void CGolfRule::OnReplayStatusInit(int id)
{
	m_pReplayStatus = DYNAMIC_CAST(FrArea, (FrWnd*)id);

	if (!IsLocalContent(S4_REPLAY_SYSTEM))
	{
		m_pReplayStatus->SetVisible(false);
	}

	if (m_pReplayStatus)
	{
		WRect rect = m_pReplayStatus->GetRect();
		rect.x = g_view->GetWidth() - (800.0f - rect.x);
		m_pReplayStatus->SetRect(rect);
	}
}

void CGolfRule::SetCurrentGolfRule()
{
	if (m_pCurrentRule)
	{
		delete m_pCurrentRule;
		m_pCurrentRule = NULL;
	}

	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_TEAM:
	{
		m_pCurrentRule = new CGolfRuleTeam(this);
	}
	break;

	case GAME_TYPE_SKINS:
	{
		m_pCurrentRule = new CGolfRuleSkins(this);
	}
	break;

	case GAME_TYPE_MATCH:
	{
		m_pCurrentRule = new CGolfRuleMatch(this);
	}
	break;

	case GAME_TYPE_STROKE:
	{
		m_pCurrentRule = new CGolfRuleStroke(this);
	}
	break;

	case GAME_TYPE_APPROACH:
	{
		m_pCurrentRule = new CGolfRuleApproach(this);
	}
	break;

	case GAME_TYPE_NEW_APPROACH:
	{
		m_pCurrentRule = new CGolfRuleNewApproach(this);
	}
	break;

	case GAME_TYPE_GUILD_MATCH:
	{
		m_pCurrentRule = new CGolfRuleGuildMatch(this);
	}
	break;

	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	{
		m_pCurrentRule = new CGolfRule30Battle(this);
	}
	break;

	case GAME_TYPE_TUTORIAL_BASIC:
	case GAME_TYPE_TUTORIAL_ADV:
	{
		m_pCurrentRule = new CGolfRuleTutorial(this);
		if (Doc()->m_tutorialStartMode == 14)
			GOLFDOC()->m_currentHole = 2;
		else
			GOLFDOC()->m_currentHole = 1;
	}
	break;

	default:
		m_pCurrentRule = NULL;
		break;
	}
}

bool CGolfRule::IsMaxWithHGParts(unsigned char player)
{
	static unsigned long hgParts[] = {
		0x08104011,
		0x0810a00c,
		0x08100008,
		0x08112005,
	};

	CPartTidList* pList = &GOLFDOC()->GetPartTidList(player);

	if (pList == NULL)
		return false;

	bool bRet = false;
	for (int i = 0; i < sizeof(hgParts) / sizeof(unsigned long); i++)
	{
		if (pList->IsEquipped(hgParts[i], 0))
		{
			bRet = true;
			break;
		}
	}

	return bRet;
}

void CGolfRule::_ImeReset()
{
	if (IsLocalContent((localContentType_t)0x2a))
	{
		if (!CChatMsg::Instance()->IsActive())
		{
			g_ime->SetActive(true);
			g_ime->SetActive(false);
			g_ime->Reset();
		}
	}
}

void CGolfRule::ReplayBoxOpen()
{
	if (m_pReplayDlg == NULL)
	{
		WPoint pos;
		pos.x = 571.0f - (800.0f - g_view->GetWidth());

		m_pReplayDlg = CreateForm<FrReplayControlDlg>(g_pFresh->GetManager(),
			this, "replaycontroldlg");
		m_pReplayDlg->Open((FRESH_PFN_RESULT)&CGolfRule::OnReplayModeDlgResult,
			WPoint(pos.x, 55.0f), 0x11);
	}
	else
	{
		m_pReplayDlg->SetVisible(true);
	}

	if (m_pReplayStatus)
		m_pReplayStatus->SetVisible(false);
}

void CGolfRule::ReplayBoxClose()
{
	if (m_pReplayDlg)
	{
		m_pReplayDlg->Close(true);
	}

	if (m_pReplayStatus)
		m_pReplayStatus->SetVisible(true);
}

bool CGolfRule::OnReplayModeDlgResult(int result, FrForm* form)
{
	if (Doc()->m_playingMode == 2 && Doc()->m_replayState == 5)
	{
		Doc()->m_replayState = 0;
		Doc()->m_rmrMode = 1;
		Doc()->m_replayTargetUID = Doc()->m_myInfo.info.dwUID;
		Doc()->m_playingMode = (eReplayModeType)3;
		SetBGMNone();
		CProjectG::Instance()->FromGolfToLobbySettings();
		CTaskMain::OnUnderBar_MyRoomUp();
		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0xc6, 2, 0, 0, 0);
	}

	m_pReplayDlg = NULL;
	return true;
}

void CGolfRule::ReplayTapeCheckAndUse()
{
	if (GetGameType() == GAME_TYPE_NEW_APPROACH ||
		Doc()->m_roomInfo.realGameType == 14)
	{
		if (CHelpMsg::Instance())
			CHelpMsg::Instance()->SetHelpMsg(0x19, NULL);
		return;
	}

	unsigned long typeId;
	if (IsLocalContent(S4_REPLAY_SYSTEM))
		typeId = Doc()->GetReplayPlayType() == 2 ? 0x1a00004f : 0;
	else
		typeId = 0x1a000000;

	ITEM_LIST_IT it = std::find(Doc()->m_myItemList.begin(),
		Doc()->m_myItemList.end(), typeId);

	Doc()->ReplaySavFileCheck();

	if (it == Doc()->m_myItemList.end())
	{
		if (IsLocalContent(S4_REPLAY_SYSTEM))
		{
			if (typeId == 0x1a00004f)
			{
				Doc()->SetReplayPlayType(0);
				CChatMsg::Instance()->AddChatMsg(
					"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc \xc5\xd7\xc0\xcc\xc7\xc1\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xffff7878, true, false);
			}
			else if (typeId == 0x1a000000)
			{
				Doc()->SetReplayPlayType(0);
				CChatMsg::Instance()->AddChatMsg(
					"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc \xc5\xd7\xc0\xcc\xc7\xc1\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xffff7878, true, false);
			}
		}
		else
		{
			CChatMsg::Instance()->AddChatMsg(
				"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc \xc5\xd7\xc0\xcc\xc7\xc1\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xffff7878, true, false);
		}
		return;
	}

	if (IsLocalContent(S4_REPLAY_SYSTEM) &&
		Doc()->m_recordedItemList.size() >= 10)
	{
		Doc()->SetReplayPlayType(0);
		CChatMsg::Instance()->AddChatMsg(
			"\xb1\xe2\xb7\xcf\xb5\xc8 \xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc \xc5\xd7\xc0\xcc\xc7\xc1\xb8\xa6 10\xb0\xb3 \xb3\xd1\xb0\xd4 \xbb\xfd\xbc\xba \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0xffff7878, true, false);
		return;
	}

	if (OnlinePlay())
	{
		if (IsLocalContent(S4_REPLAY_SYSTEM))
		{
			int ret;
			switch (Doc()->m_golfGame.gameType)
			{
			case GAME_TYPE_STROKE:
			case GAME_TYPE_TEAM:
			case GAME_TYPE_30S:
			case GAME_TYPE_30S_TEAM:
			case GAME_TYPE_GUILD_MATCH:
			case GAME_TYPE_SKINS:
				ret = ShotDataSave(NULL, false);
				break;

			default:
				ret = 0;
				break;
			}

			if (ret == 1)
			{
				WSendPacket packet((enumClientPacket)0x4a);
				packet.Encode4(typeId);
				packet.Send(TO_GAME);
			}
			else if (ret == -1)
			{
				CChatMsg::Instance()->AddChatMsg(
					MakeStr(
						"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc \xc6\xc4\xc0\xcf\xc0\xcc 100\xb0\xb3\xb8\xa6 \xc3\xca\xb0\xfa\xc7\xcf\xbf\xa9 \xc0\xfa\xc0\xe5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xbb\xe8\xc1\xa6 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9."),
					0xffff7878, true, false);
			}
			else if (ret == 0)
			{
				CChatMsg::Instance()->AddChatMsg(
					MakeStr(
						"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc\xb8\xa6 \xc0\xfa\xc0\xe5\xc7\xcf\xb4\xc2 \xb5\xb5\xc1\xdf \xb9\xae\xc1\xa6\xb0\xa1 \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9."),
					0xffff7878, true, false);
			}
		}
		else
		{
			WSendPacket packet((enumClientPacket)0x4a);
			packet.Encode4(typeId);
			packet.Send(TO_GAME);
		}
	}
	else
	{
		WSendPacket packet((enumClientPacket)0x49);
		packet.Send(TO_GAME);
	}
}

void CGolfRule::OnRecordAviBtnUp()
{
	if (m_pRecordAviDlg == NULL)
	{
		char* path = GetPangyaInstallDic();
		char msg[255] = {
			0,
		};
		m_pRecordAviDlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");

		sprintf(msg,
			"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc \xc5\xd7\xc0\xd9\xc0\xc7 \xb3\xbb\xbf\xeb\xc0\xbb\n\xb5\xbf\xbf\xb5\xbb\xf3 \xc6\xc4\xc0\xcf\xb7\xce \xc0\xfa\xc0\xe5\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?\n\n\t\t\t\t\t\t   \xc0\xfa\xc0\xe5 \xb0\xe6\xb7\xce\n\t\t\t\t\t\t   %s/replay",
			path);
		m_pRecordAviDlg->SetMessage(msg, false);
		m_pRecordAviDlg->Open((FRESH_PFN_RESULT)&CGolfRule::OnRecordAviResult,
			0);
	}
}

bool CGolfRule::OnRecordAviResult(int result, FrForm* form)
{
	if (result == 1)
	{
		Doc()->SetReplayState(9);
	}

	m_pRecordAviDlg = NULL;
	return true;
}

char* CGolfRule::GetPangyaInstallDic()
{
	static char path[255];

	CRegistry registry;
	registry.SetRootKey(HKEY_LOCAL_MACHINE);
	registry.SetKey("Software\\Ntreev\\Pangya_QA", true);

	WTL::CString directory = "";
	directory = registry.ReadString("Install_Dir", "");
	sprintf(path, "%s", directory);

	return path;
}

void CGolfRule::ReplayDlgRun()
{
	if (m_pReplayStatus && m_pReplayStatus->IsVisible() &&
		CMouseCursor::Instance()->InArea(m_pReplayStatus->GetRect()))
	{
		if (m_pReplayDlg && !m_pReplayDlg->IsVisible())
		{
			m_pReplayDlg->SetVisible(true);
		}
		else
		{
			ReplayBoxOpen();
		}
	}

	if (!m_pReplayDlg || !m_pReplayDlg->IsVisible())
	{
		if (m_pReplayStatus)
			m_pReplayStatus->SetVisible(true);
	}

	if (m_pReplayStatus && m_pReplayStatus->IsVisible())
	{
		switch (Doc()->GetReplayState())
		{
		case 2:
		case 3:
			m_pReplayStatus->SetBgImg("replay_icon_play_n");
			break;

		case 4:
			m_pReplayStatus->SetBgImg("replay_icon_stop_n");
			break;
		}
	}
}

void CGolfRule::AddSomeInfo(WSendPacket& packet)
{
	unsigned char rate = rand() % 100;

	if (rate < 30)
	{
		packet.Encode2(1);

		std::list<unsigned long>& cardList =
			CCardManager::Instance()->GetActiveCardList();
		unsigned long count = cardList.size();
		int pos = packet.GetLengthData();
		packet.Encode4(count);
		std::list<unsigned long>::iterator it = cardList.begin();
		for (; it != cardList.end(); ++it)
		{
			IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(*it);
			if (pCard)
			{
				struct
				{
					unsigned long tid;
					unsigned short avility;
					unsigned short value;
				} info;
				memset(&info, 0, sizeof(info));
				info.tid = pCard->c.TypeId;
				info.avility = pCard->Avility;
				info.value = pCard->AvilityValue;
				packet.EncodeBuffer(&info, sizeof(info));
			}
			else
			{
				count--;
			}
		}

		if (count != cardList.size())
		{
			packet.EncodeData(pos, (char*)&count, sizeof(count));
		}

		if (IsLocalContent(S4_HACKINGTOOL))
		{
			if (Wind().IsGlobalWind_Compare() == true)
			{
				if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000006 ||
					PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000020)
				{
					packet.Encode1(1);
					packet.Encode4(1);
				}
				else
				{
					unsigned long value =
						(unsigned long)Wind().GetGlobalIntensity();

					if (Wind().IsActiveSpecialWind())
					{
						packet.Encode1(2);
						packet.Encode4(value);
					}
					else
					{
						packet.Encode1(0);
						float f = value - 1.0f;
						if (f < 1.0f)
							packet.Encode4(value);
						else
							packet.Encode4(value - 1);
					}
				}
			}
			else
			{
				packet.Encode4(0);
			}
		}
		else
		{
			packet.Encode4((unsigned long)Wind().GetGlobalIntensity());
		}
	}
	else
	{
		packet.Encode2(0);
	}
}

void CGolfRule::ProcessObserver()
{
	BOOL bObserver = IsObserver(0xffffffff);

	if (bObserver)
	{
		if ((g_input->GetDown("NUM_9", true) ||
				g_input->GetDown("PAD_9", true)) &&
			(GOLFDOC()->GameMode() & 0x8338))
		{
			ChangeGameMode(0x200000);
			return;
		}

		if (GetGameType() == GAME_TYPE_STROKE &&
			(g_input->Get("LCONTROL", false) ||
				g_input->Get("RCONTROL", false)) &&
			g_input->GetDown("INSERT", true))
		{
			WSendPacket packet((enumClientPacket)0x5d);
			packet.Send(TO_GAME);
		}

		if (IsMassGame() && GOLFDOC()->GameMode() != 0x2000 &&
			g_CurrentTime - m_observerTime > 3000)
		{
			unsigned long uid = 0xffffffff;
			for (int key = 1; key < 9; key++)
			{
				if (g_input->GetDown(MakeStr("PAD_%d", key), true) ||
					g_input->GetDown(MakeStr("NUM_%d", key), true))
				{
					unsigned char count = 0;
					std::vector<sRivalData>::iterator it;
					for (it = Doc()->m_rivalList.begin();
						it != Doc()->m_rivalList.end(); ++it)
					{
						if ((*it).state)
						{
							(*it).observerRank = 0;
							continue;
						}

						(*it).observerRank = 1;
						count++;

						std::vector<sRivalData>::iterator it2;
						for (it2 = Doc()->m_rivalList.begin();
							it2 != Doc()->m_rivalList.end(); ++it2)
						{
							if (!(*it2).state && it != it2)
							{
								if (((*it).totalScore > (*it2).totalScore ||
										((*it).totalScore ==
												(*it2).totalScore &&
											(*it).totalPang <
												(*it2).totalPang) ||
										((*it).totalScore ==
												(*it2).totalScore &&
											(*it).totalPang ==
												(*it2).totalPang &&
											(*it).order < (*it2).order)) != 0)
								{
									(*it).observerRank++;
								}
							}
						}
					}

					switch (key)
					{
					case 1:
						for (it = Doc()->m_rivalList.begin();
							it != Doc()->m_rivalList.end(); ++it)
						{
							if (Doc()->m_briefUserInfoMap[(*it).oid].IsIdentity(
									0x10))
							{
								uid = (*it).oid;
								break;
							}
						}
						break;

					case 2:
					{
						unsigned char r = rand() % count;
						for (it = Doc()->m_rivalList.begin();
							it != Doc()->m_rivalList.end(); ++it, r--)
						{
							if ((*it).oid !=
									Doc()->m_myInfo.info.dwGalleryGuid &&
								(*it).observerState != 1 && !(*it).state &&
								!Doc()
									->m_briefUserInfoMap[(*it).oid]
									.IsIdentity(0x10) &&
								(*it).titleTypeId != 0x30000001)
							{
								if ((*it).observerRank != 1 &&
									(*it).observerRank != 2 &&
									(*it).observerRank != 3)
								{
									if (r <= 0)
									{
										uid = (*it).oid;
										break;
									}
								}
							}
						}
					}
					break;

					case 3:
						if (count >= 1)
						{
							for (it = Doc()->m_rivalList.begin();
								it != Doc()->m_rivalList.end(); ++it)
							{
								if ((*it).observerRank == 1)
								{
									uid = (*it).oid;
									break;
								}
							}
						}
						break;

					case 4:
						if (count >= 2)
						{
							for (it = Doc()->m_rivalList.begin();
								it != Doc()->m_rivalList.end(); ++it)
							{
								if ((*it).observerRank == 2)
								{
									uid = (*it).oid;
									break;
								}
							}
						}
						break;

					case 5:
						if (count >= 3)
						{
							for (it = Doc()->m_rivalList.begin();
								it != Doc()->m_rivalList.end(); ++it)
							{
								if ((*it).observerRank == 3)
								{
									uid = (*it).oid;
									break;
								}
							}
						}
						break;

					case 6:
						if (count >= 1)
						{
							for (it = Doc()->m_rivalList.begin();
								it != Doc()->m_rivalList.end(); ++it)
							{
								if ((*it).observerRank == count)
								{
									uid = (*it).oid;
									break;
								}
							}
						}
						break;

					case 7:
						if (count >= 2)
						{
							for (it = Doc()->m_rivalList.begin();
								it != Doc()->m_rivalList.end(); ++it)
							{
								if ((*it).observerRank == count - 1)
								{
									uid = (*it).oid;
									break;
								}
							}
						}
						break;

					case 8:
						if (IsLocalCountry(1))
						{
							for (it = Doc()->m_rivalList.begin();
								it != Doc()->m_rivalList.end(); ++it)
							{
								if ((*it).titleTypeId == 0x30000001)
								{
									uid = (*it).oid;
									break;
								}
							}
						}
						break;
					}

					sRivalData data = Doc()->m_rivalList[Doc()->GetIndex(uid)];
					if (uid != 0xffffffff &&
						uid != Doc()->m_myInfo.info.dwGalleryGuid &&
						data.observerState == 0 && data.state == 0)
					{
						m_observerTime = g_CurrentTime;

						WSendPacket packet((enumClientPacket)0x3f);
						packet.Encode4(uid);
						packet.Send(TO_GAME);
						// HACK
						if (count >= 2 && uid != 0xffffffff)
							return;
					}
					break;
				}
			}
		}
	}
}

bool CGolfRule::IsLastShotRelplay()
{
	if (m_replayCamera == 2)
	{
		return true;
	}

	return false;
}

void CGolfRule::HoleComboGaugeSetting()
{
	const unsigned long items[2] = { 0x70010010, 0x70010012 };

	if (IsMassGame())
	{
		sCharacterInfo& charInfo =
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo;
		float bonus = 0.0f;
		for (int j = 0; j < 2; j++)
		{
			if (charInfo.tidAuxParts[0] == items[j])
				bonus += 1.0f;
		}

		PLAYER(GOLFDOC()->m_currentPlayer)->gauge =
			Between(0.0f, PLAYER(GOLFDOC()->m_currentPlayer)->gauge + bonus,
				Doc()->GetComboGaugeLimit(GOLFDOC()->m_currentPlayer));
	}
	else
	{
		for (int i = 0; i < GOLFDOC()->m_playerNum; i++)
		{
			float bonus = 0.0f;
			for (int j = 0; j < 2; j++)
			{
				if (Doc()->m_userInfo[i].charInfo.tidAuxParts[0] == items[j])
					bonus += 1.0f;
			}

			if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
			{
				TEAM(PLAYER(i)->team)->gauge =
					Between(0.0f, TEAM(PLAYER(i)->team)->gauge + bonus,
						Doc()->GetComboGaugeLimit(PLAYER(i)->team));
			}
			else
			{
				PLAYER(i)->gauge = Between(0.0f, PLAYER(i)->gauge + bonus,
					Doc()->GetComboGaugeLimit(i));
			}
		}
	}
}

const char* ball_stop_replay_data_name = "save/pangya_bug_report.sav";

void CGolfRule::CollectBallStopData()
{
	int playType = Doc()->GetReplayPlayType();
	Doc()->SetReplayPlayType(2);
	if (ShotDataSave(ball_stop_replay_data_name, false) == 1)
	{
		NetResManager()->UploadBugReportData(ball_stop_replay_data_name, 1,
			GolfBall().m_rollFrame != -1 ? 2 : 1, true);
	}

	Doc()->SetReplayPlayType(playType);
}

void CGolfRule::EncodePacket(enumClientPacket type, WSendPacket& packet)
{
	if (type == 0x1c)
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->oid == PLAYER(MyIndex())->oid)
			packet.Encode1(1);
		else
			packet.Encode1(0);

		std::vector<FieldItem::AcquiredItemInfo>* pList = NULL;
		SendMsg(this, "GroundItemMan", 0x277, (int)&pList, 0, 0, 0);

		if (pList)
		{
			packet.Encode1(pList->size());

			std::vector<FieldItem::AcquiredItemInfo>::const_iterator it =
				pList->begin();
			while (it != pList->end())
			{
				(*it).EncodeTo(packet);
				++it;
			}

			pList->clear();
		}
		else
		{
			packet.Encode1(0);
		}
	}
}

ILFILLTU_GOLFRULE2
