#include "projectg.h"
#include "grid.h"
#include "golfdoc.h"
#include "golfball.h"
#include "club.h"
#include "quadtree.h"
#include "capturedbg.h"

IMPLEMENT_ACTOR(CGrid, IActor)

CGrid::CGrid()
{
	m_slopeTex = 0;
	m_middleTex = 0;
	m_dispPriority = DISP_PRIORITY_1;
}

void CGrid::OnDestroy()
{
	if (g_resrcmng && m_slopeTex)
	{
		g_resrcmng->Release(m_slopeTex);
		m_slopeTex = 0;
	}
	if (g_resrcmng && m_middleTex)
	{
		g_resrcmng->Release(m_middleTex);
		m_middleTex = 0;
	}
}

void CGrid::OnPreLoadInit()
{
	for (int i = 0; i < 4; i++)
	{
		m_v[i].tu = (i & 2) ? 1.f : 0.f;
		m_v[i].tv = (i & 1) ? 0.f : 1.f;
	}
	m_vl[0] = &m_v[0];
	m_vl[1] = &m_v[1];
	m_vl[2] = &m_v[3];
	m_vl[3] = &m_v[2];
	m_vl[4] = NULL;
	m_angle = 0.f;
	m_gridValue = -1.f;
	m_bBlink = false;
	Reset();
	CSceneManager::Instance()->RegisterElement(OBJ_FUNC, NULL, this,
		"\xb1\xd7\xb8\xae\xb5\xe5", 0, NULL);
}

void CGrid::OnLoad()
{
	m_slopeTex = g_resrcmng->LoadTexture("[slope.jpg", 0, 0, 0);
	m_middleTex = g_resrcmng->LoadTexture("middle_blue.jpg", 0, 0, 0);
}

void CGrid::OnReload()
{
	m_bBlink = false;
	Reset();
}

void CGrid::Reset()
{
	m_blinkTime = 0.f;
	m_blinkColor = 0xffff0000;
	for (int row = 0; row < 21; row++)
		for (int col = 0; col < 21; col++)
			m_block[row][col].bPass = false;
	m_bHoleGrid = false;
	m_bCheckPass = false;
	m_flag.Disable(4);
	switch (GetCurMap())
	{
	case 6:
		m_color = 0xff509e75;
		break;
	case 9:
		m_color = 0xff677943;
		break;
	case 11:
		m_color = 0xff3e7d30;
		break;
	case 13:
		m_color = 0xfffbb817;
		break;
	default:
		m_color = 0xff00562f;
		break;
	}
}

void CGrid::SetGridType()
{
	unsigned char playerIndex = GOLFDOC()->m_currentPlayer;
	sPlayerData* player = GOLFDOC()->GetPlayer(playerIndex);
	WVector pin = GOLFDOC()->GetHoleData().pin;
	WVector diff = pin - player->pos;
	diff.y = 0.f;
	if (GolfBall().m_groundType == 2 || diff.Magnitude() < 128.f)
	{
		m_flag.Disable(2);
		if (GolfBall().m_groundType == 2 && GolfClub().GetType() == 3 &&
			!GolfClub().IsPowerPutter())
		{
			m_colNum = 10;
			m_rowNum = 11;
			m_interval = 3.2f;
			m_colNumAdd = -4;
			m_rowNumAdd = 0;
			m_intervalAdd = 3.2f;
		}
		else
		{
			m_colNum = 8;
			m_rowNum = 16;
			m_interval = 6.4f;
			m_colNumAdd = -2;
			m_rowNumAdd = 5;
			m_intervalAdd = 0.f;
		}
	}
	else
	{
		m_flag.Enable(2);
		m_colNum = 10;
		m_rowNum = 11;
		m_interval = 12.8f;
		m_colNumAdd = 0;
		m_rowNumAdd = 0;
		m_intervalAdd = 0.f;
		MakeStaticGrid();
	}
	Reset();
}

void CGrid::OnProcess(float delta)
{
	if (GOLFDOC()->m_tutorialMode < 4 || CCapturedBg::Instance()->IsCaptured())
		return;
	if (GolfClub().Updated())
		m_flag.Enable(16);
	if (m_flag.GetFlag(4))
		return;
	WVector diff = g_camera.pivot - m_center;
	diff.y = 0.f;
	if (diff.Magnitude() > 320.f)
	{
		m_flag.Enable(1);
		return;
	}
	m_flag.Disable(1);
	if (!m_flag.GetFlag(2))
	{
		if (m_flag.GetFlag(16))
		{
			m_dir = RotMat(m_angle, 1).za;
			m_right = WCrossProduct(WVector::UNIT_POS_Y, m_dir);
			MakeDynamicGrid();
			SetHoleGrid();
			MakeSlopeIndex();
			m_flag.Disable(8);
		}
		else
			m_flag.Enable(8);
	}
	m_flag.Disable(16);
	if (GolfBall().m_state)
		CheckGridPass();
	if (m_bBlink)
		m_blinkTime += delta;
}

void CGrid::CheckGridPass()
{
	if (m_bCheckPass)
	{
		WVector offset = GolfBall().m_pos - m_block[0][0].pos;
		offset.y = 0.f;
		offset /= m_interval + (GolfClub().Is("1PT") ? m_intervalAdd : 0.f);
		int row = offset * m_dir;
		int col = offset * m_right;
		if (row < m_rowNum + (GolfClub().Is("1PT") ? m_rowNumAdd : 0) - 1 &&
			col < m_colNum + (GolfClub().Is("1PT") ? m_colNumAdd : 0) - 1)
			m_block[row][col].bPass = true;
	}
}

void CGrid::Display()
{
	int row, col;
	if (Skipped() || GOLFDOC()->m_tutorialMode < 4 ||
		CCapturedBg::Instance()->IsCaptured() || m_flag.GetFlag(1) ||
		m_flag.GetFlag(4))
		return;
	for (row = 0; row < m_rowNum + (GolfClub().Is("1PT") ? m_rowNumAdd : 0) - 1;
		row++)
		for (col = 0;
			col < m_colNum + (GolfClub().Is("1PT") ? m_colNumAdd : 0) - 1;
			col++)
			if (m_block[row][col].bPass)
			{
				for (int i = 0; i < 4; i++)
				{
					WVector point = m_block[row + (i & 1)]
										   [col + (((unsigned int)i & 2) >> 1)]
											   .pos;
					m_v[i].SetPosition(point);
					m_v[i].diffuse = 0x80ffffff;
				}
				g_view->DrawPolygonFan(m_vl, 0x20000000, 0, 0);
			}
	for (row = 0; row < m_rowNum + (GolfClub().Is("1PT") ? m_rowNumAdd : 0);
		row++)
		for (col = 0;
			col < m_colNum + (GolfClub().Is("1PT") ? m_colNumAdd : 0) - 1;
			col++)
			g_view->DrawLine(m_block[row][col].pos, m_color,
				m_block[row][col + 1].pos, m_color, 0);
	for (row = 0; row < m_colNum + (GolfClub().Is("1PT") ? m_colNumAdd : 0);
		row++)
		for (col = 0;
			col < m_rowNum + (GolfClub().Is("1PT") ? m_rowNumAdd : 0) - 1;
			col++)
			g_view->DrawLine(m_block[col][row].pos, m_color,
				m_block[col + 1][row].pos, m_color, 0);
	for (row = 0; row < m_rowNum + (GolfClub().Is("1PT") ? m_rowNumAdd : 0) - 1;
		row++)
		for (col = 0;
			col < m_colNum + (GolfClub().Is("1PT") ? m_colNumAdd : 0) - 1;
			col++)
		{
			for (int i = 0; i < 4; i++)
			{
				WVector point = m_block[row][col].slope;
				point.x += ((i & 2) ? 1.f : -1.f) * .2f;
				point.z += ((i & 1) ? 1.f : -1.f) * .2f;
				m_v[i].SetPosition(point);
				if (m_bBlink)
				{
					if (m_blinkTime > .5f)
					{
						m_blinkTime = 0;
						if (m_blinkColor == 0xffff0000)
							m_blinkColor = m_color;
						else
							m_blinkColor = 0xffff0000;
					}
					m_v[i].diffuse = m_blinkColor;
				}
				else
					m_v[i].diffuse =
						m_block[row][col].bPass ? 0xffff0000 : m_color;
			}
			g_view->DrawPolygonFan(m_vl, (m_slopeTex & 0x7ff) | 0x20000000, 0,
				0);
		}
	if (!m_flag.GetFlag(2) && m_flag.GetFlag(8) && m_holeRow >= 0 &&
		m_holeCol >= 0)
	{
		WTVertex* vertices = m_v;
		vertices[0].SetPosition(m_block[m_holeRow][m_holeCol].pos);
		vertices[0].diffuse = 0x5000ff00;
		vertices[1].SetPosition(m_block[m_holeRow + 1][m_holeCol].pos);
		vertices[1].diffuse = 0x5000ff00;
		vertices[2].SetPosition(m_block[m_holeRow][m_holeCol + 1].pos);
		vertices[2].diffuse = 0x5000ff00;
		vertices[3].SetPosition(m_block[m_holeRow + 1][m_holeCol + 1].pos);
		vertices[3].diffuse = 0x5000ff00;
		g_view->DrawPolygonFan(m_vl, 0, 0x400, 0);
	}
}

void CGrid::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 116:
		if (msg.param1)
			m_center = *(WVector*)msg.param1;
		SetGridType();
		break;
	case 135:
		m_bCheckPass = msg.param1 != 0;
		break;
	case 355:
		m_flag.Disable(4);
		break;
	case 356:
		if (!m_bCheckPass)
			m_flag.Enable(4);
		break;
	case 417:
		m_angle += *(float*)msg.param1;
		m_flag.Enable(16);
		break;
	case 466:
		*(float*)msg.param1 =
			m_interval + (GolfClub().Is("1PT") ? m_intervalAdd : 0.f);
		*(int*)msg.param2 = m_rowNum + (GolfClub().Is("1PT") ? m_rowNumAdd : 0);
		break;
	case 467:
		m_gridValue = msg.param1 * 3.2f;
		break;
	case 468:
		SetHoleGrid();
		break;
	case 519:
		if (IsLocalContent((localContentType_t)56))
		{
			if (msg.param1)
			{
				m_bBlink = true;
				m_blinkTime = 0.f;
				m_blinkColor = 0xffff0000;
			}
			else
				m_bBlink = false;
		}
		break;
	case 10008:
		m_angle = *(float*)msg.param1;
		SetGridType();
		m_flag.Enable(16);
		break;
	}
}

void CGrid::MakeStaticGrid()
{
	for (int row = 0; row < m_rowNum; row++)
	{
		for (int col = 0; col < m_colNum; col++)
		{
			WVector pos = m_center +
				WVector((col - m_colNum / 2) * m_interval, 0,
					(row - m_rowNum / 2) * m_interval);
			float radius = GolfBall().GetRadius();
			m_block[row][col].pos =
				GetPVS().GetGroundPoint(pos, true, NULL, true) +
				WVector(0, radius, 0);
		}
	}
	m_dir = WVector::UNIT_POS_Z;
	m_right = WVector::UNIT_POS_X;
	MakeSlopeIndex();
}

void CGrid::MakeDynamicGrid()
{
	for (int row = 0; row < m_rowNum + (GolfClub().Is("1PT") ? m_rowNumAdd : 0);
		row++)
	{
		for (int col = 0;
			col < m_colNum + (GolfClub().Is("1PT") ? m_colNumAdd : 0); col++)
		{
			WVector pos = GolfBall().m_pos +
				(m_interval + (GolfClub().Is("1PT") ? m_intervalAdd : 0.f)) *
					(m_dir * row +
						m_right *
							(col + .5f -
								(m_colNum +
									(GolfClub().Is("1PT") ? m_colNumAdd : 0)) /
									2));
			float radius = GolfBall().GetRadius();
			m_block[row][col].pos =
				GetPVS().GetGroundPoint(pos, true, NULL, true) +
				WVector(0, radius, 0);
		}
	}
}

int CGrid::GetColor(float slope)
{
	float factor = Min((slope > 0.f ? slope : -slope) / (.1f * 3.2f), 1.f);
	if (slope > 0.f)
		return ULblend(0xffa1c400, 0xffc6f2de, factor * 255.f);
	return ULblend(0xffa1c400, 0xff00562f, factor * 255.f);
}

void CGrid::SetHoleGrid()
{
	WVector diff = GetHoleCupPos() - GolfBall().m_pos;
	diff.y = 0.f;
	float interval = m_interval + (GolfClub().Is("1PT") ? m_intervalAdd : 0.f);
	m_holeRow = (diff * m_dir) / interval;
	m_holeCol = (int)floorf((diff * m_right) / interval - .5f) +
		(m_colNum + (GolfClub().Is("1PT") ? m_colNumAdd : 0)) / 2;
	const int& rowCount = m_rowNum;
	if (m_holeRow > rowCount + (GolfClub().Is("1PT") ? m_rowNumAdd : 0) - 2)
		m_holeRow = -1;
	const int& colCount = m_colNum;
	if (m_holeCol > colCount + (GolfClub().Is("1PT") ? m_colNumAdd : 0) - 2)
		m_holeCol = -1;
	m_bHoleGrid = true;
	int distance = diff.Magnitude() /
		(GolfClub().GetApproachMode() > 1
				? 6.4f
				: (GolfClub().Is("1PT") ? 6.4f : 3.2f));
	int rows = m_rowNum + (GolfClub().Is("1PT") ? m_rowNumAdd : 0);
	int middle = (m_colNum + (GolfClub().Is("1PT") ? m_colNumAdd : 0)) * .5f;
	if (GolfBall().m_groundType == 2 && GolfClub().GetType() == 3 &&
		middle >= 1)
	{
		float* heights = new float[rows];
		heights[0] =
			(m_block[0][middle].pos.y + m_block[0][middle - 1].pos.y) * .5f;
		for (int i = 1; i < rows; i++)
			heights[i] =
				(m_block[i][middle].pos.y + m_block[i][middle - 1].pos.y +
					m_block[i - 1][middle - 1].pos.y +
					m_block[i - 1][middle].pos.y) *
				.25f;
		SendMsg(this, "Screen", 430, distance, rows - 1, (int)heights, 0);
		delete[] heights;
	}
	else
	{
		SendMsg(this, "Screen", 430, distance, rows - 1, 0, 0);
	}
}

void CGrid::MoveLine(float delta)
{
	m_linePos += delta;
}

void CGrid::MakeSlopeIndex()
{
	float interval = m_interval + (GolfClub().Is("1PT") ? m_intervalAdd : 0.f);
	for (int row = 0;
		row < m_rowNum + (GolfClub().Is("1PT") ? m_rowNumAdd : 0) - 1; row++)
	{
		for (int col = 0;
			col < m_colNum + (GolfClub().Is("1PT") ? m_colNumAdd : 0) - 1;
			col++)
		{
			float r =
				Between(-.45f, GetSlope(row, col, interval, true), .45f) + .5f;
			float c =
				Between(-.45f, GetSlope(row, col, interval, false), .45f) + .5f;
			float radius = GolfBall().GetRadius();
			m_block[row][col].slope =
				GetPVS().GetGroundPoint(m_block[row][col].pos +
						(r * m_dir + c * m_right) * interval,
					true, NULL, true) +
				WVector(0, radius, 0);
		}
	}
}

float CGrid::GetSlope(int row, int col, float interval, bool bRow)
{
	if (bRow)
		return atanf((GetPVS().GetGreenPos(m_block[row][col].pos) -
						 GetPVS().GetGreenPos(m_block[row + 1][col].pos)) /
				   interval) *
			8.f;
	else
		return atanf((GetPVS().GetGreenPos(m_block[row][col].pos) -
						 GetPVS().GetGreenPos(m_block[row][col + 1].pos)) /
				   interval) *
			8.f;
}
