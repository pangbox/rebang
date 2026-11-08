#include "compounddlg.h"
#include "frwnd.h"
#include "frarea.h"
#include "frbutton.h"
#include "frgaugebar.h"
#include "fredit.h"
#include "fresh.h"
#include "soundmanager.h"
#include "projectg.h"
#include "../../Wangreal/include/wfont.h"
#include "../../Wangreal/include/wresrcmng.h"

IMPLEMENT_OBJECT(FrCompoundDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrCompoundDlg, FrForm)

ON_FRESH_VI("yes", FRCMD_INIT, FrCompoundDlg::OnYesInit)
ON_FRESH_VV("yes", FRCMD_LBUTTONUP, FrCompoundDlg::OnYesBtnUp)
ON_FRESH_VI("no", FRCMD_INIT, FrCompoundDlg::OnNoInit)
ON_FRESH_VV("no", FRCMD_LBUTTONUP, FrCompoundDlg::OnNoBtnUp)
ON_FRESH_VI("ok", FRCMD_INIT, FrCompoundDlg::OnOKInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrCompoundDlg::OnOKBtnUp)
ON_FRESH_VI("gauge", FRCMD_INIT, FrCompoundDlg::OnGaugeBarInit)
ON_FRESH_VI("etext", FRCMD_INIT, FrCompoundDlg::OnTextEditInit)
ON_FRESH_VI("compres", FRCMD_INIT, FrCompoundDlg::OnCompResInit)
ON_FRESH_VV("compres", FRCMD_OWNERDRAW, FrCompoundDlg::OnCompResOwnerDraw)

END_FRESH_MSGMAP()

FrCompoundDlg::FrCompoundDlg()
{
	memset(&m_quest, 0, sizeof(m_quest));
	m_pCompRes = NULL;
	m_bShowResult = false;
	memset(m_pBtn, 0, sizeof(m_pBtn));
	m_pGaugeBar = NULL;
	m_pTextEdit = NULL;
	m_result = 0xff;
	memset(&m_resultItem, 0, sizeof(m_resultItem));

	static char* s_fontFile[] = { "[font_wind.jpg" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = s_fontFile;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

FrCompoundDlg::~FrCompoundDlg()
{
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

void FrCompoundDlg::SetCompound(IFF_STRUCT::sQuest& quest)
{
	m_quest = quest;

	if (m_pTextEdit)
		m_pTextEdit->ClearLine();

	IFF_STRUCT::sQuestDrop* pDrop =
		ItemManager()->FindQuestDrop(m_quest.DropTid[0]);

	if (m_pTextEdit)
	{
		m_pTextEdit->AddText(
			MakeStr(
				"\\c0xffff0000\\c\xc0\xaf\xb4\xcf\xc5\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\\c0xff000000\\c\xc0\xc7 \xc1\xb6\xc7\xd5\xbf\xa1\n\\c0xffff0000\\c%s %d\\c0xff000000\\c\xb0\xb3\xb0\xa1 \xbc\xd2\xba\xf1\xb5\xcb\xb4\xcf\xb4\xd9.",
				pDrop ? pDrop->c.Name : "??", m_quest.DropNum[0]),
			false, true);
	}

	if (Doc()->CanCompound(m_quest.TypeId))
	{
		m_pTextEdit->AddText(
			"\xc1\xb6\xc7\xd5\xc0\xbb \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false, true);

		if (m_pBtn[0])
			m_pBtn[0]->SetVisible(true);
		if (m_pBtn[1])
			m_pBtn[1]->SetVisible(true);
	}
	else
	{
		m_pTextEdit->AddText(
			MakeStr(
				"\\c0xffff0000\\c%s\\c0xff000000\\c\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
				pDrop ? pDrop->c.Name : "??"),
			false, true);

		if (m_pBtn[2])
			m_pBtn[2]->SetVisible(true);
	}
}

void FrCompoundDlg::SetCompoundRes(unsigned char result, sItemInfo& itemInfo)
{
	m_result = result;
	m_resultItem = itemInfo;
}

void FrCompoundDlg::OnProc(const float time)
{
	if (m_pGaugeBar == NULL)
		return;

	if (WisEqual(m_pGaugeBar->GetPos(), 100.0f, g_EPSILON) && !m_bShowResult)
	{
		if (m_result != 0xff)
		{
			m_bShowResult = true;

			if (m_pBtn[2])
				m_pBtn[2]->Enable(true);

			if (m_pTextEdit)
			{
				m_pTextEdit->ClearLine();

				switch (m_result)
				{
				case 0:
					m_pTextEdit->AddText(
						"\xc1\xb6\xc7\xd5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
						false, true);
					break;

				case 1:
				{
					const char* name =
						ItemManager()->GetItemName(m_resultItem.tid);
					if (name == NULL)
						name = "??";
					m_pTextEdit->AddText(
						MakeStr(
							"\xc3\xe0\xc7\xcf\xc7\xd5\xb4\xcf\xb4\xd9!\n\\c0xffff0000\\c%s\\c0xff000000\\c\xb8\xa6 \xc8\xb9\xb5\xe6\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.\n'\xb8\xb6\xc0\xcc\xb7\xeb > \xc0\xc7\xbb\xf3\xbd\xc7'\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.",
							name),
						false, true);

					if (m_pCompRes)
					{
						IFF_ITEM_COMMON* pItem =
							ItemManager()->FindCommonItem(m_resultItem.tid);
						if (pItem)
							m_pCompRes->SetBgImg(pItem->Icon);
						m_pCompRes->SetAlpha(0.0f);
					}

					g_audio->PlaySfx("ui_desktop_icon_click");
					break;
				}
				}
			}
		}
	}

	if (m_bShowResult && m_pCompRes)
	{
		float alpha = time + m_pCompRes->GetAlpha();
		if (1.0f < alpha)
			alpha = 1.0f;
		m_pCompRes->SetAlpha(alpha);
	}
}

void FrCompoundDlg::OnYesInit(int param)
{
	m_pBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pBtn[0])
		m_pBtn[0]->SetVisible(false);
}

void FrCompoundDlg::OnYesBtnUp()
{
	if (m_pBtn[0])
		m_pBtn[0]->SetVisible(false);
	if (m_pBtn[1])
		m_pBtn[1]->SetVisible(false);

	if (m_pBtn[2])
	{
		m_pBtn[2]->SetVisible(true);
		m_pBtn[2]->Enable(false);
	}

	if (m_pGaugeBar)
	{
		m_pGaugeBar->SetVisible(true);
		m_pGaugeBar->SetDestPos(100, false);
		m_pGaugeBar->SetColor(0xffff0000);
	}

	if (m_pTextEdit)
	{
		IFF_STRUCT::sQuestDrop* pDrop =
			ItemManager()->FindQuestDrop(m_quest.DropTid[0]);
		m_pTextEdit->ClearLine();
		m_pTextEdit->AddText(
			MakeStr(
				"\\c0xffff0000\\c%s\\c0xff000000\\c\xc0\xbb \xc1\xb6\xc7\xd5 \xc1\xdf \xc0\xd4\xb4\xcf\xb4\xd9.\n\xc0\xe1\xbd\xc3\xb8\xb8 \xb1\xe2\xb4\xd9\xb7\xc1 \xc1\xd6\xbc\xbc\xbf\xe4.",
				pDrop ? pDrop->c.Name : ""),
			false, true);
	}

	WSendPacket packet(0x68);
	packet.Encode4(m_quest.TypeId);
	packet.Send(TO_GAME);
}

void FrCompoundDlg::OnNoInit(int param)
{
	m_pBtn[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pBtn[1])
		m_pBtn[1]->SetVisible(false);
}

void FrCompoundDlg::OnNoBtnUp()
{
	Close(FrCANCEL, true);
}

void FrCompoundDlg::OnOKInit(int param)
{
	m_pBtn[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pBtn[2])
		m_pBtn[2]->SetVisible(false);
}

void FrCompoundDlg::OnOKBtnUp()
{
	Close(FrOK, true);
}

void FrCompoundDlg::OnGaugeBarInit(int param)
{
	m_pGaugeBar = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	if (m_pGaugeBar)
	{
		m_pGaugeBar->SetVisible(false);
		m_pGaugeBar->SetBarSpeed(10.0f);
	}
}

void FrCompoundDlg::OnTextEditInit(int param)
{
	m_pTextEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrCompoundDlg::OnCompResInit(int param)
{
	m_pCompRes = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrCompoundDlg::OnCompResOwnerDraw()
{
	if (!m_bShowResult)
		return;

	if (m_pCompRes && WisEqual(m_pCompRes->GetAlpha(), 1.0f, g_EPSILON))
	{
		int type = m_resultItem.tid >> 26;
		if (type == 5 || type == 28)
		{
			float x = m_pCompRes->GetRect().x + 68.0f -
				m_pFont->GetTextWidth(g_view,
					MakeStr("%d", m_resultItem.Common[0]));
			m_pFont->Print(g_view, x, m_pCompRes->GetRect().y + 63.0f,
				MakeStr("%d", m_resultItem.Common[0]), 0, 0xffffffff, NULL);
		}
	}
}

void FrCompoundDlg::SetDlgText(const char* text)
{
	if (m_pTextEdit)
	{
		m_pTextEdit->ClearLine();
		m_pTextEdit->AddText(text, false, true);
	}
}
