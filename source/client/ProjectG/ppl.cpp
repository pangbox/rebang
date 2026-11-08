#include <wininet.h>
#include "clientsetting.h"
#include "ppl.h"

CPPL::CPPL()
{
}

CPPL::~CPPL()
{
}

bool CPPL::IsPPLEnable()
{
	char url[] = "http://www.hjwon.com:8000/test.swf";

	DWORD size = 0;

	HINTERNET hInternet = InternetOpenA("PPL Check", 0, NULL, NULL, 0);
	if (hInternet == NULL)
	{
		return false;
	}

	HINTERNET hUrl =
		InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (hUrl == NULL)
	{
		InternetCloseHandle(hInternet);
		return false;
	}

	if (!InternetQueryDataAvailable(hUrl, &size, 0, 0))
	{
		return false;
	}

	if (COption::Instance()->gGetPPLSize() == size)
	{
		if (!COption::Instance()->gIsPPLEnable())
		{
			return false;
		}
	}

	COption::Instance()->gSetPPLEnable(true);
	COption::Instance()->gSetPPLSize(size);

	return true;
}
