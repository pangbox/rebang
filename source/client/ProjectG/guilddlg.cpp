#include "guilddlg.h"
#include "frlistbox.h"
#include "fredit.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include <WinInet.h>

extern Fresh* g_pFresh;

struct sRSSItem
{
	char* title;
	char* description;
};

IMPLEMENT_OBJECT(FrGuildDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrGuildDlg, FrForm)

ON_FRESH_VI("board", FRCMD_INIT, FrGuildDlg::OnBoardInit)
ON_FRESH_VV("board", FRCMD_LBUTTONUP, FrGuildDlg::OnBoardBtnUp)
ON_FRESH_VV("board", FRCMD_LBUTTONDOWN, FrGuildDlg::OnBoardBtnDown)
ON_FRESH_VI("board", FRCMD_OWNERDRAW, FrGuildDlg::OnBoardOwnerDraw)
ON_FRESH_VI("view", FRCMD_INIT, FrGuildDlg::OnViewInit)
ON_FRESH_VV("view", FRCMD_LBUTTONUP, FrGuildDlg::OnViewBtnUp)

END_FRESH_MSGMAP()

FrGuildDlg::FrGuildDlg()
{
	m_pBoard = NULL;
	m_pView = NULL;
}

FrGuildDlg::~FrGuildDlg()
{
	UnloadRSS();
}

bool FrGuildDlg::OnInit()
{
	if (m_pBoard == NULL)
		return FrForm::OnInit();

	LoadRSS("index2.xml");

	m_pBoard->SelectItem(*m_pBoard->m_itemList.begin(), true);
	SelectTitle(*m_pBoard->m_itemList.begin());

	return FrForm::OnInit();
}

void FrGuildDlg::OnBoardInit(int param)
{
	m_pBoard = DYNAMIC_CAST(FrListBox, param);
}

void FrGuildDlg::OnBoardBtnUp()
{
}

void FrGuildDlg::OnBoardOwnerDraw(int param)
{
	if (param == 0)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	FrListItem* pItem = (FrListItem*)param;
	sRSSItem* pData = (sRSSItem*)pItem->pData;
	if (m_pBoard->GetSelected() == pItem)
	{
		WRect rect;

		m_pBoard->GetClientRect(rect);
		pGDI->Box(WRect(pItem->pos.x + 4.0f, pItem->pos.y, rect.w, 16.0f),
			0x999b9dff);
	}

	if (pItem->underCursor)
	{
		pGDI->SetTextColor(0xffffffff, 0xff808080);
		pGDI->SetTextStyle(2);
	}
	else
	{
		pGDI->SetTextColor(0xff000000, 0xffffffff);
		pGDI->SetTextStyle(0);
	}
	pGDI->Print(pItem->pos, 0, pData->title);
}

void FrGuildDlg::OnBoardBtnDown()
{
	FrListItem* pItem = m_pBoard->GetItemUnderCursor();
	if (pItem)
	{
		m_pBoard->SelectItem(pItem, true);
		SelectTitle(pItem);
	}
	else
	{
		m_pBoard->SelectItem(NULL, true);
	}
}

void FrGuildDlg::OnViewInit(int param)
{
	m_pView = DYNAMIC_CAST(FrEdit, param);
}

void FrGuildDlg::OnViewBtnUp()
{
}

bool FrGuildDlg::DownLoad(const char* localFile, const char* remoteFile,
	const char* localPath, const char* remotePath)
{
	char szUrl[MAX_PATH];
	char szFile[MAX_PATH];
	DWORD dwSize;
	DWORD dwRead;

	sprintf(szUrl, "%s%s", remotePath, remoteFile);
	sprintf(szFile, "%s%s", localPath, localFile);

	HINTERNET hInternet = InternetOpen("cHttp DownLoad",
		INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInternet == NULL)
		return false;

	HINTERNET hUrl = InternetOpenUrl(hInternet, szUrl, NULL, 0,
		INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if (hUrl == NULL)
	{
		InternetCloseHandle(hInternet);
		return false;
	}

	FILE* fp = fopen(szFile, "wb");
	if (fp == NULL)
		return false;

	InternetQueryDataAvailable(hUrl, &dwSize, 0, 0);
	char* pBuf = new char[dwSize];
	do
	{
		InternetReadFile(hUrl, pBuf, dwSize, &dwRead);
		fwrite(pBuf, dwRead, 1, fp);
	} while (dwRead);

	delete[] pBuf;
	fclose(fp);

	InternetCloseHandle(hUrl);
	InternetCloseHandle(hInternet);

	return true;
}

void FrGuildDlg::UnloadRSS()
{
	for (std::list<FrListItem*>::iterator it = m_pBoard->m_itemList.begin();
		it != m_pBoard->m_itemList.end(); ++it)
	{
		FrListItem* pListItem = *it;
		sRSSItem* pItem = (sRSSItem*)pListItem->pData;
		delete[] pItem->title;
		delete[] pItem->description;
		delete pItem;
	}
}

void FrGuildDlg::SelectTitle(FrListItem* pItem)
{
	m_pView->ClearLine();

	m_pView->AddText(((sRSSItem*)pItem->pData)->description, false, true);
}

void FrGuildDlg::ParseRSS(TiXmlDocument& doc)
{
	TiXmlNode* pNode = doc.FirstChild("rss");
	if (pNode == NULL)
		return;

	pNode = pNode->FirstChild("channel");
	if (pNode == NULL)
		return;

	pNode = pNode->FirstChild("item");
	while (pNode)
	{
		sRSSItem* pItem = new sRSSItem;

		TiXmlNode* pTitle = pNode->FirstChild("title");
		const char* title = pTitle->FirstChild()->Value();

		int len = strlen(title);
		pItem->title = new char[len + 1];
		strcpy(pItem->title, title);

		TiXmlNode* pDesc = pNode->FirstChild("description");
		const char* desc = pDesc->FirstChild()->Value();

		len = strlen(desc);
		pItem->description = new char[len + 1];
		strcpy(pItem->description, desc);

		m_pBoard->AddItem(pItem);

		pNode = pNode->NextSibling("item");
	}
}

void FrGuildDlg::LoadRSS(const char* filename)
{
	TiXmlDocument doc;

	DownLoad("index.xml", "http://kwanny.ntreev.net/tt/index.xml", "", "");

	FILE* fp = fopen("index.xml", "rb");
	if (fp == NULL)
		return;

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	char* pBuf = new char[size + 1];
	fseek(fp, 0, SEEK_SET);
	fread(pBuf, size, 1, fp);

	if (doc.LoadFileFromMemoryEx(pBuf))
		ParseRSS(doc);

	delete[] pBuf;
}
