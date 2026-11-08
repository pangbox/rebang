#include "stdafx.h"
#include "guildformcontroller.h"
#include "guildactor.h"
#include "guilddefine.h"
#include "s5/utilities.h"

namespace _guild
{
	CGuildFormController::CGuildFormController()
		: m_formStyle(FORM_NONE)
	{
	}

	CGuildFormController::~CGuildFormController()
	{
		std::for_each(m_postActions.begin(), m_postActions.end(),
			Delete_SecondObject());
	}

	void CGuildFormController::ControllForm(eFormStyle style,
		eAuthority authority)
	{
		m_formStyle = style;
		std::for_each(m_controls.begin(), m_controls.end(),
			S5::DisableFreshWindow);
		for (PostActionIterator it = m_postActions.begin();
			it != m_postActions.end(); ++it)
		{
			sPostAction* action = (*it).second;
			if ((style & action->formStyle) && (authority & action->authority))
				(*action)(this);
		}
	}

	void CGuildFormController::InsertControl(FrWnd* wnd)
	{
		m_controls.push_back(wnd);
	}

	void CGuildFormController::ReadOnlywindow(sPostAction* action)
	{
		PostActionIterator it = m_postActions.find(action);
		if (it == m_postActions.end())
			return;
		if ((*it).second->type == GI_EDIT)
			static_cast<FrEdit*>(action->pWnd)->SetReadOnly(true);
	}

	void CGuildFormController::WritableWindow(sPostAction* action)
	{
		PostActionIterator it = m_postActions.find(action);
		if (it == m_postActions.end())
			return;
		if ((*it).second->type == GI_EDIT)
			static_cast<FrEdit*>(action->pWnd)->SetReadOnly(false);
	}

	void CGuildFormController::EnableWindow(sPostAction* action)
	{
		S5::EnableFreshWindow(action->pWnd, true);
	}
	void CGuildFormController::DisableWindow(sPostAction* action)
	{
		S5::EnableFreshWindow(action->pWnd, false);
	}

	void CGuildFormController::MoveLeftSide(sPostAction* action)
	{
		WRect rect = action->pWnd->GetRect();
		rect.x = 145.0f;
		action->pWnd->SetRect(rect);
	}

	void CGuildFormController::MoveRightSide(sPostAction* action)
	{
		WRect rect = action->pWnd->GetRect();
		rect.x = 375.0f;
		action->pWnd->SetRect(rect);
	}

	void CGuildFormController::ClearEdit(sPostAction* action)
	{
		PostActionIterator it = m_postActions.find(action);
		if (it == m_postActions.end())
			return;
		if ((*it).second->type == GI_EDIT)
			static_cast<FrEdit*>(action->pWnd)->ClearLine();
	}

	void CGuildFormController::VisibleWindow(sPostAction* action)
	{
		action->pWnd->SetVisible(true);
	}
	void CGuildFormController::InVisibleWindow(sPostAction* action)
	{
		action->pWnd->SetVisible(false);
	}

	void CPageArrow::RefreshArrowButton(int page, int total)
	{
		InitControl();
		if ((unsigned int)total > 5)
			m_state.Enable(1);
		if (((unsigned int)total - 1) / 5 < ((unsigned int)page - 1) / 5)
			m_state.Enable(2);
		if (m_state.GetFlag(1))
			m_pLeftArrow->Enable(true);
		if (m_state.GetFlag(2))
			m_pRightArrow->Enable(true);
	}

	void CPageArrow::InitControl()
	{
		m_state.Reset();
		m_pLeftArrow->Enable(false);
		m_pRightArrow->Enable(false);
	}

	int ConvertGuildHistory(GUILD_STATE_IDX state)
	{
		switch (state)
		{
		case 0:
			return 0;
		case 1:
			return 1061;
		case 2:
			return 1062;
		case 3:
			return 1063;
		case 4:
			return 1064;
		case 5:
			return 1065;
		case 6:
			return 1066;
		case 7:
			return 1067;
		case 8:
			return 1068;
		case 9:
			return 1069;
		case 10:
			return 1070;
		case 11:
			return 1071;
		case 12:
			return 1072;
		case 13:
			return 1073;
		case 51:
			return 1074;
		case 52:
			return 1075;
		case 53:
			return 1076;
		default:
			return 0;
		}
	}
}
