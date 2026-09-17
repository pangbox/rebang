#include "mousecursor.h"
#include "frcursor.h"

FrCursor::FrCursor()
{
}

FrCursor::~FrCursor()
{
}

void FrCursor::Init()
{
	CMouseCursor::Instance()->Reset(true);
	CMouseCursor::Instance()->SetMode(CMouseCursor::WRONG);
}

void FrCursor::SetCursor(eCursor id)
{
	CMouseCursor::Instance()->SetCursor(id);
}

int FrCursor::GetCursor()
{
	return CMouseCursor::Instance()->GetCursor();
}

void FrCursor::MoveCursor(const WPoint& point)
{
	CMouseCursor::Instance()->ForceMove(point.x, point.y, -1.0f);
}

void FrCursor::ShowCursor(bool bShow)
{
	m_bShow = bShow;
}
