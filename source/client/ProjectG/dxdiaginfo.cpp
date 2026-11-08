#include "dxdiaginfo.h"

#define DXDIAG_FILE "./gameguard/npsys.des"

void CDxDiagInfo::Init()
{
	memset(m_osName, 0, sizeof(m_osName));
	memset(m_cpuInfo, 0, sizeof(m_cpuInfo));
	memset(m_graphicCardInfo, 0, sizeof(m_graphicCardInfo));
	m_bGathered = false;
}

void CDxDiagInfo::StartGathering()
{
	CreateDirectory("GameGuard", NULL);
	char cmd[256];
	sprintf(cmd, "cmd /c \"dxdiag /whql:off /x %s\"", DXDIAG_FILE);
	WinExec(cmd, SW_HIDE);
}

void CDxDiagInfo::GatherInfo()
{
	if (m_bGathered)
		return;

	TiXmlDocument doc;

	if (doc.LoadFile(DXDIAG_FILE))
	{
		m_bGathered = true;
		TiXmlNode* pDxDiag = doc.FirstChild("DxDiag");
		if (pDxDiag)
		{
			TiXmlNode* pSystem = pDxDiag->FirstChild("SystemInformation");
			if (pSystem)
			{
				TiXmlNode* pNode = pSystem->FirstChild("Processor");
				if (pNode && pNode->FirstChild())
				{
					strcpy(m_cpuInfo, pNode->FirstChild()->Value());
				}

				pNode = pSystem->FirstChild("OperatingSystem");
				if (pNode && pNode->FirstChild())
				{
					strcpy(m_osName, pNode->FirstChild()->Value());
				}
			}

			TiXmlNode* pDisplay = pDxDiag->FirstChild("DisplayDevices");
			if (pDisplay)
			{
				TiXmlNode* pDevice = pDisplay->FirstChild("DisplayDevice");
				if (pDevice)
				{
					TiXmlNode* pCard = pDevice->FirstChild("CardName");
					if (pCard && pCard->FirstChild())
					{
						strcpy(m_graphicCardInfo, pCard->FirstChild()->Value());
					}
				}
			}
		}

		DeleteFile(DXDIAG_FILE);
	}
}
