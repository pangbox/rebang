#include "upgradecaddiedlg.h"
#include "fredit.h"
#include "actor.h"
IMPLEMENT_OBJECT(FrUpgradeCaddieDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrUpgradeCaddieDlg, FrForm)

ON_FRESH_VI("message", FRCMD_INIT, FrUpgradeCaddieDlg::OnMessageInit)
ON_FRESH_VV("Yes", FRCMD_LBUTTONUP, FrUpgradeCaddieDlg::OnYesBtnUp)
ON_FRESH_VV("No", FRCMD_LBUTTONUP, FrUpgradeCaddieDlg::OnNoBtnUp)

END_FRESH_MSGMAP()

FrUpgradeCaddieDlg::FrUpgradeCaddieDlg()
{
	m_pMessage = NULL;
	m_caddieId = 0;
	memset(m_stat, 0, sizeof(m_stat));
	m_oldPrice = m_newPrice = 0;
}

void FrUpgradeCaddieDlg::OnMessageInit(int param)
{
	m_pMessage = DYNAMIC_CAST(FrEdit, param);
}

void FrUpgradeCaddieDlg::OnYesBtnUp()
{
	WSendPacket packet((enumClientPacket)236);
	packet.Encode4(m_caddieId);
	packet.Send(TO_GAME);
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	Close(FrOK, true);
}

void FrUpgradeCaddieDlg::OnNoBtnUp()
{
	Close(FrCANCEL, true);
}

void FrUpgradeCaddieDlg::SetUpgradeInfo(unsigned long caddieId, int* stat,
	int oldPrice, int newPrice)
{
	m_caddieId = caddieId;
	memcpy(m_stat, stat, sizeof(m_stat));
	if (m_pMessage == NULL)
		return;

	m_pMessage->IsEnabled();

	m_pMessage->AddText(
		MakeStr(
			"\xc4\xb3\xb5\xf0 \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xc1\xf8\xc7\xe0\xc7\xd5\xb4\xcf\xb4\xd9.\n\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc \xc0\xcc\xc8\xc4 \xba\xaf\xb0\xe6\xbb\xe7\xc7\xd7\xc0\xba \xbe\xc6\xb7\xa1\xbf\xcd \xb0\xb0\xbd\xc0\xb4\xcf\xb4\xd9.\n\n\\c0xffff0000\\c\xc0\xe7\xb0\xed\xbf\xeb \xba\xf1\xbf\xeb : %s\n\\c0xff000000\\c\xc6\xc4\xbf\xf6 : %s\n\xc4\xc1\xc6\xae\xb7\xd1 : %s\n\xc1\xa4\xc8\xae\xb5\xb5 : %s\n\xbd\xba\xc7\xc9 : %s\n\xc4\xbf\xba\xea : %s\n",
			(newPrice - oldPrice == 0)
				? "\xc7\xd8\xb4\xe7\xbb\xe7\xc7\xd7 \xbe\xf8\xc0\xbd"
				: MakeStr("%d\xc6\xce -> %d\xc6\xce", oldPrice, newPrice),
			(m_stat[0] > 0) ? MakeStr("+%d", m_stat[0])
							: MakeStr("%d", m_stat[0]),
			(m_stat[1] > 0) ? MakeStr("+%d", m_stat[1])
							: MakeStr("%d", m_stat[1]),
			(m_stat[2] > 0) ? MakeStr("+%d", m_stat[2])
							: MakeStr("%d", m_stat[2]),
			(m_stat[3] > 0) ? MakeStr("+%d", m_stat[3])
							: MakeStr("%d", m_stat[3]),
			(m_stat[4] > 0) ? MakeStr("+%d", m_stat[4])
							: MakeStr("%d", m_stat[4])),
		false, true);
}
