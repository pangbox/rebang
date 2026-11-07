#pragma once

#include <list>

#include "../../shared/globalgamedefine.h"
#include "parttidlist.h"
#include "fx.h"

class CPetFrame;
class CRealMyRoomMain;
struct sTexturePart;
struct Attacher;
struct defines_t;
class ntBalloonText;

class CRealMyRoom_AvatarBurden
{
public:
	CRealMyRoom_AvatarBurden();
	CRealMyRoom_AvatarBurden(unsigned long guid, const sCharacterInfo& info,
		CPetFrame* pPetFrame);
	~CRealMyRoom_AvatarBurden();

	bool LoadAvatarBurden();

	unsigned long m_guid;
	sCharacterInfo m_charInfo;
	CPartTidList m_partTidList;
	CPetFrame* m_pPetFrame;
	unsigned long m_reserved;
};

class CRealMyRoom_Avatar
{
public:
	CRealMyRoom_Avatar(const sSlotInfo& slot, const sCharacterInfo& info,
		CRealMyRoomMain* pMain);
	~CRealMyRoom_Avatar();

	void Process(float delta);
	void Render();
	void RenderOverlay(bool bSelected);
	bool IsClickable(float x, float y) const;
	bool GetBoneScreenCoord(const char* bone, int* x, int* y) const;

	void ChangeCharacterInfo();
	void SetCharactorInfo(const sCharacterInfo& info);
	void SetPosition(const WVector& pos);
	void SetStartPosition(const WVector& pos);
	void SetTargetPosition(const WVector& pos);
	bool IsTargetPositionChanged(bool bReset);
	void SetAngle(float angle);
	void SetTargetAngle(float angle);
	void SetPendingTargetAngle(float angle);
	void SetState(unsigned long state);
	void SetAction(int action);
	void ChangeMotion(int motion, const char* name, float blendTime);
	void SetChatFacial(int facial);
	void Stop();
	void Follow(const char* nickName);
	CRealMyRoom_Avatar* GetFollowingAvatar() const;
	void SetBalloonText(const char* text);
	void SetTradeTitle(const char* title);
	void RefreshUccClothes(unsigned long typeId, unsigned long uid);
	void SetHide(bool bHide);

	bool IsHoldState() const { return (m_state & 0x7e) ? true : false; }

	CPetFrame* GetPetFrame() { return m_pPetFrame; }
	CPartTidList& GetPartTidList() { return m_partTidList; }

	const char* GetIDName() const { return m_idName; }
	const char* GetNickName() const { return m_nickName; }

	const WVector& GetPosition() const { return m_pos; }
	const WVector& GetTargetPosition() const { return m_targetPos; }

	int GetMotion() const { return m_motion; }

	unsigned long GetAction() const { return m_action; }

	bool GetHide() { return m_bHide; }

	void SubState(unsigned long state) { SetState(m_state & ~state); }

	void SetHeadIcon(int icon) { m_headIcon = icon; }

private:
	void SendMove(const WVector& pos, bool bForce);
	void ProcessMotion(float delta);
	void* AttachFx(const Attacher& attacher, const char* name, bool bLoop,
		defines_t* defines);
	void ProcessChangeState();
	void ScriptProcess(const w_motion_data* data, float from, float to,
		bool bLoop);
	void ParsePuppetScript(const char* script);
	void ParseScript_ShowVis(bool bShow, const char* name);
	void ParseBoundingBoxScript();
	sTexturePart* FindTexturePart(const char* part, const char* tex);
	void SetPendingTexPart(const char* part, const char* tex);

public:
	char m_idName[22];
	char m_nickName[22];
	char m_targetName[22];
	char m_followName[22];
	bool m_bShowProviderId;
	char m_providerId[128];
	int m_guid;
	bool m_bGM;
	unsigned char m_mark;
	bool m_bGoodPlayer;
	bool m_bAngelWing;
	unsigned long m_charTypeId;
	unsigned long m_state;
	unsigned long m_prevState;
	unsigned long m_action;
	WVector m_pos;
	WVector m_startPos;
	float m_angle;
	float m_targetAngle;
	bool m_bPendingTargetAngle;
	float m_pendingTargetAngle;
	WVector m_up;
	WVector m_targetPos;
	bool m_bTargetPosChanged;
	int m_headIcon;
	WVector m_moveDir;
	CRealMyRoom_AvatarBurden m_burden;
	bool m_bCharInfoChanged;
	sCharacterInfo m_charInfo;
	CPartTidList m_partTidList;
	CPetFrame* m_pPetFrame;
	std::list<sTexturePart*> m_texPartList;
	float m_alpha;
	LightSet m_light;
	bool m_bHide;
	w_motion_data* m_pMotionData;
	int m_motion;
	WBoneSet m_bones;
	w_motion_data* m_pPrevMotionData;
	int m_prevMotion;
	WBoneSet m_prevBones;
	float m_blendTime;
	float m_motionTime;
	float m_elapsedTime;
	float m_facialTime;
	unsigned long m_lastMotionSendTime;
	ntBalloonText* m_pBalloon;
	float m_random[3];
	float m_balloonTime;
	unsigned long m_balloonElapsed;
	char m_tradeTitle[64];
	const Bitmap* m_pEmblem;
	CRealMyRoomMain* m_pMain;
	CPvsFxBox m_fxBox;
};

class CRealMyRoom_AvatarManager : public WSingleton<CRealMyRoom_AvatarManager>
{
public:
	CRealMyRoom_AvatarManager();
	virtual ~CRealMyRoom_AvatarManager();

	CRealMyRoom_Avatar* CreateAvatar(const sSlotInfo& slot,
		const sCharacterInfo& info, CRealMyRoomMain* pMain);
	bool DeleteAvatar(const char* idName);
	void FreeAllAvatars();
	void Process(float delta);
	void Render();
	void RenderOverlay();
	void ResetAvatarPositionAngle();
	CRealMyRoom_Avatar* FindAvatarByIDName(const char* idName) const;
	CRealMyRoom_Avatar* FindAvatarByNickName(const char* nickName) const;
	CRealMyRoom_Avatar* FindAvatarByGuid(int guid) const;
	CRealMyRoom_Avatar* CheckClick(float x, float y) const;
	CRealMyRoom_Avatar* AvatarByRayIntersect(const WVector& from,
		const WVector& to) const;

	int m_avatarCount;
	CRealMyRoom_Avatar* m_avatar[30];
	CRealMyRoom_Avatar* m_pMyAvatar;
	CRealMyRoom_Avatar* m_pPickedAvatar;
	CRealMyRoom_Avatar* m_pClickedAvatar;
	bool m_bActive;
	bool m_bShowName;
};
