#pragma once

#include <string>
#include "netsyncactor.h"
#include "util.h"
#include "thgeometry.h"

class GroundItemMan;
class CGimmickContainer;
class WPuppet;

class GroundItem : public IObject, public NetSyncable
{
	DECLARE_OBJECT(GroundItem)

public:
	GroundItem();
	GroundItem(GroundItemMan* pMan, unsigned long id, GIMMICK_ITEM_TYPE type,
		FieldItem::eDisposeTextureType texType, const char* modelName,
		const Ut::Spatial& spatial);
	virtual ~GroundItem();

	enum eGroundItemState
	{
		GROUND_ITEM_IDLE,
		GROUND_ITEM_ATTRACTED,
		GROUND_ITEM_ACQUIRED,
		GROUND_ITEM_READY
	};

	enum eGoundItemUpdateType
	{
		UPDATE_PANG,
		UPDATE_BOX,
		UPDATE_MAGNET,
		UPDATE_NONE
	};

	virtual void Initialize();
	virtual void Terminate();
	virtual void DrawBoundingVolume();
	virtual void GetBoundingSphere(gaV3& center, float& radius) const;
	virtual void GetBoundingSphere(WVector& center, float& radius) const;

	virtual void NSC_OnStep(bool bForce, bool bReplay, float step, float time,
		int ballState);
	virtual void NSC_OnFirstNewShotFrame(float time, int ballState);
	virtual void NSC_OnFirstReplayFrame(int ballState);

	void Enable(bool bEnable);

	static const ThSphere& GetBallBoundingVolume();
	static void EnableGreenItemJackpot(bool bEnable);

protected:
	virtual void OnChangeState(int frame);
	virtual bool TestIntersection(int frame);
	virtual bool TestIntersection2(int frame, float radius);

	void Transform();
	eGroundItemState ChangeState(eGroundItemState state, int frame);
	void Update(eGoundItemUpdateType type, bool bForce, bool bReplay,
		float step, float time, int ballState);

public:
	struct BallInfo
	{
		BallInfo() { Reset(); }

		void Reset()
		{
			frame = -1;
			volume.pos = gaV3::MAX;
		}
		const ThSphere& MakeBallBoundingVolume(int frame);

		int frame;
		ThSphere volume;
	};

protected:
	static BallInfo ms_ballInfo;
	static unsigned int ms_initCnt;
	static bool ms_greenItemJackpot;

	GroundItemMan* m_pMan;
	unsigned long m_id;
	GIMMICK_ITEM_TYPE m_type;
	eGroundItemState m_state;
	bool m_bEnable;
	int m_stateFrame[3];
	FieldItem::eDisposeTextureType m_texType;
	std::string m_modelName;
	WPuppet* m_pPuppet;
	std::string m_sceneElemName;
	Ut::Spatial m_spatial;
	WMatrix m_mat;
	CGimmickContainer* m_pGimmickContainer;

public:
	bool NeedToUpdate() const
	{
		return m_bEnable && m_state != GROUND_ITEM_ACQUIRED;
	}
};
