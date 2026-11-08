#include "specialboxdlg.h"
#include "frarea.h"
#include "fredit.h"
#include "frgaugebar.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include "messagemanager.h"
extern Fresh* g_pFresh;
namespace _gateway
{
	IMPLEMENT_OBJECT(FrSpecialBoxDlg, FrForm)

	BEGIN_FRESH_MSGMAP(FrSpecialBoxDlg, FrForm)

	ON_FRESH_VI("notify_area", FRCMD_INIT, FrSpecialBoxDlg::OnNotifyAreaInit)
	ON_FRESH_VI("notify_area", FRCMD_OWNERDRAW,
		FrSpecialBoxDlg::OnNotifyAreaOwnerdraw)

	ON_FRESH_VI("explane", FRCMD_INIT, FrSpecialBoxDlg::OnExplaneEditInit)
	ON_FRESH_VI("progressbar", FRCMD_INIT, FrSpecialBoxDlg::OnProgressBarInit)

	END_FRESH_MSGMAP()

	FrSpecialBoxDlg::FrSpecialBoxDlg()
	{
		m_pNotifyArea = NULL;
		m_pExplaneEdit = NULL;
		m_pProgressBar = NULL;
		m_pKeyItemBitmap = NULL;

		m_bDisableProgress = false;

		m_fOpenDelay = 0.0f;
		m_fElapsed = 0.0f;
	}

	bool FrSpecialBoxDlg::OnInit()
	{
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(0x1A00015C);

		m_pKeyItemBitmap = g_pFresh->GetBitmap(pItem->Icon);

		m_fOpenDelay = 3.0f;

		return FrForm::OnInit();
	}

	void FrSpecialBoxDlg::OnProc(const float fElapsed)
	{
		if (m_bDisableProgress)
		{
			return;
		}

		m_fElapsed += fElapsed;
		if (m_fElapsed >= m_fOpenDelay)
		{
			m_pProgressBar->SetPos(100);
			m_bDisableProgress = true;
			WSendPacket send((enumClientPacket)0xf1);
			send.Encode4(0x1A00015B);
			send.Send(TO_GAME);

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			Close(FrOK, true);
		}
		else
		{
			int pos = (int)(m_fElapsed / m_fOpenDelay * 100.0f);
			m_pProgressBar->SetPos(pos);
		}
	}

	void FrSpecialBoxDlg::SetMessageType(int type)
	{
		const char* msg = _systemmsg::CMessageManager::Instance()->GetMsg(
			(_systemmsg::eMsgIdentifier)type);

		m_pExplaneEdit->AddText(msg, false, true);
	}

	void FrSpecialBoxDlg::SetMessage(const char* msg)
	{
		m_pExplaneEdit->AddText(msg, false, true);
	}

	void FrSpecialBoxDlg::DisbleProgressBar()
	{
		m_bDisableProgress = true;
		if (m_pProgressBar)
		{
			m_pProgressBar->SetVisible(false);
		}
	}

	void FrSpecialBoxDlg::OnNotifyAreaInit(int param)
	{
		m_pNotifyArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	}

	void FrSpecialBoxDlg::OnNotifyAreaOwnerdraw(int param)
	{
		g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pKeyItemBitmap,
			WRect(m_rect.x + 150.0f, m_rect.y + 20.0f,
				(float)m_pKeyItemBitmap->Width(),
				(float)m_pKeyItemBitmap->Height()),
			0xFFFFFFFF, 0);
	}

	void FrSpecialBoxDlg::OnExplaneEditInit(int param)
	{
		m_pExplaneEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	}

	void FrSpecialBoxDlg::OnProgressBarInit(int param)
	{
		m_pProgressBar = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);

		m_pProgressBar->SetRange(0, 100, 0);
	}
}
