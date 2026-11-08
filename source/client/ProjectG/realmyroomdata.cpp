#include "ntavatar.h"
#include "realmyroomdata.h"
#include "actor.h"
#include "commonutil.h"
#include "fresh.h"
#include "frform.h"
#include "frwndmanager.h"
#include "golfdoc.h"
#include "../../shared/localize.h"
#include "networksystem.h"
#include "packet.h"
#include "projectg.h"
#include "quadtree.h"
#include "scenemanager.h"
#include "shareddoc.h"
#include "taskmanager.h"
#include "wbone.h"
#include "wpuppet.h"
#include "wresrcmng.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;

CRealMyRoomData::CRealMyRoomData()
	: m_modelList(16, 16)
{
	m_modelList.Reset();

	m_pSelModel = NULL;
	m_pCameraModel = NULL;
	m_state = 0;

	m_targetPos = WVector(0.0f, -2.0f, 0.0f);
	m_endPos = m_startPos = m_targetPos;

	m_bCollision = false;
}

CRealMyRoomData::~CRealMyRoomData()
{
	std::list<sFurniture_List>::iterator it;

	for (it = m_saveList.begin(); it != m_saveList.end(); ++it)
	{
		sFurniture_List furniture = *it;

		if ((furniture.typeId & 0x3FF800) == 0x2800)
		{
			for (cModel* pModel = m_modelList.Start(); pModel;
				pModel = m_modelList.Next())
			{
				if (pModel->m_pFurniture->id == furniture.id)
				{
					if (pModel->m_pFurniture->bArrange != furniture.bArrange)
					{
						if (IsLocalContent((localContentType_t)0x66))
						{
							WSendPacket packet((enumClientPacket)0xcc);
							packet.Encode4(
								CSharedDoc::Instance()->m_stallItem.stallKey);
							packet.Encode1(furniture.bArrange);
							packet.Encode1(0);
							packet.Send(TO_GAME);

							AfxGetTask()->SendMsgToMainActor(
								MsgObject(0, 1, 0, 0, 0, 0, 0));
						}
						break;
					}
				}
			}
		}
	}

	ResetList(true);
}

void CRealMyRoomData::OnDestory()
{
	for (cModel* pModel = m_modelList.Start(); pModel;
		pModel = m_modelList.Next())
	{
		delete pModel;
	}
	m_modelList.Reset();
}

void CRealMyRoomData::Process(float elapsed)
{
	// HACK!
	(void)&g_CM_TO_WU;
}

void CRealMyRoomData::Render()
{
	Wobb ilPadding;

	static bool s_bFadeIn = true;
	static int s_alpha = 128;

	if (m_pSelModel == NULL)
	{
		return;
	}

	if (m_state != 1 && m_state != 2)
	{
		return;
	}

	if ((m_pSelModel->m_pFurniture->typeId & 0x3FF800) != 0x7000)
	{
		return;
	}

	CSceneManager::Instance()->SetAlpha(m_pSelModel->m_pModel->name,
		(unsigned char)s_alpha);

	if (s_bFadeIn == true)
	{
		s_alpha += 3;
	}
	else
	{
		s_alpha -= 3;
	}

	if (s_alpha > 253)
	{
		s_bFadeIn = false;
	}

	if (s_alpha < 128)
	{
		s_bFadeIn = true;
	}
}

void CRealMyRoomData::OnLoad()
{
	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		if (ItemManager()->m_FurnitureMap.size() != 0)
		{
			for (std::map<unsigned int, IFF_STRUCT::sFurniture>::iterator it =
					 ItemManager()->m_FurnitureMap.begin();
				it != ItemManager()->m_FurnitureMap.end(); ++it)
			{
				sFurniture_List furniture;
				memset(&furniture, 0, sizeof(furniture));

				furniture.typeId = (*it).second.c.TypeId;
				furniture.id = rand() % 100;
				furniture.bArrange = 0;
				furniture.x = (*it).second.Pos[0];
				furniture.y = (*it).second.Pos[1];
				furniture.z = (*it).second.Pos[2];
				furniture.r = (*it).second.Pos[3];

				furniture.reserved = (*it).second.COM[0];

				if (furniture.typeId)
				{
					AddModel(furniture);
				}
			}
		}
	}
	else
	{
		if (CSharedDoc::Instance()->m_furnitureList.size() != 0)
		{
			std::list<sFurniture_List>::iterator it;
			for (it = CSharedDoc::Instance()->m_furnitureList.begin();
				it != CSharedDoc::Instance()->m_furnitureList.end(); it++)
			{
				if (it == CSharedDoc::Instance()->m_furnitureList.end())
				{
					break;
				}

				IFF_STRUCT::sFurniture* pFurniture =
					ItemManager()->FindFurniture((*it).typeId);
				if (pFurniture)
				{
					char name[256];
					sprintf(name, "%s.pet", pFurniture->Data);

					WVector pos(pFurniture->Pos[0], pFurniture->Pos[1],
						pFurniture->Pos[2]);

					if (GOLFDOC()->m_pPolySoup->FindModel(name) == NULL)
					{
						cModel* pModel = CreateModel(name, pos,
							pFurniture->Pos[3], false, pFurniture->IsMove);
						if (pModel == NULL)
						{
							return;
						}

						m_modelList += pModel;
						m_backupModel = *pModel;

						GOLFDOC()->m_pPolySoup->CreateDataModel(
							pModel->m_pModel);

						pModel->m_pFurniture = &(*it);
						m_state = 1;
						return;
					}
				}
			}
		}
	}
}

bool CRealMyRoomData::AddModel(sFurniture_List furniture)
{
	std::list<sFurniture_List>::iterator it;

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		for (it = CSharedDoc::Instance()->m_furnitureList.begin();
			it != CSharedDoc::Instance()->m_furnitureList.end(); it++)
		{
			if ((*it).id == furniture.id)
			{
				break;
			}
		}

		if (it == CSharedDoc::Instance()->m_furnitureList.end())
		{
			CSharedDoc::Instance()->AddFurniture(furniture);
		}
	}

	IFF_STRUCT::sFurniture* pFurniture =
		ItemManager()->FindFurniture(furniture.typeId);

	if (furniture.typeId == 0 || pFurniture == NULL)
	{
		return true;
	}

	char name[0x100];
	sprintf(name, "%s.pet", pFurniture->Data);

	WVector pos(furniture.x, furniture.y, furniture.z);

	if (furniture.bArrange)
	{
		cModel* pModel;

		for (cModel* existing = m_modelList.Start(); existing;
			existing = m_modelList.Next())
		{
			if (existing && existing->m_pFurniture->id)
			{
				IFF_STRUCT::sFurniture* pOther = ItemManager()->FindFurniture(
					existing->m_pFurniture->typeId);

				if (pFurniture->IsFunction != 0 &&
					pOther->IsFunction == pFurniture->IsFunction &&
					pOther->IsOwn == 0)
				{
					FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "notify", 0);
					pForm->SetMessage(
						"\xc1\xdf\xba\xb9\xb5\xc8 \xb1\xe2\xb4\xc9\xc0\xc7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc3\xdf\xb0\xa1\xb7\xce \xb9\xe8\xc4\xa1\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xb5\xbf\xc0\xcf \xb1\xe2\xb4\xc9\xc0\xc7 \xb0\xa1\xb1\xb8\xb8\xa6 \xc1\xa6\xb0\xc5\xc7\xcf\xb0\xed \xb9\xe8\xc4\xa1\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4",
						false);
					pForm->Open(NULL, 3);
					return false;
				}
			}
		}

		if (GOLFDOC()->m_pPolySoup->FindModel(name) == NULL &&
			((furniture.typeId >> 11) & 0x7ff) != 8 &&
			((furniture.typeId >> 11) & 0x7ff) != 9)
		{
			pModel =
				CreateModel(name, pos, furniture.r, false, pFurniture->IsMove);

			if (pModel == NULL)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					MakeStr(
						"\xb0\xa1\xb1\xb8 \xbb\xfd\xbc\xba\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. error %d ",
						furniture.id),
					false);
				pForm->Open(NULL, 3);
				return false;
			}

			GOLFDOC()->m_pPolySoup->ChangeDataStructure(pModel->m_pModel);

			for (it = CSharedDoc::Instance()->m_furnitureList.begin();
				it != CSharedDoc::Instance()->m_furnitureList.end(); it++)
			{
				if ((*it).id == furniture.id)
				{
					break;
				}
			}

			if (it != CSharedDoc::Instance()->m_furnitureList.end())
			{
				pModel->m_pFurniture = &*it;
			}
			else
			{
				CSharedDoc::Instance()->AddFurniture(furniture);

				pModel->m_pFurniture->id = furniture.id;
				pModel->m_pFurniture->typeId = furniture.typeId;
				pModel->m_pFurniture->reserved = furniture.reserved;
				pModel->m_pFurniture->x = furniture.x;
				pModel->m_pFurniture->y = furniture.y;
				pModel->m_pFurniture->z = furniture.z;
				pModel->m_pFurniture->r = furniture.r;
				pModel->m_pFurniture->bArrange = furniture.bArrange;
			}

			m_modelList += pModel;
			m_state = 1;

			if ((pModel->m_pFurniture->typeId & 0x3FF800) == 0x7000)
			{
				ChangePosterTexture(pModel->m_pFurniture->typeId,
					*pModel->m_pModel);
				m_pSelModel = pModel;
				m_state = 2;
			}

			return true;
		}

		if (GOLFDOC()->m_pPolySoup->FindModel(name) != NULL &&
			pFurniture->IsOwn == 1)
		{
			pModel =
				CreateModel(name, pos, furniture.r, true, pFurniture->IsMove);

			if (pModel == NULL)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					MakeStr(
						"\xb0\xa1\xb1\xb8 \xbb\xfd\xbc\xba\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. error %d ",
						furniture.id),
					false);
				pForm->Open(NULL, 3);
				return false;
			}

			GOLFDOC()->m_pPolySoup->ChangeDataStructure(pModel->m_pModel);

			for (it = CSharedDoc::Instance()->m_furnitureList.begin();
				it != CSharedDoc::Instance()->m_furnitureList.end(); it++)
			{
				if ((*it).id == furniture.id)
				{
					break;
				}
			}

			if (it != CSharedDoc::Instance()->m_furnitureList.end())
			{
				pModel->m_pFurniture = &*it;
			}
			else
			{
				CSharedDoc::Instance()->AddFurniture(furniture);

				pModel->m_pFurniture->id = furniture.id;
				pModel->m_pFurniture->typeId = furniture.typeId;
				pModel->m_pFurniture->reserved = furniture.reserved;
				pModel->m_pFurniture->x = furniture.x;
				pModel->m_pFurniture->y = furniture.y;
				pModel->m_pFurniture->z = furniture.z;
				pModel->m_pFurniture->r = furniture.r;
				pModel->m_pFurniture->bArrange = furniture.bArrange;
			}

			m_modelList += pModel;

			if ((pModel->m_pFurniture->typeId & 0x3FF800) == 0x7000)
			{
				ChangePosterTexture(pModel->m_pFurniture->typeId,
					*pModel->m_pModel);
				m_pSelModel = pModel;
				m_state = 2;
			}

			return true;
		}

		unsigned long type = (furniture.typeId >> 11) & 0x7ff;
		if (type == 8)
		{
			bool bToggleOff = false;
			for (pModel = m_modelList.Start(); pModel;
				pModel = m_modelList.Next())
			{
				if ((pModel->m_pFurniture->typeId & 0x3FF800) == 0x4000)
				{
					if (furniture.id == pModel->m_pFurniture->id)
					{
						pModel->m_pFurniture->bArrange = 0;
						pModel->m_pFurniture = NULL;
						m_modelList -= pModel;
						delete pModel;
						bToggleOff = true;
						break;
					}

					pModel->m_pFurniture->bArrange = 1;
					m_modelList -= pModel;
					break;
				}
			}

			if (bToggleOff)
			{
				ChangeFloorTexture(&furniture, true);
				return true;
			}

			ChangeFloorTexture(&furniture, false);

			bool bFound = false;
			cModel* pNew = new cModel;
			furniture.bArrange = 1;

			std::list<sFurniture_List>::const_iterator end =
				CSharedDoc::Instance()->m_furnitureList.end();
			it = CSharedDoc::Instance()->m_furnitureList.begin();
			std::list<sFurniture_List>::iterator next = it;
			while (next != end)
			{
				if ((*it).typeId == furniture.typeId &&
					(*it).id == furniture.id)
				{
					pNew->m_pFurniture = &*it;
					bFound = true;
				}

				++next;
				end = CSharedDoc::Instance()->m_furnitureList.end();
				it = next;
			}

			if (!bFound)
			{
				ChangeFloorTexture(&furniture, true);

				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					MakeStr(
						"\xba\xae\xc1\xf6 \xb9\xd7 \xb9\xd9\xb4\xda\xc1\xa6 \xb9\xd9\xb2\xd9\xb1\xe2 \xbd\xc7\xc6\xd0- \xb0\xa1\xc1\xf6\xb0\xed\xc0\xd6\xb4\xc2 \xb0\xa1\xb1\xb8\xbf\xa1\xbc\xad \xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
						furniture.id),
					false);
				pForm->Open(NULL, 3);
				return true;
			}
			m_modelList += pNew;
		}

		if (type == 9)
		{
			bool bToggleOff = false;
			for (pModel = m_modelList.Start(); pModel;
				pModel = m_modelList.Next())
			{
				if ((pModel->m_pFurniture->typeId & 0x3FF800) == 0x4800)
				{
					if (furniture.id == pModel->m_pFurniture->id)
					{
						pModel->m_pFurniture->bArrange = 0;
						pModel->m_pFurniture = NULL;
						m_modelList -= pModel;
						delete pModel;
						bToggleOff = true;
						break;
					}

					pModel->m_pFurniture->bArrange = 0;
					m_modelList -= pModel;
					break;
				}
			}

			if (bToggleOff)
			{
				ChangeWallTexture(&furniture, true);
				return true;
			}

			ChangeWallTexture(&furniture, false);

			bool bFound = false;
			cModel* pNew = new cModel;
			furniture.bArrange = 1;

			std::list<sFurniture_List>::const_iterator end =
				CSharedDoc::Instance()->m_furnitureList.end();
			it = CSharedDoc::Instance()->m_furnitureList.begin();
			std::list<sFurniture_List>::iterator next = it;
			while (next != end)
			{
				if ((*it).typeId == furniture.typeId &&
					(*it).id == furniture.id)
				{
					pNew->m_pFurniture = &*it;
					bFound = true;
				}

				++next;
				end = CSharedDoc::Instance()->m_furnitureList.end();
				it = next;
			}

			if (!bFound)
			{
				ChangeWallTexture(&furniture, true);

				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					MakeStr(
						"\xba\xae\xc1\xf6 \xb9\xd7 \xb9\xd9\xb4\xda\xc1\xa6 \xb9\xd9\xb2\xd9\xb1\xe2 \xbd\xc7\xc6\xd0- \xb0\xa1\xc1\xf6\xb0\xed\xc0\xd6\xb4\xc2 \xb0\xa1\xb1\xb8\xbf\xa1\xbc\xad \xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
						furniture.id),
					false);
				pForm->Open(NULL, 3);
				return true;
			}
			m_modelList += pNew;
		}
		else
		{
			return true;
		}
	}
	else
	{
		for (it = CSharedDoc::Instance()->m_furnitureList.begin();
			it != CSharedDoc::Instance()->m_furnitureList.end(); it++)
		{
			if ((*it).id == furniture.id)
			{
				break;
			}
		}

		if (it == CSharedDoc::Instance()->m_furnitureList.end())
		{
			CSharedDoc::Instance()->AddFurniture(furniture);
		}
		return true;
	}

	return true;
}

bool CRealMyRoomData::SaveModel(bool bSave)
{
	WList<cModel*> posterList(16, 16);
	bool bCollide = false;
	cModel* pModel;

	for (pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next())
	{
		IFF_STRUCT::sFurniture* pFurniture =
			ItemManager()->FindFurniture(pModel->m_pFurniture->typeId);

		unsigned long kind = (pModel->m_pFurniture->typeId >> 11) & 0x7ff;
		if (kind == 8 || kind == 9)
		{
			continue;
		}

		if (pModel->m_pModel == NULL)
		{
			continue;
		}

		m_targetPos = pModel->m_pModel->pos;

		trace_s trace;
		WVector delta;
		if (pFurniture->IsMove == 1 || pFurniture->IsMove == 2 ||
			pFurniture->IsMove == 3)
		{
			GetPVS().TraceOBB_Avatar_genPosition(pModel->m_pModel->pos,
				m_targetPos, &trace, pModel->m_pModel);

			if (trace.endpos.x == pModel->m_pModel->pos.x &&
				trace.endpos.z == pModel->m_pModel->pos.z)
			{
				m_targetPos = pModel->m_pModel->pos;
			}
			else
			{
				m_targetPos = trace.endpos;
			}

			GetPVS().TraceOBB_WallSliding(pModel->m_pModel->pos, m_targetPos,
				&trace, pModel->m_pModel, 0, false);

			delta = trace.endpos - pModel->m_pModel->pos;

			bCollide = GetPVS().TraceOBB_Object(pModel->m_pModel->pos,
				delta + pModel->m_pModel->pos, pModel->m_pModel->aabb, &trace,
				pModel->m_pModel, false);
		}
		else if (pFurniture->IsMove == 5)
		{
			if ((pModel->m_pFurniture->typeId & 0x3FF800) == 0x7000)
			{
				posterList += pModel;
			}
			else
			{
				GetPVS().TraceOBB_WallSliding(pModel->m_pModel->pos,
					m_targetPos, &trace, pModel->m_pModel, 1, false);

				delta = trace.endpos - pModel->m_pModel->pos;
				m_targetPos = delta + pModel->m_pModel->pos;

				bCollide = GetPVS().TraceOBB_Object(pModel->m_pModel->pos,
					m_targetPos, pModel->m_pModel->aabb, &trace,
					pModel->m_pModel, false);
			}
		}

		if (bCollide)
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"rmr_notify", 0);
			pForm->SetMessage(
				MakeStr(
					"[\xc0\xfa\xc0\xe5\xbd\xc7\xc6\xd0]\n\xc3\xe6\xb5\xb9\xc7\xcf\xb4\xc2 \xb0\xa1\xb1\xb8\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."),
				false);
			pForm->Open(NULL, 3);
			return false;
		}
	}

	if (posterList.GetCount() > 0)
	{
		for (pModel = posterList.Start(); pModel; pModel = posterList.Next())
		{
			WVector point;
			Waabb selected;
			Waabb box;

			switch (pModel->m_nWallIndex)
			{
			case 0:
			{
				selected = pModel->m_pModel->aabb;
				selected.min.z = selected.max.z = 0.0f;

				for (cModel* pOther = m_modelList.Start(); pOther;
					pOther = m_modelList.Next())
				{
					if (pOther &&
						(pOther->m_pFurniture->typeId & 0x3FF800) == 0x7000 &&
						pOther->m_pFurniture->id != pModel->m_pFurniture->id &&
						pOther->m_nWallIndex == pModel->m_nWallIndex)
					{
						box = pOther->m_pModel->aabb;
						box.max.z = 0.0f;
						box.min.z = 0.0f;

						point = selected.min;
						if (box.IsInclude(point))
						{
							bCollide = true;
							break;
						}
						point = selected.min;
						point.x = selected.max.x;
						if (box.IsInclude(point))
						{
							bCollide = true;
							break;
						}
						point = selected.max;
						if (box.IsInclude(point))
						{
							bCollide = true;
							break;
						}
						point = selected.max;
						point.x = selected.min.x;
						if (box.IsInclude(point))
						{
							bCollide = true;
							break;
						}
					}
				}
				break;
			}
			case 1:
			case 2:
			{
				selected = pModel->m_pModel->aabb;
				selected.min.x = selected.max.x = 0.0f;

				for (cModel* pOther = m_modelList.Start(); pOther;
					pOther = m_modelList.Next())
				{
					if (pOther &&
						(pOther->m_pFurniture->typeId & 0x3FF800) == 0x7000 &&
						pOther->m_pFurniture->id != pModel->m_pFurniture->id &&
						pOther->m_nWallIndex == pModel->m_nWallIndex)
					{
						box = pOther->m_pModel->aabb;
						box.max.x = 0.0f;
						box.min.x = 0.0f;

						point = selected.min;
						if (box.IsInclude(point))
						{
							bCollide = true;
							break;
						}
						point = selected.min;
						point.z = selected.max.z;
						if (box.IsInclude(point))
						{
							bCollide = true;
							break;
						}
						point = selected.max;
						if (box.IsInclude(point))
						{
							bCollide = true;
							break;
						}
						point = selected.max;
						point.z = selected.min.z;
						if (box.IsInclude(point))
						{
							bCollide = true;
							break;
						}
					}
				}
				break;
			}
			}
		}

		if (bCollide)
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"rmr_notify", 0);
			pForm->SetMessage(
				MakeStr(
					"[\xc0\xfa\xc0\xe5\xbd\xc7\xc6\xd0]\n\xc3\xe6\xb5\xb9\xc7\xcf\xb4\xc2 \xb0\xa1\xb1\xb8\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."),
				false);
			pForm->Open(NULL, 3);
			return false;
		}
	}

	if (bSave)
	{
		m_saveList.clear();
	}

	std::list<sFurniture_List>::iterator it;
	if (IsLocalContent((localContentType_t)0x80))
	{
		if (bSave)
		{
			for (it = CSharedDoc::Instance()->m_furnitureList.begin();
				it != CSharedDoc::Instance()->m_furnitureList.end(); it++)
			{
				cModel* pSaved = GetModelData((*it).id, (*it).typeId);
				if (pSaved)
				{
					if (pSaved->m_pModel != NULL)
					{
						(*it).x = pSaved->m_pModel->pos.x;
						(*it).y = pSaved->m_pModel->pos.y;
						(*it).z = pSaved->m_pModel->pos.z;
					}
					else
					{
						(*it).x = pSaved->m_pFurniture->x;
						(*it).y = pSaved->m_pFurniture->y;
						(*it).z = pSaved->m_pFurniture->z;
					}
					(*it).r = pSaved->m_fRotate;
				}

				{
					sFurniture_List furniture = *it;
					m_saveList.push_back(furniture);
				}
			}
		}
	}
	else if (bSave)
	{
		for (it = CSharedDoc::Instance()->m_furnitureList.begin();
			it != CSharedDoc::Instance()->m_furnitureList.end(); it++)
		{
			sFurniture_List furniture = *it;
			m_saveList.push_back(furniture);
		}
	}

	if (!IsLocalContent((localContentType_t)0x80) || bSave)
	{
		m_pSelModel = NULL;
		m_state = 0;
	}

	return true;
}

cModel* CRealMyRoomData::CreateModel(const char* name)
{
	m_pSelModel = NULL;
	m_pCameraModel = NULL;

	if (!IsLocalContent((localContentType_t)0x56))
	{
		return NULL;
	}

	if (name == NULL)
	{
		return NULL;
	}

	WPuppet* pCenterPuppet = g_resrcmng->GetPuppet(name, false, true, false);

	pCenterPuppet->SetLightMode(0x4000000);
	w_bound_box* pBox = pCenterPuppet->m_bbList.Find("center");

	cModel* pModel = new cModel;
	pModel->m_pModel = new WPolySoup::sModel;

	pModel->m_pModel->type = 1;
	pModel->m_pModel->flag = 0;

	WMatrix mat;
	mat.Reset();
	pModel->m_pModel->mat = mat;

	pModel->m_pModel->scale = 1.0f;

	strncpy(pModel->m_pModel->name, name, 0x1F);

	bool bSubOrder = pModel->m_pModel->type != 0;

	pModel->m_pModel->pet =
		g_resrcmng->GetPuppet(pModel->m_pModel->name, true, bSubOrder, false);

	if (pBox != NULL)
	{
		pModel->m_pModel->aabb.Reset();
		pModel->m_pModel->aabb = pBox->info->aabb;

		pModel->m_pModel->obb = pBox->bone->GetMatrix();
		pModel->m_pModel->obb *=
			(pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;

		pModel->m_pModel->obb.center =
			pBox->info->spherePivot * pBox->bone->GetMatrix();

		pModel->m_pModel->pos =
			(pModel->m_pModel->aabb.max - pModel->m_pModel->aabb.min) * 0.5f;

		pModel->m_pModel->mat.pivot.x = pModel->m_pModel->pos.x;
		pModel->m_pModel->mat.pivot.y = pModel->m_pModel->pos.y;
		pModel->m_pModel->mat.pivot.z = pModel->m_pModel->pos.z;

		pModel->m_pModel->obb.center = pModel->m_pModel->pos;

		pModel->m_pModel->aabb.min += pModel->m_pModel->pos;
		pModel->m_pModel->aabb.max += pModel->m_pModel->pos;
	}

	pModel->m_pModel->pet->ApplyBones(NULL, pModel->m_pModel->mat);
	pModel->m_pModel->pet->UpdateBound(true, false);

	pModel->m_pModel->pet->UpdateLightSource(&g_lightset, false, 0);

	unsigned long flags = 0;
	if (pModel->m_pModel->type)
	{
		flags = 4;
	}

	if ((pModel->m_pModel->flag & 0x10) ||
		pModel->m_pModel->pet->GetBoundSphere().radius > 128.0f)
	{
		flags |= 2;
	}

	if (pModel->m_pModel->flag & 8)
	{
		flags |= 1;
	}

	CSceneManager::Instance()->RegisterElement((eObjType)3,
		pModel->m_pModel->pet, NULL, pModel->m_pModel->name, flags,
		&pModel->m_pModel->bbox);

	return pModel;
}

cModel* CRealMyRoomData::CreateModel(const char* name, WVector pos, float angle,
	bool bRename, short type)
{
	cModel* pModel = NULL;

	m_pSelModel = NULL;
	m_pCameraModel = NULL;

	if (!IsLocalContent((localContentType_t)0x56))
	{
		return NULL;
	}

	if (name == NULL)
	{
		return NULL;
	}

	WPuppet* pCenterPuppet = g_resrcmng->GetPuppet(name, false, true, false);

	if (pCenterPuppet == NULL)
	{
		return NULL;
	}

	pCenterPuppet->SetLightMode(0x4000000);
	w_bound_box* pBox = pCenterPuppet->m_bbList.Find("center");

	pModel = new cModel;
	pModel->m_pModel = new WPolySoup::sModel;

	pModel->m_pModel->type = 1;
	pModel->m_pModel->flag = 0;

	WMatrix mat;
	mat.Reset();
	pModel->m_pModel->mat = mat;

	pModel->m_pModel->scale = 1.0f;

	strncpy(pModel->m_pModel->name, name, 0x1F);

	bool bSubOrder = pModel->m_pModel->type != 0;

	pModel->m_pModel->pet =
		g_resrcmng->GetPuppet(pModel->m_pModel->name, true, bSubOrder, false);

	if (pBox != NULL)
	{
		pModel->m_pModel->aabb.Reset();
		pModel->m_pModel->aabb = pBox->info->aabb;

		pModel->m_pModel->obb = pBox->bone->GetMatrix();
		pModel->m_pModel->obb *=
			(pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
		pModel->m_pModel->obb.center =
			pBox->info->spherePivot * pBox->bone->GetMatrix();

		pModel->m_pModel->pos = pos;

		if (type == 2 || type == 3)
		{
			pModel->m_pModel->pos.y = 0.0f;
		}

		pModel->m_pModel->obb.center = pModel->m_pModel->pos;

		pModel->SetPosition(pModel->m_pModel->pos);

		pModel->m_fRotate = 0.0f;
		pModel->SetRotate(angle);

		if (type == 2 || type == 3)
		{
			pModel->m_pModel->obb.center.y = pBox->info->aabb.max.y * 0.5f;
		}
		else if (type == 5 || type == 6)
		{
			pModel->m_pModel->obb.center.y = pos.y;
		}
		else
		{
			pModel->m_pModel->obb.center.y = pBox->info->aabb.max.y * 0.5f;
		}

		pModel->m_pModel->type = 1;

		WVector& minimum = pModel->m_pModel->aabb.min;
		const WVector& position = pModel->m_pModel->pos;
		minimum.x += position.x;
		minimum.y += position.y;
		minimum.z += position.z;
		pModel->m_pModel->aabb.max += pModel->m_pModel->pos;
		pModel->OBBToAABB();
	}

	pModel->m_pModel->pet->ApplyBones(NULL, pModel->m_pModel->mat);
	pModel->m_pModel->pet->UpdateBound(true, false);

	pModel->m_pModel->pet->UpdateLightSource(&g_lightset, false, 0);

	if (bRename)
	{
		int index = 1;
		char sceneName[0x20];
		for (;;)
		{
			sprintf(sceneName, "%02d%s", index, pModel->m_pModel->name);

			bool bUsed = false;
			for (cModel* pOther = m_modelList.Start(); pOther;
				pOther = m_modelList.Next())
			{
				if (pOther && pOther->m_pModel)
				{
					if (stricmp(sceneName, pOther->m_pModel->name) == 0)
					{
						bUsed = true;
						index++;
						break;
					}
				}
			}

			if (!bUsed)
			{
				strncpy(pModel->m_pModel->name, sceneName, 0x20);
				break;
			}
		}
	}

	WVector wallPos;
	pModel->m_nWallIndex = GetPVS().Get_WallSlidingAngle(pModel->m_pModel->pos,
		pModel->m_pModel, wallPos);

	unsigned long flags = 0;
	if (pModel->m_pModel->type)
	{
		flags = 4;
	}

	if ((pModel->m_pModel->flag & 0x10) ||
		pModel->m_pModel->pet->GetBoundSphere().radius > 128.0f)
	{
		flags |= 2;
	}

	if (pModel->m_pModel->flag & 8)
	{
		flags |= 1;
	}

	CSceneManager::Instance()->RegisterElement((eObjType)3,
		pModel->m_pModel->pet, NULL, pModel->m_pModel->name, flags,
		&pModel->m_pModel->bbox);

	return pModel;
}

void CRealMyRoomData::ResotreList()
{
	ResetList(false);

	for (std::list<sFurniture_List>::iterator it = m_saveList.begin();
		it != m_saveList.end(); ++it)
	{
		sFurniture_List furniture = *it;

		if (furniture.bArrange)
		{
			std::list<sFurniture_List>& docList =
				CSharedDoc::Instance()->m_furnitureList;
			for (std::list<sFurniture_List>::iterator docIt = docList.begin();
				docIt != docList.end(); ++docIt)
			{
				if ((*docIt).typeId == (*it).typeId && (*docIt).id == (*it).id)
				{
					(*docIt).bArrange = 1;
					break;
				}
			}

			AddModel(furniture);
		}
	}
}

void CRealMyRoomData::DeleteList()
{
	if (m_pSelModel == NULL)
	{
		return;
	}

	if (m_pCameraModel)
	{
		m_pCameraModel = NULL;
	}

	if (!GOLFDOC()->m_pPolySoup->DeleteDataStructure(m_pSelModel->m_pModel))
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pForm->SetMessage(
			"\xbb\xe8\xc1\xa6\xc7\xd2 \xb0\xa1\xb1\xb8\xb8\xa6 \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 3);
		return;
	}

	CSceneManager::Instance()->DeleteElement(m_pSelModel->m_pModel->name);

	m_pSelModel->m_pModel->pet = NULL;
	m_modelList -= m_pSelModel;
	delete m_pSelModel->m_pModel;
	if (!m_pSelModel)
	{
		return;
	}

	delete m_pSelModel;
	m_pSelModel = NULL;
}

void CRealMyRoomData::ResetList(bool all)
{
	for (cModel* pModel = m_modelList.Start(); pModel;
		pModel = m_modelList.Next())
	{
		if (pModel && pModel->m_pModel)
		{
			CSceneManager::Instance()->DeleteElement(pModel->m_pModel->name);

			if (!GOLFDOC()->m_pPolySoup->DeleteDataStructure(pModel->m_pModel))
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					"\xbb\xe8\xc1\xa6\xc7\xd2 \xb0\xa1\xb1\xb8\xb8\xa6 \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open(NULL, 3);
				return;
			}
		}
	}

	ChangeWallTexture(NULL, true);
	ChangeFloorTexture(NULL, true);

	if (all)
	{
		for (cModel* pModel = m_modelList.Start(); pModel;
			pModel = m_modelList.Next())
		{
			if (pModel)
			{
				m_modelList -= pModel;

				if (pModel->m_pModel)
				{
					pModel->m_pModel->pet = NULL;
					if (pModel->m_pModel)
					{
						delete[] pModel->m_pModel;
						pModel->m_pModel = NULL;
					}
				}
				else if (pModel->m_pFurniture)
				{
					pModel->m_pFurniture = NULL;
				}

				delete pModel;
			}
		}
	}
	else
	{
		for (cModel* pModel = m_modelList.Start(); pModel;
			pModel = m_modelList.Next())
		{
			m_modelList -= pModel;

			if (pModel->m_pModel)
			{
				pModel->m_pModel->pet = NULL;
				if (pModel->m_pModel)
				{
					delete[] pModel->m_pModel;
					pModel->m_pModel = NULL;
				}
			}

			delete pModel;
		}
	}

	for (cModel* pModel = m_modelList.Start(); pModel;
		pModel = m_modelList.Next())
	{
		if (pModel)
		{
			delete pModel;
		}
	}
	m_modelList.Reset();

	m_modelList.Reset();
	m_pSelModel = NULL;
	m_pCameraModel = NULL;
}

cModel* CRealMyRoomData::ModelByRayIntersect(const WVector& origin,
	const WVector& dir)
{
	Waabb ilPadding;

	for (cModel* pModel = m_modelList.Start(); pModel;
		pModel = m_modelList.Next())
	{
		if (pModel && pModel->m_pModel)
		{
			if (RayIntersectAABB(pModel->m_pModel->aabb, origin, dir, NULL, 0) <
				1.0f)
			{
				return pModel;
			}
		}
	}

	return NULL;
}

cModel* CRealMyRoomData::ModelByRayIntersectOBB(const WVector& pos)
{
	if (m_state != 1)
	{
		return NULL;
	}

	for (cModel* pModel = m_modelList.Start(); pModel;
		pModel = m_modelList.Next())
	{
		if (pModel && pModel->m_pModel)
		{
			if (pModel->m_pModel->obb.IsInclude(pos))
			{
				return pModel;
			}
		}
	}

	return NULL;
}

void CRealMyRoomData::ChangeFloorTexture(sFurniture_List* pList, bool bReset)
{
	static int s_floorTextureIndex = 2;
	char newTex[256];
	char oldTex[256];

	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		if (s_floorTextureIndex > 6)
		{
			s_floorTextureIndex = 2;
		}

		sprintf(newTex, "my_room_01_%02d.dds", s_floorTextureIndex);
		sprintf(oldTex, "floor_a.dds");

		GOLFDOC()->m_pPolySoup->ChangeFloorTexture(oldTex, newTex);
	}
	else
	{
		IFF_STRUCT::sFurniture* pFurniture;
		if (bReset)
		{
			pFurniture = ItemManager()->FindFurniture(0x48004008);
		}
		else
		{
			pFurniture = ItemManager()->FindFurniture(pList->typeId);
		}

		if (pFurniture)
		{
			for (int i = 0; i < 3; i++)
			{
				if (stricmp(pFurniture->Tex[i], "") == 0 ||
					stricmp(pFurniture->Tex[i], "") == 0)
				{
					break;
				}

				sprintf(newTex, "%s.dds", pFurniture->Tex[i]);
				sprintf(oldTex, "%s.dds", pFurniture->OrgTex[i]);

				GOLFDOC()->m_pPolySoup->ChangeFloorTexture(oldTex, newTex);
			}
		}
	}

	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		s_floorTextureIndex++;
	}
}

void CRealMyRoomData::ChangeWallTexture(sFurniture_List* pList, bool bReset)
{
	static int s_wallTextureIndex = 2;
	char newTex[256];
	char oldTex[256];

	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		if (s_wallTextureIndex > 6)
		{
			s_wallTextureIndex = 2;
		}

		sprintf(newTex, "my_room_01_%02d.dds", s_wallTextureIndex);
		sprintf(oldTex, "+wall_a.dds");
	}
	else
	{
		IFF_STRUCT::sFurniture* pFurniture;
		if (bReset)
		{
			pFurniture = ItemManager()->FindFurniture(0x4800480E);
		}
		else
		{
			pFurniture = ItemManager()->FindFurniture(pList->typeId);
		}

		if (pFurniture)
		{
			for (int i = 0; i < 3; i++)
			{
				if (stricmp(pFurniture->Tex[i], "") == 0 ||
					stricmp(pFurniture->Tex[i], "") == 0)
				{
					break;
				}

				sprintf(newTex, "%s.dds", pFurniture->Tex[i]);
				sprintf(oldTex, "%s.dds", pFurniture->OrgTex[i]);

				GOLFDOC()->m_pPolySoup->ChangeObjectTexture(oldTex, newTex, -1);
			}
		}
	}

	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		s_wallTextureIndex++;
	}
}

bool CRealMyRoomData::ChangePosterTexture(unsigned long typeId,
	WPolySoup::sModel& model)
{
	char newTex[256];
	char oldTex[256];

	IFF_STRUCT::sFurniture* pFurniture = ItemManager()->FindFurniture(typeId);
	if (pFurniture)
	{
		for (int i = 0; i < 3; i++)
		{
			if (stricmp(pFurniture->Tex[i], "") == 0 ||
				stricmp(pFurniture->Tex[i], "") == 0)
			{
				break;
			}

			if ((typeId & 0x3FF800) == 0x7000)
			{
				sprintf(newTex, "%s.jpg", pFurniture->Tex[i]);
				sprintf(oldTex, "%s.jpg", pFurniture->OrgTex[i]);
			}
			else
			{
				sprintf(newTex, "%s.dds", pFurniture->Tex[i]);
				sprintf(oldTex, "%s.dds", pFurniture->OrgTex[i]);
			}

			GOLFDOC()->m_pPolySoup->ChangeObjectTexture(oldTex, newTex,
				model.id);
		}
	}

	return true;
}

void CRealMyRoomData::SetRotate(float angle)
{
	if (m_state != 3 && m_state != 7)
	{
		return;
	}

	if (angle == 0.0f)
	{
		return;
	}

	if (m_pSelModel == NULL)
	{
		return;
	}

	for (cModel* pModel = m_modelList.Start(); pModel;
		pModel = m_modelList.Next())
	{
		if (pModel == NULL || pModel->m_pModel == NULL ||
			stricmp(pModel->m_pModel->name, m_pSelModel->m_pModel->name) != 0)
		{
			continue;
		}

		Wobb obb;
		obb = pModel->m_pModel->obb;
		WMatrix mat;
		mat = pModel->m_pModel->mat;

		pModel->SetRotate(angle * 0.03f);
		pModel->OBBToAABB();

		WVector pos;
		pos = ChangeModelPosition(7, pModel, pModel->m_pModel);
		WPolySoup::sModel* pData = pModel->m_pModel;
		if (!WisEqual(pos, pData->pos, g_EPSILON))
		{
			pData->obb = obb;
			pModel->m_pModel->mat = mat;
			pModel->OBBToAABB();
			return;
		}

		pModel->Apply_ChangeModelData();
		m_pSelModel = pModel;
		return;
	}
}

void CRealMyRoomData::SetTargetPosition(const WVector& pos, unsigned char mode)
{
	if (m_state != 2)
	{
		return;
	}

	switch (mode)
	{
	case 1:
		m_startPos = m_pSelModel->m_pModel->pos;
		m_targetPos = pos;
		break;
	case 2:
		m_targetPos = pos;
		m_endPos = pos;
		MoveModelPosition(mode);
		break;
	case 3:
		m_bCollision = false;
		m_targetPos = pos;
		MoveModelPosition(mode);
		break;
	}
}

static void ClipVelocity(const WVector& in, const WVector& normal, WVector& out,
	float overbounce)
{
	float backoff = (in * normal) * overbounce;
	out = in - normal * backoff;
}

void CRealMyRoomData::MoveModelPosition(unsigned char mode)
{
	WVector move;

	WVector delta;
	delta.Reset();
	IFF_STRUCT::sFurniture* pFurniture;
	trace_s trace;
	WVector wallPos;

	if (m_pSelModel)
	{
		pFurniture =
			ItemManager()->FindFurniture(m_pSelModel->m_pFurniture->typeId);
	}

	if (pFurniture->IsMove == 1 || pFurniture->IsMove == 2 ||
		pFurniture->IsMove == 3)
	{
		WVector move;
		WVector delta;

		GetPVS().TraceOBB_Avatar_genPosition(m_pSelModel->m_pModel->pos,
			m_targetPos, &trace, m_pSelModel->m_pModel);

		if (trace.endpos.x == m_pSelModel->m_pModel->pos.x &&
			trace.endpos.z == m_pSelModel->m_pModel->pos.z)
		{
			m_targetPos = m_pSelModel->m_pModel->pos;
		}
		else
		{
			m_targetPos = trace.endpos;
		}

		GetPVS().TraceOBB_WallSliding(m_pSelModel->m_pModel->pos, m_targetPos,
			&trace, m_pSelModel->m_pModel, 0, false);

		const WVector& selPos = m_pSelModel->m_pModel->pos;
		delta.x = trace.endpos.x - selPos.x;
		delta.y = trace.endpos.y - selPos.y;
		delta.z = trace.endpos.z - selPos.z;
		move = delta;

		m_bCollision = GetPVS().TraceOBB_Object(m_pSelModel->m_pModel->pos,
			move + m_pSelModel->m_pModel->pos, m_pSelModel->m_pModel->aabb,
			&trace, m_pSelModel->m_pModel, false);

		WVector objDelta = trace.endpos - m_pSelModel->m_pModel->pos;
		m_targetPos = move + m_pSelModel->m_pModel->pos;

		if (mode == 2)
		{
			if (move.x == 0.0f && move.z == 0.0f)
			{
				move = delta;
			}
			else
			{
				move = objDelta;
			}
		}
		else if (mode == 3)
		{
			move = delta;
		}
		else
		{
			move = delta;
		}

		if (pFurniture->IsMove == 2 || pFurniture->IsMove == 3)
		{
			move.y = 0.0f;
		}

		m_pSelModel->m_pModel->pos += move;
		m_pSelModel->m_pModel->obb.center += move;

		m_pSelModel->SetPosition(m_pSelModel->m_pModel->pos);

		m_pSelModel->m_pModel->aabb.min += move;
		m_pSelModel->m_pModel->aabb.max += move;

		m_pSelModel->m_pModel->pet->ApplyBones(NULL,
			m_pSelModel->m_pModel->mat);
		m_pSelModel->m_pModel->pet->UpdateBound(true, false);
		m_pSelModel->Apply_ChangeModelData();
	}
	else if (pFurniture->IsMove == 5)
	{
		move.Reset();

		if (GetPVS().CheckPickRoom(m_targetPos))
		{
			int wall = GetPVS().Get_WallSlidingAngle(m_targetPos,
				m_pSelModel->m_pModel, wallPos);

			move = m_targetPos - m_pSelModel->m_pModel->pos;

			if (wall >= 0)
			{
				if (wall != m_pSelModel->m_nWallIndex &&
					m_pSelModel->m_nWallIndex >= 0)
				{
					m_pSelModel->m_nWallIndex = wall;
					delta.Reset();

					switch (wall)
					{
					case 0:
						if (m_pSelModel->m_fRotate != 0.0f)
						{
							if (m_pSelModel->m_fRotate == 360.0f)
							{
								m_pSelModel->SetRotate(-360.0f);
							}
							else
							{
								m_pSelModel->SetRotate(
									360.0f - m_pSelModel->m_fRotate);
							}
						}
						break;
					case 1:
						if (m_pSelModel->m_fRotate == 0.0f ||
							m_pSelModel->m_fRotate == 360.0f)
						{
							m_pSelModel->SetRotate(270.0f);
						}
						break;
					case 2:
						if (m_pSelModel->m_fRotate == 0.0f ||
							m_pSelModel->m_fRotate == 360.0f)
						{
							m_pSelModel->SetRotate(90.0f);
						}
						break;
					}

					delta = m_targetPos - m_pSelModel->m_pModel->pos;

					m_pSelModel->m_pModel->pos += delta;
					m_pSelModel->m_pModel->obb.center += delta;
					m_pSelModel->SetPosition(m_pSelModel->m_pModel->pos);
					m_pSelModel->OBBToAABB();

					m_pSelModel->m_pModel->pet->ApplyBones(NULL,
						m_pSelModel->m_pModel->mat);
					m_pSelModel->m_pModel->pet->UpdateBound(true, false);
					m_pSelModel->Apply_ChangeModelData();
				}

				GetPVS().TraceOBB_WallSliding(m_pSelModel->m_pModel->pos,
					m_targetPos, &trace, m_pSelModel->m_pModel,
					m_pSelModel->m_nWallIndex + 1, false);

				WPolySoup::sModel* pModel = m_pSelModel->m_pModel;
				move.x = trace.endpos.x - pModel->pos.x;
				move.y = trace.endpos.y - pModel->pos.y;
				move.z = trace.endpos.z - pModel->pos.z;

				pModel->pos += move;
				m_pSelModel->m_pModel->obb.center += move;
				m_pSelModel->SetPosition(m_pSelModel->m_pModel->pos);
				m_pSelModel->OBBToAABB();

				m_pSelModel->m_pModel->pet->ApplyBones(NULL,
					m_pSelModel->m_pModel->mat);
				m_pSelModel->m_pModel->pet->UpdateBound(true, false);
				m_pSelModel->Apply_ChangeModelData();

				if (((m_pSelModel->m_pFurniture->typeId >> 11) & 0x7FF) ==
						0xE &&
					mode == 2)
				{
					{
						int handle = m_pSelModel->m_nWallIndex;
						Waabb otherBox;
						Waabb box;

						switch (handle)
						{
						case 0:
						{
							box = m_pSelModel->m_pModel->aabb;
							box.max.z = 0.0f;
							box.min.z = 0.0f;

							for (cModel* pModel = m_modelList.Start(); pModel;
								pModel = m_modelList.Next())
							{
								if (pModel == NULL ||
									(pModel->m_pFurniture->typeId & 0x3FF800) !=
										0x7000 ||
									stricmp(pModel->m_pModel->name,
										m_pSelModel->m_pModel->name) == 0)
								{
									continue;
								}

								otherBox = pModel->m_pModel->aabb;
								otherBox.max.z = 0.0f;
								otherBox.min.z = 0.0f;

								move = box.min;
								if (otherBox.IsInclude(move))
								{
									m_bCollision = true;
									break;
								}
								move = box.min;
								move.x = box.max.x;
								if (otherBox.IsInclude(move))
								{
									m_bCollision = true;
									break;
								}
								move = box.max;
								if (otherBox.IsInclude(move))
								{
									m_bCollision = true;
									break;
								}
								move = box.max;
								move.x = box.min.x;
								if (otherBox.IsInclude(move))
								{
									m_bCollision = true;
									break;
								}
							}
						}
						break;
						case 1:
						case 2:
						{
							box = m_pSelModel->m_pModel->aabb;
							box.max.x = 0.0f;
							box.min.x = 0.0f;

							for (cModel* pModel = m_modelList.Start(); pModel;
								pModel = m_modelList.Next())
							{
								if (pModel == NULL ||
									(pModel->m_pFurniture->typeId & 0x3FF800) !=
										0x7000 ||
									stricmp(pModel->m_pModel->name,
										m_pSelModel->m_pModel->name) == 0)
								{
									continue;
								}

								otherBox = pModel->m_pModel->aabb;
								otherBox.max.x = 0.0f;
								otherBox.min.x = 0.0f;

								move = box.min;
								if (otherBox.IsInclude(move))
								{
									m_bCollision = true;
									break;
								}
								move = box.min;
								move.z = box.max.z;
								if (otherBox.IsInclude(move))
								{
									m_bCollision = true;
									break;
								}
								move = box.max;
								if (otherBox.IsInclude(move))
								{
									m_bCollision = true;
									break;
								}
								move = box.max;
								move.z = box.min.z;
								if (otherBox.IsInclude(move))
								{
									m_bCollision = true;
									break;
								}
							}
						}
						break;
						}
					}
				}
				else if (((m_pSelModel->m_pFurniture->typeId >> 11) & 0x7FF) !=
					0xE)
				{
					GetPVS().TraceOBB_Object(m_pSelModel->m_pModel->pos,
						m_targetPos, m_pSelModel->m_pModel->aabb, &trace,
						m_pSelModel->m_pModel, false);

					delta = trace.endpos - m_pSelModel->m_pModel->pos;

					if (delta.x == 0.0f && delta.y == 0.0f &&
						(m_pSelModel->m_pModel->pos.x != m_targetPos.x ||
							m_pSelModel->m_pModel->pos.y != m_targetPos.y))
					{
						m_bCollision = true;
					}
					else
					{
						m_bCollision = false;
					}

					if (mode == 2)
					{
						if (move.x == 0.0f && move.y == 0.0f)
						{
							memcpy(&move, &move, sizeof(move) + (mode & 0));
						}
						else
						{
							move = delta;
						}
					}
					else if (mode == 3)
					{
						memcpy(&move, &move, sizeof(move) + (mode & 0));
					}
					else
					{
						memcpy(&move, &move, sizeof(move) + (mode & 0));
					}
					move.z = 0.0f;

					m_pSelModel->m_pModel->pos += move;
					m_pSelModel->m_pModel->obb.center += move;
					m_pSelModel->SetPosition(m_pSelModel->m_pModel->pos);
					m_pSelModel->m_pModel->aabb.min += move;
					m_pSelModel->m_pModel->aabb.max += move;

					m_pSelModel->m_pModel->pet->ApplyBones(NULL,
						m_pSelModel->m_pModel->mat);
					m_pSelModel->m_pModel->pet->UpdateBound(true, false);
					m_pSelModel->Apply_ChangeModelData();
				}
			}
		}
	}

	if (m_bCollision && mode == 2)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pForm->SetMessage(
			"\xb0\xa1\xb1\xb8\xb0\xa1 \xc3\xe6\xb5\xb9\xb5\xc7\xb4\xc2 \xc0\xa7\xc4\xa1\xbf\xa1 \xb3\xf5\xbf\xa9\xbc\xad \xb4\xd9\xbd\xc3 \xb9\xe8\xc4\xa1\xc7\xcf\xbc\xc5\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 3);

		WVector move;
		move.Reset();

		for (std::list<sFurniture_List>::iterator it = m_saveList.begin();
			it != m_saveList.end(); ++it)
		{
			sFurniture_List furniture = *it;

			if (m_pSelModel->m_pFurniture->id != furniture.id)
			{
				continue;
			}

			move.x = furniture.x;
			move.y = furniture.y;
			move.z = furniture.z;
			if (pFurniture->IsMove == 2)
			{
				move.y = 0.0f;
			}

			m_pSelModel->SetRotate(furniture.r - m_pSelModel->m_fRotate);

			move = move - m_pSelModel->m_pModel->pos;

			m_pSelModel->m_pModel->pos += move;
			m_pSelModel->m_pModel->obb.center += move;

			m_pSelModel->SetPosition(m_pSelModel->m_pModel->pos);

			m_pSelModel->OBBToAABB();

			m_pSelModel->m_pModel->pet->ApplyBones(NULL,
				m_pSelModel->m_pModel->mat);
			m_pSelModel->m_pModel->pet->UpdateBound(true, false);
			m_pSelModel->Apply_ChangeModelData();

			m_pSelModel->m_nWallIndex = GetPVS().Get_WallSlidingAngle(
				m_pSelModel->m_pModel->pos, m_pSelModel->m_pModel, move);
			break;
		}
	}

	if (mode == 2 && m_pSelModel &&
		(m_pSelModel->m_pFurniture->typeId & 0x3FF800) == 0x7000)
	{
		CSceneManager::Instance()->SetAlpha(m_pSelModel->m_pModel->name, 0xFF);
	}
}

WVector CRealMyRoomData::ChangeModelPosition(int mode, cModel* pModel,
	WPolySoup::sModel* pData)
{
	WVector result = WVector::ZERO;
	WVector move;
	WVector dir;
	WVector newPos;

	Waabb roomBox;
	GetPVS().GetAABB(roomBox);

	switch (mode)
	{
	case 2:
	{
		dir = WVector(m_targetPos.x - pModel->m_pModel->pos.x,
			m_targetPos.y - pModel->m_pModel->pos.y,
			m_targetPos.z - pModel->m_pModel->pos.z);
		move = dir;
		move.z = move.y;
		move.y = 0.0f;
		dir = move;

		move.Normalize();

		move *= 17.0f;
		move = move * 0.04f;

		if (fabs(dir.x) < fabs(move.x))
		{
			move.x = dir.x;
		}
		if (fabs(dir.z) < fabs(move.z))
		{
			move.z = dir.z;
		}

		newPos = WVector(move.x + pModel->m_pModel->pos.x,
			move.y + pModel->m_pModel->pos.y, move.z + pModel->m_pModel->pos.z);

		trace_s trace;
		cModel* pm = *(cModel* volatile*)&pModel;
		if (!GetPVS().TraceAABB_BackGround(pm->m_pModel->pos, newPos,
				pm->m_pModel->aabb, &trace, pData))
		{
			break;
		}

		GetPVS().TraceOBB_Object(pm->m_pModel->pos, newPos, pm->m_pModel->aabb,
			&trace, pData, false);

		move = trace.endpos - pm->m_pModel->pos;
		move.y = 0.0f;
		result = move;
		break;
	}
	case 3:
	{
		move = WVector(m_targetPos.x - pModel->m_pModel->pos.x,
			m_targetPos.y - pModel->m_pModel->pos.y,
			m_targetPos.z - pModel->m_pModel->pos.z);

		move.y = 0.0f;
		roomBox.max.z -= 0.5f;
		move.z = roomBox.max.z -
			fabs(pModel->m_pModel->aabb.max.z - pModel->m_pModel->aabb.min.z);

		dir = move;

		move.Normalize();

		move *= 17.0f;
		move = move * 0.04f;

		if (fabs(dir.x) < fabs(move.x))
		{
			move.x = dir.x;
		}
		if (fabs(dir.y) < fabs(move.y))
		{
			move.y = dir.y;
		}

		newPos = WVector(move.x + pModel->m_pModel->pos.x,
			move.y + pModel->m_pModel->pos.y, move.z + pModel->m_pModel->pos.z);

		trace_s trace;
		cModel* pm = *(cModel* volatile*)&pModel;
		if (!GetPVS().TraceAABB_BackGround(pm->m_pModel->pos, newPos,
				pm->m_pModel->aabb, &trace, pData))
		{
			break;
		}

		GetPVS().TraceOBB_Object(pm->m_pModel->pos, newPos, pm->m_pModel->aabb,
			&trace, pData, false);

		move = trace.endpos - pm->m_pModel->pos;
		move.y = 0.0f;
		move.z = 0.0f;
		result = move;
		break;
	}
	case 5:
		result = SlideWallPosition(pModel, pData);
		break;
	case 7:
	{
		trace_s trace;
		GetPVS().TraceOBB_Object(pModel->m_pModel->pos, pModel->m_pModel->pos,
			pModel->m_pModel->aabb, &trace, pData, true);

		move = trace.endpos - pModel->m_pModel->pos;

		if (WisZero(move.x) && WisZero(move.z))
		{
			result = WVector::ZERO;
			break;
		}

		result = pModel->m_pModel->pos;
		break;
	}
	}

	return result;
}

WVector CRealMyRoomData::SlideWallPosition(cModel* pModel,
	WPolySoup::sModel* pData)
{
	WVector result = WVector::ZERO;

	Waabb roomBox;
	GetPVS().GetAABB(roomBox);

	WMatrix rot;
	pModel->m_pModel->mat.GetRotMatrix(&rot);

	float angle = acos(rot.xx) * g_RADTODEG;

	WVector dir;
	dir = m_targetPos - pModel->m_pModel->pos;
	WVector move;
	move = m_targetPos.Normalize() - pModel->m_pModel->pos;

	if (angle == 90.0f)
	{
		move.x = (roomBox.max.x - 0.5f) -
			fabs(pModel->m_pModel->aabb.max.x - pModel->m_pModel->aabb.min.x);
	}
	else if (angle == 0.0f)
	{
		move.z = (roomBox.max.z - 0.5f) -
			fabs(pModel->m_pModel->aabb.max.z - pModel->m_pModel->aabb.min.z);
	}
	else if (angle == 270.0f || angle == -90.0f)
	{
		move.x = (roomBox.max.x - 0.5f) -
			fabs(pModel->m_pModel->aabb.max.x - pModel->m_pModel->aabb.min.x);
	}

	move.Normalize();

	move *= 17.0f;
	move = move * 0.04f;

	if (fabs(dir.x) < fabs(move.x))
	{
		move.x = dir.x;
	}
	if (fabs(dir.y) < fabs(move.y))
	{
		move.y = dir.y;
	}

	WVector newPos;
	newPos = move + pModel->m_pModel->pos;

	trace_s trace;
	if (GetPVS().TraceAABB_BackGround(pModel->m_pModel->pos, newPos,
			pModel->m_pModel->aabb, &trace, pData))
	{
		GetPVS().TraceOBB_Object(pModel->m_pModel->pos, newPos,
			pModel->m_pModel->aabb, &trace, pData, false);

		dir = trace.endpos - pModel->m_pModel->pos;
		if (angle == 90.0f)
		{
			dir.x = 0.0f;
		}
		else if (angle == 0.0f)
		{
			dir.z = 0.0f;
		}
		else if (angle == 270.0f || angle == -90.0f)
		{
			dir.x = 0.0f;
		}

		result = dir;
	}

	return result;
}

void CRealMyRoomData::SetModelPosition()
{
	WVector delta, newPos;
	delta.Reset();
	WPolySoup::sModel* pData = new WPolySoup::sModel;

	Waabb roomBox;
	GetPVS().GetAABB(roomBox);

	for (cModel* pModel = m_modelList.Start(); pModel;
		pModel = m_modelList.Next())
	{
		if (pModel && m_pSelModel &&
			strcmp(pModel->m_pModel->name, m_pSelModel->m_pModel->name) == 0)
		{
			memcpy(pData, pModel->m_pModel, sizeof(WPolySoup::sModel));

			IFF_STRUCT::sFurniture* pFurniture =
				ItemManager()->FindFurniture(pModel->m_pFurniture->typeId);

			WVector dir;
			dir = m_targetPos - pModel->m_pModel->pos;
			dir.z = (roomBox.max.z - 0.5f) -
				fabs(pModel->m_pModel->aabb.max.z -
					pModel->m_pModel->aabb.min.z);
			WVector to = dir;

			dir.Normalize();

			WVector move;
			move = dir * 17.0f * 0.04f;
			if (fabs(to.x) < fabs(move.x))
			{
				move.x = to.x;
			}
			if (fabs(to.y) < fabs(move.y))
			{
				move.y = to.y;
			}

			newPos = move + pModel->m_pModel->pos;
			WVector slide;
			slide.Reset();

			trace_s trace;
			if (GetPVS().TraceAABB_BackGround(pModel->m_pModel->pos, newPos,
					pModel->m_pModel->aabb, &trace, pModel->m_pModel))
			{
				GetPVS().TraceOBB_Object(pModel->m_pModel->pos, newPos,
					pModel->m_pModel->aabb, &trace, pData, false);

				const WVector& normal = trace.plane.normal;
				delta = trace.endpos - pModel->m_pModel->pos;

				if (pFurniture->IsMove == 5)
				{
					delta.z = 0.0f;
					if (delta.x == 0.0f && delta.y == 0.0f)
					{
						if ((normal * move) < 0.99f)
						{
							ClipVelocity(move, normal, slide, 1.001f);

							newPos = pModel->m_pModel->pos;

							if (slide.x != 0.0f)
							{
								newPos.x += slide.x;
							}
							else
							{
								newPos.y += slide.y;
							}
						}
					}

					pModel->m_pModel->pos.x += delta.x;
					pModel->m_pModel->pos.y += delta.y;
				}
				else
				{
					delta.y = 0.0f;
					if (delta.x == 0.0f && delta.z == 0.0f)
					{
						if ((normal * move) < 0.99f)
						{
							ClipVelocity(move, normal, slide, 1.001f);

							newPos = pModel->m_pModel->pos;

							if (slide.x != 0.0f)
							{
								newPos.x += slide.x;
							}
							else
							{
								newPos.z += slide.z;
							}
						}
					}

					pModel->m_pModel->pos.x += delta.x;
					pModel->m_pModel->pos.z += delta.z;
				}
			}

			pModel->m_pModel->pos += delta;

			pModel->m_pModel->aabb.min += delta;
			pModel->m_pModel->aabb.max += delta;
			pModel->m_pModel->obb.center += delta;

			pModel->SetPosition(pModel->m_pModel->pos);

			pModel->Apply_ChangeModelData();
			return;
		}
	}
}

unsigned long CRealMyRoomData::GetState()
{
	return m_state;
}

void CRealMyRoomData::SetState(unsigned long state)
{
	switch (state)
	{
	case 6:
		DeleteList();
		break;
	}

	m_state = state;
}

void CRealMyRoomData::ListFurnitureSelect(sFurniture_List* pList)
{
	Waabb ilPadding;

	for (cModel* pModel = m_modelList.Start(); pModel;
		pModel = m_modelList.Next())
	{
		if (pModel && pModel->m_pModel)
		{
			if (pModel->m_pFurniture->id == pList->id)
			{
				m_pSelModel = pModel;
			}
		}
	}
}

bool CRealMyRoomData::IsArragementFurniture(unsigned long id)
{
	bool bArranged = false;
	std::list<sFurniture_List>::iterator it;

	for (it = m_saveList.begin(); it != m_saveList.end(); ++it)
	{
		if (id == (*it).id)
		{
			if ((*it).bArrange)
			{
				bArranged = true;
			}
			break;
		}
	}

	return bArranged;
}

cModel* CRealMyRoomData::GetModelData(unsigned long id, unsigned long typeId)
{
	for (cModel* pModel = m_modelList.Start(); pModel;
		pModel = m_modelList.Next())
	{
		if (pModel)
		{
			if (pModel->m_pFurniture->typeId == typeId &&
				pModel->m_pFurniture->id == id)
			{
				return pModel;
			}
		}
	}

	return NULL;
}

bool CRealMyRoomData::IsModelData_SaveList(unsigned long id,
	unsigned long typeId)
{
	bool bFound = false;
	std::list<sFurniture_List>::iterator it;

	for (it = m_saveList.begin(); it != m_saveList.end(); ++it)
	{
		if (id == (*it).id && typeId == (*it).typeId && (*it).bArrange)
		{
			bFound = true;
			break;
		}
	}

	return bFound;
}

int CRealMyRoomData::IsFunritureValueChange(cModel& model)
{
	int result = -1;
	std::list<sFurniture_List>::iterator it;

	for (it = m_saveList.begin(); it != m_saveList.end(); ++it)
	{
		if (model.m_pFurniture->id == (*it).id)
		{
			result = 0;

			if (model.m_pModel)
			{
				if ((*it).x != model.m_pModel->pos.x)
				{
					result = 1;
				}

				if ((*it).y != model.m_pModel->pos.y)
				{
					result = 1;
				}

				if ((*it).z != model.m_pModel->pos.z)
				{
					result = 1;
				}

				if ((*it).r != model.m_fRotate)
				{
					result = 1;
				}

				if (model.m_pFurniture->bArrange != (*it).bArrange)
				{
					result = 1;
				}
			}
			else if (model.m_pFurniture->bArrange != (*it).bArrange)
			{
				result = 2;
			}

			break;
		}
	}

	return result;
}

cModel::cModel()
{
	m_nState = 1;
	m_pModel = NULL;
	m_pFurniture = NULL;

	m_fRotate = 0.0f;
	m_nWallIndex = -1;
}

cModel::~cModel()
{
}

void cModel::SetPosition(const WVector& pos)
{
	m_pModel->mat.pivot.x = pos.x;
	m_pModel->mat.pivot.y = pos.y;
	m_pModel->mat.pivot.z = pos.z;
}

void cModel::SetRotate(float angle)
{
	if (angle + m_fRotate < 0.0f)
	{
		m_fRotate = 360.0f;
	}

	if (angle + m_fRotate > 360.0f)
	{
		m_fRotate = 0.0f;
	}

	m_fRotate += angle;

	WMatrix mat;
	mat.Reset();

	float rad = angle * g_DEGTORAD;
	m_pModel->mat.Rotate(rad, 1);

	SetPosition(m_pModel->pos);

	mat.xa = m_pModel->obb.extend[0];
	mat.ya = m_pModel->obb.extend[1];
	mat.za = m_pModel->obb.extend[2];

	mat.Rotate(rad, 1);

	m_pModel->obb.center += mat.pivot;
	m_pModel->obb.extend[0] = mat.xa;
	m_pModel->obb.extend[1] = mat.ya;
	m_pModel->obb.extend[2] = mat.za;

	if (m_pFurniture)
	{
		m_pFurniture->r = angle;
	}
}

void cModel::Apply_ChangeModelData()
{
	m_pModel->pet->ApplyBones(NULL, m_pModel->mat);
	m_pModel->pet->Transform(NULL, 1.0f, 0);
	m_pModel->pet->UpdateBSphere();

	CSceneManager::Instance()->SetPuppet(m_pModel->name, m_pModel->pet);

	GOLFDOC()->m_pPolySoup->ChangeDataStructure(m_pModel);
}

void cModel::OBBToAABB()
{
	WPolySoup::sModel* pRoom = GOLFDOC()->m_pPolySoup->FindModel(NULL);

	Waabb roomBox = pRoom->aabb;
	WVector vMax = roomBox.min;
	vMax.y = 0.0f;
	WVector vMin = roomBox.max;
	vMin.y = 30.0f;

	WMatrix matrix;
	matrix.pivot = m_pModel->obb.center;
	matrix.xa = m_pModel->obb.extend[0];
	matrix.ya = m_pModel->obb.extend[1];
	matrix.za = m_pModel->obb.extend[2];
	WVector cube[8] = { WVector(-1.0f, 1.0f, -1.0f),
		WVector(-1.0f, -1.0f, -1.0f), WVector(1.0f, -1.0f, -1.0f),
		WVector(1.0f, 1.0f, -1.0f), WVector(-1.0f, 1.0f, 1.0f),
		WVector(-1.0f, -1.0f, 1.0f), WVector(1.0f, -1.0f, 1.0f),
		WVector(1.0f, 1.0f, 1.0f) };
	WVector corner[8];
	for (int i = 0; i < 8; ++i)
	{
		corner[i] = cube[i] * matrix;
	}
	WVector* pCorner = corner;
	for (int count = 8; count != 0; count--, pCorner++)
	{
		const float& cornerZ = pCorner->z;
		if (!(vMax.x > pCorner->x))
		{
			vMax.x = pCorner->x;
		}
		if (!(vMax.y > pCorner->y))
		{
			vMax.y = pCorner->y;
		}
		if (!(vMax.z > cornerZ))
		{
			vMax.z = cornerZ;
		}
		if (!(vMin.x < pCorner->x))
		{
			vMin.x = pCorner->x;
		}
		if (!(vMin.y < pCorner->y))
		{
			vMin.y = pCorner->y;
		}
		if (!(vMin.z < cornerZ))
		{
			vMin.z = cornerZ;
		}
	}
	m_pModel->aabb.max = vMax;
	m_pModel->aabb.min = vMin;
}
