#include "trainingoptiondlg.h"
#include "wind.h"
#include <math.h>

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrTrainingOptionDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrTrainingOptionDlg, FrForm)

ON_FRESH_VI("map", FRCMD_INIT, FrTrainingOptionDlg::OnMapInit)
ON_FRESH_VI("mapprev", FRCMD_INIT, FrTrainingOptionDlg::OnMapPrevBtnInit)
ON_FRESH_VV("mapprev", FRCMD_LBUTTONUP, FrTrainingOptionDlg::OnMapPrevBtnUp)
ON_FRESH_VI("mapnext", FRCMD_INIT, FrTrainingOptionDlg::OnMapNextBtnInit)
ON_FRESH_VV("mapnext", FRCMD_LBUTTONUP, FrTrainingOptionDlg::OnMapNextBtnUp)
ON_FRESH_VI("weather", FRCMD_INIT, FrTrainingOptionDlg::OnWeatherInit)
ON_FRESH_VI("weatherprev", FRCMD_INIT,
	FrTrainingOptionDlg::OnWeatherPrevBtnInit)
ON_FRESH_VV("weatherprev", FRCMD_LBUTTONUP,
	FrTrainingOptionDlg::OnWeatherPrevBtnUp)
ON_FRESH_VI("weathernext", FRCMD_INIT,
	FrTrainingOptionDlg::OnWeatherNextBtnInit)
ON_FRESH_VV("weathernext", FRCMD_LBUTTONUP,
	FrTrainingOptionDlg::OnWeatherNextBtnUp)
ON_FRESH_VI("direction", FRCMD_INIT, FrTrainingOptionDlg::OnDirectionInit)
ON_FRESH_VI("directionprev", FRCMD_INIT,
	FrTrainingOptionDlg::OnDirectionPrevBtnInit)
ON_FRESH_VV("directionprev", FRCMD_LBUTTONUP,
	FrTrainingOptionDlg::OnDirectionPrevBtnUp)
ON_FRESH_VI("directionnext", FRCMD_INIT,
	FrTrainingOptionDlg::OnDirectionNextBtnInit)
ON_FRESH_VV("directionnext", FRCMD_LBUTTONUP,
	FrTrainingOptionDlg::OnDirectionNextBtnUp)
ON_FRESH_VI("intensity", FRCMD_INIT, FrTrainingOptionDlg::OnIntensityInit)
ON_FRESH_VI("intensityprev", FRCMD_INIT,
	FrTrainingOptionDlg::OnIntensityPrevBtnInit)
ON_FRESH_VV("intensityprev", FRCMD_LBUTTONUP,
	FrTrainingOptionDlg::OnIntensityPrevBtnUp)
ON_FRESH_VI("intensitynext", FRCMD_INIT,
	FrTrainingOptionDlg::OnIntensityNextBtnInit)
ON_FRESH_VV("intensitynext", FRCMD_LBUTTONUP,
	FrTrainingOptionDlg::OnIntensityNextBtnUp)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrTrainingOptionDlg::OnOkBtnUp)

END_FRESH_MSGMAP()

FrTrainingOptionDlg::FrTrainingOptionDlg()
{
	m_bEnableChange = false;
	m_map = 0;
}

void FrTrainingOptionDlg::OnOkBtnUp()
{
	if (m_map != GetCurMap())
	{
		SetCurMap(m_map);
		GOLFDOC()->m_currentHole = 1;
		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(NULL, 10010, 0, 0, 0, 0, 0);
		AfxGetTask()->GetActor("GolfBg") << MsgObject(NULL, 320, 0, 0, 0, 0, 0);
		AfxGetTask()->GetActor("GolfBg") << MsgObject(NULL, 318, 0, 0, 0, 0, 0);
		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(NULL, 153, 0, 0, 0, 0, 0);
	}
	CSceneManager::Instance()->SetWeather(m_weather, false);
	OnFreshOkay();
}

void FrTrainingOptionDlg::OnMapInit(int param)
{
	m_pMap = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pMap)
	{
		unsigned int map = (m_map <= 127 || m_map == 253) ? m_map : m_map - 128;
		IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(map | 0x28000000);
		if (desc)
			m_pMap->SetLine(1, desc->Desc, 0, 0, 0);
		else
			m_pMap->SetLine(1, "", 0, 0, 0);
	}
}

void FrTrainingOptionDlg::OnMapPrevBtnInit(int param)
{
	m_pMapPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapPrev)
	{
		m_pMapPrev->SetPushDelay(0.0f);
		if (true)
			m_pMapPrev->Enable(false);
	}
}

void FrTrainingOptionDlg::OnMapPrevBtnUp()
{
	std::map<unsigned int, IFF_STRUCT::sCourse>& courses =
		ItemManager()->m_CourseMap;
	std::map<unsigned int, IFF_STRUCT::sCourse>::iterator it;
	for (it = courses.begin(); it != courses.end(); ++it)
	{
		unsigned int map = (m_map <= 127 || m_map == 253) ? m_map : m_map - 128;
		if ((*it).first == (map | 0x28000000))
		{
			it--;
			m_map = (unsigned char)(*it).first;
			if (it == courses.begin())
				m_pMapPrev->Enable(false);
			m_pMapNext->Enable(true);
			break;
		}
	}
	OnMapInit((int)m_pMap);
}

void FrTrainingOptionDlg::OnMapNextBtnInit(int param)
{
	m_pMapNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapNext)
	{
		m_pMapNext->SetPushDelay(0.0f);
		if (true)
			m_pMapNext->Enable(false);
	}
}

void FrTrainingOptionDlg::OnMapNextBtnUp()
{
	std::map<unsigned int, IFF_STRUCT::sCourse>& courses =
		ItemManager()->m_CourseMap;
	std::map<unsigned int, IFF_STRUCT::sCourse>::iterator it;
	for (it = courses.begin(); it != courses.end(); ++it)
	{
		unsigned int map = (m_map <= 127 || m_map == 253) ? m_map : m_map - 128;
		if ((*it).first == (map | 0x28000000))
		{
			it++;
			m_map = (unsigned char)(*it).first;
			it++;
			m_pMapPrev->Enable(true);
			if (it == courses.end())
				m_pMapNext->Enable(false);
			break;
		}
	}
	OnMapInit((int)m_pMap);
}

void FrTrainingOptionDlg::OnWeatherInit(int param)
{
	m_pWeather = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_weather = Doc()->m_golfGame.weather;
	if (m_pWeather)
	{
		switch (m_weather)
		{
		case 0:
			m_pWeather->AddLine("\xb8\xbc\xc0\xbd", 0, 0);
			break;
		case 2:
			m_pWeather->AddLine("\xba\xf1", 0, 0);
			break;
		case 3:
			m_pWeather->AddLine("\xb4\xab", 0, 0);
			break;
		}
	}
}

void FrTrainingOptionDlg::OnWeatherPrevBtnInit(int param)
{
	m_pWeatherPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pWeatherPrev)
	{
		m_pWeatherPrev->SetPushDelay(0.0f);
		if (m_bEnableChange)
		{
			if (m_weather == 0)
				m_pWeatherPrev->Enable(false);
		}
		else
			m_pWeatherPrev->Enable(false);
	}
}

void FrTrainingOptionDlg::OnWeatherPrevBtnUp()
{
	// HACK
	if (0)
		OnWeatherPrevBtnInit(0);
	m_weather -= 1;
	if (m_weather == 0)
	{
		if (m_pWeatherPrev)
			m_pWeatherPrev->Enable(false);
	}
	if (m_pWeatherNext)
		m_pWeatherNext->Enable(true);
	if (m_pWeather)
	{
		switch (m_weather)
		{
		case 0:
			m_pWeather->SetLine(1, "\xb8\xbc\xc0\xbd", 0, 0, 0);
			break;
		case 2:
			m_pWeather->SetLine(1, "\xba\xf1", 0, 0, 0);
			break;
		case 3:
			m_pWeather->SetLine(1, "\xb4\xab", 0, 0, 0);
			break;
		}
	}
}

void FrTrainingOptionDlg::OnWeatherNextBtnInit(int param)
{
	m_pWeatherNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pWeatherNext)
	{
		m_pWeatherNext->SetPushDelay(0.0f);
		if (m_bEnableChange)
		{
			if (m_weather == 2)
				m_pWeatherNext->Enable(false);
		}
		else
			m_pWeatherNext->Enable(false);
	}
}

void FrTrainingOptionDlg::OnWeatherNextBtnUp()
{
	// HACK
	if (0)
		OnWeatherNextBtnInit(0);
	m_weather += 1;
	if (m_weather == 2)
	{
		if (m_pWeatherNext)
			m_pWeatherNext->Enable(false);
	}
	if (m_pWeatherPrev)
		m_pWeatherPrev->Enable(true);
	if (m_pWeather)
	{
		switch (m_weather)
		{
		case 0:
			m_pWeather->SetLine(1, "\xb8\xbc\xc0\xbd", 0, 0, 0);
			break;
		case 2:
			m_pWeather->SetLine(1, "\xba\xf1", 0, 0, 0);
			break;
		case 3:
			m_pWeather->SetLine(1, "\xb4\xab", 0, 0, 0);
			break;
		}
	}
}

void FrTrainingOptionDlg::OnDirectionInit(int param)
{
	m_pDirection = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_direction =
		(int)(floor((float)(Wind().GetGlobalDirection() * g_RADTODEG) * 0.1f) *
			10.0f);
	if (m_pDirection)
	{
		m_pDirection->AddLine(MakeStr("%d\xb5\xb5", m_direction), 0, 0);
	}
}

void FrTrainingOptionDlg::OnDirectionPrevBtnInit(int param)
{
	m_pDirectionPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pDirectionPrev)
	{
		m_pDirectionPrev->SetPushDelay(0.0f);
		if (m_bEnableChange)
		{
			if (m_direction == 0)
				m_pDirectionPrev->Enable(false);
		}
		else
			m_pDirectionPrev->Enable(false);
	}
}

void FrTrainingOptionDlg::OnDirectionPrevBtnUp()
{
	// HACK
	if (0)
		OnDirectionPrevBtnInit(0);
	m_direction -= 10;
	if (m_direction == 0)
	{
		if (m_pDirectionPrev)
			m_pDirectionPrev->Enable(false);
	}
	if (m_pDirectionNext)
		m_pDirectionNext->Enable(true);
	if (m_pDirection)
	{
		m_pDirection->SetLine(1, MakeStr("%d\xb5\xb5", m_direction), 0, 0, 0);
	}
}

void FrTrainingOptionDlg::OnDirectionNextBtnInit(int param)
{
	m_pDirectionNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pDirectionNext)
	{
		m_pDirectionNext->SetPushDelay(0.0f);
		if (m_bEnableChange)
		{
			if (m_direction == 360)
				m_pDirectionNext->Enable(false);
		}
		else
			m_pDirectionNext->Enable(false);
	}
}

void FrTrainingOptionDlg::OnDirectionNextBtnUp()
{
	// HACK
	if (0)
		OnDirectionNextBtnInit(0);
	m_direction += 10;
	if (m_direction == 360)
	{
		if (m_pDirectionNext)
			m_pDirectionNext->Enable(false);
	}
	if (m_pDirectionPrev)
		m_pDirectionPrev->Enable(true);
	if (m_pDirection)
	{
		m_pDirection->SetLine(1, MakeStr("%d\xb5\xb5", m_direction), 0, 0, 0);
	}
}

void FrTrainingOptionDlg::OnIntensityInit(int param)
{
	m_pIntensity = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_intensity = (int)Wind().GetGlobalIntensity();
	if (m_pIntensity)
	{
		m_pIntensity->AddLine(MakeStr("%dm", m_intensity), 0, 0);
	}
}

void FrTrainingOptionDlg::OnIntensityPrevBtnInit(int param)
{
	m_pIntensityPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pIntensityPrev)
	{
		m_pIntensityPrev->SetPushDelay(0.0f);
		if (m_bEnableChange)
		{
			if (m_intensity == 0)
				m_pIntensityPrev->Enable(false);
		}
		else
			m_pIntensityPrev->Enable(false);
	}
}

void FrTrainingOptionDlg::OnIntensityPrevBtnUp()
{
	m_intensity -= 1;
	if (m_intensity == 0)
	{
		if (m_pIntensityPrev)
			m_pIntensityPrev->Enable(false);
	}
	if (m_pIntensityNext)
		m_pIntensityNext->Enable(true);
	if (m_pIntensity)
	{
		m_pIntensity->SetLine(1, MakeStr("%dm", m_intensity), 0, 0, 0);
	}
}

void FrTrainingOptionDlg::OnIntensityNextBtnInit(int param)
{
	m_pIntensityNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pIntensityNext)
	{
		m_pIntensityNext->SetPushDelay(0.0f);
		if (m_bEnableChange)
		{
			if (m_intensity == 9)
				m_pIntensityNext->Enable(false);
		}
		else
			m_pIntensityNext->Enable(false);
	}
}

void FrTrainingOptionDlg::OnIntensityNextBtnUp()
{
	m_intensity += 1;
	if (m_intensity == 9)
	{
		if (m_pIntensityNext)
			m_pIntensityNext->Enable(false);
	}
	if (m_pIntensityPrev)
		m_pIntensityPrev->Enable(true);
	if (m_pIntensity)
	{
		m_pIntensity->SetLine(1, MakeStr("%dm", m_intensity), 0, 0, 0);
	}
}

IMPLEMENT_OBJECT(FrNewTrainingOptionDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrNewTrainingOptionDlg, FrForm)

ON_FRESH_VI("training_map", FRCMD_INIT, FrNewTrainingOptionDlg::OnInitMapButton)
ON_FRESH_VV("training_map", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownMapButton)
ON_FRESH_VI("training_mapprev", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitMapPrevButton)
ON_FRESH_VV("training_mapprev", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownMapPrevButton)
ON_FRESH_VI("training_mapnext", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitMapNextButton)
ON_FRESH_VV("training_mapnext", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownMapNextButton)
ON_FRESH_VI("training_fronthole", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitFrontHoleButton)
ON_FRESH_VV("training_fronthole", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownFrontHoleButton)
ON_FRESH_VI("training_backhole", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitBackHoleButton)
ON_FRESH_VV("training_backhole", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownBackHoleButton)
ON_FRESH_VI("training_randomhole", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitRandomHoleButton)
ON_FRESH_VV("training_randomhole", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownRandomHoleButton)
ON_FRESH_VI("training_shufflehole", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitShuffleHoleButton)
ON_FRESH_VV("training_shufflehole", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownShuffleHoleButton)
ON_FRESH_VI("training_hare", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitHareButton)
ON_FRESH_VV("training_hare", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownHareButton)
ON_FRESH_VI("training_kumori", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitKumoriButton)
ON_FRESH_VV("training_kumori", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownKumoriButton)
ON_FRESH_VI("training_ame", FRCMD_INIT, FrNewTrainingOptionDlg::OnInitAmeButton)
ON_FRESH_VV("training_ame", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownAmeButton)
ON_FRESH_VI("training_windedit", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitWindDirectionEdit)
ON_FRESH_VI("training_play", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitPlayButton)
ON_FRESH_VV("training_play", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownPlayButton)
ON_FRESH_VI("training_cancel", FRCMD_INIT,
	FrNewTrainingOptionDlg::OnInitCancelButton)
ON_FRESH_VV("training_cancel", FRCMD_LBUTTONDOWN,
	FrNewTrainingOptionDlg::OnLBDownCancelButton)

END_FRESH_MSGMAP()

FrNewTrainingOptionDlg::FrNewTrainingOptionDlg()
{
	ClearVariables();
}

FrNewTrainingOptionDlg::~FrNewTrainingOptionDlg()
{
}

void FrNewTrainingOptionDlg::ClearVariables()
{
	m_pMapButton = 0;
	m_pMapPrevButton = 0;
	m_pMapNextButton = 0;
	m_pFrontHoleButton = 0;
	m_pBackHoleButton = 0;
	m_pRandomHoleButton = 0;
	m_pShuffleHoleButton = 0;
	m_reserved11c = 0;
	m_pHareButton = 0;
	m_pKumoriButton = 0;
	m_pAmeButton = 0;
	m_pWindDirectionEdit = 0;
	m_reserved140 = 0;
	m_pPlayButton = 0;
	m_pCancelButton = 0;
	m_bExpansion = false;
	m_map = 0xffff;
	m_hole = 0xffff;
	m_weather = 0xffff;
	m_windDirection = 0xffff;
	m_windPower = 0xffff;
}

void FrNewTrainingOptionDlg::OnInitMapButton(int param)
{
	m_pMapButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownMapButton()
{
}

void FrNewTrainingOptionDlg::OnInitMapPrevButton(int param)
{
	m_pMapPrevButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownMapPrevButton()
{
}

void FrNewTrainingOptionDlg::OnInitMapNextButton(int param)
{
	m_pMapNextButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownMapNextButton()
{
}

void FrNewTrainingOptionDlg::OnInitFrontHoleButton(int param)
{
	m_pFrontHoleButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownFrontHoleButton()
{
}

void FrNewTrainingOptionDlg::OnInitBackHoleButton(int param)
{
	m_pBackHoleButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownBackHoleButton()
{
}

void FrNewTrainingOptionDlg::OnInitRandomHoleButton(int param)
{
	m_pRandomHoleButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownRandomHoleButton()
{
}

void FrNewTrainingOptionDlg::OnInitShuffleHoleButton(int param)
{
	m_pShuffleHoleButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownShuffleHoleButton()
{
}

void FrNewTrainingOptionDlg::OnInitHareButton(int param)
{
	m_pHareButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownHareButton()
{
	m_weather = 0;
}

void FrNewTrainingOptionDlg::OnInitKumoriButton(int param)
{
	m_pKumoriButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownKumoriButton()
{
	m_weather = 1;
}

void FrNewTrainingOptionDlg::OnInitAmeButton(int param)
{
	m_pAmeButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownAmeButton()
{
	m_weather = 2;
}

void FrNewTrainingOptionDlg::OnInitWindDirectionEdit(int param)
{
	m_pWindDirectionEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnInitPlayButton(int param)
{
	m_pPlayButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownPlayButton()
{
	Close((eFormRet)1, true);
}

void FrNewTrainingOptionDlg::OnInitCancelButton(int param)
{
	m_pCancelButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrNewTrainingOptionDlg::OnLBDownCancelButton()
{
	Close((eFormRet)2, true);
}

void FrNewTrainingOptionDlg::ShowExpansionMenu(bool show)
{
}

void FrNewTrainingOptionDlg::SetEnvironmentToGame()
{
}

void FrNewTrainingOptionDlg::GetEnvironmentFromGame()
{
}
