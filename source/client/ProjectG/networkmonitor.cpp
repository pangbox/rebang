#include "networkmonitor.h"

const float s_graphLeft = 10.0f;
const float s_graphTop = 200.0f;
const float s_graphHeight = 50.0f;
const float s_graphLimit = 100.0f;

CNetworkMonitor::CNetworkMonitor()
	: m_bShow(true)
{
}

CNetworkMonitor::~CNetworkMonitor()
{
}

void CNetworkMonitor::Init()
{
	memset(m_inBytes, 0, sizeof(m_inBytes));
	memset(m_outBytes, 0, sizeof(m_outBytes));

	m_elapsed = 0.0f;
	m_index = 0;
}

void CNetworkMonitor::TraceInBytes(unsigned int bytes)
{
	m_inBytes[m_index] += bytes;
}

void CNetworkMonitor::TraceOutBytes(unsigned int bytes)
{
	m_outBytes[m_index] += bytes;
}

void CNetworkMonitor::Render(WView* pView) const
{
	if (!m_bShow)
		return;
	for (int i = 0; i < NETMON_SECONDS; ++i)
	{
		float inBytes = (float)m_inBytes[(m_index + i + 1) % NETMON_SECONDS];
		float outBytes = (float)m_outBytes[(m_index + i + 1) % NETMON_SECONDS];

		inBytes = min(inBytes, s_graphLimit);
		outBytes = min(outBytes, s_graphLimit);
		float y = s_graphTop + s_graphHeight;
		float inTop = y - ((inBytes / s_graphLimit) * s_graphHeight);
		float outTop = inTop - ((outBytes / s_graphLimit) * s_graphHeight) - 1;
		float x = s_graphLeft + i;

		pView->DrawLine2D(WPoint(x, y + 1), WPoint(x, inTop), 0xffff0000, 0);

		pView->DrawLine2D(WPoint(x, inTop), WPoint(x, outTop), 0xff0000ff, 0);
	}
}

void CNetworkMonitor::Process(float delta)
{
	m_elapsed += delta;
	if (m_elapsed >= 1.0f)
	{
		m_elapsed -= 1.0f;
		m_index = (m_index + 1) % NETMON_SECONDS;
		m_inBytes[m_index] = 0;
		m_outBytes[m_index] = 0;
	}
}
