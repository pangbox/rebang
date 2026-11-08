#include "notifyform.h"

IMPLEMENT_OBJECT(FrNotifyForm, FrForm)

BEGIN_FRESH_MSGMAP(FrNotifyForm, FrForm)

ON_FRESH_VV("ok_btn", FRCMD_LBUTTONUP, FrNotifyForm::OnFreshOkay)
ON_FRESH_VV("cancel_btn", FRCMD_LBUTTONUP, FrNotifyForm::OnFreshCancel)
ON_FRESH_VV("yes_btn", FRCMD_LBUTTONUP, FrNotifyForm::OnFreshYes)
ON_FRESH_VV("no_btn", FRCMD_LBUTTONUP, FrNotifyForm::OnFreshNo)

END_FRESH_MSGMAP()

FrNotifyForm::FrNotifyForm()
{
}

FrNotifyForm::~FrNotifyForm()
{
}

void FrNotifyForm::OnFreshOkay()
{
	if (!(m_flag & FrNOSOUND))
		g_audio->PlaySfx("ui_button_ok_click");

	Close(FrOK, true);
}

void FrNotifyForm::OnFreshCancel()
{
	if (!(m_flag & FrNOSOUND))
		g_audio->PlaySfx("ui_button_cancel_click");

	Close(FrCANCEL, true);
}

void FrNotifyForm::OnFreshYes()
{
	if (!(m_flag & FrNOSOUND))
		g_audio->PlaySfx("ui_button_ok_click");

	Close(FrYES, true);
}

void FrNotifyForm::OnFreshNo()
{
	if (!(m_flag & FrNOSOUND))
		g_audio->PlaySfx("ui_button_cancel_click");

	Close(FrNO, true);
}
