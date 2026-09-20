#pragma once
#include <map>
#include <vector>
#include "wmath.h"
#include "wscene.h"

class WScene;

struct w_mesh;

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
		struct N
		{
			WVector m_n;
			std::vector<int, std::allocator<int> > m_idxList;
		};
		struct P
		{
			WVector m_p;
			std::map<w_mesh*, std::vector<WBone::w_normalmerge::N> > m_meshList;
		};

		std::map<float, std::vector<WBone::w_normalmerge::P> > m_wlist;
	};

	void SetAlpha(unsigned char alpha, int recursive);
	void xSetAlpha(unsigned char alpha, int recursive);
	void SetLight(LightSet* light);
	void CalcLight(LightSet* light, bool recursive, int lightMode,
		WScene* scene);

	void UpdatePhysicsModel(float delta, const WVector& accel);
	void ResetPhysicsModel();
	void SetPhysicsModel(int type);

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
