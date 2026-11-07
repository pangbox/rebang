#pragma once
#include "baseobject.h"
#include "singleton.h"
#include "commonutil.h"

class CCapturedBg : public CStackableSingleton<CCapturedBg>
{
public:
	CCapturedBg();
	virtual ~CCapturedBg() { }

	void Render();
	void Capture();
	void Reset();
	bool IsCaptured() { return m_CaptureMode; }

protected:
	void SetCapturedBgMode(bool mode);

	bool m_CaptureMode;
};
