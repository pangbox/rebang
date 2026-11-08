#include "emoticondlg.h"
#include "frlistbox.h"
#include "frframe.h"
#include "fremoticon.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "inputmanager.h"
#include "../../shared/localize.h"
#include "wlocalize.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrEmoticonDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrEmoticonDlg, FrForm)

ON_FRESH_VI("list", FRCMD_INIT, FrEmoticonDlg::OnListInit)
ON_FRESH_VI("list", FRCMD_OWNERDRAW, FrEmoticonDlg::OnListOwnerDraw)
ON_FRESH_VV("list", FRCMD_LBUTTONUP, FrEmoticonDlg::OnListBtnUp)

END_FRESH_MSGMAP()

FrEmoticonDlg::FrEmoticonDlg()
{
	m_pList = NULL;
	m_selectedIcon[0] = '\0';
}

FrEmoticonDlg::~FrEmoticonDlg()
{
}

void FrEmoticonDlg::OnOK()

{
	FrEmoticon* pEmo = g_pFresh->GetManager()->GetEmoticon();
	FrListItem* pItem = m_pList->GetItemUnderCursor();

	if (pEmo && pItem)
	{
		const char* name = pEmo->GetIconName((int)pItem->pData);
		if (name)
		{
			if (IsLocalCountry(30))
			{
				static const char* szOpen = K2L_Compatibility("(");
				static const char* szClose = K2L_Compatibility(")");

				static const char* szFlip = K2L_Compatibility("-");

				strcpy(m_selectedIcon, szOpen);
				if (g_input->Get("LSHIFT", false) ||
					g_input->Get("RSHIFT", false))
					strcat(m_selectedIcon, szFlip);
				strcat(m_selectedIcon, name);
				strcat(m_selectedIcon, szClose);
			}
			else
			{
				strcpy(m_selectedIcon, "(");
				if (g_input->Get("LSHIFT", false) ||
					g_input->Get("RSHIFT", false))
					strcat(m_selectedIcon, "-");
				strcat(m_selectedIcon, name);
				strcat(m_selectedIcon, ")");
			}
		}
	}

	FrForm::OnOK();
}

bool FrEmoticonDlg::OnInit()

{
	if (m_pBaseFrm)
		m_pBaseFrm->UseEmoAtDesc(false);

	return true;
}

void FrEmoticonDlg::OnListInit(int param)

{
	m_pList = DYNAMIC_CAST(FrListBox, param);
	if (!m_pList)
		return;

	FrEmoticon* pEmo = g_pFresh->GetManager()->GetEmoticon();
	if (!pEmo)
		return;

	int num = pEmo->GetIconNum();

	for (int i = 0; i < num; i++)
		m_pList->AddItem((void*)i);

	m_startTime = timeGetTime();

	m_pList->SetKeyFocus(true);
}

void FrEmoticonDlg::OnListOwnerDraw(int param)

{
	FrEmoticon* pEmo = g_pFresh->GetManager()->GetEmoticon();
	if (!pEmo)
		return;

	if (!param)
		return;

	pEmo->SetAnim(true);
	pEmo->SetAnimTime(m_startTime);

	FrListItem* pItem = (FrListItem*)param;
	int icon = (int)pItem->pData;

	if (m_pList->GetItemUnderCursor() == pItem)
	{
		pEmo->Draw(icon,
			WRect(pItem->pos.x - 3.0f, pItem->pos.y - 3.0f,
				pEmo->GetWidth() + 6.0f, pEmo->GetHeight() + 6.0f),
			0xffffffff,
			g_input->Get("LSHIFT", false) || g_input->Get("RSHIFT", false));

		const char* name = pEmo->GetIconName(icon);
		if (name)
		{
			if (IsLocalCountry(30))
			{
				static const char* szOpen = K2L_Compatibility("(");
				static const char* szClose = K2L_Compatibility(")");

				static const char* szFlip = K2L_Compatibility("-");

				if (g_input->Get("LSHIFT", false) ||
					g_input->Get("RSHIFT", false))
					SetDesc(MakeStr("%s%s%s%s", szOpen, szFlip, name, szClose));
				else
					SetDesc(MakeStr("%s%s%s", szOpen, name, szClose));
			}
			else
			{
				if (g_input->Get("LSHIFT", false) ||
					g_input->Get("RSHIFT", false))
					SetDesc(MakeStr("(-%s)", name));
				else
					SetDesc(MakeStr("(%s)", name));
			}
		}
	}
	else

		pEmo->Draw(icon, pItem->pos.x, pItem->pos.y, 0xffffffff,
			g_input->Get("LSHIFT", false) || g_input->Get("RSHIFT", false));

	pEmo->SetAnim(false);
}

void FrEmoticonDlg::OnListBtnUp()

{
	FrEmoticon* pEmo = g_pFresh->GetManager()->GetEmoticon();
	if (!pEmo)
		return;

	FrListItem* pItem = m_pList->GetItemUnderCursor();
	if (!pItem)
		return;

	const char* name = pEmo->GetIconName((int)pItem->pData);
	if (name)
	{
		if (IsLocalCountry(30))
		{
			static const char* szOpen = K2L_Compatibility("(");
			static const char* szClose = K2L_Compatibility(")");

			static const char* szFlip = K2L_Compatibility("-");

			strcpy(m_selectedIcon, szOpen);
			if (g_input->Get("LSHIFT", false) || g_input->Get("RSHIFT", false))
				strcat(m_selectedIcon, szFlip);
			strcat(m_selectedIcon, name);
			strcat(m_selectedIcon, szClose);
		}
		else
		{
			strcpy(m_selectedIcon, "(");
			if (g_input->Get("LSHIFT", false) || g_input->Get("RSHIFT", false))
				strcat(m_selectedIcon, "-");
			strcat(m_selectedIcon, name);
			strcat(m_selectedIcon, ")");
		}
	}

	Close(FrOK, true);
}
