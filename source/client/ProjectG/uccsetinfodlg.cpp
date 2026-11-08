#include "uccsetinfodlg.h"
#include "uccdrawdlg.h"
#include "uccclothes.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "fredit.h"
#include "frbutton.h"
#include "frarea.h"
#include "actor.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrUccSetInfoDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrUccSetInfoDlg, FrForm)

ON_FRESH_VI("name", FRCMD_INIT, FrUccSetInfoDlg::OnNameInit)
ON_FRESH_BI("name", FRCMD_ENTERKEY, FrUccSetInfoDlg::OnNameEnterKey)
ON_FRESH_VI("ok_btn", FRCMD_INIT, FrUccSetInfoDlg::OnOkBtnInit)
ON_FRESH_VV("ok_btn", FRCMD_LBUTTONUP, FrUccSetInfoDlg::OnOkBtnUp)
ON_FRESH_VI("cancel_btn", FRCMD_INIT, FrUccSetInfoDlg::OnCancelBtnInit)
ON_FRESH_VV("cancel_btn", FRCMD_LBUTTONUP, FrUccSetInfoDlg::OnCancelBtnUp)
ON_FRESH_VI("icon_area", FRCMD_INIT, FrUccSetInfoDlg::OnIconAreaInit)
ON_FRESH_VI("icon_area", FRCMD_OWNERDRAW, FrUccSetInfoDlg::OnIconAreaOwnerDraw)

END_FRESH_MSGMAP()

FrUccSetInfoDlg::FrUccSetInfoDlg()
{
	m_pParent = NULL;
}

void FrUccSetInfoDlg::OnNameInit(int param)
{
	m_pName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

bool FrUccSetInfoDlg::OnNameEnterKey(int param)
{
	OnOkBtnUp();
	return true;
}

void FrUccSetInfoDlg::OnOkBtnInit(int param)
{
	m_pOkBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccSetInfoDlg::OnOkBtnUp()
{
	if (!m_pParent)
		return;

	char nick[208];
	strcpy(nick, m_pName->GetLine(1, false));

	const char* err = Doc()->m_chatManager.FilteringNick(nick);
	if (err)
	{
		if (!strcmp(err, NICKNAME_ERR_EMPTY))
		{
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 0x23,
				(int)"\xc0\xc7\xbb\xf3 \xc0\xcc\xb8\xa7\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
		}
		else if (!strcmp(err, NICKNAME_ERR_LENGTH_LIMIT))
		{
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 0x23,
				(int)"\xc0\xc7\xbb\xf3 \xc0\xcc\xb8\xa7\xc0\xba \\c0xffff0000\\c\xbf\xb5\xb9\xae 4 - 16 \xb1\xdb\xc0\xda\\c0xff000000\\c \xb6\xc7\xb4\xc2 \\c0xffff0000\\c\xc7\xd1\xb1\xdb 2 - 8\xb1\xdb\xc0\xda\\c0xff000000\\c\xc0\xc7 \xb1\xe6\xc0\xcc\xb8\xa6 \xb0\xa1\xc1\xae\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}
		else if (!strcmp(err, NICKNAME_ERR_SPACE))
		{
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 0x23,
				(int)"\xc0\xc7\xbb\xf3 \xc0\xcc\xb8\xa7\xc0\xba \\c0xffff0000\\c\xb0\xf8\xb9\xe9\xb9\xae\xc0\xda\\c0xff000000\\c\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}
		else if (!strcmp(err, NICKNAME_ERR_INVALID_CHAR))
		{
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 0x23,
				(int)"\xc0\xc7\xbb\xf3 \xc0\xcc\xb8\xa7\xc0\xb8\xb7\xce \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb9\xae\xc0\xda\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}
		else if (!strcmp(err, NICKNAME_ERR_BAD_WORD) ||
			!strcmp(err, NICKNAME_ERR_CONTAINS_BAD_WORD))
		{
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 0x23,
				(int)"\xc0\xc7\xbb\xf3 \xc0\xcc\xb8\xa7\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xc7\xa5\xc7\xf6\xc0\xcc \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}
	}
	else
	{
		std::string name = m_pName->GetLine(1, false);
		name = name.substr(0, Min((int)name.size(), 40));
		m_pParent->SetName(name.c_str());

		OnOK();
	}
}

void FrUccSetInfoDlg::OnCancelBtnInit(int param)
{
	m_pCancelBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccSetInfoDlg::OnCancelBtnUp()
{
	OnCancel();
}

void FrUccSetInfoDlg::OnIconAreaInit(int param)
{
	m_pIconArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccSetInfoDlg::OnIconAreaOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = GetGDI();
	if (!pGDI)
		return;

	WRect rect = m_rect;

	if (m_pParent && m_pParent->m_pItemInfo)
	{
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(
			0x4000000 | ((m_pParent->m_pItemInfo->tid >> 18) & 0xff));
		if (pChar)
		{
			const Bitmap* pBmp =
				g_pFresh->GetBitmap(MakeStr("s_%s", pChar->c.Icon));
			if (pBmp)

				pGDI->DrawTexture(pBmp,
					WRect(rect.x + 60.0f, rect.y + 68.0f, (float)pBmp->Width(),
						(float)pBmp->Height()),
					0xffffffff, 0);
		}
	}

	if (m_thumbnail.Width() > 0 && m_thumbnail.Height() > 0)
	{
		pGDI->DrawTexture(&m_thumbnail,
			WRect(rect.x + 66.0f, rect.y + 70.0f, (float)m_thumbnail.Width(),
				(float)m_thumbnail.Height()),
			0xffffffff, 0);
	}
}

void FrUccSetInfoDlg::SetParent(FrUccDrawDlg* pParent)
{
	m_pParent = pParent;

	Bitmap* texture = m_pParent->m_pClothes[0]->GetTexture();
	Bitmap* mask = m_pParent->m_pClothes[0]->GetMask();

	m_thumbnail.Create(64, 84, 32);
	CreateItemThumbnail(texture, mask, &m_thumbnail);
}
