#pragma once
#include <map>
#include <vector>
#include "wmath.h"
#include "wscene.h"

class WScene;
class WPuppet;
class WBoneSet;
class WBlockModel;
class cFile;

struct w_mesh;
struct w_pet_vertex;
struct w_pet_tri_point;
struct w_pet_texture_info;

class w_keyframe_pos
{
	float t;
	WVector pos;
};

class w_keyframe_rot
{
	float t;
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

		std::map<float, std::vector<WBone::w_normalmerge::P> > m_wlist;
	};

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
};
