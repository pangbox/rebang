#pragma once
#include "wflag.h"
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
	float t;
	WVector pos;
};

struct w_keyframe_rot
{
	float t;
	WQuat rot;
};

class WBone
{
public:
	class w_normalmerge
	{
		struct N
		{
			N(const WVector& n)
				: m_n(n)
			{
			}
			WVector m_n;
			std::vector<int, std::allocator<int> > m_idxList;
		};
		typedef std::vector<N> NLIST;
		typedef std::map<w_mesh*, NLIST> MLIST;

		struct P
		{
			P(const WVector& p)
				: m_p(p)
			{
			}
			WVector m_p;
			MLIST m_meshList;
		};

		typedef std::vector<P> PLIST;
		typedef std::map<float, PLIST> WLIST;

	public:
		w_normalmerge(WPuppet& puppet);
		void AddVertex(w_mesh* mesh, int index);

	private:
		void MergeNormal();
		void NormalizeMergedNormals();
		void ApplyToBlendedNormalList();
		static P* Find(PLIST& list, const WVector& v);
		static N* Find(NLIST& list, const WVector& v);
		static void SetMyNormalsEqualToFirstNormal(w_mesh*,
			const std::vector<int>&);
		WLIST m_wlist;
	};
	WBone();
	~WBone();
	WBone* MakeClone(WBone* parent);
	w_mesh* CopyMesh(w_mesh* source);
	void ResetMeshBone(WBone* root, bool bRecur);
	void CopyBone(w_common_bone_data* data);
	void SetName(char* name);
	void SetParent(WBone* parent);
	void SetNext(WBone* bone, bool recursive);
	void ReleaseChild(WBone* child);
	void ReleaseLink()
	{
		m_child = 0;
		m_next = 0;
	}
	void SetLocalMatrix(const WMatrix& matrix);
	void SetLocalMatrix(const WQuat& quat, const WVector& pos)
	{
		m_localMat = quat;
		m_localMat.pivot = pos;
	}
	void SetBasicMatrix(const WMatrix& matrix);
	void CopyBasicMatrix(WBone* root, bool child);
	void SetMatrix(const WMatrix& matrix, WView* view);
	void InsertBoneSetList(float time, WBoneSet* set, int flag);
	void ApplyBoneSetList(WBoneSet* set);
	void OptimizeBoneSet(float (*range)[2], int count, int child);
	void SetKeyframe(w_keyframe* frame, float end, float start);
	void ReplaceKeyframe(WBone* bone, float time);
	void SetMesh(w_faces* faces, int count);
	void SetMesh(w_pet_vertex* vertex, int count, w_pet_tri_point* point,
		w_pet_texture_info** texture, WBone** bone, int flag);
	void SetBoneMesh(w_pet_vertex* vertex, int count, w_pet_tri_point* point,
		w_pet_texture_info** texture, WBone** bone, int flag);
	int GetMeshForFaceAnimation(int group, w_mesh** mesh, int count);
	int GetMeshForFaceAnimation(unsigned char group, w_mesh** mesh, int count);
	w_mesh* GetIndexedmesh(w_faces** faces, int count);
	w_mesh* GetIndexedmesh(w_pet_vertex* vertexList, w_pet_tri_point* pointList,
		WBone** boneList, w_pet_texture_info** texList, int faceNum,
		int* faceIndexList);
	w_mesh* GetMesh() { return m_mesh; }
	void CombineVtxColor(bool recursive);
	unsigned long* FindVColor(const WVector& pos, const WVector& normal);
	enum RenderFlag
	{
		NOCLIP = 0x1,
		NOUPDATEMAT = 0x2,
	};
	void Transform(const WMatrix& mat, WView* view, float matscale, int flag);
	WVector Transform(const WVector& vector) { return vector * m_matrix; }
	void SetRenderMode(int mode);
	void SetAlpha(unsigned char alpha, int recursive);
	unsigned char GetAlpha() { return m_alpha; }
	void RestoreVtxColor(void);
	void SetLightLine(float value);
	void SetLight(LightSet* light);
	void DisableFog();
	void EnableAlphaBlend(void);
	const LightSet& GetLight() { return m_light; }
	void CalcLight(WScene* scene, int lightMode);
	void CalcLight(LightSet* light, bool recursive, int lightMode,
		WScene* scene);
	void ResetLight(void);

protected:
	void CalcLight_SelfIllum(void);
	void CalcLight_Point(LightSet* light, int flag, WScene* scene);
	void CalcLight_Directional_Per_Bone(LightSet* light, int flag,
		WScene* scene);
	void CalcLight_Directional(LightSet* light, int flag, WScene* scene);
	void CalcLight_Diffuse_Equals_Ambient(LightSet* light, int flag);

public:
	void CalcENVCoord(WView* view, w_mesh* mesh);
	void CalcHilightCoord(WView* view, const LightSet& light, w_mesh* mesh);
	void SetLightMode(unsigned long mode);
	void Render(WView* view, const LightSet& light);
	void RenderHierarchy(WView* view, float axisLen);
	void RenderNormals(WView* view, float axisLen);
	void ApplyScale(float scale);
	void SetBoundBox(const Waabb box, bool flag);
	void FixNormal(const WMatrix& matrix);
	void MergeMesh(WBone* source, WBone* root);
	bool CheckRigidVtx(WBone* bone);
	WVector GetDeltaVec(float time);
	WVector GetKeyPos(float time);
	WQuat GetKeyRot(float time);
	WSphere GetBoundSphere() { return m_bound_sphere; }
	Waabb GetBoundBox();
	bool IsVisible() { return m_flag.GetFlag(VISIBLE); }
	bool HasRigidVtx();
	bool IsHidden() { return m_flag.GetFlag(HIDEBONE); }
	bool IsSelfIllum();
	void SetRigidVtxFlag(void);
	void SetInvisible(bool recursive, bool invisible);
	bool IsInvisible() { return m_flag.GetFlag(INVISIBLE); }
	bool HasAnimation()
	{
		if (!m_keyPos && !m_keyRot)
			return false;
		return true;
	}
	WBone* FindBone(const char* name, unsigned long hashCode);
	WBone* FindBone(int id);
	WBone* FindBoneByTail(const char* tail);
	WBone* FindFakeBodyBone(w_pet_vertex* vertex, int count);
	int GetConvexArea(WBlockModel* model);
	void ApplySpecularMap(int handle);
	void SetIDNumber(int value) { m_id = value; }
	int GetID() { return m_id; }
	int SaveBoneName(FILE* file, int type, char* name);
	int SaveAnimationKey(FILE* file, int id, float (*range)[2], int count);
	int SetID(int id, bool flag, char* name);
	void CountBones(int& count);
	float LoadAnimationKey(cFile* file);
	void GetName(char** names, int& count);
	char* GetBoneName() { return m_name; }
	char* GetBoneMotionName() { return m_motion_bone_name; }
	void GetBoneInfo(int* totals);
	int GetOverlapBox(float* box, WBone* bone);
	void GetTotalOverlapBox(WVector* min, WVector* max, WMatrix* matrix,
		bool flag);
	void Rotate(float angle, int axis, bool flag);
	void Move(WVector& vector, bool recursive);
	WVector& GetPivot() { return m_basicMat.pivot; }
	WMatrix& GetLocalMatrix() { return m_localMat; }
	WMatrix& GetBasicMatrix() { return m_basicMat; }
	WQuat& GetBasicRot() { return m_basicQuat; }
	WMatrix& GetMatrix() { return m_matrix; }
	void HideBone(bool recursive);
	void ShowBone(bool recursive);
	void SetSelfIllum(bool enabled, bool recursive);
	void SetSelfIllumColor(unsigned long color, bool recursive);
	unsigned long GetSelfIllumColor() const;
	void ToggleSelfIllum()
	{
		unsigned char off = (unsigned char)~((unsigned long)m_flag >> 1);
		if (off & 1)
			m_flag.Enable(SELFILLUM);
		else
			m_flag.Disable(SELFILLUM);
	}
	WBone* GetChild() const { return m_child; }
	WBone* GetNext() const { return m_next; }
	WBone* GetParent() const { return m_parent; }
	WBone* GetRoot();
	void SetPhysicsModel(int type);
	void ResetPhysicsModel();
	void UpdatePhysicsModel(float delta, const WVector& accel);
	struct w_bone_physic_link
	{
		int vtxnum;
		WVector* pos;
		WVector* vel;
		WVector** list;

		struct w_bone_physic_link_element
		{
			int offset;
			float length;
		};

		w_bone_physic_link_element* link;
		int* linkoffset;
	};

protected:
	WBone::w_bone_physic_link* m_physic;
	void ReleasePhysicsModel();
	int FindLink(int body, w_mesh* mesh,
		w_bone_physic_link::w_bone_physic_link_element* out, int offset);
	w_mesh* m_mesh;

private:
	bool CheckSuitableForKeyframe(float start, float end, float (*range)[2],
		int count, bool last);
	void ClearMesh(w_mesh* mesh);
	int OptimizeRotKey(float (*range)[2], int count, w_keyframe_rot** out);
	int OptimizePosKey(float (*range)[2], int count, w_keyframe_pos** out);
	static int __cdecl SortByTextureHandle(const void* left, const void* right);
	char m_name[64];
	unsigned long m_hashCode;
	char* m_motion_bone_name;
	char* GetNameSubBip(void);
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
	unsigned long m_selfillumColor;
	unsigned char m_alpha;
	float m_scale;
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
	WFlags m_flag;
	WMatrix m_localMat;

public:
	WMatrix m_matrix;

private:
	WMatrix m_rotateMat;
	WVector m_moveVec;
	LightSet m_light;
	void CountVtxBuff(int count);
	WTVertex* GetVtxBuff() { return m_vtxList; }
	WTVertex* GetVtxBuff(int index) { return m_vtxList + index; }

public:
	void AllocVTX(int count);
	void SetVtxColor(unsigned long color);

private:
	unsigned long m_vtxColor;
	static WTVertex* m_vtxList;
	static WVector* m_vecList;
	static int m_vtx_len;
	static int m_vtx_count;

	float m_bonescale;

public:
	bool CompareBasicMatrix(WBone* bone);
	void xBuildTransfMatPtrList(std::vector<const WMatrix*>& list) const;
	void xSetAlpha(unsigned char alpha, int recursive);
	void xRender(WView* view, WxBatchState* state, bool setMatrix, int handle,
		int newHandle);
	WBone* AttachMeshBone(WBone* bone, int handle, int oldHandle);
	void ChangeTextureByHandle(int from, int to, bool recursive, bool flag);
	bool ChangeTexture(const char* name, int oldHandle, int handle);
	bool ChangeTexture(const char* name, int handle);
	void ResetMyNormalList(void);
	void GatherNormalMergeInfo(w_normalmerge& merge) const;
	void SetMirrored(bool mirrored);
	void SetScale(float scale);
	float GetBoneScale();
	static void __fastcall CalcMeshAABB(WBone& bone, w_mesh& mesh);
	static bool IsMeshVisible(WBone* bone, w_mesh* mesh);
	bool CheckMeshBoneAllVertexRigid() const;
};
