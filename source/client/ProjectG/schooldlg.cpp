#include "schooldlg.h"
#include "frcombobox.h"
#include "wresrcmng.h"
#include "../../shared/token.h"

extern Fresh* g_pFresh;

std::string szJpSchoolType[] = { "\xc1\xdf\xc7\xd0", "\xb0\xed\xb1\xb3",
	"\xb4\xdc\xb4\xeb", "\xb4\xeb\xc7\xd0" };

IMPLEMENT_OBJECT(FrSchoolDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrSchoolDlg, FrForm)

ON_FRESH_VI("like", FRCMD_INIT, FrSchoolDlg::OnLikeInit)
ON_FRESH_BI("like", FRCMD_ENTERKEY, FrSchoolDlg::OnLikeEnterKey)
ON_FRESH_VI("school", FRCMD_INIT, FrSchoolDlg::OnSchoolInit)
ON_FRESH_VI("school", FRCMD_LBUTTONDOWN, FrSchoolDlg::OnSchoolBtnDown)
ON_FRESH_VV("search", FRCMD_LBUTTONUP, FrSchoolDlg::OnSearchBtnUp)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrSchoolDlg::OnOkBtnUp)

END_FRESH_MSGMAP()

FrSchoolDlg::FrSchoolDlg()
{
	m_schoolCode = 0;
	m_pType = NULL;
	m_pDist0 = NULL;
	m_pDist1 = NULL;
	m_pStaticDist1 = NULL;
	m_type = 0;
	m_dist0 = 0;
	m_dist1 = 0;
}

FrSchoolDlg::~FrSchoolDlg()
{
}

void FrSchoolDlg::OnLikeInit(int param)
{
	m_pLike = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pLike)
		m_pLike->SetKeyFocus(true);
}

bool FrSchoolDlg::OnLikeEnterKey(int param)
{
	OnSearchBtnUp();
	return false;
}

void FrSchoolDlg::OnSchoolInit(int param)
{
	m_pSchool = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrSchoolDlg::OnSchoolBtnDown(int param)
{
	FrListItem* item = (FrListItem*)param;
	std::string* name = (std::string*)item->pData;
	m_schoolCode = m_searchCode[item->no];
	m_schoolName = *name;
}

void FrSchoolDlg::OnSearchBtnUp()
{
	// HACK
	if (0)
		OnSchoolInit(0);
	if (!m_pLike || !m_pSchool)
		return;
	const char* like = m_pLike->GetLine(1, false);
	if (strlen(like) < 4)
		return;
	m_pSchool->ClearList();
	m_pSchool->ClearLine();
	cFile* file = g_resrcmng->GetCFile("school.txt", 0xffff);
	if (!file)
	{
		m_pSchool->AddString(like);
		m_pSchool->SetLine(1, like, 0, false, 0);
		return;
	}
	{
		char line[8192];
		char code[64];
		char name[128];
		int count = 0;
		while (file->Scan("%n", line))
		{
			int length = strlen(line);
			cTokenV token;
			token.Init(line, length);
			token.GetToken(code, " ", 1);
			token.GetToken(name, "\r\n", 2);
			if (strstr(name, like))
			{
				if (count < 10)
				{
					m_searchCode[count++] = atoi(code);
					m_pSchool->AddString(name);
					if (!m_pSchool->GetLineNum())
					{
						m_schoolCode = atoi(code);
						m_schoolName = name;
						m_pSchool->SetLine(1, name, 0, false, 0);
					}
				}
				else
				{
					m_schoolCode = 0;
					m_schoolName = "";
					m_pSchool->ClearList();
					m_pSchool->SetLine(1,
						"\xc1\xbb\xb4\xf5 \xc0\xda\xbc\xbc\xc7\xcf\xb0\xd4 \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4",
						0, false, 0);
					break;
				}
			}
		}
		CloseCFile(file);
		if (!m_pSchool->GetLineNum())
			m_pSchool->SetLine(1,
				"\xb8\xf1\xb7\xcf\xbf\xa1\xbc\xad \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
				0, false, 0);
	}
}

void FrSchoolDlg::OnOkBtnUp()
{
	if (!m_schoolCode)
	{
		m_schoolName = "";
		m_pSchool->ClearList();
	}
	OnFreshOkay();
}

void FrSchoolDlg::OnExampleInit(int param)
{
	FrStatic* control = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (control)
		control->SetVisible(false);
}

void FrSchoolDlg::OnTypeInit(int param)
{
	m_pType = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrSchoolDlg::OnTypeListLBtnDown(int param)
{
	const char* name = m_pType->GetLine(1, false);
	if (!strcmp(name, m_typeName.c_str()))
		return;
	m_type = 5;
	for (unsigned int i = 0; i < 4; ++i)
	{
		if (!strcmp(name, szJpSchoolType[i].c_str()))
		{
			m_type = i + 1;
			m_typeName = name;
			break;
		}
	}
	if (m_type > 4)
		return;
	m_dist0Name = "";
	m_dist1Name = "";
	m_schoolName = "";
	m_dist0 = 0;
	m_dist1 = 0;
	m_schoolCode = 0;
	if (m_pDist0)
	{
		m_pDist0->SetLine(1, "", 0, false, 0);
		m_pDist0->ClearList();
		for (std::list<sJpSchool>::iterator it = m_dist0List.begin();
			it != m_dist0List.end(); ++it)
			if ((((*it).code >> 28) & 7) == m_type)
				m_pDist0->AddString((*it).name.c_str());
	}
	if (m_pDist1)
	{
		m_pDist1->SetLine(1, "", 0, false, 0);
		m_pDist1->ClearList();
	}
	if (m_pSchool)
	{
		m_pSchool->SetLine(1, "", 0, false, 0);
		m_pSchool->ClearList();
	}
	m_dist1Name = "";
	m_schoolName = "";
	m_dist1 = 0;
	m_schoolCode = 0;
	if (m_type - 1 == 3 || m_type - 1 == 2)
	{
		if (m_pDist1)
			m_pDist1->SetVisible(false);
		if (m_pStaticDist1)
			m_pStaticDist1->SetVisible(false);
	}
	else
	{
		if (m_pDist1)
			m_pDist1->SetVisible(true);
		if (m_pStaticDist1)
			m_pStaticDist1->SetVisible(true);
	}
}

void FrSchoolDlg::OnDist0Init(int param)
{
	m_pDist0 = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrSchoolDlg::OnDist0ListLBtnDown(int param)
{
	const char* name = m_pDist0->GetLine(1, false);
	if (!strcmp(name, m_dist0Name.c_str()))
		return;
	std::list<sJpSchool>::iterator it;
	for (it = m_dist0List.begin(); it != m_dist0List.end(); ++it)
	{
		if ((((*it).code >> 28) & 7) == m_type &&
			!strcmp(name, (*it).name.c_str()))
		{
			m_dist0 = (*it).code;
			m_dist0Name = name;
			break;
		}
	}
	if (it == m_dist0List.end())
		return;
	if (m_type - 1 == 3 || m_type - 1 == 2)
	{
		m_schoolName = "";
		m_schoolCode = 0;
		if (m_pSchool)
		{
			m_pSchool->SetLine(1, "", 0, false, 0);
			m_pSchool->ClearList();
			for (it = m_schoolList.begin(); it != m_schoolList.end(); ++it)
				if ((((*it).code >> 28) & 7) == m_type &&
					((*it).code & 0xfe00000) == (m_dist0 & 0xfe00000))
					m_pSchool->AddString((*it).name.c_str());
		}
	}
	else
	{
		m_dist1Name = "";
		m_schoolName = "";
		m_dist1 = 0;
		m_schoolCode = 0;
		if (m_pDist1)
		{
			m_pDist1->SetLine(1, "", 0, false, 0);
			m_pDist1->ClearList();
			for (it = m_dist1List.begin(); it != m_dist1List.end(); ++it)
				if ((((*it).code >> 28) & 7) == m_type &&
					((*it).code & 0xfe00000) == (m_dist0 & 0xfe00000))
					m_pDist1->AddString((*it).name.c_str());
		}
		if (m_pSchool)
		{
			m_pSchool->SetLine(1, "", 0, false, 0);
			m_pSchool->ClearList();
		}
	}
}

void FrSchoolDlg::OnDist1Init(int param)
{
	m_pDist1 = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrSchoolDlg::OnDist1ListLBtnDown(int param)
{
	const char* name = m_pDist1->GetLine(1, false);
	if (!strcmp(name, m_dist1Name.c_str()))
		return;
	std::list<sJpSchool>::iterator it;
	for (it = m_dist1List.begin(); it != m_dist1List.end(); ++it)
	{
		if ((((*it).code >> 28) & 7) == m_type &&
			((*it).code & 0xfe00000) == (m_dist0 & 0xfe00000) &&
			!strcmp(name, (*it).name.c_str()))
		{
			m_dist1 = (*it).code;
			m_dist1Name = name;
			break;
		}
	}
	if (it == m_dist0List.end())
		return;
	m_schoolName = "";
	m_schoolCode = 0;
	if (m_pSchool)
	{
		m_pSchool->SetLine(1, "", 0, false, 0);
		m_pSchool->ClearList();
		for (it = m_schoolList.begin(); it != m_schoolList.end(); ++it)
			if ((((*it).code >> 28) & 7) == m_type &&
				((*it).code & 0xfe00000) == (m_dist0 & 0xfe00000) &&
				((*it).code & 0x1fe000) == (m_dist1 & 0x1fe000))
				m_pSchool->AddString((*it).name.c_str());
	}
}

void FrSchoolDlg::OnStaticDist1Init(int param)
{
	m_pStaticDist1 = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrSchoolDlg::OnSearchInit(int param)
{
	FrButton* control = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (control)
		control->SetVisible(false);
}

bool FrSchoolDlg::OnInit()
{
	if (m_pType)
		for (int i = 0; i < 4; ++i)
			m_pType->AddString(szJpSchoolType[i].c_str());
	cFile* file = g_resrcmng->GetCFile("school.txt", 0xffff);
	if (file)
	{
		int code;
		char name[1024];
		sJpSchool school;
		m_dist0List.clear();
		while (file->Scan("%d %s", &code, name))
		{
			school.code = code;
			school.name = name;
			if (code & 0x1fff)
				m_schoolList.push_back(school);
			else if (code & 0x1fe000)
				m_dist1List.push_back(school);
			else if (code & 0xfe00000)
				m_dist0List.push_back(school);
		}
		CloseCFile(file);
	}
	return true;
}
