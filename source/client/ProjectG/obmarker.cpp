#include "obmarker.h"
#include "projectg.h"
#include "golfdoc.h"
#include "quadtree.h"
#include "w3dspr.h"

CMarker::CMarker()
	: m_sprite(NULL)
{
}

CMarker::~CMarker()
{
	Reset();
}

void CMarker::Reset()
{
	m_points.clear();
	if (m_sprite)
	{
		delete m_sprite;
		m_sprite = NULL;
	}
}

void CMarker::AddList(const std::vector<WVector>& points)
{
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		int index;
		WVector pos;
		pos = GetPVS().GetGroundPoint(points[i], true, &index, false);
		if (index >= 0)
			m_points.push_back(pos);
	}
}

void CMarker::Display()
{
	if (m_points.size() == 0)
		return;
	if (!m_sprite)
		return;
	for (unsigned int i = 0; i < m_points.size(); ++i)
	{
		m_sprite->SetPos(m_points[i]);
		m_sprite->Render(g_view, 0, W3dSpr::CAMERA_X_ALIGN);
	}
}

void CMarker::GetReady()
{
	if (m_points.size() == 0)
		return;

	std::string filename;
	switch (GetCurMap())
	{
	case MAP_WHITE_WIZ:
		filename = "ob_counter03.jpg";
		break;
	case MAP_DEEP_INFERNO:
		filename = "ob_counter04.jpg";
		break;
	default:
		filename = "ob_counter01.jpg";
		break;
	}
	m_sprite = g_resrcmng->Get3DSpr(filename.c_str(), 0);
	m_sprite->SetRect(1.0f, 2.0f, 0.5f, 0.0f);
}
