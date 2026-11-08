#include "guildnoticedlg.h"
#include "newsreader.h"
#include "frbutton.h"
#include "fredit.h"
#include "frscrollbar.h"
#include "uwin.h"

IMPLEMENT_OBJECT(FrGuildNoticeDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrGuildNoticeDlg, FrForm)

ON_FRESH_VI("close", FRCMD_INIT, FrGuildNoticeDlg::OnClose_Init)
ON_FRESH_VI("index", FRCMD_INIT, FrGuildNoticeDlg::OnIndex_Init)
ON_FRESH_VI("article", FRCMD_INIT, FrGuildNoticeDlg::OnArticle_Init)
ON_FRESH_VI("prev", FRCMD_INIT, FrGuildNoticeDlg::OnPrev_Init)
ON_FRESH_VI("next", FRCMD_INIT, FrGuildNoticeDlg::OnNext_Init)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrGuildNoticeDlg::OnClose_LBtnUp)
ON_FRESH_VV("prev", FRCMD_LBUTTONUP, FrGuildNoticeDlg::OnPrev_LBtnUp)
ON_FRESH_VV("next", FRCMD_LBUTTONUP, FrGuildNoticeDlg::OnNext_LBtnUp)

END_FRESH_MSGMAP()

FrGuildNoticeDlg::FrGuildNoticeDlg()
{
	m_pClose = NULL;
	m_pIndex = NULL;
	m_pArticle = NULL;
	m_pPrev = NULL;
	m_pNext = NULL;
	m_curArticle = -1;
}

FrGuildNoticeDlg::~FrGuildNoticeDlg()
{
}

void FrGuildNoticeDlg::DataBind(const NewsReader* reader, unsigned int index)
{
	if (reader == NULL)
		return;

	m_pReader = reader;
	SetArticle(index);
}

void FrGuildNoticeDlg::SetArticle(unsigned int index)
{
	if (m_pReader == NULL || m_pClose == NULL || m_pArticle == NULL ||
		m_pPrev == NULL || m_pNext == NULL)
		return;

	if (m_pReader->GetNumArticles() == 0)
	{
		m_pIndex->ClearLine();
		m_pArticle->AddLine(
			"\xb1\xe6\xb5\xe5 \xb0\xf8\xc1\xf6\xbb\xe7\xc7\xd7\xc0\xbb \xbe\xf2\xbe\xee\xbf\xc0\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, false);
		m_pPrev->SetVisible(false);
		m_pNext->SetVisible(false);
		return;
	}

	if (index >= m_pReader->GetNumArticles())
		return;

	if (m_curArticle == index)
		return;

	m_curArticle = index;

	m_pPrev->Enable(index != 0);
	m_pNext->Enable(index != m_pReader->GetNumArticles() - 1);

	m_pIndex->ClearLine();
	m_pIndex->AddLine(
		MakeStr("%d / %d", index + 1, m_pReader->GetNumArticles()), 0, false);

	std::string text;
	m_pArticle->ClearLine();
	const NewsReader::sArticle* pArticle = m_pReader->GetArticle(index);

	text = "\\cT\\c\\c0xffff0000\\c" + pArticle->title + "\\c0xff000000\\c";
	m_pArticle->AddText(text.c_str(), false, true);

	text = "\\cT\\c" + pArticle->userId + " / " + pArticle->regDate;
	m_pArticle->AddText(text.c_str(), false, true);

	text = pArticle->content;
	UWIN::AlterChar(text, ';', '\n');
	text = "\n\\cN\\c" + text;
	m_pArticle->AddText(text.c_str(), false, true);

	m_pArticle->GetScrollBar()->ScrollToFirst();
}

void FrGuildNoticeDlg::Prev()
{
	OnPrev_LBtnUp();
}

void FrGuildNoticeDlg::Next()
{
	OnNext_LBtnUp();
}

void FrGuildNoticeDlg::OnClose_Init(int param)
{
	m_pClose = DYNAMIC_CAST(FrButton, param);
}

void FrGuildNoticeDlg::OnIndex_Init(int param)
{
	m_pIndex = DYNAMIC_CAST(FrEdit, param);
}

void FrGuildNoticeDlg::OnArticle_Init(int param)
{
	m_pArticle = DYNAMIC_CAST(FrEdit, param);
}

void FrGuildNoticeDlg::OnPrev_Init(int param)
{
	m_pPrev = DYNAMIC_CAST(FrButton, param);
}

void FrGuildNoticeDlg::OnNext_Init(int param)
{
	m_pNext = DYNAMIC_CAST(FrButton, param);
}

void FrGuildNoticeDlg::OnClose_LBtnUp()
{
	Close(FrNONE, true);
}

void FrGuildNoticeDlg::OnPrev_LBtnUp()
{
	SetArticle(m_curArticle - 1);
}

void FrGuildNoticeDlg::OnNext_LBtnUp()
{
	SetArticle(m_curArticle + 1);
}
