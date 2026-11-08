#include "rankingdlg.h"
#include "rankinginfo.h"
#include "binstr.h"
#include "inputmanager.h"
#include "emoticondlg.h"
#include "avatar_task.h"
#include "avatar_avatar.h"
#include "frtextbutton.h"
#include "frcombobox.h"
#include "exhibition.h"
#include "parttidlist.h"
#include "mousecursor.h"
#include "taskmanager.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;
extern WResourceManager* g_resrcmng;

const int FrRankingDlg::s_iListCountPerPage = 12;
int FrRankingDlg::s_iRankType;
int FrRankingDlg::s_iSubType;
int FrRankingDlg::s_iTermType;
int FrRankingDlg::s_iClassType;
int FrRankingDlg::s_iPageNum;
int FrRankingDlg::s_iPageTotal;
int FrRankingDlg::s_iSearchType;

const sTypeName FrRankingDlg::s_aRankType[] = {
	{ 0, K2L_Compatibility("\xc1\xbe\xc7\xd5 \xb7\xa9\xc5\xb7")         },
	{ 1, K2L_Compatibility("\xc4\xda\xbd\xba\xba\xb0 \xb7\xa9\xc5\xb7") },
	{ 2, K2L_Compatibility("\xb1\xe2\xb7\xcf \xb7\xa9\xc5\xb7")         }
};

const sTypeName FrRankingDlg::s_aClassType[] = {
	{ 0, K2L_Compatibility("\xc0\xfc\xc3\xbc") },
	{ 1, K2L_Compatibility("\xc3\xca\xb1\xde") },
	{ 2, K2L_Compatibility("\xc1\xdf\xb1\xde") },
	{ 3, K2L_Compatibility("\xbb\xf3\xb1\xde") }
};

const sTypeName FrRankingDlg::s_aGenSubType[] = {
	{ 0, K2L_Compatibility("\xc1\xbe\xc7\xd5 \xc1\xa1\xbc\xf6")             },
	{ 1, K2L_Compatibility("\xbd\xba\xc4\xda\xbe\xee \xc1\xa1\xbc\xf6")     },
	{ 2, K2L_Compatibility("\xc6\xae\xb7\xce\xc7\xc7 \xc1\xa1\xbc\xf6")     },
	{ 3, K2L_Compatibility("\xc8\xb9\xb5\xe6\xc6\xce \xc1\xa1\xbc\xf6")     },
	{ 4,
     K2L_Compatibility("\xc7\xc3\xb7\xb9\xc0\xcc\xc8\xa6 \xc1\xa1\xbc\xf6") }
};

const sTypeName FrRankingDlg::s_aCourseType[] = {
	{ 0,  K2L_Compatibility("\xba\xed\xb7\xe7\xb6\xf3\xb1\xba")          },
	{ 1,  K2L_Compatibility("\xba\xed\xb7\xe7\xbf\xf6\xc5\xcd")          },
	{ 2,  K2L_Compatibility("\xbc\xbc\xc7\xc7\xbe\xc6\xc0\xa9\xb5\xe5")  },
	{ 3,  K2L_Compatibility("\xc0\xa9\xb5\xe5\xc8\xfa")                  },
	{ 4,  K2L_Compatibility("\xc0\xa7\xc1\xee\xc0\xa7\xc1\xee")          },
	{ 5,  K2L_Compatibility("\xbf\xfe\xbd\xba\xc6\xae\xc0\xa7\xc1\xee")  },
	{ 6,  K2L_Compatibility("\xba\xed\xb7\xe7\xb9\xae")                  },
	{ 7,  K2L_Compatibility("\xbd\xc7\xba\xf1\xbe\xc6\xc4\xb3\xb3\xed")  },
	{ 8,  K2L_Compatibility("\xbe\xc6\xc0\xcc\xbd\xba\xc4\xb3\xb3\xed")  },
	{ 9,  K2L_Compatibility("\xc8\xad\xc0\xcc\xc6\xae\xc0\xa7\xc1\xee")  },
	{ 10, K2L_Compatibility("\xbb\xfe\xc0\xcc\xb4\xd7\xbb\xf7\xb5\xe5")  },
	{ 11, K2L_Compatibility("\xc7\xce\xc5\xa9\xc0\xa9\xb5\xe5")          },
	{ 12, K2L_Compatibility("\xb5\xf6\xc0\xce\xc6\xe4\xb8\xa3\xb3\xeb")  },
	{ 13, K2L_Compatibility("\xbe\xc6\xc0\xcc\xbd\xba \xbd\xba\xc6\xc4") },
	{ 14,
     K2L_Compatibility(
			"\xb7\xce\xbd\xba\xc6\xae \xbd\xc3\xbf\xfe\xc0\xcc")         },
	{ 15, K2L_Compatibility("\xc0\xcc\xbd\xba\xc5\xcf \xba\xa7\xb8\xae") },
	{ 16, K2L_Compatibility("\xc0\xa7\xc1\xee \xbd\xc3\xc6\xbc")         }
};

const sTypeName FrRankingDlg::s_aRecordType[] = {
	{ 0, K2L_Compatibility("\xbe\xcb\xb9\xd9\xc6\xae\xb7\xce\xbd\xba")         },
	{ 1, K2L_Compatibility("\xc8\xa6\xc0\xce\xbf\xf8")                         },
	{ 2, K2L_Compatibility("\xb0\xad\xc1\xbe\xc0\xb2")                         },
	{ 3, K2L_Compatibility("\xb7\xb9\xba\xa7")                                 },
	{ 4, K2L_Compatibility("\xc3\xd1\xc5\xb8\xb1\xb8\xba\xf1\xb0\xc5\xb8\xae") }
};

const sTypeName FrRankingDlg::s_aTermType[] = {
	{ 0,
     K2L_Compatibility("\xbd\xc3\xc1\xf0"
						  "5") }
};

const sTypeName FrRankingDlg::s_aSearchType[] = {
	{ 0, K2L_Compatibility("\xb4\xd0\xb3\xd7\xc0\xd3") },
	{ 1, K2L_Compatibility("\xbc\xf8\xc0\xa7")         }
};

long GetRealCourseType(long course)
{
	if (course == 12)
		course = 13;
	else if (course == 13)
		course = 14;
	else if (course == 14)
		course = 15;
	else if (course == 15)
		course = 16;
	else if (course == 16)
		course = 19;
	return course;
}

IMPLEMENT_ACTOR(FrRankingDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrRankingDlg, FrForm)
ON_FRESH_VI("class_type", FRCMD_INIT, FrRankingDlg::OnClassType_ComboInit)
ON_FRESH_VI("class_type", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnClassType_ComboDown)
ON_FRESH_VI("btn_current_season", FRCMD_INIT,
	FrRankingDlg::OnTermType_S5_BtnInit)
ON_FRESH_VV("btn_current_season", FRCMD_LBUTTONUP,
	FrRankingDlg::OnTermType_S5_BtnUp)
ON_FRESH_VI("course", FRCMD_INIT, FrRankingDlg::OnCourseAreaInit)
ON_FRESH_VI("course", FRCMD_OWNERDRAW, FrRankingDlg::OnCourseAreaOwnerDraw)
ON_FRESH_VI("user_rank", FRCMD_INIT, FrRankingDlg::OnUserRankEditInit)
ON_FRESH_VI("user", FRCMD_INIT, FrRankingDlg::OnRankList_Init)
ON_FRESH_VI("user", FRCMD_OWNERDRAW, FrRankingDlg::OnRankList_OwnerDraw)
ON_FRESH_VV("user", FRCMD_LBUTTONDOWN, FrRankingDlg::OnRankList_LBtnDown)
ON_FRESH_VI("prev_page", FRCMD_INIT, FrRankingDlg::OnRankList_PrevPageInit)
ON_FRESH_VV("prev_page", FRCMD_LBUTTONUP,
	FrRankingDlg::OnRankList_PrevPageBtnUp)
ON_FRESH_VI("next_page", FRCMD_INIT, FrRankingDlg::OnRankList_NextPageInit)
ON_FRESH_VV("next_page", FRCMD_LBUTTONUP,
	FrRankingDlg::OnRankList_NextPageBtnUp)
ON_FRESH_VI("cur_page", FRCMD_INIT, FrRankingDlg::OnRankList_CurrentPageInit)
ON_FRESH_VI("tot_page", FRCMD_INIT, FrRankingDlg::OnRankList_TotalPageInit)
ON_FRESH_VI("rlist_myinfo", FRCMD_INIT, FrRankingDlg::OnRankList_MyInfo_Init)
ON_FRESH_VV("rlist_myinfo", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnRankList_MyInfo_LBtnDown)
ON_FRESH_VI("rlist_myinfo", FRCMD_OWNERDRAW,
	FrRankingDlg::OnRankList_MyInfo_OwnerDraw)
ON_FRESH_VI("rlist_s6", FRCMD_INIT, FrRankingDlg::OnRankList_NoInRank_Init)
ON_FRESH_VI("search_user", FRCMD_INIT, FrRankingDlg::OnRankList_SearchUserInit)
ON_FRESH_VV("search_user", FRCMD_LBUTTONUP,
	FrRankingDlg::OnRankList_SearchUserBtnUp)
ON_FRESH_VI("edit_id", FRCMD_INIT, FrRankingDlg::OnRankList_UserIdEditInit)
ON_FRESH_VV("edit_id", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnRankList_UserIdEditDown)
ON_FRESH_VI("emoticon", FRCMD_INIT, FrRankingDlg::OnEmoticonInit)
ON_FRESH_VV("emoticon", FRCMD_LBUTTONUP, FrRankingDlg::OnEmoticonBtnUp)
ON_FRESH_VI("language", FRCMD_INIT, FrRankingDlg::OnLanguageInit)
ON_FRESH_VI("pet_char", FRCMD_INIT, FrRankingDlg::OnPetCharacterInit)
ON_FRESH_VV("pet_char", FRCMD_OWNERDRAW, FrRankingDlg::OnPetCharacterOwnerDraw)
ON_FRESH_VI("pet_load", FRCMD_INIT, FrRankingDlg::OnPetLoadInit)
ON_FRESH_VI("info_up", FRCMD_INIT, FrRankingDlg::OnUpperMiniInfoInit)
ON_FRESH_VV("info_up", FRCMD_OWNERDRAW, FrRankingDlg::OnUpperMiniInfoOwnerDraw)
ON_FRESH_VI("about", FRCMD_INIT, FrRankingDlg::OnAboutBtnInit)
ON_FRESH_VV("about", FRCMD_LBUTTONUP, FrRankingDlg::OnAboutBtnUp)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrRankingDlg::OnCloseBtnUp)
ON_FRESH_VI("message", FRCMD_INIT, FrRankingDlg::OnBottomMessageInit)
ON_FRESH_VI("rank_type", FRCMD_INIT, FrRankingDlg::OnRankTypeListInit)
ON_FRESH_VV("rank_type", FRCMD_LBUTTONDOWN, FrRankingDlg::OnRankTypeListLDown)
ON_FRESH_VI("rank_type", FRCMD_OWNERDRAW, FrRankingDlg::OnRankTypeListOwnerDraw)
ON_FRESH_VI("rank_type_v", FRCMD_INIT, FrRankingDlg::OnRankTypeViewerInit)
ON_FRESH_VV("rank_type_v", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnRankTypeViewerDown)
ON_FRESH_VV("rank_type_v", FRCMD_OWNERDRAW,
	FrRankingDlg::OnRankTypeViewerOwnerDraw)
ON_FRESH_VI("gensub_type", FRCMD_INIT, FrRankingDlg::OnGeneralSubTypeListInit)
ON_FRESH_VV("gensub_type", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnGeneralSubTypeListLDown)
ON_FRESH_VI("gensub_type", FRCMD_OWNERDRAW,
	FrRankingDlg::OnGeneralSubTypeListOwnerDraw)
ON_FRESH_VI("gensub_type_v", FRCMD_INIT,
	FrRankingDlg::OnGeneralSubTypeViewerInit)
ON_FRESH_VV("gensub_type_v", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnGeneralSubTypeViewerDown)
ON_FRESH_VV("gensub_type_v", FRCMD_OWNERDRAW,
	FrRankingDlg::OnGeneralSubTypeViewerOwnerDraw)
ON_FRESH_VI("course_type", FRCMD_INIT, FrRankingDlg::OnCourseTypeListInit)
ON_FRESH_VV("course_type", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnCourseTypeListLDown)
ON_FRESH_VI("course_type", FRCMD_OWNERDRAW,
	FrRankingDlg::OnCourseTypeListOwnerDraw)
ON_FRESH_VI("course_type_v", FRCMD_INIT, FrRankingDlg::OnCourseTypeViewerInit)
ON_FRESH_VV("course_type_v", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnCourseTypeViewerDown)
ON_FRESH_VV("course_type_v", FRCMD_OWNERDRAW,
	FrRankingDlg::OnCourseTypeViewerOwnerDraw)
ON_FRESH_VI("record_type", FRCMD_INIT, FrRankingDlg::OnRecordTypeListInit)
ON_FRESH_VV("record_type", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnRecordTypeListLDown)
ON_FRESH_VI("record_type", FRCMD_OWNERDRAW,
	FrRankingDlg::OnRecordTypeListOwnerDraw)
ON_FRESH_VI("record_type_v", FRCMD_INIT, FrRankingDlg::OnRecordTypeViewerInit)
ON_FRESH_VV("record_type_v", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnRecordTypeViewerDown)
ON_FRESH_VV("record_type_v", FRCMD_OWNERDRAW,
	FrRankingDlg::OnRecordTypeViewerOwnerDraw)
ON_FRESH_VI("search_type", FRCMD_INIT, FrRankingDlg::OnSearchTypeListInit)
ON_FRESH_VV("search_type", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnSearchTypeListLDown)
ON_FRESH_VI("search_type", FRCMD_OWNERDRAW,
	FrRankingDlg::OnSearchTypeListOwnerDraw)
ON_FRESH_VI("search_type_v", FRCMD_INIT, FrRankingDlg::OnSearchTypeViewerInit)
ON_FRESH_VV("search_type_v", FRCMD_LBUTTONDOWN,
	FrRankingDlg::OnSearchTypeViewerDown)
ON_FRESH_VV("search_type_v", FRCMD_OWNERDRAW,
	FrRankingDlg::OnSearchTypeViewerOwnerDraw)
ON_FRESH_VI("ghost_btn", FRCMD_INIT, FrRankingDlg::OnGhostBtnInit)
ON_FRESH_VV("ghost_btn", FRCMD_LBUTTONDOWN, FrRankingDlg::OnGhostBtnUp)
END_FRESH_MSGMAP()

FrRankingDlg::FrRankingDlg()
{
	s_iRankType = 0;
	s_iSubType = 0;
	s_iTermType = 0;
	s_iClassType = 0;
	s_iPageNum = 0;
	s_iPageTotal = 0;
	s_iSearchType = 0;
	m_pExhibition = NULL;
	m_pPartTidList = NULL;
	m_pUpperMiniInfo = NULL;
	m_requestDelay = -1.f;
	m_rankInfoList.clear();
	memset(&m_userInfo, 0, sizeof(m_userInfo));
	m_myRankInfo.uid = m_userInfo.uid = -1;
	ResetControl();
	if (IsLocalContent(S4_MATCHING_SYSTEM))
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(2);
		packet.Send(TO_GAME);
	}
}

FrRankingDlg::~FrRankingDlg()
{
	HidePet(true);
	g_resrcmng->UnlockAllByThread(m_anotherHand.GetThreadID());
	m_anotherHand.BreakWorkingThread();
	if (CMouseCursor::IsInstantiated())
		CMouseCursor::Instance()->SetActive(true);
	if (IsLocalContent(S4_MATCHING_SYSTEM) && WNetworkSystem::IsInstantiated())
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(1);
		packet.Send(TO_GAME);
	}
}

bool FrRankingDlg::OnInit()
{
	FrForm::OnInit();
	SendRequestRank(true);
	SendRequestUserInfo(Doc()->m_myInfo.info.dwUID, Doc()->m_myInfo.info.sID);
	return true;
}

void FrRankingDlg::OnCancel()
{
	Close(true);
}

void FrRankingDlg::OnProc(const float delta)
{
	if (m_requestDelay > 0)
	{
		m_requestDelay -= delta;
		if (m_requestDelay < 0)
		{
			SetVisibleUserInfo(true);
			SetAllControlsEnable(true);
		}
	}
	if (g_input->GetButton(LEFT_BUTTON) || g_input->GetButton(RIGHT_BUTTON))
		CloseAllComboBtns();
	if ((g_input->GetDown("ENTER", true) ||
			g_input->GetDown("PADENTER", true)) &&
		m_pUserIdEdit->IsViewFocused())
	{
		OnRankList_SearchUserBtnUp();
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.f);
	}
	static bool english = true;
	if (m_pLanguage && g_ime->IsAlphaNumericMode() != english)
	{
		english = g_ime->IsAlphaNumericMode();
		if (english)
			m_pLanguage->SetBgImg("chat_eng");
		else
			m_pLanguage->SetBgImg("chat_kor");
	}
	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("EMOTICON", false))
		OnEmoticonBtnUp();
}

void FrRankingDlg::OnAnotherHand(int key, void* param)
{
	if (key == 0)
		SetPetCharacter();
}

void FrRankingDlg::ResetControl()
{
	m_pTermTypeS5 = NULL;
	m_pRankList = NULL;
	m_pMyInfo = NULL;
	m_pClassTypeCombo = NULL;
	m_pCourseArea = NULL;
	m_pNoInRank = NULL;
	m_pPetCharacter = NULL;
	m_pSearchUser = NULL;
	m_pEmoticon = NULL;
	m_pUserIdEdit = NULL;
	m_pCurrentPage = NULL;
	m_pTotalPage = NULL;
	m_pUserRankEdit = NULL;
	m_pRankTypeList = NULL;
	m_pRankTypeViewer = NULL;
	m_pGenSubTypeList = NULL;
	m_pGenSubTypeViewer = NULL;
	m_pCourseTypeList = NULL;
	m_pCourseTypeViewer = NULL;
	m_pRecordTypeList = NULL;
	m_pRecordTypeViewer = NULL;
	m_pSearchTypeList = NULL;
	m_pSearchTypeViewer = NULL;
	m_pPetLoad = NULL;
	m_pBottomMessage = NULL;
	m_pEmoticonDlg = NULL;
	m_pAboutDlg = NULL;
	m_pGhostBtn = NULL;
	memset(m_pRankTypeBtn, 0, sizeof(m_pRankTypeBtn));
	memset(m_pGenSubTypeBtn, 0, sizeof(m_pGenSubTypeBtn));
	memset(m_pCourseTypeBtn, 0, sizeof(m_pCourseTypeBtn));
	memset(m_pRecordTypeBtn, 0, sizeof(m_pRecordTypeBtn));
	memset(m_pSearchTypeBtn, 0, sizeof(m_pSearchTypeBtn));
}

void FrRankingDlg::SetVisibleByRankType()
{
	if (m_pGenSubTypeViewer)
		m_pGenSubTypeViewer->SetVisible(s_iRankType == 0);
	if (m_pCourseTypeViewer)
		m_pCourseTypeViewer->SetVisible(s_iRankType == 1);
	if (m_pRecordTypeViewer)
		m_pRecordTypeViewer->SetVisible(s_iRankType == 2);
}

void FrRankingDlg::SetVisibleUserInfo(bool visible)
{
	if (m_pPetCharacter)
		m_pPetCharacter->SetVisible(visible);
	if (m_pUpperMiniInfo)
		m_pUpperMiniInfo->SetVisible(visible);
	if (m_pPetLoad)
	{
		m_pPetLoad->SetVisible(!visible);
		m_pPetLoad->SetPos(0);
		m_pPetLoad->SetDestPos(150, visible);
	}
	if (m_pUserRankEdit)
		m_pUserRankEdit->SetVisible(visible);
}

void FrRankingDlg::UpdateRankList()
{
	if (m_pRankList)
	{
		m_pRankList->ClearItem();
		int count = 0;
		for (std::list<ranktype::sListInfo>::iterator it =
				 m_rankInfoList.begin();
			it != m_rankInfoList.end(); ++it, ++count)
		{
			if (count >= s_iListCountPerPage)
				break;
			m_pRankList->AddItem(&*it);
		}
	}
	SynchronizeButtons();
	char text[32];
	itoa(s_iPageNum + 1, text, 10);
	m_pCurrentPage->SetLine(1, text, 0, false, 0);
	m_pCurrentPage->SetVisible(true);
}

void FrRankingDlg::ClearRankList()
{
	if (m_pRankList)
		m_pRankList->ClearItem();
	char text[32];
	s_iPageNum = 0;
	itoa(s_iPageNum, text, 10);
	m_pCurrentPage->SetLine(1, text, 0, false, 0);
	s_iPageTotal = 0;
	itoa(s_iPageTotal, text, 10);
	m_pTotalPage->SetLine(1, text, 0, false, 0);
	SynchronizeButtons();
}

void FrRankingDlg::UpdateMyInfo()
{
	if (m_pMyInfo)
	{
		m_pMyInfo->ClearItem();
		if (m_myRankInfo.uid != -1)
		{
			m_pMyInfo->AddItem(&m_myRankInfo);
			m_pMyInfo->SetVisible(true);
			m_pNoInRank->SetVisible(false);
		}
		else
		{
			m_pMyInfo->SetVisible(false);
			m_pNoInRank->SetVisible(true);
		}
	}
}

void FrRankingDlg::UpdateUserInfo()
{
	m_anotherHand.Work(this, 0, NULL, true, -1);
	if (m_pPetCharacter)
		m_pPetCharacter->SetVisible(true);
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	gdi->SetTextStyle(1);
	char text[32];
	if (m_userInfo.record[0].rank <= 0)
		sprintf(text, K2L_Compatibility("- \xc0\xa7"));
	else
		sprintf(text, K2L_Compatibility("%d \xc0\xa7"),
			m_userInfo.record[0].rank);
	m_pUserRankEdit->SetLine(1, text, 0xffffffff, false, 0);
	gdi->SetTextStyle(0);
}

void FrRankingDlg::SetPetCharacter()
{
	m_anotherHand.Enter();
	if (!m_pPetCharacter)
	{
		m_anotherHand.Leave();
		return;
	}
	m_pPetLoad->SetVisible(true);
	HidePet(false);
	WRect rect = m_pPetCharacter->GetRect();
	m_pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
		rect.y + rect.h * 0.5f, 0.15f, true);
	m_pExhibition->SetDistance(g_view->GetWidth() * 0.0203125f, true, false);
	m_pExhibition->SetArea(rect.w, rect.h);
	m_pPartTidList = new CPartTidList;
	sCharacterInfo* info = &m_userInfo.charInfo;
	if (!m_pPartTidList->SetTids(info, 0xff) || !m_pPartTidList->IsComboValid())
		m_pPartTidList->SetDefaultTids();
	m_pExhibition->SetModel(*m_pPartTidList, info->tidAuxParts, NULL, 3.f, 0);
	m_anotherHand.Leave();
}

void FrRankingDlg::SetListItemInfo(FrListItem* item)
{
	unsigned long color = 0xffffffff;
	ranktype::sListInfo* info = (ranktype::sListInfo*)item->pData;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (info->uid == m_userInfo.uid)
	{
		WRect rect(item->pos.x + 1, item->pos.y - 5,
			(float)m_pRankList->GetItemWidth() + 17,
			(float)m_pRankList->GetItemHeight());
		WOverlay::DrawBox(g_view, WRect(rect), 0, 0xa0ffff00, 0.f);
		color = 0xff000000;
	}
	gdi->SetTextStyle(0);
	if (info->preRank == 0 ||
		fabs((float)info->rank - (float)info->preRank) > 10000.f)
	{
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("ICONS", "new");
		if (bitmap)
			gdi->DrawTexture(bitmap,
				WRect(item->pos.x + 45, item->pos.y + 4, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else if (info->preRank < info->rank)
	{
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("RANKING", "dn");
		if (bitmap)
			gdi->DrawTexture(bitmap,
				WRect(item->pos.x + 45, item->pos.y + 2, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
		gdi->SetTextColor(0xff0000ff, 0xffffffff);
		gdi->PrintText(WPoint(item->pos.x + bitmap->Width() + 45, item->pos.y),
			0, MakeStr("%d", info->rank - info->preRank), NULL);
	}
	else if (info->rank < info->preRank)
	{
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("RANKING", "up");
		if (bitmap)
			gdi->DrawTexture(bitmap,
				WRect(item->pos.x + 45, item->pos.y + 2, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
		gdi->SetTextColor(0xffff0000, 0xffffffff);
		gdi->PrintText(WPoint(item->pos.x + bitmap->Width() + 45, item->pos.y),
			0, MakeStr("%d", info->preRank - info->rank), NULL);
	}
	else
	{
		gdi->SetTextColor(color, 0xffffffff);
		gdi->PrintText(WPoint(item->pos.x + 53, item->pos.y), 0, "-", NULL);
	}
	const char* rankText;
	if (info->rank == 0 || info->rank > 10000 || info->value == 0)
		rankText = "-";
	else
		rankText = MakeStr("%d", info->rank);
	gdi->SetTextColor(color, 0xffffffff);
	gdi->SetTextStyle(1);
	gdi->PrintText(WPoint(item->pos.x + 7, item->pos.y), 0, rankText, NULL);
	gdi->SetTextStyle(0);
	if (!CProjectG::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(WPoint(item->pos.x + 85, item->pos.y),
			0, info->nickname.c_str(), -1.f, 0xffffffff);
	gdi->SetTextColor(color, 0xffffffff);
	if (s_iRankType == 2 && s_iSubType == 3)
	{
		unsigned short level = info->level + 1 < 71 ? info->level + 1 : 71;
		const Bitmap* bitmap = g_pFresh->GetManager()->GetBitmap("LEVELS",
			MakeStr("level_%03d", level));
		if (bitmap)
			gdi->DrawTexture(bitmap,
				WRect(item->pos.x + 240, item->pos.y - 4,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
	}
	else if ((int)info->value > 9999999999i64)
		gdi->PrintText(WPoint(item->pos.x + 310, item->pos.y), 2,
			K2L_Compatibility("\xb1\xe2\xb7\xcf\xbe\xf8\xc0\xbd"), NULL);
	else if (s_iRankType == 2 && s_iSubType == 2)
		gdi->PrintText(WPoint(item->pos.x + 310, item->pos.y), 2,
			MakeStr("%.1f", (float)(int)info->value * 0.1f), NULL);
	else if (s_iRankType == 2 && s_iSubType == 4)
		gdi->PrintText(WPoint(item->pos.x + 310, item->pos.y), 2,
			MakeStr(K2L_Compatibility("%d \xb8\xb8y"),
				(int)info->value / 10000),
			NULL);
	else
		gdi->PrintText(WPoint(item->pos.x + 310, item->pos.y), 2,
			MakeStr("%d", info->value), NULL);
	gdi->SetTextColor(0xffffffff, 0xffffffff);
}

void FrRankingDlg::ShowUserInfo()
{
	if (m_userInfo.uid == -1)
		return;
	WRect rect = m_pUpperMiniInfo->GetRect();
	WPoint pos(rect.x, rect.y);
	pos.y += 40;
	DrawUserInfoLine(1, 0, pos);
	DrawUserInfoLine(2, 1, pos);
	DrawUserInfoLine(3, 2, pos);
	DrawUserInfoLine(4, 3, pos);
	pos.y += 7;
	DrawUserInfoLine(0, 4, pos);
}

void FrRankingDlg::DrawUserInfoLine(int index, int row, WPoint& pos)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->SetTextStyle(0);
	unsigned long rank = m_userInfo.record[index].rank;
	const char* rankText;
	if (rank == 0 || rank >= 10000 || m_userInfo.record[index].value == 0)
		rankText = "- ";
	else
		rankText = MakeStr(K2L_Compatibility("%d\xc0\xa7"), rank);
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->PrintText(WPoint(pos.x + 100, pos.y + row * 15), 2, rankText, NULL);
	if (m_userInfo.record[index].value > 99999999)
		gdi->PrintText(WPoint(pos.x + 195, pos.y + row * 15), 2,
			K2L_Compatibility("\xb1\xe2\xb7\xcf\xbe\xf8\xc0\xbd"), NULL);
	else
		gdi->PrintText(WPoint(pos.x + 195, pos.y + row * 15), 2,
			MakeStr("%d", m_userInfo.record[index].value), NULL);
	unsigned long previous = m_userInfo.record[index].preRank;
	if (previous == 0 || fabs((float)rank - (float)previous) > 10000.f)
	{
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("ICONS", "new");
		if (bitmap)
			gdi->DrawTexture(bitmap,
				WRect(pos.x + 105, pos.y + row * 15 + 4, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else if (previous < rank)
	{
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("RANKING", "dn");
		if (bitmap)
			gdi->DrawTexture(bitmap,
				WRect(pos.x + 105, pos.y + row * 15 + 2, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
		gdi->SetTextColor(0xff0000ff, 0xffffffff);
		gdi->PrintText(WPoint(pos.x + bitmap->Width() + 105, pos.y + row * 15),
			0, MakeStr("%d", rank - previous), NULL);
		gdi->SetTextColor(0xffffffff, 0xffffffff);
	}
	else if (rank < previous)
	{
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("RANKING", "up");
		if (bitmap)
			gdi->DrawTexture(bitmap,
				WRect(pos.x + 105, pos.y + row * 15 + 2, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
		gdi->SetTextColor(0xffff0000, 0xffffffff);
		gdi->PrintText(WPoint(pos.x + bitmap->Width() + 105, pos.y + row * 15),
			0, MakeStr("%d", previous - rank), NULL);
		gdi->SetTextColor(0xffffffff, 0xffffffff);
	}
	else
	{
		gdi->SetTextColor(0xffffffff, 0xffffffff);
		gdi->PrintText(WPoint(pos.x + 113, pos.y + row * 15), 0, "-", NULL);
	}
}

void FrRankingDlg::DisconnectRankServer()
{
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_RANK))
		WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_RANK);
}

void FrRankingDlg::SetRankTypes(unsigned char rank, unsigned char sub,
	unsigned char term, unsigned char cls, int page, int total)
{
	s_iRankType = rank;
	s_iSubType = sub;
	s_iTermType = term;
	s_iClassType = cls;
	s_iPageNum = page;
	s_iPageTotal = total;
}

void FrRankingDlg::SelectRankList(int index)
{
	if (m_pRankList)
	{
		FrListItem* item = m_pRankList->GetItem(index);
		if (!item)
			return;
		m_pRankList->SelectItem(item, true);
	}
	ranktype::sListInfo* info =
		(ranktype::sListInfo*)m_pRankList->GetSelected()->pData;
	if (info)
		SendRequestUserInfo(info->uid, info->id.c_str());
}

void FrRankingDlg::SynchronizeButtons()
{
	if (m_pRankTypeList)
	{
		const char* name =
			((FrLine*)m_pRankTypeList->GetSelected()->pData)->text.c_str();
		if (!strcmpi(s_aRankType[s_iRankType].name, name))
		{
			m_pRankTypeList->SelectItem(m_pRankTypeList->GetItem(s_iRankType),
				true);
			m_pRankTypeList->SetVisible(false);
			SetVisibleByRankType();
		}
	}
	switch (s_iRankType)
	{
	case 0:
	{
		if (m_pGenSubTypeList)
		{
			m_pGenSubTypeList->SelectItem(
				m_pGenSubTypeList->GetItem(s_iSubType), true);
			m_pGenSubTypeList->SetVisible(false);
		}
	}
	break;
	case 1:
	{
		if (m_pCourseTypeList)
		{
			m_pCourseTypeList->SelectItem(
				m_pCourseTypeList->GetItem(s_iSubType), true);
			m_pCourseTypeList->SetVisible(false);
		}
	}
	break;
	case 2:
	{
		if (m_pRecordTypeList)
		{
			m_pRecordTypeList->SelectItem(
				m_pRecordTypeList->GetItem(s_iSubType), true);
			m_pRecordTypeList->SetVisible(false);
		}
	}
	break;
	}
}

void FrRankingDlg::CloseAllComboBtns()
{
	WRect rect;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	if (m_pRankTypeList && m_pRankTypeViewer && m_pRankTypeList->IsVisible())
	{
		rect = m_pRankTypeViewer->GetRect();
		if (!rect.IsInRect(mouse))
		{
			rect = m_pRankTypeList->GetRect();
			if (!rect.IsInRect(mouse))
				m_pRankTypeList->SetVisible(false);
		}
	}
	if (m_pGenSubTypeList && m_pGenSubTypeList->IsVisible())
	{
		rect = m_pGenSubTypeViewer->GetRect();
		if (!rect.IsInRect(mouse))
		{
			rect = m_pGenSubTypeList->GetRect();
			if (!rect.IsInRect(mouse))
				m_pGenSubTypeList->SetVisible(false);
		}
	}
	if (m_pCourseTypeList && m_pCourseTypeList->IsVisible())
	{
		rect = m_pCourseTypeViewer->GetRect();
		if (!rect.IsInRect(mouse))
		{
			rect = m_pCourseTypeList->GetRect();
			if (!rect.IsInRect(mouse))
				m_pCourseTypeList->SetVisible(false);
		}
	}
	if (m_pRecordTypeList && m_pRecordTypeList->IsVisible())
	{
		rect = m_pRecordTypeViewer->GetRect();
		if (!rect.IsInRect(mouse))
		{
			rect = m_pRecordTypeList->GetRect();
			if (!rect.IsInRect(mouse))
				m_pRecordTypeList->SetVisible(false);
		}
	}
	if (m_pSearchTypeList && m_pSearchTypeList->IsVisible())
	{
		rect = m_pSearchTypeViewer->GetRect();
		if (!rect.IsInRect(mouse))
		{
			rect = m_pSearchTypeList->GetRect();
			if (!rect.IsInRect(mouse))
				m_pSearchTypeList->SetVisible(false);
		}
	}
}

void FrRankingDlg::SetAllControlsEnable(bool enable)
{
	m_pTermTypeS5->Enable(enable);
	m_pRankList->Enable(enable);
	m_pMyInfo->Enable(enable);
	m_pSearchUser->Enable(enable);
	m_pRankTypeList->Enable(enable);
	m_pRankTypeViewer->Enable(enable);
	m_pGenSubTypeList->Enable(enable);
	m_pGenSubTypeViewer->Enable(enable);
	m_pCourseTypeList->Enable(enable);
	m_pCourseTypeViewer->Enable(enable);
	m_pRecordTypeList->Enable(enable);
	m_pRecordTypeViewer->Enable(enable);
	m_pSearchTypeList->Enable(enable);
	m_pSearchTypeViewer->Enable(enable);
}

void FrRankingDlg::HidePet(bool interrupt)
{
	// HACK!
	if (interrupt)
	{
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
		if (m_pPartTidList)
		{
			delete m_pPartTidList;
			m_pPartTidList = NULL;
		}
	}
	else
	{
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
		if (m_pPartTidList)
		{
			delete m_pPartTidList;
			m_pPartTidList = NULL;
		}
	}
}

void FrRankingDlg::SendRequestRank(bool reset)
{
	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_RANK))
		return;
	WSendPacket packet(0);
	packet.Encode4(MyUID());
	packet.EncodeStr(std::string(MyId()));
	packet.Encode1((unsigned char)s_iRankType);
	packet.Encode1((unsigned char)s_iSubType);
	packet.Encode1((unsigned char)s_iTermType);
	packet.Encode1((unsigned char)s_iClassType);
	packet.Encode4(s_iPageNum);
	packet.Encode1(reset);
	packet.Send(TO_RANK);
	m_requestDelay = 1.f;
	SetAllControlsEnable(false);
	RANKINGDLG()->SetBottomMsg("");
}

void FrRankingDlg::SendRequestUserInfo(unsigned long uid, const char* id)
{
	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_RANK) ||
		uid == -1)
		return;
	WSendPacket packet(1);
	packet.Encode4(uid);
	packet.EncodeStr(id);
	packet.Encode1((unsigned char)s_iTermType);
	packet.Send(TO_RANK);
	m_requestDelay = 1.f;
	SetVisibleUserInfo(false);
	SetAllControlsEnable(false);
	RANKINGDLG()->SetBottomMsg("");
}

void FrRankingDlg::SendRequestSearchUser(const char* text)
{
	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_RANK))
		return;
	int empty = strcmp(text, "");
	if (empty == 0)
	{
		OnNotify(
			K2L_Compatibility(
				"\xc3\xa3\xc0\xbb \xb4\xeb\xbb\xf3\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xd8\xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0."),
			2);
		return;
	}
	if (strstr(text, "'") || strstr(text, ";") || strstr(text, "\""))
	{
		OnNotify(
			K2L_Compatibility(
				"\xc6\xaf\xbc\xf6\xb1\xe2\xc8\xa3 ' ; \042 \xb4\xc2 \xc0\xd4\xb7\xc2\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."),
			2);
		return;
	}
	WSendPacket packet(2);
	packet.Encode1((unsigned char)s_iSearchType);
	if (s_iSearchType == 1)
	{
		int rank = atoi(text);
		if (rank <= 0)
		{
			OnNotify(
				K2L_Compatibility(
					"\xbc\xfd\xc0\xda\xb0\xa1 \xbe\xc6\xb4\xcf\xb0\xc5\xb3\xaa \xb0\xaa\xc0\xcc \xb3\xca\xb9\xab \xc0\xdb\xbd\xc0\xb4\xcf\xb4\xd9."),
				2);
			return;
		}
		if (rank > 100000)
		{
			OnNotify(
				K2L_Compatibility(
					"\xc3\xa3\xb4\xc2 \xbc\xf8\xc0\xa7\xc0\xc7 \xb0\xaa\xc0\xcc \xb3\xca\xb9\xab \xc5\xae\xb4\xcf\xb4\xd9."),
				2);
			return;
		}
		packet.Encode4(rank);
	}
	else
	{
		packet.EncodeStr(std::string(text));
	}
	packet.Encode1((unsigned char)s_iRankType);
	packet.Encode1((unsigned char)s_iSubType);
	packet.Encode1((unsigned char)s_iTermType);
	packet.Encode1((unsigned char)s_iClassType);
	packet.Encode4(s_iPageNum);
	packet.Send(TO_RANK);
	m_requestDelay = 1.f;
	SetAllControlsEnable(false);
	RANKINGDLG()->SetBottomMsg("");
}

void FrRankingDlg::OnTermType_S5_BtnInit(int param)
{
	m_pTermTypeS5 = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pTermTypeS5->SetPushDelay(1.f);
	m_pTermTypeS5->SetStatus(FrButton::PRESSED);
}

void FrRankingDlg::OnTermType_S5_BtnUp()
{
	s_iTermType = 0;
	m_pTermTypeS5->SetStatus(FrButton::PRESSED);
	s_iPageNum = 0;
	SendRequestRank(true);
	SendRequestUserInfo(m_userInfo.uid, m_userInfo.id);
}

void FrRankingDlg::OnCourseAreaInit(int param)
{
	m_pCourseArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrRankingDlg::OnCourseAreaOwnerDraw(int param)
{
	// HACK: There is a dead branch here somewhere. We don't know it exactly.
	if (false)
		CreateForm<FrRankingDlg>(g_pFresh->GetManager(), NULL, "ranking", NULL);
}

void FrRankingDlg::OnRankList_Init(int param)
{
	if (m_pRankList)
		m_pRankList->ClearItem();
	else
		m_pRankList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrRankingDlg::OnRankList_OwnerDraw(int param)
{
	if (!m_rankInfoList.empty() && param)
		SetListItemInfo((FrListItem*)param);
}

void FrRankingDlg::OnRankList_LBtnDown()
{
	if (m_requestDelay > 0)
		return;
	if (!m_anotherHand.IsWorking(0))
	{
		FrListItem* item = m_pRankList->GetItemUnderCursor();
		if (item)
		{
			ranktype::sListInfo* info = (ranktype::sListInfo*)item->pData;
			if (info && strcmpi(info->id.c_str(), m_userInfo.id))
				SendRequestUserInfo(info->uid, info->id.c_str());
		}
	}
}

void FrRankingDlg::OnRankList_PrevPageInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	button->SetPushDelay(1.f);
}

void FrRankingDlg::OnRankList_PrevPageBtnUp()
{
	if (s_iPageNum > 0)
	{
		--s_iPageNum;
		SendRequestRank(false);
	}
	else
	{
		SetBottomMsg(K2L_Compatibility(
			"\xb4\xf5 \xc0\xcc\xbb\xf3 \xb7\xa9\xc5\xb7 \xb1\xe2\xb7\xcf\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	}
}

void FrRankingDlg::OnRankList_NextPageInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	button->SetPushDelay(1.f);
}

void FrRankingDlg::OnRankList_NextPageBtnUp()
{
	++s_iPageNum;
	SendRequestRank(false);
}

void FrRankingDlg::OnRankList_CurrentPageInit(int param)
{
	m_pCurrentPage = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrRankingDlg::OnRankList_TotalPageInit(int param)
{
	m_pTotalPage = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrRankingDlg::OnRankList_MyInfo_Init(int param)
{
	if (m_pMyInfo)
		m_pMyInfo->ClearItem();
	else
		m_pMyInfo = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pMyInfo->UseRightButton(true);
}

void FrRankingDlg::OnRankList_MyInfo_LBtnDown()
{
	if (m_requestDelay > 0)
		return;
	if (!m_anotherHand.IsWorking(0))
	{
		FrListItem* item = m_pMyInfo->GetItemUnderCursor();
		if (item)
		{
			ranktype::sListInfo* info = (ranktype::sListInfo*)item->pData;
			if (info && strcmpi(info->id.c_str(), m_userInfo.id))
				SendRequestUserInfo(info->uid, info->id.c_str());
		}
	}
}

void FrRankingDlg::OnRankList_MyInfo_OwnerDraw(int param)
{
	if (param)
		SetListItemInfo((FrListItem*)param);
}

void FrRankingDlg::OnRankList_NoInRank_Init(int param)
{
	m_pNoInRank = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pNoInRank->SetVisible(false);
}

void FrRankingDlg::OnRankList_SearchUserInit(int param)
{
	m_pSearchUser = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pSearchUser->SetPushDelay(0.1f);
}

void FrRankingDlg::OnRankList_SearchUserBtnUp()
{
	char text[260];
	text[0] = 0;
	if (m_pUserIdEdit)
	{
		strcpy(text, m_pUserIdEdit->GetLine(1, false));
		TrimA(text, " \t\n\r\0");
	}
	SendRequestSearchUser(text);
}

void FrRankingDlg::OnRankList_UserIdEditInit(int param)
{
	m_pUserIdEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_pUserIdEdit->SetCharLimit(32, true);
}

void FrRankingDlg::OnRankList_UserIdEditDown()
{
}

void FrRankingDlg::OnEmoticonInit(int param)
{
	m_pEmoticon = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRankingDlg::OnEmoticonBtnUp()
{
	if (m_pEmoticon && !m_pEmoticonDlg)
	{
		m_pEmoticonDlg = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this,
			"emoticon", NULL);
		if (m_pEmoticonDlg)
		{
			WPoint pos(0, 0);
			if (m_pEmoticon)
			{
				pos.x = m_pEmoticon->GetRect().x - 200;
				pos.y = m_pEmoticon->GetRect().y - 260;
			}
			m_pEmoticonDlg->Open(
				(FRESH_PFN_RESULT)&FrRankingDlg::OnEmoticonResult, pos, 3);
		}
	}
}

void FrRankingDlg::OnLanguageInit(int param)
{
	m_pLanguage = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (!m_pLanguage)
		return;
	if (g_ime->IsAlphaNumericMode())
		m_pLanguage->SetBgImg("chat_eng");
	else
		m_pLanguage->SetBgImg("chat_kor");
}

void FrRankingDlg::OnAboutBtnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRankingDlg::OnAboutBtnUp()
{
	m_pAboutDlg = CreateForm<FrRankingAboutDlg>(g_pFresh->GetManager(), this,
		"ranking_about", NULL);
	if (m_pAboutDlg)
		m_pAboutDlg->Open((FRESH_PFN_RESULT)&FrRankingDlg::OnAboutDlgResult, 3);
}

void FrRankingDlg::OnCloseBtnUp()
{
	Close(true);
}

void FrRankingDlg::OnBottomMessageInit(int param)
{
	m_pBottomMessage = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pBottomMessage->SetVisible(false);
}

void FrRankingDlg::OnPetCharacterInit(int param)
{
	m_pPetCharacter = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrRankingDlg::OnPetCharacterOwnerDraw()
{
	if ((!m_pPetLoad || !m_pPetLoad->IsVisible()) &&
		!m_anotherHand.IsWorking(0))
	{
		m_anotherHand.Enter();
		if (m_pExhibition)
			m_pExhibition->Display(0.f, 0.f);
		m_anotherHand.Leave();
	}
}

void FrRankingDlg::OnPetLoadInit(int param)
{
	m_pPetLoad = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	if (m_pPetLoad)
	{
		m_pPetLoad->SetRange(0, 150, 0);
		m_pPetLoad->SetVisible(false);
	}
}

void FrRankingDlg::OnUpperMiniInfoInit(int param)
{
	m_pUpperMiniInfo = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrRankingDlg::OnUpperMiniInfoOwnerDraw()
{
	if (!m_pUpperMiniInfo)
		return;
	WRect rect = m_pUpperMiniInfo->GetRect();
	WPoint pos(rect.x, rect.y);
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (m_userInfo.uid != -1)
	{
		unsigned char level =
			(m_userInfo.level + 1 < 71) ? m_userInfo.level + 1 : 71;
		const Bitmap* bitmap = g_pFresh->GetManager()->GetBitmap("LEVELS",
			MakeStr("level_%03d", (unsigned char)level));
		if (bitmap)
			gdi->DrawTexture(bitmap,
				WRect(pos.x + 5, pos.y + 6, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
		char nickname[256];
		sprintf(nickname, "%s", m_userInfo.nickname);
		gdi->SetTextStyle(0);
		if (!CProjectG::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(WPoint(pos.x + 60, pos.y + 9), 0,
				nickname, -1.f, 0xffffffff);
	}
	ShowUserInfo();
}

void FrRankingDlg::OnClassType_ComboInit(int param)
{
	m_pClassTypeCombo = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
	for (int i = 0; i < 4; ++i)
		m_pClassTypeCombo->AddString(s_aClassType[i].name);
	m_pClassTypeCombo->SetLine(1, s_aClassType[s_iClassType].name, 0, false, 0);
	m_pClassTypeCombo->SetVisible(false);
}

void FrRankingDlg::OnClassType_ComboDown(int param)
{
	const char* text = m_pClassTypeCombo->GetLine(1, false);
	int oldType = s_iClassType;
	s_iClassType = m_pClassTypeCombo->FindString(text) - 1;
	if (s_iClassType < 0)
		s_iClassType = 0;
	else if (s_iClassType > 3)
		s_iClassType = 3;
	if (s_iClassType != oldType)
		SendRequestRank(true);
}

void FrRankingDlg::OnRankTypeListInit(int param)
{
	m_pRankTypeList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pRankTypeList)
		return;
	for (int i = 0; i < 3; ++i)
	{
		m_rankTypeLine[i].text = std::string(s_aRankType[i].name);
		m_pRankTypeList->AddItem(&m_rankTypeLine[i]);
	}
	m_pRankTypeList->SelectItem(m_pRankTypeList->FindItem(&m_rankTypeLine[0]),
		true);
	m_pRankTypeList->SetVisible(false);
	m_pRankTypeList->EnableScrollBar(false);
}

void FrRankingDlg::OnRankTypeListLDown()
{
	FrListItem* item = m_pRankTypeList->GetItemUnderCursor();
	if (!item)
		return;
	m_pRankTypeList->SelectItem(item, true);
	m_pRankTypeList->SetVisible(false);
	s_iRankType = item->idx;
	if (s_iRankType < 0)
		s_iRankType = 0;
	else if (s_iRankType > 2)
		s_iRankType = 2;
	s_iSubType = 0;
	s_iPageNum = 0;
	SendRequestRank(true);
	SetVisibleByRankType();
}

void FrRankingDlg::OnRankTypeListOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	FrListItem* item = (FrListItem*)param;
	if (!gdi || !item)
		return;
	FrLine* line = (FrLine*)item->pData;
	WPoint pos(item->pos.x, item->pos.y);
	WOverlay::DrawBox(g_view, WRect(pos, 82.f, 22.f), 0, 0xaa0e4f75, 0.f);
	if (item->underCursor)
	{
		gdi->Box(WRect(item->pos.x + 1, item->pos.y + 1, 80.f, 20.f),
			0x80082b46, 0, 0.f);
		gdi->SetTextStyle(1);
	}
	else
	{
		gdi->SetTextStyle(0);
	}
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(item->pos.x + 10, item->pos.y + 6), 0,
		line->text.c_str());
}

void FrRankingDlg::OnRankTypeViewerInit(int param)
{
	m_pRankTypeViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_pRankTypeBtn[0] = g_pFresh->RegisterBitmap("btn_drop01_n");
	m_pRankTypeBtn[1] = g_pFresh->RegisterBitmap("btn_drop01_o");
	m_pRankTypeBtn[2] = g_pFresh->RegisterBitmap("btn_drop01_d");
}

void FrRankingDlg::OnRankTypeViewerDown()
{
	if (m_pRankTypeList)
	{
		m_pRankTypeList->SetVisible(!m_pRankTypeList->IsVisible());
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.f);
	}
}

void FrRankingDlg::OnRankTypeViewerOwnerDraw()
{
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* gdi = manager->GetGDI();
	if (!gdi || !m_pRankTypeViewer)
		return;
	int state = 0;
	if (m_pRankTypeList->IsVisible())
	{
		state = 2;
	}
	else
	{
		WRect rect = m_pRankTypeViewer->GetRect();
		if (rect.IsInRect(manager->GetMousePos()))
			state = 1;
	}
	gdi->DrawTexture(m_pRankTypeBtn[state], m_pRankTypeViewer->GetRect(),
		0xffffffff, 0);
	FrLine* line = (FrLine*)m_pRankTypeList->GetSelected()->pData;
	WRect rect = m_pRankTypeViewer->GetRect();
	WPoint pos = rect.TopLeft();
	WOverlay::DrawBox(g_view, WRect(rect), 0, 0x800e4f75, 0.f);
	gdi->SetTextStyle(1);
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(pos.x + 4, pos.y + 5), 0, line->text.c_str());
}

void FrRankingDlg::OnGeneralSubTypeListInit(int param)
{
	m_pGenSubTypeList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pGenSubTypeList)
		return;
	for (int i = 0; i < 5; ++i)
	{
		m_genSubTypeLine[i].text = std::string(s_aGenSubType[i].name);
		m_pGenSubTypeList->AddItem(&m_genSubTypeLine[i]);
	}
	m_pGenSubTypeList->SelectItem(
		m_pGenSubTypeList->FindItem(&m_genSubTypeLine[0]), true);
	m_pGenSubTypeList->SetVisible(false);
	m_pGenSubTypeList->EnableScrollBar(false);
}

void FrRankingDlg::OnGeneralSubTypeListLDown()
{
	FrListItem* item = m_pGenSubTypeList->GetItemUnderCursor();
	if (!item)
		return;
	m_pGenSubTypeList->SelectItem(item, true);
	m_pGenSubTypeList->SetVisible(false);
	s_iSubType = item->idx;
	if (s_iSubType < 0)
		s_iSubType = 0;
	else if (s_iSubType > 4)
		s_iSubType = 4;
	s_iPageNum = 0;
	SendRequestRank(true);
}

void FrRankingDlg::OnGeneralSubTypeListOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	FrListItem* item = (FrListItem*)param;
	if (!gdi || !item)
		return;
	FrLine* line = (FrLine*)item->pData;
	WPoint pos(item->pos.x, item->pos.y);
	WOverlay::DrawBox(g_view, WRect(pos, 91.f, 22.f), 0, 0xaa0e4f75, 0.f);
	if (item->underCursor)
	{
		gdi->Box(WRect(item->pos.x + 1, item->pos.y + 1, 90.f, 20.f),
			0x80082b46, 0, 0.f);
		gdi->SetTextStyle(1);
	}
	else
	{
		gdi->SetTextStyle(0);
	}
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(item->pos.x + 10, item->pos.y + 6), 0,
		line->text.c_str());
}

void FrRankingDlg::OnGeneralSubTypeViewerInit(int param)
{
	m_pGenSubTypeViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_pGenSubTypeBtn[0] = g_pFresh->RegisterBitmap("btn_drop02_n");
	m_pGenSubTypeBtn[1] = g_pFresh->RegisterBitmap("btn_drop02_o");
	m_pGenSubTypeBtn[2] = g_pFresh->RegisterBitmap("btn_drop02_d");
}

void FrRankingDlg::OnGeneralSubTypeViewerDown()
{
	if (m_pGenSubTypeList)
	{
		m_pGenSubTypeList->SetVisible(!m_pGenSubTypeList->IsVisible());
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.f);
	}
}

void FrRankingDlg::OnGeneralSubTypeViewerOwnerDraw()
{
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* gdi = manager->GetGDI();
	if (!gdi || !m_pGenSubTypeViewer)
		return;
	int state = 0;
	if (m_pGenSubTypeList->IsVisible())
	{
		state = 2;
	}
	else
	{
		WRect rect = m_pGenSubTypeViewer->GetRect();
		if (rect.IsInRect(manager->GetMousePos()))
			state = 1;
	}
	gdi->DrawTexture(m_pGenSubTypeBtn[state], m_pGenSubTypeViewer->GetRect(),
		0xffffffff, 0);
	FrLine* line = (FrLine*)m_pGenSubTypeList->GetSelected()->pData;
	WRect rect = m_pGenSubTypeViewer->GetRect();
	WPoint pos = rect.TopLeft();
	WOverlay::DrawBox(g_view, WRect(rect), 0, 0x800e4f75, 0.f);
	gdi->SetTextStyle(1);
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(pos.x + 4, pos.y + 5), 0, line->text.c_str());
}

void FrRankingDlg::OnCourseTypeListInit(int param)
{
	m_pCourseTypeList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pCourseTypeList)
		return;
	for (int i = 0; i < 17; ++i)
	{
		IFF_STRUCT::sCourse* course =
			ItemManager()->FindCourse(0x28000000 | GetRealCourseType(i));
		if (course)
		{
			m_courseTypeLine[i].text = std::string(s_aCourseType[i].name);
			m_pCourseTypeList->AddItem(&m_courseTypeLine[i]);
		}
	}
	WPoint pos(m_pCourseTypeList->GetRect().x, m_pCourseTypeList->GetRect().y);
	WRect rect(pos, 107.f, 374.f);
	m_pCourseTypeList->SetRect(rect);
	m_pCourseTypeList->SelectItem(
		m_pCourseTypeList->FindItem(&m_courseTypeLine[0]), true);
	m_pCourseTypeList->SetVisible(false);
	m_pCourseTypeList->EnableScrollBar(false);
}

void FrRankingDlg::OnCourseTypeListLDown()
{
	FrListItem* item = m_pCourseTypeList->GetItemUnderCursor();
	if (!item)
		return;
	m_pCourseTypeList->SelectItem(item, true);
	m_pCourseTypeList->SetVisible(false);
	s_iSubType = item->idx;
	if (s_iSubType < 0)
		s_iSubType = 0;
	else if (s_iSubType > 16)
		s_iSubType = 16;
	s_iPageNum = 0;
	SendRequestRank(true);
}

void FrRankingDlg::OnCourseTypeListOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	FrListItem* item = (FrListItem*)param;
	if (!gdi || !item)
		return;
	FrLine* line = (FrLine*)item->pData;
	WPoint pos(item->pos.x, item->pos.y);
	WOverlay::DrawBox(g_view, WRect(pos, 91.f, 22.f), 0, 0xaa0e4f75, 0.f);
	if (item->underCursor)
	{
		gdi->Box(WRect(item->pos.x + 1, item->pos.y + 1, 90.f, 20.f),
			0x80082b46, 0, 0.f);
		gdi->SetTextStyle(1);
	}
	else
		gdi->SetTextStyle(0);
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(item->pos.x + 10, item->pos.y + 6), 0,
		line->text.c_str());
}

void FrRankingDlg::OnCourseTypeViewerInit(int param)
{
	m_pCourseTypeViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_pCourseTypeBtn[0] = g_pFresh->RegisterBitmap("btn_drop02_n");
	m_pCourseTypeBtn[1] = g_pFresh->RegisterBitmap("btn_drop02_o");
	m_pCourseTypeBtn[2] = g_pFresh->RegisterBitmap("btn_drop02_d");
}

void FrRankingDlg::OnCourseTypeViewerDown()
{
	if (!m_pCourseTypeList)
		return;
	m_pCourseTypeList->SetVisible(!m_pCourseTypeList->IsVisible());
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.f);
}

void FrRankingDlg::OnCourseTypeViewerOwnerDraw()
{
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* gdi = manager->GetGDI();
	if (!gdi || !m_pCourseTypeViewer)
		return;
	int state = 0;
	if (m_pCourseTypeList->IsVisible())
	{
		state = 2;
	}
	else
	{
		WRect rect = m_pCourseTypeViewer->GetRect();
		if (rect.IsInRect(manager->GetMousePos()))
			state = 1;
	}
	gdi->DrawTexture(m_pCourseTypeBtn[state], m_pCourseTypeViewer->GetRect(),
		0xffffffff, 0);
	FrLine* line = (FrLine*)m_pCourseTypeList->GetSelected()->pData;
	WRect rect = m_pCourseTypeViewer->GetRect();
	WPoint pos = rect.TopLeft();
	WOverlay::DrawBox(g_view, WRect(rect), 0, 0x800e4f75, 0.f);
	gdi->SetTextStyle(1);
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(pos.x + 4, pos.y + 5), 0, line->text.c_str());
}

void FrRankingDlg::OnRecordTypeListInit(int param)
{
	m_pRecordTypeList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pRecordTypeList)
		return;
	for (int i = 0; i < 5; ++i)
	{
		m_recordTypeLine[i].text = std::string(s_aRecordType[i].name);
		m_pRecordTypeList->AddItem(&m_recordTypeLine[i]);
	}
	m_pRecordTypeList->SelectItem(
		m_pRecordTypeList->FindItem(&m_recordTypeLine[0]), true);
	m_pRecordTypeList->SetVisible(false);
	m_pRecordTypeList->EnableScrollBar(false);
}

void FrRankingDlg::OnRecordTypeListLDown()
{
	FrListItem* item = m_pRecordTypeList->GetItemUnderCursor();
	if (!item)
		return;
	m_pRecordTypeList->SelectItem(item, true);
	m_pRecordTypeList->SetVisible(false);
	s_iSubType = item->idx;
	if (s_iSubType < 0)
		s_iSubType = 0;
	else if (s_iSubType > 4)
		s_iSubType = 4;
	s_iPageNum = 0;
	SendRequestRank(true);
}

void FrRankingDlg::OnRecordTypeListOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	FrListItem* item = (FrListItem*)param;
	if (!gdi || !item)
		return;
	FrLine* line = (FrLine*)item->pData;
	WPoint pos(item->pos.x, item->pos.y);
	WOverlay::DrawBox(g_view, WRect(pos, 91.f, 22.f), 0, 0xaa0e4f75, 0.f);
	if (item->underCursor)
	{
		gdi->Box(WRect(item->pos.x + 1, item->pos.y + 1, 90.f, 20.f),
			0x80082b46, 0, 0.f);
		gdi->SetTextStyle(1);
	}
	else
	{
		gdi->SetTextStyle(0);
	}
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(item->pos.x + 10, item->pos.y + 6), 0,
		line->text.c_str());
}

void FrRankingDlg::OnRecordTypeViewerInit(int param)
{
	m_pRecordTypeViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_pRecordTypeBtn[0] = g_pFresh->RegisterBitmap("btn_drop02_n");
	m_pRecordTypeBtn[1] = g_pFresh->RegisterBitmap("btn_drop02_o");
	m_pRecordTypeBtn[2] = g_pFresh->RegisterBitmap("btn_drop02_d");
}

void FrRankingDlg::OnRecordTypeViewerDown()
{
	if (m_pRecordTypeList)
	{
		m_pRecordTypeList->SetVisible(!m_pRecordTypeList->IsVisible());
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.f);
	}
}

void FrRankingDlg::OnRecordTypeViewerOwnerDraw()
{
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* gdi = manager->GetGDI();
	if (!gdi || !m_pRecordTypeViewer)
		return;
	int state = 0;
	if (m_pRecordTypeList->IsVisible())
	{
		state = 2;
	}
	else
	{
		WRect rect = m_pRecordTypeViewer->GetRect();
		if (rect.IsInRect(manager->GetMousePos()))
			state = 1;
	}
	gdi->DrawTexture(m_pRecordTypeBtn[state], m_pRecordTypeViewer->GetRect(),
		0xffffffff, 0);
	FrLine* line = (FrLine*)m_pRecordTypeList->GetSelected()->pData;
	WRect rect = m_pRecordTypeViewer->GetRect();
	WPoint pos = rect.TopLeft();
	WOverlay::DrawBox(g_view, WRect(rect), 0, 0x800e4f75, 0.f);
	gdi->SetTextStyle(1);
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(pos.x + 6, pos.y + 5), 0, line->text.c_str());
}

void FrRankingDlg::OnSearchTypeListInit(int param)
{
	m_pSearchTypeList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pSearchTypeList)
		return;
	for (int i = 0; i < 2; ++i)
	{
		m_searchTypeLine[i].text = std::string(s_aSearchType[i].name);
		m_pSearchTypeList->AddItem(&m_searchTypeLine[i]);
	}
	m_pSearchTypeList->SelectItem(
		m_pSearchTypeList->FindItem(&m_searchTypeLine[0]), true);
	m_pSearchTypeList->SetVisible(false);
	m_pSearchTypeList->EnableScrollBar(false);
}

void FrRankingDlg::OnSearchTypeListLDown()
{
	FrListItem* item = m_pSearchTypeList->GetItemUnderCursor();
	if (!item)
		return;
	m_pSearchTypeList->SelectItem(item, true);
	m_pSearchTypeList->SetVisible(false);
	s_iSearchType = item->idx;
}

void FrRankingDlg::OnSearchTypeListOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	FrListItem* item = (FrListItem*)param;
	if (!gdi || !item)
		return;
	FrLine* line = (FrLine*)item->pData;
	WPoint pos(item->pos.x, item->pos.y);
	WOverlay::DrawBox(g_view, WRect(pos, 62.f, 22.f), 0, 0xaa0e4f75, 0.f);
	if (item->underCursor)
	{
		gdi->Box(WRect(item->pos.x + 1, item->pos.y + 1, 62.f, 20.f),
			0x80082b46, 0, 0.f);
		gdi->SetTextStyle(1);
	}
	else
	{
		gdi->SetTextStyle(0);
	}
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(item->pos.x + 10, item->pos.y + 6), 0,
		line->text.c_str());
}

void FrRankingDlg::OnSearchTypeViewerInit(int param)
{
	m_pSearchTypeViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_pSearchTypeBtn[0] = g_pFresh->RegisterBitmap("btn_drop03_n");
	m_pSearchTypeBtn[1] = g_pFresh->RegisterBitmap("btn_drop03_o");
	m_pSearchTypeBtn[2] = g_pFresh->RegisterBitmap("btn_drop03_d");
}

void FrRankingDlg::OnSearchTypeViewerDown()
{
	if (m_pSearchTypeList)
	{
		m_pSearchTypeList->SetVisible(!m_pSearchTypeList->IsVisible());
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.f);
	}
}

void FrRankingDlg::OnSearchTypeViewerOwnerDraw()
{
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* gdi = manager->GetGDI();
	if (!gdi || !m_pSearchTypeViewer)
		return;
	int state = 0;
	if (m_pSearchTypeList->IsVisible())
	{
		state = 2;
	}
	else
	{
		WRect rect = m_pSearchTypeViewer->GetRect();
		if (rect.IsInRect(manager->GetMousePos()))
			state = 1;
	}
	gdi->DrawTexture(m_pSearchTypeBtn[state], m_pSearchTypeViewer->GetRect(),
		0xffffffff, 0);
	FrLine* line = (FrLine*)m_pSearchTypeList->GetSelected()->pData;
	WRect rect = m_pSearchTypeViewer->GetRect();
	WPoint pos = rect.TopLeft();
	WOverlay::DrawBox(g_view, WRect(rect), 0, 0x000e4f75, 0.f);
	gdi->SetTextStyle(1);
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	gdi->Print(WPoint(pos.x + 4, pos.y + 5), 0, line->text.c_str());
}

void FrRankingDlg::OnUserRankEditInit(int param)
{
	m_pUserRankEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_pUserRankEdit->SetVisible(false);
}

void FrRankingDlg::OnNotify(const char* msg, unsigned long type)
{
	FrForm* form =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
	form->SetTimeLimit(1.f);
	form->SetMessage(msg, false);
	form->Open(NULL, type);
	m_pBottomMessage->SetVisible(false);
}

void FrRankingDlg::SetBottomMsg(const char* msg)
{
	m_pBottomMessage->SetCaption(msg);
	m_pBottomMessage->SetVisible(true);
}

void FrRankingDlg::OnCustomProc(float delta)
{
	if (m_anotherHand.IsWorking(0))
	{
		if (m_pGhostBtn && m_pGhostBtn->IsVisible())
			m_pGhostBtn->SetVisible(false);
		Sleep(10);
		return;
	}
	m_anotherHand.Enter();
	if (m_pExhibition && m_pPetLoad && !m_pPetLoad->IsVisible())
	{
		WRect rect = m_pPetCharacter->GetRect();
		m_pExhibition->SetCenter(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f);
		m_pExhibition->Process(delta, true);
	}
	m_anotherHand.Leave();
}

bool FrRankingDlg::OnEmoticonResult(int result, FrForm* form)
{
	if (m_pEmoticonDlg && result == 1)
	{
		FrEmoticonDlg* dlg = DYNAMIC_CAST(FrEmoticonDlg, form);
		if (dlg && dlg->GetSelectedIcon() && m_pUserIdEdit)
		{
			FrEdit* edit = m_pUserIdEdit;
			edit->SetLine(1,
				MakeStr("%s%s", edit->GetEditText_Front(),
					dlg->GetSelectedIcon()),
				0, false, 0);
			edit->SetKeyFocus(true);
		}
	}
	m_pEmoticonDlg = NULL;
	return true;
}

bool FrRankingDlg::OnAboutDlgResult(int result, FrForm* form)
{
	m_pAboutDlg = NULL;
	return true;
}

void FrRankingDlg::OnGhostBtnInit(int param)
{
	m_pGhostBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pGhostBtn->SetVisible(false);
	m_pGhostBtn->Enable(false);
}

void FrRankingDlg::OnGhostBtnUp()
{
	FrForm* form =
		CreateForm<FrForm>(g_pFresh->GetManager(), NULL, "notify", NULL);
	form->SetTimeLimit(5.f);
	form->SetMessage(
		K2L_Compatibility(
			"\xbc\xad\xba\xf1\xbd\xba \xc1\xd8\xba\xf1 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9."),
		false);
	form->Open(NULL, 3);
}

CRankingInfo::CRankingInfo()
{
	m_pDlg = NULL;
}

bool CRankingInfo::Open()
{
	if (!m_pDlg)
	{
		m_pDlg = CreateForm<FrRankingDlg>(g_pFresh->GetManager(), this,
			"rankingdlg", NULL);
		m_pDlg->Open((FRESH_PFN_RESULT)&CRankingInfo::OnRankingDlgResult, 1);
		m_pDlg->EnableDrag(false);
		if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
			ntAvatarManager::Instance()->SetActive(false);
	}
	return true;
}

void CRankingInfo::Close()
{
	if (m_pDlg)
		m_pDlg->Close(true);
}

bool CRankingInfo::OnRankingDlgResult(int result, FrForm* form)
{
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_RANK))
		WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_RANK);
	m_pDlg = NULL;
	if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
		ntAvatarManager::Instance()->SetActive(true);
	return true;
}

void CRankingInfo::Process(float delta)
{
	if (m_pDlg)
		m_pDlg->OnCustomProc(delta);
}

void CRankingInfo::OnNotify(const char* msg, unsigned long type)
{
	if (m_pDlg)
	{
		m_pDlg->OnNotify(msg, type);
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetTimeLimit(3.f);
		form->SetMessage(msg, 0);
		form->Open(NULL, type);
	}
}

IMPLEMENT_OBJECT(FrRankingAboutDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrRankingAboutDlg, FrForm)
ON_FRESH_VI("aboutviewer", FRCMD_INIT, FrRankingAboutDlg::OnAboutViewerInit)
ON_FRESH_VI("index01", FRCMD_INIT, FrRankingAboutDlg::OnIndex01Init)
ON_FRESH_VV("index01", FRCMD_LBUTTONUP, FrRankingAboutDlg::OnIndex01BnUp)
ON_FRESH_VI("index02", FRCMD_INIT, FrRankingAboutDlg::OnIndex02Init)
ON_FRESH_VV("index02", FRCMD_LBUTTONUP, FrRankingAboutDlg::OnIndex02BnUp)
ON_FRESH_VI("index03", FRCMD_INIT, FrRankingAboutDlg::OnIndex03Init)
ON_FRESH_VV("index03", FRCMD_LBUTTONUP, FrRankingAboutDlg::OnIndex03BnUp)
ON_FRESH_VI("index04", FRCMD_INIT, FrRankingAboutDlg::OnIndex04Init)
ON_FRESH_VV("index04", FRCMD_LBUTTONUP, FrRankingAboutDlg::OnIndex04BnUp)
ON_FRESH_VI("backward", FRCMD_INIT, FrRankingAboutDlg::OnBackwardBnInit)
ON_FRESH_VV("backward", FRCMD_LBUTTONUP, FrRankingAboutDlg::OnBackwardBnUp)
ON_FRESH_VI("forward", FRCMD_INIT, FrRankingAboutDlg::OnForwardBnInit)
ON_FRESH_VV("forward", FRCMD_LBUTTONUP, FrRankingAboutDlg::OnForwardBnUp)
ON_FRESH_VI("ok", FRCMD_INIT, FrRankingAboutDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrRankingAboutDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrRankingAboutDlg::FrRankingAboutDlg()
{
	for (int i = 0; i < 4; ++i)
		m_pIndex[i] = NULL;
	m_index = 0;
	m_pBackward = NULL;
	m_pForward = NULL;
}

FrRankingAboutDlg::~FrRankingAboutDlg()
{
}

void FrRankingAboutDlg::OnAboutViewerInit(int param)
{
	m_pViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

void FrRankingAboutDlg::OnIndex01Init(int param)
{
	m_pIndex[0] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrRankingAboutDlg::OnIndex01BnUp()
{
	SetImgAboutIndex(0);
}

void FrRankingAboutDlg::OnIndex02Init(int param)
{
	m_pIndex[1] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrRankingAboutDlg::OnIndex02BnUp()
{
	SetImgAboutIndex(1);
}

void FrRankingAboutDlg::OnIndex03Init(int param)
{
	m_pIndex[2] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrRankingAboutDlg::OnIndex03BnUp()
{
	SetImgAboutIndex(2);
}

void FrRankingAboutDlg::OnIndex04Init(int param)
{
	m_pIndex[3] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrRankingAboutDlg::OnIndex04BnUp()
{
	SetImgAboutIndex(3);
}

void FrRankingAboutDlg::OnBackwardBnInit(int param)
{
	m_pBackward = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRankingAboutDlg::OnBackwardBnUp()
{
	SetImgAboutIndex(--m_index);
	m_pBackward->SetPushDelay(0.f);
}

void FrRankingAboutDlg::OnForwardBnInit(int param)
{
	m_pForward = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRankingAboutDlg::OnForwardBnUp()
{
	SetImgAboutIndex(++m_index);
	m_pForward->SetPushDelay(0.f);
}

void FrRankingAboutDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRankingAboutDlg::OnOkBnUp()
{
	FrForm::OnFreshOkay();
}

bool FrRankingAboutDlg::OnInit()
{
	SetImgAboutIndex(0);
	return true;
}

void FrRankingAboutDlg::SetImgAboutIndex(int index)
{
	for (int i = 0; i < 4; ++i)
		m_pIndex[i]->Unselect(false);
	if (m_pBackward)
		m_pBackward->Enable(true);
	if (m_pForward)
		m_pForward->Enable(true);
	if (index <= 0)
	{
		index = 0;
		m_pIndex[0]->Select(false);
		if (m_pBackward)
			m_pBackward->Enable(false);
	}
	else if (index >= 3)
	{
		index = 3;
		m_pIndex[3]->Select(false);
		if (m_pForward)
			m_pForward->Enable(false);
	}
	m_pIndex[index]->Select(false);
	m_index = index;
	if (m_pViewer)
		m_pViewer->Open(MakeStr("help_ranking_%02d.jpg", index + 1));
}
