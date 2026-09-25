#pragma once
#include <map>
#include <vector>
#include <stdio.h>
#include "wmath.h"
#include "wscene.h"
#include "wvideo.h"

class WScene;
class WPuppet;
class WBoneSet;
class WBlockModel;
class cFile;

struct w_mesh;
struct w_faces;
struct w_keyframe;
struct w_common_bone_data;
struct w_pet_vertex;
struct w_pet_tri_point;
struct w_pet_texture_info;

struct w_keyframe_pos
{
	float time;
	WVector pos;
};

struct w_keyframe_rot
{
	float time;
	WQuat rot;
};

class WBone
{
public:
	enum RenderFlag
	{
		NOCLIP = 0x1,
		NOUPDATEMAT = 0x2,
	};

	enum w_bone_flag
	{
		HIDEBONE = 0x1,
		SELFILLUM = 0x2,
		HASRIGIDVTX = 0x4,
		VISIBLE = 0x8,
		CLIPPING = 0x10,
		CLONED_MESH = 0x20,
		CLONED_ANI = 0x40,
		ROTATEBONE = 0x200,
		MOVEBONE = 0x400,
		TRANSFORM = 0x1000,
		PHYSICSMODEL = 0x2000,
		COLORPERVERTEX = 0x4000,
		CHECK_SHADOW = 0x8000,
		MESHBONE = 0x10000,
		MIRRORED = 0x20000,
		INVISIBLE = 0x40000,
		SCALEBONE = 0x80000,
	};

	struct w_bone_physic_link
	{
		struct w_bone_physic_link_element
		{
			int offset;
			float length;
		};

		int vtxnum;
		WVector* pos;
		WVector* vel;
		WVector** list;
		w_bone_physic_link_element* link;
		int* linkoffset;
	};
	class w_normalmerge
	{
	public:
		w_normalmerge(WPuppet& puppet);

	private:
		struct N
		{
			N(const WVector& n)
				: m_n(n)
			{
			}
			WVector m_n;
			std::vector<int, std::allocator<int> > m_idxList;
		};
		struct P
		{
			P(const WVector& p)
				: m_p(p)
			{
			}
			WVector m_p;
			std::map<w_mesh*, std::vector<WBone::w_normalmerge::N> > m_meshList;
		};

	public:
		void AddVertex(w_mesh* mesh, int index);

	private:
		void MergeNormal();
		void NormalizeMergedNormals();
		void ApplyToBlendedNormalList();
		static P* Find(std::vector<P>& list, const WVector& v);
		static N* Find(std::vector<N>& list, const WVector& v);
		static void SetMyNormalsEqualToFirstNormal(w_mesh*,
			const std::vector<int>&);
		std::map<float, std::vector<WBone::w_normalmerge::P> > m_wlist;
	};

	void SetLocalMatrix(const WQuat& quat, const WVector& pos)
	{
		m_localMat = quat;
		m_localMat.pivot = pos;
	}

	bool IsVisible() { return m_flag.GetFlag(VISIBLE); }

	bool IsHidden() { return m_flag.GetFlag(HIDEBONE); }

	WQuat& GetBasicRot() { return m_basicQuat; }

private:
	WTVertex* GetVtxBuff() { return m_vtxList; }

	WTVertex* GetVtxBuff(int index) { return m_vtxList + index; }

public:
	void EnableAlphaBlend(void);
	void RestoreVtxColor(void);
	void ResetLight(void);
	void GetBoneInfo(int* totals);
	void SetInvisible(bool recursive, bool invisible);
	void ResetMyNormalList(void);

protected:
	void CalcLight_Directional(LightSet* light, int flag, WScene* scene);

	void CalcLight_Directional_Per_Bone(LightSet* light, int flag,
		WScene* scene);

	void CalcLight_Point(LightSet* light, int flag, WScene* scene);

	void CalcLight_Diffuse_Equals_Ambient(LightSet* light, int flag);

	void CalcLight_SelfIllum(void);

public:
	void ApplyScale(float scale);
	WQuat GetKeyRot(float time);
	void SetMatrix(const WMatrix& matrix, WView* view);
	void CalcENVCoord(WView* view, w_mesh* mesh);
	void CalcHilightCoord(WView* view, const LightSet& light, w_mesh* mesh);
	int GetOverlapBox(float* box, WBone* bone);
	int SaveBoneName(FILE* file, int type, char* name);
	int SaveAnimationKey(FILE* file, int id, float (*range)[2], int count);

private:
	int OptimizePosKey(float (*range)[2], int count, w_keyframe_pos** out);

	int OptimizeRotKey(float (*range)[2], int count, w_keyframe_rot** out);

public:
	void GatherNormalMergeInfo(w_normalmerge& merge) const;
	static void __fastcall CalcMeshAABB(WBone& bone, w_mesh& mesh);
	w_mesh* GetIndexedmesh(w_pet_vertex* vertexList, w_pet_tri_point* pointList,
		WBone** boneList, w_pet_texture_info** texList, int faceNum,
		int* faceIndexList);
	w_mesh* CopyMesh(w_mesh* source);
	void CopyBone(w_common_bone_data* data);

	void SetRigidVtxFlag(void);
	void AllocVTX(int count);
	int SetID(int id, bool flag, char* name);
	bool ChangeTexture(const char* name, int handle);
	bool ChangeTexture(const char* name, int oldHandle, int handle);
	void OptimizeBoneSet(float (*range)[2], int count, int child);
	void CopyBasicMatrix(WBone* root, bool child);
	void xRender(WView* view, WxBatchState* state, bool setMatrix, int handle,
		int newHandle);

private:
	static int __cdecl SortByTextureHandle(const void* left, const void* right);

	char* GetNameSubBip(void);

	bool CheckSuitableForKeyframe(float start, float end, float (*range)[2],
		int count, bool last);

public:
	void SetMesh(w_faces* faces, int count);
	void SetKeyframe(w_keyframe* frame, float end, float start);
	void ReplaceKeyframe(WBone* bone, float time);
	w_mesh* GetIndexedmesh(w_faces** faces, int count);
	unsigned long* FindVColor(const WVector& pos, const WVector& normal);

private:
	void CountVtxBuff(int count);

	void ClearMesh(w_mesh* mesh);

public:
	WBone();
	~WBone();
	WBone* FindBone(const char* name, unsigned long hashCode);
	WBone* FindBone(int id);
	WBone* FindBoneByTail(const char* tail);
	static bool IsMeshVisible(WBone* bone, w_mesh* mesh);

	void SetAlpha(unsigned char alpha, int recursive);
	void xSetAlpha(unsigned char alpha, int recursive);
	void SetLight(LightSet* light);
	void CalcLight(LightSet* light, bool recursive, int lightMode,
		WScene* scene);
	void CalcLight(WScene* scene, int lightMode);

	void UpdatePhysicsModel(float delta, const WVector& accel);
	void ResetPhysicsModel();
	void SetPhysicsModel(int type);
	void SetRenderMode(int mode);

	void Transform(const WMatrix& mat, WView* view, float matscale, int flag);
	void Transform(WVector& result, const WVector* vec);

	void Render(WView* view, const LightSet& light);
	void RenderHierarchy(WView* view, float axisLen);
	void RenderNormals(WView* view, float axisLen);

	void ResetMeshBone(WBone* root, bool bRecur);
	void ApplyBoneSetList(WBoneSet* set);
	int GetConvexArea(WBlockModel* model);
	void SetLightLine(float value);
	void SetMirrored(bool mirrored);
	void GetName(char** names, int& count);
	char* GetBoneName() { return m_name; }
	void SetVtxColor(unsigned long color);
	float LoadAnimationKey(cFile* file);
	void SetBoneMesh(w_pet_vertex* vertex, int count, w_pet_tri_point* point,
		w_pet_texture_info** texture, WBone** bone, int flag);
	void SetMesh(w_pet_vertex* vertex, int count, w_pet_tri_point* point,
		w_pet_texture_info** texture, WBone** bone, int flag);
	WBone* FindFakeBodyBone(w_pet_vertex* vertex, int count);
	void FixNormal(const WMatrix& matrix);
	void DisableFog();
	bool CompareBasicMatrix(WBone* bone);
	void GetTotalOverlapBox(WVector* min, WVector* max, WMatrix* matrix,
		bool flag);
	void SetBasicMatrix(const WMatrix& matrix);
	void SetNext(WBone* bone, bool recursive);
	void SetParent(WBone* parent);
	void SetName(char* name);
	void ChangeTextureByHandle(int from, int to, bool recursive, bool flag);
	WVector GetDeltaVec(float time);
	bool CheckRigidVtx(WBone* bone);
	void SetBoundBox(Waabb box, bool flag);
	void MergeMesh(WBone* source, WBone* root);
	void ReleaseChild(WBone* child);
	int GetMeshForFaceAnimation(unsigned char group, w_mesh** mesh, int count);
	int GetMeshForFaceAnimation(int group, w_mesh** mesh, int count);
	void CombineVtxColor(bool recursive);
	WBone* MakeClone(WBone* parent);
	void Rotate(float angle, int axis, bool flag);
	void Move(WVector& vector, bool recursive);
	void SetScale(float scale);
	void InsertBoneSetList(float time, WBoneSet* set, int flag);
	void SetSelfIllum(bool enabled, bool recursive);
	void SetSelfIllumColor(unsigned long color, bool recursive);
	void ShowBone(bool recursive);
	void HideBone(bool recursive);
	WVector GetKeyPos(float time);
	void ApplySpecularMap(int handle);
	void xBuildTransfMatPtrList(std::vector<const WMatrix*>& list) const;
	void CountBones(int& count);
	WBone* AttachMeshBone(WBone* bone, int handle, int oldHandle);
	bool CheckMeshBoneAllVertexRigid() const;

	void ReleaseLink()
	{
		m_child = 0;
		m_next = 0;
	}

	WVector Transform(const WVector& vector) { return vector * m_matrix; }
	WSphere GetBoundSphere() { return m_bound_sphere; }
	bool HasAnimation()
	{
		if (!m_keyPos && !m_keyRot)
			return false;
		return true;
	}
	void SetIDNumber(int value) { m_id = value; }
	int GetID() { return m_id; }
	char* GetBoneMotionName() { return m_motion_bone_name; }
	WVector& GetPivot() { return m_basicMat.pivot; }
	WMatrix& GetBasicMatrix() { return m_basicMat; }
	WMatrix& GetMatrix() { return m_matrix; }
	void ToggleSelfIllum()
	{
		unsigned char off = (unsigned char)~((unsigned long)m_flag >> 1);
		if (off & 1)
			m_flag.Enable(SELFILLUM);
		else
			m_flag.Disable(SELFILLUM);
	}

	WBone* GetParent() const { return m_parent; }

protected:
	void ReleasePhysicsModel();
	int FindLink(int body, w_mesh* mesh,
		w_bone_physic_link::w_bone_physic_link_element* out, int offset);

public:
	WBone::w_bone_physic_link* m_physic;
	w_mesh* m_mesh;
	char m_name[64];
	unsigned int m_hashCode;
	char* m_motion_bone_name;
	WQuat m_basicQuat;
	WMatrix m_basicMat;
	WSphere m_bound_sphere;
	Waabb m_bound_aabb;
	w_keyframe_pos* m_keyPos;
	int m_keyPosNum;
	w_keyframe_rot* m_keyRot;
	int m_keyRotNum;
	int m_keyFlag;
	WBone* m_parent;
	WBone* m_child;
	WBone* m_next;
	float m_lightLine;
	int m_rendMode;
	int m_id;
	unsigned int m_selfillumColor;
	unsigned __int8 m_alpha;
	float m_scale;
	WFlags m_flag;
	WMatrix m_localMat;
	WMatrix m_matrix;
	WMatrix m_rotateMat;
	WVector m_moveVec;
	LightSet m_light;
	unsigned int m_vtxColor;
	float m_bonescale;

private:
	static WTVertex* m_vtxList;
	static WVector* m_vecList;
	static int m_vtx_len;
	static int m_vtx_count;
};
