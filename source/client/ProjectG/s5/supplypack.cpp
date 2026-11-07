#include "stdafx.h"
#include "s5/supplypack.h"
#include "exhibition.h"
#include "titlefontex.h"
#include "../../shared/sharedtables.h"
#include "minatl.h"

extern Fresh* g_pFresh;

class CFontAnimation
{
public:
	struct FONTFXPIECE
	{
		std::string text;
		float life;
		float alpha;
		WVector2D position;
		WVector2D direction;
		float speed;
	};

	CFontAnimation(WTitleFontEx* font, const WVector2D* point = NULL)
		: m_font(font)
	{
		if (point)
			m_start = *point;
		else
			m_start = WVector2D(0, 0);
	}
	void SetStartPoint(const WVector2D& point) { m_start = point; }

	void AddFontFx(const char* text, const WVector2D& direction, float speed,
		float life)
	{
		FONTFXPIECE piece;
		piece.life = life;
		piece.alpha = 255;
		piece.direction = direction;
		piece.position = m_start;
		piece.speed = speed;
		piece.text = text;
		m_pieces.push_back(piece);
	}

	void Process(float dt)
	{
		for (std::list<FONTFXPIECE>::iterator it = m_pieces.begin();
			it != m_pieces.end();)
		{
			FONTFXPIECE& piece = *it;
			if (piece.life > 0)
			{
				float rate = 1.0f / piece.life;
				piece.alpha -= rate * dt * 255.0f;
				piece.position += piece.direction * piece.speed * dt;
			}
			else
				piece.alpha = 0;
			if (piece.alpha <= 0)
				it = m_pieces.erase(it);
			else
				++it;
		}
	}

	void Display()
	{
		for (std::list<FONTFXPIECE>::iterator it = m_pieces.begin();
			it != m_pieces.end(); ++it)
		{
			FONTFXPIECE& piece = *it;
			m_font->SetColor(((unsigned char)piece.alpha << 24) | 0xffffff);
			m_font->Print(piece.position.x, piece.position.y,
				piece.text.c_str(), TFA_CENTER, 1, 1);
		}
	}

private:
	WTitleFontEx* m_font;
	std::list<FONTFXPIECE> m_pieces;
	WVector2D m_start;
};

namespace
{
	const float WAIT_TIME = 5.0f;
	int GetSupplyGroup(unsigned long tid)
	{
		switch (tid)
		{
		case 436207877:
		case 436207947:
			return 5;
		default:
			return -1;
		}
	}

	bool IsSuppliableItem(unsigned long tid)
	{
		if (IsLicenseLimitedItem(tid))
			return false;
		if (!ItemManager()->IsCanOverlapped(tid, 0, 0))
			return false;
		if ((tid & 0xfc000000) == 0x14000000)
		{
			if (ItemManager()->IsBasicAztecFamily(tid))
				return false;
			if (!ItemManager()->FindBall(tid))
				return false;
		}
		return true;
	}
}

namespace S5
{
	IMPLEMENT_OBJECT(FrSupplyPackDlg, FrForm)
	BEGIN_FRESH_MSGMAP(FrSupplyPackDlg, FrForm)
	ON_FRESH_VI("lt_item", FRCMD_INIT, FrSupplyPackDlg::OnItemListInit)
	ON_FRESH_VI("bt_cancel", FRCMD_INIT, FrSupplyPackDlg::OnCancelBtnInit)
	ON_FRESH_VI("bt_ok", FRCMD_INIT, FrSupplyPackDlg::OnOkayBtnInit)
	ON_FRESH_VV("bt_cancel", FRCMD_LBUTTONUP, FrSupplyPackDlg::OnCancelBtnUp)
	ON_FRESH_VV("bt_ok", FRCMD_LBUTTONUP, FrSupplyPackDlg::OnOkayBtnUp)
	ON_FRESH_VV("bg_area", FRCMD_OWNERDRAW, FrSupplyPackDlg::OnBgAreaDraw)
	ON_FRESH_VI("lt_item", FRCMD_OWNERDRAW, FrSupplyPackDlg::OnItemListDraw)
	ON_FRESH_VV("lt_item", FRCMD_LBUTTONUP, FrSupplyPackDlg::OnItemListUp)
	END_FRESH_MSGMAP()

	FrSupplyPackDlg::FrSupplyPackDlg()
		: m_typeId(0),
		  m_pItemList(NULL),
		  m_pCancelBtn(NULL),
		  m_pExhibition(NULL),
		  m_alpha(0),
		  m_pSelectedItem(NULL),
		  m_pWindFont(NULL),
		  m_waitTime(WAIT_TIME),
		  m_bStarted(0),
		  m_supplyCount(0),
		  m_elapsed(1),
		  m_arrowOffset(0),
		  m_baseQuantity(0),
		  m_quantity(0),
		  m_step(0)
	{
		(void)&g_HUGE;
		m_pWindFont = new CWindFont;
		m_pBlueWindFont = new CBlueWindFontBig;
		m_pFontAnimation = new CFontAnimation(m_pBlueWindFont);
		memset(m_bonus, 0, sizeof(m_bonus));
		g_input->BlockKey("ESCAPE");
	}

	FrSupplyPackDlg::~FrSupplyPackDlg()
	{
		g_input->FreeKey("ESCAPE");
		if (m_pFontAnimation)
		{
			delete m_pFontAnimation;
			m_pFontAnimation = NULL;
		}
		if (m_pBlueWindFont)
		{
			delete m_pBlueWindFont;
			m_pBlueWindFont = NULL;
		}
		if (m_pWindFont)
		{
			delete m_pWindFont;
			m_pWindFont = NULL;
		}
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
	}

	void FrSupplyPackDlg::Initialize(unsigned long typeId)
	{
		m_typeId = typeId;
		m_pItemList->EnableToolTip(true);
		m_pFontAnimation->SetStartPoint(
			WVector2D(m_rect.x + m_rect.w * 0.5f, m_rect.y + m_rect.h * 0.5f));
		switch (GetSupplyGroup(m_typeId))
		{
		case 5:
			FrForm::SetDesc(
				"\xbc\xb1\xc5\xc3\xb5\xc8\x20\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7\x20\xbc\xf6\xb7\xae\xc0\xbb\x20\xba\xb8\xc3\xe6\x20\xc7\xd5\xb4\xcf\xb4\xd9\x2e");
			FrForm::SetCaption(
				"\xbe\xc6\xc1\xee\xc5\xd8\x20\xbc\xf6\xb7\xae\x20\xba\xb8\xc3\xe6\x20\xc6\xd1");
			for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
				it != Doc()->m_ballList.end(); ++it)
			{
				if (IsSuppliableItem(it->tid) && it->Common[0] > 0)
				{
					m_itemList.push_back(*it);
					m_pItemList->AddItem(&m_itemList.back());
				}
			}
			while (m_pItemList->GetCurrentItemSize(true) % 3)
				m_pItemList->AddItem(NULL);
			break;
		default:
			m_typeId = 0;
			break;
		}
	}

	void FrSupplyPackDlg::UpdateData(unsigned int count)
	{
		if (m_bStarted || count < 10)
		{
			FrForm::Close(FrCANCEL, true);
			FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), NULL,
				"notify", NULL);
			form->SetMessage(
				"\xbe\xf7\xb5\xa5\xc0\xcc\xc6\xae\x20\xbf\xa1\xb4\xcf\xb8\xde\xc0\xcc\xbc\xc7\xbf\xa1\x20\xb9\xae\xc1\xa6\xb0\xa1\x20\xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9\x2e",
				false);
			form->Open(NULL, FrESCAPE | FrENTER);
			return;
		}
		m_elapsed = 1;
		m_bStarted = 1;
		m_supplyCount = count;
		for (int i = 0; i < 10; ++i)
			++m_bonus[i];
		unsigned int extra = count - 10;
		for (unsigned int i = 0; i != extra; ++i)
		{
			int index = rand() % 10;
			m_bonus[index]++;
		}
	}

	void FrSupplyPackDlg::OnProc(const float dt)
	{
		if (!m_typeId)
		{
			FrForm::Close(FrCANCEL, true);
			return;
		}
		if (m_pSelectedItem && m_pExhibition)
		{
			if (m_alpha < 255)
			{
				m_alpha += dt * 120;
				if (m_alpha > 255)
					m_alpha = 255;
			}
			m_pExhibition->Process(dt, false);
		}
		if (m_bStarted && m_alpha >= 250)
		{
			static bool right = true;
			if (right)
			{
				if (m_arrowOffset < 3)
					m_arrowOffset += dt * 12;
				if (m_arrowOffset >= 3)
					right = false;
			}
			else
			{
				if (m_arrowOffset > -3)
					m_arrowOffset -= dt * 12;
				if (m_arrowOffset <= -3)
					right = true;
			}
			if (m_waitTime > 0 || m_step < 10)
			{
				m_elapsed += dt;
				m_waitTime -= dt;
				if (m_elapsed > 0.5f)
				{
					m_elapsed = 0;
					int step = m_step;
					if (step < 0)
						step = 0;
					else if (step > 9)
						step = 9;
					int bonus = m_bonus[step];
					WVector2D dir((rand() % 201 - 100) * 0.01f, -1);
					dir.Normalize();
					m_pFontAnimation->AddFontFx(MakeStr("+%d", bonus), dir, 80,
						1.5f);
					m_quantity += bonus;
					++m_step;
				}
			}
			else if (m_step == 10)
			{
				m_pOkayBtn->Enable(true);
				++m_step;
			}
			m_pFontAnimation->Process(dt);
		}
	}

	bool FrSupplyPackDlg::OnResultConfirm(int ret, FrForm* form)
	{
		m_pItemList->EnableToolTip(true);
		if (ret == FrOK)
		{
			sItemInfo* item = (sItemInfo*)m_pSelectedItem->pData;
			if (item)
			{
				IFF_ITEM_COMMON* common =
					ItemManager()->FindCommonItem(item->tid);
				if (common)
				{
					m_pItemList->EnableToolTip(false);
					WRect rect = m_pItemList->GetRect();
					rect.x -= 7;
					m_pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
						rect.y + rect.h / 3.0f, 0, false);
					m_pExhibition->SetArea(rect.w, rect.h);
					m_pExhibition->SetDistance(1.2f, true, false);
					m_pExhibition->SetModel(item->tid);
					m_pExhibition->SetEnableRotate(false);
					m_pExhibition->SetEnableZoom(false);
					m_pExhibition->SetAlpha((unsigned char)m_alpha);
					m_pOkayBtn->SetVisible(true);
					m_pOkayBtn->Enable(false);
					m_pCancelBtn->Enable(false);
					m_pCancelBtn->SetVisible(false);
					m_waitTime = WAIT_TIME;
					m_quantity = m_baseQuantity = item->Common[0];
					WSendPacket packet((enumClientPacket)238);
					packet.Encode4(m_typeId);
					packet.Encode4(item->tid);
					packet.Send(TO_GAME);
					AfxGetTask()->GetMainActor()
						<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
				}
			}
		}
		else
			m_pSelectedItem = NULL;
		return true;
	}

	void FrSupplyPackDlg::OnItemListInit(int param)
	{
		m_pItemList = DYNAMIC_CAST(FrListBox, param);
	}

	void FrSupplyPackDlg::OnOkayBtnInit(int param)
	{
		m_pOkayBtn = DYNAMIC_CAST(FrButton, param);
		m_pOkayBtn->SetVisible(false);
	}

	void FrSupplyPackDlg::OnCancelBtnInit(int param)
	{
		m_pCancelBtn = DYNAMIC_CAST(FrButton, param);
	}

	void FrSupplyPackDlg::OnCancelBtnUp()
	{
		FrForm::Close(FrCANCEL, true);
	}

	void FrSupplyPackDlg::OnOkayBtnUp()
	{
		FrForm::Close(FrOK, true);
	}

	void FrSupplyPackDlg::OnBgAreaDraw()
	{
		FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
		if (m_pExhibition)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("supplypack_bg_2");
			if (bitmap)
			{
				WRect rect = m_rect;
				rect.x += 9;
				rect.y += 53;
				rect.w = bitmap->Width();
				rect.h = bitmap->Height();
				device->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
			bitmap = g_pFresh->GetBitmap("supplypack_tex_2");
			if (bitmap)
			{
				WRect rect = m_rect;
				rect.x += (rect.w - bitmap->Width()) * 0.5f;
				rect.y += 28;
				rect.w = bitmap->Width();
				rect.h = bitmap->Height();
				device->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
			m_pExhibition->SetAlpha((unsigned char)m_alpha);
			m_pExhibition->Display(0, 0);
			bitmap = g_pFresh->GetBitmap("supplypack_num");
			if (bitmap)
			{
				WRect rect = m_rect;
				rect.x += (rect.w - bitmap->Width()) * 0.5f;
				rect.y += 192;
				rect.w = bitmap->Width();
				rect.h = bitmap->Height();
				device->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
			m_pWindFont->SetColor(((unsigned char)m_alpha << 24) | 0xffffff);
			{
				WRect pos = m_rect;
				float y = pos.y + 212;
				m_pWindFont->Print(pos.x + 71, y, MakeStr("%d", m_baseQuantity),
					TFA_CENTER, 1, 1);
				m_pWindFont->SetColor(
					((unsigned char)m_alpha << 24) | 0xff80ff);
				m_pWindFont->Print(pos.x + 177, y, MakeStr("%d", m_quantity),
					TFA_CENTER, 1, 1);
			}
			bitmap = g_pFresh->GetBitmap("supplypack_arrow");
			if (bitmap)
			{
				WRect rect = m_rect;
				rect.x += (rect.w - bitmap->Width()) * 0.5f;
				rect.x += m_arrowOffset;
				rect.y += 205;
				rect.w = bitmap->Width();
				rect.h = bitmap->Height();
				device->DrawTexture(bitmap, rect,
					((unsigned char)m_alpha << 24) | 0xffffff, 0);
			}
		}
		else
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("supplypack_bg_1");
			if (bitmap)
			{
				WRect rect = m_rect;
				rect.x += 13;
				rect.y += 53;
				rect.w = bitmap->Width();
				rect.h = bitmap->Height();
				device->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
			bitmap = g_pFresh->GetBitmap("supplypack_tex_1");
			if (bitmap)
			{
				WRect rect = m_rect;
				rect.x += (rect.w - bitmap->Width()) * 0.5f;
				rect.y += 28;
				rect.w = bitmap->Width();
				rect.h = bitmap->Height();
				device->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
		}
		if (m_bStarted)
			m_pFontAnimation->Display();
	}

	void FrSupplyPackDlg::OnItemListDraw(int param)
	{
		if (!m_pItemList->IsVisible())
			return;
		FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
		FrListItem* listItem = (FrListItem*)param;
		if (!listItem)
			return;
		float alpha = 255 - m_alpha;
		if (Wabs(alpha) < g_EPSILON)
		{
			m_pItemList->SetVisible(false);
			return;
		}
		sItemInfo* item = (sItemInfo*)listItem->pData;
		if (!item)
		{
			if (listItem == m_pItemList->GetItemUnderCursor() &&
				m_pItemList->IsEnableToolTip())
				m_pItemList->SetToolTipText(std::string(""));
			return;
		}
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->tid);
		if (!common)
			return;
		const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
		if (!bitmap)
			return;
		unsigned long color;
		float x = (m_pItemList->GetItemWidth() - bitmap->Width()) * 0.5f;
		float y = (m_pItemList->GetItemHeight() - bitmap->Height()) * 0.5f;
		WRect dest(x + listItem->pos.x, y + listItem->pos.y, bitmap->Width(),
			bitmap->Height());
		color = ((unsigned char)alpha << 24) | 0xffffff;
		device->DrawTexture(bitmap, dest, color, 0);
		m_pWindFont->SetColor(color);
		m_pWindFont->Print(listItem->pos.x + m_pItemList->GetItemWidth(),
			listItem->pos.y + m_pItemList->GetItemHeight() -
				m_pWindFont->GetFontH(),
			MakeStr("%d", item->Common[0]), TFA_RIGHT, 1, 1);
		if (!m_pSelectedItem)
		{
			if (listItem != m_pItemList->GetItemUnderCursor())
				return;
		}
		else if (listItem != m_pSelectedItem)
			return;
		{
			const Bitmap* selected = g_pFresh->GetBitmap("supplypack_btn_ch_o");
			{
				WRect rect(listItem->pos.x, listItem->pos.y,
					(float)m_pItemList->GetItemWidth(),
					(float)m_pItemList->GetItemHeight());
				device->DrawTexture(selected, rect, color, 0);
			}
			if (m_pItemList->IsEnableToolTip())
				m_pItemList->SetToolTipText(
					std::string(MakeStr("%s", common->Name)));
		}
	}

	void FrSupplyPackDlg::OnItemListUp()
	{
		FrListItem* listItem = m_pItemList->GetItemUnderCursor();
		if (!listItem)
			return;
		sItemInfo* item = (sItemInfo*)listItem->pData;
		if (!item)
			return;
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->tid);
		if (!common)
			return;
		if (m_pExhibition || m_pSelectedItem)
			return;
		m_pSelectedItem = listItem;
		m_pItemList->EnableToolTip(false);
		FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this,
			"notify_yesno", NULL);
		form->SetMessage(
			MakeStr(
				"\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5\x20\xbe\xc6\xc0\xcc\xc5\xdb\x20\x27\x25\x73\x27\xc0\xc7\x20\xbc\xf6\xb7\xae\xc0\xbb\x20\xba\xb8\xc3\xe6\x20\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
				ItemManager()->GetItemName(item->tid)),
			false);
		form->Open((FRESH_PFN_RESULT)&FrSupplyPackDlg::OnResultConfirm,
			FrESCAPE);
	}
}
