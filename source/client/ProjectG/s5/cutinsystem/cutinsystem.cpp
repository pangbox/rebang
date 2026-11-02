#include "stdafx.h"
#include "cutinsystem.h"
#include "clientsetting.h"

const float WAIT_TIME = 0.1f;

namespace
{
	float ROTATE_MAX = g_PI * 8.0f;

	bool IsScalingCutInStyle(eCUTINANISTLYE style)
	{
		return style == CUTIN_ZOOM_IN || style == CUTIN_ZOOM_OUT;
	}
	bool IsAlphaCutInStyle(eCUTINANISTLYE style)
	{
		return style == CUTIN_FADE_IN || style == CUTIN_FADE_OUT;
	}
	bool IsRotateCutInStyle(eCUTINANISTLYE style)
	{
		return style == CUTIN_ROTATE;
	}
}

namespace S5
{
	CCutin::CCutin()
		: m_step(0),
		  m_pCharacter(NULL),
		  m_pBackground(NULL),
		  m_pPattern(NULL),
		  m_pText(NULL),
		  m_alpha(255.0f),
		  m_rotate(0),
		  m_scale(1.0f),
		  m_waitTime(WAIT_TIME),
		  m_gameState(0),
		  m_condition(0),
		  m_reservedItemId(0),
		  m_bReserved(false)
	{
		m_dispPriority = DISP_PRIORITY_4;
		ClearAllData(true);
	}

	void CCutin::OnDisplay()
	{
		if (m_step > 2 && m_pCharacter)
		{
			if (m_step >= 6)
			{
				DisplayOverlay(m_pBackground, m_aniInfo.OutType, m_rect[1]);
				DisplayOverlay(m_pPattern, m_aniInfo.OutType, m_rect[2]);
				DisplayOverlay(m_pCharacter, m_aniInfo.OutType, m_rect[0]);
				DisplayOverlay(m_pText, m_aniInfo.OutType, m_rect[3]);
			}
			else
			{
				DisplayOverlay(m_pBackground, m_aniInfo.BgType, m_rect[1]);
				DisplayOverlay(m_pPattern, m_aniInfo.PatternType, m_rect[2]);
				DisplayOverlay(m_pCharacter, m_aniInfo.CharType, m_rect[0]);
				DisplayOverlay(m_pText, m_aniInfo.TextType, m_rect[3]);
			}
		}
	}

	void CCutin::HandleMsg(const MsgObject& msg)
	{
		if (msg.time > g_CurrentTime)
		{
			StoreDelayedMsg(msg);
			return;
		}
		switch (msg.message)
		{
		case 120:
		{
			SetGameState(msg.param1);
			if (!msg.param1)
				ClearAllData(false);
			break;
		}
		case 588:
		{
			const char* image = (const char*)msg.param1;
			if (!GetCondition())
			{
				m_aniInfo = *(CUTIN_ANI_STYPEINFO*)msg.param2;
				if (g_resrcmng && m_pCharacter)
				{
					g_resrcmng->Release(m_pCharacter);
					m_pCharacter = NULL;
				}
				if (g_resrcmng && m_pBackground)
				{
					g_resrcmng->Release(m_pBackground);
					m_pBackground = NULL;
				}
				if (g_resrcmng && m_pPattern)
				{
					g_resrcmng->Release(m_pPattern);
					m_pPattern = NULL;
				}
				if (g_resrcmng && m_pText)
				{
					g_resrcmng->Release(m_pText);
					m_pText = NULL;
				}
				m_pCharacter = g_resrcmng->GetOverlay(m_aniInfo.szCharTex, 0);
				if (m_pCharacter)
				{
					m_pBackground =
						g_resrcmng->GetOverlay(m_aniInfo.szBgTex, 0);
					m_pPattern = g_resrcmng->GetOverlay(m_aniInfo.szPattern, 0);
					m_pText = g_resrcmng->GetOverlay(m_aniInfo.szText, 0);
				}
				SetCondition(msg.param3);
				if (IsGameState(16) && ISCondition(8))
					PostMsg(this, "GolfRule", 589, 0,
						(int)"mode_modeDriveMotion", 0, 0);
			}
			else
			{
				m_bReserved = true;
				m_reservedAniInfo = *(CUTIN_ANI_STYPEINFO*)msg.param2;
				m_reservedImage = image;
				m_reservedItemId = msg.param4;
				SetCondition(msg.param3);
			}
			m_step = 1;
			break;
		}
		}
	}

	bool CCutin::ProcessWorkIn(float dt)
	{
		bool bg =
			ProcessOverlay(dt, m_pBackground, m_aniInfo.BgType, m_rect[1]);
		bool pattern =
			ProcessOverlay(dt, m_pPattern, m_aniInfo.PatternType, m_rect[2]);
		bool character =
			ProcessOverlay(dt, m_pCharacter, m_aniInfo.CharType, m_rect[0]);
		bool text = ProcessOverlay(dt, m_pText, m_aniInfo.TextType, m_rect[3]);
		if (bg == true && pattern == true && character == true && text == true)
			return true;
		return false;
	}

	bool CCutin::ProcessWorkOut(float dt)
	{
		bool bg =
			ProcessOverlay_Out(dt, m_pBackground, m_aniInfo.BgType, m_rect[1]);
		bool pattern = ProcessOverlay_Out(dt, m_pPattern, m_aniInfo.PatternType,
			m_rect[2]);
		bool character =
			ProcessOverlay_Out(dt, m_pCharacter, m_aniInfo.CharType, m_rect[0]);
		bool text =
			ProcessOverlay_Out(dt, m_pText, m_aniInfo.TextType, m_rect[3]);
		if (bg == true && pattern == true && character == true && text == true)
			return true;
		return false;
	}

	bool CCutin::PrepareCutIn()
	{
		if (m_step <= 1 || !m_pCharacter)
			return false;
		SetOverlayInfo(m_pCharacter, m_aniInfo.CharType, m_rect[0]);
		SetOverlayInfo(m_pBackground, m_aniInfo.BgType, m_rect[1]);
		SetOverlayInfo(m_pPattern, m_aniInfo.PatternType, m_rect[2]);
		SetOverlayInfo(m_pText, m_aniInfo.TextType, m_rect[3]);
		return true;
	}

	void CCutin::SetOverlayInfo(WOverlay* overlay, eCUTINANISTLYE style,
		WRect& rect)
	{
		if (!overlay)
			return;
		rect.w = (float)overlay->GetWidth();
		rect.h = (float)overlay->GetHeight();
		switch (m_aniInfo.PosType)
		{
		case CUTIN_DP_NONE:
			rect.x = 0;
			rect.y = 0;
			break;
		case CUTIN_DP_TOP:
			rect.x = 0;
			rect.y = 0;
			break;
		case CUTIN_DP_CENTER:
			rect.x = (g_view->GetWidth() - rect.w) * 0.5f;
			rect.y = (g_view->GetHeight() - rect.h) * 0.5f;
			break;
		case CUTIN_DP_BOTTOM:
			rect.x = g_view->GetWidth() - rect.w;
			rect.y = g_view->GetHeight() - rect.h;
			break;
		}
		ScreenRate(rect, overlay);
		switch (style)
		{
		case CUTIN_TOLEFT:
			rect.x = g_view->GetWidth();
			break;
		case CUTIN_TORIGHT:
			rect.x = -rect.w;
			break;
		case CUTIN_ZOOM_IN:
			m_scale = 1.0f;
			break;
		case CUTIN_ZOOM_OUT:
			m_scale = 0.0f;
			break;
		case CUTIN_FADE_IN:
			m_alpha = 0.0f;
			break;
		case CUTIN_FADE_OUT:
			m_alpha = 255.0f;
			break;
		}
	}

	void CCutin::OnInit()
	{
	}

	void CCutin::OnDestroy()
	{
		if (g_resrcmng && m_pCharacter)
		{
			g_resrcmng->Release(m_pCharacter);
			m_pCharacter = NULL;
		}
		if (g_resrcmng && m_pBackground)
		{
			g_resrcmng->Release(m_pBackground);
			m_pBackground = NULL;
		}
		if (g_resrcmng && m_pPattern)
		{
			g_resrcmng->Release(m_pPattern);
			m_pPattern = NULL;
		}
		if (g_resrcmng && m_pText)
		{
			g_resrcmng->Release(m_pText);
			m_pText = NULL;
		}
	}

	void CCutin::OutDraw_Setting()
	{
		switch (m_aniInfo.OutType)
		{
		case CUTIN_ZOOM_IN:
			m_scale = 1.0f;
			break;
		case CUTIN_ZOOM_OUT:
			m_scale = 0.0f;
			break;
		case CUTIN_FADE_IN:
			m_alpha = 0.0f;
			break;
		case CUTIN_FADE_OUT:
			m_alpha = 255.0f;
			break;
		}
	}

	bool CCutin::IsGameState(unsigned long state)
	{
		if (m_gameState & state)
			return true;
		return false;
	}

	bool CCutin::ISCondition(unsigned long condition)
	{
		if (m_condition & condition)
			return true;
		return false;
	}

	void CCutin::RectScaling(WRect& rect, float scale)
	{
		float x = rect.x + rect.w * 0.5f;
		float y = rect.y + rect.h * 0.5f;
		rect.w *= scale;
		rect.h *= scale;
		rect.x = x - rect.w * 0.5f;
		rect.y = y - rect.h * 0.5f;
	}

	bool CCutin::ProcessWorkInFinish(float dt)
	{
		SAFE_SUBVALUE(m_waitTime, 0.0f, dt);
		if (WisZero(m_waitTime))
		{
			m_waitTime = WAIT_TIME;
			if (m_aniInfo.OutType == CUTIN_ROTATE)
				m_rotate = ROTATE_MAX;
			return true;
		}
		return false;
	}

	void CCutin::ClearAllData(bool bKeep)
	{
		if (!bKeep)
		{
			m_step = 0;
			m_alpha = 255.0f;
			m_rotate = 0.0f;
			m_scale = 1.0f;
			m_bReserved = false;
			m_waitTime = WAIT_TIME;
			m_reservedItemId = 0;
			m_condition = 0;
		}
		m_bReserved = false;
		m_aniInfo.Reset();
		m_reservedAniInfo.Reset();
		m_reservedImage.clear();
		memset(&m_rect[0], 0, sizeof(WRect));
		memset(&m_rect[1], 0, sizeof(WRect));
		memset(&m_rect[2], 0, sizeof(WRect));
		memset(&m_rect[3], 0, sizeof(WRect));
		if (g_resrcmng && m_pCharacter)
		{
			g_resrcmng->Release(m_pCharacter);
			m_pCharacter = NULL;
		}
		if (g_resrcmng && m_pBackground)
		{
			g_resrcmng->Release(m_pBackground);
			m_pBackground = NULL;
		}
		if (g_resrcmng && m_pPattern)
		{
			g_resrcmng->Release(m_pPattern);
			m_pPattern = NULL;
		}
		if (g_resrcmng && m_pText)
		{
			g_resrcmng->Release(m_pText);
			m_pText = NULL;
		}
	}

	const float ALPHA_MAX = 255.0f;
	const float ALPHA_MIN = 0.0f;

	bool CCutin::ProcessOverlay(float dt, WOverlay* overlay,
		eCUTINANISTLYE style, WRect& rect)
	{
		if (overlay)
		{
			switch (style)
			{
			case CUTIN_NONE:
				SAFE_SUBVALUE(m_waitTime, 0.0f, dt);
				if (WisZero(m_waitTime))
					return true;
				return false;
			case CUTIN_TORIGHT:
			{
				float end = g_view->GetWidth() > 1024.0f ? 0.0f : 0.0f;
				if (SAFE_ADDVALUE(rect.x, end,
						(rect.x < -100.0f ? 3.0f : 0.08f) * rect.w * dt) < end)
					return false;
				break;
			}
			case CUTIN_TOLEFT:
				if (SAFE_SUBVALUE(rect.x, 0.0f,
						(rect.x < 100.0f ? 0.08f : 3.0f) * rect.w * dt) > 0.0f)
					return false;
				break;
			case CUTIN_ZOOM_IN:
				if (SAFE_SUBVALUE(m_scale, 0.0f, dt) > 0.0f)
					return false;
				break;
			case CUTIN_ZOOM_OUT:
				if (SAFE_ADDVALUE(m_scale, 1.0f, dt) < 1.0f)
					return false;
				break;
			case CUTIN_ROTATE:
				if (SAFE_ADDVALUE(m_rotate, ROTATE_MAX, ROTATE_MAX * dt) <
					ROTATE_MAX)
					return false;
				break;
			case CUTIN_FADE_IN:
				if (SAFE_ADDVALUE(m_alpha, ALPHA_MAX, dt * ALPHA_MAX) <
					ALPHA_MAX)
					return false;
				break;
			case CUTIN_FADE_OUT:
				if (SAFE_SUBVALUE(m_alpha, ALPHA_MIN, dt * ALPHA_MAX) > 0.0f)
					return false;
				break;
			}
		}
		return true;
	}

	bool CCutin::ProcessOverlay_Out(float dt, WOverlay* overlay,
		eCUTINANISTLYE style, WRect& rect)
	{
		if (overlay)
		{
			eCUTINANISTLYE out = m_aniInfo.OutType;
			if (out == CUTIN_REVERSE || out == CUTIN_UNTIL)
			{
				{
					if (m_aniInfo.OutType == CUTIN_UNTIL)
					{
						out = style;
						if (style == CUTIN_TORIGHT)
							out = CUTIN_TOLEFT;
						else if (style == CUTIN_TOLEFT)
							out = CUTIN_TORIGHT;
					}
					else
						out = style;
				}
			}
			switch (out)
			{
			case CUTIN_NONE:
				SAFE_SUBVALUE(m_waitTime, 0.0f, dt);
				if (WisZero(m_waitTime))
					return true;
				return false;
			case CUTIN_TORIGHT:
			{
				float end = -rect.w;
				if (SAFE_SUBVALUE(rect.x, end,
						(rect.x > end * 0.9f ? 0.08f : 3.0f) * dt * rect.w) >
					end)
					return false;
				break;
			}
			case CUTIN_TOLEFT:
			{
				float end = g_view->GetWidth();
				if (SAFE_ADDVALUE(rect.x, end, dt * rect.w) <
					g_view->GetWidth())
					return false;
				break;
			}
			case CUTIN_ZOOM_IN:
				if (SAFE_SUBVALUE(m_scale, 0.0f, dt) > 0.0f)
					return false;
				break;
			case CUTIN_ZOOM_OUT:
				if (SAFE_ADDVALUE(m_scale, 1.0f, dt) < 1.0f)
					return false;
				break;
			case CUTIN_ROTATE:
				if (SAFE_SUBVALUE(m_rotate, 0.0f, ROTATE_MAX * dt) > 0.0f)
					return false;
				break;
			case CUTIN_FADE_IN:
				if (SAFE_ADDVALUE(m_alpha, ALPHA_MAX, dt * ALPHA_MAX) <
					ALPHA_MAX)
					return false;
				break;
			case CUTIN_FADE_OUT:
				if (SAFE_SUBVALUE(m_alpha, ALPHA_MIN, dt * ALPHA_MAX) > 0.0f)
					return false;
				break;
			}
		}
		return true;
	}

	void CCutin::DisplayOverlay(WOverlay* overlay, eCUTINANISTLYE style,
		WRect& rect)
	{
		float alpha = 255;
		float rotate = 0;
		WRect dest(rect);
		if (overlay)
		{
			if (IsAlphaCutInStyle(style))
				alpha = m_alpha;
			if (IsRotateCutInStyle(style))
				rotate = m_rotate;
			else if (IsScalingCutInStyle(style))
				RectScaling(dest, m_scale);
			overlay->Render(g_view, WRect(0, 0, 1, 1), dest, 0,
				((unsigned char)alpha << 24) | 0xffffff, rotate, 0);
		}
	}

	void CCutin::ScreenRate(WRect& rect, WOverlay* overlay)
	{
		if (overlay)
		{
			struct
			{
				float scaleX;
				float centerY;
			} placement;
			float x = rect.x + rect.w * 0.5f;
			placement.centerY = rect.y + rect.h * 0.5f;
			placement.scaleX = g_view->GetWidth() / 1024.0f;
			float sy = g_view->GetHeight() * (1.0f / 768.0f);
			rect.w *=
				COption::Instance()->vIsWideMode() && overlay == m_pCharacter
				? sy
				: placement.scaleX;
			rect.h *= sy;
			rect.x = x - rect.w * 0.5f;
			rect.y = placement.centerY - rect.h * 0.5f;
			if (overlay->GetWidth() > overlay->GetHeight())
				rect.x = 0;
			else
				rect.y = 0;
		}
	}

	bool CCutin::CheckGameState()
	{
		if (IsGameState(4))
		{
			if (Doc()->m_gameMode == 0)
			{
				if (m_bReserved)
				{
					if (ISCondition(7))
					{
						if (IsGameState(8) && IsGameState(16))
							return true;
					}
				}
				else
				{
					if (ISCondition(8))
						return IsGameState(32);
					if (IsGameState(8) && IsGameState(16))
						return true;
				}
			}
			else
			{
				if (m_bReserved)
				{
					if (ISCondition(7))
						return IsGameState(16);
				}
				else
				{
					if (ISCondition(8))
						return IsGameState(32);
					return IsGameState(16);
				}
			}
		}
		return false;
	}

	std::string CCutin::TwAnimateTest(const tweaker_param& param)
	{
		IFF_STRUCT::S5::sCutinInformation* info =
			ItemManager()->FindCutinInformation(param[0].to_int());
		if (info)
		{
			if (g_resrcmng && m_pCharacter)
			{
				g_resrcmng->Release(m_pCharacter);
				m_pCharacter = NULL;
			}
			m_pCharacter = g_resrcmng->GetOverlay(info->Char_Tex, 0);
			if (g_resrcmng && m_pBackground)
			{
				g_resrcmng->Release(m_pBackground);
				m_pBackground = NULL;
			}
			m_pBackground = g_resrcmng->GetOverlay(info->Bg_Tex, 0);
			if (g_resrcmng && m_pPattern)
			{
				g_resrcmng->Release(m_pPattern);
				m_pPattern = NULL;
			}
			m_pPattern = g_resrcmng->GetOverlay(info->Pattern_Tex, 0);
			if (g_resrcmng && m_pText)
			{
				g_resrcmng->Release(m_pText);
				m_pText = NULL;
			}
			m_pText = g_resrcmng->GetOverlay(info->Text_Tex, 0);
			m_aniInfo.PosType = (eCUTINANI_DRAWPOS)param[1].to_int();
			m_aniInfo.CharType = (eCUTINANISTLYE)param[2].to_int();
			m_aniInfo.BgType = (eCUTINANISTLYE)param[3].to_int();
			m_aniInfo.PatternType = (eCUTINANISTLYE)param[4].to_int();
			m_aniInfo.TextType = (eCUTINANISTLYE)param[5].to_int();
			m_aniInfo.OutType = (eCUTINANISTLYE)param[6].to_int();
			m_step = 2;
			return "setup ok.";
		}
		m_step = 0;
		return "img loding failed.";
	}

	IMPLEMENT_ACTOR(CCutin, IActor)

	void CCutin::OnProcess(float dt)
	{
		switch (m_step)
		{
		case 1:
			if (CheckGameState())
				m_step = 2;
			break;
		case 2:
			if (PrepareCutIn())
			{
				m_step = 3;
				SendMsg(this, "Screen", 400, 0, 0, 0, 0);
			}
			else
				ClearAllData(false);
			break;
		case 3:
			if (ProcessWorkIn(dt))
				m_step = 5;
			break;
		case 5:
			if (ProcessWorkInFinish(dt))
			{
				OutDraw_Setting();
				m_step = 6;
			}
			break;
		case 6:
			if (ProcessWorkOut(dt))
				m_step = 7;
			break;
		case 7:
			if (g_resrcmng && m_pCharacter)
			{
				g_resrcmng->Release(m_pCharacter);
				m_pCharacter = NULL;
			}
			if (g_resrcmng && m_pBackground)
			{
				g_resrcmng->Release(m_pBackground);
				m_pBackground = NULL;
			}
			if (g_resrcmng && m_pPattern)
			{
				g_resrcmng->Release(m_pPattern);
				m_pPattern = NULL;
			}
			if (g_resrcmng && m_pText)
			{
				g_resrcmng->Release(m_pText);
				m_pText = NULL;
			}
			PostMsg(this, "GolfRule", 589, 0, (int)"CUTIN_DRAW_SUCCES", 0, 0);
			if (m_bReserved == true)
			{
				m_aniInfo = m_reservedAniInfo;
				m_step = 1;
				m_bReserved = false;
				m_pCharacter =
					g_resrcmng->GetOverlay(m_reservedImage.c_str(), 0);
				IFF_STRUCT::S5::sCutinInformation* info =
					ItemManager()->FindCutinInformation(m_reservedItemId);
				if (info)
				{
					m_pBackground = g_resrcmng->GetOverlay(info->Bg_Tex, 0);
					m_pPattern = g_resrcmng->GetOverlay(info->Pattern_Tex, 0);
					m_pText = g_resrcmng->GetOverlay(info->Text_Tex, 0);
				}
			}
			else
			{
				m_step = 8;
				SendMsg(this, "Screen", 400, 1, 0, 0, 0);
			}
			break;
		case 8:
			ClearAllData(false);
			break;
		}
	}
}
