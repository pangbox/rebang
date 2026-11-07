#include "minatl.h"
#include "wbone.h"
#include "wxpuppet.h"
#include "wpuppetutil.h"
#include "fxbox.h"
#include "quadtree.h"
#include "fx.h"
#include "fxspray.h"
#include "fxsequence.h"
#include "polysoup.h"
#include "golfdoc.h"
#include "clientsetting.h"
#include "contentsdoc.h"
#include "wind.h"
#include "mathconsts.h"
#include "slicestr.h"

extern "C" __declspec(dllimport) int __cdecl mkdir(const char*);

static bool IsSeaPet(const char* name)
{
	static const char* seaPet[] = {
		"un_sea.pet",
		"silv_dn.pet",
		"ice_unsea.pet",
		"panda_sea.pet",
	};

	for (unsigned int i = 0; i < sizeof(seaPet) / sizeof(seaPet[0]); i++)
	{
		if (stricmp(name, seaPet[i]) == 0)
			return true;
	}

	return false;
}

WPolySoup::WPolySoup(CGolfDoc* pDoc)
	: m_texture(NULL),
	  m_oldTextureNum(0),
	  m_oldTexture(NULL),
	  m_camera(NULL),
	  m_area(NULL),
	  m_point(NULL),
	  m_baseModel(NULL),
	  m_bUnused(false),
	  m_bBuildTnL(false),
	  m_smapMeshNum(0),
	  m_smapTexHandle(NULL),
	  m_smapMesh(NULL),
	  m_bSmapLoadFailed(false),
	  m_holecupTex(0)
{
	m_pDoc = pDoc;
	m_mapName[0] = 0;
	memset(&m_header, 0, sizeof(m_header));
	m_modelNum = 0;

	m_cullDistance = 0.0f;
	m_fadeNear = 800.0f;
	m_fadeFar = 960.0f;

	m_model.clear();
	m_rmrModelNum = 0;

	m_extraHoleStartArea.Reset();
}

WPolySoup::~WPolySoup()
{
	Clear();
	ClearOldTextures();

	if (g_resrcmng && m_holecupTex)
	{
		g_resrcmng->Release(m_holecupTex);
		m_holecupTex = 0;
	}
}

void WPolySoup::Clear()
{
	memset(m_mapName, 0, sizeof(m_mapName));

	if (m_model.size())
	{
		for (int i = 0; i < m_modelNum; i++)
		{
			sModel* model = m_model[i];

			if (model->pet)
			{
				g_resrcmng->Release(model->pet, false);
				model->pet = NULL;
			}

			if (model->quad)
			{
				delete[] model->quad;
				model->quad = NULL;
			}
			if (model->tri)
			{
				delete[] model->tri;
				model->tri = NULL;
			}

			delete model;
		}
	}

	if (m_baseModel)
	{
		if (m_baseModel->tri)
		{
			delete[] m_baseModel->tri;
			m_baseModel->tri = NULL;
		}
		if (m_baseModel->pet)
		{
			g_resrcmng->Release(m_baseModel->pet, true);
			m_baseModel->pet = NULL;
		}

		delete m_baseModel;
		m_baseModel = NULL;
	}

	BackupTextures();

	m_model.clear();

	if (m_camera)
	{
		delete[] m_camera;
		m_camera = NULL;
	}
	if (m_point)
	{
		delete[] m_point;
		m_point = NULL;
	}
	if (m_area)
	{
		delete[] m_area;
		m_area = NULL;
	}

	m_node.clear();
	m_texIndex.clear();

	m_lightList.clear();
	m_npcList.clear();
	m_soundBox.clear();

	memset(m_bgFx, 0, sizeof(m_bgFx));
	m_bgFxNum = 0;

	for (int i = 0; i < m_smapMeshNum; i++)
		g_resrcmng->Release(m_smapTexHandle[i]);

	m_smapMeshNum = 0;
	if (m_smapTexHandle)
	{
		delete[] m_smapTexHandle;
		m_smapTexHandle = NULL;
	}
	m_smapMesh = NULL;

	m_staticPetGrp.Clear();

	if (CSceneManager::Instance())
		CSceneManager::Instance()->ReleaseElement();

	if (CFx::Instance())
		CFx::Instance()->ClearAll(false);

	m_triList.clear();
}

void WPolySoup::BackupTextures()
{
	ClearOldTextures();

	m_oldTextureNum = m_header.textureNum;
	m_oldTexture = m_texture;
	m_texture = NULL;
}

void WPolySoup::ClearOldTextures()
{
	for (int i = 0; i < m_oldTextureNum; i++)
	{
		if (g_resrcmng && m_oldTexture[i].texHandle)
		{
			g_resrcmng->Release(m_oldTexture[i].texHandle);
			m_oldTexture[i].texHandle = 0;
		}
	}

	if (m_oldTexture)
	{
		delete[] m_oldTexture;
		m_oldTexture = NULL;
	}
	m_oldTextureNum = 0;
}

bool WPolySoup::LoadMapCheckData(unsigned char hole)
{
	cFile* file = GetResrcManager()->GetCFile(
		MakeStr("%s_%02d.gbin",
			Doc()->m_golfGame.pCourse ? Doc()->m_golfGame.pCourse->Data : NULL,
			hole),
		-1);

	if (file == NULL)
		return false;

	sHeader header;
	file->Read(&header, sizeof(header));

	if (header.version != 0x71)
	{
		MessageBox(NULL,
			MakeStr(
				"%d\xb9\xf8\xc8\xa6 \xb9\xe8\xb0\xe6 \xc6\xc4\xc0\xcf\xc0\xc7 \xb9\xf6\xc1\xaf\xc0\xcc \xb8\xc2\xc1\xf6 "
				"\xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
				hole),
			"\xb9\xe8\xb0\xe6\xc6\xc4\xc0\xcf \xb9\xf6\xc1\xaf", 0);
		return false;
	}

	file->Seek(-0x41, SEEK_END);

	file->Read(&GOLFDOC()->GetHoleData(hole).par, 1);
	Doc()->m_holePar[hole - 1] = GOLFDOC()->GetHoleData(hole).par;

	unsigned char index = GetHoleIndex(hole) - 1;

	unsigned char diff =
		Doc()->m_golfGame.pCourse ? Doc()->m_golfGame.pCourse->DiffFlag : 0;
	if (diff == 2)
	{
		file->Seek(8, SEEK_CUR);

		file->Read(&GOLFDOC()->GetHoleData(hole).tee.x, 4);
		file->Read(&GOLFDOC()->GetHoleData(hole).tee.z, 4);

		file->Seek(Doc()->m_courseMap[index] * 8 + 24, SEEK_CUR);
		file->Read(&GOLFDOC()->GetHoleData(hole).pin.x, 4);
		file->Read(&GOLFDOC()->GetHoleData(hole).pin.z, 4);
		file->Seek((2 - Doc()->m_courseMap[index]) * 8, SEEK_CUR);
	}
	else
	{
		file->Read(&GOLFDOC()->GetHoleData(hole).tee.x, 4);
		file->Read(&GOLFDOC()->GetHoleData(hole).tee.z, 4);

		file->Seek(8, SEEK_CUR);

		file->Seek(Doc()->m_courseMap[index] * 8, SEEK_CUR);
		file->Read(&GOLFDOC()->GetHoleData(hole).pin.x, 4);
		file->Read(&GOLFDOC()->GetHoleData(hole).pin.z, 4);
		file->Seek((5 - Doc()->m_courseMap[index]) * 8, SEEK_CUR);
	}

	file->Seek(0, SEEK_SET);
	GOLFDOC()->GetHoleData(hole).checkSum = 0;
	unsigned char c;
	int len = file->GetByte();
	for (int i = 0; i < len; i++)
	{
		file->Read(&c, 1);
		GOLFDOC()->GetHoleData(hole).checkSum += c;
	}

	if (GOLFDOC()->m_bNoStat)
	{
		file->Seek(sizeof(sHeader), SEEK_SET);

		int size = header.cameraNum * 64;
		file->Seek(size, SEEK_CUR);

		for (int i = 0; i < header.pointNum; i++)
		{
			unsigned char type;
			file->Read(&type, 1);

			int skip = 44;
			if (type)
				skip = 108;

			file->Seek(skip, SEEK_CUR);
		}

		sArea area;
		WVector v;
		int i;
		for (i = 0; i < header.areaNum; i++)
		{
			file->Read(&area.type, 1);
			file->Read(area.name, sizeof(area.name));
			file->Read(&v, sizeof(WVector));
			area.aabb.min = v;
			file->Read(&v, sizeof(WVector));
			area.aabb.max = v;

			if (strstr(area.name, "*extra"))
			{
				if (GOLFDOC()->m_currentHole == hole)
					m_extraHoleStartArea = area.aabb;
				break;
			}
		}

		if (i == header.areaNum)
		{
			if (!NET()->IsConnected(WNetworkSystem::NET_GAME))
				MessageBox(NULL,
					MakeStr(
						"%d\xc8\xa6\xbf\xa1 \xbe\xee\xc7\xc1\xb7\xce\xc4\xa1 \xbf\xb5\xbf\xaa\xc0\xcc \xbc\xb3\xc1\xa4\xb5\xc7"
						"\xbe\xee \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9",
						hole),
					"Warning", 0);
		}
		else
		{
			float r = (float)(Doc()->m_holeRandom[0] & 0x7f);
			GOLFDOC()->GetHoleData(hole).tee.x =
				(area.aabb.max.x - area.aabb.min.x) * r / 128.0f +
				area.aabb.min.x;
			r = (float)(Doc()->m_holeRandom[1] & 0x7f);
			GOLFDOC()->GetHoleData(hole).tee.z =
				(area.aabb.max.z - area.aabb.min.z) * r / 128.0f +
				area.aabb.min.z;
		}
	}

	CloseCFile(file);
	return true;
}

int WPolySoup::CompareModel(const void* a, const void* b)
{
	sModel* m1 = *(sModel**)a;
	sModel* m2 = *(sModel**)b;

	int ret = strcmpi(m1->name, m2->name);

	if (ret)
		return ret;

	return m1->flag < m2->flag ? -1 : m1->type > m2->type;
}

static bool IsSilviaCannonArea(const WPolySoup::sArea* area)
{
	if (area->type != 6)
		return false;

	WSliceStr slice(area->name);
	char name[64];
	float value[3];

	slice.Get("*fx", "%s %f %f %f", name, &value[0], &value[1], &value[2]);

	if (strcmpi(name, K2L_Compatibility("silviacannon.seq")) == 0)
		return true;

	return false;
}

// HACK!
__declspec(dllimport) __forceinline WVector ModelBoundsCenter(
	const Waabb& bounds)
{
	WVector sum(bounds.min.x + bounds.max.x, bounds.min.y + bounds.max.y,
		bounds.min.z + bounds.max.z);
	return sum * 0.5f;
}

bool WPolySoup::Load(const char* filename, bool bOption)
{
	int caps;
	int i;

	g_resrcmng->VideoReference()->Command(WX_VDEV_GET_CAPS, 1, (int)&caps);

	if (caps)
		m_bBuildTnL = true;

	Clear();

	strncpy(m_mapName, filename, 63);
	m_mapName[63] = 0;

	CSharedDoc* pDoc = Doc();
	IFF_STRUCT::sCourse* pCourse = pDoc->m_golfGame.pCourse;
	unsigned char detail = !pCourse ? 0 : pCourse->DiffFlag;

	cFile* file = GetResrcManager()->GetCFile(filename, -1);
	if (file == NULL)
	{
		MessageBox(NULL,
			MakeStr("\xb9\xe8\xb0\xe6 \xc6\xc4\xc0\xcf(%s)\xc0\xbb "
					"\xc0\xd0\xbe\xee\xbf\xc0\xc1\xf6 "
					"\xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				filename),
			"\xc6\xc4\xc0\xcf \xc0\xd0\xb1\xe2 \xbd\xc7\xc6\xd0", 0);
		return false;
	}

	file->Read(&m_header, sizeof(sHeader));

	if (m_header.version != 0x71)
	{
		MessageBox(NULL,
			"\xb9\xe8\xb0\xe6 \xc6\xc4\xc0\xcf\xc0\xc7 \xb9\xf6\xc1\xaf\xc0\xcc "
			"\xb8\xc2\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
			"\xb9\xe8\xb0\xe6\xc6\xc4\xc0\xcf \xb9\xf6\xc1\xaf", 0);
		return false;
	}

	switch (detail)
	{
	case 0:
		m_modelNum = m_header.modelNum;
		break;
	case 1:
		m_modelNum = m_header.modelNum1 + m_header.modelNum;
		break;
	case 2:
		m_modelNum = m_header.modelNum2 + m_header.modelNum;
		break;
	default:
		m_modelNum =
			m_header.modelNum2 + m_header.modelNum1 + m_header.modelNum;
		break;
	}

	m_texture = new sTexture[m_header.textureNum];

	sModel** models = new sModel*[m_modelNum];
	for (i = 0; i < m_modelNum; i++)
		models[i] = new sModel;

	m_point = new sPoint[m_header.pointNum];
	m_area = new sArea[m_header.areaNum];
	m_camera = new sCamera[m_header.cameraNum];
	m_baseModel = new sModel;

	for (i = 0; i < m_header.cameraNum; i++)
	{
		sCamera* camera = &m_camera[i];
		float eye[3];
		float at[3];

		file->Read(camera->name, 32);
		file->Read(eye, 12);
		file->Read(at, 12);
		camera->mat = MakeCameraMatrix(WVector(eye), WVector(at));

		file->Read(&camera->fov, 4);
		file->Read(&camera->reserved, 4);
	}

	g_lightset.type = 2;
	g_lightset.diffuse = 0xffffff;
	g_lightset.ambient = 0x808080;
	g_lightset.ambient2 = 0xffffff;

	for (i = 0; i < m_header.pointNum; i++)
	{
		sPoint* point = &m_point[i];
		WVector pos;

		file->Read(&point->type, 1);
		file->Read(point->name, 32);
		file->Read(&pos, 12);
		point->pos.x = pos.x;
		point->pos.y = pos.y;
		point->pos.z = pos.z;

		if (point->type)
		{
			char option[64];
			file->Read(option, 64);
			point->option = option;
		}

		switch (point->type)
		{
		case 1:
			sscanf(point->option.c_str(), "%x %x %x", &g_lightset.diffuse,
				&g_lightset.ambient, &g_lightset.ambient2);
			g_lightset.nearone = point->pos;
			break;
		case 3:
			GOLFDOC()->SetDay(false);
			break;
		}
	}

	g_lightset.diffuse = ModifyColor(g_lightset.diffuse, GOLFDOC()->m_bright);
	g_lightset.ambient = ModifyColor(g_lightset.ambient, GOLFDOC()->m_bright);
	g_lightset.ambient2 = ModifyColor(g_lightset.ambient2, GOLFDOC()->m_bright);

	for (i = 0; i < m_header.areaNum; i++)
	{
		sArea* area = &m_area[i];
		WVector pos;

		file->Read(&area->type, 1);
		file->Read(area->name, 64);
		file->Read(&pos, 12);
		area->aabb.min.x = pos.x;
		area->aabb.min.y = pos.y;
		area->aabb.min.z = pos.z;
		file->Read(&pos, 12);
		area->aabb.max.x = pos.x;
		area->aabb.max.y = pos.y;
		area->aabb.max.z = pos.z;

		if (area->type == 5)
		{
			sSoundBox sound;
			sound.interval = -1.0f;
			sound.distance = 60.0f;
			sound.time = 0.0f;
			sscanf(area->name, "%s %f %f", sound.name, &sound.distance,
				&sound.interval);
			sound.obb = area->aabb;
			m_soundBox.push_back(sound);
		}

		if (strstr(area->name, "*extra"))
			m_extraHoleStartArea = area->aabb;
	}

	IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(
		((GetCurMap() > 127 && GetCurMap() != 253) ? GetCurMap() - 128
												   : GetCurMap()) |
		0x28000000);

	if (GOLFDOC()->m_currentHole != 19)
	{
		if (course && course->TexProp[0])
			m_pDoc->LoadXml(course->TexProp, GOLFDOC()->m_currentHole - 1,
				false);
		else
			m_pDoc->LoadXml(MakeStr("%s_property.xml",
								Doc()->m_golfGame.pCourse
									? Doc()->m_golfGame.pCourse->Data
									: NULL),
				GOLFDOC()->m_currentHole - 1, false);
	}

	for (int i = 0; i < m_header.textureNum; i++)
	{
		file->Read(m_texture[i].name, 32);
		m_texture[i].texHandle =
			GetResrcManager()->LoadTexture(m_texture[i].name, 0, 0, NULL);
		m_texture[i].propIndex = GetPropertyIndex(m_texture[i].name);
		m_texIndex[m_texture[i].texHandle] = i;
	}

	struct
	{
		char name[16];
		int num;
		int reserved;
	} nodeInfo;

	m_bCameraArea = false;
	for (i = 0; i < m_header.nodeNum; i++)
	{
		file->Read(&nodeInfo, sizeof(nodeInfo));

		CNodeContainer node(std::string(nodeInfo.name));

		if (strstr(nodeInfo.name, "outline"))
			node.SetType(CNodeContainer::TYPE_OUTLINE);
		else if (strstr(nodeInfo.name, "ib"))
			node.SetType(CNodeContainer::TYPE_IB);
		else if (strstr(nodeInfo.name, "ob"))
			node.SetType(CNodeContainer::TYPE_OB);
		else if (strstr(nodeInfo.name, "auto"))
			node.SetType(CNodeContainer::TYPE_AUTO);
		else if (strstr(nodeInfo.name, "beach"))
			node.SetType(CNodeContainer::TYPE_BEACH);
		else if (strstr(nodeInfo.name, "riverex"))
			node.SetType(CNodeContainer::TYPE_RIVEREX);
		else if (strstr(nodeInfo.name, "waterfall"))
			node.SetType(CNodeContainer::TYPE_WATERFALL);
		else if (strstr(nodeInfo.name, "river"))
			node.SetType(CNodeContainer::TYPE_RIVER);
		else if (strstr(nodeInfo.name, "lava"))
			node.SetType(CNodeContainer::TYPE_LAVA);
		else if (strstr(nodeInfo.name, "sea"))
			node.SetType(CNodeContainer::TYPE_SEA);
		else if (strstr(nodeInfo.name, "wave"))
			node.SetType(CNodeContainer::TYPE_WAVE);
		else if (strstr(nodeInfo.name, "npc"))
			node.SetType(CNodeContainer::TYPE_NPC);
		else if (strstr(nodeInfo.name, "camera"))
		{
			m_bCameraArea = true;
			node.SetType(CNodeContainer::TYPE_CAMERA);
		}

		for (int k = 0; k < nodeInfo.num; k++)
		{
			float buf[3];
			file->Read(buf, 12);
			WVector pos(buf);
			node.Add(pos);
		}

		m_node.push_back(node);
	}

	if (g_lightset.type == 2)
	{
		g_lightset.nearone *= -1.0f;
		g_lightset.nearone.Normalize();
	}

	sBgModel bgModel;
	file->Read(&bgModel, sizeof(sBgModel));

	m_baseModel->type = bgModel.flag & 7;
	m_baseModel->flag = bgModel.flag & 0xf8;
	m_baseModel->aabb = bgModel.localAabb;
	m_baseModel->mat = bgModel.mat;
	m_baseModel->pos = ModelBoundsCenter(m_baseModel->aabb);
	m_baseModel->center =
		(m_baseModel->aabb.max + m_baseModel->aabb.min) * 0.5f;
	m_baseModel->radius =
		(m_baseModel->aabb.max - m_baseModel->aabb.min).Magnitude() * 0.5f;
	m_baseModel->scale = bgModel.mat.xa.Magnitude();
	strncpy(m_baseModel->name, bgModel.name, 31);

	m_baseModel->pet =
		g_resrcmng->GetPuppet(m_baseModel->name, false, false, false);
	m_baseModel->pet->m_lightmode = 0x1000000;
	m_baseModel->pet->ApplyBones(NULL, m_baseModel->mat);
	m_baseModel->pet->UpdateBound(true, false);
	m_baseModel->pet->UpdateLightSource(&g_lightset, false, 0);

	m_baseModel->tri = new sTriangle[m_baseModel->pet->GetFaceNum()];

	WBone* root = m_baseModel->pet->GetRootBone();
	bool bBaseMeshTest = COption::Instance()->dIsBaseMeshTestEnabled();

	unsigned long color;
	i = 0;
	for (w_mesh* mesh = root->GetMesh(); mesh; mesh = mesh->next)
	{
		for (int j = 0; j < mesh->indexNum; j += 3)
		{
			for (int k = 0; k < 3; k++)
			{
				int index = mesh->indexList[j + k];

				file->Read(&color, 4);
				mesh->vtxColorList[index] =
					ModifyColor(color, GOLFDOC()->m_bright);
				if (color & 0xff000000)
					mesh->vtxColorList[index] |= 0xff000000;

				m_baseModel->tri[i].v[k] =
					mesh->vecList[index] * root->GetMatrix();
			}

			WVector* v = m_baseModel->tri[i].v;
			m_baseModel->tri[i].plane.normal =
				WCrossProduct(v[2] - v[1], v[0] - v[1]);
			m_baseModel->tri[i].plane.normal.Normalize();
			m_baseModel->tri[i].plane.d =
				(m_baseModel->tri[i].plane.normal * -1.0f) * v[0];
			m_baseModel->tri[i].bDoubleSide =
				((unsigned long)mesh->xiDrawFlag2 >> 10) & 1;
			m_baseModel->tri[i].propIndex =
				m_texture[m_texIndex[mesh->texHandle]].propIndex;
			m_baseModel->tri[i].texHandle =
				m_texture[m_texIndex[mesh->texHandle]].texHandle;

			CGimmickContainer* gimmick = NULL;
			CContentsDoc::Instance()->GetContainer((localContentType_t)0xa3,
				gimmick);

			w_texlist* tex =
				g_resrcmng->FindTexture(m_baseModel->tri[i].texHandle);
			if (gimmick->NeedToMakeTriPtArray(tex->texname, NULL))
			{
				std::map<int, std::vector<sTriangle*> >::iterator it =
					m_triList.find(mesh->texHandle);
				if (it == m_triList.end())
				{
					std::vector<sTriangle*> list;
					list.push_back(&m_baseModel->tri[i]);
					m_triList[mesh->texHandle] = list;
				}
				else
				{
					it->second.push_back(&m_baseModel->tri[i]);
				}
			}

			if (bBaseMeshTest)
			{
				if (m_baseModel->tri[i].plane.b < 0.0f)
				{
					*(unsigned short*)&mesh->vtxColorList[mesh->indexList[j]] =
						0;
					*(unsigned short*)&mesh
						 ->vtxColorList[mesh->indexList[j + 1]] = 0;
					*(unsigned short*)&mesh
						 ->vtxColorList[mesh->indexList[j + 2]] = 0;
				}
				else if (Wabs(m_baseModel->tri[i].plane.b) < g_EPSILON)
				{
					mesh->vtxColorList[mesh->indexList[j]] &= 0xff0000ff;
					mesh->vtxColorList[mesh->indexList[j + 1]] &= 0xff0000ff;
					mesh->vtxColorList[mesh->indexList[j + 2]] &= 0xff0000ff;
				}
			}

			i++;
		}
	}

	w_bound_box* center = m_baseModel->pet->m_bbList.Find("center");
	if (center && center->info->option)
	{
		WSliceStr slice(center->info->option);
		slice.Get("*wind", "%f", &m_baseModel->windFactor);
	}

	if (m_baseModel->pet->m_bbList.Find("npc"))
		m_npcList.push_back(0xffff);

	CSceneManager::Instance()->RegisterElement(OBJ_BASE, m_baseModel->pet, NULL,
		m_baseModel->name, 3, NULL);

	int animatedNum = 0;
	int n = 0;
	int total = m_header.modelNum2 + m_header.modelNum1 + m_header.modelNum;
	for (i = 0; i < total; i++)
	{
		file->Read(&bgModel, sizeof(sBgModel));

		if (bgModel.detail == 1)
		{
			if (detail == 2)
				continue;
		}
		else if (bgModel.detail == 2)
		{
			if (detail == 1)
				continue;
		}

		sModel* model = models[n++];
		model->type = bgModel.flag & 7;
		model->flag = bgModel.flag & 0xf8;
		model->aabb = bgModel.localAabb;
		model->bbox = bgModel.sceneAabb;
		model->mat = bgModel.mat;
		model->pos = (model->aabb.max + model->aabb.min) * 0.5f;
		model->center = (model->aabb.max + model->aabb.min) * 0.5f;
		model->radius = (model->aabb.max - model->aabb.min).Magnitude() * 0.5f;
		model->scale = bgModel.mat.xa.Magnitude();
		strncpy(model->name, bgModel.name, 31);
		strncpy(model->option, bgModel.option, 31);

		model->pet = g_resrcmng->GetPuppet(model->name, IsSeaPet(model->name),
			model->type != 0, false);

		if (model->type)
			animatedNum++;

		LogOut(0, "%s : radius %f, scale %f\n", model->name,
			(model->aabb.max - model->aabb.min).Magnitude() * 0.5f,
			bgModel.mat.xa.Magnitude());

		if (model->pet == NULL)
		{
			if (m_pDoc->m_modelNameList.Find(model->name) == NULL)
			{
				char* name = new char[strlen(model->name) + 1];
				strcpy(name, model->name);
				m_pDoc->m_modelNameList.AddItem(name, name, false);
			}

			continue;
		}

		model->pet->ApplyBones(NULL, model->mat);
		model->pet->UpdateBound(true, false);
		model->pet->UpdateLightSource(&g_lightset, false, 0);
		ProcessPostLoadPet(model);

		unsigned long flags = 0;
		if (model->type)
			flags |= 4;
		if ((model->flag & 0x10) ||
			model->pet->GetBoundSphere().radius > 128.0f)
			flags |= 2;
		if (model->flag & 8)
			flags |= 1;

		CSceneManager::Instance()->RegisterElement(model->type ? OBJ_MODEL
															   : OBJ_STATIC,
			model->pet, NULL, model->name, flags, &model->bbox);
	}

	if (m_modelNum > 1)
		qsort(models, m_modelNum, sizeof(sModel*), CompareModel);

	for (int i = 0; i < m_modelNum; i++)
		m_model.push_back(models[i]);

	delete[] models;

	GetPVS().Reserve((short)animatedNum);

	m_bgFxNum = 0;
	for (i = 0; i < m_header.areaNum; i++)
	{
		sArea* area = &m_area[i];

		if (IsSilviaCannonArea(area))
			continue;

		if (area->type != 6)
			continue;

		Attacher attacher;
		char fxName[64];
		WVector rotation;

		WSliceStr slice(area->name);
		slice.Get("*fx", "%s %f %f %f", fxName, &rotation.x, &rotation.y,
			&rotation.z);

		if (bOption)
		{
			if (GetCurMap() == 4 || GetCurMap() == 5)
			{
				if (stricmp(fxName, "CL_Default.spr") == 0)
					continue;
			}
		}

		m_bgFx[m_bgFxNum].area = area;
		m_bgFx[m_bgFxNum].mat.Reset();
		m_bgFx[m_bgFxNum].mat.Rotate(rotation);
		m_bgFx[m_bgFxNum].mat.pivot = (area->aabb.min + area->aabb.max) * 0.5f;
		m_bgFx[m_bgFxNum].offset.Reset();
		attacher.mat[0] = &m_bgFx[m_bgFxNum].mat;
		attacher.pos[0] = &m_bgFx[m_bgFxNum].offset;

		void* fx = CFx::Instance()->Attach(attacher, fxName, false, NULL);
		if (fx == NULL)
			continue;

		char* ext = strrchr(fxName, '.');
		if (ext == NULL || strlen(ext) < 4)
			continue;

		if (stricmp(ext + 1, "spr") == 0)
		{
			((CFxSpray*)fx)->SetLight(g_lightset);
			m_bgFx[m_bgFxNum].spray = (CFxSpray*)fx;
		}
		else if (stricmp(ext + 1, "seq") == 0)
		{
			((CFxSequence*)fx)->SetLight(g_lightset);
			m_bgFx[m_bgFxNum].sequence = (CFxSequence*)fx;
		}
		else
		{
			continue;
		}

		m_bgFxNum++;

		if (m_bgFxNum >= 256)
			break;
	}

	m_bSmapLoadFailed = false;
	if (COption::Instance()->vIsShadowmapEnabled())
		m_bSmapLoadFailed = !LoadShadowmapInfo();

	CFx::Instance()->SetPvsFxBoxGroundHeight(m_baseModel->aabb.max.y,
		m_baseModel->aabb.min.y);

	CloseCFile(file);
	return true;
}

void WPolySoup::ProcessPostLoadPet(sModel* model)
{
	for (w_bound_box* box = model->pet->m_bbList.Start(); box;
		box = model->pet->m_bbList.Next())
	{
		const char* option = box->info->option;

		if (stricmp(box->info->name, "night_only") == 0)
		{
			if (GOLFDOC()->m_bDay)
			{
				if (box->bone && box->bone->GetChild())
					box->bone->GetChild()->SetInvisible(true, true);
			}
		}
		else if (stricmp(box->info->name, "day_only") == 0)
		{
			if (!GOLFDOC()->m_bDay)
			{
				if (box->bone && box->bone->GetChild())
					box->bone->GetChild()->SetInvisible(true, true);
			}
		}
		else if (stricmp(box->info->name, "center") == 0)
		{
			WSliceStr slice(option);
			slice.Get("*wind", "%f", &model->windFactor);
			slice.Get("*nocol", "%d", &model->noCollision);

			if (option)
			{
				if (strstr(option, "*nofog"))
					model->pet->DisableFog();
				if (strstr(option, "*blend"))
					model->pet->EnableAlphaBlend();
			}
		}
		else if (stricmp(box->info->name, "npc") == 0)
		{
		}
		else if (strstr(box->info->name, "sound"))
		{
			sSoundBox sound;
			sound.interval = -1.0f;
			sound.distance = 60.0f;
			sound.time = 0.0f;
			sscanf(option, "%s %f %f", sound.name, &sound.distance,
				&sound.interval);
			sound.obb = box->bone->GetMatrix();
			sound.obb *= (box->info->aabb.max - box->info->aabb.min) * 0.5f;
			sound.obb.center = box->info->spherePivot * box->bone->GetMatrix();

			m_soundBox.push_back(sound);
		}
		else if (strstr(_strlwr(box->info->name), "abv"))
		{
			box->bone->HideBone(false);
		}

		if (option == NULL)
			continue;

		const char* fx = strstr(option, "*fx");
		if (fx == NULL)
			continue;

		Attacher attacher;
		char cmd[8], file[32] = "", type[32] = "";
		sscanf(fx, "%s %s %s", cmd, file, type);

		if (box->bone == NULL || box->bone->IsInvisible())
			continue;

		const char* ext = strrchr(file, '.');
		if (ext == NULL || strlen(ext) < 4)
			continue;

		const char* extName = ext + 1;
		if (stricmp(extName, "spr") != 0 && stricmp(extName, "seq") != 0)
			continue;

		if (strcmpi(type, "tree") == 0)
		{
			ApplyFx(model, box->bone->GetChild(), file);
		}
		else
		{
			Attacher attacher;
			attacher.mat[0] = &box->bone->GetMatrix();
			attacher.pos[0] = &box->info->spherePivot;

			defines_t defines;
			strcpy(defines.name[0], "pet_scale");
			defines.value[0] = model->scale;
			defines.num = 1;

			if (stricmp(extName, "spr") == 0)
				CFx::Instance()->Attach(attacher, file, false, &defines);
			else
				CFx::Instance()->Attach(attacher, file, false, &defines);
		}
	}
}

bool WPolySoup::IsResisteredBgSequence(const std::string& name)
{
	for (int i = 0; i < m_bgFxNum; i++)
	{
		if (m_bgFx[i].sequence && m_bgFx[i].sequence->m_filename == name)
			return true;
	}

	return false;
}

void WPolySoup::ApplyFx(sModel* model, WBone* bone, const char* fx)
{
	w_bound_box* box = model->pet->FindBoundBox(bone->GetBoneName());

	if (box)
	{
		Attacher attacher;
		attacher.mat[0] = &box->bone->GetMatrix();
		attacher.pos[0] = &box->info->spherePivot;

		defines_t defines;
		strcpy(defines.name[0], "pet_scale");
		defines.value[0] = model->scale;
		defines.num = 1;

		if (stricmp(fx, ".spr") == 0)
			CFx::Instance()->Attach(attacher, fx, false, &defines);
		else
			CFx::Instance()->Attach(attacher, fx, false, &defines);
	}

	if (bone->GetNext())
		ApplyFx(model, bone->GetNext(), fx);

	if (bone->GetChild())
		ApplyFx(model, bone->GetChild(), fx);
}

void WPolySoup::MakeDataStructure()
{
	GetPVS().SetPolySoup(this);

	GetPVS().MakeBaseStructure(m_baseModel, false);

	for (short i = 0; i < m_modelNum; i++)
		GetPVS().MakeObjStructure(i, -1, false);
	m_rmrModelNum = m_modelNum;
}

void WPolySoup::CreateDataModel(sModel* model)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	model->id = m_rmrModelNum;

	m_model.push_back(model);

	GetPVS().MakeObjStructure(model->id, m_modelNum, false);

	m_rmrModelNum++;

	m_modelNum++;
}

void WPolySoup::ChangeDataStructure(sModel* model)
{
	int index = 0;

	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	for (std::vector<sModel*>::iterator it = m_model.begin();
		it != m_model.end(); ++it, ++index)
	{
		if (stricmp((*it)->name, model->name) == 0)
		{
			*it = model;

			GetPVS().MakeObjStructure(model->id, index, true);

			for (int i = 0; i < m_modelNum; i++)
				GetPVS().MakeObjStructure(m_model[i]->id, i, true);

			return;
		}
	}

	CreateDataModel(model);
}

bool WPolySoup::DeleteDataStructure(sModel* model)
{
	bool bResult = false;
	int index = 0;
	int num = m_modelNum;

	if (IsLocalContent(S4_REAL_MYROOM))
	{
		std::vector<sModel*>::iterator it;
		for (it = m_model.begin(); it != m_model.end(); ++it, ++index)
		{
			if (stricmp((*it)->name, model->name) == 0)
			{
				if (!(it != m_model.end()))
					break;

				bResult = true;
				(*it)->pet = NULL;

				GetPVS().MakeBaseStructure(m_baseModel, true);

				GetPVS().MakeObjStructure((*it)->id, index, true);

				for (int i = 0; i < num; i++)
					GetPVS().MakeObjStructure(m_model[i]->id, i, true);

				for (it = m_model.begin(); it != m_model.end(); ++it)
				{
					if (stricmp((*it)->name, model->name) == 0)
					{
						if (!(it != m_model.end()))
							break;

						if ((*it)->tri)
						{
							delete[] (*it)->tri;
							(*it)->tri = NULL;
						}
						if ((*it)->quad)
						{
							delete[] (*it)->quad;
							(*it)->quad = NULL;
						}

						m_model.erase(it);

						m_modelNum--;
						break;
					}
				}

				break;
			}
		}
	}

	return bResult;
}

const sProperty& WPolySoup::GetProperty(int index)
{
	return m_pDoc->m_propertyInfo.GetProperty(index);
}

int WPolySoup::GetPropertyIndex(const char* name)
{
	return m_pDoc->m_propertyInfo.GetPropertyIndex(name);
}

void WPolySoup::EndLoad(bool bPlaceHoleCup)
{
	ClearOldTextures();

	if (bPlaceHoleCup)
		PlaceHoleCup();

	xBuildTnLBuffers();
}

void WPolySoup::ChangeFloorTexture(const char* from, const char* to)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	if (from == NULL || to == NULL)
		return;

	int fromHandle = g_resrcmng->FindTexture(from)->texhandle;

	int toHandle = g_resrcmng->LoadTexture(to, 0, 0, 0);

	if (m_baseModel->pet->GetRootBone()->GetMesh())
		m_baseModel->pet->ChangeTextureByHandle(toHandle, fromHandle,
			m_baseModel->pet);
}

void WPolySoup::ChangeObjectTexture(const char* from, const char* to, int index)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	if (from == NULL || to == NULL)
		return;

	if (index == -1)
	{
		int fromHandle = g_resrcmng->FindTexture(from)->texhandle;

		int toHandle = g_resrcmng->LoadTexture(to, 0, 0, 0);

		w_mesh* mesh = m_model[0]->pet->GetRootBone()->GetMesh();
		if (mesh)
		{
			mesh = mesh->next;
			while (mesh)
			{
				if (mesh)
					break;
				mesh = mesh->next;
			}

			if (mesh)
				m_model[0]->pet->ChangeTextureByHandle(toHandle, fromHandle,
					m_model[0]->pet);
		}
	}
	else
	{
		for (int i = 0; i < m_modelNum; i++)
		{
			if (m_model[i]->id == index)
			{
				index = i;
				break;
			}
		}

		int fromHandle = g_resrcmng->FindTexture(from)->texhandle;

		int toHandle = g_resrcmng->LoadTexture(to, 0, 0, 0);

		w_mesh* mesh = m_model[index]->pet->GetRootBone()->GetMesh();
		if (mesh)
		{
			mesh = mesh->next;
			while (mesh)
			{
				if (mesh)
				{
					m_model[index]->pet->ChangeTextureByHandle(toHandle,
						fromHandle, m_model[index]->pet);
					return;
				}
				mesh = mesh->next;
			}
			m_model[index]->pet->ChangeTextureByHandle(toHandle, fromHandle,
				m_model[index]->pet);
			mesh = NULL;
		}
	}
}

void WPolySoup::xBuildTnLBuffers()
{
	if (m_bBuildTnL)
	{
		m_baseModel->pet->xUpdateTnLBuffers(true);
		m_staticPetGrp.Build(m_model, m_modelNum);
	}
}

unsigned char WPolySoup::OverlapCamera(int index)
{
	if (m_model[index]->flag & 8)
		return 0xff;
	Waabb aabb = m_model[index]->aabb;

	float radius =
		Min(WVectorLen(aabb.max - aabb.min) * 0.5f, 300.0f * g_CM_TO_WU);
	float dist = 0.0f;

	for (int i = 0; i < 3; i++)
	{
		float d;
		if (g_camera.pivot.p[i] < aabb.min.p[i])
			d = g_camera.pivot.p[i] - aabb.min.p[i];
		else if (g_camera.pivot.p[i] > aabb.max.p[i])
			d = g_camera.pivot.p[i] - aabb.max.p[i];
		else
			continue;
		dist += d * d;
	}

	return (unsigned char)(Between(0.25f, sqrtf(dist) / radius, 1.0f) * 255.0f);
}

bool WPolySoup::IsPointInPoly(const std::vector<WVector>& poly,
	const WVector& pos)
{
	int num = poly.size();
	if (num < 3)
		return false;

	int sum = 0;
	for (int i = 0; i < num; i++)
	{
		WVector a = poly[i];
		WVector b = poly[(i + 1) % num];
		int qa = a.x > pos.x ? (a.z > pos.z ? 1 : 4) : (a.z > pos.z ? 2 : 3);
		int qb = b.x > pos.x ? (b.z > pos.z ? 1 : 4) : (b.z > pos.z ? 2 : 3);
		int d = qb - qa;

		switch (d)
		{
		case 2:
		case -2:
			if (b.x - (a.x - b.x) * (b.z - pos.z) / (a.z - b.z) > pos.x)
				d = -d;
			break;
		case 3:
			d = -1;
			break;
		case -3:
			d = 1;
			break;
		}

		sum += d;
	}

	if (Abs(sum) == 4)
		return true;
	return false;
}

bool WPolySoup::IsOutOfBound(const WVector& pos)
{
	std::vector<CNodeContainer>::const_iterator it;
	for (it = m_node.begin(); it != m_node.end(); it++)
	{
		CNodeContainer::eType type = (*it).GetType();

		if (type == CNodeContainer::TYPE_OB ||
			type == CNodeContainer::TYPE_OUTLINE)
		{
			if (!IsPointInPoly(it->GetList(), pos))
				return true;
		}
		else if (type == CNodeContainer::TYPE_IB)
		{
			if (IsPointInPoly(it->GetList(), pos))
				return true;
		}
	}

	if (GetPVS().IsOBArea(pos))
		return true;

	return false;
}

WVector WPolySoup::GetCameraPos(const WVector& from, const WVector& to)
{
	WVector pos = from;
	std::vector<CNodeContainer>::iterator it;

	for (it = m_node.begin(); it != m_node.end(); it++)
	{
		CNodeContainer::eType type = it->GetType();
		if ((HaveCameraArea() && type == CNodeContainer::TYPE_CAMERA) ||
			(!HaveCameraArea() && type == CNodeContainer::TYPE_OUTLINE))
		{
			if (!IsPointInPoly(it->GetList(), from))
				pos = PosInWorld(it->GetList(), from, to);
			break;
		}
	}

	return pos;
}

WVector WPolySoup::PosInWorld(const std::vector<WVector>& poly,
	const WVector& a, const WVector& b)
{
	WVector dir, e, f, c, g;
	int num = poly.size();
	dir = a - b;
	dir.y = 0.0f;
	float length = dir.Magnitude();

	if (IsClockWise(poly, b))
	{
		for (int i = 0; i < num; i++)
		{
			e = poly[i] - b;
			e.y = 0.0f;
			if (e * dir < 0.0f)
				continue;
			c = WCrossProduct(dir, e);

			f = poly[(i + 1) % num] - b;
			f.y = 0.0f;
			if (f * dir < 0.0f)
				continue;
			g = WCrossProduct(dir, f);

			if (c.y < 0.0f && g.y > 0.0f)
			{
				float r = GetCameraRadius(e, f, dir);
				if (length > r)
					length = r;
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < num; i++)
		{
			e = poly[i] - b;
			if (e * dir < 0.0f)
				continue;
			c = WCrossProduct(dir, e);

			f = poly[(i + 1) % num] - b;
			f.y = 0.0f;
			if (f * dir < 0.0f)
				continue;
			g = WCrossProduct(dir, f);

			if (c.y > 0.0f && g.y < 0.0f)
			{
				float r = GetCameraRadius(e, f, dir);
				if (length > r)
					length = r;
				break;
			}
		}
	}

	dir.Normalize();
	WVector pos = b + dir * length;
	return pos;
}

float WPolySoup::GetCameraRadius(WVector eye, WVector target, WVector up)
{
	eye.y = 0.0f;
	target.y = 0.0f;
	up.y = 0.0f;

	float lenA = eye.Magnitude();
	float lenB = target.Magnitude();

	eye.Normalize();
	target.Normalize();
	up.Normalize();
	LogOut(0, "GetCameraRadius : %f\n", up.x);

	float angle[2];
	angle[0] = (float)acos(Between(-1.0f, eye * up, 1.0f));
	angle[1] = (float)acos(Between(-1.0f, target * up, 1.0f));

	return (float)(sin(angle[0] + angle[1]) * lenB * lenA /
		(sin(angle[0]) * lenA + sin(angle[1]) * lenB));
}

bool WPolySoup::IsClockWise(const std::vector<WVector>& poly,
	const WVector& pos)
{
	WVector a = poly[1] - pos;
	WVector b = poly[0] - pos;
	a.x = a.x * b.z - b.x * a.z;
	if (a.x > 0.0f)
		return true;
	return false;
}

WPolySoup::sCamera* WPolySoup::FindCamera(const char* name)
{
	if (!name || !*name)
		return NULL;

	for (int i = 0; i < m_header.cameraNum; i++)
	{
		if (strcmpi(m_camera[i].name, name) == 0)
			return &m_camera[i];
	}

	return NULL;
}

WPolySoup::sPoint* WPolySoup::FindPoint(const char* name)
{
	if (!name || !*name)
		return NULL;

	for (int i = 0; i < m_header.pointNum; i++)
	{
		if (strcmpi(m_point[i].name, name) == 0)
			return &m_point[i];
	}

	return NULL;
}

WPolySoup::sArea* WPolySoup::FindArea(const char* name)
{
	if (!name || !*name)
		return NULL;

	for (int i = 0; i < m_header.areaNum; i++)
	{
		if (strstr(m_area[i].name, name))
			return &m_area[i];
	}

	return NULL;
}

WPolySoup::sModel* WPolySoup::FindModel(const char* name)
{
	if (!name || !*name)
		return m_baseModel;

	for (int i = 0; i < m_modelNum; i++)
	{
		if (strcmpi(m_model[i]->name, name) == 0)
			return m_model[i];
	}

	return NULL;
}

CNodeContainer* WPolySoup::FindNode(const char* name)
{
	if (!name || !*name)
		return NULL;

	for (std::vector<CNodeContainer>::iterator it = m_node.begin();
		it != m_node.end(); it++)
	{
		if (strcmpi(it->GetName().c_str(), name) == 0)
			return &(*it);
	}

	return NULL;
}

WPolySoup::sArea* WPolySoup::GetAreaByPos(const WVector& pos)
{
	for (int i = 0; i < m_header.areaNum; i++)
	{
		if (strstr(m_area[i].name, "*cam"))
		{
			if (m_area[i].aabb.IsInclude(pos))
				return &m_area[i];
		}
	}

	return NULL;
}

WPolySoup::sCamera* WPolySoup::GetCameraByName(const char* name)
{
	for (int i = 0; i < m_header.cameraNum; i++)
	{
		if (stricmp(m_camera[i].name, name) == 0)
			return &m_camera[i];
	}

	return NULL;
}

w_bound_box* WPolySoup::GetObjBoundBox(int index, const char* name)
{
	if (index == 0xffff)
		return m_baseModel->pet->m_bbList.Find(name);

	return m_model[index]->pet->m_bbList.Find(name);
}

int WPolySoup::GetObjAlpha(int index)
{
	if (index == 0xffff)
		return 0xff;

	return m_model[index]->alpha;
}

float WPolySoup::GetObjWindFactor(int index)
{
	if (index == 0xffff)
		return m_baseModel->windFactor;

	return m_model[index]->windFactor;
}

void WPolySoup::UpdateBgFxWind()
{
	CFx::Instance()->UpdateExtWind(Wind().GetGlobalWind());

	for (int i = 0; i < m_bgFxNum; i++)
	{
		if (m_bgFx[i].spray)
			m_bgFx[i].spray->SetWind(Wind().GetWind(m_bgFx[i].spray->Pos()));

		if (m_bgFx[i].sequence &&
			CFx::Instance()->FindSequence(m_bgFx[i].sequence))
			m_bgFx[i].sequence->SetWind(
				Wind().GetWind(m_bgFx[i].sequence->Pos()));
	}
}

void WPolySoup::SetAdditionModel(const char* name, const WVector& pos)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	if (!name)
		return;

	sModel* model = m_model[m_modelNum++];

	model->type = 0;
	model->flag = 0;

	model->mat = m_model[4]->mat;

	model->mat.xm = pos.x;
	model->mat.ym = pos.y;
	model->mat.zm = pos.z;

	Waabb& aabb = model->aabb;
	model->pos = (aabb.min + aabb.max) * 0.5f;
	model->center = pos * 0.5f;
	WVector size = aabb.max - aabb.min;
	float zz = size.z * size.z;
	model->radius = sqrtf(size.x * size.x + size.y * size.y + zz) * 0.5f;
	model->scale = m_model[4]->scale;
	strncpy(model->name, name, 31);
	strncpy(model->option, m_model[4]->option, 31);

	model->pet = g_resrcmng->GetPuppet(model->name, IsSeaPet(model->name),
		model->type != 0, false);

	w_bound_box* bb = model->pet->m_bbList.Find("char0");
	if (bb)
	{
		model->obb = bb->bone->GetMatrix();
		model->obb *= (bb->info->aabb.max - bb->info->aabb.min) * 0.5f +
			WVector::ONE * 1.6f;
		model->obb.center = bb->info->spherePivot * bb->bone->GetMatrix();
		g_view->DrawOBB(model->obb, 0xff00ff00);
	}

	if (!m_pDoc->m_modelNameList.Find(model->name))
	{
		char* copy = new char[strlen(model->name) + 1];
		strcpy(copy, model->name);
		m_pDoc->m_modelNameList.AddItem(copy, copy, false);
	}

	model->pet->ApplyBones(NULL, model->mat);
	model->pet->UpdateBound(true, false);
	model->pet->UpdateLightSource(&g_lightset, false, NULL);
	ProcessPostLoadPet(model);

	unsigned long flag = 0;
	if (model->type)
		flag |= 4;
	if ((model->flag & 0x10) || model->pet->GetBoundSphere().radius > 128.0f)
		flag |= 2;
	if (model->flag & 8)
		flag |= 1;

	CSceneManager::Instance()->RegisterElement(model->type ? OBJ_MODEL
														   : OBJ_STATIC,
		model->pet, NULL, model->name, flag, &model->bbox);
}

void WPolySoup::SetRenderMode(int mode)
{
	for (int i = 0; i < m_modelNum; i++)
	{
		sModel* model = m_model[i];

		if (model->pet)
			model->pet->SetRenderMode(mode);
	}

	m_baseModel->pet->SetRenderMode(mode);
}

static float Interpolate(float a, float b, float c, float u, float v)
{
	return (b - a) * u + (c - a) * v + a;
}

static void BarycentricXZ(WVector* a, WVector* p, WVector* b, WVector* c,
	float* u, float* v)
{
	WVector2D pp(p->x, p->z);
	WVector2D d[3];

	d[0].x = pp.x - a->x;
	d[0].y = pp.y - a->z;
	d[1].x = b->x - a->x;
	d[1].y = b->z - a->z;
	d[2].x = c->x - a->x;
	d[2].y = c->z - a->z;

	*u = (d[2].y * d[0].x - d[2].x * d[0].y) /
		(d[2].y * d[1].x - d[2].x * d[1].y);
	*v = (d[1].x * d[0].y - d[1].y * d[0].x) /
		(d[2].y * d[1].x - d[2].x * d[1].y);
}

static void Barycentric(float* u, float* v, const WVector* p, const WVector* a,
	const WVector* b, const WVector* c)
{
	WVector n;
	n = WCrossProduct(*b - *a, *c - *a);

	float ax = Abs(n.x);
	float ay = Abs(n.y);
	float az = Abs(n.z);
	WVector2D pp;
	WVector2D d[3];

	if (ax >= ay && ax >= az)
	{
		pp.x = p->z;
		pp.y = p->y;
		d[0].x = pp.x - a->z;
		d[0].y = pp.y - a->y;
		d[1].x = b->z - a->z;
		d[1].y = b->y - a->y;
		d[2].x = c->z - a->z;
		d[2].y = c->y - a->y;
	}
	else if (ay >= ax && ay >= az)
	{
		pp.x = p->x;
		pp.y = p->z;
		d[0].x = pp.x - a->x;
		d[0].y = pp.y - a->z;
		d[1].x = b->x - a->x;
		d[1].y = b->z - a->z;
		d[2].x = c->x - a->x;
		d[2].y = c->z - a->z;
	}
	else
	{
		pp.x = p->x;
		pp.y = p->y;
		d[0].x = pp.x - a->x;
		d[0].y = pp.y - a->y;
		d[1].x = b->x - a->x;
		d[1].y = b->y - a->y;
		d[2].x = c->x - a->x;
		d[2].y = c->y - a->y;
	}

	*u = (d[2].y * d[0].x - d[2].x * d[0].y) /
		(d[2].y * d[1].x - d[2].x * d[1].y);
	*v = (d[1].x * d[0].y - d[1].y * d[0].x) /
		(d[2].y * d[1].x - d[2].x * d[1].y);
}

void WPolySoup::MakeStatistics()
{
	mkdir("statistics");
	FILE* fp = fopen(MakeStr("statistics/%s_%02d_stat.txt", GetCurMapName(),
						 GOLFDOC()->m_currentHole),
		"wt");
	if (!fp)
		return;

	int total = 0;
	int maxFace = 0;
	int minFace = 1000;
	int count[11];
	memset(count, 0, sizeof(count));

	for (int i = 0; i < m_modelNum; i++)
	{
		sModel* model = m_model[i];
		int faceNum = model->pet->GetFaceNum();
		int index = faceNum / 100;
		if (index > 10)
			index = 10;
		count[index]++;

		total += model->pet->GetFaceNum();
		maxFace = Max(model->pet->GetFaceNum(), maxFace);
		minFace = Min(faceNum, minFace);
	}

	fprintf(fp,
		"*[%s_%02d]\xc0\xc7 \xbf\xc0\xba\xea\xc1\xa7\xc6\xae \xb8\xde\xbd\xac \xba\xd0\xc6\xf7\xb5\xb5\n\n",
		GetCurMapName(), GOLFDOC()->m_currentHole);

	for (int i = 0; i < 11; i++)
	{
		if (i == 10)
			fprintf(fp, "%3d~    : ", 1000);
		else
			fprintf(fp, "%3d~%3d : ", i * 100, i * 100 + 100);

		for (int j = count[i]; j > 0; j--)
			fprintf(fp, "*");

		fprintf(fp, "(%d)\n", count[i]);
	}

	int average = total / GetModelNum();
	int sum = 0;
	for (int i = 0; i < m_modelNum; i++)
	{
		int faceNum = m_model[i]->pet->GetFaceNum();
		sum += faceNum * faceNum;
	}
	int variance = (sum - m_modelNum * average * average) / (m_modelNum - 1);

	fprintf(fp, "\n\xc7\xd5\xb0\xe8   : %d(%d)\n",
		m_baseModel->pet->GetFaceNum() + total, m_modelNum);
	fprintf(fp, "\xc6\xf2\xb1\xd5\xb0\xaa : %d\n", average);
	fprintf(fp, "\xc3\xd6\xbc\xd2\xb0\xaa : %d\n", minFace);
	fprintf(fp, "\xc3\xd6\xb4\xeb\xb0\xaa : %d\n", maxFace);
	fprintf(fp, "\xc7\xa5\xc1\xd8\xc6\xed\xc2\xf7 : %.0f\n",
		sqrt((double)variance));
	fprintf(fp, "\xc5\xd8\xbd\xba\xc3\xc4\xb0\xb9\xbc\xf6 : %d\n",
		m_header.textureNum);

	fclose(fp);
}

bool WPolySoup::LoadShadowmapInfo()
{
	char name[64] = "";
	const char* dot;
	cFile* file;
	w_mesh* mesh;
	w_mesh* first;
	w_mesh** meshList;
	int i, meshNum, vtxNum, indexNum, numVtx, numUV, maxVtx;
	WVector* vtxList;
	float (*uvList)[2];

	dot = strrchr(m_mapName, '.');
	if (dot == NULL || dot - m_mapName >= 58)
		return false;

	strncpy(name, m_mapName, dot - m_mapName);
	strcat(name, ".sbin");

	file = GetResrcManager()->GetCFile(name, -1);
	if (file == NULL)
		return false;

	first = m_baseModel->pet->GetRootBone()->GetMesh();
	meshNum = 0;
	for (mesh = first; mesh; mesh = mesh->next)
		meshNum++;

	meshList = new w_mesh*[meshNum];

	for (i = 0, mesh = first; mesh; mesh = mesh->next, i++)
		meshList[i] = mesh;

	file->Read(&i, 4);

	if (i != meshNum)
		goto failed;

	if (meshNum == 0)
		return true;

	for (i = 0; i < meshNum; i++)
	{
		file->Read(&vtxNum, 4);
		file->Read(&indexNum, 4);

		if (vtxNum != meshList[i]->vtxNum || indexNum != meshList[i]->indexNum)
			goto failed;
	}

	numVtx = numUV = 0;
	file->Read(&numVtx, 4);
	file->Read(&numUV, 4);

	if (numVtx == 0 || numUV == 0)
	{
failed:
		delete[] meshList;
		CloseCFile(file);
		return false;
	}

	vtxList = new WVector[numVtx];
	file->Read(vtxList, numVtx * sizeof(WVector));

	uvList = new float[numUV][2];
	file->Read(uvList, numUV * sizeof(float[2]));

	{
		SmCase smCase[2];

		smLoadSmapMeshes(smCase, file, meshList, vtxList, uvList);
		smLoadOutFrags(file, meshNum, meshList, vtxList, smCase);

		if (vtxList)
			delete[] vtxList;
		if (uvList)
			delete[] uvList;

		delete[] meshList;

		maxVtx = 0;
		for (mesh = first; mesh; mesh = mesh->next)
		{
			if (maxVtx < mesh->vtxNum)
				maxVtx = mesh->vtxNum;
		}

		m_baseModel->pet->GetRootBone()->AllocVTX(maxVtx);

		CloseCFile(file);

		return true;
	}
}

void WPolySoup::smLoadSmapMeshes(SmCase* smCase, cFile* file, w_mesh** meshList,
	WVector* vtxList, float (*uvList)[2])
{
	w_mesh* last = m_baseModel->pet->GetRootBone()->GetMesh();

	while (last && last->next)
		last = last->next;

	w_mesh* first;
	w_mesh* mesh;
	int size;

	file->Read(&size, 4);

	first = NULL;

	if (size > 0)
	{
		file->Read(&smCase[0].num, 4);

		if (smCase[0].num > 0)
			smCase[0].list = new SmInFragList[smCase[0].num];

		for (int i = 0; i < smCase[0].num; i++)
		{
			mesh = smAddSmapPack(&smCase[0].list[i], file, 0, i, meshList,
				vtxList, uvList);

			last->next = mesh;
			last = mesh;

			if (i == 0)
				first = mesh;
		}
	}

	switch (GetDiffFlag())
	{
	case 0:
		file->Read(&size, 4);
		file->Seek(size, 1);
		file->Read(&size, 4);
		file->Seek(size, 1);
		break;

	case 2:
		file->Read(&size, 4);
		file->Seek(size, 1);
	case 1:
		file->Read(&size, 4);

		if (size > 0)
			file->Read(&smCase[1].num, 4);
		break;
	}

	if (smCase[1].num > 0)
		smCase[1].list = new SmInFragList[smCase[0].num];

	for (int i = 0; i < smCase[1].num; i++)
	{
		mesh = smAddSmapPack(&smCase[1].list[i], file, GetDiffFlag(), i,
			meshList, vtxList, uvList);

		last->next = mesh;
		last = mesh;
	}

	if (GetDiffFlag() == 1)
	{
		file->Read(&size, 4);
		file->Seek(size, 1);
	}

	m_smapMesh = first;

	for (m_smapMeshNum = 0; first; first = first->next)
		m_smapMeshNum++;

	m_smapTexHandle = new int[m_smapMeshNum];

	int i = 0;
	for (mesh = m_smapMesh; mesh; mesh = mesh->next)
		m_smapTexHandle[i++] = mesh->drawFlag & 0x7ff;
}

struct sDDSHeader
{
	unsigned long dwSize;
	unsigned long dwFlags;
	unsigned long dwHeight;
	unsigned long dwWidth;
	unsigned long dwLinearSize;
	unsigned long dwDepth;
	unsigned long dwMipMapCount;
	unsigned long dwReserved1[11];
	unsigned long dwPfSize;
	unsigned long dwPfFlags;
	unsigned long dwFourCC;
	unsigned long dwRGBBitCount;
	unsigned long dwRBitMask;
	unsigned long dwGBitMask;
	unsigned long dwBBitMask;
	unsigned long dwABitMask;
	unsigned long dwCaps;
	unsigned long dwCaps2;
	unsigned long dwCaps3;
	unsigned long dwCaps4;
	unsigned long dwReserved2;
};

w_mesh* WPolySoup::smAddSmapPack(SmInFragList* fragList, cFile* file,
	int caseIndex, int listIndex, w_mesh** meshList, WVector* vtxList,
	float (*uvList)[2])
{
	int width, height;
	file->Read(&width, 4);
	file->Read(&height, 4);

	sDDSHeader ddsd;
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = 0x80000;
	ddsd.dwMipMapCount = 1;
	ddsd.dwPfSize = 0x20;
	ddsd.dwPfFlags = 4;
	ddsd.dwFourCC = '1TXD';
	ddsd.dwCaps = 0x1000;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
	ddsd.dwLinearSize = width * height / 2;

	int size = ddsd.dwLinearSize + 0x80;
	unsigned char* buf = new unsigned char[size];
	memcpy(buf + 4, &ddsd, sizeof(ddsd));
	buf[0] = 'D';
	buf[1] = 'D';
	buf[2] = 'S';
	buf[3] = ' ';
	file->Read(buf + 0x80, ddsd.dwLinearSize);

	char name[32];
	sprintf(name, "%s-%d-%d.dds", m_baseModel->name, caseIndex, listIndex);

	int texHandle = g_resrcmng->LoadTexture_DDS(buf, size, name, 0, 0);
	delete[] buf;

	int fragNum;
	file->Read(&fragNum, 4);

	SmInFragment* frag = new SmInFragment[fragNum];
	fragList->num = fragNum;
	fragList->frag = frag;
	file->Read(frag, fragNum * sizeof(SmInFragment));

	w_mesh* mesh = new w_mesh;
	memset(mesh, 0, sizeof(w_mesh));

	mesh->texHandle = texHandle;
	mesh->drawFlag = (texHandle & 0x7ff) | 0x21100000;
	mesh->diffuse = 0xffffff;
	mesh->alpha = 1.0f;

	mesh->indexNum = fragNum * 3;
	mesh->indexList = new unsigned short[mesh->indexNum];
	WVector* vtx = new WVector[mesh->indexNum];
	float (*uv)[2] = new float[mesh->indexNum][2];
	unsigned long* color = new unsigned long[mesh->indexNum];
	int vtxNum = 0;

	for (int i = 0; i < fragNum; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			int code = frag[i].vtx[k];
			int uvIndex = frag[i].uv[k];
			int j = 0;
			w_mesh* src;

			if (code < 0x8000)
			{
				src = meshList[frag[i].mesh];
				unsigned short i0 = src->indexList[frag[i].tri * 3];
				unsigned short i1 = src->indexList[frag[i].tri * 3 + 1];
				int i2 = src->indexList[frag[i].tri * 3 + 2];
				float u, v;
				WVector* vec = src->vecList;
				Barycentric(&u, &v, &vtxList[code], &vec[i0], &vec[i1],
					&vec[i2]);

				unsigned char* c = (unsigned char*)src->vtxColorList;
				unsigned long c0 = (unsigned char)Interpolate(c[i0 * 4 + 3],
					c[i1 * 4 + 3], c[i2 * 4 + 3], u, v);
				unsigned long col = (c0 << 24) | 0xffffff;

				for (j = 0; j < vtxNum; j++)
				{
					if (WisEqual(vtx[j].x, vtxList[code].x, g_EPSILON) &&
						WisEqual(vtx[j].y, vtxList[code].y, g_EPSILON) &&
						WisEqual(vtx[j].z, vtxList[code].z, g_EPSILON) &&
						color[j] == col &&
						WisEqual(uv[j][0], uvList[uvIndex][0], g_EPSILON) &&
						WisEqual(uv[j][1], uvList[uvIndex][1], g_EPSILON))
						break;
				}

				if (j == vtxNum)
				{
					vtx[vtxNum] = vtxList[code];
					color[vtxNum] = col;
					memcpy(uv[vtxNum], uvList[uvIndex], sizeof(float) * 2);
					mesh->indexList[i * 3 + k] = vtxNum;
					vtxNum++;
				}
				else
				{
					mesh->indexList[i * 3 + k] = j;
				}
			}
			else
			{
				src = meshList[frag[i].mesh];
				code -= 0x8000;

				for (j = 0; j < vtxNum; j++)
				{
					if (WisEqual(vtx[j].x, src->vecList[code].x, g_EPSILON) &&
						WisEqual(vtx[j].y, src->vecList[code].y, g_EPSILON) &&
						WisEqual(vtx[j].z, src->vecList[code].z, g_EPSILON) &&
						color[j] == (src->vtxColorList[code] | 0xffffff) &&
						WisEqual(uv[j][0], uvList[uvIndex][0], g_EPSILON) &&
						WisEqual(uv[j][1], uvList[uvIndex][1], g_EPSILON))
						break;
				}

				if (j == vtxNum)
				{
					vtx[vtxNum] = src->vecList[code];
					memcpy(uv[vtxNum], uvList[uvIndex], sizeof(float) * 2);
					color[vtxNum] = src->vtxColorList[code] | 0xffffff;
					mesh->indexList[i * 3 + k] = vtxNum;
					vtxNum++;
				}
				else
				{
					mesh->indexList[i * 3 + k] = j;
				}
			}
		}
	}

	mesh->vtxNum = vtxNum;
	mesh->vecList = new WVector[vtxNum];
	memcpy(mesh->vecList, vtx, vtxNum * sizeof(WVector));
	mesh->vtxColorList = new unsigned long[vtxNum];
	memcpy(mesh->vtxColorList, color, vtxNum * sizeof(unsigned long));
	mesh->originalVtxColorList = new unsigned long[vtxNum];
	memcpy(mesh->originalVtxColorList, color, vtxNum * sizeof(unsigned long));
	mesh->uvData = new float[vtxNum][2];
	memcpy(mesh->uvData, uv, vtxNum * sizeof(float[2]));

	WBone::CalcMeshAABB(*m_baseModel->pet->GetRootBone(), *mesh);

	delete[] vtx;
	delete[] uv;
	delete[] color;

	return mesh;
}

void WPolySoup::smLoadOutFrags(cFile* file, int num, w_mesh** meshList,
	WVector* vtxList, SmCase* smCase)
{
	int c, i, j, k, r;
	SmInFragList* list;

	int extra = 0;
	for (c = 0; c < 2; c++)
	{
		if (smCase[c].list)
		{
			for (i = 0; i < smCase[c].num; i++)
			{
				list = smCase[c].list;
				if (list)
				{
					for (j = 0; j < list[i].num; j++)
					{
						if (list[i].frag[j].tri != 0xffff)
							extra++;
					}
				}
			}
		}
	}

	int outNum;
	file->Read(&outNum, 4);

	int detailNum = 0;
	int skipAfter = 0;
	int skipBefore = 0;

	switch (GetDiffFlag())
	{
	case 0:
		file->Read(&skipBefore, 4);
		file->Read(&skipAfter, 4);
		break;

	case 1:
		file->Read(&detailNum, 4);
		file->Read(&skipAfter, 4);
		break;

	case 2:
		file->Read(&skipBefore, 4);
		file->Read(&detailNum, 4);
		break;
	}

	int* count = new int[num * 4];
	memset(count, 0, num * 4 * sizeof(int));

	SmOutFrag* frag = new SmOutFrag[detailNum + outNum + extra];

	if (outNum > 0)
		file->Read(&count[num], num * sizeof(int));
	if (skipBefore > 0)
		file->Seek(num * sizeof(int), 1);
	if (detailNum > 0)
		file->Read(&count[num * 2], num * sizeof(int));
	if (skipAfter > 0)
		file->Seek(num * sizeof(int), 1);

	for (c = 0; c < 2; c++)
	{
		if (smCase[c].list)
		{
			for (j = 0; j < smCase[c].num; j++)
			{
				list = smCase[c].list;
				if (list)
				{
					for (k = 0; k < list[j].num; k++)
					{
						SmInFragment* in = &list[j].frag[k];
						if (in->tri != 0xffff)
							count[num * 3 + in->mesh]++;
					}
				}
			}
		}
	}

	count[0] = 0;
	for (i = 1; i < num; i++)
		count[i] = count[i - 1] + count[num * 3 + i - 1] + count[num + i - 1] +
			count[num * 2 + i - 1];

	memset(&count[num * 3], 0, num * sizeof(int));

	for (i = 0; i < num; i++)
	{
		if (count[num + i] > 0)
			file->Read(&frag[count[i]], count[num + i] * sizeof(SmOutFrag));
	}

	if (skipBefore)
		file->Seek(skipBefore * sizeof(SmOutFrag), 1);

	if (detailNum > 0)
	{
		for (i = 0; i < num; i++)
		{
			if (count[num * 2 + i] > 0)
				file->Read(&frag[count[i] + count[num + i]],
					count[num * 2 + i] * sizeof(SmOutFrag));
		}
	}

	for (c = 0; c < 2; c++)
	{
		if (smCase[c].list)
		{
			for (j = 0; j < smCase[c].num; j++)
			{
				list = smCase[c].list;
				if (list)
				{
					for (k = 0; k < list[j].num; k++)
					{
						SmInFragment* in = &list[j].frag[k];
						if (in->tri != 0xffff)
						{
							int mesh = in->mesh;
							int* slot = &count[num * 3 + mesh];
							SmOutFrag* out = &frag[count[num * 2 + mesh] +
								count[num + mesh] + count[mesh] + *slot];
							out->tri = in->tri;
							memcpy(out->vtx, in->vtx, sizeof(out->vtx));
							(*slot)++;
						}
					}
				}
			}
		}
	}

	if (skipAfter)
		file->Seek(skipAfter * sizeof(SmOutFrag), 1);

	for (i = 0; i < num; i++)
	{
		if (count[num * 2 + i] > 0)
			file->Read(&frag[count[i] + count[num + i]],
				count[num * 2 + i] * sizeof(SmOutFrag));

		count[num + i] += count[num * 3 + i] + count[num * 2 + i];
	}

	for (i = 0; i < num; i++)
	{
		if (count[num + i] > 0)
		{
			int unique = 0;

			for (j = 0; j < count[num + i] - 1; j++)
			{
				for (k = j + 1; k < count[num + i]; k++)
				{
					for (r = 0; r < 3; r++)
					{
						if (frag[count[i] + j].vtx[0] ==
								frag[count[i] + k].vtx[r] &&
							frag[count[i] + j].vtx[1] ==
								frag[count[i] + k].vtx[(r + 1) % 3] &&
							frag[count[i] + j].vtx[2] ==
								frag[count[i] + k].vtx[(r + 2) % 3])
							break;
					}

					if (r < 3)
						break;
				}

				if (k == count[num + i])
					unique++;
			}

			if (unique + 1 != count[num + i])
			{
				SmOutFrag* packed = new SmOutFrag[unique + 1];

				for (unique = 0, j = 0; j < count[num + i] - 1; j++)
				{
					for (k = j + 1; k < count[num + i]; k++)
					{
						for (r = 0; r < 3; r++)
						{
							if (frag[count[i] + j].vtx[0] ==
									frag[count[i] + k].vtx[r] &&
								frag[count[i] + j].vtx[1] ==
									frag[count[i] + k].vtx[(r + 1) % 3] &&
								frag[count[i] + j].vtx[2] ==
									frag[count[i] + k].vtx[(r + 2) % 3])
								break;
						}

						if (r < 3)
							break;
					}

					if (k == count[num + i])
						memcpy(&packed[unique++], &frag[count[i] + j],
							sizeof(SmOutFrag));
				}

				memcpy(&packed[unique++], &frag[count[i] + j],
					sizeof(SmOutFrag));

				qsort(packed, unique, sizeof(SmOutFrag), smCompareOutFrag);
				smBuildOutFragMeshes(meshList[i], packed, unique, vtxList);

				delete[] packed;
			}
			else
			{
				qsort(&frag[count[i]], count[num + i], sizeof(SmOutFrag),
					smCompareOutFrag);
				smBuildOutFragMeshes(meshList[i], &frag[count[i]],
					count[num + i], vtxList);
			}
		}
	}

	delete[] count;
	delete[] frag;
}

int WPolySoup::smCompareOutFrag(const void* a, const void* b)
{
	const SmOutFrag* fa = (const SmOutFrag*)a;
	const SmOutFrag* fb = (const SmOutFrag*)b;

	if (fa->tri < fb->tri)
		return -1;
	return fa->tri > fb->tri;
}

void WPolySoup::smBuildOutFragMeshes(w_mesh* mesh, SmOutFrag* frag, int num,
	WVector* vtxList)
{
	if (num == 0)
		return;

	unsigned short* indexList = mesh->indexList;
	WVector* vecList = mesh->vecList;
	WVector* normList = mesh->normList;
	float (*uvData)[2] = mesh->uvData;
	unsigned long* colorList = mesh->vtxColorList;

	int i, j, k;
	int triNum = 1;
	for (i = 1; i < num; i++)
	{
		if (frag[i].tri != frag[i - 1].tri)
			triNum++;
	}

	int keep = mesh->indexNum - triNum * 3;
	int indexNum = keep + num * 3;
	unsigned short* newIndex = new unsigned short[indexNum];
	WVector* vtx = new WVector[num * 3];
	WVector* normal = new WVector[num * 3];
	float (*uv)[2] = new float[num * 3][2];
	unsigned long* color = new unsigned long[num * 3];

	int vtxNum = 0;
	for (i = 0; i < num; i++)
	{
		int tri = frag[i].tri * 3;
		int i0 = indexList[tri];
		int i1 = indexList[tri + 1];
		int i2 = indexList[tri + 2];

		for (k = 0; k < 3; k++)
		{
			int code = frag[i].vtx[k];

			if (code < 0x8000)
			{
				float u, v;
				Barycentric(&u, &v, &vtxList[code], &vecList[i0], &vecList[i1],
					&vecList[i2]);

				WVector n;
				n.x = Interpolate(normList[i0].x, normList[i1].x,
					normList[i2].x, u, v);
				n.y = Interpolate(normList[i0].y, normList[i1].y,
					normList[i2].y, u, v);
				n.z = Interpolate(normList[i0].z, normList[i1].z,
					normList[i2].z, u, v);

				unsigned char* c0 = (unsigned char*)&colorList[i0];
				unsigned char* c1 = (unsigned char*)&colorList[i1];
				unsigned char* c2 = (unsigned char*)&colorList[i2];
				float a0 = c0[3];
				float r0 = c0[2];
				float g0 = c0[1];
				float b0 = (float)(colorList[i0] & 0xff);
				float a2 = c2[3];
				float g2 = c2[1];
				unsigned char a =
					(unsigned char)Interpolate(a0, c1[3], a2, u, v);
				unsigned char r = (unsigned char)Interpolate(r0,
					(float)((colorList[i1] >> 16) & 0xff),
					(float)((colorList[i2] >> 16) & 0xff), u, v);
				unsigned char g =
					(unsigned char)Interpolate(g0, c1[1], g2, u, v);
				unsigned char b = (unsigned char)Interpolate(b0,
					(float)(colorList[i1] & 0xff),
					(float)(colorList[i2] & 0xff), u, v);
				unsigned long col = (a << 24) | (r << 16) | (g << 8) | b;

				float t[2];
				t[0] = Interpolate(uvData[i0][0], uvData[i1][0], uvData[i2][0],
					u, v);
				t[1] = Interpolate(uvData[i0][1], uvData[i1][1], uvData[i2][1],
					u, v);

				for (j = 0; j < vtxNum; j++)
				{
					if (WisEqual(vtx[j].x, vtxList[code].x, g_EPSILON) &&
						WisEqual(vtx[j].y, vtxList[code].y, g_EPSILON) &&
						WisEqual(vtx[j].z, vtxList[code].z, g_EPSILON) &&
						WisEqual(normal[j].x, n.x, g_EPSILON) &&
						WisEqual(normal[j].y, n.y, g_EPSILON) &&
						WisEqual(normal[j].z, n.z, g_EPSILON) &&
						WisEqual(uv[j][0], t[0], g_EPSILON) &&
						WisEqual(uv[j][1], t[1], g_EPSILON))
						break;
				}

				if (j == vtxNum)
				{
					vtx[vtxNum] = vtxList[code];
					normal[vtxNum] = n;
					memcpy(uv[vtxNum], t, sizeof(t));
					color[vtxNum] = col;
					newIndex[keep + i * 3 + k] = mesh->vtxNum + vtxNum;
					vtxNum++;
				}
				else
				{
					newIndex[keep + i * 3 + k] = mesh->vtxNum + j;
				}
			}
			else
			{
				newIndex[keep + i * 3 + k] = code + 0x8000;
			}
		}
	}

	keep = 0;
	if (frag[0].tri > 0)
	{
		memcpy(newIndex, indexList, frag[0].tri * 3 * sizeof(unsigned short));
		keep = frag[0].tri * 3;
	}

	for (i = 1; i < num; i++)
	{
		int prev = frag[i - 1].tri * 3;
		int cur = frag[i].tri * 3;

		if (cur - prev > 3)
		{
			memcpy(&newIndex[keep], &indexList[prev + 3],
				(cur - prev - 3) * sizeof(unsigned short));
			keep += cur - prev - 3;
		}
	}

	int prev = frag[i - 1].tri * 3;
	int rest = mesh->indexNum - prev;

	if (rest > 3)
		memcpy(&newIndex[keep], &indexList[prev + 3],
			(rest - 3) * sizeof(unsigned short));

	WVector* newVec = new WVector[mesh->vtxNum + vtxNum];
	memcpy(newVec, mesh->vecList, mesh->vtxNum * sizeof(WVector));
	memcpy(&newVec[mesh->vtxNum], vtx, vtxNum * sizeof(WVector));

	WVector* newNorm = new WVector[mesh->vtxNum + vtxNum];
	memcpy(newNorm, mesh->normList, mesh->vtxNum * sizeof(WVector));
	memcpy(&newNorm[mesh->vtxNum], normal, vtxNum * sizeof(WVector));

	unsigned long* newColor = new unsigned long[mesh->vtxNum + vtxNum];
	memcpy(newColor, mesh->vtxColorList, mesh->vtxNum * sizeof(unsigned long));
	memcpy(&newColor[mesh->vtxNum], color, vtxNum * sizeof(unsigned long));

	unsigned long* newOrigColor = new unsigned long[mesh->vtxNum + vtxNum];
	memcpy(newOrigColor, newColor,
		(mesh->vtxNum + vtxNum) * sizeof(unsigned long));

	float (*newUV)[2] = new float[mesh->vtxNum + vtxNum][2];
	memcpy(newUV, mesh->uvData, mesh->vtxNum * sizeof(float) * 2);
	memcpy(&newUV[mesh->vtxNum], uv, vtxNum * sizeof(float) * 2);

	delete[] vtx;
	delete[] normal;
	delete[] uv;
	delete[] color;

	delete[] mesh->vecList;
	delete[] mesh->normList;
	delete[] mesh->vtxColorList;
	delete[] mesh->originalVtxColorList;
	delete[] mesh->uvData;
	delete[] mesh->indexList;

	mesh->vtxNum += vtxNum;
	mesh->indexNum = indexNum;
	mesh->vecList = newVec;
	mesh->normList = newNorm;
	mesh->vtxColorList = newColor;
	mesh->originalVtxColorList = newOrigColor;
	mesh->uvData = newUV;
	mesh->indexList = newIndex;
}

void WPolySoup::EnableShadowmap(bool bEnable)
{
	if (bEnable)
	{
		if (m_smapMesh)
		{
			for (w_mesh* mesh = m_smapMesh; mesh; mesh = mesh->next)
			{
				if ((mesh->drawFlag & 0x21100000) == 0x21100000)
					mesh->alpha = 1.0f;
			}
		}
		else if (!m_bSmapLoadFailed)
			ReloadBasePet();
	}
	else
	{
		for (w_mesh* mesh = m_smapMesh; mesh; mesh = mesh->next)
		{
			if ((mesh->drawFlag & 0x21100000) == 0x21100000)
				mesh->alpha = 0.0f;
		}
	}
}

void WPolySoup::ReloadBasePet()
{
	if (g_resrcmng == NULL)
		return;

	if (m_baseModel->pet)
	{
		g_resrcmng->Release(m_baseModel->pet, true);
		m_baseModel->pet = NULL;
	}

	m_baseModel->pet =
		g_resrcmng->GetPuppet(m_baseModel->name, false, false, false);

	m_baseModel->pet->m_lightmode = 0x1000000;
	m_baseModel->pet->ApplyBones(NULL, m_baseModel->mat);
	m_baseModel->pet->UpdateBound(true, false);
	m_baseModel->pet->UpdateLightSource(&g_lightset, false, NULL);

	cFile* file = GetResrcManager()->GetCFile(m_mapName, -1);

	int offset = (m_header.textureNum + m_header.cameraNum * 2) * 32 +
		m_header.areaNum * 89 + 40 + m_header.pointNum * 45;
	for (int i = 0; i < m_header.pointNum; i++)
	{
		if (m_point[i].type)
			offset += 64;
	}

	for (std::vector<CNodeContainer>::iterator it = m_node.begin();
		it != m_node.end(); it++)
		offset += (it->GetList().size() * 3 + 6) * 4;

	file->Seek(offset + 0xac, 1);

	for (w_mesh* mesh = m_baseModel->pet->GetRootBone()->GetMesh(); mesh;
		mesh = mesh->next)
	{
		for (int i = 0; i < mesh->indexNum; i += 3)
		{
			for (int j = 0; j < 3; j++)
				file->Read(&mesh->vtxColorList[mesh->indexList[i + j]], 4);
		}
	}

	CloseCFile(file);

	m_bSmapLoadFailed = !LoadShadowmapInfo();

	if (strstr(m_mapName, "rank") == NULL)
		PlaceHoleCup();

	m_baseModel->pet->xUpdateTnLBuffers(true);

	WSingleton<CSceneManager>::Instance()->SetPuppet(m_baseModel->name,
		m_baseModel->pet);
}

void WPolySoup::PlaceHoleCup()
{
	int i;
	for (i = 0; i < m_header.pointNum; i++)
	{
		if (strstr(m_point[i].name, "\xb3\xa1\xc1\xa1"))
			break;
	}

	if (i == m_header.pointNum)
		return;

	WMatrix inv;
	WVector center;
	std::vector<sCupVtx> cup;
	std::vector<sHoleTri> tris;
	std::vector<sHoleVtx> vtxs;
	std::vector<sHoleMesh> meshes;

	inv = ~m_baseModel->pet->GetRootBone()->GetMatrix();
	WVector pin = GOLFDOC()->GetHoleData().pin;
	center = pin * inv;

	float radius = GolfBall().GetRadius() * 3.0f;

	if (IsLocalContent(S3_ROOKIE_CHANNEL))
	{
		if ((Doc()->m_curChannel.Type & 0x800) &&
			Doc()->m_golfGame.gameType != 0xb &&
			Doc()->m_golfGame.gameType != 0xc)
			radius = GolfBall().GetRadius() * 6.0f;
	}

	sCupVtx vtx;
	for (i = 0; i < 33; i++)
	{
		float angle = i * (g_PI / 16.0f);
		vtx.pos.x = (float)cos(angle) * radius + center.x;
		vtx.pos.z = center.z - (float)sin(angle) * radius;
		cup.push_back(vtx);
	}

	GatherHolecupTris(tris, cup, center);

	if (tris.size() == 0)
		return;

	CreateNewHolecupMeshes(meshes, cup, tris);
	AddHolecup(meshes, cup, tris);

	w_mesh* mesh = m_baseModel->pet->GetRootBone()->GetMesh();
	int maxVtx = 0;
	for (; mesh; mesh = mesh->next)
	{
		if (maxVtx < mesh->vtxNum)
			maxVtx = mesh->vtxNum;
	}

	m_baseModel->pet->GetRootBone()->AllocVTX(maxVtx);
}

void WPolySoup::GatherHolecupTris(std::vector<sHoleTri>& tris,
	std::vector<sCupVtx>& cup, WVector& center)
{
	std::vector<int> holeTex;
	int i, j, k;
	sHoleTri hole;

	for (i = 0; i < m_header.textureNum; i++)
	{
		const sProperty& prop = GetProperty(m_texture[i].propIndex);

		if (prop.ground == 2)
			holeTex.push_back(m_texture[i].texHandle);
	}

	for (int pass = 0; pass < 2; pass++)
	{
		for (w_mesh* mesh = m_baseModel->pet->GetRootBone()->GetMesh(); mesh;
			mesh = mesh->next)
		{
			if (pass == 0)
			{
				if ((mesh->drawFlag & 0x300000) != 0x100000)
				{
					if (std::find(holeTex.begin(), holeTex.end(),
							mesh->drawFlag & 0x7ff) == holeTex.end())
						continue;
				}
			}
			else
			{
				if ((mesh->drawFlag & 0x300000) == 0x100000)
					continue;

				if (std::find(holeTex.begin(), holeTex.end(),
						mesh->drawFlag & 0x7ff) != holeTex.end())
					continue;
			}

			for (i = 0; i < mesh->indexNum; i += 3)
			{
				hole.pos[0] = &mesh->vecList[mesh->indexList[i]];
				hole.pos[1] = &mesh->vecList[mesh->indexList[i + 1]];
				hole.pos[2] = &mesh->vecList[mesh->indexList[i + 2]];

				WVector normal = WCrossProduct(*hole.pos[1] - *hole.pos[0],
					*hole.pos[2] - *hole.pos[0])
									 .Normalize();

				if (normal.y < 0.001f)
					continue;

				for (j = 0; j < (int)cup.size() - 1; j++)
				{
					if (IsInTri(cup[j].pos, hole.pos))
					{
						GenHolecupTri(&hole, mesh, i);
						tris.push_back(hole);
						break;
					}
				}

				if (j < (int)cup.size() - 1)
					continue;

				for (k = 0; k < 3; k++)
				{
					if (IsInPoly(*hole.pos[k], cup))
					{
						GenHolecupTri(&hole, mesh, i);
						tris.push_back(hole);
						break;
					}
				}

				if (k < 3)
					continue;

				for (j = 0; j < (int)cup.size() - 1; j++)
				{
					sCupVtx* v = &cup[j];

					for (k = 0; k < 3; k++)
					{
						if (AreCrossed(&v[0].pos, &v[1].pos, hole.pos[k],
								hole.pos[(k + 1) % 3]))
						{
							GenHolecupTri(&hole, mesh, i);
							tris.push_back(hole);
							break;
						}
					}

					if (k < 3)
						break;
				}
			}
		}

		for (j = 0; j < (int)tris.size(); j++)
		{
			if ((tris[j].mesh->drawFlag & 0x300000) != 0x100000)
				goto found;
		}
	}

found:
	for (i = 0; i < (int)cup.size(); i++)
	{
		sCupVtx& cv = cup[i];

		for (j = 0; j < (int)tris.size(); j++)
		{
			sHoleTri& tv = tris[j];

			if (IsInTri(cv.pos, tv.pos))
			{
				cv.mesh = tv.mesh;
				cup[i].mesh = tris[j].mesh;
				cup[i].index = tris[j].offset;
				break;
			}
		}
	}
}

bool WPolySoup::IsInTri(WVector& pos, WVector** const tri)
{
	WVector2D v[2], p;

	v[0].x = tri[1]->x - tri[0]->x;
	v[0].y = tri[1]->z - tri[0]->z;
	v[0] /= v[0].Magnitude();
	v[1].x = tri[2]->x - tri[0]->x;
	v[1].y = tri[2]->z - tri[0]->z;
	v[1] /= v[1].Magnitude();
	p.x = pos.x;
	p.y = pos.z;

	float s = v[0].x * v[1].y - v[0].y * v[1].x;

	if ((s > 0.0f ? s : -s) < 0.001f)
		return false;

	for (int i = 0; i < 3; i++)
	{
		v[0].x = tri[(i + 1) % 3]->x - tri[i]->x;
		v[0].y = tri[(i + 1) % 3]->z - tri[i]->z;
		v[0] /= v[0].Magnitude();
		v[1].x = p.x - tri[i]->x;
		v[1].y = p.y - tri[i]->z;
		v[1] /= v[1].Magnitude();

		float c = v[0].x * v[1].y - v[0].y * v[1].x;

		if (c * s < 0.0f)
			return false;
	}

	return true;
}

bool WPolySoup::IsInPoly(WVector& pos, std::vector<sCupVtx>& cup)
{
	if (cup.size() < 4)
		return false;

	int n = cup.size();
	WVector2D v[3];
	for (int k = 0; k < 3; k++)
	{
		v[k].x = cup[k].pos.x;
		v[k].y = cup[k].pos.z;
	}
	WVector2D p(pos.x, pos.z);
	WVector2D e[2];
	e[0] = v[1] - v[0];
	float len = e[0].Magnitude();
	e[0].x /= len;
	e[0].y /= len;
	e[1] = v[2] - v[0];
	e[1] /= e[1].Magnitude();
	float s = e[0].x * e[1].y - e[0].y * e[1].x;

	for (int i = 0; i < (int)cup.size() - 1; i++)
	{
		WVector2D c0(cup[i].pos.x, cup[i].pos.z);
		WVector2D c1(cup[i + 1].pos.x, cup[i + 1].pos.z);
		e[0] = c1 - c0;
		len = e[0].Magnitude();
		e[0].x /= len;
		e[0].y /= len;
		e[1] = p - c0;
		e[1] /= e[1].Magnitude();

		if ((e[0].x * e[1].y - e[0].y * e[1].x) * s <= 0.0f)
			return false;
	}

	return true;
}

bool WPolySoup::AreCrossed(WVector* a1, WVector* a2, WVector* b1, WVector* b2)
{
	WVector2D n, m;

	n.x = a2->x - a1->x;
	n.y = a2->z - a1->z;
	n /= n.Magnitude();
	m.x = b1->x - a1->x;
	m.y = b1->z - a1->z;
	m /= m.Magnitude();

	float s[2];
	s[0] = n.x * m.y - n.y * m.x;

	m.x = b2->x - a1->x;
	m.y = b2->z - a1->z;
	m /= m.Magnitude();

	s[1] = n.x * m.y - n.y * m.x;

	if (s[0] * s[1] > 0.0f)
		return false;

	n.x = b2->x - b1->x;
	n.y = b2->z - b1->z;
	n /= n.Magnitude();
	m.x = a1->x - b1->x;
	m.y = a1->z - b1->z;
	m /= m.Magnitude();

	s[0] = n.x * m.y - n.y * m.x;

	m.x = a2->x - b1->x;
	m.y = a2->z - b1->z;
	m /= m.Magnitude();

	s[1] = n.x * m.y - n.y * m.x;

	if (s[0] * s[1] > 0.0f)
		return false;

	return true;
}

void WPolySoup::GenHolecupTri(sHoleTri* tri, w_mesh* mesh, int offset)
{
	tri->mesh = mesh;
	tri->offset = offset;

	for (int i = 0; i < 3; i++)
	{
		tri->index[i] = mesh->indexList[offset + i];
		tri->normal[i] = &mesh->normList[tri->index[i]];
		tri->color[i] = mesh->vtxColorList[tri->index[i]];
		tri->uv[i][0] = mesh->uvData[tri->index[i]][0];
		tri->uv[i][1] = mesh->uvData[tri->index[i]][1];

		if (mesh->uvBackup)
		{
			tri->buv[i][0] = mesh->uvBackup[tri->index[i]][0];
			tri->buv[i][1] = mesh->uvBackup[tri->index[i]][1];
		}

		WVector* p = tri->pos[i];
		tri->edge[i].z = 0.0f;
		tri->edge[i].x = p->z - tri->pos[(i + 1) % 3]->z;
		tri->edge[i].y = tri->pos[(i + 1) % 3]->x - p->x;
		tri->edge[i].Normalize();
		tri->edge[i].z = -1.0f *
			(tri->pos[i]->x * tri->edge[i].x + tri->pos[i]->z * tri->edge[i].y);
	}
}

bool WPolySoup::GetIntersection(WVector* out, WVector* p0, WVector* p1,
	WVector* e0, WVector* e1, WVector* line)
{
	float d[2];

	d[0] = p0->x * line->x + p0->z * line->y + line->z;

	if ((int)(Wabs(d[0]) < 0.001f))
		return false;

	d[1] = p1->x * line->x + p1->z * line->y + line->z;

	if (Wabs(d[1]) < 0.001f)
		return false;

	if (d[1] * d[0] > 0.0f)
		return false;

	out->x = (p1->x - p0->x) * d[0] / (d[0] - d[1]) + p0->x;
	out->z = (p1->z - p0->z) * d[0] / (d[0] - d[1]) + p0->z;

	float ax = Wabs(e1->x - e0->x);
	float az = Wabs(e1->z - e0->z);

	bool in;
	if (ax > az)
		in = (out->x - e0->x) * (e1->x - out->x) > 0.0f;
	else
		in = (out->z - e0->z) * (e1->z - out->z) > 0.0f;

	return in;
}

void WPolySoup::CreateNewHolecupMeshes(std::vector<sHoleMesh>& meshes,
	std::vector<sCupVtx>& cup, std::vector<sHoleTri>& tris)
{
	std::vector<WVector> poly;
	std::vector<sHoleTri>::iterator it = tris.begin();

	while (it != tris.end())
	{
		if (ClipHolecup(poly, *it, cup) >= 3)
		{
			BuildNewHolecupMesh(meshes, *it, poly);
			it++;
		}
		else
		{
			it = tris.erase(it);
		}
	}
}

int WPolySoup::ClipHolecup(std::vector<WVector>& poly, sHoleTri& tri,
	std::vector<sCupVtx>& cup)
{
	std::vector<WVector> work;
	std::vector<WVector>* in;
	std::vector<WVector>* out;
	int i, k;
	int side[2];
	float d;

	poly.clear();
	int n = cup.size();
	WVector* v = NULL;

	for (k = 0; k < n; k++)
		work.push_back(cup[k].pos);

	in = &work;
	out = &poly;

	for (k = 0; k < 3; k++)
	{
		v = &(*in)[0];
		d = v->x * tri.edge[k].x + v->z * tri.edge[k].y + tri.edge[k].z;
		side[0] = d > 0.001f ? 1 : (d < -0.001f ? -1 : 0);

		if (side[0] <= 0)
			out->push_back(*v);

		for (i = 1; i < n; i++)
		{
			d = (*in)[i].x * tri.edge[k].x + (*in)[i].z * tri.edge[k].y +
				tri.edge[k].z;
			side[1] = d > 0.001f ? 1 : (d < -0.001f ? -1 : 0);

			if (side[0] == 0)
			{
				if (side[1] < 0)
					out->push_back((*in)[i]);
			}
			else if (side[1] == 0)
			{
				out->push_back((*in)[i]);
			}
			else
			{
				if (side[1] * side[0] < 0)
				{
					WVector cross;
					GetIntersection(&cross, &(*in)[i - 1], &(*in)[i],
						tri.pos[k], tri.pos[(k + 1) % 3], &tri.edge[k]);
					out->push_back(cross);
				}

				if (side[1] < 0)
					out->push_back((*in)[i]);
			}

			side[0] = side[1];
		}

		if (out->size() < 3)
			return out->size();

		if (!WisEqual(*out->begin(), *out->rbegin(), g_EPSILON))
			out->push_back(*out->begin());

		std::vector<WVector>* temp = in;
		in = out;
		out = temp;

		if (k < 2)
			out->clear();

		n = in->size();
	}

	return poly.size();
}

void WPolySoup::BuildNewHolecupMesh(std::vector<sHoleMesh>& meshes,
	sHoleTri& tri, std::vector<WVector>& poly)
{
	std::vector<WVector> lines;
	int n = poly.size();
	lines.reserve(n);

	for (int i = 0; i < n; i++)
	{
		int j = (i + 1 < n ? i + 1 : i + 2) % n;

		WVector& a = poly[i];
		WVector line(a.z - poly[j].z, poly[j].x - a.x, 0.0f);
		line.Normalize();
		float dx = line.x * a.x;
		float dz = line.y * a.z;
		line.z = (dx + dz) * -1.0f;
		lines.push_back(line);
	}

	bool done[3];
	int first = -1;
	bool open = false;
	int prevCount = 0;
	memset(done, 0, sizeof(done));
	int prev[3];

	for (int i = 0; i < n; i++)
	{
		int cur[3];
		int count = 0;

		for (int k = 0; k < 3; k++)
		{
			float d = tri.pos[k]->x * lines[i].x + tri.pos[k]->z * lines[i].y +
				lines[i].z;
			if (d > 0.001f)
				cur[count++] = k;
		}

		if (count == 2)
		{
			if (cur[0] == 0 && cur[1] == 2)
			{
				cur[1] = 0;
				cur[0] = 2;
			}

			if (i == 0)
				first = cur[0];
		}

		switch (count)
		{
		case 0:
			open = true;
			break;
		case 1:
			if (prevCount == 1)
			{
				if (cur[0] != prev[0] && !done[prev[0]])
				{
					done[prev[0]] = true;
					AddNewHolecupTri(meshes, tri, tri.pos[prev[0]],
						tri.pos[cur[0]], &poly[i], tri.index[prev[0]],
						tri.index[cur[0]], -1);
				}
			}
			else if (prevCount == 2)
			{
				if (cur[0] != prev[1] && !done[prev[1]])
				{
					done[prev[1]] = true;
					AddNewHolecupTri(meshes, tri, tri.pos[prev[1]],
						tri.pos[cur[0]], &poly[i], tri.index[prev[1]],
						tri.index[cur[0]], -1);
				}
			}

			if (i < n - 1)
				AddNewHolecupTri(meshes, tri, tri.pos[cur[0]], &poly[i + 1],
					&poly[i], tri.index[cur[0]], -1, -1);

			open = true;
			break;
		case 2:
			if (prevCount == 1)
			{
				int p = prev[0];

				if (cur[0] != p && !done[p])
				{
					done[p] = true;
					open = true;
					AddNewHolecupTri(meshes, tri, tri.pos[p], tri.pos[cur[0]],
						&poly[i], tri.index[p], tri.index[cur[0]], -1);
				}
			}

			if (i < n - 1)
			{
				if (!done[cur[0]])
				{
					done[cur[0]] = true;
					AddNewHolecupTri(meshes, tri, tri.pos[cur[0]],
						tri.pos[cur[1]], &poly[i], tri.index[cur[0]],
						tri.index[cur[1]], -1);
					AddNewHolecupTri(meshes, tri, tri.pos[cur[1]], &poly[i + 1],
						&poly[i], tri.index[cur[1]], -1, -1);
				}
				else if (cur[0] == first && open)
				{
					AddNewHolecupTri(meshes, tri, tri.pos[cur[0]], &poly[i + 1],
						&poly[i], tri.index[cur[0]], -1, -1);
				}
				else
				{
					AddNewHolecupTri(meshes, tri, tri.pos[cur[1]], &poly[i + 1],
						&poly[i], tri.index[cur[1]], -1, -1);
				}
			}
			break;
		}

		memcpy(prev, cur, sizeof(int) * count);
		prevCount = count;
	}
}

void WPolySoup::AddNewHolecupTri(std::vector<sHoleMesh>& meshes, sHoleTri& tri,
	WVector* a, WVector* b, WVector* c, int ia, int ib, int ic)
{
	WVector* pos[3] = { a, b, c };
	int index[3] = { ia, ib, ic };
	sHoleMesh* target = NULL;
	std::vector<sHoleMesh>::iterator it;
	for (it = meshes.begin(); it != meshes.end(); it++)
	{
		if ((*it).mesh == tri.mesh)
		{
			target = &*it;
			break;
		}
	}
	if (it == meshes.end())
	{
		sHoleMesh mesh;
		mesh.mesh = tri.mesh;
		meshes.push_back(mesh);
		it = meshes.end();
		it--;
		target = &*it;
	}
	std::vector<sHoleVtx>& vtxs = target->vtx;
	std::vector<int>& indices = target->index;
	for (int i = 0; i < 3; i++)
	{
		if (index[i] == -1)
		{
			sHoleVtx vtx;
			vtx.pos = *pos[i];
			GenNewHolecupVtx(&vtx, &tri);
			int n = (int)vtxs.size();
			int j;
			for (j = 0; j < n; j++)
			{
				sHoleVtx& v = vtxs[j];
				if (WisEqual(v.pos.x, vtx.pos.x, g_EPSILON) &&
					WisEqual(v.pos.y, vtx.pos.y, g_EPSILON) &&
					WisEqual(v.pos.z, vtx.pos.z, g_EPSILON) &&
					WisEqual(v.normal.x, vtx.normal.x, g_EPSILON) &&
					WisEqual(v.normal.y, vtx.normal.y, g_EPSILON) &&
					WisEqual(v.normal.z, vtx.normal.z, g_EPSILON) &&
					WisEqual(v.uv[0], vtx.uv[0], g_EPSILON) &&
					WisEqual(v.uv[1], vtx.uv[1], g_EPSILON))
				{
					if (tri.mesh->uvBackup == NULL)
						break;
					if (WisEqual(v.buv[0], vtx.buv[0], g_EPSILON) &&
						WisEqual(v.buv[0], vtx.buv[0], g_EPSILON))
						break;
				}
			}
			if (j == n)
			{
				indices.push_back(tri.mesh->vtxNum + n);
				vtxs.push_back(vtx);
			}
			else
			{
				indices.push_back(tri.mesh->vtxNum + j);
			}
		}
		else
		{
			indices.push_back(index[i]);
		}
	}
}

void WPolySoup::GenNewHolecupVtx(sHoleVtx* vtx, sHoleTri* tri)
{
	float u, v;
	BarycentricXZ(tri->pos[0], &vtx->pos, tri->pos[1], tri->pos[2], &u, &v);
	vtx->pos.y =
		Interpolate(tri->pos[0]->y, tri->pos[1]->y, tri->pos[2]->y, u, v);
	if (tri->mesh->normList)
	{
		vtx->normal.x = Interpolate(tri->normal[0]->x, tri->normal[1]->x,
			tri->normal[2]->x, u, v);
		vtx->normal.y = Interpolate(tri->normal[0]->y, tri->normal[1]->y,
			tri->normal[2]->y, u, v);
		vtx->normal.z = Interpolate(tri->normal[0]->z, tri->normal[1]->z,
			tri->normal[2]->z, u, v);
	}
	const unsigned char* c0 = (const unsigned char*)&tri->color[0];
	const unsigned char* c1 = (const unsigned char*)&tri->color[1];
	const unsigned char* c2 = (const unsigned char*)&tri->color[2];
	float a0 = c0[3];
	float r0 = c0[2];
	float g0 = c0[1];
	float b0 = *(const ulong*)c0 & 0xff;
	unsigned char a = (unsigned char)Interpolate(a0, c1[3], c2[3], u, v);
	unsigned char r = (unsigned char)Interpolate(r0, c1[2], c2[2], u, v);
	unsigned char g = (unsigned char)Interpolate(g0, c1[1], c2[1], u, v);
	unsigned char b = (unsigned char)Interpolate(b0, *(const ulong*)c1 & 0xff,
		*(const ulong*)c2 & 0xff, u, v);
	vtx->color = (a << 24) | (r << 16) | (g << 8) | b;
	vtx->uv[0] = Interpolate(tri->uv[0][0], tri->uv[1][0], tri->uv[2][0], u, v);
	vtx->uv[1] = Interpolate(tri->uv[0][1], tri->uv[1][1], tri->uv[2][1], u, v);
	if (tri->mesh->uvBackup)
	{
		vtx->buv[0] =
			Interpolate(tri->buv[0][0], tri->buv[1][0], tri->buv[2][0], u, v);
		vtx->buv[1] =
			Interpolate(tri->buv[0][1], tri->buv[1][1], tri->buv[2][1], u, v);
	}
}

void WPolySoup::AddHolecup(std::vector<sHoleMesh>& meshes,
	std::vector<sCupVtx>& cup, std::vector<sHoleTri>& tris)
{
	DigHolecup(cup, tris);

	std::vector<sHoleMesh>::iterator it;

	for (it = meshes.begin(); it != meshes.end(); it++)
	{
		ApplyHoleMesh(*it, tris);
	}
}

void WPolySoup::ApplyHoleMesh(sHoleMesh& hole, std::vector<sHoleTri>& tris)
{
	w_mesh* mesh;
	int vtxNum;
	int indexNum;
	int removed;
	WVector* vecList;
	WVector* normList;
	ulong* colorList;
	ulong* originalColorList;
	float (*uvData)[2];
	float (*uvBackup)[2];
	ushort* indexList;
	std::vector<sHoleTri>::iterator tri;

	mesh = hole.mesh;

	vtxNum = mesh->vtxNum + (int)hole.vtx.size();
	vecList = new WVector[vtxNum];

	if (mesh->normList)
		normList = new WVector[vtxNum];
	else
		normList = NULL;

	colorList = new ulong[vtxNum];
	originalColorList = new ulong[vtxNum];
	uvData = new float[vtxNum][2];
	uvBackup = mesh->uvBackup ? new float[vtxNum][2] : NULL;

	removed = 0;
	for (tri = tris.begin(); tri != tris.end(); tri++)
	{
		if ((*tri).mesh == mesh)
			removed++;
	}

	indexNum = (int)hole.index.size() - removed * 3 + mesh->indexNum;
	indexList = new ushort[indexNum];

	memcpy(vecList, mesh->vecList, sizeof(WVector) * mesh->vtxNum);

	if (normList)
		memcpy(normList, mesh->normList, sizeof(WVector) * mesh->vtxNum);

	memcpy(colorList, mesh->vtxColorList, sizeof(ulong) * mesh->vtxNum);
	memcpy(originalColorList, mesh->originalVtxColorList,
		sizeof(ulong) * mesh->vtxNum);
	memcpy(uvData, mesh->uvData, sizeof(float) * 2 * mesh->vtxNum);
	if (mesh->uvBackup)
		memcpy(uvBackup, mesh->uvBackup, sizeof(float) * 2 * mesh->vtxNum);

	int v = mesh->vtxNum;
	if (v < vtxNum)
		do
		{
			sHoleVtx& vtx = hole.vtx[v - mesh->vtxNum];

			vecList[v] = vtx.pos;

			if (normList)
				normList[v] = vtx.normal;

			colorList[v] = vtx.color;
			originalColorList[v] = vtx.color;
			memcpy(uvData[v], vtx.uv, sizeof(float) * 2);

			if (mesh->uvBackup)
				memcpy(uvBackup[v], vtx.buv, sizeof(float) * 2);

			v++;
		} while (v < vtxNum);

	for (tri = tris.begin(); tri != tris.end(); tri++)
	{
		if ((*tri).mesh == mesh)
		{
			int prev;
			int written;

			if ((*tri).offset > 0)
				memcpy(indexList, mesh->indexList,
					sizeof(ushort) * (*tri).offset);

			written = prev = (*tri).offset;

			for (tri++; tri != tris.end(); tri++)
			{
				if ((*tri).mesh != mesh)
					break;

				int gap = (*tri).offset - prev;
				if (gap > 3)
				{
					memcpy(&indexList[written], &mesh->indexList[prev + 3],
						sizeof(ushort) * (gap - 3));
					written += (*tri).offset - prev - 3;
				}

				prev = (*tri).offset;
			}

			if (mesh->indexNum - prev > 3)
			{
				memcpy(&indexList[written], &mesh->indexList[prev + 3],
					sizeof(ushort) * (mesh->indexNum - prev - 3));
				written += mesh->indexNum - prev - 3;
			}

			for (int j = written, k = 0; j < indexNum; j++, k++)
				indexList[j] = (ushort)hole.index[k];

			break;
		}
	}

	delete[] mesh->vecList;
	delete[] mesh->normList;
	delete[] mesh->vtxColorList;
	delete[] mesh->originalVtxColorList;
	delete[] mesh->uvData;
	if (mesh->uvBackup)
		delete[] mesh->uvBackup;
	delete[] mesh->indexList;

	mesh->vtxNum = vtxNum;

	mesh->vecList = vecList;
	mesh->normList = normList;
	mesh->vtxColorList = colorList;
	mesh->originalVtxColorList = originalColorList;
	mesh->uvData = uvData;
	mesh->uvBackup = uvBackup;
	mesh->indexNum = indexNum;
	mesh->indexList = indexList;
}

void WPolySoup::DigHolecup(std::vector<sCupVtx>& cup,
	std::vector<sHoleTri>& tris)
{
	FindNewHolecupVtxs(cup, tris);

	WVector up(0.0f, 1.0f, 0.0f);
	WMatrix inv;
	WVector center;
	WVector light;
	WVector dir;
	float radius;
	float bottom;
	w_mesh* mesh = m_baseModel->pet->GetRootBone()->GetMesh();

	if (mesh == NULL)
		return;

	for (; mesh->next; mesh = mesh->next)
		;

	w_mesh* hole = new w_mesh;
	memset(hole, 0, sizeof(w_mesh));
	mesh->next = hole;

	if (m_holecupTex == 0)
		m_holecupTex = g_resrcmng->LoadTexture("holecup.dds", 0, 0, 0);

	hole->texHandle = m_holecupTex;
	hole->drawFlag = (m_holecupTex & 0x7ff) | 0x8000000;
	hole->diffuse = 0xffffff;
	hole->alpha = 1.0f;

	int n = (int)cup.size();

	hole->vtxNum = n * 2 + 4;
	hole->indexNum = n * 6;

	hole->vecList = new WVector[hole->vtxNum];
	hole->normList = new WVector[hole->vtxNum];
	hole->vtxColorList = new ulong[hole->vtxNum];
	hole->originalVtxColorList = new ulong[hole->vtxNum];
	hole->uvData = new float[hole->vtxNum][2];
	hole->uvBackup = NULL;
	hole->indexList = new ushort[hole->indexNum];

	radius = GolfBall().GetRadius() * 3.0f;
	if (IsLocalContent(S3_ROOKIE_CHANNEL) &&
		WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType != 0xb &&
		WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType != 0xc)
	{
		if (WSingleton<CSharedDoc>::Instance()->m_curChannel.Type & 0x800)
			radius = GolfBall().GetRadius() * 6.0f;
	}

	inv = ~m_baseModel->pet->GetRootBone()->GetMatrix();
	center =
		WVector(
			WSingleton<CSharedDoc>::Instance()->m_pGolfDoc->GetHoleData().pin) *
		inv;
	bottom = cup[0].pos.y - 1.0f;
	light = RotVec(g_lightset.nearone, inv) * -1.0f;
	light.Normalize();

	int i;
	for (i = 0; i < n; i++)
	{
		sCupVtx& vtx = cup[i];

		dir = WVector(center.x - vtx.pos.x, 0.0f, center.z - vtx.pos.z);
		dir.Normalize();

		float angle;

		hole->vecList[i * 2] = hole->vecList[i * 2 + 1] = vtx.pos;
		hole->vecList[i * 2 + 1].y = bottom;

		hole->normList[i * 2] = hole->normList[i * 2 + 1] = dir;

		if (i == n - 1)
			angle = g_PI * 2.0f;
		else
		{
			float c = -dir.x < -1.0f ? -1.0f : (-dir.x > 1.0f ? 1.0f : -dir.x);
			angle = (float)acos(c);

			if (dir.z < 0.0f)
				angle = g_PI * 2.0f - angle;
		}

		float u = angle * 0.47746482f;

		hole->uvData[i * 2][0] = u;
		hole->uvData[i * 2][1] = 0.015625f;
		hole->uvData[i * 2 + 1][0] = u;
		hole->uvData[i * 2 + 1][1] = 0.90625f;

		float lit = light * dir * 255.0f;
		ulong color;
		if (lit <= 0.0f)
			color = g_lightset.ambient;
		else
			color = AddDiffuse(g_lightset.ambient, g_lightset.diffuse,
				(unsigned char)(int)lit);

		color |= 0xff000000;

		hole->originalVtxColorList[i * 2 + 1] = color;
		hole->originalVtxColorList[i * 2] = color;
		hole->vtxColorList[i * 2 + 1] = color;
		hole->vtxColorList[i * 2] = color;
	}

	int base = n * 2;
	ulong ambient = g_lightset.ambient | 0xff000000;
	WVector offset[4] = {
		WVector(-1.0f, 0.0f, 1.0f),
		WVector(1.0f, 0.0f, 1.0f),
		WVector(-1.0f, 0.0f, -1.0f),
		WVector(1.0f, 0.0f, -1.0f),
	};

	for (int j = 0; j < 4; j++)
	{
		hole->vecList[base + j] = center + offset[j] * radius;
		hole->vecList[base + j].y = bottom;

		hole->normList[base + j] = up;

		hole->uvData[base + j][0] = 0.0f;
		hole->uvData[base + j][1] = 0.96875f;

		hole->originalVtxColorList[base + j] = ambient;
		hole->vtxColorList[base + j] = ambient;
	}

	for (i = 0; i < n - 1; i++)
	{
		hole->indexList[i * 6] = (ushort)(i * 2);
		hole->indexList[i * 6 + 1] = (ushort)(i * 2 + 2);
		hole->indexList[i * 6 + 2] = (ushort)(i * 2 + 1);
		hole->indexList[i * 6 + 3] = (ushort)(i * 2 + 1);
		hole->indexList[i * 6 + 4] = (ushort)(i * 2 + 2);
		hole->indexList[i * 6 + 5] = (ushort)(i * 2 + 3);
	}

	hole->indexList[i * 6] = (ushort)n * 2;
	hole->indexList[i * 6 + 1] = (ushort)(n * 2 + 1);
	hole->indexList[i * 6 + 2] = (ushort)(n * 2 + 2);
	hole->indexList[i * 6 + 3] = (ushort)(n * 2 + 2);
	hole->indexList[i * 6 + 4] = (ushort)(n * 2 + 1);
	hole->indexList[i * 6 + 5] = (ushort)(n * 2 + 3);

	WBone::CalcMeshAABB(*m_baseModel->pet->GetRootBone(), *hole);
}

void WPolySoup::FindNewHolecupVtxs(std::vector<sCupVtx>& cup,
	std::vector<sHoleTri>& tris)
{
	std::vector<sHoleTri>::iterator tri;
	std::vector<sCupVtx>::iterator it;
	std::vector<sCupVtx>::iterator prev;
	bool inside[2];
	sCupVtx newVtx;
	sHoleVtx vtx;

	for (tri = tris.begin(); tri != tris.end(); tri++)
	{
		prev = cup.begin();
		inside[0] = IsInTri((*prev).pos, (*tri).pos);
		it = prev;
		for (it++; it != cup.end(); it++)
		{
			inside[1] = IsInTri((*it).pos, (*tri).pos);
			if (inside[0] ^ inside[1])
			{
				for (int k = 0; k < 3; k++)
				{
					if (GetIntersection(&newVtx.pos, &(*prev).pos, &(*it).pos,
							(*tri).pos[k], (*tri).pos[(k + 1) % 3],
							(WVector*)&(*tri).edge[k]))
					{
						newVtx.mesh = (*tri).mesh;
						newVtx.index = (*tri).offset;
						it = cup.insert(it, newVtx);
						it++;
						break;
					}
				}
			}
			inside[0] = inside[1];
			prev = it;
		}
	}
	for (it = cup.begin(); it != cup.end(); it++)
	{
		vtx.pos = (*it).pos;
		for (tri = tris.begin(); tri != tris.end(); tri++)
		{
			if ((*tri).mesh == (*it).mesh && (*tri).offset == (*it).index)
			{
				GenNewHolecupVtx(&vtx, &*tri);
				(*it).pos.y = vtx.pos.y;
				break;
			}
		}
	}
}

WPolySoup::CStaticPetGrp::CStaticPetGrp()
	: m_num(0), m_grp(NULL)
{
}

WPolySoup::CStaticPetGrp::~CStaticPetGrp()
{
	Clear();
}

void WPolySoup::CStaticPetGrp::Build(const std::vector<sModel*> models, int num)
{
	Clear();

	int groups = 0;
	int i = 0;
	int j;
	int k;
	int index;
	int run;
	const char* last = "\0";

	m_num = 0;

	while (i < num)
	{
		if (models[i]->pet && models[i]->type == 0 &&
			strcmpi(models[i]->name, last))
		{
			last = models[i]->name;
			groups++;
		}

		i++;
	}

	if (groups == 0)
		return;

	m_grp = new WxStaticPuppetGrp*[groups];
	memset(m_grp, 0, sizeof(WxStaticPuppetGrp*) * groups);

	last = "\0";
	run = 1;
	index = 0;
	i = 0;

	while (index < groups)
	{
		sModel* const* cur = &models[i];

		if ((*cur)->pet && (*cur)->type == 0 && strcmpi((*cur)->name, last))
		{
			last = (*cur)->name;
			j = i + 1;
			sModel* const* next = cur;

			while (j < num && (*cur)->type == 0 &&
				strcmpi(next[1]->name, last) == 0)
			{
				j++;
				run++;
				next++;
			}

			WPuppet** pets = new WPuppet*[run];

			for (k = 0; k < run; k++)
				pets[k] = models[i + k]->pet;

			m_grp[index] =
				g_resrcmng->xGetStaticPuppetGrp(run, (*cur)->name, pets);

			i += run;
			index++;
			delete[] pets;
		}
		else
			i++;

		run = 1;
	}

	WSingleton<CSceneManager>::Instance()->RegisterElement(OBJ_STATIC, NULL,
		this, "StaticPetGrp", 2, NULL);

	m_num = groups;
}

void WPolySoup::CStaticPetGrp::Clear()
{
	for (int i = 0; i < m_num; i++)
	{
		g_resrcmng->Release(m_grp[i]);
	}

	if (m_grp)
	{
		delete[] m_grp;
		m_grp = NULL;
	}
	m_num = 0;
}

void WPolySoup::CStaticPetGrp::Display()
{
	g_view->xSetLight(0, g_lightset);

	for (int i = 0; i < m_num; i++)
	{
		if (m_grp[i])
		{
			m_grp[i]->xUpdateCullFlag(g_view);
			m_grp[i]->xRender(g_view);
		}
	}
}

void WPolySoup::UpdateBright()
{
	if (m_baseModel->pet)
		m_baseModel->pet->UpdateLightSource(&g_lightset, false, NULL);

	for (int i = 0; i < m_modelNum; i++)
	{
		if (m_model[i]->pet)
			m_model[i]->pet->UpdateLightSource(&g_lightset, false, NULL);
	}
}

void WPolySoup::ChangePointType(unsigned char from, unsigned char to)
{
	sPoint* point = m_point;
	unsigned int num = m_header.pointNum;
	for (unsigned int i = 0; i < num; i++, point++)
	{
		if (point == NULL)
			continue;
		if (point->type == from)
		{
			point->type = to;
			return;
		}
	}
}

const std::vector<WPolySoup::sTriangle*>* WPolySoup::GetTriArray(
	int texHandle) const
{
	std::map<int, std::vector<sTriangle*> >::const_iterator it =
		m_triList.find(texHandle);
	if (it == m_triList.end())
		return NULL;

	return &(*it).second;
}
