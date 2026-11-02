#pragma once

#include <vector>
#include <list>

#include "fx.h"
#include "wflag.h"

#include "parttidlist.h"

class CPetFrame;
class CFxSpray;
class CFxSequence;
struct sTexturePart;

enum eExhibitType
{
	EXHIBIT_CHARACTER = 0,
	EXHIBIT_CADDIE = 1,
	EXHIBIT_BALL = 2,
	EXHIBIT_CLUB = 3,
	EXHIBIT_MATCH = 4,
	EXHIBIT_ITEM = 5,
	EXHIBIT_MASCOT = 6,
	EXHIBIT_FURNITURE = 7,
	EXHIBIT_FURNITURE_BASE = 8,
	EXHIBIT_FURNITURE_WALL = 9,
	EXHIBIT_FURNITURE_FLOOR = 10,
	EXHIBIT_FURNITURE_ROOM = 11,
	EXHIBIT_CARD = 12,
	EXHIBIT_FURNITURE_TEXTURE = 13,
};

class CExhibition
{
public:
	CExhibition(float x, float y, float anchor, bool bAutoRecover);
	~CExhibition();

	bool SetModel(CPartTidList& parts, unsigned long* auxParts,
		const char* motion, float delay, int option);
	bool SetModel(unsigned long typeId);
	bool SetModel(const char* name, eExhibitType type);
	void ParseBoundingBoxScript();
	void AttachFx();
	void AttachAngelWingFx();

	void Process(float dt, bool bInput);
	int ProcessBongdariShop(float dt);
	void Display(float x, float y);
	void DisplayClip(const WRect& rect, float x, float y, float scale);
	void MoveModel(const WPoint& pos, const WRect* rect, float scale);

	bool SetMotion(const char* name, bool bContinue, float time);
	bool NextMotion();
	const char* GetMotionName();
	w_motion_data* GetMotionData();
	const char* GetNextCharMotion(int index);
	const char* GetNextCaddieMotion(int index);

	void SetCenter(float x, float y);
	const WVector2D& GetCenter();
	void SetArea(float radius);
	void SetArea(float width, float height);
	void SetDistance(float dist, bool bForce, bool bSetView);
	float GetDistance();
	void Zoom(float delta);
	void SetDirection(float dir);
	void SetDefaultDirection(float dir);
	bool IsDirectionChanged();
	void SetAlpha(unsigned char alpha);
	void SetChatFacial(int facial, unsigned long time);
	bool ChangeBoneTexture(const char* bone, const char* from, const char* to);
	bool ChangeBoneTexture(const char* from, const char* to);

	void ResetCameraRecoverTime() { m_recoverTime = 0.0f; }

	bool IsNextMotion() { return m_bNextMotion; }

	void SetDistanceRange(float fMin, float fMax)
	{
		m_minDist = fMin;
		m_maxDist = fMax;
	}
	void SetControlFlag(unsigned long flag) { m_controlFlag.Set(flag); }
	void SetAutoRecoverCamera(bool bEnable) { m_bAutoRecover = bEnable; }
	void SetEnableRotate(bool bEnable) { m_bEnableRotate = bEnable; }
	void SetEnableZoom(bool bEnable) { m_bEnableZoom = bEnable; }

	WPuppet* GetModel() { return m_pModel; }
	CPetFrame* GetPetFrame() { return m_pPetFrame; }
	float GetInitialMotionLength() { return m_motionLength; }

	unsigned long GetTypeId() const { return m_typeId; }
	float GetDirection() { return m_angle.y; }
	WMatrix& GetMatrix() { return m_modelMat; }

protected:
	void Init();
	void ReleasePet();
	void UpdateCamera(float dt);
	void Animate();
	bool InValidArea();
	float GetBaseAngle(float x, float y);
	void SetScreenPos(WPuppet* pPuppet);
	void ScriptProcess(const w_motion_data* motion, float from, float to,
		bool bInit);
	void ParsePuppetScript(const char* script);
	void ParseScript_ShowVis(bool bShow, const char* groups);
	sTexturePart* FindTexturePart(char* target, char* texture);
	void SetPendingTexPart(char* target, const char* texture);

	float m_delay;
	float m_motionLength;
	int m_motionIndex;
	int m_charIndex;
	int m_caddieIndex;
	int m_recoverState;
	bool m_bAutoRecover;
	bool m_bLButtonDown;
	bool m_bRButtonDown;
	float m_alpha;
	float m_prevYaw;
	WVector2D m_angle;
	WVector2D m_angleVel;
	WVector2D m_defaultAngle;
	float m_curDist;
	float m_dist;
	float m_defDist;
	float m_anchor;
	float m_recoverTime;
	float m_pitchBias;
	unsigned long m_reserved;
	float m_width;
	float m_height;
	WVector2D m_center;
	float m_radius;
	WMatrix m_camMat;
	float m_motionTime;
	w_motion_data* m_pMotion;
	CPetFrame* m_pPetFrame;
	WPuppet* m_pModel;
	WMatrix m_modelMat;
	int m_msgTexture;
	LightSet m_light;
	float m_minDist;
	float m_maxDist;
	CPvsFxBox m_fxBox;
	std::vector<CFxSpray*> m_bbSpray;
	std::vector<CFxSequence*> m_bbSeq;
	std::list<sTexturePart*> m_texParts;
	eExhibitType m_type;
	unsigned long m_typeId;
	unsigned long* m_pAuxParts;
	WFlags m_controlFlag;
	bool m_bNextMotion;
	float m_dirTime;
	float m_facialTime;
	bool m_bDirChanged;
	bool m_bChatFacial;
	bool m_bUnused;
	CPartTidList m_parts;
	CFxSequence* m_pWingR;
	CFxSequence* m_pWingL;
	bool m_bEnableZoom;
	bool m_bEnableRotate;
};
