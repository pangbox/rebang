#include "stdafx.h"
#include "capturedbg.h"
#include "headicon.h"
#include "contentsdoc.h"
#include "letter_effect.h"

IMPLEMENT_ACTOR(CLetter, IActor)

CLetter::CLetter()
	: m_pText(NULL), m_textList(32, 16)
{
	(void)&g_EPSILON;
	m_pFont = NULL;
	m_pFastForward = NULL;
	m_pReplay = NULL;
	m_dispPriority = DISP_PRIORITY_4;
	AddSkipList(0x2000);
}

void CLetter::OnPreLoadInit()
{
	m_fadeTime = 0.5f;
	m_bShow = true;
	m_replayTime = 0;
}

void CLetter::OnInit()
{
	m_state = 0;
	m_align = 2;
}

void CLetter::OnDestroy()
{
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
	for (sText* text = m_textList.Start(); text; text = m_textList.Next())
		delete text;
	m_textList.Reset();
}

void CLetter::OnLoad()
{
	if (CSharedDoc::Instance()->GetPeriodDoc()->IsUsableContents(3))
		AddOverlay("\xc6\xce\xbe\xdf"
				   "00",
			"[text_pangya32.jpg", NULL, false);
	else if (IsLocalContent((localContentType_t)101))
		AddOverlay("\xc6\xce\xbe\xdf"
				   "00",
			"[text_pangya12.jpg", NULL, false);
	else if (IsLocalContent((localContentType_t)126))
		AddOverlay("\xc6\xce\xbe\xdf"
				   "00",
			"[text_pangya17.jpg", NULL, false);
	else if (IsLocalContent((localContentType_t)139))
		AddOverlay("\xc6\xce\xbe\xdf"
				   "00",
			"[text_pangya18.jpg", NULL, false);
	else if (IsLocalContent((localContentType_t)147))
		AddOverlay("\xc6\xce\xbe\xdf"
				   "00",
			"[text_pangya22.jpg", NULL, false);
	else
		AddOverlay("\xc6\xce\xbe\xdf"
				   "00",
			"[text_pangya00.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "01",
		"[text_pangya01.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "02",
		"[text_pangya02.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "03",
		"[text_pangya03.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "04",
		"[text_pangya04.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "06",
		"[text_pangya06.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "08",
		"[text_pangya08.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "11",
		"[text_pangya11.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "13",
		"[text_pangya13.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "16",
		"[text_pangya16.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "19",
		"[text_pangya19.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "20",
		"[text_pangya20.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "21",
		"[text_pangya21.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "23",
		"[text_pangya23.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "26",
		"[text_pangya26.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "27",
		"[text_pangya27.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "28",
		"[text_pangya28.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "29",
		"[text_pangya29.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "31",
		"[text_pangya31.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "58",
		"[text_pangya58.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "67",
		"[text_pangya67.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf"
			   "68",
		"[text_pangya68.jpg", NULL, false);
	AddOverlay("OB", "[ob.jpg", NULL, false);
	WRect rect;
	if (CSharedDoc::Instance()->m_golfGame.gameType == 1 ||
		CSharedDoc::Instance()->m_golfGame.gameType == 3)
	{
		rect.x = 0;
		rect.y = 384;
		rect.w = 256;
		rect.h = 64;
		AddOverlay("win", "[hole_end.jpg", &rect, false);
		rect.x = 256;
		rect.y = 384;
		rect.w = 256;
		rect.h = 64;
		AddOverlay("lose", "[hole_end.jpg", &rect, false);
		rect.x = 0;
		rect.y = 448;
		rect.w = 256;
		rect.h = 64;
		AddOverlay("draw", "[hole_end.jpg", &rect, false);
	}
	else
	{
		rect.x = 0;
		rect.y = 0;
		rect.w = 512;
		rect.h = 64;
		AddOverlay("\xbe\xcb\xb9\xd9\xc6\xae\xb7\xce\xbd\xba", "[hole_end.jpg",
			&rect, false);
	}
	rect.x = 0;
	rect.y = 64;
	rect.w = 256;
	rect.h = 64;
	AddOverlay("\xc0\xcc\xb1\xdb", "[hole_end.jpg", &rect, false);
	rect.x = 256;
	rect.y = 64;
	rect.w = 256;
	rect.h = 64;
	AddOverlay("\xb9\xf6\xb5\xf0", "[hole_end.jpg", &rect, false);
	rect.x = 0;
	rect.y = 128;
	rect.w = 256;
	rect.h = 64;
	AddOverlay("\xc6\xc4", "[hole_end.jpg", &rect, false);
	rect.x = 256;
	rect.y = 128;
	rect.w = 256;
	rect.h = 64;
	AddOverlay("\xba\xb8\xb1\xe2", "[hole_end.jpg", &rect, false);
	rect.x = 0;
	rect.y = 192;
	rect.w = 512;
	rect.h = 64;
	AddOverlay("\xb4\xf5\xba\xed\xba\xb8\xb1\xe2", "[hole_end.jpg", &rect,
		false);
	rect.x = 0;
	rect.y = 256;
	rect.w = 512;
	rect.h = 64;
	AddOverlay("\xc6\xae\xb8\xae\xc7\xc3\xba\xb8\xb1\xe2", "[hole_end.jpg",
		&rect, false);
	rect.x = 0;
	rect.y = 320;
	rect.w = 64;
	rect.h = 64;
	AddOverlay("+4", "[hole_end.jpg", &rect, false);
	rect.x = 64;
	rect.y = 320;
	rect.w = 64;
	rect.h = 64;
	AddOverlay("+5", "[hole_end.jpg", &rect, false);
	rect.x = 128;
	rect.y = 320;
	rect.w = 64;
	rect.h = 64;
	AddOverlay("+6", "[hole_end.jpg", &rect, false);
	rect.x = 192;
	rect.y = 320;
	rect.w = 64;
	rect.h = 64;
	AddOverlay("+7", "[hole_end.jpg", &rect, false);
	rect.x = 256;
	rect.y = 320;
	rect.w = 64;
	rect.h = 64;
	AddOverlay("+8", "[hole_end.jpg", &rect, false);
	rect.x = 320;
	rect.y = 320;
	rect.w = 64;
	rect.h = 64;
	AddOverlay("+9", "[hole_end.jpg", &rect, false);
	rect.x = 384;
	rect.y = 320;
	rect.w = 128;
	rect.h = 64;
	AddOverlay("+10", "[hole_end.jpg", &rect, false);
	rect.x = 0;
	rect.y = 0;
	rect.w = 512;
	rect.h = 64;
	AddOverlay("\xbf\xf6\xc5\xcd\xc7\xd8\xc0\xfa\xb5\xe5", "[shot_end.jpg",
		&rect, false);
	rect.x = 0;
	rect.y = 64;
	rect.w = 256;
	rect.h = 64;
	AddOverlay("\xc6\xf7\xb1\xe2", "[shot_end.jpg", &rect, false);
	AddOverlay("\xc8\xa6\xc0\xce\xbf\xf8", "[text_holeinone.jpg", NULL, false);
	AddOverlay("\xc5\xb8\xc0\xd3\xbe\xc6\xbf\xf4", "[timeout.jpg", NULL, false);
	AddOverlay("\xb9\xf6\xb5\xf0\xc2\xf9\xbd\xba", "[birdie_shot.jpg", NULL,
		false);
	AddOverlay("\xc0\xcc\xb1\xdb\xc2\xf9\xbd\xba", "[eagle_shot.jpg", NULL,
		false);
	AddOverlay("winningshot", "[for_win1.jpg", NULL, false);
	AddOverlay("matchshot", "[for_win2.jpg", NULL, false);
	AddOverlay("winningshot2", "[for_win1_2.jpg", NULL, false);
	AddOverlay("matchshot2", "[for_win2_2.jpg", NULL, false);
	AddOverlay("winningshot3", "[for_win1_3.jpg", NULL, false);
	AddOverlay("matchshot3", "[for_win2_2.jpg", NULL, false);
	AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
			   "30",
		"[approach30.jpg", NULL, true);
	AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
			   "60",
		"[approach60.jpg", NULL, true);
	AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
			   "30_\xbf\xac\xc0\xe5",
		"[approach30_extra.jpg", NULL, true);
	SetPos("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
		   "30_\xbf\xac\xc0\xe5",
		g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.25f + 64.0f);
	AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
			   "60_\xbf\xac\xc0\xe5",
		"[approach60_extra.jpg", NULL, true);
	SetPos("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"
		   "60_\xbf\xac\xc0\xe5",
		g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.25f + 64.0f);
	AddOverlay("OneShot", "[oneshot.jpg", NULL, true);
	SetPos("OneShot", g_view->GetWidth() * 0.5f,
		g_view->GetHeight() * 0.25f + 42.0f);
	AddOverlay("\xc2\xf7\xb7\xca\xc7\xa5\xbd\xc3", "[play.jpg", NULL, true);
	AddOverlay("NoPlay", "[noplay.jpg", NULL, false);
	AddOverlay("Retry", "[noplay2.jpg", NULL, false);
	AddOverlay("Wait", "[wait.jpg", NULL, false);
	SetPos("Wait", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);
	AddOverlay("AI", "[ai.jpg", NULL, false);
	SetPos("AI", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);
	AddOverlay("Replay", "[replay-icon.jpg", NULL, false);
	SetPos("Replay", 45.0f, 60.0f);
	AddOverlay("FastForward", "[forward-icon.jpg", NULL, false);
	SetPos("FastForward", 45.0f, 60.0f);
	AddOverlay("Demo", "[demo.png", NULL, false);
	SetPos("Demo", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);
	m_pFont = g_resrcmng->GetTitleFont();
	char* filename = "[bigfont.jpg";
	m_fontInfo.filename = &filename;
	m_fontInfo.flag = 0;
	m_fontInfo.fonth = 32;
	m_fontInfo.fontw = 32;
	m_fontInfo.numPages = 1;
	m_fontInfo.texw = 256;
	m_fontInfo.texh = 128;
	m_fontInfo.pCharSet = "1234567890ym%-./x ";
	m_pFont->Create(&m_fontInfo);
}

void CLetter::AddOverlay(const char* name, const char* filename, WRect* rect,
	bool bSkip)
{
	sText* text = new sText;
	text->overlay = g_resrcmng->GetOverlay(filename, 0);
	if (!text->overlay)
	{
		delete text;
		return;
	}
	strcpy(text->name, name);
	text->time = 0;
	float width = (float)text->overlay->GetWidth();
	float height = (float)text->overlay->GetHeight();
	if (rect)
	{
		text->uv.x = rect->x / width;
		text->uv.y = rect->y / height;
		text->uv.w = rect->w / width;
		text->uv.h = rect->h / height;
	}
	else
	{
		text->uv = WRect(0.0f, 0.0f, 1.0f, 1.0f);
	}
	text->rect.w = width * text->uv.w;
	text->rect.h = height * text->uv.h;
	text->rect.x =
		(float)(int)(g_view->GetWidth() * 0.5f - text->rect.w * 0.5f);
	text->rect.y =
		(float)(int)(g_view->GetHeight() * 0.25f - text->rect.h * 0.5f);
	text->bSkip = bSkip;
	text->number.bShow = false;
	m_textList.AddItem(text, text->name, 0);
}

void CLetter::SetPos(const char* name, float x, float y)
{
	sText* text = m_textList.Find(name);
	if (text)
	{
		text->rect.x = (float)(int)(x - text->rect.w * 0.5f);
		text->rect.y = (float)(int)(y - text->rect.h * 0.5f);
	}
}

void CLetter::OnProcess(float delta)
{
	m_replayTime += delta;
	if (m_pText && !CCapturedBg::Instance()->IsCaptured())
	{
		if (GOLFDOC()->m_gameMode & 0x8210)
			m_state = 0;
		switch (m_state)
		{
		case 1:
			m_alpha += delta / m_fadeTime * 255.0f;
			if (m_alpha > 255.0f)
				m_state = 2;
			break;
		case 2:
			if (m_pText->time > 0)
			{
				m_pText->time -= delta;
				if (m_pText->time < 0)
					m_state = 3;
			}
			if (m_pText->bSkip && g_input->GetList())
				m_state = 3;
			break;
		case 3:
			m_alpha -= delta / m_fadeTime * 255.0f;
			if (m_alpha < 0)
			{
				m_alpha = 0;
				m_state = 0;
			}
			break;
		}
		float alpha = m_alpha;
		m_alpha = alpha < 0.0f ? 0.0f : alpha > 255.0f ? 255.0f : alpha;
	}
}

void CLetter::DisplayReplay()
{
	static bool display = false;
	static bool blink = false;
	if (m_bShow)
	{
		if (m_replayTime > 1.0f)
		{
			blink ^= true;
			m_replayTime = 0.5f;
			display = true;
		}
		if (blink)
		{
			if (display && m_pReplay)
				m_pReplay->overlay->Render(g_view, m_pReplay->uv,
					m_pReplay->rect, 0, 0xffffffff, 0, 0);
		}
		else if (display)
		{
			display = false;
		}
	}
}

void CLetter::OnDisplay()
{
	if (CCapturedBg::Instance()->IsCaptured())
		return;
	if (CHeadIcon::Instance())
		CHeadIcon::Instance()->DisplayOverPlayer();
	DisplayReplay();
	if (m_pFastForward)
	{
		if (m_pReplay)
			m_pFastForward->rect.x = 88.0f;
		else
			m_pFastForward->rect.x = 13.0f;
		m_pFastForward->overlay->Render(g_view, m_pFastForward->uv,
			m_pFastForward->rect, 0, 0xffffffff, 0, 0);
	}
	if (m_bShow && m_state && m_pText && !(GOLFDOC()->m_gameMode & 0x20400))
	{
		m_pText->overlay->Render(g_view, m_pText->uv, m_pText->rect, 0,
			((int)m_alpha << 24) + 0xffffff, 0, 0);
		if (m_pText->number.bShow)
			Print(g_view->GetWidth() * 0.5f + 5.0f,
				g_view->GetHeight() * 0.25f + 5.0f,
				MakeStr("%d", m_pText->number.number), 3.0f, 0,
				m_pText->number.scale, m_pText->number.color);
	}
}

void CLetter::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
	{
		StoreDelayedMsg(msg);
		return;
	}
	switch (msg.message)
	{
	case 402:
		m_pReplay = m_textList.Find("Replay");
		m_replayTime = 0.5f;
		break;
	case 403:
		m_pFastForward = m_textList.Find("FastForward");
		break;
	case 396:
		if (m_pText && msg.param1)
		{
			if (strstr(m_pText->name, (const char*)msg.param1))
				m_pText = NULL;
		}
		else
		{
			m_pText = NULL;
			m_pReplay = NULL;
			m_pFastForward = NULL;
		}
		break;
	case 399:
		m_state = 3;
		break;
	case 397:
		m_pText = m_textList.Find((const char*)msg.param1);
		if (m_pText)
		{
			if (msg.param2)
				m_pText->time = *(float*)msg.param2;
			else
				m_pText->time = 0;
			if (msg.param3)
			{
				m_alpha = 255.0f;
				m_state = 2;
			}
			else
			{
				m_alpha = 0;
				m_state = 1;
			}
		}
		break;
	case 398:
		m_state = 1;
		break;
	case 401:
		m_pText = m_textList.Find((const char*)msg.param1);
		if (m_pText)
		{
			m_pText->number = *(sNumber*)msg.param2;
			m_pText->number.bShow = true;
		}
		break;
	case 400:
		m_bShow = msg.param1 == 1;
		break;
	case 10011:
		for (sText* text = m_textList.Start(); text; text = m_textList.Next())
		{
			text->rect.x =
				(float)(int)(g_view->GetWidth() * 0.5f - text->rect.w * 0.5f);
			text->rect.y =
				(float)(int)(g_view->GetHeight() * 0.25f - text->rect.h * 0.5f);
		}
		SetPos("Wait", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);
		SetPos("AI", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);
		SetPos("Replay", 45.0f, 60.0f);
		SetPos("FastForward", 45.0f, 60.0f);
		break;
	}
}

void CLetter::Print(float x, float y, const char* str, float gap, int align,
	float scale, unsigned long color)
{
	if (!str || !strlen(str))
		return;
	const char* charset = m_fontInfo.pCharSet;
	m_pFont->SetScale(scale);
	float offset = 0;
	const char* p = str;
	switch (m_align)
	{
	case 1:
		while (*p)
		{
			int ch = *p++;
			const char* found = strchr(charset, ch);
			offset +=
				m_pFont->GetCharWidth(g_view, found - charset) + gap * scale;
		}
		offset = (offset - gap) * 0.5f;
		break;
	case 2:
		while (*p)
		{
			int ch = *p++;
			const char* found = strchr(charset, ch);
			offset +=
				m_pFont->GetCharWidth(g_view, found - charset) + gap * scale;
		}
		offset -= gap;
		break;
	}
	x -= offset;
	p = str;
	while (*p)
	{
		int ch = *p++;
		const char* found = strchr(charset, ch);
		const float width =
			m_pFont->PutChar(g_view, x, y, found - charset, align, color);
		x = (width + gap) + x;
	}
}
