#pragma once

#include <vector>

struct sCameraSelectionData
{
	int rate;
	bool bCheckVisible;
	bool bEnable;
};

class CCameraWork : public WSingleton<CCameraWork>
{
public:
	enum eCamMode
	{
		CAMMODE_HOLEIN,
		CAMMODE_RANK,
		CAMMODE_DRIVE,
		CAMMODE_INTRO,
	};

	CCameraWork();
	virtual ~CCameraWork();

	void Load();
	void ReplayCamera(bool bReplay);
	void SelectCamera(int type);
	void SetCaddieCamera(const WMatrix& rot, const WVector& pos);
	float LoadCamera(eCamMode mode, char* name, const WVector& pos);
	WVector GetSourceCamera();
	WVector GetDestCamera();
	void SetDestCamera(const WVector& dest);
	void SetInstalledCamera(const WVector& pos);
	void UninstallCamera();
	void InputProcess(float delta);
	void Process(float delta, float frame);
	bool CheckState();
	void StopBallSound();
	void EarthQuake();
	void Quake();
	WMatrix GetCameraMatrix(const WVector& src, const WVector& dest,
		float roll);

	int GetCameraType() { return m_type; }
	void SetCamera(WMatrix& mat) { m_matrix = mat; }

	void Lock() { m_bLock = true; }
	void Unlock()
	{
		if (m_bLock)
		{
			m_bLock = false;
			m_input.Reset();
		}
	}

	bool ToggleCameraChecking() { return m_bCheckCamera = !m_bCheckCamera; }

	bool Finished() { return m_finishTime > 0.0f && m_time > m_finishTime; }

	void SetCaddieChase(bool bChase) { m_bCaddieChase = bChase; }

	void SetReplayCount(int count) { m_replayCount = count; }

	void SetManualType(int type) { m_manualType = type; }

	void SetAbsAnim() { m_bAbsAnim = true; }

protected:
	WPuppet* FindHoleInCam(unsigned long charTypeId);
	void SetCamera(int type, bool bReplay);
	void CheckCameraType(bool bForce);
	void InitCtrlPoints(int type);
	void InitVars();
	bool CheckGround(WVector& src, WVector& dest);
	void ScriptProcess(w_motion_data* motion, float delta, float frame);
	void ParsePuppetScript(char* script);
	void ProcessBallSound();

private:
	void InterpolateBezier(WVector* ctrl, float t, WVector* out, int num);
	void CheckDriveCameraBallVisibilityAndSetBallVisibilitySafeCamera();
	void FollowCamera(float delta, float frame);
	void LookBackCamera(float delta, float frame);
	void Rail_or_SideCamera(float delta, float frame);
	void PanCamera(float delta, float frame);
	void DropCamera(float delta, float frame);
	void Chase_and_FollowCamera(float delta, float frame);
	void CircleCamera(float delta, float frame);
	void Chase_and_LookBackCamera(float delta, float frame);
	void ZoomInCamera(float delta, float frame);
	void CaddyViewCamera(float delta, float frame);
	void Drop_and_ZoomCamera(float delta, float frame);
	void BezierCamera(float delta, float frame);
	void FollowWithSwingCamera(float delta, float frame);
	void SidePanWithZoomInCamera(float delta, float frame);
	void FromMidPointCamera(float delta, float frame);
	void FromColPointCamera(float delta, float frame);
	void PangyaACamera(float delta, float frame);
	void FixedAtSideCamera(float delta, float frame);
	void FromRandomPointCamera(float delta, float frame);
	void BankFromHeadCamera(float delta, float frame);
	void CobraACamera(float delta, float frame);
	void CobraBCamera(float delta, float frame);
	void TomahawkACamera(float delta, float frame);
	void SpikeACamera(float delta, float frame);
	void TopViewCamera(float delta, float frame);
	void JustLookingCamera(float delta, float frame);
	void SideCamera(float delta, float frame);
	void BallSideCamera(float delta, float frame);
	void FollowBallCamera(float delta, float frame);
	void FixedCamera(float delta, float frame);
	void PuttSecondACamera(float delta, float frame);
	void PuttSecondBCamera(float delta, float frame);
	void FromHoleCupCamera(float delta, float frame);
	void IntroACamera(float delta, float frame);
	void IntroBCamera(float delta, float frame);
	void ZoomOutCamera(float delta, float frame);
	void IntroCCamera(float delta, float frame);
	void IntroDCamera(float delta, float frame);
	void ToDefaultCamera(float delta, float frame);
	void FirstPersonCamera(float delta, float frame);
	void StillFrameCamera(float delta, float frame);
	void CaddieChanceCamera(float delta, float frame);
	void SwingReplayACamera(float delta, float frame);
	void SwingReplayBCamera(float delta, float frame);
	void ExtraTimeCamera(float delta, float frame);

public:
	struct sInput
	{
		float fov;
		WVector2D rot;
		bool bActive;

		void Reset()
		{
			bActive = false;
			fov = 0.0f;
			rot.Reset();
		}
	};

	struct sCharCam
	{
		unsigned long charTypeId;
		WPuppet* pPuppet;
	};

protected:
	sInput m_input;
	void (CCameraWork::*m_pfnCamera)(float delta, float frame);
	WVector m_src[3];
	WVector m_dest;
	int m_driveNum;
	int m_puttNum;
	int m_driveRateSum;
	int m_puttRateSum;
	std::vector<int> m_enableList;
	int m_type;
	int m_selectType;
	int m_nextType;
	unsigned long m_state;
	float m_topFrame;
	float m_contactFrame;
	float m_lastFrame;
	float m_startFrame;
	float m_curFrame;
	int m_colFrame;
	int m_latchedFrame;
	int m_frame;
	int m_hideFrame;
	WMatrix m_inputMatrix;
	WMatrix m_matrix;
	WVector m_ctrlPoint[4];
	unsigned char m_reserved150[0x30];
	WVector m_srcPos;
	float m_lift;
	float m_time;
	float m_quakeStepTime;
	float m_quakeTime;
	float m_soundTime;
	bool m_bReserved1a0;
	bool m_bPassHole;
	bool m_bPanSound;
	int m_ballSoundState;
	float m_angle;
	WMatrix m_caddieRot;
	bool m_bLock;
	bool m_bCheckCamera;
	bool m_bCaddieChase;
	bool m_bScript;
	WVector m_scriptPos;
	sCharCam m_charCam[4];
	WPuppet* m_pRankPuppet;
	WPuppet* m_pPuppet;
	w_motion_data* m_pMotion;
	WVector m_oldBallPos;
	WQuat m_srcRot;
	WQuat m_destRot;
	WVector m_srcTrans;
	WVector m_destTrans;
	float m_distance;
	float m_height;
	float m_finishTime;
	float m_fov;
	bool m_bCheckGround;
	bool m_bGreenFov;
	std::vector<int> m_unusedList;
	unsigned long m_reserved280;
	std::vector<int> m_replayList;
	int m_replayIndex;
	int m_rand;
	bool m_bInstalled;
	float m_roll;
	int m_replayCount;
	WVector* m_pPivot;
	int m_manualType;
	bool m_bPitchLimit;
	bool m_bAbsAnim;
};
