#include "ghostgamedlg.h"
#include "inputmanager.h"
#include "shareddoc.h"
#include "frwnd.h"
#include "frviewer.h"
#include "golfdoc.h"
#include "../../shared/sharedtables.h"
#include "../../shared/gamerewardtable.h"
#include "../../shared/ghostshared.h"
#include "levelupitemdlg.h"
#include "taskmanager.h"

extern Fresh* g_pFresh;
extern CInputManager* g_input;
extern WView* g_view;
extern WResourceManager* g_resrcmng;

IMPLEMENT_OBJECT(FrRecentMatch, FrForm)
BEGIN_FRESH_MSGMAP(FrRecentMatch, FrForm)
ON_FRESH_VI("bg_area", FRCMD_INIT, FrRecentMatch::OnInitBgArea)
ON_FRESH_VV("bg_area", FRCMD_OWNERDRAW, FrRecentMatch::OnDrawBgArea)
ON_FRESH_VI("match_list", FRCMD_INIT, FrRecentMatch::OnInitHistoryList)
ON_FRESH_VI("match_list", FRCMD_OWNERDRAW, FrRecentMatch::OnDrawHistoryList)
ON_FRESH_VI("text1", FRCMD_INIT, FrRecentMatch::OnInitStaticText1)
END_FRESH_MSGMAP()

FrRecentMatch::FrRecentMatch()
	: m_pCourseImg(NULL),
	  m_pCharacterImg(NULL),
	  m_score(0),
	  m_pang(0),
	  m_pMatchList(NULL),
	  m_pBgArea(NULL),
	  m_pText(NULL),
	  m_pFont(NULL)
{
	static char* filenames[] = { "[font_wind.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = filenames;
	font.fontw = 16;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ymP-.?/:";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&font);
}

FrRecentMatch::~FrRecentMatch()
{
	std::for_each(m_historyList.begin(), m_historyList.end(), Delete_Object());
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

void FrRecentMatch::Init(const std::string& nick, int course, int character,
	int score, int pang)
{
	m_ghostNick = nick;
	IFF_STRUCT::sCourse* courseInfo = ItemManager()->FindCourse(
		(course > 127 && course != 253 ? course - 128 : course) | 0x28000000);
	IFF_ITEM_COMMON* characterInfo =
		ItemManager()->FindCommonItem(character | 0x04000000);
	if (courseInfo && characterInfo)
	{
		m_pCourseImg =
			g_pFresh->GetBitmap(MakeStr("ghost_mid_%s", courseInfo->c.Icon));
		m_pCharacterImg =
			g_pFresh->GetBitmap(MakeStr("%s", characterInfo->Icon));
		m_pPangImg = g_pFresh->GetBitmap("ghost_pang");
		m_score = score;
		m_pang = pang;
	}
}

void FrRecentMatch::AddHistory(const sRecentMatchForm& form)
{
	sRecentMatchForm* entry = new sRecentMatchForm;
	*entry = form;
	m_historyList.push_back(entry);
	m_pMatchList->AddItem(entry);
}

void FrRecentMatch::AddFinish()
{
	m_pText->SetCaption("");
	int count = m_pMatchList->GetCurrentItemSize(true);
	for (int i = count; i < 5; ++i)
		m_pMatchList->AddItem(NULL);
}

void FrRecentMatch::Exception()
{
	m_pText->SetCaption(
		"\xbc\xad\xb9\xf6\xc0\xc7 \xbf\xe4\xc3\xbb\xc0\xcc \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3\xc8\xc4 \xb4\xd9\xbd\xc3 \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
}

void FrRecentMatch::OnInitBgArea(int param)
{
	m_pBgArea = DYNAMIC_CAST(FrArea, param);
}

void FrRecentMatch::OnDrawBgArea()
{
	WRect area = m_pBgArea->GetRect();
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device || !m_pCourseImg || !m_pCharacterImg || !m_pPangImg)
		return;
	float x = area.x + 20.f;
	float y = area.y + 7.f;
	device->DrawTexture(m_pCourseImg,
		WRect(x, y, (float)m_pCourseImg->Width(),
			(float)m_pCourseImg->Height()),
		0xffffffff, false);
	x += (float)m_pCourseImg->Width() + 20.f;
	device->SetTextStyle(4);
	device->SetTextColor(0xff000000, 0xffc8c8c8);
	g_pFresh->GetManager()->PrintText(WPoint(x + 60.f, y + 5.f), 0,
		m_ghostNick.c_str(), -1.f, 0xffffffff);
	device->SetTextStyle(0);
	device->SetTextColor(0xff000000, 0xffffffff);
	device->DrawTexture(m_pCharacterImg,
		WRect(x - 5.f, y + 25.f, (float)m_pCharacterImg->Width(),
			(float)m_pCharacterImg->Height()),
		0xffffffff, false);
	x += 115.f;
	m_pFont->Print(g_view, x, y + 35.f, MakeStr("-%d", m_score), 0, 0xffffffff,
		NULL);
	y += 55.f;
	float width = m_pFont->Print(g_view, x, y, MakeStr("%d", m_pang), 0,
		0xffffffff, NULL);
	device->DrawTexture(m_pPangImg,
		WRect(x + width + 5.f, y, (float)m_pPangImg->Width(),
			(float)m_pPangImg->Height()),
		0xffffffff, false);
}

void FrRecentMatch::OnInitHistoryList(int param)
{
	m_pMatchList = DYNAMIC_CAST(FrListBox, param);
}

void FrRecentMatch::OnDrawHistoryList(int param)
{
	FrListItem* item = (FrListItem*)param;
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!item || !device)
		return;
	device->SetTextStyle(0);
	device->SetTextColor(0xff000000, 0xffffffff);
	float x = item->pos.x;
	float y = item->pos.y + 8.f;
	sRecentMatchForm* entry = (sRecentMatchForm*)item->pData;
	if (entry)
	{
		const Bitmap* sex;
		if (entry->sex)
			sex = g_pFresh->RegisterBitmap("i_female");
		else
			sex = g_pFresh->RegisterBitmap("i_male");
		if (sex)
		{
			device->DrawTexture(sex,
				WRect(x + 3.f, item->pos.y, (float)sex->Width(),
					(float)sex->Height()),
				0xffffffff, false);
			x += (float)sex->Width() + 10.f;
		}
		g_pFresh->GetManager()->PrintText11(WPoint(x, y), 0,
			entry->nick.c_str(), -1.f, 0xffffffff);
		x += 130.f;
		if (entry->win)
			device->SetTextColor(0xff1c1c1c, 0xffffffff);
		else
			device->SetTextColor(0xff999999, 0xffffffff);
		x += g_pFresh->GetManager()->PrintText11(WPoint(x, y), 0,
				 entry->win ? "\xbd\xc2" : "\xc6\xd0", -1.f, 0xffffffff) +
			30.f;
		device->SetTextColor(0xff000000, 0xffffffff);
		const Bitmap* grade =
			g_pFresh->GetBitmap(MakeStr("ghost_grade_%d", entry->grade));
		if (grade)
			device->DrawTexture(grade,
				WRect(x - 10.f, y - 2.f, (float)grade->Width(),
					(float)grade->Height()),
				0xffffffff, false);
		x += 35.f;
		device->DrawTexture(m_pPangImg,
			WRect(x, y - 1.f, (float)m_pPangImg->Width(),
				(float)m_pPangImg->Height()),
			0xffffffff, false);
		x += (float)m_pPangImg->Width() + 5.f;
		device->Print11(WPoint(x, y), 0, "%d", entry->pang);
		g_pFresh->GetManager()->PrintText11(WPoint(x + 50.f, y), 0,
			MakeStr("%02d.%02d.%02d", entry->date.wYear, entry->date.wMonth,
				entry->date.wDay),
			-1.f, 0xffffffff);
	}
	else
	{
		device->SetTextStyle(4);
		device->SetTextColor(0xffc8c8c8, 0xff000000);
		x += 55.f;
		device->Print(WPoint(x, y), 1, "\xb1\xe2\xb7\xcf \xbe\xf8\xc0\xbd");
		x += 105.f;
		m_pFont->Print(g_view, x, y, "-", 0, 0xffffffff, NULL);
		x += 40.f;
		m_pFont->Print(g_view, x, y, "-", 0, 0xffffffff, NULL);
		x += 65.f;
		m_pFont->Print(g_view, x, y, "-", 0, 0xffffffff, NULL);
		m_pFont->Print(g_view, x + 70.f, y, "-", 0, 0xffffffff, NULL);
	}
	device->SetTextStyle(0);
	device->SetTextColor(0xff000000, 0xffffffff);
}

void FrRecentMatch::OnInitStaticText1(int param)
{
	m_pText = DYNAMIC_CAST(FrStatic, param);
	m_pText->SetCaption(
		"\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa4\xba\xb8\xb8\xa6 \xbf\xe4\xc3\xbb\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
}

IMPLEMENT_OBJECT(FrGhostGameMapDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrGhostGameMapDlg, FrForm)
ON_FRESH_VV("no", FRCMD_LBUTTONDOWN, FrGhostGameMapDlg::OnDownCancel)
ON_FRESH_VV("yes", FRCMD_LBUTTONDOWN, FrGhostGameMapDlg::OnDownOK)
ON_FRESH_VI("recent", FRCMD_INIT, FrGhostGameMapDlg::OnInitRecentMatch)
ON_FRESH_VV("recent", FRCMD_LBUTTONDOWN, FrGhostGameMapDlg::OnDownRecentMatch)
ON_FRESH_VI("bg_area", FRCMD_INIT, FrGhostGameMapDlg::OnInitAreaBg)
ON_FRESH_VI("bg_area", FRCMD_OWNERDRAW, FrGhostGameMapDlg::OnDrawAreaBg)
ON_FRESH_VI("map_select", FRCMD_INIT, FrGhostGameMapDlg::OnInitSelMapList)
ON_FRESH_VI("map_select", FRCMD_OWNERDRAW, FrGhostGameMapDlg::OnDrawSelMapList)
ON_FRESH_VV("map_select", FRCMD_LBUTTONDOWN,
	FrGhostGameMapDlg::OnDownSelMapList)
ON_FRESH_VI("choice_text", FRCMD_INIT, FrGhostGameMapDlg::OnInitAreaSelecText)
ON_FRESH_VV("choice_text", FRCMD_OWNERDRAW, FrGhostGameMapDlg::OnDrawSelecText)
ON_FRESH_VI("choice_map", FRCMD_INIT, FrGhostGameMapDlg::OnInitAreaMap)
ON_FRESH_VI("choice_map", FRCMD_OWNERDRAW, FrGhostGameMapDlg::OnDrawChoiceMap)
END_FRESH_MSGMAP()

void FrGhostGameMapDlg::sImgTag::Animate(float dt)
{
	if (bgAlpha < 255.f)
	{
		bgAlpha += dt * 204.f;
		if (bgAlpha > 255.f)
			bgAlpha = 255.f;
	}
	if (infoAlpha < 255.f && x < infoRect.x + infoRect.w)
	{
		infoAlpha += dt * 357.f;
		if (infoAlpha > 255.f)
			infoAlpha = 255.f;
	}
	if (infoRect.x < x)
	{
		x -= (800.f - infoRect.x) * dt * 1.5f;
		if (x < infoRect.x)
			x = infoRect.x;
	}
}

void FrGhostGameMapDlg::sImgTag::Reset()
{
	bgAlpha = infoAlpha = 0.f;
	x = 800.f;
}

void FrGhostGameMapDlg::sImgInfo::AnimateLock(float dt)
{
	if (bArrived && !bSelectable && lockAlpha < 200.f)
	{
		lockAlpha += dt * 400.f;
		if (lockAlpha > 200.f)
			lockAlpha = 200.f;
	}
}

FrGhostGameMapDlg::FrGhostGameMapDlg()
	: m_pMapList(NULL),
	  m_selCourse(0),
	  m_pChoiceText(NULL),
	  m_tipAlpha(0.f),
	  m_pRecentBtn(NULL)
{
	m_imgInfo.reserve(20);
	m_imgTag.reserve(20);
	for (int i = 0; i < 20; ++i)
	{
		sImgInfo info(0.f, 0.f);
		info.course = 20;
		m_imgInfo.push_back(info);
		sImgTag tag;
		memset(&tag, 0, sizeof(tag));
		m_imgTag.push_back(tag);
	}
}

FrGhostGameMapDlg::~FrGhostGameMapDlg()
{
}

void* FrGhostGameMapDlg::GetSelectItem()
{
	std::list<GHOST_GAME_BRIEF>::iterator it =
		std::find(m_briefList.begin(), m_briefList.end(), m_selCourse);
	if (it != m_briefList.end())
		return &*it;
	return NULL;
}

void FrGhostGameMapDlg::Init(unsigned long uid, const std::string& nick,
	const std::list<GHOST_GAME_BRIEF>& briefList)
{
	if (uid == MyUID())
	{
		m_pRecentBtn->SetVisible(true);
		m_pRecentBtn->Enable(true);
	}
	m_briefList.assign(briefList.begin(), briefList.end());
	m_uid = uid;
	m_ghostNick = nick;
	if (!m_pMapList)
		return;
	std::list<int> selectable;
	std::list<int> unavailable;
	for (int index = 19; index >= 0; --index)
	{
		if (index == 17 || index == 12)
			continue;
		IFF_STRUCT::sCourse* course = Doc()->m_itemManager.FindCourse(
			(index > 127 && index != 253 ? index - 128 : index) | 0x28000000);
		if (!course)
			continue;
		sImgInfo info(-100.f, 0.f);
		info.course = index;
		std::list<GHOST_GAME_BRIEF>::iterator it =
			std::find(m_briefList.begin(), m_briefList.end(), index);
		if (it != m_briefList.end())
		{
			if ((*it).recordCount)
			{
				selectable.push_back(index);
				info.bSelectable = true;
			}
			else
			{
				info.bLock = true;
				unavailable.push_back(index);
				info.bSelectable = false;
			}
		}
		else
		{
			unavailable.push_back(index);
			info.bSelectable = false;
		}
		m_imgInfo[index] = info;
		sImgTag tag;
		tag.pBg = g_pFresh->GetBitmap(MakeStr("30in_back_%s", course->Data));
		if (tag.pBg)
			tag.bgRect = WRect(m_choiceMapRect.x, m_choiceMapRect.y,
				(float)tag.pBg->Width(), (float)tag.pBg->Height());
		tag.pInfo = g_pFresh->GetBitmap("ghost_mapdlg_info");
		if (tag.pInfo)
			tag.infoRect =
				WRect(m_choiceMapRect.x + 1.f, m_choiceMapRect.y + 6.f,
					(float)tag.pInfo->Width(), (float)tag.pInfo->Height());
		tag.Reset();
		m_imgTag[index] = tag;
	}
	if (!selectable.empty())
		m_selCourse = *selectable.begin();
	else
		m_selCourse = 19;
	for (std::list<int>::iterator it = selectable.begin();
		it != selectable.end(); ++it)
	{
		int index = *it;
		IFF_STRUCT::sCourse* course = Doc()->m_itemManager.FindCourse(
			(index > 127 && index != 253 ? index - 128 : index) | 0x28000000);
		if (course)
			m_pMapList->AddItem(course);
	}
	for (std::list<int>::iterator it = unavailable.begin();
		it != unavailable.end(); ++it)
	{
		int index = *it;
		IFF_STRUCT::sCourse* course = Doc()->m_itemManager.FindCourse(
			(index > 127 && index != 253 ? index - 128 : index) | 0x28000000);
		if (course)
			m_pMapList->AddItem(course);
	}
}

void FrGhostGameMapDlg::OnProc(const float dt)
{
	for (std::vector<sImgInfo>::iterator it = m_imgInfo.begin();
		it != m_imgInfo.end(); ++it)
	{
		if ((*it).course == 20 || !(*it).bDrawn)
			continue;
		if ((*it).destX > (*it).x)
		{
			(*it).x += ((*it).destX + 100.f) * dt * 1.3f;
			if ((*it).x > (*it).destX)
				(*it).x = (*it).destX;
		}
		if ((*it).alpha < 255.f && (*it).x > 0.f)
		{
			float alpha = (*it).alpha + dt * 331.5f;
			(*it).alpha = alpha;
			if (alpha > 255.f)
				(*it).alpha = 255.f;
		}
		if ((*it).destX == (*it).x)
			(*it).bArrived = true;
		(*it).AnimateLock(dt);
	}
	sImgTag& tag = m_imgTag[m_selCourse];
	tag.Animate(dt);
	if (m_tipAlpha < 255.f)
	{
		m_tipAlpha += dt * 127.5f;
		if (m_tipAlpha >= 255.f)
			m_tipAlpha = 255.f;
	}
	FrForm::OnProc(dt);
}

void FrGhostGameMapDlg::OnDownOK()
{
}

void FrGhostGameMapDlg::OnDownCancel()
{
	if (IsLocalContent(S4_MATCHING_SYSTEM))
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(1);
		packet.Send(TO_GAME);
	}
	FrForm::Close(true);
}

void FrGhostGameMapDlg::OnDownRecentMatch()
{
	FrRecentMatch* recent = CreateForm<FrRecentMatch>(g_pFresh->GetManager(),
		NULL, "GHOST_HISTORY", NULL);
	std::list<GHOST_GAME_BRIEF>::iterator it =
		std::find(m_briefList.begin(), m_briefList.end(), m_selCourse);
	if (it != m_briefList.end())
	{
		recent->Init(m_ghostNick, m_selCourse, (*it).itemId, (*it).score,
			(*it).pang);
		recent->SetTopmost(true);
		recent->FindNextTopFocus(false);
		recent->Open(NULL, FrESCAPE);
		WSendPacket packet((enumClientPacket)0xba);
		packet.Encode1(9);
		packet.Encode1((unsigned char)m_selCourse);
		packet.Encode4((*it).recordId);
		packet.Encode4(m_uid);
		packet.Send(TO_GAME);
	}
}

void FrGhostGameMapDlg::OnInitRecentMatch(int param)
{
	m_pRecentBtn = DYNAMIC_CAST(FrButton, param);
	m_pRecentBtn->SetVisible(false);
}

void FrGhostGameMapDlg::OnInitSelMapList(int param)
{
	m_pMapList = DYNAMIC_CAST(FrListBox, param);
}

void FrGhostGameMapDlg::OnDownSelMapList()
{
	FrListItem* item = m_pMapList->GetSelected();
	if (!item || !item->pData)
		return;
	IFF_STRUCT::sCourse* course = (IFF_STRUCT::sCourse*)item->pData;
	unsigned int index = course->c.TypeId & 0x03ffffff;
	sImgInfo& info = m_imgInfo[index];
	if (info.bSelectable)
	{
		m_selCourse = index;
		m_imgTag[index].Reset();
	}
}

void FrGhostGameMapDlg::OnDrawSelMapList(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	FrListItem* item = (FrListItem*)param;
	if (!item || !device || !item->pData)
		return;
	IFF_STRUCT::sCourse* course = (IFF_STRUCT::sCourse*)item->pData;
	const Bitmap* bitmap =
		g_pFresh->GetBitmap(MakeStr("ghost_mid_%s", course->c.Icon));
	if (!bitmap)
		return;
	unsigned int index = course->c.TypeId & 0x03ffffff;
	sImgInfo& info = m_imgInfo[index];
	info.bDrawn = true;
	info.destX = item->pos.x;
	device->DrawTexture(bitmap,
		WRect(info.x, item->pos.y + 6.f, (float)bitmap->Width(),
			(float)bitmap->Height()),
		((unsigned char)info.alpha << 24) | 0xffffff, false);
	if (m_selCourse == index)
	{
		const Bitmap* selected = g_pFresh->GetBitmap("ghost_mid_select_o");
		if (selected)
		{
			device->DrawTexture(selected,
				WRect(item->pos.x, item->pos.y + 6.f, (float)selected->Width(),
					(float)selected->Height()),
				((unsigned char)info.alpha << 24) | 0xffffff, false);
		}
	}
	if (!info.bSelectable)
	{
		const Bitmap* lock;
		if (info.bLock)
			lock = g_pFresh->GetBitmap("ghost_mid_map_lock");
		else
			lock = g_pFresh->GetBitmap("ghost_mid_map_nodata");
		if (lock)
		{
			device->DrawTexture(lock,
				WRect(info.x, item->pos.y + 6.f, (float)lock->Width(),
					(float)lock->Height()),
				((unsigned char)info.lockAlpha << 24) | 0xffffff, false);
		}
	}
}

void FrGhostGameMapDlg::OnInitAreaSelecText(int param)
{
	m_pChoiceText = DYNAMIC_CAST(FrArea, param);
}

void FrGhostGameMapDlg::OnDrawSelecText()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	const Bitmap* bitmap = g_pFresh->GetBitmap("ghost_map_select_tip");
	if (!bitmap)
		return;
	WRect rect = m_pChoiceText->GetRect();
	rect.w = (float)bitmap->Width();
	rect.h = (float)bitmap->Height();
	device->DrawTexture(bitmap, rect,
		((unsigned char)m_tipAlpha << 24) | 0xffffff, false);
}

void FrGhostGameMapDlg::OnInitAreaMap(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, param);
	if (area)
		m_choiceMapRect = area->GetRect();
}

void FrGhostGameMapDlg::OnDrawChoiceMap(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (device)
	{
		sImgTag& tag = m_imgTag[m_selCourse];
		if (tag.pBg)
			device->DrawTexture(tag.pBg, tag.bgRect,
				((unsigned char)tag.bgAlpha << 24) | 0xffffff, false);
		if (tag.pInfo)
		{
			WRect pos = tag.infoRect;
			pos.x = tag.x;
			float right = pos.w + tag.bgRect.x + 1.f;
			if (pos.x < right)
			{
				pos.w = right - pos.x;
				WRect dest(pos);
				WRect src(0.f, 0.f, pos.w, pos.h);
				device->DrawTexture(tag.pInfo, src, dest,
					((unsigned char)tag.infoAlpha << 24) | 0xffffff, false);
				std::list<GHOST_GAME_BRIEF>::iterator it = std::find(
					m_briefList.begin(), m_briefList.end(), m_selCourse);
				if (it != m_briefList.end())
				{
					IFF_ITEM_COMMON* character = ItemManager()->FindCommonItem(
						(*it).itemId | 0x04000000);
					const Bitmap* bitmap =
						g_pFresh->GetBitmap(MakeStr("%s", character->Icon));
					if (bitmap)
						device->DrawTexture(bitmap,
							WRect(pos.x + 7.f, pos.y + 6.f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							((unsigned char)tag.infoAlpha << 24) | 0xffffff,
							false);
					if (tag.infoAlpha >= 184.f)
					{
						device->SetTextStyle(2);
						device->SetTextColor(0xffffffff, 0xff000000);
						g_pFresh->GetManager()->PrintText(
							WPoint(tag.bgRect.x + 108.f, tag.bgRect.y + 19.f),
							0, m_ghostNick.c_str(), -1.f, 0xffffffff);
						int score = -(*it).score;
						device->Print(
							WPoint(tag.bgRect.x + 108.f, tag.bgRect.y + 37.f),
							0, "%d", score);
						const int& pangValue = (*it).pang;
						float width = device->Print(
							WPoint(tag.bgRect.x + 108.f, tag.bgRect.y + 52.f),
							0, "%d", pangValue);
						const Bitmap* pang = g_pFresh->GetBitmap("ghost_pang");
						if (pang)
						{
							device->DrawTexture(pang,
								WRect(width + tag.bgRect.x + 110.f,
									tag.bgRect.y + 52.f, (float)pang->Width(),
									(float)pang->Height()),
								0xffffffff, false);
						}
						device->SetTextStyle(0);
						device->SetTextColor(0xff000000, 0xffffffff);
					}
				}
			}
		}
	}
}

void FrGhostGameMapDlg::OnDrawAreaBg(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	WRect rect = GetRect();
	device->SetTextStyle(2);
	device->SetTextColor(0xffffffff, 0xff000000);
	device->PrintText(WPoint(rect.x + 45.f, rect.y + rect.h - 40.f), 0,
		"\xb0\xed\xbd\xba\xc6\xae \xb5\xa5\xc0\xcc\xc5\xcd\xb4\xc2 \xc0\xd3\xc0\xc7\xb7\xce \xbb\xe8\xc1\xa6 \xb5\xc9 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
		NULL);
	device->SetTextStyle(0);
	device->SetTextColor(0xff000000, 0xff000000);
}

void FrGhostGameMapDlg::OnInitAreaBg(int param)
{
	FrViewer* viewer = DYNAMIC_CAST(FrViewer, param);
	if (viewer)
	{
		viewer->Open("ghost_mapdlg_bg.jpg");
		viewer->SetAlpha(.8f);
	}
}

IMPLEMENT_OBJECT(FrGhostTimeBar, FrFormBar)
BEGIN_FRESH_MSGMAP(FrGhostTimeBar, FrFormBar)
ON_FRESH_VI("gauge_area", FRCMD_INIT, FrGhostTimeBar::OnInitGaugeArea)
ON_FRESH_VV("gauge_area", FRCMD_OWNERDRAW, FrGhostTimeBar::OnDrawGaugeArea)
ON_FRESH_VI("mouse_area", FRCMD_INIT, FrGhostTimeBar::OnInitMouseArea)
ON_FRESH_VV("mouse_area", FRCMD_OWNERDRAW, FrGhostTimeBar::OnDrawMouseArea)
END_FRESH_MSGMAP()

FrGhostTimeBar::FrGhostTimeBar()
	: m_gaugePos(0.f), m_bFinished(false), m_pMouseArea(NULL), m_pMouseImg(NULL)
{
	m_bonusTime = 4.f;
	m_closeDelay = 1.f;
}

FrGhostTimeBar::~FrGhostTimeBar()
{
}

void FrGhostTimeBar::OnInitMouseArea(int param)
{
	m_pMouseArea = DYNAMIC_CAST(FrArea, param);
}

void FrGhostTimeBar::OnDrawMouseArea()
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice && m_pMouseImg)
	{
		WPoint pos = GetRect().TopLeft();
		pDevice->DrawTexture(m_pMouseImg,
			WRect(pos.x + 256.f, pos.y + 18.f, m_pMouseImg->Width(),
				m_pMouseImg->Height()),
			0xffffffff, false);
	}
}

void FrGhostTimeBar::OnProc(const float dt)
{
	FrFormBar::OnProc(dt);
	if (m_bFinished)
	{
		m_closeDelay -= dt;
		if (m_closeDelay < 0.f)
			OnCancel();
	}
	else
	{
		m_bonusTime -= dt;
		if (m_bonusTime < 0.f)
		{
			m_bonusTime = 0.f;
			m_bFinished = true;
			m_pMouseImg = g_pFresh->GetBitmap("ghost_bonus_nclick");
			SetPos(0.f);
			return;
		}
		m_gaugePos = m_bonusTime * 25.f;
		SetPos(m_gaugePos);
		unsigned int flags = 0;
		if (g_input->GetButton(LEFT_BUTTON) == 3)
			flags = 4;
		if (g_input->GetButton(LEFT_BUTTON) == 1)
			flags |= 1;
		if (g_input->GetButton(RIGHT_BUTTON) == 3)
			flags |= 8;
		if (g_input->GetButton(RIGHT_BUTTON) == 1)
			flags |= 2;
		if (flags)
		{
			if (!(flags & 3))
			{
				SetGaugeImg("formbar_gauge_n");
			}
			else
			{
				g_audio->PlaySfx(MakeStr("pang_coin_%d", rand() % 3 + 1));
				SetGaugeImg("formbar_gauge_o");
			}
			if (flags & 4)
			{
				if (flags & 8)
					m_pMouseImg = g_pFresh->GetBitmap("ghost_bonus_aclick");
				else
					m_pMouseImg = g_pFresh->GetBitmap("ghost_bonus_lclick");
			}
			else
			{
				m_pMouseImg = g_pFresh->GetBitmap("ghost_bonus_rclick");
			}
		}
		else
		{
			SetGaugeImg("formbar_gauge_n");
			m_pMouseImg = g_pFresh->GetBitmap("ghost_bonus_nclick");
		}
	}
}

bool FrGhostTimeBar::Open(FRESH_PFN_RESULT pFnResult, unsigned long flag)
{
	SetFixedWnd();
	bool result = FrFormBar::Open(pFnResult, flag);
	WRect rect = GetGaugeRect();
	MoveBar(WPoint(rect.x - 47.f, rect.y - 26.f));
	return result;
}

IMPLEMENT_OBJECT(FrGhostAward, FrForm)
BEGIN_FRESH_MSGMAP(FrGhostAward, FrForm)
ON_FRESH_VI("text_desc", FRCMD_INIT, FrGhostAward::OnInitDescText)
ON_FRESH_VI("gauge_exp", FRCMD_INIT, FrGhostAward::OnInitGaugeBar)
ON_FRESH_VI("gauge_text", FRCMD_INIT, FrGhostAward::OnInitGaugeText)
ON_FRESH_VI("bg_pouch", FRCMD_INIT, FrGhostAward::OnInitPouchArea)
ON_FRESH_VI("bg_score", FRCMD_OWNERDRAW, FrGhostAward::OnDrawScore)
ON_FRESH_VI("bg_pouch", FRCMD_OWNERDRAW, FrGhostAward::OnDrawPouch)
END_FRESH_MSGMAP()

FrGhostAward::cPangFx::cPangFx()
{
}

FrGhostAward::cPangFx::~cPangFx()
{
}

bool FrGhostAward::cPangFx::IsAlive()
{
	return m_rect.y <= m_endY;
}

void FrGhostAward::cPangFx::Create(const WRect& pouch)
{
	m_pBitmap = g_pFresh->GetBitmap(MakeStr("ghost_pang_0%d", rand() % 5 + 1));
	float margin = m_pBitmap->Width() * 2.5f;
	float offset = (float)(rand() % (int)(pouch.w - margin));
	m_rect = WRect(m_pBitmap->Width() * .8f + pouch.x + offset,
		pouch.y + (rand() % 5 - 5), m_pBitmap->Width(), m_pBitmap->Height());
	m_speed = (rand() % 50 + 200) * .01f;
	m_endY = pouch.y + 30.f;
	m_alpha = 125.f;
}

void FrGhostAward::cPangFx::Proc(float dt)
{
	if (m_alpha < 255.f)
	{
		m_alpha += dt * 1020.f;
		if (m_alpha >= 255.f)
			m_alpha = 255.f;
	}
	if (m_rect.y < m_endY)
		m_rect.y += dt * m_speed * 30.f;
}

void FrGhostAward::cPangFx::Render(FrGraphicInterface* pDevice)
{
	if (m_pBitmap)
		pDevice->DrawTexture(m_pBitmap, m_rect,
			((unsigned char)m_alpha << 24) | 0xffffff, false);
}

FrGhostAward::FrGhostAward()
	: m_pTimeBar(NULL),
	  m_pPouchArea(NULL),
	  m_pExpGauge(NULL),
	  m_pDescText(NULL),
	  m_pGaugeText(NULL),
	  m_elapsedTime(0.f),
	  m_clickCount(0),
	  m_autoBonus(0),
	  m_clickBonus(10),
	  m_basePang(0),
	  m_displayPang(0.f),
	  m_exp(0),
	  m_remainExp(0)
{
	static char* filenames[] = { "[font_wind.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = filenames;
	font.fontw = 16;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:,";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&font);
}

FrGhostAward::~FrGhostAward()
{
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
	std::for_each(m_pangFxList.begin(), m_pangFxList.end(), Delete_Object());
}

void FrGhostAward::Init(bool bUserFirst)
{
	SetFixed(true);
	EnableDrag(false);
	SetCaption("\xb0\xed\xbd\xba\xc6\xae \xb0\xd4\xc0\xd3\xb0\xe1\xb0\xfa");
	int ghostPang = PLAYER(1)->totalPang;
	int userPang = PLAYER(0)->totalPang;
	CGhostGameBonusUtil bonus;
	bool userWin = bonus.IsUserWin(PLAYER(0)->score, userPang, PLAYER(1)->score,
		ghostPang);
	int difficulty = 1;
	IFF_STRUCT::sCourse* course =
		ItemManager()->FindCourse(GetCurMap() | 0x28000000);
	if (course)
		difficulty = course->Difficulty;
	int clickBonus =
		bonus.GetOneClickBonus(GetHoleIndex(GOLFDOC()->m_currentHole), userWin,
			PLAYER(0)->score, difficulty, Doc()->m_userInfo[1].stat.Level,
			Doc()->m_userInfo[0].stat.Level);
	m_clickBonus = min(clickBonus, 200);
	m_basePang = min(ghostPang, userPang);
	unsigned char level = Doc()->m_myInfo.stat.Level;
	std::string desc = "\xbe\xcb \xbc\xf6 \xbe\xf8\xb4\xc2 \xb7\xb9\xba\xa7";
	if (level < 21)
	{
		desc = s_levelUpText[level][2];
	}
	else if (level < 65)
	{
		desc = g_LevelTable[level + 1].name;
		desc += s_levelUpText[21][2];
	}
	m_pDescText->SetCaption(desc.c_str());
	unsigned char holes = GetHoleIndex(GOLFDOC()->m_currentHole) - 1;
	if (holes < 1)
		holes = 1;
	else if (holes > 18)
		holes = 18;
	unsigned char courseDifficulty;
	if (!Doc()->m_golfGame.pCourse)
		courseDifficulty = 1;
	else
		courseDifficulty = Doc()->m_golfGame.pCourse->Difficulty;
	unsigned char difficultyIndex = courseDifficulty - 1;
	if (bUserFirst)
		m_exp = g_ExperienceTable[difficultyIndex][holes][2];
	else
		m_exp = g_ExperienceTable[difficultyIndex][holes][3];
	m_remainExp = m_exp;
	int maxExp = INT_MAX;
	if (level < 71)
		maxExp = g_LevelTable[level].exp;
	m_pExpGauge->SetRange(0, maxExp, 0);
	m_pExpGauge->SetPos(Doc()->m_myInfo.stat.dwExp);
	m_pGaugeText->SetCaption(
		MakeStr("%d / %d", Doc()->m_myInfo.stat.dwExp, maxExp));
	WRect pos = GetRect();
	pos.y = (float)((double)g_view->GetHeight() - pos.h);
	MoveWindow(pos.TopLeft());
	m_result1.pos.x = m_result2.pos.x = pos.x + 180.f;
	m_result1.pos.y = pos.y + 58.f;
	m_result2.pos.y = pos.y + 86.f;
	if (bUserFirst)
	{
		SetResultInfo(m_result1, 0);
		SetResultInfo(m_result2, 1);
	}
	else
	{
		SetResultInfo(m_result1, 1);
		SetResultInfo(m_result2, 0);
		WRect gauge = m_pExpGauge->GetRect();
		gauge.y += 29.f;
		m_pExpGauge->SetRect(gauge);
		WRect text = m_pGaugeText->GetRect();
		text.y += 29.f;
		m_pGaugeText->SetRect(text);
	}
	m_pTimeBar = CreateForm<FrGhostTimeBar>(g_pFresh->GetManager(), NULL,
		"FORM_BAR_GHOST", NULL);
	if (m_pTimeBar)
		m_pTimeBar->Open(NULL, FrMODALESS);
}

void FrGhostAward::SetResultInfo(sResultInfo& info, int index)
{
	info.bGhost = index == 1;
	info.exp = Doc()->m_userInfo[index].stat.dwExp;
	info.nick = Doc()->m_userInfo[index].info.sNick;
	int level = Doc()->m_userInfo[index].stat.Level + 1;
	if (level >= 71)
		level = 71;
	info.levelImg = MakeStr("level_%03d", level);
	info.score = PLAYER((unsigned char)index)->score;
}

bool FrGhostAward::IsFinishCalcBonus()
{
	return m_elapsedTime > 4.f;
}

void FrGhostAward::OnProc(const float dt)
{
	static float elapsed = 0.f;
	static float interval = .3f;
	elapsed += dt;
	m_elapsedTime += dt;
	if (m_displayPang < m_basePang)
	{
		m_displayPang += dt * m_basePang * .2f;
		if (m_displayPang > m_basePang)
			m_displayPang = (float)m_basePang;
	}
	if (elapsed > interval)
	{
		elapsed = 0.f;
		interval = (rand() % 2 + 2) * .1f;
		if (m_elapsedTime < 4.f)
		{
			cPangFx* effect = new cPangFx;
			effect->Create(m_pPouchArea->GetRect());
			m_pangFxList.push_back(effect);
			m_autoBonus += 10;
			g_audio->PlaySfx(MakeStr("pang_coin_%d", rand() % 3 + 1));
		}
	}
	if (m_elapsedTime < 4.f &&
		(g_input->GetButton(LEFT_BUTTON) == 1 ||
			g_input->GetButton(RIGHT_BUTTON) == 1))
	{
		++m_clickCount;
		cPangFx* effect = new cPangFx;
		effect->Create(m_pPouchArea->GetRect());
		m_pangFxList.push_back(effect);
	}
	for (std::list<cPangFx*>::iterator it = m_pangFxList.begin();
		it != m_pangFxList.end();)
	{
		(*it)->Proc(dt);
		if (!(*it)->IsAlive())
		{
			delete *it;
			m_pangFxList.erase(it++);
		}
		else
			++it;
	}
	AwardProc(dt);
}

void FrGhostAward::AwardProc(float dt)
{
	if (!(m_elapsedTime < 1.f) && m_exp > 0)
	{
		unsigned char& levelRef = Doc()->m_userInfo[0].stat.Level;
		unsigned char level = levelRef;
		if (level < 65)
		{
			unsigned long& exp = Doc()->m_userInfo[0].stat.dwExp;
			if (exp < g_LevelTable[level].exp)
			{
				if (m_remainExp > 0)
				{
					++exp;
					++Doc()->m_myInfo.stat.dwExp;
					--m_remainExp;
				}
			}
			else
			{
				exp = 0;
				levelRef = ++level;
				Doc()->m_myInfo.stat.dwExp = 0;
				++Doc()->m_myInfo.stat.Level;
				m_pExpGauge->SetRange(0, g_LevelTable[levelRef].exp, 0);
				if (m_result1.bGhost)
					m_result2.levelImg =
						MakeStr("level_%03d", Doc()->m_myInfo.stat.Level);
				else
					m_result1.levelImg =
						MakeStr("level_%03d", Doc()->m_myInfo.stat.Level);
				AfxGetTask()->GetActor("Player")
					<< MsgObject(NULL, 504, 0, 0, 0, 0, 0);
			}
			m_pExpGauge->SetPos(exp);
			m_pGaugeText->SetCaption(
				MakeStr("%d / %d", exp, g_LevelTable[levelRef].exp));
		}
		else
		{
			m_exp = 0;
		}
	}
}

bool FrGhostAward::IsLevelUpState()
{
	sPangYaUserStatistics& stat = Doc()->m_userInfo[0].stat;
	if (stat.Level < 65 && m_exp + stat.dwExp >= g_LevelTable[stat.Level].exp)
		return true;
	return false;
}

void FrGhostAward::OnInitDescText(int param)
{
	m_pDescText = DYNAMIC_CAST(FrStatic, param);
}

void FrGhostAward::OnInitGaugeText(int param)
{
	m_pGaugeText = DYNAMIC_CAST(FrStatic, param);
}

void FrGhostAward::OnInitGaugeBar(int param)
{
	m_pExpGauge = DYNAMIC_CAST(FrGaugeBar, param);
}

void FrGhostAward::OnInitPouchArea(int param)
{
	m_pPouchArea = DYNAMIC_CAST(FrArea, param);
}

void FrGhostAward::OnDrawScore(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	WRect pos = GetRect();
	pos.x += 2.f;
	pos.y += 49.f;
	device->SetTextStyle(1);
	WRect pouch = m_pPouchArea->GetRect();
	if (!m_result1.bGhost)
	{
		WOverlay::DrawBox(g_view, WRect(pos.x, pos.y, 498.f, 30.f), 0,
			0x999b9dff, 0);
		m_pFont->Print(g_view, pouch.x + 170.f, pouch.y - 25.f, "-", 0,
			0xffffffff, NULL);
	}
	else
	{
		WOverlay::DrawBox(g_view, WRect(pos.x, pos.y + 27.f, 498.f, 30.f), 0,
			0x999b9dff, 0);
		m_pFont->Print(g_view, pouch.x + 170.f, pouch.y - 55.f, "-", 0,
			0xffffffff, NULL);
	}
	const Bitmap* first = g_pFresh->GetBitmap(m_result1.levelImg.c_str());
	WRect firstRect = WRect(m_result1.pos.x - first->Width() - 90.f,
		m_result1.pos.y - 2.f, (float)first->Width(), (float)first->Height());
	device->DrawTexture(first, firstRect, 0xffffffff, false);
	g_pFresh->GetManager()->PrintText(m_result1.pos, 1, m_result1.nick.c_str(),
		-1.f, 0xffffffff);
	g_pFresh->GetManager()->PrintText(m_result1.pos + WPoint(100.f, 0.f), 1,
		MakeStr("%d", m_result1.score), -1.f, 0xffffffff);
	const Bitmap* second = g_pFresh->GetBitmap(m_result2.levelImg.c_str());
	WRect secondRect = WRect(m_result2.pos.x - second->Width() - 90.f,
		m_result2.pos.y - 2.f, (float)second->Width(), (float)second->Height());
	device->DrawTexture(second, secondRect, 0xffffffff, false);
	g_pFresh->GetManager()->PrintText(m_result2.pos, 1, m_result2.nick.c_str(),
		-1.f, 0xffffffff);
	g_pFresh->GetManager()->PrintText(m_result2.pos + WPoint(100.f, 0.f), 1,
		MakeStr("%d", m_result2.score), -1.f, 0xffffffff);
	device->SetTextStyle(0);
}

void FrGhostAward::OnDrawPouch(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	const Bitmap* back = g_pFresh->GetBitmap("pang_bag2");
	const Bitmap* front = g_pFresh->GetBitmap("pang_bag1");
	const WRect& rect = m_pPouchArea->GetRect();
	if (back)
		device->DrawTexture(back, rect, 0xffffffff, false);
	for (std::list<cPangFx*>::iterator it = m_pangFxList.begin();
		it != m_pangFxList.end(); ++it)
		(*it)->Render(device);
	if (front)
		device->DrawTexture(front, rect, 0xffffffff, false);
	std::string base = MakeStr("%05d", (int)m_displayPang);
	int autoBonus = m_autoBonus < 200 ? m_autoBonus : 200;
	int clickBonus = m_clickBonus < 200 ? m_clickBonus : 200;
	unsigned int clicks = m_clickCount < 40 ? m_clickCount : 40;
	std::string total =
		MakeStr("%07d", (int)m_displayPang + clicks * clickBonus + autoBonus);
	m_pFont->Print(g_view, rect.x - 130.f, rect.y + 24.f, base.c_str(), 0,
		0xffffffff, NULL);
	m_pFont->Print(g_view, rect.x + 150.f, rect.y + 24.f, total.c_str(), 0,
		0xffffffff, NULL);
}
