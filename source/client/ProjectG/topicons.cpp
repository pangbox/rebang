#include "frbutton.h"
#include "actor.h"
#include "gatewayactor.h"
#include "topicons.h"
#include "mathconsts.h"

void CWizcityOpenEvent::OnInit(FrButton& button)
{
	button.SetButtonImg("hat_event_top_n", FrButton::NORMAL);
	button.SetButtonImg("hat_event_top_o", FrButton::OVER);
	button.SetButtonImg("hat_event_top_o", FrButton::PRESSED);
	button.SetButtonImg("hat_event_top_o", FrButton::BLINK);
}

void CWizcityOpenEvent::OnButtonDown()
{
	AfxGetTask()->GetActor(s_gatewayActor[1])
		<< MsgObject(NULL, 0x26f, 0xd, 0, 0, 0, 0);
}
