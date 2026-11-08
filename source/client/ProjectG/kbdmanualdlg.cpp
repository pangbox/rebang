#include "kbdmanualdlg.h"

IMPLEMENT_OBJECT(FrKbdManualDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrKbdManualDlg, FrForm)

ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrKbdManualDlg::OnCloseBtnUp)

END_FRESH_MSGMAP()

void FrKbdManualDlg::OnCloseBtnUp()
{
	OnFreshOkay();
}
