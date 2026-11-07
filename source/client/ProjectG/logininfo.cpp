#include "stdafx.h"
#include "logininfo.h"
#include "s5/utilities.h"

namespace S5
{
	namespace UTIL
	{
		int DecodingUrl(const char* src, char* dst);
	}
}

CLoginInfo* CLoginInfo::m_pInst = NULL;

int NtreevLogin::Init()
{
	_sNtreevUserIdentity identity;
	memset(&identity, 0, sizeof(identity));
	SetNtreevUserIdentity((const char*)GetChannelingChecker()->GetMapData(),
		identity);
	Convert(&identity);
	return TRUE;
}

void NtreevLogin::SetProvType()
{
	m_provType = 2;
}

void NtreevLogin::Convert(void* pIdentity)
{
	_sNtreevUserIdentity* identity = (_sNtreevUserIdentity*)pIdentity;
	m_id = identity->loginId;
	m_fullId = identity->loginId;
	m_passwd = identity->authKey;
	m_authUid = identity->memberNo;
	m_pcBang = identity->pcBangNo != 0;
	m_birthDay = "19801129";
	m_zipCode = "131-116";
	m_gender = identity->sex == 'M' ? 0 : 1;
}

void NtreevLogin::SetNtreevUserIdentity(const char* data,
	_sNtreevUserIdentity& identity)
{
	std::string text(data);
	std::string name;
	std::string value;
	std::string::size_type end = 0;
	do
	{
		std::string::size_type begin = text.find('|', end) + 1;
		end = text.find('|', begin);
		std::string field;
		if (begin != std::string::npos)
		{
			if (end != std::string::npos)
				field.assign(text.begin() + begin, text.begin() + end);
			else
				field.assign(text.begin() + begin, text.end());
			std::string::size_type equals = field.find('=');
			name.assign(field.begin(), field.begin() + equals);
			value.assign(field.begin() + equals + 1, field.end());
			if (_stricmp(name.c_str(), "LoginID") == 0)
			{
				strncpy(identity.loginId, value.c_str(), 50);
				continue;
			}
			if (_stricmp(name.c_str(), "AuthKey") == 0)
			{
				strncpy(identity.authKey, value.c_str(), 64);
				continue;
			}
			if (_stricmp(name.c_str(), "Sex") == 0)
			{
				identity.sex = value == "M" ? 0 : 1;
				continue;
			}
			if (_stricmp(name.c_str(), "MemberNo") == 0)
			{
				identity.memberNo = atoi(value.c_str());
				continue;
			}
			if (_stricmp(name.c_str(), "Age") == 0)
			{
				identity.age = atoi(value.c_str());
				continue;
			}
			if (_stricmp(name.c_str(), "WardNo") == 0)
			{
				identity.wardNo = atoi(value.c_str());
				continue;
			}
			if (_stricmp(name.c_str(), "CityCode") == 0)
			{
				identity.cityCode = atoi(value.c_str());
				continue;
			}
			if (_stricmp(name.c_str(), "PCBangNo") == 0)
			{
				identity.pcBangNo = atoi(value.c_str());
				continue;
			}
		}
	} while (end != std::string::npos);
}

int NhnLogin::Init()
{
	_sNhnUserIdentity identity;
	memset(&identity, 0, sizeof(identity));
	SetNhnUserIdentity((const char*)GetChannelingChecker()->GetMapData(),
		identity);
	Convert(&identity);
	return TRUE;
}

void NhnLogin::SetProvType()
{
	m_provType = 4;
}

void NhnLogin::SetNHNMemberNumber(__int64 memberNumber)
{
	m_nhnMemberNumber = memberNumber;
}

__int64 NhnLogin::GetNHNMemberNumber() const
{
	return m_nhnMemberNumber;
}

void NhnLogin::SetSiteCookie(const char* cookie)
{
	m_siteCookie = cookie;
}

const char* NhnLogin::GetSiteCookie() const
{
	return m_siteCookie.c_str();
}

void NhnLogin::SetNhnUserIdentity(const char* data, _sNhnUserIdentity& identity)
{
	std::string text(data);
	std::string name;
	std::string value;
	std::string::size_type end = 0;
	do
	{
		std::string::size_type begin = text.find('|', end) + 1;
		end = text.find('|', begin);
		std::string field;
		if (begin !=
			static_cast<const volatile std::string::size_type&>(
				std::string::npos))
		{
			if (end != std::string::npos)
				field.assign(text.begin() + begin, text.begin() + end);
			else
				field.assign(text.begin() + begin, text.end());
			std::string::size_type equals = field.find('=');
			name.assign(field.begin(), field.begin() + equals);
			value.assign(field.begin() + equals + 1, field.end());
			if (_stricmp(name.c_str(), "LoginID") == 0)
			{
				strncpy(identity.loginId, value.c_str(), 80);
				continue;
			}
			if (_stricmp(name.c_str(), "AuthKey") == 0)
			{
				strncpy(identity.authKey, value.c_str(), 64);
				continue;
			}
			if (_stricmp(name.c_str(), "SiteCookie") == 0)
			{
				strncpy(identity.siteCookie, value.c_str(), 1023);
				continue;
			}
			if (_stricmp(name.c_str(), "Sex") == 0)
			{
				identity.sex = value == "M" ? 0 : 1;
				continue;
			}
			if (_stricmp(name.c_str(), "MemberNo") == 0)
			{
				identity.memberNo = atoi(value.c_str());
				continue;
			}
			if (_stricmp(name.c_str(), "Age") == 0)
			{
				identity.age = atoi(value.c_str());
				continue;
			}
			if (_stricmp(name.c_str(), "WardNo") == 0)
			{
				identity.wardNo = atoi(value.c_str());
				continue;
			}
			if (_stricmp(name.c_str(), "CityCode") == 0)
			{
				identity.cityCode = atoi(value.c_str());
				continue;
			}
			if (_stricmp(name.c_str(), "PCBangNo") == 0)
			{
				identity.pcBangNo = atoi(value.c_str());
				continue;
			}
			if (_stricmp(name.c_str(), "SiteMemberNo") == 0)
			{
				identity.siteMemberNo = _atoi64(value.c_str());
				continue;
			}
			{
				char site[32] = { 0 };
				if (_stricmp(name.c_str(), "SiteCode") == 0)
					strncpy(site, value.c_str(), 31);
				if (strcmp(site, "NHN") == 0)
					identity.provType = 4;
			}
		}
	} while (end != std::string::npos);
}

void NhnLogin::Convert(void* pIdentity)
{
	_sNhnUserIdentity* identity = (_sNhnUserIdentity*)pIdentity;
	char id[22] = { 0 };
	S5::UTIL::DecodingUrl(identity->loginId, id);
	m_id = id;
	m_fullId = id;
	m_passwd = identity->authKey;
	m_authUid = identity->memberNo;
	m_pcBang = identity->pcBangNo != 0;
	m_nhnMemberNumber = identity->siteMemberNo;
	m_birthDay = "19801129";
	m_zipCode = "131-116";
	m_siteCookie = "HG_CP_LOGIN=";
	m_siteCookie += identity->siteCookie;
	m_gender = identity->sex == 'M' ? 0 : 1;
}

CLoginInfo::CLoginInfo()
	: m_pLoginInfo(NULL), m_bWebLogin(FALSE)
{
}

int CLoginInfo::Destroy()
{
	if (m_pLoginInfo)
	{
		delete m_pLoginInfo;
		m_pLoginInfo = NULL;
	}
	if (m_pInst)
	{
		delete m_pInst;
		m_pInst = NULL;
	}
	return TRUE;
}

CLoginInfo::~CLoginInfo()
{
}

int CLoginInfo::InitWebLogin(const char* cmdLine)
{
	std::string command(cmdLine);
	if (command.find("-vwccnp") != std::string::npos)
	{
		ChannelingChecker checker;
		checker.ChannelingCheck();
		switch (checker.GetProvType())
		{
		case 2:
			m_pLoginInfo = new NtreevLogin;
			break;
		case 4:
			m_pLoginInfo = new NhnLogin;
			break;
		default:
			m_pLoginInfo = NULL;
			break;
		}
		if (m_pLoginInfo)
			((BaseLoginInfo*)m_pLoginInfo)->DependChannelingChecker(&checker);
	}
	if (m_pLoginInfo)
	{
		if (m_pLoginInfo->Init())
		{
			m_pLoginInfo->SetProvType();
			m_bWebLogin = TRUE;
			return TRUE;
		}
		delete m_pLoginInfo;
	}
	m_pLoginInfo = new PangYaLogin;
	return FALSE;
}

unsigned long CLoginInfo::ProvType() const
{
	return m_pLoginInfo->ProvType();
}

const char* CLoginInfo::FullId() const
{
	return m_pLoginInfo->FullId();
}

const char* CLoginInfo::Id() const
{
	return m_pLoginInfo->Id();
}

unsigned long CLoginInfo::Uid() const
{
	return m_pLoginInfo->Uid();
}

const char* CLoginInfo::Passwd() const
{
	return m_pLoginInfo->Passwd();
}

int CLoginInfo::IsPcBang() const
{
	return m_pLoginInfo->IsPcBang();
}

unsigned long CLoginInfo::AuthUid() const
{
	return m_pLoginInfo->AuthUid();
}

unsigned char CLoginInfo::Gender() const
{
	return m_pLoginInfo->Gender();
}

const char* CLoginInfo::ZipCode() const
{
	return m_pLoginInfo->ZipCode();
}

const char* CLoginInfo::BirthDay() const
{
	return m_pLoginInfo->BirthDay();
}

void CLoginInfo::EnablePcBang(int enable)
{
	m_pLoginInfo->EnablePcBang(enable);
}

void CLoginInfo::SetId(const std::string& id)
{
	m_pLoginInfo->Id(id);
	m_pLoginInfo->FullId(id);
}

void CLoginInfo::SetUid(unsigned long value)
{
	m_pLoginInfo->Uid(value);
}

void CLoginInfo::SetAuthUid(unsigned long value)
{
	m_pLoginInfo->AuthUid(value);
}

void CLoginInfo::SetPw(const std::string& value)
{
	m_pLoginInfo->Passwd(value);
}

void CLoginInfo::SetGender(unsigned char value)
{
	m_pLoginInfo->Gender(value);
}

void CLoginInfo::SetZipCode(const std::string& value)
{
	m_pLoginInfo->ZipCode(value);
}

void CLoginInfo::SetBirthDay(const std::string& value)
{
	m_pLoginInfo->BirthDay(value);
}

ILoginInfo* CLoginInfo::QueryInterface()
{
	return m_pLoginInfo;
}

void CLoginInfo::SetProvType(unsigned long type)
{
	m_pLoginInfo->ProvType(type);
}

bool cHttpNtreevAuthResult::GetResult()
{
	std::string result = GetElement("result");
	if (result == "true")
		return true;
	return false;
}

std::string cHttpNtreevAuthResult::GetMessageA()
{
	std::string message = GetElement("messages");
	char output[260] = { 0 };
	wchar_t wide[260] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, wide, 260);
	WideCharToMultiByte(CP_OEMCP, 0, wide, -1, output, 260, NULL, NULL);
	return output;
}

std::string cHttpNtreevAuthResult::GetElement(const char* tag)
{
	std::string beginTag("<");
	beginTag += tag;
	beginTag += ">";
	std::string endTag("</");
	endTag += tag;
	endTag += ">";
	std::string::size_type begin = m_xml.find(beginTag);
	std::string::size_type end = m_xml.find(endTag);
	if (begin == std::string::npos || end == std::string::npos)
		return "";
	begin += beginTag.length();
	return std::string(m_xml.begin() + begin, m_xml.begin() + end);
}

struct FnTrim
{
	FnTrim(const char* trimChars)
		: m_trimChars(trimChars), m_result("")
	{
	}
	void operator()(char c)
	{
		if (m_trimChars.find(c) == std::string::npos)
			m_result += c;
	}
	std::string m_trimChars;
	std::string m_result;
};

std::string cHttpNtreevAuthResult::GetFieldValue(const char* name)
{
	std::string arg = GetElement("arg");
	FnTrim trim = std::for_each(arg.begin(), arg.end(), FnTrim(" \t\n\r\0"));
	arg = trim.m_result;
	std::string::size_type begin = arg.find(name);
	if (begin == std::string::npos)
		return "";
	begin = arg.find("=", begin + 1);
	std::string::size_type end = arg.find("|", begin + 1);
	if (begin != std::string::npos)
	{
		if (end != std::string::npos)
			return std::string(arg.begin() + begin + 1, arg.begin() + end);
		else
			return std::string(arg.begin() + begin + 1, arg.end());
	}
	return "";
}

std::string cHttpNtreevAuthResult::GetExternalLink()
{
	return "";
}
