#pragma once

class CShadowCaster;

// TODO: incomplete
class CShadowManager : public WSingleton<CShadowManager>
{
public:
	CShadowManager(WResourceManager* pResMgr, bool bEnable);
	virtual ~CShadowManager();

	void UpdateLight(const char* lightName, LightSet& light);
	CShadowCaster* RegisterCaster_New(const char* name, int state,
		const char* lightName, LightSet& light);
	void AddCastPet_New(const char* recvName, bool flag, WPuppet* pet);
	void Enable(bool bEnable);
	void SetActive(bool bActive);
	void Reset();

protected:
	bool m_bEnable;
	unsigned char m_unused2c[0x30];
};
