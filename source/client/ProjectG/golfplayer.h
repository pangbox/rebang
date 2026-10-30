#pragma once
#include <vector>
#include <list>
#include <utility>
#include "wpuppet.h"
#include "actor.h"
#include "extendedeffect.h"

class CFxSpray;
class CFxSequence;
class CPetFrame;
class CTailEffect;
class CPartTidList;
struct sTexturePart;

class CGolfPlayer : public IActor
{
public:
	virtual ~CGolfPlayer() { }
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI() const { return &m_RTTI; }
	friend IObject* CGolfPlayerMakeInstance();

	virtual void OnPreLoadInit();
	virtual void OnLoad();
	virtual void OnInit();
	virtual void OnReload();
	virtual void OnProcess(float delta);
	virtual void OnDisplay();
	virtual void OnDestroy();
	virtual void HandleMsg(const MsgObject& msg);

	struct sCondBBSpray
	{
		unsigned long typeId;
		CFxSpray* spray;
	};

	struct sCondBBSequence
	{
		unsigned long typeId;
		CFxSequence* sequence;
	};

	struct sClub
	{
		WVector ballPos;
		float gripPos;
	};

	struct Object
	{
		std::allocator<unsigned short> unknown;
	};

protected:
	CGolfPlayer();

	void SetLight(int player);
	void SetLight();
	void ResetComboState();
	void StartComboState();
	void EndComboState();
	void InputProcess(float delta);
	WMatrix GetHeadMatrix();
	WMatrix GetFootMatrix(int foot);
	void SearchGround();
	bool SearchGround(const WVector& pos, float angle);
	unsigned char HasStandGround(const WVector& pos);
	void RegisterClub(int player);
	void CheckShotScript();
	void ParseScript_ShowVis(int player, bool bShow, const char* script);
	void PlayShotSfx(int player);
	bool Decide_PowerShot();
	bool IsAnimFxRunning(int player, unsigned long typeId);
	void RefreshAllUccClothes();
	void SetPendingTexPart(int player, char* part);
	void ParseBoundingBoxScript(int player, bool b1, bool b2, bool b3);
	void CloseBoundingBoxFx(int player, unsigned long typeId);
	void ParsePuppetScript(char* script, int player, bool bSkip);
	void ScriptProcess(w_motion_data* motion, float from, float to, int player,
		bool bAll, bool bSkip);
	void ScoreEvent();
	void SetMotion(const char* name, float time, int player);
	void SetSwingType();
	void Shot_PowerShot();
	void Cancel_PowerShot();
	void PowerShotInput();
	bool ShowAbilityParts(CPartTidList* partList);

	struct sPetInfo
	{
		float time;
		w_motion_data* motion;
		WMatrix mat;
		bool bVisible;
		char nextMotion[32];
		unsigned long flags;
		int powerShot;
		float shotTime[16];
		sTexturePart* pendingTexPart;
		sTexturePart* texPart;
		float alpha;
		float alphaVel;

		sPetInfo()
			: time(0),
			  motion(NULL),
			  bVisible(false),
			  flags(0),
			  powerShot(0),
			  pendingTexPart(NULL),
			  texPart(NULL),
			  alpha(1.0f),
			  alphaVel(0)
		{
			mat.Reset();
			nextMotion[0] = 0;
			memset(shotTime, 0, sizeof(shotTime));
		}

		~sPetInfo() { }
	};

	std::vector<sCondBBSpray> m_condBBSpray[4];
	std::vector<sCondBBSequence> m_condBBSequence[4];
	std::vector<unsigned long> m_animFx[4];
	std::vector<CFxSpray*> m_spray;
	std::vector<CFxSequence*> m_sequence;
	CFxSequence* m_safeTeeSeq;
	WPuppet* m_clubPuppet;
	Object m_unknown100;
	WMatrix m_mat;
	WVector m_pos;
	float m_dir;
	CPetFrame* m_petFrame;
	sPetInfo* m_petInfo;
	float m_ballDist;
	w_bound_box* m_clubNear;
	w_bound_box* m_clubFar;
	CTailEffect* m_tail;
	bool m_bTail;
	bool m_bDisplay;
	bool m_bBlocked;
	char m_swingType[16];
	char m_motionName[32];
	bool m_bOldTurn;
	bool m_bTurn;
	sClub m_club[4][4];
	float m_footAngle;
	WBone* m_footBone[2];
	bool m_bAlpha;
	LightSet m_lightSet;
	WMatrix m_clubMat;
	bool m_bClubMat;
	float m_cupDist;
	std::list<sTexturePart*> m_texPartList;
	bool m_bMotionEnd;
	int m_reserved308;
	float m_headAngle;
	WBone* m_headBone;
	bool m_bCombo;
	float m_comboTime;
	int m_comboLevel;
	int m_reserved320;
	unsigned long m_powerShotTime;
	bool m_bPowerShot;
	bool m_bForcePowerShot;
	int m_powerShotState;
	bool m_bSkipScript;

private:
	void AddAbilityParts(unsigned long from, unsigned long to);

	std::vector<std::pair<unsigned long, unsigned long> > m_abilityParts;
	int GetAbilityPartsSize() { return m_abilityParts.size(); }

	CExtendedEffect m_extendedEffect;
};
