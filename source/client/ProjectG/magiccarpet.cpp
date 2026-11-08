#include "projectg.h"
#include "clientsetting.h"
#include "magiccarpet.h"

IMPLEMENT_ACTOR(CMagicCarpetHelper, IActor)

CMagicCarpetHelper::CMagicCarpetHelper()
	: m_bVolumeDown(false), m_bAutoVolume(false)
{
}

CMagicCarpetHelper::~CMagicCarpetHelper()
{
	g_audio->SetBGMVolume(COption::Instance()->aGetBgmVolume());
}

void CMagicCarpetHelper::OnInit()
{
}

void CMagicCarpetHelper::OnLoad()
{
}

void CMagicCarpetHelper::OnProcess(float delta)
{
	if (!m_bAutoVolume)
		return;

	AutoVolumeAdjustProcess(delta);
}

void CMagicCarpetHelper::AutoVolumeAdjustProcess(const float& delta)
{
	float volume;
	if (m_bVolumeDown)
	{
		float sfx = COption::Instance()->aGetSfxVolume();
		if (g_audio->GetBGMVolume() > sfx * 0.5f)
		{
			volume = g_audio->GetBGMVolume() - delta * 0.25f;
			if (!(volume > 0.0f))
				volume = 0.0f;

			if (COption::Instance()->aGetBgmVolume() * 0.5f < volume)
				g_audio->SetBGMVolume(volume);
		}
		else
		{
			g_audio->SetBGMVolume(COption::Instance()->aGetSfxVolume() * 0.5f);
		}
	}
	else
	{
		float bgm = COption::Instance()->aGetBgmVolume();
		if (g_audio->GetBGMVolume() < bgm)
		{
			volume = g_audio->GetBGMVolume() + delta * 0.25f;
			if (volume > 1.0f)
				volume = 1.0f;
			g_audio->SetBGMVolume(volume);
		}
		else
		{
			m_bAutoVolume = false;
			g_audio->SetBGMVolume(COption::Instance()->aGetBgmVolume());
		}
	}
}

void CMagicCarpetHelper::OnDisplay()
{
}

void CMagicCarpetHelper::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 640:
		PlaySfxMagicCarpetBoundSound();
		break;

	case 638:
		m_bAutoVolume = true;
		m_bVolumeDown = true;
		break;

	case 639:
		m_bVolumeDown = false;
		break;
	}
}

void CMagicCarpetHelper::PlaySfxMagicCarpetBoundSound()
{
	int melody1 = rand() % 5 + 1;
	g_audio->PlaySfx(MakeStr("magic_melody%d", melody1));

	int melody2 = rand() % 5 + 1;

	if (melody2 != melody1)
	{
		g_audio->PlaySfx(MakeStr("magic_melody%d", melody2));

		int melody3 = rand() % 5 + 1;

		if (melody3 != melody1 && melody3 != melody2)

			g_audio->PlaySfx(MakeStr("magic_melody%d", melody3));
	}
	else
	{
		g_audio->PlaySfx("magic_bounce");
	}

	bool bLong1 = g_audio->IsPlaying("magicharp_goal_long1");

	bool bShort1 = g_audio->IsPlaying("magicharp_goal_short1");
	bool bShort2 = g_audio->IsPlaying("magicharp_goal_short2");

	if (bLong1 || bShort1 || bShort2)
		return;

	int r = rand() % 50;
	if (r == 1)
		g_audio->PlaySfx("magicharp_goal_long1");
	else if (r == 2)
		g_audio->PlaySfx("magicharp_goal_short1");
	else if (r == 3)
		g_audio->PlaySfx("magicharp_goal_short2");
}
