#include "pangfbi.h"
#include "generichttpclient.h"

CPangFBI::CPangFBI()
{
}

CPangFBI::~CPangFBI()
{
}

void CPangFBI::ProcessReport(const char* type, const char* data)
{
	if (strcmpi("crime_log", type) == 0)
	{
		int crimeType = 0;
		char szFile[776];

		char szID[128] = { 0 };
		sscanf(data, "%s %d %s", szID, &crimeType, szFile);

		FILE* fp = fopen(szFile, "rb");

		if (fp == NULL)
			return;
		fclose(fp);

		GenericHTTPClient* pClient = new GenericHTTPClient;

		pClient->InitilizePostArguments();

		pClient->AddPostArguments("id", szID, FALSE);
		pClient->AddPostArguments("type", crimeType);
		pClient->AddPostArguments("file", szFile, TRUE);

		if (pClient->Request(
				"http://qa.reports.pangya.gametree.co.kr:50000/post/CrimeReport_KR.asp",
				3, "MERONG(0.9/;p)"))
		{
			pClient->QueryHTTPResponse();
		}

		delete pClient;
		DeleteFile(szFile);
	}
	else if (strcmpi("app_log", type) == 0)
	{
		int code = 0;
		char szUserID[128] = { 0 };
		char szPacketVer[64];
		char szClientVer[64], szComment[128];

		sscanf(data, "%d %s %s %s %s", &code, szUserID, szPacketVer,
			szClientVer, szComment);

		GenericHTTPClient* pClient = new GenericHTTPClient;
		pClient->InitilizePostArguments();

		pClient->AddPostArguments("code", code);
		pClient->AddPostArguments("userid", szUserID, FALSE);
		pClient->AddPostArguments("packet_ver", szPacketVer, FALSE);
		pClient->AddPostArguments("client_ver", szClientVer, FALSE);
		pClient->AddPostArguments("comment", szComment, FALSE);

		if (pClient->Request(
				"http://qa.reports.pangya.gametree.co.kr:50000/post/GameBugReport_KR.asp",
				3, "MERONG(0.9/;p)"))
		{
			pClient->QueryHTTPResponse();
		}

		delete pClient;
	}
}
