#include "stdafx.h"
#include "helpmsg.h"
#include "clientsetting.h"
#include "golfdoc.h"
#include "golfball.h"
#include "club.h"
#include "mousecursor.h"
#include "inputmanager.h"
#include "minatl.h"

CHelpMsg::CHelpMsg()
	: m_pOverlay(NULL),
	  m_helpId(-1),
	  m_itemTypeId(0),
	  m_time(0),
	  m_bItemChanged(false),
	  m_pReplayTape(NULL)
{
	float barY = COption::Instance()->gGetBar_Y();

	m_pOverlay = g_resrcmng->GetOverlay("[help.jpg", 0);
	m_pFont = CChatMsg::Instance()->GetMaskedFont();
	m_srcRect[0] = WRect(0, 0, 1.0f, 0.25f);
	m_srcRect[1] = WRect(0, 0.25f, 1.0f, 0.5f);
	m_srcRect[2] = WRect(0, 0.75f, 1.0f, 0.25f);
	m_dstRect[0] = WRect(g_view->GetWidth() - 168, 0, 256, 8);
	m_dstRect[1] = WRect(g_view->GetWidth() - 168, 0, 256, 0);
	m_dstRect[2] = WRect(g_view->GetWidth() - 168, barY - 110, 256, 8);

	// clang-format off
	char* messages[29][3] = {
		{
			"\xc6\xce\xbe\xdf \xc4\xde\xba\xb8\xb0\xd4\xc0\xcc\xc1\xf6",
			"\xc6\xce\xbe\xdf \xc4\xde\xba\xb8 \xb0\xd4\xc0\xcc\xc1\xf6\xb8\xa6 \xb8\xf0\xb5\xce \xc3\xa4\xbf\xec\xb8\xe9 \xc6\xc4\xbf\xf6\xbc\xa6\xc0\xbb \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\012/v",
			NULL
		},
		{
			"\xc6\xc4\xbf\xf6\xbc\xa6",
			"\xc6\xce\xbe\xdf \xc4\xde\xba\xb8 \xb0\xd4\xc0\xcc\xc1\xf6\xb0\xa1 \xb8\xf0\xc0\xda\xb6\xf3\xbc\xad \xc6\xc4\xbf\xf6\xbc\xa6\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee \xc1\xa4\xba\xb8",
			"\xc7\xf6\xc0\xe7 \xc2\xfc\xb0\xa1\xc0\xda\xc0\xc7 \xc5\xb8\xbc\xf6\xb3\xaa \xb0\xb3\xc0\xce \xc1\xa4\xba\xb8\xb8\xa6 \xba\xbc \xbc\xf6 \xc0\xd6\xb4\xc2 \xb0\xf7 \xc0\xd4\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xc8\xa6\xc4\xc5\xc1\xf6\xc1\xa1 \xc1\xa4\xba\xb8",
			"\xc8\xa6\xc4\xc5 \xc1\xf6\xc1\xa1\xc0\xc7 \xc0\xa7\xc4\xa1\xb8\xa6 \xb0\xc5\xb8\xae\xbf\xcd \xb3\xf4\xc0\xcc \xc1\xc2\xc7\xa5\xb7\xce \xbe\xcb\xb7\xc1 \xc1\xdd\xb4\xcf\xb4\xd9.\012/v",
			NULL
		},
		{
			"\xc5\xac\xb7\xb4 \xbc\xb1\xc5\xc3 /k",
			"\xb8\xb6\xbf\xec\xbd\xba \xc8\xd9\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xd8 \xc0\xfb\xc0\xfd\xc7\xd1 \xba\xf1\xb0\xc5\xb8\xae\xc0\xc7 \xc5\xac\xb7\xb4\xc0\xbb \xbc\xb1\xc5\xc3 \xc7\xcf\xbd\xca\xbd\xc3\xbf\xe4. /v",
			"(PgUp, PgDn)"
		},
		{
			"\xc3\xa4\xc6\xc3 \xc3\xa2 /k",
			"\xc3\xa4\xc6\xc3\xc3\xa2\xc0\xbb \xbf\xad\xb0\xed \xb4\xdd\xbd\xc0\xb4\xcf\xb4\xd9.",
			"(ENTER)"
		},
		{
			"\xbe\xc6\xc0\xcc\xc5\xdb \xc3\xa2 /k",
			"\xbe\xc6\xc0\xcc\xc5\xdb\xc3\xa2\xc0\xbb \xbf\xad\xb0\xed \xb4\xdd\xbd\xc0\xb4\xcf\xb4\xd9.",
			"(I)"
		},
		{
			"\xbe\xc6\xc0\xcc\xc5\xdb \xc3\xa2 /k",
			"\xb0\xd4\xc0\xd3 \xc1\xdf\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbe\xb5 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc7\xd1\xc5\xb8\xbc\xf6\xbf\xa1 1\xc8\xb8 \xb9\xdb\xbf\xa1 \xb8\xf8\xbe\xb2\xb4\xcf \xbd\xc5\xc1\xdf\xc8\xf7 \xbb\xe7\xbf\xeb\xc7\xcf\xbc\xbc\xbf\xe4.",
			"(I)"
		},
		{
			"\xc5\xb8\xb1\xb8 \xc1\xa6\xc7\xd1 \xbd\xc3\xb0\xe8 /v",
			"\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3\xbe\xc8\xbf\xa1 \xbc\xa6\xc0\xbb \xc7\xcf\xc1\xf6 \xb8\xf8\xc7\xcf\xb8\xe9 \xbd\xc3\xb0\xe8\xb0\xa1 \xbf\xef\xb8\xae\xb8\xe9\xbc\xad \xb9\xfa\xc1\xa1\xc0\xbb \xb9\xde\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xc7\xb3\xc7\xe2 \xc7\xb3\xbc\xd3 \xc7\xa5\xbd\xc3\xb1\xe2",
			"\xb9\xd9\xb6\xf7\xc0\xc7 \xbc\xbc\xb1\xe2\xbf\xcd \xb9\xe6\xc7\xe2\xc0\xbb \xc7\xa5\xbd\xc3\xc7\xd8 \xc1\xdd\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			NULL,
			"Bunker\xbf\xa1 \xba\xfc\xc1\xf6\xb8\xe9 \xc8\xc4\xc5\xf0 \xc7\xcf\xb4\xf5\xb6\xf3\xb5\xb5 \xc0\xcf\xb4\xdc\xc0\xba \xb0\xa1\xb1\xee\xbf\xee Fairway\xb7\xce \xb0\xf8\xc0\xbb \xc4\xa1\xbc\xbc\xbf\xe4.",
			NULL
		},
		{
			"\xc5\xb8\xb1\xb8 \xb9\xd9 /k",
			"\xbc\xa6\xc0\xbb \xc7\xcf\xb7\xc1\xb8\xe9 \xc0\xcc\xb0\xf7\xc0\xbb \xc5\xac\xb8\xaf \xc7\xcf\xbc\xbc\xbf\xe4.",
			"(SPACE)"
		},
		{
			"\xc5\xb8\xc1\xa1 \xb0\xe1\xc1\xa4 /k",
			"\xb8\xb6\xbf\xec\xbd\xba \xb5\xe5\xb7\xa1\xb1\xd7\xb7\xce \xb0\xf8\xc0\xc7 \xc5\xb8\xc1\xa1\xc0\xbb \xb0\xe1\xc1\xa4\xc7\xcf\xbf\xa9 \xbd\xba\xc7\xc9\xc0\xbb \xb0\xa1\xb4\xc9\xc7\xcf\xb0\xd4 \xc7\xd5\xb4\xcf\xb4\xd9.",
			"(CTRL+\xb9\xe6\xc7\xe2\xc5\xb0)"
		},
		{
			"\xba\xbc\xc4\xc1\xb5\xf0\xbc\xc7\xb0\xfa \xc1\xf6\xb8\xe9 \xbb\xf3\xc8\xb2",
			"\xc1\xf6\xb8\xe9 \xc6\xaf\xbc\xba\xb0\xfa \xb1\xe2\xbf\xef\xb1\xe2\xbf\xa1 \xb5\xfb\xb6\xf3 \xb0\xf8\xc0\xc7 \xc8\xfb \xc0\xfc\xb4\xde \xbb\xf3\xc5\xc2\xb0\xa1 \xb4\xde\xb6\xf3 \xc1\xfd\xb4\xcf\xb4\xd9. /v\012/c0xFF007A39/cFairway : 90~100%\012/c0xFF063100/cRough : 85~95%\012/c0xFFD38600/cBunker : 55~85%",
			NULL
		},
		{
			NULL,
			"/c0xFFFF0000/cAlt /c0xFF000000/c\xc5\xb0\xb7\xce \xc4\xb3\xb8\xaf\xc5\xcd\xb8\xa6 \xc5\xf5\xb8\xed\xc7\xcf\xb0\xd4 \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xc0\xcc\xb8\xf0\xc6\xbc\xc4\xdc \xb1\xd7\xb7\xec",
			"/v",
			NULL
		},
		{
			"\xc5\xac\xb7\xb4\xbb\xe7\xbf\xeb\xc1\xa6\xc7\xd1",
			"\xb4\xf5 \xc0\xcc\xbb\xf3\xc0\xc7 \xc5\xac\xb7\xb4\xc0\xba \xc7\xf6\xc0\xa7\xc4\xa1\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xbe\xc6\xc0\xcc\xc5\xdb\xbb\xe7\xbf\xeb\xc1\xa6\xc7\xd1",
			"\xbc\xb1\xc5\xc3\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc7\xf6\xc0\xe7 \xbb\xf3\xc8\xb2\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xc5\xac\xb7\xb4\xbb\xe7\xbf\xeb\xc1\xa6\xc7\xd1",
			"\xc6\xc4\xbf\xf6\xbc\xa6 \xbb\xf3\xc5\xc2\xbf\xa1\xbc\xad\xb4\xc2 \xc6\xdb\xc5\xcd\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xb0\xf8 \xbf\xc5\xb1\xe2\xb1\xe2 \xb8\xf0\xb5\xe5",
			"\xb0\xcb\xc0\xba \xbf\xf8\xc0\xbb \xc1\xa6\xbf\xdc\xc7\xd1 \xc6\xe4\xbe\xee\xbf\xfe\xc0\xcc \xba\xce\xba\xd0\xbf\xa1 Ctrl+\xb8\xb6\xbf\xec\xbd\xba \xbf\xde\xc2\xca \xc5\xac\xb8\xaf\xc0\xbb \xc7\xcf\xbf\xa9 \xb0\xf8\xc0\xbb \xbf\xc5\xb1\xe6 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xb0\xf8 \xc0\xcc\xb5\xbf \xba\xd2\xb0\xa1 \xc1\xf6\xbf\xaa",
			"\xc0\xcc\xb0\xf7\xbf\xa3 \xb0\xf8\xc0\xbb \xbf\xc5\xb1\xe6 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xcc\xb0\xf7\xc0\xbb \xc1\xa6\xbf\xdc\xc7\xd1 \xc6\xe4\xbe\xee\xbf\xfe\xc0\xcc\xbf\xa1 \xb0\xf8\xc0\xbb \xbf\xc5\xb0\xdc \xc1\xd6\xbd\xca\xbd\xc3\xbf\xe4.",
			NULL
		},
		{
			"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc /k",
			"\xbc\xa6\xc0\xbb \xc7\xcf\xbf\xa9 \xb3\xaf\xbe\xc6\xb0\xa3 \xb0\xf8\xc0\xcc \xbf\xcf\xc0\xfc\xc8\xf7 \xb8\xd8\xc3\xe1 \xc8\xc4 \xb6\xc7\xb4\xc2 \xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc \xb5\xb5\xc1\xdf, R\xc5\xb0\xb8\xa6 \xb4\xad\xb7\xaf \xbc\xa6\xc0\xbb \xb4\xd9\xbd\xc3 \xba\xbc \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\012/v",
			"(R)"
		},
		{
			"\xb1\xe2\xb4\xc9\xbb\xe7\xbf\xeb\xba\xd2\xb0\xa1",
			"\xb7\xa1\xb4\xf5\xc0\xfc\xbf\xa1\xbc\xad\xb4\xc2 \xbb\xf3\xb4\xeb \xc5\xcf\xc0\xcf\xb6\xa7 \xc5\xbe\xba\xe4\xb8\xa6 \xba\xbc \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xb1\xe2\xb4\xc9\xbb\xe7\xbf\xeb\xba\xd2\xb0\xa1",
			"\xc1\xf6\xb1\xdd\xc0\xba \xb0\xf8 \xbf\xc5\xb1\xe2\xb1\xe2\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xb1\xe2\xb4\xc9\xbb\xe7\xbf\xeb\xba\xd2\xb0\xa1",
			"\xc1\xf6\xb1\xdd\xc0\xba \xc0\xcf\xbd\xc3\xc1\xa4\xc1\xf6\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xb1\xe2\xb4\xc9\xbb\xe7\xbf\xeb\xba\xd2\xb0\xa1",
			"\xc1\xf6\xb1\xdd\xc0\xba \xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xc6\xc4\xbf\xf6\xbc\xa6",
			"[\xc0\xd3\xbd\xc3\xb8\xde\xbc\xbc\xc1\xf6] \xc3\xbc\xb7\xc2\xba\xb8\xc1\xb6\xc1\xa6 \xbb\xe7\xbf\xeb\xc8\xc4 \xc6\xc4\xbf\xf6\xbc\xa6""2 \xba\xd2\xb0\xa1~",
			NULL
		},
		{
			"\xbe\xc6\xc0\xcc\xc5\xdb\xbb\xe7\xbf\xeb\xc1\xa6\xc7\xd1",
			"\xbf\xc0\xc5\xe4\xc4\xb6\xb8\xae\xc6\xdb\xbd\xba\xb8\xa6 \xbb\xe7\xbf\xeb\xc1\xdf\xc0\xcc\xb9\xc7\xb7\xce \xc1\xdf\xba\xb9 \xbb\xe7\xbf\xeb \xba\xd2\xb0\xa1\xb4\xc9 \xc7\xd5\xb4\xcf\xb4\xd9.",
			NULL
		},
		{
			"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc (\xc8\xa6) /k",
			"\xc5\xb0\xba\xb8\xb5\xe5 'R'\xc0\xbb \xb4\xa9\xb8\xa3\xb8\xe9 \xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc \xc5\xd7\xc0\xd9 1\xb0\xb3\xb8\xa6 \xbc\xd2\xba\xf1\xc7\xcf\xbf\xa9, \xc7\xf6\xc0\xe7 \xc8\xa6\xc0\xc7 \xc7\xc3\xb7\xb9\xc0\xcc \xb3\xbb\xbf\xeb\xc0\xbb \xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc\xb7\xce \xc0\xfa\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\012/v",
			"(R)"
		}
	};
	// clang-format on

	m_helpMsg = new char*[29][3];
	for (int i = 0; i < 29; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (messages[i][j])
			{
				m_helpMsg[i][j] = new char[strlen(messages[i][j]) + 1];
				strcpy(m_helpMsg[i][j], messages[i][j]);
			}
			else
			{
				m_helpMsg[i][j] = NULL;
			}
		}
	}

	memset(m_title, 0, sizeof(m_title));
	memset(m_msg, 0, sizeof(m_msg));
	memset(m_key, 0, sizeof(m_key));
	memset(m_param, 0, sizeof(m_param));
	CheckReplayAvailable();
}

CHelpMsg::~CHelpMsg()
{
	for (int i = 0; i < 29; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (m_helpMsg[i][j])
			{
				delete[] m_helpMsg[i][j];
				m_helpMsg[i][j] = NULL;
			}
		}
	}

	if (m_helpMsg)
	{
		delete[] m_helpMsg;
		m_helpMsg = NULL;
	}

	if (g_resrcmng && m_pOverlay)
	{
		g_resrcmng->Release(m_pOverlay);
		m_pOverlay = NULL;
	}
}

void CHelpMsg::ResetUiPosition()
{
	float barY = COption::Instance()->gGetBar_Y();
	m_dstRect[0].x = g_view->GetWidth() - 168;
	m_dstRect[1].x = g_view->GetWidth() - 168;
	m_dstRect[2].x = g_view->GetWidth() - 168;
	m_dstRect[2].y = barY - 110;
	if (m_helpId != -1 || m_itemTypeId)
	{
		CalcWindowSize(m_title, m_msg);
	}
}

void CHelpMsg::Process(float delta)
{
	char buffer[2][32];

	if (IsLocalContent((localContentType_t)79) && Doc()->GetPlayingMode() == 1)
	{
		if (Doc()->GetReplayPlayType() == 4 && IsMyTurn(false))
		{
			if (Doc()->m_gameMode != 5 && Doc()->m_gameMode == 0 &&
				GOLFDOC()->m_tutorialMode == 15 &&
				Doc()->m_golfGame.gameType != 10 &&
				Doc()->m_roomInfo.realGameType != 14)
			{
				if (GetCurMap() != 7)
					SetHelpMsg(28, NULL);
				return;
			}
		}
	}
	else
	{
		if (GOLFDOC()->m_gameMode & 0xc0)
		{
			if (GOLFDOC()->m_tutorialMode == 15 && Doc()->m_gameMode == 0 &&
				Doc()->m_golfGame.gameType != 10 && IsMyTurn(false) &&
				m_pReplayTape && m_helpId == -1 && !m_itemTypeId &&
				(GOLFDOC()->m_gameMode & 0x80) &&
				(GolfBall().m_state == 0 || GOLFDOC()->m_bReplay))
			{
				if (!IsLocalContent((localContentType_t)79))
				{
					sprintf(buffer[0],
						"(%d\xb9\xf8 \xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9)",
						m_pReplayTape->Common[0]);
					SetHelpMsg(21, buffer[0]);
				}
				return;
			}
			else if (m_helpId != 21)
			{
				m_helpId = -1;
			}
		}
		else if (m_helpId == 21)
		{
			m_helpId = -1;
		}
	}

	if (!CMouseCursor::Instance()->IsActive())
		return;

	if (m_helpId == 17 || m_helpId == 16 || m_helpId == 1 || m_helpId == 18 ||
		m_helpId == 20 || m_helpId == 22 || m_helpId == 23 || m_helpId == 24 ||
		m_helpId == 27)
	{
		if (m_time > 2)
			m_helpId = -1;
		else
		{
			m_time += delta;
			return;
		}
	}

	m_param[0] = 0;
	char param[32];
	param[0] = 0;
	int id = 29;
	int area = CMouseCursor::Instance()->GetArea();
	switch (area)
	{
	case 6:
		id = 6;
		break;
	case 18:
		id = 7;
		break;
	case 1:
	case 2:
		id = 4;
		sprintf(m_param, "(%.0fy\xbf\xeb %s)", GolfClub().GetRange(),
			GolfClub().GetName());
		break;
	case 7:
		id = 5;
		break;
	case 5:
		id = 11;
		break;
	case 14:
	case 15:
	case 16:
	case 17:
	{
		id = 0;
		if (Doc()->m_golfGame.gameType == 1)
			sprintf(m_param,
				"(\xc6\xc4\xbf\xf6\xbc\xa6\xc0\xbb %d\xb9\xf8 \xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9)",
				float2int(TEAM(area - 14)->gauge / 33.0f));
		else
			sprintf(m_param,
				"(\xc6\xc4\xbf\xf6\xbc\xa6\xc0\xbb %d\xb9\xf8 \xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9)",
				float2int(PLAYER(area - 14)->gauge / 33.0f));
	}
	break;
	case 10:
	case 11:
	case 12:
	case 13:
		id = 2;
		break;
	case 19:
		id = 9;
		break;
	case 20:
	{
		id = 8;
		unsigned long remain = GOLFDOC()->m_remainTime / 1000;
		if (GOLFDOC()->m_remainTime > remain * 1000)
			++remain;
		sprintf(m_param, "(\xc7\xf6\xc0\xe7 %d\xc3\xca)", remain);
	}
	break;
	case 21:
	{
		WVector diff = GetHoleCupPos() - GolfBall().m_pos;
		float height = diff.y * 0.3125f;
		diff.y = 0;
		height *= 0.9144f;
		float distance = diff.Magnitude();
		distance *= 0.3125f;
		float absoluteHeight = Abs(height);
		if (absoluteHeight < 0.1f)
			sprintf(param, "(\xb3\xf4\xc0\xcc:0.0m, ");
		else if (absoluteHeight > 10)
			sprintf(param, "(\xb3\xf4\xc0\xcc:%+.0fm, ", height);
		else
			sprintf(param, "(\xb3\xf4\xc0\xcc:%+.1fm, ", height);

		if (distance < 1)
			sprintf(buffer[1], "\xb0\xc5\xb8\xae:%.1fy)", distance);
		else
			sprintf(buffer[1], "\xb0\xc5\xb8\xae:%.0fy)", distance);
		strcat(param, buffer[1]);
		id = 3;
	}
	break;
	case 3:
		if (!g_input->GetButton(LEFT_BUTTON))
		{
			float x, y;
			AfxGetTask()->GetActor("Spin")
				<< MsgObject(NULL, 358, (int)&x, (int)&y, 0, 0, 0);
			x -= g_input->GetMousePoint().x;
			y -= g_input->GetMousePoint().y;
			if (GolfClub().GetType() != 3 && x * x + y * y < 25)
				id = 12;
			else
			{
				id = 13;
				float percent;
				AfxGetTask()->GetActor("Screen")
					<< MsgObject(NULL, 450, (int)&percent, 0, 0, 0, 0);
				sprintf(param, "(\xc7\xf6\xc0\xe7 %.0f%%)", percent * 100);
			}
		}
		break;
	}

	if (area >= 51 && area <= 62)
		id = 15;

	static int previous = 29;
	if (m_itemTypeId)
	{
		if (id == 29)
		{
			previous = id;
			id = 7;
		}
		else
		{
			if (id != previous)
			{
				previous = id;
				m_bItemChanged = false;
			}
			if (m_bItemChanged)
			{
				id = 7;
			}
		}
	}

	if (GOLFDOC()->m_bDropRequest)
		id = 19;
	if (m_helpId == 21)
		id = 21;

	if (id < 29)
	{
		if (id != 15)
			SetHelpMsg(id, param[0] ? param : NULL);
	}
	else
	{
		m_helpId = -1;
	}
}

void CHelpMsg::CalcWindowSize(char* title, char* msg)
{
	if (title)
		m_dstRect[1].h = 20.0f;
	else
		m_dstRect[1].h = 0;

	int offset = 0, begin = 0, end = 0;
	int result = -1;
	for (;;)
	{
		result = GetLine(&begin, &end, NULL, msg + offset,
			result == 5 || result == 6);
		offset += end;
		if (result == 0)
			break;
		if (result != 1 && result != 4 && result != 2 && result != 3)
		{
			if (result == 6)
				++offset;
			m_dstRect[1].h += 15;
		}
	}

	m_dstRect[1].h += 11;
	m_dstRect[1].y = m_dstRect[2].y - m_dstRect[1].h;
	m_dstRect[0].y = m_dstRect[1].y - 8;
}

int CHelpMsg::GetLine(int* pBegin, int* pEnd, unsigned long* pColor, char* str,
	bool bSkipSpace)
{
	static int lineWidth = 0;

	int begin = 0;
	if (bSkipSpace)
		while (str[begin] == ' ' || str[begin] == '\t')
			++begin;

	int result = 0;
	if (str[begin] == '/')
	{
		switch (str[begin + 1])
		{
		case 'c':
			if (pColor)
			{
				begin += 4;
				*pColor = 0;
				for (int i = 28; i >= 0; i -= 4)
				{
					int value;
					if (str[begin] >= '0' && str[begin] <= '9')
						value = str[begin] - '0';
					else
						value = str[begin] - 'A' + 10;
					++begin;
					*pColor |= value << i;
				}
				begin += 2;
			}
			else
			{
				begin += 14;
			}
			result = 1;
			break;
		case 'v':
			begin += 2;
			result = 2;
			break;
		case 'k':
			begin += 2;
			result = 3;
			break;
		}
	}

	if (result)
	{
		*pBegin = *pEnd = begin;
	}
	else
	{
		bool foundSpace = false;
		int space;
		int end = begin;
		while (str[end])
		{
			if (str[end] == '\n')
				break;
			if (str[end] == '/' &&
				(str[end + 1] == 'c' || str[end + 1] == 'v' ||
					str[end + 1] == 'k'))
				break;
			if (str[end] == ' ')
			{
				space = end;
				foundSpace = true;
			}
			end += Custom_IsLeadByte(str[end]) ? 2 : 1;
			if (lineWidth + end - begin > 25)
				break;
		}
		lineWidth += end - begin;
		*pBegin = begin;
		*pEnd = end;
		if (!str[end])
		{
			lineWidth = 0;
			result = 0;
		}
		else if (str[end] == '/' &&
			(str[end + 1] == 'c' || str[end + 1] == 'v' || str[end + 1] == 'k'))
		{
			result = 4;
		}
		else if (str[end] != '\n')
		{
			if (foundSpace && lineWidth + end - begin > 25)
				*pEnd = space;
			lineWidth = 0;
			result = 5;
		}
		else
		{
			lineWidth = 0;
			result = 6;
		}
	}

	return result;
}

void CHelpMsg::Display()
{
	if (m_helpId < 0)
		return;

	float width = 0;

	m_pOverlay->Render(g_view, m_srcRect[0], m_dstRect[0], 0, -1, 0, 0);
	m_pOverlay->Render(g_view, m_srcRect[1], m_dstRect[1], 0, -1, 0, 0);
	m_pOverlay->Render(g_view, m_srcRect[2], m_dstRect[2], 0, -1, 0, 0);

	float y = m_dstRect[0].y + 8;
	float x;
	char saved;

	if (m_title[0])
	{
		char* marker = strstr(m_title, "/v");
		if (marker)
		{
			saved = *marker;
			*marker = 0;
			x = g_view->GetWidth() - 159;
			x += m_pFont->Print(g_view, x, y, m_title, 0, 0xffff0000, 0);
			*marker = saved;
			m_pFont->Print(g_view, x, y, m_param, 0, 0xffff0000, 0);
		}
		else
		{
			marker = strstr(m_title, "/k");
			if (marker)
			{
				saved = *marker;
				*marker = 0;
				x = g_view->GetWidth() - 159;
				x += m_pFont->Print(g_view, x, y, m_title, 0, 0xffff0000, 0);
				*marker = saved;
				m_pFont->Print(g_view, x, y, m_key, 0, 0xffff0000, 0);
			}
			else
			{
				m_pFont->Print(g_view, g_view->GetWidth() - 159, y, m_title, 0,
					0xffff0000, 0);
			}
		}
		y += 20;
	}

	int offset = 0, end = 0, begin = 0, result = -1;
	x = g_view->GetWidth() - 156;
	WView* view;
	unsigned long color = 0xff000000;
	for (;;)
	{
		result = GetLine(&begin, &end, &color, m_msg + offset,
			result == 5 || result == 6);
		begin += offset;
		offset += end;
		if (result == 1)
			continue;
		if (result == 2)
		{
			width = m_pFont->Print(g_view, x, y, m_param, 0, 0xffff0000, 0);
			x += width;
		}
		else if (result == 3)
		{
			width = m_pFont->Print(g_view, x, y, m_key, 0, 0xffff0000, 0);
			x += width;
		}
		else
		{
			if (offset > begin)
			{
				saved = m_msg[offset];
				m_msg[offset] = 0;
				width =
					m_pFont->Print(g_view, x, y, m_msg + begin, 0, color, 0);
				m_msg[offset] = saved;
			}
			if (result == 4)
			{
				x += width;
			}
			else
			{
				view = *const_cast<WView* volatile*>(&g_view);
				x = view->GetWidth() - 156;
				y += 15;
				if (result == 6)
					++offset;
				else if (result == 0)
					break;
			}
		}
	}

	m_pFont->Flush(view);
}

void CHelpMsg::SetItemHelpMsg(unsigned long typeId)
{
	if (typeId != m_itemTypeId && typeId)
		m_bItemChanged = true;
	m_itemTypeId = typeId;
}

void CHelpMsg::SetHelpMsg(int id, char* param)
{
	if (GOLFDOC()->m_tutorialMode < 15)
		return;

	if (id < 0)
	{
		m_helpId = -1;
		return;
	}

	if (id == 17 || id == 1 || id == 16 || id == 18 || id == 20 || id == 22 ||
		id == 23 || id == 24 || id == 27)
		m_time = 0;

	if (param)
		strcpy(m_param, param);

	if (id == 7 && m_itemTypeId && m_itemTypeId != 0x1800002d)
	{
		m_helpId = 7;
		IFF_STRUCT::sItem* item = ItemManager()->FindItem(m_itemTypeId);
		IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(m_itemTypeId);
		if (item)
		{
			strncpy(m_title, item->c.Name, 31);
			strcat(m_title, " /k");
		}
		if (desc)
			strncpy(m_msg, desc->Desc, 255);
		strncpy(m_key, "(\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9)", 16);
		CalcWindowSize(m_title, m_msg);
	}
	else if (id > -1 && m_helpId != id)
	{
		m_helpId = id;
		if (m_helpMsg[id][0])
			strncpy(m_title, m_helpMsg[id][0], 31);
		else
			m_title[0] = 0;
		strcpy(m_msg, m_helpMsg[m_helpId][1]);
		if (m_helpMsg[m_helpId][2])
			strncpy(m_key, m_helpMsg[m_helpId][2], 15);
		else
			m_key[0] = 0;
		CalcWindowSize(m_helpMsg[m_helpId][0], m_helpMsg[m_helpId][1]);
	}
}

void CHelpMsg::SetEmoticonHelpMsg(char** msg)
{
	m_helpId = 15;
	const char* title =
		"\xc0\xcc\xb8\xf0\xc6\xbc\xc4\xdc \xb8\xed\xb7\xc9\xbe\xee";
	strcpy(m_title, title);
	m_msg[0] = 0;
	strcat(m_msg, "/c0xFFFF0000/c/");
	strcat(m_msg, msg[0]);
	for (int i = 1; msg[i] && i <= 2; ++i)
	{
		strcat(m_msg, "/c0xFF000000/c, ");
		strcat(m_msg, "/c0xFFFF0000/c/");
		strcat(m_msg, msg[i]);
	}

	CalcWindowSize(m_title, m_msg);
}

void CHelpMsg::OnChangeTurn()
{
	m_helpId = -1;
	m_itemTypeId = 0;
}

void CHelpMsg::CheckReplayAvailable()
{
	const std::list<sItemInfo>& items = Doc()->m_myItemList;
	std::list<sItemInfo>::const_iterator it =
		std::find(items.begin(), items.end(), 0x1a00004f);
	if (it == items.end())
		m_pReplayTape = NULL;
	else
		m_pReplayTape = (sItemInfo*)&*it;
}
