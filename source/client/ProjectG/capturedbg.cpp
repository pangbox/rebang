#include "capturedbg.h"
#include "projectg.h"

CCapturedBg::CCapturedBg()
	: m_CaptureMode(false)
{
}

void CCapturedBg::Render()
{
}

void CCapturedBg::Reset()
{
	if (m_CaptureMode)
		SetCapturedBgMode(false);
}

void CCapturedBg::Capture()
{
	SetCapturedBgMode(true);
}

void CCapturedBg::SetCapturedBgMode(bool mode)
{
	if (mode == true)
	{
		if (m_CaptureMode)
			return;

		if (g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURED_BG, 1, 0) ==
			1)
			m_CaptureMode = true;

		CProjectG::Instance()->m_mainFlags.Enable(2);
		CProjectG::Instance()->SetFPS(
			g_resrcmng->VideoReference()->GetMonitorSupportFPS());
	}
	else
	{
		g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURED_BG, 0, 0);

		m_CaptureMode = false;

		CProjectG::Instance()->m_mainFlags.Disable(2);
	}
}
