#include "cardboxdlg.h"
#include "exhibition.h"
#include "particle.h"
#include "titlefontex.h"
#include "bongdarishopdlg.h"
#include "wbone.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrCardBoxDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrCardBoxDlg, FrForm)
ON_FRESH_VI("ID_BTN_ABOUT", FRCMD_INIT, FrCardBoxDlg::OnResultBtnInit)
ON_FRESH_VV("ID_BTN_ABOUT", FRCMD_LBUTTONUP, FrCardBoxDlg::OnResultBtnUp)
ON_FRESH_VI("ID_BTN_CLOSE", FRCMD_INIT, FrCardBoxDlg::OnCloseBtnInit)
ON_FRESH_VV("ID_BTN_CLOSE", FRCMD_LBUTTONUP, FrCardBoxDlg::OnCloseBtnUp)
ON_FRESH_VI("ID_AREA_BG", FRCMD_INIT, FrCardBoxDlg::OnAreaBgInit)
ON_FRESH_VV("ID_AREA_BG", FRCMD_OWNERDRAW, FrCardBoxDlg::OnAreaBgOwnerDraw)
ON_FRESH_VI("ID_BTN_PACKOPEN", FRCMD_INIT, FrCardBoxDlg::OnPackOpenBtnInit)
ON_FRESH_VV("ID_BTN_PACKOPEN", FRCMD_LBUTTONUP, FrCardBoxDlg::OnPackOpenBtnUp)
ON_FRESH_VI("ID_NEXT_OPEN_OK", FRCMD_INIT, FrCardBoxDlg::OnOpenNextBtnInit)
ON_FRESH_VV("ID_NEXT_OPEN_OK", FRCMD_LBUTTONUP, FrCardBoxDlg::OnOpenNextBtnUp)
END_FRESH_MSGMAP()

FrCardBoxDlg::FrCardBoxDlg()
	: m_pFont(NULL),
	  m_pExhibition(NULL),
	  m_state(0),
	  m_pPackOpenBtn(NULL),
	  m_pOpenNextBtn(NULL),
	  m_pCloseBtn(NULL),
	  m_pResultBtn(NULL),
	  m_cardTypeId(0),
	  m_cardId(0),
	  m_packShake(0.0f)
{
}

FrCardBoxDlg::~FrCardBoxDlg()
{
	m_resultCards.clear();
	if (m_pParticle)
	{
		delete m_pParticle;
		m_pParticle = NULL;
	}
	if (m_pFont)
	{
		delete m_pFont;
		m_pFont = NULL;
	}
	if (m_pExhibition)
	{
		delete m_pExhibition;
		m_pExhibition = NULL;
	}
}

void FrCardBoxDlg::Init(unsigned long typeId, unsigned long id, int count)
{
	FrForm::EnableDrag(false);
	std::list<std::string> images;
	for (int i = 0; i < 5; ++i)
		images.push_back(std::string(MakeStr("particle_shine_%d", i)));
	m_pParticle = new CParticle2D;
	m_pParticle->Initialize(images, m_bgRect, 20, CParticle2D::STYLE_NORMAL,
		20.0f, 100.0f, WVector2D(0.0f, 1.0f), WVector2D(0.0f, 1.0f));
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(typeId);
	if (!card || count < 1)
		return;
	unsigned long kind = (typeId >> 22) & 15;
	m_cardTypeId = typeId;
	m_cardId = id;
	if (kind == 4)
		LoadCardBoxPet(typeId, count);
	else if (kind == 3)
		LoadCardPackPet(typeId);
}

void FrCardBoxDlg::RecievedPacket(WReceivedPacket* packet)
{
	if (!m_pExhibition)
		return;
	unsigned long result = packet->Decode4();
	if (result == 0)
	{
		SetAnimation(3);
		sCards card;
		packet->DecodeBuffer(&card, sizeof(card));
		CARD_LIST_IT it = std::find_if(Doc()->m_cardList.begin(),
			Doc()->m_cardList.end(), _FindByUid(card.uid));
		if (it != Doc()->m_cardList.end() && (*it).tid == card.tid)
		{
			--(*it).cardCount;
			if ((*it).cardCount < 1)
				Doc()->m_cardList.erase(it);
		}
		unsigned int count = packet->Decode1();
		for (int i = 0; i < (int)count; ++i)
		{
			packet->DecodeBuffer(&card, sizeof(card));
			unsigned long quantity = packet->Decode4();
			std::map<unsigned long, unsigned int>::iterator found =
				m_resultCards.find(card.tid);
			if (found == m_resultCards.end())
				m_resultCards[card.tid] = 1;
			else
				++found->second;
			CARD_LIST_IT it = std::find_if(Doc()->m_cardList.begin(),
				Doc()->m_cardList.end(), _FindByUid(card.uid));
			if (it != Doc()->m_cardList.end())
			{
				if ((*it).tid == card.tid)
					(*it).cardCount += quantity;
			}
			else
				Doc()->m_cardList.push_back(card);
			IFF_STRUCT::sCard* info = ItemManager()->FindCard(card.tid);
			if (m_pExhibition && info)
				m_pExhibition->ChangeBoneTexture(
					MakeStr("card_base%02d_Dummy", i + 1), "card_back-00.png",
					MakeStr("%s.png", info->Image));
		}
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x229, 0, 0, 0, 0, 0));
	}
	else if (result == 1)
	{
		FrForm::OnCancel();
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
			(int)"\xc4\xab\xb5\xe5\xc6\xd1 \xbf\xc0\xc7\xc2\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9",
			0, 0, 0, 0));
	}
}

bool FrCardBoxDlg::OnInit()
{
	if (!m_pFont)
		m_pFont = new CWindFont;
	return true;
}

void FrCardBoxDlg::OnProc(const float delta)
{
	if (!m_pExhibition)
		return;
	w_motion_data* motion = m_pExhibition->GetMotionData();
	if (!motion)
		return;
	switch (m_state)
	{
	case 0:
		if (!_stricmp(motion->name, "default\xb3\xa1") &&
			m_cardTypeId == 0x7d000000)
			LoadCardPackPet(0x7cc00000);
		break;
	case 1:
		if (!_stricmp(motion->name,
				"\xc6\xd1\xb4\xd9\xbf\xee\xc1\xbe\xb7\xe1") &&
			m_pPackOpenBtn && !m_pPackOpenBtn->IsVisible())
			m_pPackOpenBtn->SetVisible(true);
		break;
	case 2:
		if (!_stricmp(motion->name, "\xc6\xd1\xc1\xa4\xc1\xf6\xbb\xf3\xc5\xc2"))
			SetAnimation(2);
		break;
	case 3:
		if (!_stricmp(motion->name, "\xc6\xd1\xc1\xa4\xc1\xf6\xbb\xf3\xc5\xc2"))
			SetAnimation(4);
		break;
	case 4:
		if (!_stricmp(motion->name, "\xc6\xd1\xbf\xc0\xc7\xc2\xc1\xbe\xb7\xe1"))
			SetAnimation(5);
		break;
	case 5:
	{
		float distance = m_pExhibition->GetDistance();
		if (distance > 4.8f)
		{
			distance -= delta * ((10.2f - 4.8f) * 3.0f);
			if (distance < 4.8f)
				distance = 4.8f;
			m_pExhibition->SetDistance(distance, true, true);
		}
		else
		{
			m_pExhibition->SetDistance(4.8f, true, true);
			SetAnimation(6);
			CARD_LIST_IT it = std::find_if(Doc()->m_cardList.begin(),
				Doc()->m_cardList.end(), _FindByUid(m_cardId));
			if (it != Doc()->m_cardList.end() && m_pOpenNextBtn)
				m_pOpenNextBtn->SetVisible(true);
			m_pCloseBtn->Enable(true);
		}
	}
	break;
	case 7:
		if (!_stricmp(motion->name,
				"\xc4\xab\xb5\xe5\xb9\xf6\xb8\xb2\xc1\xbe\xb7\xe1"))
		{
			m_pExhibition->SetDistance(10.2f, true, true);
			CARD_LIST_IT it = std::find_if(Doc()->m_cardList.begin(),
				Doc()->m_cardList.end(), _FindByUid(m_cardId));
			if (it != Doc()->m_cardList.end())
			{
				if ((((*it).tid >> 22) & 15) == 4)
					LoadCardBoxPet((*it).tid, (*it).cardCount);
				else
					SetAnimation(1);
			}
			else
			{
				FrForm::OnCancel();
			}
		}
		break;
	}
	if (m_pExhibition)
		m_pExhibition->ProcessBongdariShop(delta);
	if (m_pParticle)
		m_pParticle->Process(delta);
	if (m_pPackOpenBtn && m_pPackOpenBtn->IsVisible())
	{
		static bool decreasing = false;
		if (decreasing)
		{
			if (SAFE_SUBVALUE(m_packShake, 0.0f, delta * 15.0f) <= 0.0f)
				decreasing = false;
		}
		else
		{
			if (SAFE_ADDVALUE(m_packShake, 15.0f, delta * 120.0f) >= 15.0f)
				decreasing = true;
		}
		WRect rect(m_packOpenRect.x + m_packShake, m_packOpenRect.y,
			m_packOpenRect.w, m_packOpenRect.h);
		m_pPackOpenBtn->FrWnd::SetRect(rect);
	}
}

void FrCardBoxDlg::OnOK()
{
}

void FrCardBoxDlg::OnCancel()
{
}

void FrCardBoxDlg::OnResultBtnInit(int param)
{
	m_pResultBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pResultBtn->SetPushSound("ui_icon_click");
}

void FrCardBoxDlg::OnResultBtnUp()
{
	std::list<sBonusBall> items;
	FrTotalItemDlg* dialog = CreateForm<FrTotalItemDlg>(g_pFresh->GetManager(),
		this, "total_item", NULL);
	if (dialog)
	{
		std::map<unsigned long, unsigned int>::iterator it =
			m_resultCards.begin();
		while (it != m_resultCards.end())
		{
			sBonusBall item;
			memset(&item, 0, sizeof(item));
			item.typeId = it->first;
			item.count = it->second;
			items.push_back(item);
			++it;
		}
		dialog->SetListItem(&items);
		dialog->Open(NULL, 3);
	}
}

void FrCardBoxDlg::OnCloseBtnInit(int param)
{
	m_pCloseBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrCardBoxDlg::OnCloseBtnUp()
{
	FrForm::OnCancel();
}

void FrCardBoxDlg::OnAreaBgInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_bgRect = area ? area->GetRect() : GetRect();
}

void FrCardBoxDlg::OnAreaBgOwnerDraw()
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (m_pExhibition)
	{
		if (m_state == 0)
			SetPackVisibleInBox();
		Display2DBg();
		m_pExhibition->DisplayClip(m_bgRect, 55.0f, 0.0f, 0.0f);
		if (m_state == 6)
		{
			CARD_LIST_IT it = std::find_if(Doc()->m_cardList.begin(),
				Doc()->m_cardList.end(), _FindByUid(m_cardId));
			if (it == Doc()->m_cardList.end())
			{
				const Bitmap* bitmap =
					g_pFresh->GetBitmap("text_cardpack_empty");
				if (bitmap)
				{
					const unsigned& height = bitmap->Height();
					const unsigned& width = bitmap->Width();
					{
						WRect rect(m_bgRect.x +
								(m_bgRect.w - bitmap->Width()) * 0.5f,
							m_bgRect.y + m_bgRect.h * 0.9f, (float)width,
							(float)height);
						graphic->DrawTexture(bitmap, rect, 0xffffffff, 0);
					}
				}
			}
			else
			{
				const Bitmap* next =
					g_pFresh->GetBitmap("text_cardpack_open_next");
				IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_cardTypeId);
				if (card)
				{
					const Bitmap* bitmap = g_pFresh->GetBitmap(card->c.Icon);
					if (bitmap && next)
					{
						WRect rect(m_bgRect);
						rect.x += 80.0f;
						rect.y += rect.h - bitmap->Height() - 4.0f;
						rect.w = (float)bitmap->Width();
						rect.h = (float)bitmap->Height();
						graphic->DrawTexture(bitmap, rect, 0xffffffff, 0);
						rect.x += 50.0f;
						rect.y += bitmap->Height() * 0.6f;
						CARD_LIST_IT it =
							std::find_if(Doc()->m_cardList.begin(),
								Doc()->m_cardList.end(), _FindByUid(m_cardId));
						float width;
						if (it != Doc()->m_cardList.end())
						{
							int count = (*it).cardCount;
							if (count > 99)
								count = 99;
							width = m_pFont->Print(rect.x, rect.y,
								MakeStr("x %02d", count));
						}
						else
						{
							width = m_pFont->Print(rect.x, rect.y,
								MakeStr("x %02d", -1));
						}
						rect.x += width + 10.0f;
						rect.y -= 4.0f;
						rect.w = (float)next->Width();
						rect.h = (float)next->Height();
						graphic->DrawTexture(next, rect, 0xffffffff, 0);
					}
				}
			}
		}
	}
}

void FrCardBoxDlg::OnPackOpenBtnInit(int param)
{
	m_pPackOpenBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pPackOpenBtn)
	{
		m_packOpenRect = m_pPackOpenBtn->GetRect();
		m_pPackOpenBtn->SetPushDelay(0.3f);
		m_pPackOpenBtn->SetVisible(false);
		m_pPackOpenBtn->SetPushSound("cardpack_open_click");
	}
}

void FrCardBoxDlg::OnPackOpenBtnUp()
{
	if (m_pPackOpenBtn->IsVisible() && m_pExhibition)
	{
		m_pCloseBtn->Enable(false);
		m_pPackOpenBtn->SetVisible(false);
		SetAnimation(2);
		WSendPacket packet((enumClientPacket)0xc2);
		packet.Encode4(m_cardTypeId);
		packet.Encode4(m_cardId);
		packet.Send(TO_GAME);
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}
}

void FrCardBoxDlg::OnOpenNextBtnInit(int param)
{
	m_pOpenNextBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pOpenNextBtn)
	{
		m_pOpenNextBtn->SetPushDelay(0.3f);
		m_pOpenNextBtn->SetVisible(false);
		m_pOpenNextBtn->SetPushSound("ui_icon_click");
	}
}

void FrCardBoxDlg::OnOpenNextBtnUp()
{
	if (m_pOpenNextBtn)
		m_pOpenNextBtn->SetVisible(false);
	SetAnimation(7);
}

void FrCardBoxDlg::SetAnimation(unsigned long state)
{
	if (m_pExhibition)
	{
		m_state = state;
		switch (state)
		{
		case 0:
			m_pExhibition->SetMotion("default", false, 0.0f);
			g_audio->PlaySfx("cardbox_open");
			break;
		case 1:
			m_pExhibition->SetMotion("\xc6\xd1\xb4\xd9\xbf\xee\xbd\xc3\xc0\xdb",
				false, 0.0f);
			break;
		case 2:
			m_pExhibition->SetMotion("\xc6\xd1\xb5\xb9\xb8\xae\xb1\xe2", false,
				0.0f);
			break;
		case 4:
			m_pExhibition->SetMotion("\xc6\xd1\xbf\xc0\xc7\xc2\xbd\xc3\xc0\xdb",
				false, 0.0f);
			if (((m_cardTypeId >> 22) & 15) == 3 &&
				m_cardTypeId != 0x7cc00000 && m_cardTypeId != 0x7cc00004)
				g_audio->PlaySfx("cardpack_open_one");
			else
				g_audio->PlaySfx("cardpack_open_multi");
			break;
		case 7:
			m_pExhibition->SetMotion(
				"\xc4\xab\xb5\xe5\xb9\xf6\xb8\xb2\xbd\xc3\xc0\xdb", false,
				0.0f);
			break;
		}
	}
}

void FrCardBoxDlg::LoadCardBoxPet(unsigned long typeId, int count)
{
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(typeId);
	if (card)
	{
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
		m_pExhibition = new CExhibition(m_bgRect.x + m_bgRect.w * 0.5f,
			m_bgRect.y + m_bgRect.h * 0.5f, 0.0f, true);
		m_pExhibition->SetDistance(10.8f, true, false);
		m_pExhibition->SetArea(m_bgRect.w, m_bgRect.h);
		if (m_pExhibition->SetModel(MakeStr("%s_open", card->Image),
				(eExhibitType)12))
		{
			if (count < 10)
			{
				WBone* root = m_pExhibition->GetModel()->GetRootBone();
				if (!root)
					return;
				WBone* main = root->FindBone("Main_Dummy01", 0);
				WBone* pack = root->FindBone(
					MakeStr("card_pack%02d_Dummy", 10 - count), 0);
				if (main && pack)
				{
					WVector offset = main->GetLocalMatrix().pivot -
						pack->GetLocalMatrix().pivot;
					offset.y = main->GetMatrix().pivot.y;
					main->Move(offset, false);
				}
			}
			SetAnimation(0);
		}
	}
}

void FrCardBoxDlg::LoadCardPackPet(unsigned long typeId)
{
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(typeId);
	if (card)
	{
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
		m_pExhibition = new CExhibition(m_bgRect.x + m_bgRect.w * 0.5f,
			m_bgRect.y + m_bgRect.h * 0.5f, -0.01f, true);
		m_pExhibition->SetDistance(10.2f, true, false);
		m_pExhibition->SetArea(m_bgRect.w, m_bgRect.h);
		if (m_pExhibition->SetModel(MakeStr("%s_open", card->Image),
				(eExhibitType)12))
			SetAnimation(1);
	}
}

void FrCardBoxDlg::SetPackVisibleInBox()
{
	CARD_LIST_IT it = std::find_if(Doc()->m_cardList.begin(),
		Doc()->m_cardList.end(), _FindByUid(m_cardId));
	if (it != Doc()->m_cardList.end() && (*it).cardCount < 10)
	{
		WBone* root = m_pExhibition->GetModel()->GetRootBone();
		for (int i = 0, count = 10 - (*it).cardCount; i < count; ++i)
		{
			WBone* pack =
				root->FindBone(MakeStr("card_pack%02d_Dummy", i + 1), 0);
			if (pack)
				pack->SetInvisible(false, true);
		}
	}
}

void FrCardBoxDlg::Display2DBg()
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	float x = m_bgRect.x;
	float y = m_bgRect.y;
	const Bitmap* bitmap = g_pFresh->GetBitmap("bg_cardcenter_0");
	if (bitmap)
		graphic->DrawTexture(bitmap,
			WRect(x, y - 28.0f, bitmap->Width(), bitmap->Height()), 0xffffffff,
			0);
	bitmap = g_pFresh->GetBitmap("bg_cardcenter_1");
	if (bitmap)
		graphic->DrawTexture(bitmap,
			WRect(x + 256.0f, y - 28.0f, bitmap->Width(), bitmap->Height()),
			0xffffffff, 0);
	bitmap = g_pFresh->GetBitmap("bg_cardcenter_2");
	if (bitmap)
		graphic->DrawTexture(bitmap,
			WRect(x, y + 256.0f - 28.0f, bitmap->Width(), bitmap->Height()),
			0xffffffff, 0);
	bitmap = g_pFresh->GetBitmap("bg_cardcenter_3");
	if (bitmap)
		graphic->DrawTexture(bitmap,
			WRect(x + 256.0f, y + 256.0f - 28.0f, bitmap->Width(),
				bitmap->Height()),
			0xffffffff, 0);
	if (m_pParticle)
		m_pParticle->Display();
}
