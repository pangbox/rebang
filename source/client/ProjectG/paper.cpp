#include "stdafx.h"
#include <stdarg.h>

#include "paper.h"

#include <atltime.h>
const char* paperThemes[] = { "PT_CUPINHOLDING", "PT_RECORD", "PT_AWARD",
	"PT_MAX" };
static ATL::CTimeSpan serverTimeOffset;

CPaperMemo::CPaperMemo(const char* signature)
	: m_signature(signature)
{
}

CPaperMemo::~CPaperMemo()
{
}

void CPaperMemo::Write(ePaperTheme theme, const char* format, ...)
{
	char buffer[512] = { 0 };
	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, 511, format, args);
	va_end(args);
	CPaper::Instance()->AddMemo(theme, m_signature.c_str(), buffer);
}

CPaper::CPaper()
{
	m_updating = 0;
	serverTimeOffset = ATL::CTimeSpan(0);
}

CPaper::~CPaper()
{
}

void CPaper::SetServerTime(const SYSTEMTIME& time)
{
	SYSTEMTIME local;
	GetLocalTime(&local);
	ATL::CTime localTime(local);
	ATL::CTime serverTime(time);
	serverTimeOffset = serverTime - localTime;
}

void CPaper::PrepareNewPaper(const char* user)
{
	Destroy();
	m_user = user;
	GetServerTime(m_createTime);
}

void CPaper::Destroy()
{
	memset(&m_createTime, 0, sizeof(m_createTime));
	memset(&m_sendTime, 0, sizeof(m_sendTime));
	m_user = "";
	m_memos.clear();
	m_signatures.clear();
	m_signatureIds.clear();
}

bool CPaper::AddMemo(ePaperTheme theme, const char* signature,
	const char* message)
{
	if (m_updating)
		return false;
	std::multimap<ePaperTheme, cMemo>::const_iterator first =
		m_memos.lower_bound(theme);
	std::multimap<ePaperTheme, cMemo>::const_iterator last =
		m_memos.upper_bound(theme);
	int count = std::distance(first, last);
	if (count < 999)
	{
		cMemo memo(GetServerTime(), GetFuncSigId(signature), message);
		m_memos.insert(
			std::multimap<ePaperTheme, cMemo>::value_type(theme, memo));
	}
	else
		return false;
	return true;
}

const char* CPaper::GetEnumStr(ePaperTheme theme) const
{
	if (theme < PT_MAX)
		return paperThemes[theme];
	return "";
}

const char* CPaper::GetFuncSigStr(unsigned int id) const
{
	if (id > 0 && id < m_signatures.size())
		return m_signatures[id].c_str();
	return "";
}

const char* CPaper::GetServerTime() const
{
	static char serverTimeBuffer[512];
	SYSTEMTIME local;
	GetLocalTime(&local);
	ATL::CTime time(local);
	time += serverTimeOffset;
	struct tm tm;
	time.GetLocalTm(&tm);
	memset(serverTimeBuffer, 0, sizeof(serverTimeBuffer));
	strftime(serverTimeBuffer, 511, "%y-%m-%d %H:%M:%S", &tm);
	return serverTimeBuffer;
}

unsigned int CPaper::GetMemoSize(ePaperTheme theme) const
{
	std::multimap<ePaperTheme, cMemo>::const_iterator first =
		m_memos.lower_bound(theme);
	std::multimap<ePaperTheme, cMemo>::const_iterator last =
		m_memos.upper_bound(theme);
	return std::distance(first, last);
}

void CPaper::GetServerTime(SYSTEMTIME& result) const
{
	SYSTEMTIME local;
	GetLocalTime(&local);
	ATL::CTime time(local);
	time += serverTimeOffset;
	time.GetAsSystemTime(result);
}

bool CPaper::PrintOut(ePaperTheme theme)
{
	std::multimap<ePaperTheme, cMemo>::const_iterator it =
		m_memos.lower_bound(theme);
	if (it == m_memos.end())
		return false;
	FILE* file = fopen("./saved/paper.xml", "wt");
	if (!file)
		return false;
	fprintf(file,
		"<?xml version=\"1.0\" encoding=\"euc-kr\" standalone=\"yes\" ?>\n");
	fprintf(file,
		"<?xml:stylesheet type=\"text/xsl\" href=\"paperview.xsl\" ?>\n");
	fprintf(file, "<!DOCTYPE PAPERS[\n");
	fprintf(file, "\t<!ELEMENT PAPERS (INDEX,PAPER)>\n");
	fprintf(file, "\t<!ELEMENT INDEX  (USERNICK,CREATETIME,SENDTIME)>\n");
	fprintf(file, "\t<!ELEMENT PAPER  (NUMBER,TIME,FUNCSIG,MESSAGE)>\n");
	fprintf(file, "\t<!ELEMENT USERNICK \t(#PCDATA)>\n");
	fprintf(file, "\t<!ELEMENT CREATETIME (#PCDATA)>\n");
	fprintf(file, "\t<!ELEMENT SENDTIME \t(#PCDATA)>\n");
	fprintf(file, "\t<!ELEMENT NUMBER \t(#PCDATA)>\n");
	fprintf(file, "\t<!ELEMENT TIME\t\t(#PCDATA)>\n");
	fprintf(file, "\t<!ELEMENT FUNCSIG\t(#PCDATA)>\n");
	fprintf(file, "\t<!ELEMENT MESSAGE\t(#PCDATA)>\n");
	fprintf(file, "]>\n");
	fprintf(file, "<PAPERS>\n");
	fprintf(file, "<INDEX>\n");
	fprintf(file, "\t<THEME>%s</THEME>\n", GetEnumStr(theme));
	fprintf(file, "\t<USERNICK>%s</USERNICK>\n", m_user.c_str());
	fprintf(file, "\t<CREATETIME>%s</CREATETIME>\n",
		SystemTimeToString(m_createTime));
	fprintf(file, "\t<SENDTIME>%s</SENDTIME>\n", GetServerTime());
	fprintf(file, "</INDEX>\n");
	int number = 0;
	std::multimap<ePaperTheme, cMemo>::const_iterator end =
		m_memos.upper_bound(theme);
	for (; it != end; ++it)
	{
		const cMemo& memo = it->second;
		fprintf(file, "<PAPER>\n");
		fprintf(file, "\t<NUMBER>%d</NUMBER>\n", ++number);
		fprintf(file, "\t<TIME>%s</TIME>\n", memo.m_time.c_str());
		fprintf(file, "\t<FUNCSIG>%s</FUNCSIG>\n",
			GetFuncSigStr(memo.m_signature));
		fprintf(file, "\t<MESSAGE>%s</MESSAGE>\n", memo.m_message.c_str());
		fprintf(file, "</PAPER>\n");
	}
	fprintf(file, "</PAPERS>\n");
	fclose(file);
	return true;
}

void CPaper::ReceivedPacket(WReceivedPacket* packet)
{
	packet->Decode1();
	m_updating = 0;
}

void CPaper::SendUpdatableQuery()
{
	m_updating = 1;
}

void CPaper::Send()
{
}

const char* CPaper::SystemTimeToString(const SYSTEMTIME& systemTime) const
{
	static char systemTimeBuffer[512];
	ATL::CTime time(systemTime);
	struct tm tm;
	time.GetLocalTm(&tm);
	memset(systemTimeBuffer, 0, sizeof(systemTimeBuffer));
	strftime(systemTimeBuffer, 511, "%y-%m-%d %H:%M:%S", &tm);
	return systemTimeBuffer;
}

unsigned int CPaper::GetFuncSigId(const char* signature)
{
	std::map<std::string, unsigned int>::iterator it =
		m_signatureIds.find(signature);
	if (it != m_signatureIds.end())
		return it->second;
	unsigned int id = m_signatureIds.size();
	m_signatureIds.insert(
		std::map<std::string, unsigned int>::value_type(signature, id));
	m_signatures.push_back(signature);
	return id;
}
