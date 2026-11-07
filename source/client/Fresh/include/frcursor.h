#pragma once
#include "../../Wangreal/include/wtypes.h"

class FrCursor
{
public:
	FrCursor();
	virtual ~FrCursor();

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

	void Init();
	void SetCursor(eCursor id);
	int GetCursor();
	void MoveCursor(const WPoint& point);
	void ShowCursor(bool bShow);

protected:
	int m_cursorIndex;
	bool m_bShow;
};
