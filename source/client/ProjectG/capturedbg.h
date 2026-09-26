#pragma once
#include "baseobject.h"
#include "singleton.h"

class CCapturedBg : public BaseObject, public WSingleton<CCapturedBg>
{
public:
	CCapturedBg();
	virtual ~CCapturedBg();
	void Render();
	void Capture();
	void Reset();
	bool IsCaptured() { return m_CaptureMode; }

protected:
	void SetCapturedBgMode(bool mode);

	bool m_CaptureMode;
};
