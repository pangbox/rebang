#include "quizdlg.h"
#include "frlistbox.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"

extern Fresh* g_pFresh;

static sQuiz s_quizList[] = {

	{ "\xc0\xcc\xc1\xdf\xbf\xa1\xbc\xad \xb0\xa1\xc0\xe5 \xc1\xc1\xc0\xba \xc1\xa1\xbc\xf6\xb0\xa1 \xb9\xab\xbe\xf9\xc0\xcf\xb1\xee?",
     { { true, "Albatross." }, { false, "Boggie" }, { false, "Par" },
			{ false, "Dobble Boggie" }, { false, "Eagle" } } },
	{ "\xc0\xcc\xc1\xdf\xbf\xa1\xbc\xad \xbc\xa6\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xc1\xf6\xc7\xfc\xc0\xba \xb9\xab\xbe\xf9\xc0\xcf\xb1\xee?",
     { { true, "O.B." }, { false, "Ice" }, { false, "Bunker" },
			{ false, "Fairway" }, { false, "Rough" } }       },
};

inline bool AnswerCard::WriteAnswer(eCheckNum num)
{
	if (m_bMultiCheck == true)
	{
		m_answer ^= num;
	}
	else
	{
		DeleteAnswer(CHECK_NONE);
		m_answer = num;
	}

	return false;
}

bool AnswerCard::IsChecked(eCheckNum num) const

{
	bool bRet = false;

	if (m_answer & num)
	{
		bRet = true;
	}

	return bRet;
}

int AnswerCard::ISAllCheck() const
{
	if (IsChecked(CHECK_1))
		return 1;
	if (IsChecked(CHECK_2))
		return 2;

	if (IsChecked(CHECK_3))
		return 3;
	if (IsChecked(CHECK_4))
		return 4;

	return 0;
}

unsigned char AnswerCard::GetAnswerCard() const
{
	return m_answer;
}

void AnswerCard::DeleteAnswer(eCheckNum num)
{
	if (num == CHECK_NONE)
	{
		m_answer = num;
	}
	else
	{
		if (IsChecked(num))
		{
			m_answer ^= num;
		}
	}
}

IMPLEMENT_OBJECT(FrQuizDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrQuizDlg, FrForm)

ON_FRESH_VI("cases", FRCMD_INIT, FrQuizDlg::OnCasesInit)
ON_FRESH_VI("cases", FRCMD_OWNERDRAW, FrQuizDlg::OnCasesOwnerDraw)
ON_FRESH_VV("cases", FRCMD_LBUTTONDOWN, FrQuizDlg::OnCasesBtnDown)
ON_FRESH_VI("button_quiz1", FRCMD_INIT, FrQuizDlg::OnBQuiz1Init)
ON_FRESH_VV("button_quiz1", FRCMD_LBUTTONDOWN, FrQuizDlg::OnBQuiz1BtnDown)
ON_FRESH_VI("button_quiz2", FRCMD_INIT, FrQuizDlg::OnBQuiz2Init)
ON_FRESH_VV("button_quiz2", FRCMD_LBUTTONDOWN, FrQuizDlg::OnBQuiz2BtnDown)
ON_FRESH_VI("button_quiz3", FRCMD_INIT, FrQuizDlg::OnBQuiz3Init)
ON_FRESH_VV("button_quiz3", FRCMD_LBUTTONDOWN, FrQuizDlg::OnBQuiz3BtnDown)
ON_FRESH_VI("button_quiz4", FRCMD_INIT, FrQuizDlg::OnBQuiz4Init)
ON_FRESH_VV("button_quiz4", FRCMD_LBUTTONDOWN, FrQuizDlg::OnBQuiz4BtnDown)

END_FRESH_MSGMAP()

FrQuizDlg::FrQuizDlg()
{
	m_pCases = NULL;
	m_pQuiz = NULL;
	m_answer = -1;

	m_reserved = 0;
	m_bRefresh = false;
	memset(m_pBQuiz, 0, sizeof(m_pBQuiz));
	m_card.SetMode(false);
	m_card.DeleteAnswer(AnswerCard::CHECK_NONE);
}

void FrQuizDlg::OnBQuiz1Init(int param)
{
	m_pBQuiz[0] = DYNAMIC_CAST(FrButton, param);

	if (m_pBQuiz[0])
	{
		m_pBQuiz[0]->SetPushDelay(0.0f);
	}
}

void FrQuizDlg::OnBQuiz1BtnDown()
{
	m_card.WriteAnswer(AnswerCard::CHECK_1);
	m_bRefresh = true;
}

void FrQuizDlg::OnBQuiz2Init(int param)
{
	m_pBQuiz[1] = DYNAMIC_CAST(FrButton, param);

	if (m_pBQuiz[1])
	{
		m_pBQuiz[1]->SetPushDelay(0.0f);
	}
}

void FrQuizDlg::OnBQuiz2BtnDown()
{
	m_card.WriteAnswer(AnswerCard::CHECK_2);
	m_bRefresh = true;
}

void FrQuizDlg::OnBQuiz3Init(int param)
{
	m_pBQuiz[2] = DYNAMIC_CAST(FrButton, param);

	if (m_pBQuiz[2])
	{
		m_pBQuiz[2]->SetPushDelay(0.0f);
	}
}

void FrQuizDlg::OnBQuiz3BtnDown()
{
	m_card.WriteAnswer(AnswerCard::CHECK_3);
	m_bRefresh = true;
}

void FrQuizDlg::OnBQuiz4Init(int param)
{
	m_pBQuiz[3] = DYNAMIC_CAST(FrButton, param);

	if (m_pBQuiz[3])
	{
		m_pBQuiz[3]->SetPushDelay(0.0f);
	}
}

void FrQuizDlg::OnBQuiz4BtnDown()
{
	m_card.WriteAnswer(AnswerCard::CHECK_4);
	m_bRefresh = true;
}

void FrQuizDlg::OnCasesInit(int param)

{
	m_pCases = DYNAMIC_CAST(FrListBox, param);
}

void FrQuizDlg::OnCasesOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (!pDevice)
		return;

	if (!m_pQuiz)
		return;

	int index = (int)pItem->pData;

	pDevice->SetTextColor(0xff000000, 0xffffffff);

	pDevice->SetTextStyle(1);

	if (pItem->selected)

		pDevice->SetTextColor(0xffff0000, 0xffffffff);

	else if (pItem->underCursor)
	{
		pDevice->SetTextStyle(4);
		pDevice->SetTextColor(0xff000000, 0xffffffff);
	}

	const char* szText = m_pQuiz->cases[index].text.c_str();

	pDevice->Print(WPoint(pItem->pos.x + 10.0f, pItem->pos.y + 7.0f), 0,
		"%d. %s", pItem->idx, szText);

	pDevice->SetTextStyle(0);
}

void FrQuizDlg::OnCasesBtnDown()
{
	if (m_pCases)
	{
		FrListItem* pItem = m_pCases->GetItemUnderCursor();
		if (pItem)
		{
			m_answer = (int)pItem->pData;
		}
	}
}

void FrQuizDlg::SetupQuiz(int index)
{
	m_answer = index == 0 ? 3 : 1;

	if (!m_pQuiz)
		return;

	m_caseIndex[0] = 1;
	m_caseIndex[1] = 2;
	m_caseIndex[2] = 3;
	m_caseIndex[3] = 4;

	m_caseIndex[rand() % 4] = 0;

	if (m_pCases)
	{
		for (int i = 0; i < 4; i++)
			m_pCases->AddItem((void*)m_caseIndex[i]);
	}
}

const char* FrQuizDlg::GetQuestion()

{
	if (m_pQuiz)

		return m_pQuiz->question.c_str();

	return NULL;
}

bool FrQuizDlg::IsCorrectAnswer()
{
	int answer = m_card.ISAllCheck();

	if (m_answer == answer)
		return true;

	return false;
}

void FrQuizDlg::OnProc(const float delta)
{
	if (m_bRefresh)
	{
		ProcessChkBox();
		m_bRefresh = false;
	}
}

void FrQuizDlg::ProcessChkBox()
{
	if (m_card.IsChecked(AnswerCard::CHECK_1))
		m_pBQuiz[0]->SetButtonImg("tutorial_btn_check_d", FrButton::NORMAL);
	else
		m_pBQuiz[0]->SetButtonImg("tutorial_btn_check_n", FrButton::NORMAL);
	if (m_card.IsChecked(AnswerCard::CHECK_2))
		m_pBQuiz[1]->SetButtonImg("tutorial_btn_check_d", FrButton::NORMAL);
	else
		m_pBQuiz[1]->SetButtonImg("tutorial_btn_check_n", FrButton::NORMAL);
	if (m_card.IsChecked(AnswerCard::CHECK_3))
		m_pBQuiz[2]->SetButtonImg("tutorial_btn_check_d", FrButton::NORMAL);
	else
		m_pBQuiz[2]->SetButtonImg("tutorial_btn_check_n", FrButton::NORMAL);
	if (m_card.IsChecked(AnswerCard::CHECK_4))
		m_pBQuiz[3]->SetButtonImg("tutorial_btn_check_d", FrButton::NORMAL);
	else
		m_pBQuiz[3]->SetButtonImg("tutorial_btn_check_n", FrButton::NORMAL);
}
