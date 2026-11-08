#include "jukeboxdlg.h"
#include "fredit.h"
#include "soundmanager.h"
#include "inputmanager.h"
#include "clientsetting.h"

extern CSoundManager* g_audio;

IMPLEMENT_OBJECT(FrJukeBoxDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrJukeBoxDlg, FrForm)

ON_FRESH_VI("jukeboxtitle", FRCMD_INIT, FrJukeBoxDlg::OnTitleInit)
ON_FRESH_VI("jukeboxplay", FRCMD_INIT, FrJukeBoxDlg::OnPlayInit)
ON_FRESH_VV("jukeboxclose", FRCMD_LBUTTONUP, FrJukeBoxDlg::OnCloseBtnUp)
ON_FRESH_VV("jukeboxplay", FRCMD_LBUTTONUP, FrJukeBoxDlg::OnPlayBtnUp)
ON_FRESH_VV("jukeboxprev", FRCMD_LBUTTONUP, FrJukeBoxDlg::OnPrevBtnUp)
ON_FRESH_VV("jukeboxnext", FRCMD_LBUTTONUP, FrJukeBoxDlg::OnNextBtnUp)
ON_FRESH_VV("jukeboxstop", FRCMD_LBUTTONUP, FrJukeBoxDlg::OnStopBtnUp)

END_FRESH_MSGMAP()

bool FrJukeBoxDlg::m_preserve_isPlaying = true;

int FrJukeBoxDlg::m_curBGM;

static const char* const s_bgmList[][2] = {
	{ "shiny.mp3",							 "shiny day"                      },
	{ "bunny.mp3",							 "bunny picnic"                   },
	{ "navy_blue.mp3",						 "navy blue memory"               },
	{ "rising_sun.mp3",						"rising sun"                     },
	{ "happy flight.mp3",                      "happy flight"                   },
	{ "crystal waver.mp3",                     "crystal waver"                  },
	{ "snowscape.mp3",						 "snowscape"                      },
	{ "winter_ride.mp3",                       "winter ride"                    },
	{ "somewhere.mp3",						 "somewhere"                      },
	{ "nowhere.mp3",						   "nowhere"                        },
	{ "volcano.mp3",						   "dive into volcano"              },
	{ "vermilion.mp3",						 "vermilion sunset"               },
	{ "samba3.mp3",							"breeze"                         },
	{ "daydream.mp3",						  "daydream"                       },
	{ "frog.mp3",							  "frog"						   },
	{ "spring.mp3",							"spring"                         },
	{ "data/Sound/season2/tea_time_piano.mp3", "tea time"                       },
	{ "crystal lake.mp3",                      "crystal lake"                   },
	{ "fade into white.mp3",                   "fade into white"                },
	{ "the mystery of the lost seaway.mp3",    "the mystery of the lost seaway" },
	{ "voyage the sky.mp3",                    "voyage the sky"                 },
	{ "zero fill love.mp3",                    "zero fill love"                 },
	{ "eastern_valley.mp3",                    "eastern valley"                 },
	{ "river.mp3",							 "river"                          },
};

void FrJukeBoxDlg::Init()

{
	m_curBGM = rand() % 24;
}

void FrJukeBoxDlg::Preserve()
{
	m_preserve_isPlaying = g_audio->IsPlaying(s_bgmList[m_curBGM][0]);
	g_audio->StopBGM(true);
}

void FrJukeBoxDlg::Restore()
{
	g_audio->StopBGM(true);
	if (m_preserve_isPlaying)
		g_audio->PlayBGM(s_bgmList[m_curBGM][0], false, true);
}

void FrJukeBoxDlg::PlayWithMap(unsigned char map)
{
	std::vector<int> list;

	switch (map)
	{
	case 4:
	case 5:
		list.push_back(0);
		list.push_back(1);
		break;
	case 7:
		list.push_back(2);
		list.push_back(3);
		break;
	case 8:
		list.push_back(4);
		list.push_back(5);
		break;
	case 9:
		list.push_back(6);
		list.push_back(7);
		break;
	case 10:
		list.push_back(8);
		list.push_back(9);
		break;
	case 13:
		list.push_back(10);
		list.push_back(11);
		break;
	case 14:
		list.push_back(17);
		list.push_back(18);
		break;
	case 15:
		list.push_back(19);
		list.push_back(20);
		break;
	case 16:
		list.push_back(22);
		list.push_back(23);
		break;
	default:

		list.push_back(12);
		list.push_back(13);
		list.push_back(14);
		list.push_back(15);
		break;
	}

	if (list.size())
	{
		g_audio->StopBGM(true);
		m_curBGM = list[rand() % list.size()];
		g_audio->PlayBGM(s_bgmList[m_curBGM][0], false, true);
	}
}

FrJukeBoxDlg::FrJukeBoxDlg()

{
	m_pTitle = NULL;
	m_pPlay = NULL;
}

FrJukeBoxDlg::~FrJukeBoxDlg()
{
}

bool FrJukeBoxDlg::OnInit()
{
	FrForm::OnInit();

	EnableDrag(false);
	return true;
}

void FrJukeBoxDlg::OnCloseBtnUp()
{
	Close(true);
}

void FrJukeBoxDlg::OnTitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrEdit, param);
	OnChangeTitle();
}

void FrJukeBoxDlg::OnPlayInit(int param)
{
	m_pPlay = DYNAMIC_CAST(FrButton, param);
	OnChangePlayPauseButton();
}

void FrJukeBoxDlg::OnPlayBtnUp()
{
	bool bPlaying = g_audio->IsPlaying(s_bgmList[m_curBGM][0]);

	if (bPlaying)
		g_audio->PauseBGM(false);
	else
		g_audio->PlayBGM(s_bgmList[m_curBGM][0], false, true);

	OnChangePlayPauseButton();

	AfxGetTask()->GetActor("AvatarChat")
		<< MsgObject(NULL, 105, !bPlaying, 0, 0, 0, 0);
}

void FrJukeBoxDlg::OnStopBtnUp()
{
	g_audio->StopBGM(true);

	OnChangePlayPauseButton();

	AfxGetTask()->GetActor("AvatarChat") << MsgObject(NULL, 105, 0, 0, 0, 0, 0);
}

void FrJukeBoxDlg::OnPrevBtnUp()
{
	bool bPlaying = g_audio->IsPlaying(s_bgmList[m_curBGM][0]);

	m_curBGM--;
	if (m_curBGM < 0)
		m_curBGM = sizeof(s_bgmList) / sizeof(s_bgmList[0]) - 1;

	OnChangeTitle();

	if (bPlaying)
	{
		g_audio->StopBGM(true);
		g_audio->PlayBGM(s_bgmList[m_curBGM][0], false, true);
	}
}

void FrJukeBoxDlg::OnNextBtnUp()
{
	bool bPlaying = g_audio->IsPlaying(s_bgmList[m_curBGM][0]);

	m_curBGM++;
	if (m_curBGM > sizeof(s_bgmList) / sizeof(s_bgmList[0]) - 1)
		m_curBGM = 0;

	OnChangeTitle();

	if (bPlaying)
	{
		g_audio->StopBGM(true);
		g_audio->PlayBGM(s_bgmList[m_curBGM][0], false, true);
	}
}

inline void FrJukeBoxDlg::OnChangeTitle()
{
	if (m_pTitle)
	{
		m_pTitle->SetLine(1, s_bgmList[m_curBGM][1], 0xffffffff, false, false);
	}
}

void FrJukeBoxDlg::OnChangePlayPauseButton()
{
	bool bPlaying = g_audio->IsPlaying(s_bgmList[m_curBGM][0]);

	if (m_pPlay)
	{
		const char* name;
		if (bPlaying)
			name = "btn_pause";
		else
			name = "btn_play";

		m_pPlay->SetButtonImg(MakeStr("%s_n", name), FrButton::NORMAL);
		m_pPlay->SetButtonImg(MakeStr("%s_o", name), FrButton::OVER);
		m_pPlay->SetButtonImg(MakeStr("%s_o", name), FrButton::PRESSED);
	}
}

void FrJukeBoxDlg::OnProc(const float delta)
{
	if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true))
	{
		OnPlayBtnUp();
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
	}

	if (g_input->GetDown("\xc1\xc2", true))
	{
		OnPrevBtnUp();
	}

	if (g_input->GetDown("\xbf\xec", true))
	{
		OnNextBtnUp();
	}

	if (g_input->Get("\xbb\xf3", true))
	{
		COption::Instance()->aSetBgmVolume(Between(0.0f,
			delta * 0.5f + COption::Instance()->aGetBgmVolume(), 1.0f));
		g_audio->SetBGMVolume(COption::Instance()->aGetBgmVolume());
	}

	if (g_input->Get("\xc7\xcf", true))
	{
		COption::Instance()->aSetBgmVolume(Between(0.0f,
			COption::Instance()->aGetBgmVolume() - delta * 0.5f, 1.0f));
		g_audio->SetBGMVolume(COption::Instance()->aGetBgmVolume());
	}
}
