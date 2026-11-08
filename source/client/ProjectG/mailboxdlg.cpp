#include "mailboxdlg.h"
#include "notedlg.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"
#include "fresh.h"
#include "commonutil.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrMailBoxDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrMailBoxDlg, FrForm)

ON_FRESH_VI("note", FRCMD_INIT, FrMailBoxDlg::OnNoteInit)
ON_FRESH_VI("note", FRCMD_OWNERDRAW, FrMailBoxDlg::OnNoteOwnerDraw)
ON_FRESH_VV("note", FRCMD_LBUTTONUP, FrMailBoxDlg::OnNoteLBtnUp)
ON_FRESH_VV("note", FRCMD_RBUTTONUP, FrMailBoxDlg::OnNoteRBtnUp)
ON_FRESH_VV("note", FRCMD_DBLCLICK, FrMailBoxDlg::OnNoteRBtnUp)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrMailBoxDlg::OnCancelBtnUp)

END_FRESH_MSGMAP()

void FrMailBoxDlg::OnNoteInit(int param)
{
	m_pNoteList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pNoteList == NULL)
		return;

	m_pNoteList->UseRightButton(true);

	for (std::list<sNoteInfo>::iterator it = Doc()->m_noteList.begin();
		it != Doc()->m_noteList.end(); ++it)
	{
		m_pNoteList->AddItem(&(*it));
	}
}

void FrMailBoxDlg::OnNoteOwnerDraw(int param)
{
	if (m_pNoteList == NULL)
		return;

	if (param == NULL)
		return;

	sNoteInfo* pNote = (sNoteInfo*)((FrListItem*)param)->pData;
	if (pNote == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	if (pNote == m_pSelNote)
		pGDI->Box(WRect(((FrListItem*)param)->pos.x + 5.0f,
					  ((FrListItem*)param)->pos.y, 560.0f, 20.0f),
			0x999B9DFF, 0, 0.0f);

	pGDI->SetTextStyle(0);

	if (pNote->bReply)
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	else
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	FrEmoticon* pEmoticon = g_pFresh->GetManager()->GetEmoticon();

	if (pEmoticon)
	{
		pEmoticon->PrintText(WPoint(((FrListItem*)param)->pos.x + 8.0f,
								 ((FrListItem*)param)->pos.y + 3.0f),
			0, pNote->sNick, 0xFFFFFFFF);
		pEmoticon->PrintText(WPoint(((FrListItem*)param)->pos.x + 118.0f,
								 ((FrListItem*)param)->pos.y + 3.0f),
			0, pNote->sNote, 0xFFFFFFFF);
		pEmoticon->PrintText(WPoint(((FrListItem*)param)->pos.x + 467.0f,
								 ((FrListItem*)param)->pos.y + 3.0f),
			0, pNote->sTime, 0xFFFFFFFF);
	}
	else
	{
		pGDI->PrintText(WPoint(((FrListItem*)param)->pos.x + 8.0f,
							((FrListItem*)param)->pos.y + 3.0f),
			0, pNote->sNick, 0);
		pGDI->PrintText(WPoint(((FrListItem*)param)->pos.x + 118.0f,
							((FrListItem*)param)->pos.y + 3.0f),
			0, pNote->sNote, 0);
		pGDI->PrintText(WPoint(((FrListItem*)param)->pos.x + 467.0f,
							((FrListItem*)param)->pos.y + 3.0f),
			0, pNote->sTime, 0);
	}
}

void FrMailBoxDlg::OnNoteLBtnUp()
{
	if (m_pNoteList == NULL)
		return;

	FrListItem* pItem = m_pNoteList->GetItemUnderCursor();
	if (pItem == NULL)
		return;

	sNoteInfo* pNote = (sNoteInfo*)pItem->pData;
	if (pNote == NULL)
		return;

	m_pSelNote = pNote;
}

void FrMailBoxDlg::OnNoteRBtnUp()
{
	if (m_pNoteList == NULL)
		return;

	FrListItem* pItem = m_pNoteList->GetItemUnderCursor();
	if (pItem == NULL)
		return;

	sNoteInfo* pNote = (sNoteInfo*)pItem->pData;
	if (pNote == NULL)
		return;

	m_pSelNote = pNote;

	if (m_pSelNote->bReply)
		return;

	if (Doc()->m_myInfo.stat.i64Pang < 10)
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
			(int)MakeStr(
				"\xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbb\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad\xb4\xc2 %d\xc6\xce\xc0\xcc \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9.",
				10),
			0, 0, 0, 0);
		return;
	}
	const char* nick = m_pSelNote->sNick;
	if (nick[0] == '@')
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
			(int)MakeStr(
				"\xbf\xee\xbf\xb5\xc0\xda\xbf\xa1\xb0\xd4\xb4\xc2 \xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				10),
			0, 0, 0, 0);
		return;
	}

	if (m_pNoteDlg == NULL)
	{
		m_pNoteDlg =
			CreateForm<FrNoteDlg>(g_pFresh->GetManager(), this, "note", NULL);
		m_pNoteDlg->SetNick(nick, m_pSelNote->sNote);
		m_pNoteDlg->Open((FRESH_PFN_RESULT)&FrMailBoxDlg::OnNoteDlgResult, 1);
	}
}

bool FrMailBoxDlg::OnNoteDlgResult(int result, FrForm* pForm)
{
	if (result == FrOK)
	{
		m_pSelNote->bReply = true;

		WSendPacket send((enumClientPacket)0x3c);
		send.Encode2(0x111);

		send.Encode4(m_pSelNote->uid);
		send.EncodeStr(std::string(m_pNoteDlg->GetNote()));
		send.Encode1(0);
		send.Send(TO_GAME);
	}

	m_pNoteDlg = NULL;
	return true;
}

void FrMailBoxDlg::OnCancelBtnUp()
{
	FrForm* pForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
	pForm->SetMessage(
		"\xc2\xca\xc1\xf6\xc7\xd4\xc0\xbb \xb4\xdd\xc0\xb8\xb8\xe9 \xc2\xca\xc1\xf6\xb0\xa1 \xb8\xf0\xb5\xce \xbb\xe7\xb6\xf3\xc1\xfd\xb4\xcf\xb4\xd9.\n\xc2\xca\xc1\xf6\xc7\xd4\xc0\xbb \xb4\xdd\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		false);
	pForm->Open((FRESH_PFN_RESULT)&FrMailBoxDlg::OnCloseDlgResult, 0);
}

bool FrMailBoxDlg::OnCloseDlgResult(int result, FrForm* pForm)
{
	if (result == FrOK)
	{
		Doc()->m_noteList.clear();
		Close(true);
	}

	return true;
}
