#include "grounditemman.h"
#include "grounditem.h"
#include "quadtree.h"
#include "contentsdoc.h"
#include "golfdoc.h"
#include "projectg.h"
#include "wresrcmng.h"
#include "mathconsts.h"

IMPLEMENT_ACTOR(GroundItemMan, NetSyncActor)

unsigned int GroundItemMan::ms_initCnt = 0;

GroundItemMan::GroundItemMan()
{
	CContentsDoc::Instance()->GetContainer((localContentType_t)0xa3,
		m_pGimmickContainer);
}

GroundItemMan::~GroundItemMan()
{
}

void GroundItemMan::OnLoad()
{
	if (!OnlinePlay())
		m_pGimmickContainer->LoadFromScript("test_table");

	m_pPlayerStats = new PlayerStats[Doc()->m_pGolfDoc->m_playerNum];
}

void GroundItemMan::OnReload()
{
}

void GroundItemMan::OnInit()
{
	NetSyncActor::OnInit();

	m_pGimmickContainer->ResetGimmikFlag(Doc()->m_pGolfDoc->m_currentHole,
		GimmickDispositionInformation::GIMMICK_UNUSED);

	Ut::MakeObjectName(m_sceneElemName, "SceneElemName", *this, ms_initCnt++);

	CSceneManager::Instance()->RegisterElement(OBJ_CUSTOM, NULL, this,
		m_sceneElemName.c_str(), 0, NULL);

	NSA_SetInitialRandomSeed(m_pGimmickContainer->GetRandomSeed());

	NSA_GetRandomGenerator().seed(m_initialSeed);

	WList<w_texlist*>& texList =
		(WList<w_texlist*>&)g_resrcmng->GetTextureList();
	w_texlist* tex = texList.Start();

	while (tex)
	{
		FieldItem::eDisposeTextureType texType =
			FieldItem::DISPOSE_TEXTURE_NONE;
		if (m_pGimmickContainer->NeedToMakeTriPtArray(tex->texname, &texType))
		{
			const std::vector<WPolySoup::sTriangle*>* triArray =
				GetPVS().GetTriArray(
					g_resrcmng->FindTexture(tex->texname)->texhandle);
			DisposeGroundItem(texType, triArray,
				m_pGimmickContainer->GetGimmickPosition(
					GOLFDOC()->m_currentHole));
		}

		tex = texList.Next();
	}

	std::list<GroundItem*>::iterator it = m_groundItemList.begin();

	while (it != m_groundItemList.end())
	{
		GroundItem* pItem = *it;
		pItem->Initialize();
		++it;
	}
}

void GroundItemMan::OnDestroy()
{
	delete[] m_pPlayerStats;

	m_pGimmickContainer->Release();

	OnHoleOut();

	NetSyncActor::OnDestroy();
}

void GroundItemMan::HandleMsg(const MsgObject& msg)
{
	NetSyncActor::HandleMsg(msg);

	switch (msg.message)
	{
	case 0x277:
		*(std::vector<FieldItem::AcquiredItemInfo>**)msg.param1 =
			&m_pGimmickContainer->GetAcquiredItemInfoArray();
		break;

	case 0x27c:
		*(int*)msg.param1 = m_groundItemList.size();
		break;

	case 0x27b:
		EnableGroundItemAll(msg.param1 == 1);
		break;
	}
}

void GroundItemMan::NSC_OnStep(bool bForce, bool bReplay, float step,
	float time, int ballState)
{
	std::list<GroundItem*>::iterator it = m_groundItemList.begin();
	while (it != m_groundItemList.end())
	{
		GroundItem* pItem = *it;
		pItem->NSC_OnStep(bForce, bReplay, step, time, ballState);
		++it;
	}
}

void GroundItemMan::NSC_OnFirstNewShotFrame(float time, int ballState)
{
	bool bJackpot = false;
	unsigned long r = NSA_GetRandomGenerator().randInt(999999);

	if ((Doc()->m_userInfo[Doc()->m_pGolfDoc->m_currentPlayer]
				.userEquip.tidBall &
			0x3ffffff) == 0)
	{
		if (r < 20000)
			bJackpot = true;
	}
	else
	{
		if (r < 40000)
			bJackpot = true;
	}

	GroundItem::EnableGreenItemJackpot(bJackpot);

	PlayerStats& stats = m_pPlayerStats[Doc()->m_pGolfDoc->m_currentPlayer];
	for (int i = 0; i < 3; ++i)
		stats.numAcquired[i] = 0;

	std::list<GroundItem*>::iterator it = m_groundItemList.begin();
	while (it != m_groundItemList.end())
	{
		GroundItem* pItem = *it;
		pItem->NSC_OnFirstNewShotFrame(time, ballState);
		++it;
	}
}

void GroundItemMan::NSC_OnFirstReplayFrame(int ballState)
{
	std::list<GroundItem*>::iterator it = m_groundItemList.begin();

	while (it != m_groundItemList.end())
	{
		GroundItem* pItem = *it;
		pItem->NSC_OnFirstReplayFrame(ballState);
		++it;
	}
}

void GroundItemMan::OnHoleOut()
{
	std::list<GroundItem*>::iterator it = m_groundItemList.begin();

	while (it != m_groundItemList.end())
	{
		GroundItem* pItem = *it;
		pItem->Terminate();
		delete pItem;
		++it;
	}

	m_groundItemList.clear();

	CSceneManager::Instance()->DeleteElement(m_sceneElemName.c_str());
}

void GroundItemMan::Display()
{
}

void GroundItemMan::DebugDisplay()
{
	std::list<GroundItem*>::iterator it = m_groundItemList.begin();

	while (it != m_groundItemList.end())
	{
		GroundItem* pItem = *it;
		pItem->DrawBoundingVolume();
		++it;
	}

	const ThSphere& ballVolume = GroundItem::GetBallBoundingVolume();
	g_view->DrawSphere((const WVector&)ballVolume.pos, ballVolume.r, 0xffff0000,
		10, 0);
}

int GroundItemMan::DisposeGroundItem(FieldItem::eDisposeTextureType texType,
	const std::vector<WPolySoup::sTriangle*>* triArray,
	std::vector<GimmickDispositionInformation>& dispInfo)
{
	int numDisposed = 0;
	float minDist = m_pGimmickContainer->GetMinDistBtwItems();
	std::vector<WVector> posList;
	Ut::BuildAppropriatePosList(triArray, minDist, posList);

	int numPos = posList.size();
	int numDispInfo =
		m_pGimmickContainer->GetNumDisposeInfo(GOLFDOC()->m_currentHole,
			GIMMICK_ITEM_ANY, GimmickDispositionInformation::GIMMICK_UNUSED);

	if (posList.size() == 0)
		return 0;

	int numToDispose = numPos;
	switch (texType)
	{
	case FieldItem::DISPOSE_TEXTURE_BOOSTER:
		numToDispose = (int)(numPos * 0.7f);
		if (numToDispose > 50)
			numToDispose = 50;
		break;
	}

	if (numToDispose > numDispInfo)
		numToDispose = numDispInfo;

	int* indices = new int[numPos];
	for (int i = 0; i < numPos; ++i)
		indices[i] = i;

	for (int j = 0; j < numPos * 3; ++j)
	{
		int r = NSA_GetRandomGenerator().randInt(numPos - 1);
		std::swap(indices[j % numPos], indices[r]);
	}

	for (int k = 0; k < numToDispose; ++k)
	{
		GimmickDispositionInformation* pInfo =
			FindUnusedDisposeInfo(texType, dispInfo);
		if (pInfo == NULL)
			break;

		Ut::Spatial spatial;
		spatial.pos = (const gaV3&)posList[indices[k]];

		GroundItem* pItem =
			CreateGroundItem(pInfo->index, pInfo->type, texType, spatial);
		m_groundItemList.push_back(pItem);

		pInfo->flag = GimmickDispositionInformation::GIMMICK_USED;
		++numDisposed;
	}

	if (indices)
		delete indices;

	return numDisposed;
}

GimmickDispositionInformation* GroundItemMan::FindUnusedDisposeInfo(
	FieldItem::eDisposeTextureType texType,
	std::vector<GimmickDispositionInformation>& dispInfo) const
{
	GimmickDispositionInformation* pInfo = NULL;

	switch (texType)
	{
	case FieldItem::DISPOSE_TEXTURE_COIN:
		pInfo = m_pGimmickContainer->FindDisposeInfo(GOLFDOC()->m_currentHole,
			GIMMICK_ITEM_PANG, GimmickDispositionInformation::GIMMICK_UNUSED);
		break;

	case FieldItem::DISPOSE_TEXTURE_BOOSTER:
		pInfo = m_pGimmickContainer->FindDisposeInfo(GOLFDOC()->m_currentHole,
			GIMMICK_ITEM_ANY, GimmickDispositionInformation::GIMMICK_UNUSED);
		break;
	}

	return pInfo;
}

GroundItem* GroundItemMan::CreateGroundItem(unsigned long id,
	GIMMICK_ITEM_TYPE type, FieldItem::eDisposeTextureType texType,
	const Ut::Spatial& spatial)
{
	GroundItem* pItem = NULL;
	std::string modelName;
	Ut::Spatial newSpatial = spatial;

	switch (type)
	{
	case GIMMICK_ITEM_PANG:
		modelName = "coin.pet";
		newSpatial.scl = 10.0f;
		newSpatial.pos.y += 0.1f;
		pItem = new GroundItem(this, id, GIMMICK_ITEM_PANG, texType,
			modelName.c_str(), newSpatial);
		break;

	case GIMMICK_ITEM_BOX:
		modelName = "ct_event.pet";
		newSpatial.scl = 14.0f;
		newSpatial.pos.y -= 3.0f;
		pItem = new GroundItem(this, id, GIMMICK_ITEM_BOX, texType,
			modelName.c_str(), newSpatial);
		break;
	}

	return pItem;
}

void GroundItemMan::EnableGroundItemAll(bool bEnable)
{
	std::list<GroundItem*>::iterator it = m_groundItemList.begin();

	while (it != m_groundItemList.end())
	{
		(*it)->Enable(bEnable);
		++it;
	}
}
