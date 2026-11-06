#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>
#include "nodecontainer.h"
#include "scenemanager.h"

class CGolfDoc;
class CFxSpray;
class CFxSequence;
class WBone;
class WPuppet;
class WxStaticPuppetGrp;
class cFile;
struct w_mesh;
struct w_bound_box;
struct sProperty;

struct sSoundBox
{
	char name[32];
	float time;
	float interval;
	float distance;
	Wobb obb;
};

class WPolySoup : public WResource
{
	friend class CSceneManager;
	friend class CSky;
	friend class CGolfBg;
	friend class CAmbientSound;
	friend class CNPC;
	friend class CQuadTree;

public:
	WPolySoup(CGolfDoc* pDoc);
	virtual ~WPolySoup();

	bool Load(const char* filename, bool bOption);
	bool LoadMapCheckData(unsigned char hole);
	void EndLoad(bool bPlaceHoleCup);
	void Clear();

	void MakeDataStructure();
	void MakeStatistics();
	void UpdateBright();
	void UpdateBgFxWind();
	void EnableShadowmap(bool bEnable);
	void SetRenderMode(int mode);
	void SetCullDistance(float dist) { m_cullDistance = dist; }
	bool IsOutOfBound(const WVector& pos);
	void ChangeFloorTexture(const char* from, const char* to);
	void ChangeObjectTexture(const char* from, const char* to, int index);
	Waabb GetExtraHoleStartArea() { return m_extraHoleStartArea; }
	void ChangePointType(unsigned char from, unsigned char to);
	bool IsResisteredBgSequence(const std::string& name);
	int GetPropertyIndex(const char* name);
	const sProperty& GetProperty(int index);

	struct sHeader
	{
		char id[4];
		unsigned long version;
		int modelNum;
		int modelNum1;
		int modelNum2;
		int cameraNum;
		int pointNum;
		int areaNum;
		int textureNum;
		int nodeNum;
	};

	struct sTriangle
	{
		WVector v[3];
		WPlane plane;
		bool bDoubleSide;
		int texHandle;
		int propIndex;
		bool bEdge;
		WBone* bone;

		sTriangle()
			: texHandle(0), propIndex(0), bone(NULL)
		{
		}
	};

	struct sCamera
	{
		char name[32];
		WMatrix mat;
		float fov;
		int reserved;
	};

	struct sTexture
	{
		char name[32];
		int texHandle;
		int propIndex;

		sTexture()
			: texHandle(0), propIndex(0)
		{
		}
	};

	struct sPoint
	{
		unsigned char type;
		char name[32];
		WVector pos;
		std::string option;
	};

	struct sArea
	{
		unsigned char type;
		char name[64];
		Waabb aabb;
	};

	struct sBgFx
	{
		sArea* area;
		CFxSpray* spray;
		CFxSequence* sequence;
		WMatrix mat;
		WVector offset;
	};

	struct sQuad
	{
		short meshNum;
		int* meshIndex;
		unsigned char objNum;
		short* objIndex;

		sQuad()
		{
			meshNum = 0;
			meshIndex = NULL;

			objNum = 0;
			objIndex = NULL;
		}

		~sQuad()
		{
			meshNum = 0;
			if (meshIndex)
			{
				delete[] meshIndex;
				meshIndex = NULL;
			}

			objNum = 0;
			if (objIndex)
			{
				delete[] objIndex;
				objIndex = NULL;
			}
		}
	};

	struct sModel
	{
		unsigned char type;
		unsigned char flag;
		unsigned char noCollision;
		unsigned char alpha;
		float windFactor;
		float scale;
		WPuppet* pet;
		WMatrix mat;
		char name[32];
		Waabb aabb;
		Waabb bbox;

		sModel()
		{
			quad = NULL;
			tri = NULL;
			pet = NULL;
			windFactor = 1.0f;
			noCollision = 0;
			alpha = 0;
			hitCount = 0;
			name[0] = 0;
			option[0] = 0;
			id = 0;
		}

		Wobb obb;
		WVector center;
		float radius;
		WVector pos;
		sTriangle* tri;
		sQuad* quad;
		bool bSingleQuad;
		unsigned char hitCount;
		char option[32];
		int id;
	};

	class CStaticPetGrp : public CRenderFuncPtr
	{
	public:
		CStaticPetGrp();
		~CStaticPetGrp();

		void Build(const std::vector<sModel*> models, int num);
		void Clear();
		virtual void Display();

	private:
		int m_num;
		WxStaticPuppetGrp** m_grp;
	};

	sCamera* FindCamera(const char* name);
	sCamera* GetCameraByName(const char* name);
	sPoint* FindPoint(const char* name);
	sArea* FindArea(const char* name);
	sArea* GetAreaByPos(const WVector& pos);
	sModel* FindModel(const char* name);
	CNodeContainer* FindNode(const char* name);
	w_bound_box* GetObjBoundBox(int index, const char* name);
	int GetObjAlpha(int index);
	float GetObjWindFactor(int index);
	WVector GetCameraPos(const WVector& from, const WVector& to);
	const std::vector<sTriangle*>* GetTriArray(int texHandle) const;

	void CreateDataModel(sModel* model);
	void ChangeDataStructure(sModel* model);
	bool DeleteDataStructure(sModel* model);
	void SetAdditionModel(const char* name, const WVector& pos);

protected:
	void ProcessPostLoadPet(sModel* model);
	void ApplyFx(sModel* model, WBone* bone, const char* fx);
	float GetCameraRadius(WVector eye, WVector target, WVector up);
	bool IsPointInPoly(const std::vector<WVector>& poly, const WVector& pos);
	bool IsClockWise(const std::vector<WVector>& poly, const WVector& pos);
	WVector PosInWorld(const std::vector<WVector>& poly, const WVector& a,
		const WVector& b);

private:
	bool HaveCameraArea() { return m_bCameraArea; }

public:
	int GetModelNum() { return m_modelNum; }
	int GetAreaNum() { return m_header.areaNum; }
	int GetRmrModelNum() { return m_rmrModelNum; }

	sModel* GetBaseModel() { return m_baseModel; }

	sArea* GetArea(int index) { return &m_area[index]; }

private:
	typedef std::list<sSoundBox>::iterator SoundBoxIter;
	std::vector<CNodeContainer> m_node;
	char m_mapName[64];
	sHeader m_header;
	int m_modelNum;
	int m_rmrModelNum;
	sTexture* m_texture;
	int m_oldTextureNum;
	sTexture* m_oldTexture;
	sCamera* m_camera;
	sArea* m_area;
	sPoint* m_point;
	std::vector<sModel*> m_model;
	sModel* m_baseModel;
	std::map<int, int> m_texIndex;
	float m_fadeNear;
	float m_fadeFar;
	Waabb m_extraHoleStartArea;
	CGolfDoc* m_pDoc;
	bool m_bUnused;
	bool m_bBuildTnL;
	CStaticPetGrp m_staticPetGrp;
	sBgFx m_bgFx[256];
	int m_bgFxNum;
	bool m_bCameraArea;
	std::list<LightSet> m_lightList;
	std::vector<unsigned long> m_npcList;
	std::vector<int> m_unusedList;

	std::list<sSoundBox> m_soundBox;
	unsigned long m_reserved;
	float m_cullDistance;
	int m_smapMeshNum;
	int* m_smapTexHandle;
	w_mesh* m_smapMesh;
	bool m_bSmapLoadFailed;
	int m_holecupTex;
	struct sCupVtx
	{
		WVector pos;
		w_mesh* mesh;
		int index;
	};

	struct sHoleVtx
	{
		WVector pos;
		WVector normal;
		unsigned long color;
		float uv[2];
		float buv[2];
	};

	struct sHoleTri
	{
		w_mesh* mesh;
		int offset;
		int index[3];
		WVector* pos[3];
		WVector* normal[3];
		float uv[3][2];
		float buv[3][2];
		unsigned long color[3];
		WVector edge[3];
	};

	struct sHoleMesh
	{
		w_mesh* mesh;
		std::vector<sHoleVtx> vtx;
		std::vector<int> index;
	};

	struct SmInFragment
	{
		unsigned char mesh;
		unsigned short tri;
		unsigned short vtx[3];
		unsigned short uv[3];
	};

	struct SmInFragList
	{
		int num;
		SmInFragment* frag;

		SmInFragList()
		{
			num = 0;
			frag = NULL;
		}
		~SmInFragList()
		{
			if (frag)
			{
				delete[] frag;
				frag = NULL;
			}
		}
	};

	struct SmCase
	{
		int num;
		SmInFragList* list;

		SmCase()
		{
			num = 0;
			list = NULL;
		}
		~SmCase()
		{
			if (list)
			{
				delete[] list;
				list = NULL;
			}
		}
	};

	struct SmOutFrag
	{
		unsigned short tri;
		unsigned short vtx[3];
	};

	typedef std::vector<sCupVtx>::iterator CupVtxIter;
	typedef std::vector<sHoleTri>::iterator HoleTriIter;
	typedef std::vector<sHoleMesh>::iterator HoleMeshIter;
	std::map<int, std::vector<sTriangle*> > m_triList;

private:
	static int __cdecl CompareModel(const void* a, const void* b);
	static int __cdecl smCompareOutFrag(const void* a, const void* b);

	void ReloadBasePet();
	void xBuildTnLBuffers();
	void BackupTextures();
	void ClearOldTextures();
	unsigned char OverlapCamera(int index);

	bool LoadShadowmapInfo();
	void smLoadSmapMeshes(SmCase* smCase, cFile* file, w_mesh** meshList,
		WVector* vtxList, float (*uvList)[2]);
	w_mesh* smAddSmapPack(SmInFragList* fragList, cFile* file, int caseIndex,
		int listIndex, w_mesh** meshList, WVector* vtxList, float (*uvList)[2]);
	void smLoadOutFrags(cFile* file, int num, w_mesh** meshList,
		WVector* vtxList, SmCase* smCase);
	void smBuildOutFragMeshes(w_mesh* mesh, SmOutFrag* frag, int num,
		WVector* vtxList);

	void PlaceHoleCup();
	void AddHolecup(std::vector<sHoleMesh>& meshes, std::vector<sCupVtx>& cup,
		std::vector<sHoleTri>& tris);
	void GatherHolecupTris(std::vector<sHoleTri>& tris,
		std::vector<sCupVtx>& cup, WVector& center);
	void GenHolecupTri(sHoleTri* tri, w_mesh* mesh, int offset);
	void FindNewHolecupVtxs(std::vector<sCupVtx>& cup,
		std::vector<sHoleTri>& tris);
	void GenNewHolecupVtx(sHoleVtx* vtx, sHoleTri* tri);
	void DigHolecup(std::vector<sCupVtx>& cup, std::vector<sHoleTri>& tris);
	void CreateNewHolecupMeshes(std::vector<sHoleMesh>& meshes,
		std::vector<sCupVtx>& cup, std::vector<sHoleTri>& tris);
	int ClipHolecup(std::vector<WVector>& poly, sHoleTri& tri,
		std::vector<sCupVtx>& cup);
	void BuildNewHolecupMesh(std::vector<sHoleMesh>& meshes, sHoleTri& tri,
		std::vector<WVector>& poly);
	void AddNewHolecupTri(std::vector<sHoleMesh>& meshes, sHoleTri& tri,
		WVector* a, WVector* b, WVector* c, int ia, int ib, int ic);
	void ApplyHoleMesh(sHoleMesh& mesh, std::vector<sHoleTri>& tris);
	bool IsInPoly(WVector& pos, std::vector<sCupVtx>& cup);
	bool IsInTri(WVector& pos, WVector** const tri);
	bool AreCrossed(WVector* a1, WVector* a2, WVector* b1, WVector* b2);
	bool GetIntersection(WVector* a1, WVector* a2, WVector* b1, WVector* b2,
		WVector* out, WVector* dummy);
};
