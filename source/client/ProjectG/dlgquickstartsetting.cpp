#include "stdafx.h"
#include "dlgquickstartsetting.h"
extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrQuickResultDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrQuickResultDlg, FrForm)
ON_FRESH_VI("ok1", FRCMD_INIT, FrQuickResultDlg::OnOkBtnInit)
ON_FRESH_VV("ok1", FRCMD_LBUTTONUP, FrQuickResultDlg::OnOkBtnUp)
END_FRESH_MSGMAP()

FrQuickResultDlg::FrQuickResultDlg()
{
}

FrQuickResultDlg::~FrQuickResultDlg()
{
}

void FrQuickResultDlg::OnOkBtnInit(int param)
{
	m_pOkBtn = DYNAMIC_CAST(FrButton, param);
	if (m_pOkBtn)
		m_pOkBtn->Enable(false);
}

void FrQuickResultDlg::OnOkBtnUp()
{
	OnOK();
}

void FrQuickResultDlg::SetContents(const char* text)
{
	if (text)
		SetMessage(text, false);
}

void FrQuickResultDlg::SetOkbtnEnable(bool enable)
{
	if (m_pOkBtn)
		m_pOkBtn->Enable(enable);
}

IMPLEMENT_OBJECT(FrQuickStartDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrQuickStartDlg, FrForm)
ON_FRESH_VI("stroke", FRCMD_INIT, FrQuickStartDlg::OnStrokeBtnInit)
ON_FRESH_VV("stroke", FRCMD_LBUTTONUP, FrQuickStartDlg::OnStrokeBtnUp)
ON_FRESH_VI("strokestop", FRCMD_INIT, FrQuickStartDlg::OnStrokeStopInit)
ON_FRESH_VI("tournament", FRCMD_INIT, FrQuickStartDlg::OnTournamentBtnInit)
ON_FRESH_VV("tournament", FRCMD_LBUTTONUP, FrQuickStartDlg::OnTournamentBtnUp)
ON_FRESH_VI("tournamentstop", FRCMD_INIT, FrQuickStartDlg::OnTournamenStopInit)
ON_FRESH_VI("battle", FRCMD_INIT, FrQuickStartDlg::OnBattleBtnInit)
ON_FRESH_VV("battle", FRCMD_LBUTTONUP, FrQuickStartDlg::OnBattleBtnUp)
ON_FRESH_VI("battlestop", FRCMD_INIT, FrQuickStartDlg::OnBattleStopInit)
ON_FRESH_VI("chat", FRCMD_INIT, FrQuickStartDlg::OnChatBtnInit)
ON_FRESH_VV("chat", FRCMD_LBUTTONUP, FrQuickStartDlg::OnChatBtnUp)
ON_FRESH_VI("chatstop", FRCMD_INIT, FrQuickStartDlg::OnChatStopInit)
ON_FRESH_VI("option", FRCMD_INIT, FrQuickStartDlg::OnOptionBtnInit)
ON_FRESH_VV("option", FRCMD_LBUTTONUP, FrQuickStartDlg::OnOptionBtnUp)
END_FRESH_MSGMAP()

FrQuickStartDlg::FrQuickStartDlg()
{
	m_pSettingDlg = NULL;
}

FrQuickStartDlg::~FrQuickStartDlg()
{
}

bool FrQuickStartDlg::OnInit()
{
	{
		bool enable = CMatchingSystem::Instance()->CheckIntrestCode(0) != 0;
		if (m_pStrokeBtn)
		{
			m_pStrokeBtn->Enable(enable);
			m_pStrokeBtn->SetVisible(enable);
		}
		if (m_pStrokeStop)
		{
			if (enable)
				m_pStrokeStop->SetVisible(false);
			else
				m_pStrokeStop->SetVisible(true);
		}
	}
	{
		bool enable = CMatchingSystem::Instance()->CheckIntrestCode(4) != 0;
		if (m_pTournamentBtn)
		{
			m_pTournamentBtn->Enable(enable);
			m_pTournamentBtn->SetVisible(enable);
		}
		if (m_pTournamentStop)
		{
			if (enable)
				m_pTournamentStop->SetVisible(false);
			else
				m_pTournamentStop->SetVisible(true);
		}
	}
	{
		bool enable = CMatchingSystem::Instance()->CheckIntrestCode(10) != 0;
		if (m_pBattleBtn)
		{
			m_pBattleBtn->Enable(enable);
			m_pBattleBtn->SetVisible(enable);
		}
		if (m_pBattleStop)
		{
			if (enable)
				m_pBattleStop->SetVisible(false);
			else
				m_pBattleStop->SetVisible(true);
		}
	}
	{
		bool enable = CMatchingSystem::Instance()->CheckIntrestCode(2) != 0;
		if (m_pChatBtn)
		{
			m_pChatBtn->Enable(enable);
			m_pChatBtn->SetVisible(enable);
		}
		if (m_pChatStop)
		{
			if (enable)
				m_pChatStop->SetVisible(false);
			else
				m_pChatStop->SetVisible(true);
		}
	}
	return true;
}

void FrQuickStartDlg::OnStrokeStopInit(int param)
{
	m_pStrokeStop = DYNAMIC_CAST(FrArea, param);
}

void FrQuickStartDlg::OnTournamenStopInit(int param)
{
	m_pTournamentStop = DYNAMIC_CAST(FrArea, param);
}

void FrQuickStartDlg::OnBattleStopInit(int param)
{
	m_pBattleStop = DYNAMIC_CAST(FrArea, param);
}

void FrQuickStartDlg::OnChatStopInit(int param)
{
	m_pChatStop = DYNAMIC_CAST(FrArea, param);
}

void FrQuickStartDlg::OnStrokeBtnInit(int param)
{
	m_pStrokeBtn = DYNAMIC_CAST(FrButton, param);
}

void FrQuickStartDlg::OnStrokeBtnUp()
{
	if (m_pStrokeBtn->IsEnabled())
	{
		SendRequestMatching(0);
		OnOK();
	}
}

void FrQuickStartDlg::OnTournamentBtnInit(int param)
{
	m_pTournamentBtn = DYNAMIC_CAST(FrButton, param);
}

void FrQuickStartDlg::OnTournamentBtnUp()
{
	if (m_pTournamentBtn->IsEnabled())
	{
		SendRequestMatching(4);
		OnOK();
	}
}

void FrQuickStartDlg::OnBattleBtnInit(int param)
{
	m_pBattleBtn = DYNAMIC_CAST(FrButton, param);
}

void FrQuickStartDlg::OnBattleBtnUp()
{
	if (m_pBattleBtn->IsEnabled())
	{
		SendRequestMatching(10);
		OnOK();
	}
}

void FrQuickStartDlg::OnChatBtnInit(int param)
{
	m_pChatBtn = DYNAMIC_CAST(FrButton, param);
}

void FrQuickStartDlg::OnChatBtnUp()
{
	if (m_pChatBtn->IsEnabled())
	{
		SendRequestMatching(2);
		OnOK();
	}
}

void FrQuickStartDlg::OnOptionBtnInit(int param)
{
	m_pOptionBtn = DYNAMIC_CAST(FrButton, param);
}

void FrQuickStartDlg::OnOptionBtnUp()
{
	m_pSettingDlg = CreateForm<FrQuickStartSettingDlg>(g_pFresh->GetManager(),
		this, "quick_start_setting");
	if (m_pSettingDlg)
		m_pSettingDlg->Open(
			(FRESH_PFN_RESULT)&FrQuickStartDlg::OnQuickStartSettingDlgResult,
			WPoint(468.0f, 158.0f), FrRET_OK | FrRET_CANCEL);
}

bool FrQuickStartDlg::OnQuickStartSettingDlgResult(int result, FrForm* form)
{
	switch (result)
	{
	case FrOK:
		m_pSettingDlg->UpdateData();
		OnInit();
		break;
	}
	m_pSettingDlg = NULL;
	return true;
}

void FrQuickStartDlg::SendRequestMatching(unsigned char type)
{
	if (type != 16)
	{
		Doc()->SetUserInfoMod(3);
		WSendPacket packet((enumClientPacket)183);
		packet.Encode1(type);
		packet.Send(TO_GAME);
	}
}

IMPLEMENT_OBJECT(FrQuickStartSettingDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrQuickStartSettingDlg, FrForm)
ON_FRESH_VI("chk_stroke", FRCMD_INIT,
	FrQuickStartSettingDlg::OnChkStrokeBtnInit)
ON_FRESH_VV("chk_stroke", FRCMD_LBUTTONUP,
	FrQuickStartSettingDlg::OnChkStrokeBtnUp)
ON_FRESH_VI("chk_tournament", FRCMD_INIT,
	FrQuickStartSettingDlg::OnChkTournamentBtnInit)
ON_FRESH_VV("chk_tournament", FRCMD_LBUTTONUP,
	FrQuickStartSettingDlg::OnChkTournamentBtnUp)
ON_FRESH_VI("chk_battle", FRCMD_INIT,
	FrQuickStartSettingDlg::OnChkBattleBtnInit)
ON_FRESH_VV("chk_battle", FRCMD_LBUTTONUP,
	FrQuickStartSettingDlg::OnChkBattleBtnUp)
ON_FRESH_VI("chk_chat", FRCMD_INIT, FrQuickStartSettingDlg::OnChkChatBtnInit)
ON_FRESH_VV("chk_chat", FRCMD_LBUTTONUP, FrQuickStartSettingDlg::OnChkChatBtnUp)
ON_FRESH_VI("combo_condition_1", FRCMD_INIT,
	FrQuickStartSettingDlg::OnCmbCondition1BoxInit)
ON_FRESH_VI("combo_condition_2", FRCMD_INIT,
	FrQuickStartSettingDlg::OnCmbCondition2BoxInit)
ON_FRESH_VI("combo_condition_3", FRCMD_INIT,
	FrQuickStartSettingDlg::OnCmbCondition3BoxInit)
END_FRESH_MSGMAP()

FrQuickStartSettingDlg::FrQuickStartSettingDlg()
{
	for (int i = 0; i < 16; ++i)
		m_bCheck[i] = 0;
}

FrQuickStartSettingDlg::~FrQuickStartSettingDlg()
{
}

void FrQuickStartSettingDlg::OnChkStrokeBtnInit(int param)
{
	m_pChkStroke = DYNAMIC_CAST(FrButton, param);
	if (m_pChkStroke && CMatchingSystem::Instance())
	{
		if (CMatchingSystem::Instance()->CheckIntrestCode(0))
		{
			m_pChkStroke->SetStatus(FrButton::PRESSED);
			m_bCheck[0] = true;
		}
		else
			m_pChkStroke->SetStatus(FrButton::NORMAL);
	}
}

void FrQuickStartSettingDlg::OnChkStrokeBtnUp()
{
	if (CMatchingSystem::Instance())
	{
		if (m_pChkStroke)
			m_bCheck[0] = m_pChkStroke->GetStatus() == FrButton::PRESSED;
	}
}

void FrQuickStartSettingDlg::OnChkTournamentBtnInit(int param)
{
	m_pChkTournament = DYNAMIC_CAST(FrButton, param);
	if (m_pChkTournament && CMatchingSystem::Instance())
	{
		if (CMatchingSystem::Instance()->CheckIntrestCode(4))
		{
			m_pChkTournament->SetStatus(FrButton::PRESSED);
			m_bCheck[4] = true;
		}
		else
			m_pChkTournament->SetStatus(FrButton::NORMAL);
	}
}

void FrQuickStartSettingDlg::OnChkTournamentBtnUp()
{
	if (CMatchingSystem::Instance())
	{
		if (m_pChkTournament)
			m_bCheck[4] = m_pChkTournament->GetStatus() == FrButton::PRESSED;
	}
}

void FrQuickStartSettingDlg::OnChkBattleBtnInit(int param)
{
	m_pChkBattle = DYNAMIC_CAST(FrButton, param);
	if (m_pChkBattle && CMatchingSystem::Instance())
	{
		if (CMatchingSystem::Instance()->CheckIntrestCode(10))
		{
			m_pChkBattle->SetStatus(FrButton::PRESSED);
			m_bCheck[10] = true;
		}
		else
			m_pChkBattle->SetStatus(FrButton::NORMAL);
	}
}

void FrQuickStartSettingDlg::OnChkBattleBtnUp()
{
	if (CMatchingSystem::Instance())
	{
		if (m_pChkBattle)
			m_bCheck[10] = m_pChkBattle->GetStatus() == FrButton::PRESSED;
	}
}

void FrQuickStartSettingDlg::OnChkChatBtnInit(int param)
{
	m_pChkChat = DYNAMIC_CAST(FrButton, param);
	if (m_pChkChat && CMatchingSystem::Instance())
	{
		if (CMatchingSystem::Instance()->CheckIntrestCode(2))
		{
			m_pChkChat->SetStatus(FrButton::PRESSED);
			m_bCheck[2] = true;
		}
		else
			m_pChkChat->SetStatus(FrButton::NORMAL);
	}
}

void FrQuickStartSettingDlg::OnChkChatBtnUp()
{
	if (CMatchingSystem::Instance())
	{
		if (m_pChkChat)
			m_bCheck[2] = m_pChkChat->GetStatus() == FrButton::PRESSED;
	}
}

void FrQuickStartSettingDlg::OnCmbCondition1BoxInit(int param)
{
	m_pCondition[0] = DYNAMIC_CAST(FrComboBox, param);
	if (!m_pCondition[0])
		return;
	CMatchingSystem* matching = CMatchingSystem::Instance();
	if (!matching)
		return;
	m_pCondition[0]->AddString(
		"\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4");
	std::vector<sIntrest> interests;
	int result = matching->GetDisplayIntrests("\xb0\xd4\xc0\xd3", &interests);
	if (!result || interests.empty())
	{
		m_pCondition[0]->SetLine(1,
			"\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4", 0, 0, 0);
		return;
	}

	std::vector<sIntrest>::iterator it;
	for (it = interests.begin(); it != interests.end(); ++it)
		m_pCondition[0]->AddString(it->name);
	m_pCondition[0]->SetLine(1,
		matching->GetNameChecked("\xb0\xd4\xc0\xd3").c_str(), 0, 0, 0);
}

void FrQuickStartSettingDlg::OnCmbCondition2BoxInit(int param)
{
	m_pCondition[1] = DYNAMIC_CAST(FrComboBox, param);
	if (!m_pCondition[1])
		return;
	CMatchingSystem* matching = CMatchingSystem::Instance();
	if (!matching)
		return;
	m_pCondition[1]->AddString(
		"\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4");
	std::vector<sIntrest> interests;
	int result = matching->GetDisplayIntrests("\xc3\xeb\xb9\xcc", &interests);
	if (!result || interests.empty())
	{
		m_pCondition[0]->SetLine(1,
			"\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4", 0, 0, 0);
		return;
	}

	std::vector<sIntrest>::iterator it;
	for (it = interests.begin(); it != interests.end(); ++it)
		m_pCondition[1]->AddString(it->name);
	m_pCondition[1]->SetLine(1,
		matching->GetNameChecked("\xc3\xeb\xb9\xcc").c_str(), 0, 0, 0);
}

void FrQuickStartSettingDlg::OnCmbCondition3BoxInit(int param)
{
	m_pCondition[2] = DYNAMIC_CAST(FrComboBox, param);
	if (!m_pCondition[2])
		return;
	CMatchingSystem* matching = CMatchingSystem::Instance();
	if (!matching)
		return;
	m_pCondition[2]->AddString(
		"\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4");
	std::vector<sIntrest> interests;
	int result = matching->GetDisplayIntrests("\xbf\xac\xbe\xd6", &interests);
	if (!result || interests.empty())
	{
		m_pCondition[0]->SetLine(1,
			"\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4", 0, 0, 0);
		return;
	}

	std::vector<sIntrest>::iterator it;
	for (it = interests.begin(); it != interests.end(); ++it)
		m_pCondition[2]->AddString(it->name);
	m_pCondition[2]->SetLine(1,
		matching->GetNameChecked("\xbf\xac\xbe\xd6").c_str(), 0, 0, 0);
}

void FrQuickStartSettingDlg::UpdateData()
{
	CMatchingSystem* matching = CMatchingSystem::Instance();
	if (matching)
	{
		std::vector<unsigned char> interests;
		for (int i = 0; i < 16; ++i)
			if (m_bCheck[i])
				interests.push_back(i);
		if (strcmp(m_pCondition[0]->GetLine(1, false),
				"\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4"))
		{
			unsigned char code = matching->GetDisplayIntrestCode(
				"\xb0\xd4\xc0\xd3", m_pCondition[0]->GetLine(1, false));
			if (code != 255)
				interests.push_back(code);
		}
		if (strcmp(m_pCondition[1]->GetLine(1, false),
				"\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4"))
		{
			unsigned char code = matching->GetDisplayIntrestCode(
				"\xc3\xeb\xb9\xcc", m_pCondition[1]->GetLine(1, false));
			if (code != 255)
				interests.push_back(code);
		}
		if (strcmp(m_pCondition[2]->GetLine(1, false),
				"\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4"))
		{
			unsigned char code = matching->GetDisplayIntrestCode(
				"\xbf\xac\xbe\xd6", m_pCondition[2]->GetLine(1, false));
			if (code != 255)
				interests.push_back(code);
		}
		matching->ClearMyIntrests();
		unsigned char count = interests.size();
		WSendPacket packet((enumClientPacket)182);
		packet.Encode1(count);
		for (int i = 0; i < count; ++i)
		{
			packet.Encode1(interests[i]);
			matching->SetMyIntrest(interests[i]);
		}
		packet.Encode1(1);
		packet.Send(TO_GAME);
	}
}

CMatchingSystem::CMatchingSystem()
{
}

CMatchingSystem::~CMatchingSystem()
{
}

void CMatchingSystem::Clear()
{
	m_myIntrests.clear();
}

int CMatchingSystem::SetDisplayIntrest(const sIntrest& interest)
{
	m_intrests.push_back(interest);
	return true;
}

const char* CMatchingSystem::GetGroup(unsigned char code)
{
	std::vector<sIntrest>::iterator it;
	for (it = m_intrests.begin(); it != m_intrests.end(); ++it)
		if (it->code == code)
			return it->group;
	return NULL;
}

const sIntrest* CMatchingSystem::GetIntrestInfo(unsigned char code)
{
	const sIntrest* result = NULL;
	std::vector<sIntrest>::iterator it;
	for (it = m_intrests.begin(); it != m_intrests.end(); ++it)
		if (it->code == code)
		{
			result = &*it;
			break;
		}
	return result;
}

unsigned char CMatchingSystem::GetDisplayIntrestCode(const std::string& group,
	const std::string& name)
{
	std::vector<sIntrest>::iterator it;
	unsigned char result = 255;
	for (it = m_intrests.begin(); it != m_intrests.end(); ++it)
		if (!strcmp(group.c_str(), it->group) &&
			!strcmp(name.c_str(), it->name))
		{
			result = it->code;
			break;
		}
	return result;
}

int CMatchingSystem::SetMyIntrest(unsigned char code)
{
	m_myIntrests.push_back(code);
	return true;
}

int CMatchingSystem::CheckIntrestCode(unsigned char code)
{
	std::vector<unsigned char>::iterator it =
		std::find(m_myIntrests.begin(), m_myIntrests.end(), code);
	return it != m_myIntrests.end() ? 1 : 0;
}

std::string CMatchingSystem::GetNameChecked(const std::string& group)
{
	unsigned char count = m_myIntrests.size();
	if (count)
	{
		for (unsigned char i = 0; i < count; ++i)
		{
			const sIntrest* interest = GetIntrestInfo(m_myIntrests[i]);
			if (interest && !strcmp(group.c_str(), interest->group))
				return interest->name;
		}
	}
	return "\xc1\xb6\xb0\xc7\xbc\xb3\xc1\xa4\xbe\xc8\xc7\xd4";
}

int CMatchingSystem::GetDisplayIntrests(const std::string& group,
	std::vector<sIntrest>* out)
{
	sIntrest interest = { 0 };
	int result = false;
	std::vector<sIntrest>::iterator it;
	for (it = m_intrests.begin(); it != m_intrests.end(); ++it)
		if (!strcmp(group.c_str(), it->group))
		{
			out->push_back(*it);
			result = true;
		}
	return result;
}
