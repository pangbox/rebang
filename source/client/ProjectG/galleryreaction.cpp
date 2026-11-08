#include "projectg.h"
#include "golfdoc.h"
#include "galleryreaction.h"

IMPLEMENT_ACTOR(CGallery, IActor)

CGallery::CGallery()
{
	m_time = 0.0f;
	m_sound[0] = '\0';
}

void CGallery::OnProcess(float delta)
{
	if (!m_sound[0])
		return;

	m_time -= delta;
}

void CGallery::SetSound(const char* name, float time)
{
	if (GOLFDOC()->m_tutorialMode < 15 && GOLFDOC()->m_tutorialMode != 8)
		return;

	if (!strcmp(m_sound, name))
	{
		if (m_time > 0.0f)
			return;

		m_time = time;
		g_audio->PlaySfx(m_sound, NULL, 0, m_sound);
	}
	else
	{
		if (m_sound[0])
			g_audio->StopSfx(m_sound);

		m_time = time;
		strcpy(m_sound, name);
		g_audio->PlaySfx(m_sound, NULL, 0, m_sound);
	}
}

void CGallery::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0xff:
	case 0x100:
		SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xb9\xda\xbc\xf6", 0.0f);
		break;

	case 0x101:
		SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbe\xc6", 0.0f);
		break;

	case 0x102:

		SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbe\xc6", 3.0f);
		break;

	case 0x103:
	case 0x104:
		SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbe\xc6", 0.0f);
		break;

	case 0x105:
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
			0x14000004)
			SetSound(MakeStr("t_Ex6-%d", rand() % 3 + 1), 0.0f);
		else
			SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbe\xc6", 0.0f);
		break;
	case 0x106:
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
			0x14000004)
			SetSound(MakeStr("t_Ex4-%d", rand() % 3 + 1), 0.0f);
		else
			SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbe\xc6", 0.0f);
		break;
	case 0x107:
	case 0x108:
		SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbe\xc6", 0.0f);
		break;

	case 0x109:
		SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbd\xc7\xb8\xc1", 0.0f);
		break;
	case 0x10a:
		SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbe\xc6", 0.0f);
		break;
	case 0x10b:
		SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbf\xc0", 0.0f);
		break;

	case 0x10d:
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
			0x14000004)
			SetSound(MakeStr("t_Ex3-%d", rand() % 4 + 1), 0.0f);
		else
			SetSound("\xb0\xb6\xb7\xaf\xb8\xae_\xbf\xc0", 0.0f);
		break;
	}
}
