#include "shufflebonus.h"
#include "projectg.h"
#include "golfdoc.h"
#include "chatmsg.h"
#include "woverlay.h"
#include "../../shared/localize.h"

CShuffleBonus::CShuffleBonus()
{
}

CShuffleBonus::~CShuffleBonus()
{
}

void CShuffleBonus::Render(WOverlay* pOverlay, float alpha)
{
	if (!pOverlay)
		return;

	if (_Validity(VAILDTYPE_0))
	{
		pOverlay->Render(g_view, WRect(0, 0, 1.0f, 1.0f),
			WRect(g_view->GetWidth() * 0.5f - 127.0f, 160, 264, 36), 0,
			((int)alpha << 24) | 0xffffff, 0, 0);
	}
}

void CShuffleBonus::ShowNoticeMsg()
{
	if (_Validity(VAILDTYPE_0))
	{
		CChatMsg::Instance()->AddChatMsg(
			"(\xbe\xcb\xb8\xb2) \xb0\xd4\xc0\xd3 \xbf\xcf\xc1\xd6\xbd\xc3 Shuffle Bonus\xb7\xce "
			"\xc6\xce\xc0\xbb 10% \xc3\xdf\xb0\xa1 \xc1\xf6\xb1\xde \xb9\xde\xbd\xc0\xb4\xcf\xb4\xd9.",
			0xffff7878, true, false);
	}
}

bool CShuffleBonus::_Validity(eVaildType type)
{
	if (!OnlinePlay())
		return false;

	unsigned char order = Doc()->m_holeOrder[0];
	bool bValid = false;
	if (type == VAILDTYPE_0)
	{
		if (order != GOLFDOC()->m_currentHole)
			return false;
	}

	if (IsLocalContent(S3_SHUFFLE_BONUS))
	{
		switch (Doc()->m_golfGame.gameType)
		{
		case 0:
		case 1:
		case 4:
		case 5:
		case 6:
			if (Doc()->m_holeType == 3)
			{
				bValid = true;
			}
			break;
		}
	}

	return bValid;
}
