#pragma once

#include <string.h>
#include <string>
#include <vector>
#include <list>
#include "netsyncactor.h"
#include "util.h"
#include "scenemanager.h"
#include "polysoup.h"

class GroundItem;
class CGimmickContainer;

class GroundItemMan : public NetSyncActor, public CRenderFuncPtr
{
	friend class GroundItem;
	friend class CPangInfo;

	DECLARE_OBJECT(GroundItemMan)

public:
	GroundItemMan();
	virtual ~GroundItemMan();

	virtual void OnLoad();
	virtual void OnReload();
	virtual void OnInit();
	virtual void OnDestroy();
	virtual void HandleMsg(const MsgObject& msg);

	struct PlayerStats
	{
		PlayerStats() { Reset(); }

		void Reset() { memset(this, 0, sizeof(PlayerStats)); }

		int numAcquired[3];
		int numAcquiredLive[3];
	};

	virtual void NSC_OnStep(bool bForce, bool bReplay, float step, float time,
		int ballState);
	virtual void NSC_OnFirstNewShotFrame(float time, int ballState);
	virtual void NSC_OnFirstReplayFrame(int ballState);
	virtual void OnHoleOut();

	virtual void Display();
	virtual void DebugDisplay();

protected:
	virtual int DisposeGroundItem(FieldItem::eDisposeTextureType texType,
		const std::vector<WPolySoup::sTriangle*>* triArray,
		std::vector<GimmickDispositionInformation>& dispInfo);
	virtual GimmickDispositionInformation* FindUnusedDisposeInfo(
		FieldItem::eDisposeTextureType texType,
		std::vector<GimmickDispositionInformation>& dispInfo) const;
	virtual GroundItem* CreateGroundItem(unsigned long id,
		GIMMICK_ITEM_TYPE type, FieldItem::eDisposeTextureType texType,
		const Ut::Spatial& spatial);
	void EnableGroundItemAll(bool bEnable);

private:
	static unsigned int ms_initCnt;

	std::list<GroundItem*> m_groundItemList;
	CGimmickContainer* m_pGimmickContainer;
	std::string m_sceneElemName;
	PlayerStats* m_pPlayerStats;
};
