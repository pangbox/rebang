#include "notedlg.h"
#include "fredit.h"
#include "frstatic.h"
#include "frarea.h"
#include "frframe.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "fresh.h"
#include "golftask.h"
#include "actor.h"
#include "commonutil.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrNoteDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrNoteDlg, FrForm)

ON_FRESH_VI("paid", FRCMD_INIT, FrNoteDlg::OnPaidInit)
ON_FRESH_VI("nick", FRCMD_INIT, FrNoteDlg::OnNickInit)
ON_FRESH_VI("snick", FRCMD_INIT, FrNoteDlg::OnNickStaticInit)
ON_FRESH_VI("received", FRCMD_INIT, FrNoteDlg::OnReceivedInit)
ON_FRESH_VI("sreceived", FRCMD_INIT, FrNoteDlg::OnReceivedStaticInit)
ON_FRESH_VI("note", FRCMD_INIT, FrNoteDlg::OnNoteInit)
ON_FRESH_VV("note", FRCMD_LBUTTONDOWN, FrNoteDlg::OnNoteLBtnDown)
ON_FRESH_BI("note", FRCMD_ENTERKEY, FrNoteDlg::OnNoteEnterKey)
ON_FRESH_VI("snote", FRCMD_INIT, FrNoteDlg::OnNoteStaticInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrNoteDlg::OnOKBtnUp)

END_FRESH_MSGMAP()

void FrNoteDlg::OnPaidInit(int param)
{
	m_pPaid = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrNoteDlg::OnNoteInit(int param)
{
	m_pNote = DYNAMIC_CAST(FrEdit, (FrWnd*)param);

	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		g_pFresh->GetManager()->SetExclusiveKey(true);
	}
}

void FrNoteDlg::OnNoteLBtnDown()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		g_pFresh->GetManager()->SetExclusiveKey(true);
	}
}

bool FrNoteDlg::OnNoteEnterKey(int param)
{
	if (!m_pNote->GetStyle().GetFlag(FWS_DISABLED))
		OnOKBtnUp();

	return false;
}

void FrNoteDlg::OnNoteStaticInit(int param)
{
	m_pNoteStatic = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrNoteDlg::OnNickInit(int param)
{
	m_pNick = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrNoteDlg::OnNickStaticInit(int param)
{
	m_pNickStatic = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrNoteDlg::OnReceivedInit(int param)
{
	m_pReceived = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrNoteDlg::OnReceivedStaticInit(int param)
{
	m_pReceivedStatic = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrNoteDlg::SetID(const char* nick, const char* received)
{
	m_pNickStatic->SetCaption("\xbe\xc6\xc0\xcc\xb5\xf0");

	SetNick(nick, received);

	if (m_pBaseFrm)
		m_pBaseFrm->SetDesc("");
}

void FrNoteDlg::SetNick(const char* nick, const char* received)
{
	if (m_pNick)
		m_pNick->SetLine(1, nick, 0, false, 0);

	if (received)
	{
		if (m_pReceived)
			m_pReceived->SetLine(1, received, 0, false, 0);

		WRect rect;
		if (m_pNote)
		{
			m_pNote->GetClientRect(rect);

			rect.y += 30.0f;
			m_pNote->SetClientRect(rect);
		}

		if (m_pNoteStatic)
		{
			m_pNoteStatic->GetClientRect(rect);

			rect.y += 30.0f;
			m_pNoteStatic->SetClientRect(rect);
		}
	}
	else
	{
		if (m_pReceived)
			m_pReceived->SetVisible(false);
		if (m_pReceivedStatic)
			m_pReceivedStatic->SetVisible(false);
		if (m_pPaid)
			m_pPaid->SetVisible(false);
	}

	if (m_pBaseFrm)
		m_pBaseFrm->SetDesc(MakeStr(
			"\xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbb\xb8\xe9 \xbf\xec\xc7\xa5\xb0\xaa\xc0\xb8\xb7\xce %d\xc6\xce\xc0\xcc \xbc\xd2\xba\xf1\xb5\xcb\xb4\xcf\xb4\xd9.",
			10));
}

const char* FrNoteDlg::GetNote()
{
	if (m_pNote == NULL)
		return NULL;

	const char* note = m_pNote->GetLine(1, false);
	Doc()->m_chatManager.FilteringHack(note, true);

	return note;
}

void FrNoteDlg::OnOKBtnUp()
{
	if (strlen(m_pNote->GetLine(1, false)) == 0)
	{
		AfxGetTask()->GetMainActor() << MsgObject(NULL, 35,
			(int)"\xc2\xca\xc1\xf6 \xb3\xbb\xbf\xeb\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
			0, 0, 0, 0);
		return;
	}

	OnFreshOkay();
}

void FrNoteDlg::OnProc(const float deltaTime)
{
	if (m_bFocused == false)
	{
		if (m_elapsed > 0.3f)
		{
			WndManager()->GetDesktop()->ResetKeyFocus();
			m_pNote->SetKeyFocus(true);
			m_bFocused = true;
		}

		m_elapsed += deltaTime;
	}
}
