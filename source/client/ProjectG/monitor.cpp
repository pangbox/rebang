#include "monitor.h"
#include "woverlay.h"

CMonitor::CMonitor()
	: m_boxColor(0xff000000),
	  m_lineColor(0xffff0000),
	  m_bVisible(true),
	  m_current(0),
	  m_sampleNum(0),
	  m_maxValue(1)
{
}

CMonitor::~CMonitor()
{
	m_samples.clear();
}

void CMonitor::Init(int sampleNum, int maxValue, WRect& rect)
{
	m_current = 0;
	m_sampleNum = sampleNum;
	m_maxValue = maxValue;
	m_rect = rect;

	m_samples.reserve(sampleNum);

	for (int i = 0; i < sampleNum; i++)
		m_samples[i] = 0;
}

void CMonitor::Trace(int value)
{
	if (value == -1)
	{
		if (m_current == 0)
		{
			if (m_sampleNum <= 0)
				m_samples[0] = 0;
			else
				m_samples[0] = m_samples[m_sampleNum - 1];
		}
		else
			m_samples[m_current] = m_samples[m_current - 1];
	}
	else
		m_samples[m_current] = value;
}

void CMonitor::Process(float fElapsed)
{
	m_current = (m_current + 1) % m_sampleNum;
}

bool CMonitor::Render(WView* view) const
{
	if (!m_bVisible)
		return false;

	WOverlay::DrawLineBox(view, m_rect, 0, m_boxColor);
	return true;
}
