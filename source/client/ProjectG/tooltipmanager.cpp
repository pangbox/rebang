#include "fresh.h"
#include "frwndmanager.h"
#include "tooltipmanager.h"

extern Fresh* g_pFresh;
CToolTipManager::CToolTipManager()
{
}

CToolTipManager::~CToolTipManager()
{
	TOOLTIP()->ClearToolTip();
}

void CToolTipManager::Add(FrWnd** ppWnd, eBarButton tip)
{
	sToolTip toolTip;
	toolTip.ppWnd = ppWnd;
	toolTip.tip = tip;

	m_toolTips.push_back(toolTip);
}

void CToolTipManager::Process()
{
	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();

	bool bShow = false;

	for (unsigned int i = 0; i < m_toolTips.size(); ++i)
	{
		WRect rect = (*m_toolTips[i].ppWnd)->GetRect();

		if (rect.x <= mousePos.x && mousePos.x <= rect.x + rect.w &&
			rect.y <= mousePos.y && mousePos.y <= rect.y + rect.h)
		{
			bShow = true;
			TOOLTIP()->SetToolTip(m_toolTips[i].tip);
		}
	}

	if (!bShow)
		TOOLTIP()->ClearToolTip();
	else
		TOOLTIP()->Render();
}
