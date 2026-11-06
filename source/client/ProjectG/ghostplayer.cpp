#include "stdafx.h"

#include "ghostplayer.h"

#include <algorithm>
#include <iterator>
#include <vector>

#include "golfdoc.h"
#include "objectfactory.h"
#include "projectg.h"
#include "packet.h"
#include "ghostmanager.h"
#include "ghostdocument.h"
#include "sequencestatemachine.h"
#include "ghostgamedlg.h"
#include "shareddoc.h"
#include "soundmanager.h"
#include "taskmanager.h"
#include "actor.h"
#include "club.h"
#include "commonutil.h"
#include "fresh.h"
extern Fresh* g_pFresh;
#include "golfball.h"
#include "ghosthandler.h"
#include "s5/utilities.h"
#include "type_convert.h"
#include "wind.h"
#include "woverlay.h"
#include "wresrcmng.h"
#include "wview.h"
#include "mathconsts.h"

namespace
{

	template <class T>
	bool DecreaseValue(T& value, const T& min, const T& sub)
	{
		if (value > min)
		{
			value -= sub;
			if (value <= min)
			{
				value = min;
			}

			return false;
		}

		return true;
	}

	template <class T>
	void IncreaseValue(T& value, const T& max, const T& add)
	{
		if (value < max)
		{
			value += add;
			if (value >= max)
			{
				value = max;
			}
		}
	}

	bool IsGhostTurn(void)
	{
		return GOLFDOC()->m_currentPlayer != 0;
	}

	class CGhostStateUnit : public CStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CGhostStateUnit";
		}

		CGhostStateUnit(void)
			: m_elapsed(0.0f), m_timer(2.0f), m_timeLimit(2.0f), m_done(false)
		{
		}

		virtual ~CGhostStateUnit(void) { }

		void SetTimer(float fTime) { m_timer = m_timeLimit = fTime; }

		virtual void OnRender(void) { }

		virtual bool OnStateRun(void) = 0;

	private:
		virtual void OnProcess(float fDelta)
		{
			m_delta = fDelta;
			if (!OnFinished() && !m_done)
			{
				m_elapsed += fDelta;
				m_done = OnStateRun();

				if (m_done)
				{
					OnPrintLog(MakeStr(
						"Runned time: %.1f sec, Finish remain : %.1f sec",
						m_elapsed,
						m_timeLimit > 0.0f ? m_timeLimit - m_elapsed : 0.0f));
				}
			}

			m_timer -= fDelta;
		}
		virtual bool OnFinished(void) { return m_timer <= 0.0f; }

	protected:
		float m_elapsed;
		float m_timer;
		float m_timeLimit;
		bool m_done;
		float m_delta;
	};

	class CWindChangeState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CWindChangeState";
		}

		CWindChangeState(unsigned char power, unsigned char direction,
			unsigned char type, unsigned char special)
			: m_power(power),
			  m_direction(direction),
			  m_type(type),
			  m_special(special)
		{
		}

		virtual bool OnStateRun(void)
		{
			SetTimer(1.0f);
			Wind().Init(m_power, m_direction, m_type, m_special);

			return true;
		}

	private:
		unsigned char m_power;
		unsigned char m_direction;
		unsigned char m_type;
		unsigned char m_special;
	};

	class CWeatherChangeState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CWeatherChangeState";
		}

		CWeatherChangeState(unsigned char weather)
			: m_weather(weather)
		{
		}

		virtual bool OnStateRun(void);

	private:
		unsigned char m_weather;
	};

	bool CWeatherChangeState::OnStateRun(void)
	{
		AfxGetTask()->GetActor("GolfBg")
			<< MsgObject(0, 0x142, m_weather, 1, 0, 0, 0);

		return true;
	}

	class CDirectionChangeState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CDirectionChangeState";
		}

		CDirectionChangeState(float direction)
			: m_direction(direction)
		{
		}

		virtual bool OnStateRun(void);

	private:
		float m_direction;
	};

	bool CDirectionChangeState::OnStateRun(void)
	{
		SetTimer(3.0f);

		AfxGetTask()->GetActor("Camera")
			<< MsgObject(0, 0xF2, (intptr_t)&m_direction, 0, 0, 0, 0);

		return true;
	}

	class CSpecialWindState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CSpecialWindState";
		}

		CSpecialWindState(void) { }

		virtual bool OnStateRun(void) { return true; }
	};

	class CClubChangeState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CClubChangeState";
		}

		CClubChangeState(unsigned char club)
			: m_club(club)
		{
		}

		virtual bool OnStateRun(void)
		{
			if (GolfClub().GetIndex() != m_club)
			{
				GolfClub().SetClub(m_club);
				g_audio->PlaySfx("\xc5\xac\xb7\xb4\xb1\xb3\xc3\xbc", 0, 0, 0, 0,
					0.5f, 200.0f);
			}

			return true;
		}

	private:
		unsigned char m_club;
	};

	class CUseItemState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CUseItemState";
		}

		CUseItemState(unsigned long itemTypeId, unsigned long itemId)
			: m_itemTypeId(itemTypeId), m_itemId(itemId)
		{
		}

		virtual bool OnStateRun(void);

	private:
		unsigned long m_itemTypeId;
		unsigned long m_itemId;
	};

	bool CUseItemState::OnStateRun(void)
	{
		AfxGetTask()->GetActor("Item")
			<< MsgObject(0, 0x19F, -2, m_itemTypeId, m_itemId, 0, 0);

		return true;
	}

	class CPoweShotMotionState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CPoweShotMotionState";
		}

		CPoweShotMotionState(unsigned char powerShot)
			: m_powerShot(powerShot)
		{
		}

		virtual bool OnStateRun(void);

	private:
		unsigned char m_powerShot;
	};

	bool CPoweShotMotionState::OnStateRun(void)
	{
		if (m_powerShot)
		{
			AfxGetTask()->GetActor("Player")
				<< MsgObject(0, 0x11F, m_powerShot, 0, 0, 0, 0);
		}

		return true;
	}

	class CBallDropState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CBallDropState";
		}

		CBallDropState(float x, float y, float z)
		{
			m_x = x;
			m_y = y;
			m_z = z;
		}

		virtual bool OnStateRun(void);

	private:
		float m_x;
		float m_y;
		float m_z;
	};

	bool CBallDropState::OnStateRun(void)
	{
		SetTimer(2.5f);

		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(0, 0x8D, (intptr_t)&m_x, 0, 0, 0, 0);

		return true;
	}

	class CTimeOutState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CTimeOutState";
		}

		CTimeOutState(void) { }

		virtual bool OnStateRun(void);
	};

	bool CTimeOutState::OnStateRun(void)
	{
		SetTimer(3.0f);
		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(0, 0x7C, -2, 0, 0, 0, 0);

		return true;
	}

	class CReadyToShotState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CReadyToShotState";
		}

		CReadyToShotState(void) { }

		virtual bool OnStateRun(void)
		{
			SetTimer(0.0f);
			return true;
		}
	};

	class CWarpEventState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CWarpEventState";
		}

		CWarpEventState(GHOST_SHOT_DATA const* pShot)
			: m_warp(false)
		{
			if (!(WisEqual(GolfBall().m_pos.x, pShot->ballPosition.x,
					  g_EPSILON) &&
					WisEqual(GolfBall().m_pos.y, pShot->ballPosition.y,
						g_EPSILON) &&
					WisEqual(GolfBall().m_pos.z, pShot->ballPosition.z,
						g_EPSILON)))
			{
				m_warp = true;
				m_pos[0] = pShot->ballPosition.x;
				m_pos[1] = pShot->ballPosition.y;
				m_pos[2] = pShot->ballPosition.z;
			}
		}

		virtual bool OnStateRun(void);

	private:
		bool m_warp;
		float m_pos[3];
	};

	bool CWarpEventState::OnStateRun(void)
	{
		SetTimer(0.0f);
		if (m_warp)
		{
			AfxGetTask()->GetActor("GolfRule")
				<< MsgObject(0, 0x8E, (intptr_t)m_pos, 0, 0, 0, 0);
		}

		return true;
	}

	class CGhostTurnBeginAnimationState : public CGhostStateUnit
	{
	public:
		virtual char const* GetClassNameA(void) const
		{
			return "CGhostTurnBeginAnimationState";
		}

		CGhostTurnBeginAnimationState(void);
		virtual ~CGhostTurnBeginAnimationState(void);

		virtual bool OnStateRun(void);
		virtual void OnRender(void);

	protected:
		void SelectMaskType(void);
		bool RunAnimate(float fDelta);

	private:
		int m_maskType;
		WOverlay* m_pOverlay;
		std::vector<WRect> m_maskRects;
		std::vector<float> m_maskSpeeds;
		float m_z;
		float m_alpha;
		float m_animTime;
	};

	inline CGhostTurnBeginAnimationState::CGhostTurnBeginAnimationState(void)
		: m_maskType(0),
		  m_pOverlay(0),
		  m_z(0.0f),
		  m_alpha(255.0f),
		  m_animTime(0.0f)
	{
		SetTimer(30.0f);
		m_pOverlay = g_resrcmng->GetOverlay("black_mask.jpg", 0);
		SelectMaskType();
	}

	inline CGhostTurnBeginAnimationState::~CGhostTurnBeginAnimationState(void)
	{
		if (g_resrcmng && m_pOverlay)
		{
			g_resrcmng->Release(m_pOverlay);
			m_pOverlay = 0;
		}
	}

	inline bool CGhostTurnBeginAnimationState::OnStateRun(void)
	{
		float fDelta = m_delta;
		if (RunAnimate(fDelta))
		{
			SetTimer(0.0f);
			return true;
		}

		return false;
	}

	inline void CGhostTurnBeginAnimationState::OnRender(void)
	{
		std::vector<WRect>::iterator it = m_maskRects.begin();
		while (it != m_maskRects.end())
		{
			m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), *it, 0,
				(unsigned char)m_alpha << 24, m_z, 0);
			++it;
		}
	}

	inline void CGhostTurnBeginAnimationState::SelectMaskType(void)
	{
		float width = g_view->GetWidth();
		float height = g_view->GetHeight();

		m_z = 0.0f;
		m_alpha = 255.0f;
		m_animTime = 0.0f;

		m_maskType = rand() % 7;
		if (m_maskType >= 7)
		{
			m_maskType = 0;
		}

		switch (m_maskType)
		{
		case 0:

			m_maskSpeeds.reserve(20);
			for (int i = 0; i < 20; ++i)
			{
				m_maskSpeeds.push_back((rand() % 30 + 10) * 0.1f);
			}
		case 4:
		case 5:
			m_maskRects.reserve(20);
			for (int i = 0; i < 20; ++i)
			{
				float barWidth = width * 0.05f;
				m_maskRects.push_back(
					WRect(i * barWidth, 0.0f, barWidth, height));
			}
			break;
		case 1:

			m_maskSpeeds.reserve(20);
			for (int i = 0; i < 20; ++i)
			{
				m_maskSpeeds.push_back((rand() % 30 + 10) * 0.1f);
			}
		case 3:
		case 6:
			m_maskRects.reserve(20);
			for (int i = 0; i < 20; ++i)
			{
				float barHeight = height * 0.05f;
				m_maskRects.push_back(
					WRect(0.0f, i * barHeight, width, barHeight));
			}
			break;
		case 2:
			m_maskRects.push_back(
				WRect(0.0f, (width - height) * -0.5f, width, width));
			break;
		}
	}

	const float s_animationSpeed = 0.8333333f;

	inline bool CGhostTurnBeginAnimationState::RunAnimate(float fDelta)
	{
		float width = g_view->GetWidth();
		float height = g_view->GetHeight();
		bool finished = false;
		switch (m_maskType)
		{
		case 5:
		{
			float move = s_animationSpeed * (fDelta * (width * 0.05f));
			for (std::vector<WRect>::iterator it = m_maskRects.begin();
				it != m_maskRects.end(); ++it)
			{
				WRect& rect = *it;
				if (DecreaseValue(rect.w, 0.0f, move))
				{
					finished = true;
				}
			}
			break;
		}
		case 4:
		{
			float move = s_animationSpeed * (fDelta * (width * 0.05f));
			for (std::vector<WRect>::iterator it = m_maskRects.begin();
				it != m_maskRects.end(); ++it)
			{
				WRect& rect = *it;
				IncreaseValue(rect.x, width, move);
				if (DecreaseValue(rect.w, 0.0f, move))
				{
					finished = true;
				}
			}
			break;
		}
		case 1:
		{
			DecreaseValue(m_alpha, 0.0f, fDelta * 127.5f);

			float move = width * fDelta * 0.5f;
			int i = 0, closed = 0;
			for (std::vector<WRect>::iterator it = m_maskRects.begin();
				it != m_maskRects.end(); ++it)
			{
				WRect& rect = *it;
				if (i % 2)
				{
					IncreaseValue(rect.x, width, move * m_maskSpeeds[i]);
					if (DecreaseValue(rect.w, 0.0f, move * m_maskSpeeds[i]))
					{
						++closed;
					}
				}
				else
				{
					if (DecreaseValue(rect.w, 0.0f, move * m_maskSpeeds[i]))
					{
						++closed;
					}
				}
				++i;
			}

			if (closed == m_maskRects.size())
			{
				finished = true;
			}
			break;
		}
		case 0:
		{
			DecreaseValue(m_alpha, 0.0f, fDelta * 127.5f);

			float move = height * fDelta * 0.5f;
			int i = 0, closed = 0;
			for (std::vector<WRect>::iterator it = m_maskRects.begin();
				it != m_maskRects.end(); ++it)
			{
				WRect& rect = *it;
				if (i % 2)
				{
					IncreaseValue(rect.y, height, move * m_maskSpeeds[i]);
					if (DecreaseValue(rect.h, 0.0f, move * m_maskSpeeds[i]))
					{
						++closed;
					}
				}
				else
				{
					if (DecreaseValue(rect.h, 0.0f, move * m_maskSpeeds[i]))
					{
						++closed;
					}
				}
				++i;
			}

			if (closed == m_maskRects.size())
			{
				finished = true;
			}
			break;
		}
		case 6:
		{
			float move = s_animationSpeed * (fDelta * (height * 0.05f));
			for (std::vector<WRect>::iterator it = m_maskRects.begin();
				it != m_maskRects.end(); ++it)
			{
				WRect& rect = *it;
				if (DecreaseValue(rect.h, 0.0f, move))
				{
					finished = true;
				}
			}
			break;
		}
		case 3:
		{
			float move = s_animationSpeed * (fDelta * (height * 0.05f));
			for (std::vector<WRect>::iterator it = m_maskRects.begin();
				it != m_maskRects.end(); ++it)
			{
				WRect& rect = *it;
				IncreaseValue(rect.y, height, move);
				if (DecreaseValue(rect.h, 0.0f, move))
				{
					finished = true;
				}
			}
			break;
		}
		case 2:
		{
			if (m_alpha > 0.0f)
			{
				m_alpha -= s_animationSpeed * (fDelta * m_alpha);
				if (m_alpha <= 0.0f)
				{
					m_alpha = 0.0f;
				}
			}

			m_z += fDelta * (g_PI * 4.0f);
			float move = s_animationSpeed * (width * fDelta);
			std::vector<WRect>::iterator it = m_maskRects.begin();
			if (it != m_maskRects.end())
			{
				WRect& rect = *it;
				if (rect.w > 0.0f)
				{
					rect.x += move * 0.5f;
					rect.y += move * 0.5f;
					float w = rect.w -= move;
					rect.h -= move;
					if (w <= 0.0f)
					{
						finished = true;
					}
				}
			}
			break;
		}
		}

		return finished;
	}

}

CGhostPlayer::cMovieStyleDisplay::cMovieStyleDisplay(void)
{
	m_height = 0.0f;
	m_pOverlay = g_resrcmng->GetOverlay("black_mask.jpg", 0);
}

CGhostPlayer::cMovieStyleDisplay::~cMovieStyleDisplay(void)
{
	if (g_resrcmng && m_pOverlay)
	{
		g_resrcmng->Release(m_pOverlay);
		m_pOverlay = 0;
	}
}

void CGhostPlayer::cMovieStyleDisplay::ResetHeight(void)
{
	m_height = 0.0f;
}

inline void CGhostPlayer::cMovieStyleDisplay::Process(float fDelta)
{
	float maxHeight = g_view->GetHeight() * (1.0f / 7.0f);
	if (m_height < maxHeight)
	{
		m_height += fDelta * maxHeight;
	}

	if (m_height > maxHeight)
	{
		m_height = maxHeight;
	}
}

void CGhostPlayer::cMovieStyleDisplay::Render(void)
{
	float maxHeight = g_view->GetHeight() * (1.0f / 7.0f);

	float alpha = 255.0f * (m_height / maxHeight);
	m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
		WRect(0.0f, 0.0f, g_view->GetWidth(), m_height), 0,
		((unsigned char)alpha << 24) | 0xFFFFFF, 0.0f, 0);

	m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
		WRect(0.0f, g_view->GetHeight() - m_height, g_view->GetWidth(),
			m_height),
		0, ((unsigned char)alpha << 24) | 0xFFFFFF, 0.0f, 0);
}

IMPLEMENT_ACTOR(CGhostPlayer, IActor)

CGhostPlayer::CGhostPlayer(void)
	: m_pGhostDoc(0),
	  m_pHoleData(0),
	  m_pStateMachine(0),
	  m_pAward(0),
	  m_pMovieStyle(0),
	  m_totalShotCount(0),
	  m_holeShotCount(0),
	  m_errorCode(0),
	  m_shooting(0),
	  m_awardSent(0)
{
	m_dispPriority = DISP_PRIORITY_5;
}

CGhostPlayer::~CGhostPlayer(void)
{
}

CStateUnit* CGhostPlayer::CreateStateUnit(int type, GHOST_ACTION const* pAction)
{
	switch (type)
	{
	case 0:
	{
		unsigned char power = (unsigned char)pAction->param[0];
		unsigned char direction = (unsigned char)pAction->param[1];
		unsigned char windType = (unsigned char)pAction->param[2];
		unsigned char special = (unsigned char)pAction->param[3];

		return new CWindChangeState(power, direction, windType, special);
	}
	case 1:
	{
		unsigned char weather = (unsigned char)pAction->param[0];
		return new CWeatherChangeState(weather);
	}
	case 2:
	{
		float direction = *(float*)&pAction->param[0];
		return new CDirectionChangeState(direction);
	}
	case 3:
		return new CSpecialWindState;
	case 4:
	{
		unsigned char club = (unsigned char)pAction->param[0];
		return new CClubChangeState(club);
	}
	case 5:
	{
		unsigned long itemTypeId = pAction->param[0];
		unsigned long itemId = pAction->param[1];
		return new CUseItemState(itemTypeId, itemId);
	}
	case 6:
	{
		unsigned char powerShot = (unsigned char)pAction->param[0];
		return new CPoweShotMotionState(powerShot);
	}
	case 7:
	{
		float x = *(float*)&pAction->param[0];
		float y = *(float*)&pAction->param[1];
		float z = *(float*)&pAction->param[2];
		return new CBallDropState(x, y, z);
	}
	case 8:
		return new CTimeOutState;
	}

	return 0;
}

void CGhostPlayer::OnLoad(void)
{
	m_pGhostDoc = WSingleton<CGhostManager>::Instance()->GetDocument();

	m_serverProperty = S5::UTIL::GetServerProperty();
	if (m_pGhostDoc->GetGameData()->serverProperty & 0x80)
	{
		S5::UTIL::SetServerProperty(m_serverProperty | 0x80);
	}
	else
	{
		S5::UTIL::SetServerProperty(m_serverProperty & ~0x80);
	}

	m_pStateMachine = new CSequenceStateMachine(this, "GhostSequenceMachine");
	m_pMovieStyle = new cMovieStyleDisplay;
}

void CGhostPlayer::OnReload(void)
{
}

void CGhostPlayer::OnInit(void)
{
	m_pStateMachine->Clear();
	m_shots.clear();
	m_holeShotCount = 0;

	m_pHoleData = m_pGhostDoc->GetHoleData(GOLFDOC()->m_currentHole);

	if (m_pHoleData)
	{
		std::list<GHOST_SHOT_DATA*> const& shotDataList =
			m_pHoleData->GetShotDataList();
		std::copy(shotDataList.begin(), shotDataList.end(),
			std::back_inserter(m_shots));

		int shotNum = m_totalShotCount;
		std::list<GHOST_SHOT_DATA*>::iterator it = m_shots.begin();
		while (it != m_shots.end())
		{
			GHOST_SHOT_DATA* pShot = *it;

			m_pStateMachine->BackInsert(new CWarpEventState(pShot));
			m_pStateMachine->BackInsert(new CGhostTurnBeginAnimationState);

			for (int i = 0; i < 9; ++i)
			{
				if (pShot->action[i].bActive)
				{
					m_pStateMachine->BackInsert(
						CreateStateUnit(i, &pShot->action[i]));
				}
			}

			if (pShot->shotIndex != ++shotNum)
			{
				m_errorCode = 1;
				break;
			}

			m_pStateMachine->BackInsert(new CReadyToShotState);
			++it;
		}

		if (!m_shots.empty())
		{
			SetWindAndWeather(GOLFDOC()->m_currentHole);
			return;
		}
	}

	m_errorCode = 3;
}

void CGhostPlayer::OnProcess(float fDelta)
{
	if (!m_pAward && m_errorCode)
	{
		char text[64] = { 0 };
		sprintf(text, "GHOST FILE BREAK! (CODE:%d)(HOLE:%d)(SHOT:%d)",
			m_errorCode, GOLFDOC()->m_currentHole, m_totalShotCount);
		GHOST_GAME_BRIEF const* pBrief =
			WSingleton<CGhostManager>::Instance()->GetLatestGameBrief();
		WSingleton<CGhostHandler>::Instance()->SendErrorReport(text,
			MakeStr("%d_%d_%d_%d_%d_%d.GST", pBrief->owner, pBrief->course,
				pBrief->itemId, pBrief->score, pBrief->pang, pBrief->recordId));

		HandleMsg(MsgObject(0, 0xE, 0, 0, 0, 0, 0));
		return;
	}

	if (m_pAward)
	{
		if (m_pAward->IsFinishCalcBonus() && !m_awardSent)
		{
			m_awardSent = 1;
			SendAwardResult();
		}

		return;
	}

	if (!IsGhostTurn())
	{
		if (m_shooting)
		{
			m_shooting = 0;
		}

		return;
	}

	if (m_shooting == 1)
	{
		m_pMovieStyle->Process(fDelta);
	}

	if (!(GOLFDOC()->GameMode() & 0x308))
	{
		return;
	}

	if (!m_pStateMachine->Process(fDelta))
	{
		m_errorCode = 4;
		return;
	}

	if (!m_pStateMachine->GetCurrentState() ||
		strcmp("CReadyToShotState",
			m_pStateMachine->GetCurrentState()->GetClassNameA()) != 0)
	{
		return;
	}

	if (m_holeShotCount >= m_shots.size())
	{
		return;
	}

	std::list<GHOST_SHOT_DATA*>::const_iterator it = m_shots.begin();
	std::advance(it, m_holeShotCount);
	if (it != m_shots.end())
	{
		GHOST_SHOT_DATA* pShot = *it;
		if (pShot->bDummy == 0)
		{
			if (DoShot(pShot))
			{
				m_shooting = 1;
				m_pMovieStyle->ResetHeight();
			}
			else
			{
				m_errorCode = 2;
				return;
			}
		}
	}

	++m_holeShotCount;
	++m_totalShotCount;
}

bool CGhostPlayer::DoShot(GHOST_SHOT_DATA const* pShot)
{
	if (pShot->holeIndex != GOLFDOC()->m_currentHole)
	{
		return false;
	}

	memset(&GOLFDOC()->m_shotInfo, 0, sizeof(sShotProtocol));
	type_convert(&pShot->shotProtocol, &GOLFDOC()->m_shotInfo);
	AfxGetTask()->GetActor("GolfRule") << MsgObject(0, 0x7A, 0, 0, 0, 0, 0);

	return true;
}

void CGhostPlayer::OnDisplay(void)
{
	if (!WSingleton<CSharedDoc>::Instance()->m_bGameOver && IsGhostTurn())
	{
		m_pStateMachine->Render();
	}
}

void CGhostPlayer::OnDestroy(void)
{
	if (m_pStateMachine)
	{
		delete m_pStateMachine;
		m_pStateMachine = 0;
	}

	if (m_pMovieStyle)
	{
		delete m_pMovieStyle;
		m_pMovieStyle = 0;
	}

	S5::UTIL::SetServerProperty(m_serverProperty);
}

void CGhostPlayer::HandleMsg(const MsgObject& msg)
{
	if (WSingleton<CSharedDoc>::Instance()->m_gameMode != 5)
	{
		return;
	}

	switch (msg.message)
	{
	case 18:
	{
		WSendPacket packet((enumClientPacket)0xba);
		packet.Encode1(3);
		packet.Encode4(msg.param1);
		packet.Send(TO_GAME);

		if (msg.param1 == 0x1A000011)
		{
			WSingleton<CProjectG>::Instance()->SetGameSpeed(3.0f);
			AfxGetTask()->GetActor("Letter")
				<< MsgObject(0, 0x193, 0, 0, 0, 0, 0);
		}
		break;
	}

	case 13:
		*(int*)msg.param1 = m_shooting;
		break;

	case 14:
		if (m_pAward == 0)
		{
			m_pAward = CreateForm<FrGhostAward>(g_pFresh->GetManager(), 0,
				"GHOST_AWARD", 0);
			m_pAward->Init(PLAYER(0)->order == 1);

			if (msg.param1 != 0)
			{
				WSingleton<CSharedDoc>::Instance()->m_bGameOver = true;
				WSingleton<CTaskManager>::Instance()->PostMsg(this, "GolfRule",
					0x78, 0x20000, 0, 0, 0);
				m_pAward->Open(0, 0);
			}
			else
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetTopmost(true);
				pForm->FindNextTopFocus(false);
				pForm->SetTimeLimit(5.0f);

				pForm->SetMessage(
					"\xbc\xd5\xbb\xf3\xb5\xc8\x20\xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1\x20\xb9\xdf\xb0\xdf\xb5\xc7\xbe\xee\x20\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc1\xdf\xb4\xdc\xc7\xcf\xb0\xed\x20\xc0\xe1\xbd\xc3\x20\xc8\xc4\x20\xbd\xc3\xbb\xf3\xbd\xc4\xc0\xbb\x20\xc1\xf8\xc7\xe0\xc7\xd5\xb4\xcf\xb4\xd9\x2e\x0a",
					false);
				pForm->Open(
					(FRESH_PFN_RESULT)&CGhostPlayer::ExceptionAlarmWaitCallBack,
					0);
			}
		}
		break;

	case 16:
		m_shooting = 0;
		break;

	case 17:
		if (m_shooting == 1)
		{
			m_pMovieStyle->Render();
		}
		break;

	case 15:
	{
		std::list<sItemInfo>::iterator it = std::find(
			WSingleton<CSharedDoc>::Instance()->m_myItemList.begin(),
			WSingleton<CSharedDoc>::Instance()->m_myItemList.end(), 0x1A000040);
		if (it != WSingleton<CSharedDoc>::Instance()->m_myItemList.end())
		{
			WSendPacket packet((enumClientPacket)0xba);
			packet.Encode1(3);
			packet.Encode4(0x1A000040);
			packet.Send(TO_GAME);
		}
		break;
	}
	}
}

void CGhostPlayer::SetWindAndWeather(unsigned char holeNum)
{
	CGhostHoleData const* pHoleData = m_pGhostDoc->GetHoleData(holeNum);

	if (!pHoleData)
	{
		return;
	}

	GHOST_SHOT_DATA const* pShot = *pHoleData->GetShotDataList().begin();
	GHOST_ACTION const& windAction = pShot->action[0];
	if (windAction.bActive)
	{
		CWindChangeState wind((unsigned char)windAction.param[0],
			(unsigned char)windAction.param[1],
			(unsigned char)windAction.param[2],
			(unsigned char)windAction.param[3]);
		wind.OnStateRun();
	}

	GHOST_ACTION const& weatherAction = pShot->action[1];
	if (!weatherAction.bActive)
	{
		return;
	}

	CWeatherChangeState weather((unsigned char)weatherAction.param[0]);
	weather.OnStateRun();
}

void CGhostPlayer::SendAwardResult(void)
{
	if (!m_pAward)
	{
		return;
	}

	const GHOST_USER_DATA* pUserData = m_pGhostDoc->GetUserData();
	WSendPacket packet((enumClientPacket)0xba);
	packet.Encode1(6);
	packet.Encode4(pUserData->uid);
	packet.Encode4(
		WSingleton<CGhostManager>::Instance()->GetLatestGameBrief()->recordId);
	packet.Encode1(GetCurMap());
	packet.Encode4(m_pAward->GetBasePang());
	packet.Encode4(m_pAward->GetAutoBonus());
	packet.Encode4(m_pAward->GetClickBonus());
	packet.Encode4(m_pAward->GetClickCount());
	packet.Encode4(GOLFDOC()->GetPlayer(0)->score);
	packet.Encode4(GOLFDOC()->GetPlayer(1)->score);
	packet.Encode4(PLAYER(0)->totalPang);
	packet.Encode4(PLAYER(1)->totalPang);
	packet.Encode1(
		WSingleton<CSharedDoc>::Instance()->m_userInfo[1].stat.Level);
	packet.Encode1(
		WSingleton<CSharedDoc>::Instance()->m_userInfo[0].stat.Level);
	packet.Encode1(GetHoleIndex(GOLFDOC()->m_currentHole));
	packet.Send(TO_GAME);

	if (m_errorCode)
	{
		m_pGhostDoc->SetInvalidate();
	}
}

void CGhostPlayer::UseItem(unsigned long itemTypeId, unsigned long itemId)
{
	if (itemTypeId == 0x1A000040)
	{
		return;
	}

	AfxGetTask()->GetActor("Item")
		<< MsgObject(0, 0x19F, -1, itemTypeId, itemId, 0, 0);
}

bool CGhostPlayer::ExceptionAlarmWaitCallBack(int result, FrForm* pForm)
{
	WSingleton<CSharedDoc>::Instance()->m_bGameOver = true;
	WSingleton<CTaskManager>::Instance()->PostMsg(this, "GolfRule", 0x78,
		0x20000, 0, 0, 0);

	if (m_pAward)
	{
		m_pAward->Open(0, 0);
	}
	else
	{
		m_pAward = CreateForm<FrGhostAward>(g_pFresh->GetManager(), 0,
			"GHOST_AWARD", 0);
		m_pAward->Init(PLAYER(0)->order == 1);
		m_pAward->Open(0, 0);
	}

	return true;
}
