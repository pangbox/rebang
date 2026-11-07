#include "minatl.h"
#include "mousecursor.h"
#include "club.h"
#include "quitwindow.h"
#include "simpleoverlay.h"
#include "quadtree.h"
#include "messengerdlg.h"
#include "frgroupbox.h"
#include "frdesktop.h"
#include "projectg.h"
#include "realmyroomtask.h"
#include "wresrcmng.h"
#include "../../shared/localize.h"

extern WMatrix g_camera;
extern Fresh* g_pFresh;

class CGroundPropt
{
public:
	CGroundPropt(const char* name);
	~CGroundPropt();
	void DrawGroundTip();
	void SelectGroundTip();
	void SetGroundType(const char* type) { strcpy(m_groundtype, type); }
	CSimpleUI* GetGroundTip() { return m_groundtip; }

private:
	char m_groundtype[16];
	CSimpleUI* m_groundtip;
};

#include "clientsetting.h"

CMouseCursor::CMouseCursor()
	: m_overlay(NULL), m_bound(NULL)
{
	m_pTextTip = NULL;
	m_overlay = new sPointer[MOUSE_MAX_NUM];
	const char* names[MOUSE_MAX_NUM] = { "1", "2", "3", "4", "_up", "_down",
		"_zoom", "_both", "_move", "_move_no", "_rotate", "_rotate_no" };
	for (int i = 0; i < MOUSE_MAX_NUM; ++i)
	{
		std::string name;
		name = "[s_pointer";
		name += names[i];
		name += ".png";
		m_overlay[i].overlay = g_resrcmng->GetOverlay(name.c_str(), 0);
		m_overlay[i].w = (float)m_overlay[i].overlay->GetWidth();
		m_overlay[i].h = (float)m_overlay[i].overlay->GetHeight();
		m_overlay[i].len = 32.0f;
		m_overlay[i].num = (int)(m_overlay[i].w / 32.0f);
	}
	m_frequency = 0.35f;
	m_frame = 0;
	m_holecup.x = 0;
	m_holecup.y = 0;
	m_autoHide = false;
	m_bShow = true;
	if (g_view)
	{
		m_pointer.x = g_view->GetWidth();
		m_pointer.y = g_view->GetHeight();
	}
	else
	{
		m_pointer.x = 800;
		m_pointer.y = 600;
	}
	SetBoundArea(NULL);
	Reset(false);
	m_mode = SCREEN_LOBBY;
	m_pointer.x = 500;
	m_pointer.y = 300;
	m_pGroundPt = new CGroundPropt("[ground_2.jpg");
	m_pGroundPt->GetGroundTip()->Add("Fairway", WRect(0, 0, 90, 25), false);
	m_pGroundPt->GetGroundTip()->Add("Rough", WRect(0, 25, 90, 25), false);
	m_pGroundPt->GetGroundTip()->Add("Green", WRect(0, 50, 90, 25), false);
	m_pGroundPt->GetGroundTip()->Add("Bunker", WRect(0, 75, 90, 25), false);
	m_pGroundPt->GetGroundTip()->Add("OB", WRect(0, 100, 90, 25), false);
	m_pGroundPt->GetGroundTip()->Add("Ice", WRect(0, 125, 90, 25), false);
	m_pGroundPt->GetGroundTip()->Add("Sand", WRect(0, 150, 90, 25), false);
	m_pGroundPt->GetGroundTip()->Add("Water", WRect(0, 175, 90, 25), false);
	m_pGroundPt->GetGroundTip()->Add("WaterHazard", WRect(0, 200, 90, 25),
		false);
	m_pGroundPt->GetGroundTip()->Add("Vector", WRect(0, 225, 90, 25), false);
	m_pGroundPt->SetGroundType("");
	m_bTerrainTooltip = COption::Instance()->gGetTerrainTooltip() != 0;
}

CMouseCursor::~CMouseCursor()
{
	for (int i = 0; i < MOUSE_MAX_NUM; ++i)
	{
		if (m_overlay[i].overlay)
		{
			g_resrcmng->Release(m_overlay[i].overlay);
			m_overlay[i].overlay = NULL;
		}
	}
	if (m_overlay)
	{
		delete[] m_overlay;
		m_overlay = NULL;
	}
	if (m_bound)
	{
		delete m_bound;
		m_bound = NULL;
	}
	if (m_pGroundPt)
	{
		delete m_pGroundPt;
		m_pGroundPt = NULL;
	}
}

void CMouseCursor::Reset(bool active)
{
	m_oldPointer = m_pointer;
	m_stopDuration = 0;
	g_input->SetMousePoint(m_pointer.x, m_pointer.y);
	m_active = active;
	m_tempActive = false;
	m_show = true;
	m_ignoreButtonDownArea = false;
	m_dt = 0;
	m_td4Tooltip = 0;
	m_area = AREA_NORMAL;
	m_cursor = UNSELECT;
	m_camMode = false;
	m_forceMoving = 0;
	m_layer = 0;
	m_itemWindow = false;
	m_bRightButton = false;
	m_activeArea.clear();
	for (int i = 0; i < 3; ++i)
		m_buttonDownArea[i] = AREA_NORMAL;
	memset(m_bArea, true, sizeof(m_bArea));
}

void CMouseCursor::Process(const float dt)
{
	if (!m_bShow)
		return;
	if (!m_active && !m_tempActive)
		return;
	if (m_forceMoving > 0)
	{
		m_forceMoving -= dt * m_forceFactor;
		if (m_forceMoving < 0)
		{
			m_forceMoving = 0;
			m_oldPointer = m_target;
		}
		m_pointer = m_target + m_forceMoving * m_delta;
		g_input->SetMousePoint(m_pointer.x, m_pointer.y);
		return;
	}
	if (m_autoHide &&
		WisEqual(g_input->GetMouseDelta(), WVector::ZERO, g_EPSILON) &&
		!g_input->GetButton(LEFT_BUTTON) && !g_input->GetButton(RIGHT_BUTTON) &&
		!g_input->GetButton(MIDDLE_BUTTON))
	{
		m_stopDuration += dt;
		if (m_stopDuration > 3)
			m_show = false;
	}
	else
	{
		m_stopDuration = 0;
		m_show = true;
	}
	if (m_overlay[m_cursor].num > 0)
	{
		m_dt += dt;
		if (m_dt > m_frequency / m_overlay[m_cursor].num)
		{
			m_dt -= m_frequency / m_overlay[m_cursor].num;
			m_frame = (m_frame + 1) % m_overlay[m_cursor].num;
		}
	}
	m_pointer.x = g_input->GetMousePoint().x;
	m_pointer.y = g_input->GetMousePoint().y;
	if (m_mode == SCREEN_TEST)
		return;
	if (m_show)
		m_cursor = CheckArea();
	if (!m_bArea[m_area])
	{
		m_area = AREA_NORMAL;
		m_cursor = UNSELECT;
	}
	for (int i = 0; i < 3; ++i)
		if (g_input->GetButton((eButton)i) == 1)
			m_buttonDownArea[i] = m_area;
	if (GOLFDOC() && Doc()->m_golfGame.gameType != 2 &&
		GOLFDOC()->m_gameMode == 0x200 && m_bTerrainTooltip &&
		(m_area == AREA_NORMAL || m_area == AREA_RANK))
	{
		m_td4Tooltip += dt;
		if (m_td4Tooltip > 0.2f)
		{
			m_td4Tooltip = 0;
			if (m_pGroundPt)
				m_pGroundPt->SelectGroundTip();
		}
	}
	switch (m_cursor)
	{
	case UNSELECT:
		if (g_input->GetButton(LEFT_BUTTON) == 3 &&
			Abs(g_input->GetButtonDownPos(LEFT_BUTTON).x - m_pointer.x) +
					Abs(g_input->GetButtonDownPos(LEFT_BUTTON).y -
						m_pointer.y) >
				0)
			m_cursor = WRONG;
		break;
	case ROLL_OVER:
		if (g_input->GetButton(LEFT_BUTTON))
		{
			if (m_buttonDownArea[0] == m_area || m_ignoreButtonDownArea)
				m_cursor = m_validButton[0] ? SELECT : WRONG;
			else
				m_cursor = UNSELECT;
		}
		else if (g_input->GetButton(RIGHT_BUTTON))
		{
			if (m_buttonDownArea[1] == m_area || m_ignoreButtonDownArea)
				m_cursor = m_validButton[1] ? SELECT : WRONG;
			else
				m_cursor = UNSELECT;
		}
	}
}

int CMouseCursor::CheckArea()
{
	m_validButton[0] = true;
	m_validButton[1] = false;
	switch (m_mode)
	{
	case SCREEN_GAME:
		return GameCheck();
	case SCREEN_CHAT:
		return ChatCheck();
	case SCREEN_TEST:
		return TestCheck();
	}
	return UNSELECT;
}

void CMouseCursor::SetBoundArea(WRect* bound)
{
	if (!bound)
	{
		if (m_bound)
		{
			delete m_bound;
			m_bound = NULL;
		}
	}
	else
	{
		if (!m_bound)
			m_bound = new WRect;
		m_bound->x = bound->x;
		m_bound->y = bound->y;
		m_bound->y = bound->y;
		m_bound->y = bound->y;
	}
}

int CMouseCursor::TestCheck()
{
	m_validButton[0] = true;
	m_validButton[1] = true;
	m_area = AREA_NORMAL;
	return UNSELECT;
}

bool CMouseCursor::IsInclude(float x, float y, float w, float h, bool relative)
{
	if (m_pointer.x >= x && m_pointer.x <= (relative ? 0 : x) + w &&
		m_pointer.y >= y && m_pointer.y <= (relative ? 0 : y) + h)
		return true;
	return false;
}

bool CMouseCursor::SetArea(int area)
{
	if (m_bArea[area])
	{
		m_area = area;
		return true;
	}
	return false;
}

int CMouseCursor::GetCursor()
{
	return m_bRightButton ? BOTH : m_cursor;
}

void CMouseCursor::SetCursor(int cursor)
{
	if (cursor < MOUSE_MAX_NUM)
	{
		m_bRightButton = false;
		m_cursor = cursor;
	}
	else
	{
		m_bRightButton = true;
		m_cursor = ROLL_OVER;
	}
}

CGroundPropt::CGroundPropt(const char* name)
{
	m_groundtip = new CSimpleUI(name);
}

bool CMouseCursor::ChildWindowCallback(FrWnd* wnd, void* param)
{
	std::string name;
	if (wnd->IsVisible())
	{
		if (IS_KINDOF(FrGroupBox, wnd))
		{
			wnd->EnumerateChildWindow(ChildWindowCallback, param);
			if (*(int*)param == ROLL_OVER)
				return false;
			return true;
		}
		WRect rect;
		rect = wnd->GetRect();
		if (Instance()->IsInclude(rect.x, rect.y, rect.w, rect.h))
		{
			wnd->GetWindowName(name);
			if (name == "chatview")
			{
				FrWnd* scrollbar = wnd->FindChild("scrollbar");
				if (scrollbar)
				{
					WRect rect;
					rect = scrollbar->GetRect();
					if (Instance()->IsInclude(rect.x, rect.y, rect.w, rect.h))
					{
						goto hit;
					}
				}
			}
			else
			{
hit:
				Instance()->m_area = AREA_RESERVE_GAME;
				*(int*)param = ROLL_OVER;
				return false;
			}
		}
	}
	Instance()->m_area = AREA_NORMAL;
	*(int*)param = UNSELECT;
	return true;
}

float CMouseCursor::RayTrace4Ground()
{
	WVector start;
	g_view->GetScreen2World(m_pointer.x, m_pointer.y, start, m_ray);
	m_ray.Normalize();
	m_ray *= 300.0f;
	if (Wabs(m_ray.y) < g_EPSILON)
		return 1;
	m_rayRatio = g_camera.pivot.y / m_ray.y * -1.0f;
	WVector point = g_camera.pivot + m_ray * m_rayRatio;
	if (point.x > m_groundArea.x && point.x < m_groundArea.w &&
		point.z > m_groundArea.y && point.z < m_groundArea.h)
		return m_rayRatio;
	return 1;
}

void CMouseCursor::SetActive(bool active)
{
	if (active)
	{
		m_pointer = m_oldPointer;
		m_stopDuration = 0;
		m_cursor = UNSELECT;
		g_input->SetMousePoint(m_pointer.x, m_pointer.y);
		m_show = true;
	}
	else
	{
		m_oldPointer = m_pointer;
		m_area = AREA_NORMAL;
		m_cursor = UNSELECT;
	}
	m_active = active;
}

void CMouseCursor::Store()
{
	m_oldPointer = m_pointer;
}

bool CMouseCursor::InArea(const WRect& rect)
{
	if (CQuitWindow::IsInstantiated() && CQuitWindow::Instance()->IsActive())
		return false;
	if (m_pointer.x > rect.x && m_pointer.x < rect.x + rect.w &&
		m_pointer.y > rect.y && m_pointer.y < rect.y + rect.h)
	{
		m_cursor = ROLL_OVER;
		return true;
	}
	return false;
}

void CMouseCursor::Display()
{
	if (!m_bShow)
		return;
	if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->m_playingMode == 2 &&
		Doc()->m_replayState == 7)
		return;
	if (!m_active && !m_tempActive)
		return;
	if (!m_show)
		return;
	if (m_mode == SCREEN_GAME && GOLFDOC() && GOLFDOC()->m_gameMode == 0x400)
		return;
	WRect dest(m_pointer.x, m_pointer.y, m_overlay[m_cursor].len,
		m_overlay[m_cursor].len);
	switch (m_cursor)
	{
	case UP:
		dest.y -= 4;
		break;
	case DOWN:
		dest.y -= 28;
		break;
	}
	if (GOLFDOC() && Doc()->m_golfGame.gameType != 2 &&
		GOLFDOC()->m_gameMode == 0x200 && m_bTerrainTooltip &&
		!IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
	{
		if ((m_area == AREA_NORMAL || m_area == AREA_RANK) && m_pGroundPt &&
			!COption::Instance()->GetQuitWindowed())
			m_pGroundPt->DrawGroundTip();
	}
	m_overlay[m_cursor].overlay->Render(g_view,
		WRect(m_frame / m_overlay[m_cursor].w * dest.w, 0.0f,
			dest.w / m_overlay[m_cursor].w, dest.h / m_overlay[m_cursor].h),
		dest, 0x2080000, 0xffffffff, 0, 0);
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (m_bRightButton && MESSENGER()->IsVisible() &&
		MESSENGER()->m_dlgRect.IsInRect(m_pointer.x, m_pointer.y))
	{
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("MESSENGER", "mouse_right_btn");
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(m_pointer.x + 2, m_pointer.y + 12, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else if (m_bRightButton)
	{
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("NEW_ICON", "mouse_right_button");
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(m_pointer.x + 2, m_pointer.y + 12, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
}

void CMouseCursor::SetCamMode(bool mode)
{
	m_camMode = mode;
	if (mode)
	{
		ForceMove(g_view->GetWidth() * 0.65f, g_view->GetHeight() - 150, 0.13f);
		m_show = true;
		m_stopDuration = 0;
		m_cursor = UNSELECT;
	}
}

void CMouseCursor::ForceMove(float x, float y, float duration)
{
	m_show = true;
	m_stopDuration = 0;
	if (duration < 0)
	{
		g_input->SetMousePoint(x, y);
		m_pointer.x = x;
		m_pointer.y = y;
		m_cursor = UNSELECT;
		m_area = AREA_NORMAL;
	}
	else
	{
		m_target.x = x;
		m_target.y = y;
		m_delta = m_pointer - m_target;
		m_cursor = UNSELECT;
		m_area = AREA_NORMAL;
		m_forceMoving = 1;
		m_forceFactor = 1 / duration;
	}
}

CGroundPropt::~CGroundPropt()
{
	if (m_groundtip)
	{
		delete m_groundtip;
		m_groundtip = NULL;
	}
}

void CGroundPropt::DrawGroundTip()
{
	if (m_groundtip)
	{
		m_groundtip->SetColor(0xffffffff);
		float x = g_input->GetMousePoint().x + 3;
		float y = g_input->GetMousePoint().y + 10;
		if (x >= g_view->GetWidth() - 90)
			x = g_view->GetWidth() - 90;
		if (y >= g_view->GetHeight() - 25)
			y = g_view->GetHeight() - 25;
		m_groundtip->Render(m_groundtype, x, y, 1, 0);
	}
}

void CGroundPropt::SelectGroundTip()
{
	WVector start, ray;
	WVector mouse = g_input->GetMousePoint();
	g_view->GetScreen2World(mouse.x, mouse.y, start, ray);
	WVector point;
	point = GetPVS().GetRayIntersection(start, ray);
	if (GOLFDOC()->m_pPolySoup->IsOutOfBound(point))
	{
		strcpy(m_groundtype, "OB");
		return;
	}
	CGolfBall::eWater water = (CGolfBall::eWater)0;
	switch (GetPVS().GetGroundType(point, false, &water))
	{
	case 1:
		strcpy(m_groundtype, "Fairway");
		break;
	case 4:
		strcpy(m_groundtype, "Rough");
		break;
	case 6:
		strcpy(m_groundtype, "Rough");
		break;
	case 2:
		strcpy(m_groundtype, "Green");
		break;
	case 3:
		strcpy(m_groundtype, "Bunker");
		break;
	case 13:
		strcpy(m_groundtype, "Rough");
		break;
	case 7:
		strcpy(m_groundtype, "Ice");
		break;
	case 8:
		strcpy(m_groundtype, "Vector");
		break;
	case 9:
		strcpy(m_groundtype, "Water");
		break;
	case 10:
		if (water == 0 || water == 1)
			strcpy(m_groundtype, "OB");
		else
			strcpy(m_groundtype, "WaterHazard");
		break;
	case 11:
		strcpy(m_groundtype, "Sand");
		break;
	case 12:
		strcpy(m_groundtype, "Rough");
		break;
	case 14:
		strcpy(m_groundtype, "Water");
		break;
	default:
		strcpy(m_groundtype, "Rough");
		break;
	}
}

int CMouseCursor::ChatCheck()
{
	if (CQuitWindow::IsInstantiated() && CQuitWindow::Instance()->IsActive())
	{
		m_area = AREA_TRAINING_OPTION;
		return UNSELECT;
	}
	std::list<FrWnd*>& windows = g_pFresh->GetManager()->GetTopWndList();
	for (std::list<FrWnd*>::iterator it = windows.begin(); it != windows.end();
		++it)
	{
		FrWnd* wnd = *it;
		if (wnd->IsVisible())
		{
			WRect rect = wnd->GetRect();
			if (IsInclude(rect.x, rect.y, rect.w, rect.h))
			{
				m_area = AREA_RESERVE_GAME;
				return ROLL_OVER;
			}
		}
	}
	int cursor = UNSELECT;
	g_pFresh->GetManager()->GetDesktop()->EnumerateChildWindow(
		ChildWindowCallback, &cursor);
	if (cursor)
		return cursor;
	m_area = AREA_NORMAL;
	return UNSELECT;
}

int CMouseCursor::GameCheck()
{
	if (!GOLFDOC() ||
		(CQuitWindow::IsInstantiated() && CQuitWindow::Instance()->IsActive()))
		return UNSELECT;
	m_validButton[0] = true;
	m_validButton[1] = false;
	if (m_bound)
	{
		m_pointer.x = Between(m_bound->x, m_pointer.x, m_bound->x + m_bound->w);
		m_pointer.y = Between(m_bound->y, m_pointer.y, m_bound->y + m_bound->h);
		g_input->SetMousePoint(m_pointer.x, m_pointer.y);
	}
	if (OnlinePlay())
	{
		CGolfDoc* golf = GOLFDOC();
		if (!(golf->m_gameMode & 0x300))
		{
			if (Doc()->m_golfGame.gameType == 1)
			{
				WRect rect;
				rect.y = 20;
				rect.w = 118;
				rect.h = 127;
				for (int i = 0; i < 2; ++i)
				{
					rect.x = (i & 1) ? g_view->GetWidth() - 118 : 0;
					if (rect.IsInRect(m_pointer.x, m_pointer.y))
					{
						rect.x += (i & 1) ? 49.0f : 4.0f;
						rect.y += 59;
						rect.w = 65;
						rect.h = 7;
						if (rect.IsInRect(m_pointer.x, m_pointer.y))
						{
							m_area = AREA_COMBO1 + i;
							return ROLL_OVER;
						}
						rect.x = (i & 1) ? g_view->GetWidth() - 118 : 74;
						int j = 0;
						rect.w = 43;
						rect.h = 47;
						for (; j < golf->m_playerNum; ++j, golf = GOLFDOC())
						{
							if (golf->GetPlayer(j)->team == i)
							{
								if (GOLFDOC()->GetPlayer(j)->teammate == 0xff)
									rect.y = 45;
								else
									rect.y =
										j < GOLFDOC()->GetPlayer(j)->teammate
										? 45.0f
										: 94.0f;
								if (rect.IsInRect(m_pointer.x, m_pointer.y))
								{
									m_area = AREA_USERINFO1 + j;
									return ROLL_OVER;
								}
							}
						}
					}
				}
			}
			else
			{
				for (unsigned char i = 0; i < golf->m_playerNum;
					++i, golf = GOLFDOC())
				{
					if (golf->GetPlayer(i)->state == 3)
						continue;
					WRect rect;
					rect.x = (i & 2) ? g_view->GetWidth() - 118 : 0;
					if (i & 1)
						rect.y = 123;
					else
						rect.y = 30;
					rect.w = 118;
					rect.h = 64;
					if (rect.IsInRect(m_pointer.x, m_pointer.y))
					{
						rect.x += (i & 2) ? 49.0f : 4.0f;
						rect.y += 19;
						rect.w = 65;
						rect.h = 7;
						if (rect.IsInRect(m_pointer.x, m_pointer.y))
							m_area = AREA_COMBO1 + i;
						else
							m_area = AREA_USERINFO1 + i;
						return ROLL_OVER;
					}
				}
			}
		}
	}
	else
	{
		if (GOLFDOC()->m_tutorialMode < 15)
		{
			if (Doc()->m_golfGame.gameType == 12 && IsInclude(0, 245, 30, 30))
			{
				m_area = AREA_CHECKPOINT;
				return ROLL_OVER;
			}
		}
		else if (Doc()->m_gameMode == 2 && IsInclude(0, 245, 30, 30))
		{
			m_area = AREA_TRAINING_OPTION;
			return ROLL_OVER;
		}
	}

	if ((Doc()->m_golfGame.gameType == 4 || Doc()->m_golfGame.gameType == 5) &&
		IsInclude(g_view->GetWidth() - 240, 5, 240, 160))
	{
		m_area = AREA_RANK;
		return UNSELECT;
	}
	if (m_pointer.y == 2)
	{
		m_area = AREA_TOP;
		return UP;
	}
	if (m_pointer.y == g_view->GetHeight() - 2)
	{
		m_area = AREA_BOTTOM;
		return DOWN;
	}
	if (g_input->GetButton(LEFT_BUTTON))
	{
		WVector point = g_input->GetButtonDownPos(LEFT_BUTTON) -
			WVector(g_view->GetWidth() * 0.65f, g_view->GetHeight() - 150, 0);
		if (sqrtf(point.x * point.x + point.y * point.y + point.z * point.z) <
			42)
			m_ignoreButtonDownArea = true;
	}
	if (m_camMode &&
		(m_ignoreButtonDownArea || !g_input->GetButton(LEFT_BUTTON)))
	{
		float dy = (float)((double)m_pointer.y - (g_view->GetHeight() - 150));
		float dx = (float)((double)m_pointer.x - g_view->GetWidth() * 0.65f);
		float length = sqrtf(dx * dx + dy * dy);
		if (length < 15)
		{
			m_area = AREA_CAMRESET;
			return ROLL_OVER;
		}
		if (length < 42)
		{
			WVector direction(m_pointer.x - g_view->GetWidth() * 0.65f, 0,
				m_pointer.y - (g_view->GetHeight() - 150));
			float angle =
				CalcDeltaAngle(WVector::UNIT_POS_Z, direction) * g_RADTODEG;
			switch (float2int(floorf(angle / 60.0f)))
			{
			case -2:
				m_area = AREA_CAMBACKWARD;
				break;
			case -1:
				m_area = AREA_CAMLEFT;
				break;
			case 0:
				m_area = AREA_CAMRIGHT;
				break;
			case 1:
				m_area = AREA_CAMDOWN;
				break;
			case 2:
				m_area = AREA_CAMUP;
				break;
			case -3:
				m_area = AREA_CAMFORWARD;
				break;
			}
			if (GOLFDOC()->m_gameMode == 0x8000 &&
				(m_area == AREA_CAMFORWARD || m_area == AREA_CAMBACKWARD))
			{
				m_area = AREA_NORMAL;
				return UNSELECT;
			}
			return ROLL_OVER;
		}
	}
	float barY = COption::Instance()->gGetBar_Y();
	if (IsInclude(55, barY - 109, 55, 45))
	{
		m_validButton[1] = true;
		m_area = AREA_CLUB;
		return ROLL_OVER;
	}
	{
		float dy = (float)((double)m_pointer.y - (barY - 34));
		float dx = m_pointer.x - 23;
		if (dx * dx + dy * dy < 19 * 19)
		{
			m_area = AREA_CLUB_CIRCLE;
			return ROLL_OVER;
		}
	}
	if (GolfClub().GetType() != 3)
	{
		if ((((float)((double)m_pointer.x - (g_view->GetWidth() - 50))) *
					((float)((double)m_pointer.x - (g_view->GetWidth() - 50))) +
				((float)((double)m_pointer.y - (barY + 4))) *
					((float)((double)m_pointer.y - (barY + 4)))) < 40 * 40 ||
			(((float)((double)m_pointer.x - (g_view->GetWidth() - 20))) *
					((float)((double)m_pointer.x - (g_view->GetWidth() - 20))) +
				((float)((double)m_pointer.y - (barY + 32))) *
					((float)((double)m_pointer.y - (barY + 32)))) < 19 * 19)
		{
			m_area = AREA_WIND;
			return ROLL_OVER;
		}
	}
	if (Doc()->m_golfGame.shotTimeLimit)
	{
		float dy = (float)((double)m_pointer.y - (barY + 40));
		float dx = m_pointer.x - 79;
		if (dx * dx + dy * dy < 10 * 10)
		{
			m_area = AREA_TIMER;
			return ROLL_OVER;
		}
	}
	if (CChatMsg::Instance()->IsActive() && IsInclude(89, barY + 9, 464, 31))
	{
		m_area = AREA_CHAT_WINDOW;
		return ROLL_OVER;
	}
	if (!m_itemWindow && !CChatMsg::Instance()->IsActive() &&
		GolfClub().BarIs(2))
	{
		if (GolfClub().GetType() != 3)
		{
			if (IsInclude(s_barStart - 5, barY - 6, 377, 31))
			{
				if (!IsMyTurn(false))
					return UNSELECT;
				m_area = AREA_BAR;
				return ROLL_OVER;
			}
		}
		else
		{
			if (IsInclude(s_barStart - 5, barY - 17, 377, 42))
			{
				if (!IsMyTurn(false))
					return UNSELECT;
				m_area = AREA_BAR;
				return ROLL_OVER;
			}
		}
		if (IsInclude(110, barY + 14, 25, 25))
		{
			if (!IsMyTurn(false))
				return UNSELECT;
			m_area = AREA_BAR;
			return ROLL_OVER;
		}
	}
	if (IsMyTurn(false) && GolfClub().GetType() != 3)
	{
		float dy = m_pointer.y - (barY + 4);
		float dx = m_pointer.x - 58;
		float distance = dx * dx + dy * dy;
		if (distance < 45 * 45 && g_input->GetButton(LEFT_BUTTON) > 0)
		{
			m_area = AREA_SPIN;
			return SELECT;
		}
		if (distance < 25 * 25 && !g_input->GetButton(LEFT_BUTTON))
		{
			m_area = AREA_SPIN;
			return ROLL_OVER;
		}
	}
	if (m_itemWindow && m_pointer.x > 114 &&
		m_pointer.x <
			PLAYER(GOLFDOC()->m_currentPlayer)->itemNum * 45.0f + 129 &&
		m_pointer.y > barY - 25 && m_pointer.y < barY - 25 + 50)
	{
		m_validButton[1] = true;
		m_area = AREA_ITEMSLOT;
		return ROLL_OVER;
	}
	float x = m_pointer.x - 58;
	float y = barY + 4 - m_pointer.y;
	float angle = atanf(y / x) * g_RADTODEG;
	float distance = sqrtf(x * x + y * y);
	if (m_pointer.x > 58 && distance > 45 && distance < 70)
	{
		if (angle > 40 && angle < 75)
		{
			m_area = AREA_ITEM;
			return ROLL_OVER;
		}
		if (angle > 10 && angle < 40)
		{
			m_area = AREA_CHAT;
			return ROLL_OVER;
		}
	}
	if (IsInclude(m_holecup.x - 32, m_holecup.y - 47, 64, 64))
	{
		m_validButton[0] = false;
		m_area = AREA_HOLEINFO;
		return UNSELECT;
	}
	m_validButton[0] = false;
	m_validButton[1] = false;
	m_area = AREA_NORMAL;
	return UNSELECT;
}
