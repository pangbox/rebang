#pragma once

#include "encrypttypes.h"
#include "wlist.h"
#include "wflag.h"
class WVector;
class WPuppet;

struct sClubSet
{
	IFF_STRUCT::sClubSet* pClubSet;
	WPuppet* puppet[4];
	WPuppet* uiPuppet[4];
	WMatrix uiCamera[4];
	LightSet uiLight[4];
};

class CClub
{
public:
	enum eClub;

	struct sClub
	{
		WCrypticValue<float> range;
		WCrypticValue<float> lie;
		unsigned char id;
		unsigned char index;
		unsigned char type;
		WCrypticValue<float> power;
		WCrypticValue<float> curve;
		WCrypticValue<float> spin;
		char name[4];
		float pointFactor;
		WCrypticValue<float> barSpeed;
	};

	const char* GetClubString(eClub club);
	void SetClub(const char* name);
	void SetClub(unsigned char index);
	void SelectBestClub(const WVector& target);
	void SelectBestClub();
	void ClearPointFactor();
	void SetPointFactor(float factor);
	sClubSet* FindClubSet(unsigned long typeId);
	sClubSet* AddClubSet(unsigned long typeId);
	void ClearClubSet();

	void AddCrypticValue();
	void Process(float dt);

	float GetRange(const char* name = NULL);
	float GetPower();
	float GetNarrowAccuracy();
	void SetAccuracy();
	float GetWideAccuracy();
	float GetBarSpeed() { return m_pClub->barSpeed; }
	unsigned char GetType() { return m_pClub->type; }
	char* GetName() { return m_pClub->name; }
	float GetCurve() { return m_pClub->curve; }
	float GetSpin() { return m_pClub->spin; }
	float GetLie(const char* name = NULL);
	void SetApproachMode();
	void SetPowerShot(int type) { m_powerShot = type; }
	int PowerShotType() { return m_powerShot; }
	unsigned char GetApproachMode() { return m_approachMode; }
	unsigned char GetIndex() { return m_pClub->index; }
	unsigned char GetID() { return m_pClub->id; }
	bool Updated() { return m_flag.GetFlag(1); }
	bool TypeUpdated() { return m_flag.GetFlag(2); }
	void SelectClubByDist(float distance);
	bool IsPowerPutter() { return m_bPowerPutter; }
	bool Is(char* name, sClub* club = NULL)
	{
		char* result;
		if (club)
			result = strstr(club->name, name);
		else
			result = strstr(m_pClub->name, name);
		return result != NULL;
	}
	void SetBar(int value) { m_bar = value; }
	void SetItemWindow(bool value) { m_bItemWindow = value; }
	bool BarIs(int state) { return m_bar == state; }
	void SetRandomPower(float power) { m_randomPower = power; }
	float GetRandomPower() const { return m_randomPower; }
	bool IsCalcRandomPower() { return m_bCalcRandomPower; }
	void SetCalcRandomPower(bool bCalc) { m_bCalcRandomPower = bCalc; }

private:
	int m_reserved;
	sClub* m_pClub;
	WList<sClub*> m_clubList;
	unsigned char m_curIndex;
	unsigned char m_limitIndex;
	WCrypticValue<int> m_powerShot;
	unsigned char m_approachMode;
	unsigned char m_unused3d[3];
	WFlags m_flag;
	bool m_bPowerPutter;
	float m_barDelay;
	float m_barAlpha;
	float m_barWidth;
	int m_bar;
	bool m_bItemWindow;
	unsigned char m_prevIndex;
	float m_narrowAccuracy;
	float m_wideAccuracy;
	float m_randomPower;
	bool m_bCalcRandomPower;
	// TODO: incomplete
};

CClub& GolfClub();
