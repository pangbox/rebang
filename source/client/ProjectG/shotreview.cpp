#include "stdafx.h"
#include "shotreview.h"
#include "titlefontex.h"
#include "golfball.h"
#include "quadtree.h"
#include "mathconsts.h"

SpotHistory::SpotHistory()
{
	m_pFont = new CBarFont;
	m_pFont->SetColor(0xffffffff);
	m_pIndicator1 = g_resrcmng->GetOverlay("[fall_indicator1.png", 0);
	m_pIndicator2 = g_resrcmng->GetOverlay("[fall_indicator2.png", 0);
}

SpotHistory::~SpotHistory()
{
	if (g_resrcmng && m_pIndicator2)
	{
		g_resrcmng->Release(m_pIndicator2);
		m_pIndicator2 = NULL;
	}

	if (g_resrcmng && m_pIndicator1)
	{
		g_resrcmng->Release(m_pIndicator1);
		m_pIndicator1 = NULL;
	}

	if (m_pFont)
	{
		delete m_pFont;
		m_pFont = NULL;
	}
}

void SpotHistory::Clear()
{
	m_spotList.clear();
}

void SpotHistory::Display()
{
	if (!(GOLFDOC()->GameMode() & 0x200))
		return;
	if (m_spotList.empty())
		return;
	std::vector<float> alpha;
	alpha.reserve(m_spotList.size());
	float a = 180.0f;
	for (unsigned int i = 0; i < m_spotList.size(); ++i)
	{
		alpha.push_back(a);
		a = (0.0f > a - 20.0f) ? 0.0f : (a - 20.0f) - 20.0f;
	}

	std::reverse(alpha.begin(), alpha.end());
	int count = 0;
	for (std::vector<std::pair<WVector, WVector> >::iterator it =
			 m_spotList.begin();
		it != m_spotList.end(); ++it)
	{
		unsigned long color = ((unsigned char)alpha[count] << 24) | 0xc8c8c8;
		int index;
		WVector from = GetPVS().GetGroundPoint((*it).first, false, NULL, false);
		WVector to = GetPVS().GetGroundPoint((*it).second, true, &index, true);
		g_view->DrawLine2D(from, to, color, 1.0f);
		g_view->Flush(0);
		WTVertex p;
		g_view->Projection2(&p, (*it).second);
		WOverlay* overlay = index == -1 ? m_pIndicator2 : m_pIndicator1;
		int width = overlay->GetWidth(), height = overlay->GetHeight();
		overlay->Render(g_view, WRect(0, 0, 1, 1),
			WRect(p.sx - (width >> 1), p.sy - (height >> 1), (float)width,
				(float)height),
			0, color, 0, 0);
		m_pFont->Print(p.sx, p.sy - m_pFont->GetFontH() * 0.5f,
			MakeStr("%d", ++count), TFA_CENTER, 0, 1);
	}
}

void SpotHistory::AddItem(unsigned int type, unsigned int param)
{
	m_spotList.push_back(
		std::pair<WVector, WVector>(*(WVector*)type, *(WVector*)param));
}

void SpotHistory::PopBack()
{
	if (!m_spotList.empty())
		m_spotList.pop_back();
}

BallTraceHistory::BallTraceHistory()
{
}

BallTraceHistory::~BallTraceHistory()
{
}

void BallTraceHistory::Clear()
{
	m_traceList.clear();
}

void BallTraceHistory::Display()
{
	if (!(GOLFDOC()->GameMode() & 0x200))
		return;
	if (m_traceList.empty())
		return;
	for (std::list<std::vector<WVector> >::iterator it = m_traceList.begin();
		it != m_traceList.end(); ++it)
	{
		std::vector<WVector>& points = *it;
		if (points.empty())
			continue;
		float alpha = 100.0f;
		float step = 140.0f / points.size();
		for (unsigned int i = 0; i < points.size() - 1; ++i)
		{
			g_view->DrawLine2D(points[i], points[i + 1],
				((unsigned char)alpha << 24) | 0xc81414, 1.0f);
			alpha += step;
		}
	}
}

void BallTraceHistory::AddItem(unsigned int type, unsigned int param)
{
	m_traceList.push_back(*(std::vector<WVector>*)type);
}

void BallTraceHistory::PopBack()
{
	if (!m_traceList.empty())
		m_traceList.pop_back();
}

FirstBoundHistory::FirstBoundHistory()
{
	m_pIndicator = g_resrcmng->GetOverlay("{indicator.bmp", 0);
	m_pFont = new CBarFont;
	m_pFont->SetColor(0xffffffff);
}

FirstBoundHistory::~FirstBoundHistory()
{
	if (m_pFont)
	{
		delete m_pFont;
		m_pFont = NULL;
	}

	if (g_resrcmng && m_pIndicator)
	{
		g_resrcmng->Release(m_pIndicator);
		m_pIndicator = NULL;
	}
}

void FirstBoundHistory::Clear()
{
	m_realList.clear();
	m_predictedList.clear();
}

void FirstBoundHistory::Display()
{
	if (!(GOLFDOC()->GameMode() & 0x200))
		return;
	for (std::vector<WVector>::iterator it = m_predictedList.begin();
		it != m_predictedList.end(); ++it)
		DrawIndicator(*it, 0xc8c8c8c8);
	for (std::vector<WVector>::iterator it = m_realList.begin();
		it != m_realList.end(); ++it)
		DrawIndicator(*it, 0xc8ff0000);
}

void FirstBoundHistory::AddItem(unsigned int type, unsigned int param)
{
	if (type == 0)
		m_realList.push_back(*(WVector*)param);
	else if (type == 1)
		m_predictedList.push_back(*(WVector*)param);
}

void FirstBoundHistory::PopBack()
{
	if (!m_realList.empty())
		m_realList.pop_back();
	if (!m_predictedList.empty())
		m_predictedList.pop_back();
}

void FirstBoundHistory::DrawIndicator(const WVector& pos, unsigned long color)
{
	int index;
	WVector ground = GetPVS().GetGroundPoint(pos, true, &index, true);
	WTVertex p;
	g_view->Projection2(&p, ground);
	ground.x = p.sx;
	ground.y = p.sy;
	WRect dst(ground.x - 10, ground.y - 20, 20, 20);
	m_pIndicator->Render(g_view, WRect(0, 0, 1, 1), dst, 0, color, 0, 0);
	WVector from = pos, to = GOLFDOC()->GetHoleData().pin;
	float distance = sqrtf(
		(from.z - to.z) * (from.z - to.z) + (from.x - to.x) * (from.x - to.x));
	m_pFont->Print(ground.x, ground.y - 40 - 3,
		MakeStr("%.1fy", distance * 0.3125f), TFA_CENTER, 0, 1);
}

IMPLEMENT_ACTOR(ShotReview, IActor)
ShotReview::ShotReview()
	: m_bVisible(true), m_bRecordFirstBound(true)
{
	AddSkipList(0x1000);
	m_dispPriority = DISP_PRIORITY_4;
	CSceneManager::Instance()->RegisterElement(OBJ_CUSTOM, NULL, this,
		"ShotReviewScene", 0, NULL);
	m_historyList.push_back(new SpotHistory);
	m_historyList.push_back(new BallTraceHistory);
	m_historyList.push_back(new FirstBoundHistory);
}

ShotReview::~ShotReview()
{
	std::for_each(m_historyList.begin(), m_historyList.end(), Delete_Object());
}

void ShotReview::OnInit()
{
	std::for_each(m_historyList.begin(), m_historyList.end(),
		std::mem_fun(&IReviewHistory::Clear));
}

void ShotReview::OnProcess(float delta)
{
	if (g_input->Get("LCONTROL", true) && g_input->GetDown("REPLAY", true))
		m_bVisible = !m_bVisible;
}

void ShotReview::Display()
{
	if (m_bVisible)
		std::for_each(m_historyList.begin(), m_historyList.end(),
			std::mem_fun(&IReviewHistory::Display));
}

void ShotReview::HandleMsg(const MsgObject& msg)
{
	std::string command = (const char*)msg.param1;
	if (command == "GET_VISIBLE_STATE")
	{
		*(bool*)msg.param2 = m_bVisible;
	}
	else if (command == "POP_BACK_ITEM")
	{
		std::for_each(m_historyList.begin(), m_historyList.end(),
			std::mem_fun(&IReviewHistory::PopBack));
	}
	else if (command == "VISIBLE")
	{
		m_bVisible = msg.param2 != 0;
	}
	else if (command == "SAVE_SPOT_INDICATOR")
	{
		std::vector<IReviewHistory*>::iterator it =
			std::find_if(m_historyList.begin(), m_historyList.end(),
				ReviewHistoryFinder("SpotHistory"));
		if (it != m_historyList.end())
		{
			WVector pos = GolfBall().m_pos, spot;
			SendMsg(this, "Spot", 244, (int)&spot, 0, 0, 0);
			(*it)->AddItem((unsigned int)&pos, (unsigned int)&spot);
		}
	}
	else if (command == "RECORD_REAL_BOUNCE_BALL")
	{
		if (m_bRecordFirstBound)
		{
			m_bRecordFirstBound = false;
			WVector pos = *(WVector*)msg.param2, predicted;
			int found;
			SendMsg(this, "ShotPreview", -1,
				(int)"GET_PREDICTED_FIRSTBOUND_POS", (int)&found,
				(int)&predicted, 0);
			std::vector<IReviewHistory*>::iterator it =
				std::find_if(m_historyList.begin(), m_historyList.end(),
					ReviewHistoryFinder("FirstBoundHistory"));
			if (it != m_historyList.end())
			{
				(*it)->AddItem(0, (unsigned int)&pos);
				if (found)
					(*it)->AddItem(1, (unsigned int)&predicted);
			}
		}
	}
	else if (command == "SAVE_BALL_TRACE")
	{
		m_bRecordFirstBound = true;
		CGolfBall* ball = (CGolfBall*)msg.param2;
		std::vector<const CGolfBall::sExpected*> data;
		data.reserve(ball->m_renderNum);
		for (int i = 0; i <= ball->m_renderNum; ++i)
			data.push_back(ball->PreData(i));
		unsigned int maxCount = 20;
		while (data.size() > maxCount)
		{
			int count = 0;
			for (std::vector<const CGolfBall::sExpected*>::iterator it =
					 data.begin();
				it != data.end();)
			{
				if ((++count & 1) == 0)
				{
					if ((*it)->event > 0)
						++maxCount;
					else
					{
						it = data.erase(it);
						continue;
					}
				}

				++it;
			}
		}

		if (!data.empty())
		{
			const CGolfBall::sExpected* first = *data.begin();
			const CGolfBall::sExpected* last = *(data.end() - 1);
			WVector from;
			from = first->pos;
			WVector to = last->pos;
			float height = to.y - from.y, prevHeight = from.y;
			WVector diff = to - from;
			diff.y = 0;
			float length = diff.Magnitude();
			if (WisZero(length, 1e-5f))
				length = 1.0f;
			std::vector<WVector> trace;
			trace.reserve(data.size());
			for (std::vector<const CGolfBall::sExpected*>::iterator it =
					 data.begin();
				it != data.end(); ++it)
			{
				int index;
				WVector pos =
					GetPVS().GetGroundPoint((*it)->pos, true, &index, true);
				GetPVS().GetGroundType(pos, true, NULL);
				if (index == -1 || fabsf(pos.y - prevHeight) > 20.0f)
					pos.y = sqrtf((pos.z - from.z) * (pos.z - from.z) +
								(pos.x - from.x) * (pos.x - from.x)) /
						length * height;
				trace.push_back(pos);
				prevHeight = pos.y;
			}

			std::vector<IReviewHistory*>::iterator historyIt =
				std::find_if(m_historyList.begin(), m_historyList.end(),
					ReviewHistoryFinder("BallTraceHistory"));
			if (historyIt != m_historyList.end())
				(*historyIt)->AddItem((unsigned int)&trace, 0);
		}
	}
}
