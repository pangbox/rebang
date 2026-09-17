#pragma once
#include "baseobject.h"
#include "singleton.h"

class CMouseCursor : public BaseObject, public WSingleton<CMouseCursor>
{
public:
	enum
	{
		UNSELECT,
		ROLL_OVER,
		SELECT,
		WRONG,
		UP,
		DOWN,
		ZOOM
	};

	void Reset(bool active);
	void SetCursor(int cursor);
	int GetCursor();
	void ForceMove(float x, float y, float z);

	void SetMode(int mode) { m_mode = mode; }

private:
	char unknown_00[0x28];
	int m_mode;
};
