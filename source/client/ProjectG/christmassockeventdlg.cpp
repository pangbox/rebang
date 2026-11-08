#include "christmassockeventdlg.h"
#include "frviewer.h"

IMPLEMENT_OBJECT(FrChristmasSockEventDescDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrChristmasSockEventDescDlg, FrForm)

ON_FRESH_VI("desc_view", FRCMD_INIT,
	FrChristmasSockEventDescDlg::OnInitDescView)

END_FRESH_MSGMAP()

FrChristmasSockEventDescDlg::FrChristmasSockEventDescDlg()
{
	m_pViewer = NULL;
}

FrChristmasSockEventDescDlg::~FrChristmasSockEventDescDlg()
{
}

void FrChristmasSockEventDescDlg::OnInitDescView(int param)
{
	m_pViewer = DYNAMIC_CAST(FrViewer, param);

	if (m_pViewer)
	{
		m_pViewer->ShowScrollBar(false);
		m_pViewer->Open("2010_Christmas_Event_Popup.jpg");
	}
}
