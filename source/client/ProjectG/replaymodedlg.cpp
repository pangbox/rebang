#include "replaymodedlg.h"
#include "frbutton.h"
#include "actor.h"

IMPLEMENT_OBJECT(FrReplayModeDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrReplayModeDlg, FrForm)

ON_FRESH_VI("ok", FRCMD_INIT, FrReplayModeDlg::OnOkInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrReplayModeDlg::OnOkBtnUp)
ON_FRESH_VI("cancel", FRCMD_INIT, FrReplayModeDlg::OnCancelInit)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrReplayModeDlg::OnCancelBtnUp)

END_FRESH_MSGMAP()

FrReplayModeDlg::FrReplayModeDlg()
{
	m_pOk = NULL;
	m_pCancel = NULL;
	SetReplayMode(0);
}

void FrReplayModeDlg::OnOkInit(int param)
{
	m_pOk = DYNAMIC_CAST(FrButton, param);
}

void FrReplayModeDlg::OnOkBtnUp()
{
	if (m_replayMode == 1)
	{
		Doc()->m_replayShotIndex = 0;
		Doc()->m_replayDuration = 0;
		Doc()->m_replayTick = Doc()->m_curChannel.Type;
		Doc()->m_replayState = 14;
	}
	else if (m_replayMode == 2)
	{
		for (std::list<sRecordedItemInfo>::iterator it =
				 Doc()->m_recordedItemList.begin();
			it != Doc()->m_recordedItemList.end(); ++it)
		{
			if (stricmp((*it).fileName, Doc()->m_fileName) == 0)
			{
				Doc()->m_recordedItemList.erase(it);

				BOOL bRet = DeleteFile(Doc()->m_fileName);

				if (bRet)
					AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
						(int)"\xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xbb\xe8\xc1\xa6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
						(int)"\xbb\xe8\xc1\xa6\xc7\xcf\xb4\xc2 \xb5\xb5\xc1\xdf \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0));

				break;
			}
		}
	}

	m_replayMode = 0;
	OnFreshOkay();
}

void FrReplayModeDlg::OnCancelInit(int param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, param);
}

void FrReplayModeDlg::OnCancelBtnUp()
{
	OnFreshCancel();
}

void FrReplayModeDlg::SetReplayMode(int mode)
{
	m_replayMode = mode;
}

int FrReplayModeDlg::GetReplayMode()
{
	return m_replayMode;
}
