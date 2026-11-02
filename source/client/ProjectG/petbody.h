#pragma once

#include <list>
#include <string>
#include <vector>
#include "wpuppet.h"

class IActor;
struct sTexturePart;

class CPetBody
{
public:
	enum eBody
	{
		BODY_HEAD = 0, // names guessed
		BODY_ROOT = 1,
		BODY_ROOT_POS = 2,
	};

	enum eFlag
	{
		FLAG_DELTA_VALID = 0x1,
		FLAG_MOVE = 0x2,
		FLAG_PLAY = 0x4,
		FLAG_UPDATE = 0x10,
		FLAG_LOOKUP = 0x20,
		FLAG_HIDE = 0x2000,
	};

	struct ScriptHandle
	{
		const char* name;
		IActor* actor;
		int param;
	};

	struct sLightSet
	{
		LightSet light;
		WVector pos;
	};

	CPetBody(const char* script, bool bOption);
	virtual ~CPetBody();

	virtual void Display();
	virtual void DisplayShadow(char* recvName);
	virtual void Process(float dt);

	void InitScript(const char* script, bool bOption);
	void Reset();
	void SetAlpha(unsigned char alpha)
	{
		if (m_pPet)
			m_pPet->SetAlpha(alpha);
	}
	void SetLight();
	void AddLocalLight(const LightSet& light, WVector pos);
	void ActiveGlobalLight(bool bActive) { m_bGlobalLight = bActive; }
	void AddScriptHandle(const char* name, IActor* actor, int param);

	void SetPos(const WVector& pos);
	void SetScale(float scale);
	void TakeOff(WVector target);
	const WVector& GetPos() const { return m_pos; }
	const WVector& GetVel() const { return m_vel; }
	WVector GetDirection() const { return m_mat.za * -1.0f; }
	void SetRotate(const WVector& rot);
	void SetRotate_Lean(const WVector& rot);
	void RestoreLeanMat() { m_leanMat = m_mat; }
	void SetMoveScale(float scale) { m_moveScale = scale; }
	void SetRotate(float angle);
	void SetRotateSlope(float angle, WVector normal);
	void Rotate(float angle);
	void MultRotate(WMatrix* mat);
	void Update();
	WMatrix GetMatrix(eBody body);
	const Waabb& GetBoundBox() const { return m_pPet->GetBoundBox(); }
	WPuppet* GetPet();
	void SetMotion(const char* name, bool bMove, float time, bool bApplyDelta);
	void SetNextMotion(const char* name, bool bMove);
	float GetAnimationLength(const char* name);
	void LookUp(WVector* target, bool bTurn);
	void ChangeTexture(char* name, int index);
	void SetCustomGround(char* name);
	const char* GetMotionName() { return m_pMotion->name; }
	void SetPendingTexPart(char* primary, char* secondary);

protected:
	void Init();
	void LoadPuppet(const char* name, bool bOptimizeBone, bool bOption);
	void SetPivot(const char* name, WVector offset);
	void SetShadowType(int type);
	bool ShouldDisplay();
	float Move(float dt);
	void Animation(float dt);
	void ScriptProcess(const w_motion_data* motion, float from, float to);
	void ParsePuppetScript(const char* script);
	void PlaySfx(char* name);
	void FlightProcess(float dt);
	void LookUpProcess(float dt);
	sTexturePart* FindTexturePart(char* primary, char* secondary);

	WPuppet* m_pPet;
	int m_shadowType;
	const w_motion_data* m_pMotion;
	float m_time;
	WMatrix m_mat;
	WMatrix m_leanMat;
	WVector m_rot;
	WVector m_pos;
	WVector m_vel;
	float m_scale;
	int m_flightState;
	bool m_bFly;
	WVector m_flightTarget;
	WVector m_flightStart;
	float m_flightHeight;
	float m_flightSpeed;
	float m_leanAngle;
	WFlags m_flags;
	WList<ScriptHandle*> m_scriptHandle;
	std::vector<sLightSet> m_localLight;
	bool m_bLocalLight;
	bool m_bGlobalLight;
	LightSet m_light;
	std::string m_customGround;
	WVector m_delta;
	float m_aniSpeed;
	float m_moveScale;
	char m_nextMotion[32];
	bool m_bNextMotionMove;
	std::list<sTexturePart*> m_texPart;
	WVector* m_pLookAt;
	bool m_bLookTurn;
	WVector2D m_lookAngle;
	WVector2D m_lookDest;
};
