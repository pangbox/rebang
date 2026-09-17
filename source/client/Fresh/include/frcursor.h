#pragma once
#include "../../Wangreal/include/wtypes.h"

class FrCursor
{
public:
	enum eCursor
	{
		UNSELECT,
		NORMAL = 0,
		ROLL_OVER,
		SELECT,
		WRONG,
		UP,
		DOWN,
		ZOOM,
		INFO
	};

	FrCursor();
	virtual ~FrCursor();
	void Init();
	void SetCursor(eCursor id);
	int GetCursor();
	void MoveCursor(const WPoint& point);
	void ShowCursor(bool bShow);

private:
	int m_cursorIndex;
	bool m_bShow;
};
