#pragma once

#include <string>

struct _sNtreevUserIdentity
{
	unsigned long memberNo;
	char sex;
	unsigned char age;
	int wardNo;
	unsigned short cityCode;
	unsigned char pcBangNo;
	char reserved[65];
	char loginId[51];
	char authKey[65];
};

struct _sNhnUserIdentity
{
	unsigned long memberNo;
	char sex;
	unsigned char age;
	int wardNo;
	unsigned short cityCode;
	unsigned char pcBangNo;
	char reserved[65];
	__int64 siteMemberNo;
	char loginId[81];
	char authKey[65];
	char siteCookie[1024];
	unsigned char provType;
};

class ILoginInfo
{
public:
	ILoginInfo() { }
	virtual ~ILoginInfo() { }

	virtual int Init() = 0;
	virtual void SetProvType() = 0;

	virtual unsigned long ProvType() = 0;
	virtual void ProvType(unsigned long type) = 0;
	virtual const char* FullId() = 0;
	virtual void FullId(std::string id) = 0;
	virtual const char* Id() = 0;
	virtual void Id(std::string id) = 0;
	virtual unsigned long Uid() = 0;
	virtual void Uid(unsigned long uid) = 0;
	virtual const char* Passwd() = 0;
	virtual void Passwd(std::string pw) = 0;
	virtual int IsPcBang() = 0;
	virtual unsigned long AuthUid() = 0;
	virtual void AuthUid(unsigned long uid) = 0;
	virtual void EnablePcBang(int enable) = 0;
	virtual unsigned char Gender() = 0;
	virtual void Gender(unsigned char gender) = 0;
	virtual const char* ZipCode() = 0;
	virtual void ZipCode(const std::string& zipCode) = 0;
	virtual const char* BirthDay() = 0;
	virtual void BirthDay(const std::string& birthDay) = 0;
	virtual void PcBang(int pcBang) = 0;
};

class CLoginInfo
{
public:
	static CLoginInfo* Instance()
	{
		if (m_pInst == NULL)
			m_pInst = new CLoginInfo;
		return m_pInst;
	}

	int Destroy();
	int InitWebLogin(const char* cmdLine);

	int IsWebLogin() const { return m_bWebLogin; }

	unsigned long ProvType() const;
	const char* FullId() const;
	const char* Id() const;
	unsigned long Uid() const;
	const char* Passwd() const;
	int IsPcBang() const;
	unsigned long AuthUid() const;
	unsigned char Gender() const;
	const char* ZipCode() const;
	const char* BirthDay() const;

	void EnablePcBang(int enable);
	void SetId(const std::string& id);
	void SetUid(unsigned long uid);
	void SetAuthUid(unsigned long uid);
	void SetPw(const std::string& pw);
	void SetGender(unsigned char gender);
	void SetZipCode(const std::string& zipCode);
	void SetBirthDay(const std::string& birthDay);
	void SetProvType(unsigned long type);

	ILoginInfo* QueryInterface();

private:
	CLoginInfo();
	virtual ~CLoginInfo();

	static CLoginInfo* m_pInst;

	ILoginInfo* m_pLoginInfo;
	int m_bWebLogin;
};

class ChannelingChecker;

class BaseLoginInfo : public ILoginInfo
{
public:
	BaseLoginInfo()
		: m_authUid(0),
		  m_uid(0),
		  m_provType(2),
		  m_pcBang(0),
		  m_gender(0),
		  m_pChannelingChecker(NULL)
	{
	}
	virtual ~BaseLoginInfo() { }

	virtual const char* FullId() { return m_fullId.c_str(); }
	virtual const char* Id() { return m_id.c_str(); }
	virtual unsigned long Uid() { return m_uid; }
	virtual unsigned long AuthUid() { return m_authUid; }
	virtual const char* Passwd() { return m_passwd.c_str(); }
	virtual unsigned long ProvType() { return m_provType; }
	virtual int IsPcBang() { return m_pcBang; }

	virtual unsigned char Gender() { return m_gender; }
	virtual const char* ZipCode() { return m_zipCode.c_str(); }
	virtual const char* BirthDay() { return m_birthDay.c_str(); }

	virtual void Id(std::string id) { m_id = id; }
	virtual void FullId(std::string id) { m_fullId = id; }
	virtual void Uid(unsigned long uid) { m_uid = uid; }
	virtual void AuthUid(unsigned long uid) { m_authUid = uid; }
	virtual void Passwd(std::string pw) { m_passwd = pw; }
	virtual void ProvType(unsigned long type) { m_provType = type; }
	virtual void PcBang(int pcBang) { m_pcBang = pcBang; }

	virtual void Gender(unsigned char gender) { m_gender = gender; }
	virtual void ZipCode(const std::string& zipCode) { m_zipCode = zipCode; }
	virtual void BirthDay(const std::string& birthDay)
	{
		m_birthDay = birthDay;
	}

	virtual void EnablePcBang(int enable) { m_pcBang = enable; }

	void DependChannelingChecker(ChannelingChecker* pChecker)
	{
		m_pChannelingChecker = pChecker;
	}
	ChannelingChecker* GetChannelingChecker() { return m_pChannelingChecker; }

protected:
	virtual void Convert(void* pIdentity) = 0;

	std::string m_id;
	std::string m_fullId;
	std::string m_passwd;
	unsigned long m_authUid;
	unsigned long m_uid;
	unsigned long m_provType;
	int m_pcBang;
	unsigned char m_gender;
	std::string m_zipCode;
	std::string m_birthDay;
	ChannelingChecker* m_pChannelingChecker;
};

class ChannelingChecker
{
public:
	ChannelingChecker()
		: m_pMapData(NULL), m_provType(0xffffffff)
	{
	}
	~ChannelingChecker() { }

	int ChannelingCheck()
	{
		m_hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE,
			"927628CA6D76A6E9162C56D4E3E6D6E3");
		if (m_hMapFile == NULL)
		{
			return FALSE;
		}

		m_pMapData = MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (m_pMapData == NULL)
		{
			return FALSE;
		}

		SetChannelingFlag();

		return TRUE;
	}

	void* GetMapData() { return m_pMapData; }
	unsigned long GetProvType() { return m_provType; }

private:
	void SetChannelingFlag()
	{
		std::string data((char*)m_pMapData);
		if (data.find("SiteCode") != std::string::npos)
		{
			m_provType = 4;
		}
		else
		{
			m_provType = 2;
		}
	}

	HANDLE m_hMapFile;
	void* m_pMapData;
	unsigned long m_provType;
};

class NtreevLogin : public BaseLoginInfo
{
public:
	NtreevLogin() { }
	virtual ~NtreevLogin() { }

	virtual int Init();
	virtual void SetProvType();

protected:
	virtual void Convert(void* pIdentity);

private:
	void SetNtreevUserIdentity(const char* data,
		_sNtreevUserIdentity& identity);
};

class NhnLogin : public BaseLoginInfo
{
public:
	NhnLogin() { }
	virtual ~NhnLogin() { }

	virtual int Init();
	virtual void SetProvType();

	void SetNHNMemberNumber(__int64 memberNumber);
	__int64 GetNHNMemberNumber() const;
	void SetSiteCookie(const char* cookie);
	const char* GetSiteCookie() const;

protected:
	virtual void Convert(void* pIdentity);

private:
	void SetNhnUserIdentity(const char* data, _sNhnUserIdentity& identity);

	__int64 m_nhnMemberNumber;
	std::string m_siteCookie;
};

class PangYaLogin : public BaseLoginInfo
{
public:
	PangYaLogin() { }
	virtual ~PangYaLogin() { }

	virtual int Init() { return FALSE; }

	virtual void Convert(void* pIdentity) { }
	virtual void SetProvType() { m_provType = 2; }
};

class cHttpNtreevAuthResult
{
public:
	cHttpNtreevAuthResult(const char* xml)
		: m_xml(xml)
	{
	}

	bool GetResult();
	std::string GetMessageA();
	std::string GetFieldValue(const char* name);
	std::string GetExternalLink();

private:
	std::string GetElement(const char* tag);

	std::string m_xml;
};

inline CLoginInfo* LOGININFO()
{
	return CLoginInfo::Instance();
}

inline const char* LOGINID()
{
	return LOGININFO()->Id();
}

inline unsigned long AUTHUID()
{
	return LOGININFO()->AuthUid();
}

inline const char* LOGINPW()
{
	return LOGININFO()->Passwd();
}

inline bool IsWebLogin()
{
	return LOGININFO()->IsWebLogin() ? true : false;
}

inline int IsPcBang()
{
	return LOGININFO()->IsPcBang();
}
