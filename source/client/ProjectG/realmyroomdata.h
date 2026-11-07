#pragma once

#include <list>

#include "frcmdtarget.h"
#include "polysoup.h"

#include "../../shared/globalgamedefine.h"

class cModel
{
public:
	cModel();
	~cModel();

	void SetPosition(const WVector& pos);
	void SetRotate(float angle);
	void Apply_ChangeModelData();
	void OBBToAABB();

	int m_nState;
	sFurniture_List* m_pFurniture;
	WPolySoup::sModel* m_pModel;
	float m_fRotate;
	int m_nWallIndex;
};

class CRealMyRoomData : public WSingleton<CRealMyRoomData>, public FrCmdTarget
{
public:
	CRealMyRoomData();
	virtual ~CRealMyRoomData();

	void OnDestory();
	void Process(float elapsed);
	void Render();
	void OnLoad();
	bool AddModel(sFurniture_List furniture);
	bool SaveModel(bool bSave);
	cModel* CreateModel(const char* name);
	cModel* CreateModel(const char* name, WVector pos, float angle,
		bool bRename, short type);
	void ResotreList();
	void DeleteList();
	void ResetList(bool all);
	cModel* ModelByRayIntersect(const WVector& org, const WVector& dir);
	cModel* ModelByRayIntersectOBB(const WVector& pos);
	void ChangeFloorTexture(sFurniture_List* pList, bool bReset);
	void ChangeWallTexture(sFurniture_List* pList, bool bReset);
	bool ChangePosterTexture(unsigned long typeId, WPolySoup::sModel& model);
	void SetRotate(float angle);
	void SetTargetPosition(const WVector& pos, unsigned char mode);
	void MoveModelPosition(unsigned char mode);
	WVector ChangeModelPosition(int mode, cModel* pModel,
		WPolySoup::sModel* pData);
	WVector SlideWallPosition(cModel* pModel, WPolySoup::sModel* pData);
	void SetModelPosition();
	unsigned long GetState();
	void SetState(unsigned long state);
	void ListFurnitureSelect(sFurniture_List* pList);
	bool IsArragementFurniture(unsigned long id);
	cModel* GetModelData(unsigned long id, unsigned long typeId);
	bool IsModelData_SaveList(unsigned long id, unsigned long typeId);
	int IsFunritureValueChange(cModel& model);

	WList<cModel*> m_modelList;
	std::list<sFurniture_List> m_saveList;
	cModel* m_pSelModel;
	cModel* m_pCameraModel;
	cModel m_backupModel;
	WVector m_targetPos;
	WVector m_startPos;
	WVector m_endPos;
	bool m_bCollision;
	unsigned long m_state;
};
