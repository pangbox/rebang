#include "wmath.h"

#include "wview.h"
#include "wpuppet.h"
#include "wbone.h"
#include "wmesh.h"
#include "wpetfile.h"
#include "wboneset.h"
#include "wblockmodel.h"
#include "cfile.h"
#include <list>
#include <algorithm>
#include "wxtnlbuffer.h"
#include <string.h>

WTVertex* WBone::m_vtxList = 0;
WVector* WBone::m_vecList = 0;
int WBone::m_vtx_len = 0;
int WBone::m_vtx_count = 0;

int WriteID(int, FILE*);
bool VECEQU(const WVector&, const WVector&);
bool QUATEQU(const WQuat&, const WQuat&);
struct w_keyframe_scale;
struct w_3d_object;
struct w_vertex;
struct w_rigid;
struct w_keyframe;
struct w_faces;

struct w_rigid
{
	char objname[32];
	w_3d_object* obj;
	WBone* bone;
	float blendWeight;
};

struct w_vertex
{
	WVector v;
	float weight;
	char objname[32];
	w_rigid* rigidVtx;
	int rigidNum;
	w_3d_object* obj;
};

struct w_keyframe
{
	float t;
	WQuat* q;
	WVector* v;
	float* scale;
	w_keyframe* next;
	w_keyframe* prev;
};

struct w_3d_object
{
	w_3d_object* parent;
	w_3d_object* child;
	w_3d_object* next;
	w_vertex* gvertex;
	w_faces* gfaces;
	WMatrix matrix;
	char name[40];
	char parentName[40];
	int nvertex;
	int nfaces;
	int first;
	int type;
	WBone* bone;
	w_3d_object* list;
	w_keyframe* keyframe;
};

struct w_faces
{
	struct w_faces_info
	{
		w_3d_object* object;
		w_vertex* gvertex;
		WBone* bone;
		WVector norm;
		int vindex;
		float tu;
		float tv;
	};

	w_faces_info p[3];
	unsigned int diffuse;
	int texhandle;
	int subNum;
	int mapType;
	int mapFlags;
	int xiMapFlags2;
};

struct w_common_bone_data
{
	int xid;
	char name[64];
	WSphere boundsphere;
	WMatrix basicMat;
	Waabb aabb;
	w_keyframe_pos* keyPos;
	w_keyframe_rot* keyRot;
	w_keyframe_scale* keyScale;
	int keyPosNum;
	int keyRotNum;
	int keyScaleNum;
	int keyFlag;
	w_mesh* mesh;
};

WBone::WBone()
{
	m_parent = 0;
	m_child = 0;
	m_next = 0;
	m_rendMode = 0;
	m_scale = 1.0f;
	m_lightLine = 0.0f;
	m_keyPos = 0;
	m_keyPosNum = 0;
	m_keyRot = 0;
	m_keyRotNum = 0;
	m_keyFlag = 0;
	m_bound_sphere = WSphere(WVector::ZERO, 0.0f);
	m_mesh = 0;
	m_selfillumColor = 0xffffff;
	m_alpha = 0xff;
	m_basicQuat.Reset();
	m_basicMat.Reset();
	m_localMat.Reset();
	m_rotateMat.Reset();
	m_moveVec.Reset();
	m_matrix.Reset();
	m_light.type = 2;
	m_light.nearone = WVector::UNIT_NEG_Y;
	m_bound_aabb = Waabb(WVector::ZERO, WVector(0.0001f, 0.0001f, 0.0001f));
	m_vtxColor = 0xffffffff;
	CountVtxBuff(1);
}

WBone::~WBone()
{
	if (m_mesh != 0)
	{
		ClearMesh(m_mesh);
		if (m_mesh != 0)
		{
			delete m_mesh;
			m_mesh = 0;
		}
	}
	bool shared = ((((ulong)m_flag) >> 6) & 1) != 0;
	if (!shared)
	{
		if (m_keyPos != 0)
			delete[] m_keyPos;
		if (m_keyRot != 0)
			delete[] m_keyRot;
	}
	if (m_next != 0)
	{
		delete m_next;
		m_next = 0;
	}
	if (m_child != 0)
	{
		delete m_child;
		m_child = 0;
	}
	--m_vtx_count;
	if (m_vtx_count == 0)
	{
		if (m_vtxList != 0)
		{
			delete[] m_vtxList;
			m_vtxList = 0;
			m_vtx_len = 0;
		}
	}
	m_bonescale = 1.0f;
}

w_mesh* WBone::CopyMesh(w_mesh* source)
{
	w_mesh* mesh = new w_mesh;
	memcpy(mesh, source, sizeof(w_mesh));
	mesh->next = 0;
	mesh->drawFlag = mesh->drawFlag & 0xfffff800 | source->texHandle;
	mesh->alpha = 1.0f;
	mesh->diffuse = source->diffuse;
	mesh->vtxColorList = new ulong[mesh->vtxNum];
	int i;
	for (i = 0; i < source->vtxNum; ++i)
		mesh->vtxColorList[i] = source->diffuse;
	if (mesh->rigidNum > 0)
	{
		mesh->boneList = (WBone**)g_mem.Alloc(mesh->rigidNum * 4);
		mesh->vtxColorPtrList = (ulong**)g_mem.Alloc(mesh->rigidNum * 4);
		memcpy(mesh->boneList, source->boneList, mesh->rigidNum * 4);
	}
	if (mesh->blendedRigidNum > 0)
	{
		mesh->blendedBoneList = (WBone**)g_mem.Alloc(mesh->blendedTotalNum * 4);
		memcpy(mesh->blendedBoneList, source->blendedBoneList,
			mesh->blendedTotalNum * 4);
	}
	if (mesh->group != 0)
	{
		mesh->uvData = new float[mesh->vtxNum][2];
		memcpy(mesh->uvData, source->uvData, mesh->vtxNum * 8);
		if (source->uvBackup != 0)
		{
			mesh->uvBackup = new float[mesh->vtxNum][2];
			memcpy(mesh->uvBackup, source->uvBackup, mesh->vtxNum * 8);
		}
	}
	if (((((ulong)m_flag) >> 16) & 1) != 0)
	{
		mesh->myNormalList = new WVector[mesh->vtxNum];
		memcpy(mesh->myNormalList, mesh->normList, mesh->vtxNum * 12);
		if (mesh->blendedTotalNum > 0)
		{
			mesh->myBlendedNormalList = new WVector[mesh->blendedTotalNum];
			memcpy(mesh->myBlendedNormalList, mesh->blendedNormalList,
				mesh->blendedTotalNum * 12);
		}
	}
	return mesh;
}

void WBone::CopyBone(w_common_bone_data* data)
{
	m_id = data->xid;
	SetName(data->name);
	m_bound_sphere = data->boundsphere;
	m_bound_aabb = data->aabb;
	m_keyPos = data->keyPos;
	m_keyPosNum = data->keyPosNum;
	m_keyRot = data->keyRot;
	m_keyRotNum = data->keyRotNum;
	m_keyFlag = data->keyFlag;
	SetBasicMatrix(data->basicMat);
	if (data->mesh != 0)
	{
		m_mesh = CopyMesh(data->mesh);
		if (m_mesh->rigidNum != 0 || m_mesh->blendedRigidNum != 0)
			m_flag.Enable(4);
		w_mesh* last = m_mesh;
		for (w_mesh* source = data->mesh->next; source != 0;
			source = source->next)
		{
			last->next = CopyMesh(source);
			if (last->next->rigidNum != 0 || last->next->blendedRigidNum != 0)
				m_flag.Enable(4);
			last = last->next;
		}
	}
	m_flag.Enable(0x60);
}

void WBone::ClearMesh(w_mesh* mesh)
{
	if (mesh->next != 0)
	{
		ClearMesh(mesh->next);
		if (mesh->next != 0)
		{
			delete mesh->next;
			mesh->next = 0;
		}
	}
	bool shared = ((((ulong)m_flag) >> 5) & 1) != 0;
	if (!shared)
	{
		if (mesh->indexList != 0)
		{
			delete[] mesh->indexList;
			mesh->indexList = 0;
		}
		if (mesh->normList != 0)
		{
			delete[] mesh->normList;
			mesh->normList = 0;
		}
		if (mesh->vecList != 0)
		{
			delete[] mesh->vecList;
			mesh->vecList = 0;
		}
		if (mesh->weightList != 0)
		{
			delete[] mesh->weightList;
			mesh->weightList = 0;
		}
		if (mesh->originalVtxColorList != 0)
		{
			delete[] mesh->originalVtxColorList;
			mesh->originalVtxColorList = 0;
		}
		if (mesh->uvData != 0)
		{
			delete[] mesh->uvData;
			mesh->uvData = 0;
		}
		if (mesh->uvBackup != 0)
		{
			delete[] mesh->uvBackup;
			mesh->uvBackup = 0;
		}
	}
	else if (mesh->group != 0)
	{
		if (mesh->uvData != 0)
		{
			delete[] mesh->uvData;
			mesh->uvData = 0;
		}
		if (mesh->uvBackup != 0)
		{
			delete[] mesh->uvBackup;
			mesh->uvBackup = 0;
		}
	}
	if (mesh->vtxColorList != 0)
	{
		delete[] mesh->vtxColorList;
		mesh->vtxColorList = 0;
	}
	if (mesh->myBlendedNormalList != 0)
	{
		delete[] mesh->myBlendedNormalList;
		mesh->myBlendedNormalList = 0;
	}
	if (mesh->myNormalList != 0)
	{
		delete[] mesh->myNormalList;
		mesh->myNormalList = 0;
	}
	if (mesh->blendedTotalNum > 0)
	{
		bool blendShared = ((((ulong)m_flag) >> 5) & 1) != 0;
		if (!blendShared)
		{
			g_mem.Free(mesh->blendWeightList);
			if (mesh->blendedVecList != 0)
			{
				delete[] mesh->blendedVecList;
				mesh->blendedVecList = 0;
			}
			if (mesh->blendedNormalList != 0)
			{
				delete[] mesh->blendedNormalList;
				mesh->blendedNormalList = 0;
			}
		}
		g_mem.Free(mesh->blendedBoneList);
	}
	if (mesh->rigidNum > 0)
	{
		g_mem.Free(mesh->boneList);
		g_mem.Free(mesh->vtxColorPtrList);
	}
}

WBone* WBone::MakeClone(WBone* parent)
{
	if (((((ulong)m_flag) >> 16) & 1) != 0)
		return 0;

	w_common_bone_data data;
	data.xid = m_id;
	strcpy(data.name, m_name);
	data.basicMat = m_basicMat;
	data.boundsphere = m_bound_sphere;
	data.aabb = m_bound_aabb;
	data.keyPos = m_keyPos;
	data.keyPosNum = m_keyPosNum;
	data.keyRot = m_keyRot;
	data.keyRotNum = m_keyRotNum;
	data.keyFlag = m_keyFlag;
	data.mesh = m_mesh;

	WBone* clone = new WBone;
	clone->CopyBone(&data);
	clone->SetParent(parent);

	if (m_next != 0)
	{
		if (parent != 0)
		{
			m_next->MakeClone(parent);
		}
		else
			clone->SetNext(m_next->MakeClone(0), true);
	}
	if (m_child != 0)
		m_child->MakeClone(clone);
	return clone;
}

void WBone::MergeMesh(WBone* source, WBone* root)
{
	WMatrix matrix;
	matrix = source->m_matrix * ~m_matrix;
	if (m_mesh != 0)
	{
		w_mesh* last;
		for (last = m_mesh; last->next; last = last->next)
		{
		}
		last->next = source->m_mesh;
	}
	else
	{
		m_mesh = source->m_mesh;
	}
	for (w_mesh* mesh = source->m_mesh; mesh != 0; mesh = mesh->next)
	{
		int i;
		for (i = 0; i < mesh->vtxNum; ++i)
		{
			WVector& vec = mesh->vecList[i];
			vec = WVector(matrix.xx * vec.x + matrix.xz * vec.z +
					matrix.xy * vec.y + matrix.xm,
				matrix.yx * vec.x + matrix.yz * vec.z + matrix.yy * vec.y +
					matrix.ym,
				matrix.zx * vec.x + matrix.zz * vec.z + matrix.zy * vec.y +
					matrix.zm);
		}
		for (i = 0; i < mesh->rigidNum; ++i)
		{
			WBone*& bone = mesh->boneList[i];
			bone = root->FindBone(bone->m_name, 0);
		}
		for (i = 0; i < mesh->blendedTotalNum; ++i)
		{
			WBone*& bone = mesh->blendedBoneList[i];
			bone = root->FindBone(bone->m_name, 0);
		}
	}
	source->m_mesh = 0;
}

bool WBone::CheckRigidVtx(WBone* bone)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		int i;
		for (i = 0; i < mesh->rigidNum; ++i)
		{
			if (mesh->boneList[i] == bone)
				return true;
		}
		for (i = 0; i < mesh->blendedTotalNum; ++i)
		{
			if (mesh->blendedBoneList[i] == bone)
				return true;
		}
	}
	if (m_next != 0 && m_next->CheckRigidVtx(bone))
		return true;
	if (m_child != 0 && m_child->CheckRigidVtx(bone))
		return true;
	return false;
}

void WBone::ResetMeshBone(WBone* root, bool child)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		int i;
		for (i = 0; i < mesh->rigidNum; ++i)
		{
			WBone*& bone = mesh->boneList[i];
			if (bone != 0)
				bone = root->FindBone(bone->m_name, 0);
		}
		for (i = 0; i < mesh->blendedTotalNum; ++i)
		{
			WBone*& bone = mesh->blendedBoneList[i];
			bone = root->FindBone(bone->m_name, 0);
		}
		if (mesh->aabbBone != 0)
			mesh->aabbBone = root->FindBone(mesh->aabbBone->m_name, 0);
	}
	if (child)
	{
		if (m_next != 0)
			m_next->ResetMeshBone(root, true);
		if (m_child != 0)
			m_child->ResetMeshBone(root, true);
	}
}

void WBone::SetName(char* name)
{
	strcpy(m_name, name);
	m_hashCode = GetHashCode(m_name);
	m_motion_bone_name = GetNameSubBip();
}

void WBone::SetParent(WBone* parent)
{
	if (parent != 0)
	{
		this->m_next = parent->m_child;
		this->m_parent = parent;
		parent->m_child = this;
	}
	else
	{
		this->m_parent = 0;
	}
}

void WBone::ReleaseChild(WBone* child)
{
	WBone* current = m_child;
	if (current == child)
	{
		m_child = current->m_next;
		return;
	}
	while (current != 0)
	{
		if (current->m_next == child)
		{
			current->m_next = child->m_next;
			return;
		}
		current = current->m_next;
	}
}

void WBone::SetNext(WBone* next, bool append)
{
	if (next && append)
	{
		WBone* tail;
		for (tail = next; tail->m_next; tail = tail->m_next)
		{
		}
		tail->m_next = m_next;
	}
	m_next = next;
}

void WBone::SetBasicMatrix(const WMatrix& matrix)
{
	m_basicMat = matrix;
	m_basicQuat = matrix;
	m_localMat = matrix;
	m_matrix = matrix;
	float z = m_localMat.zz;
	float y = m_localMat.yz;
	float x = m_localMat.xz;
	float len = x * x + y * y + z * z;
	m_scale = (float)sqrt(len);
	m_basicQuat.Normalize();
}

void WBone::CopyBasicMatrix(WBone* root, bool child)
{
	WBone* target = root->FindBone(m_name, 0);
	if (target == 0 && strstr(m_name, "Bip") != 0)
		target = root->FindBone(GetNameSubBip(), 0);
	if (target != 0)
	{
		WMatrix matrix;
		matrix = GetKeyRot(0.0f);
		matrix.pivot = GetKeyPos(0.0f);
		target->SetBasicMatrix(matrix);
	}
	if (child)
	{
		if (m_next != 0)
			m_next->CopyBasicMatrix(root, child);
		if (m_child != 0)
			m_child->CopyBasicMatrix(root, child);
	}
}

void WBone::SetMatrix(const WMatrix& matrix, WView* view)
{
	if ((((ulong)m_flag) & 0x600) != 0)
	{
		bool rotated = ((((ulong)m_flag) >> 9) & 1) != 0;
		if (rotated)
			m_matrix = m_rotateMat * matrix;
		bool moved = ((((ulong)m_flag) >> 10) & 1) != 0;
		if (moved)
		{
			m_matrix.pivot -= m_moveVec * m_localMat;
		}
	}
	else
	{
		m_matrix = matrix;
	}
	for (WBone* bone = m_child; bone != 0; bone = bone->m_next)
		bone->SetMatrix(bone->m_localMat * m_matrix, view);
}

WVector WBone::GetKeyPos(float time)
{
	if (m_keyPos == 0)
		return m_basicMat.pivot;
	int low = 0;
	int high = m_keyPosNum - 1;
	int mid;
	do
	{
		mid = (low + high) >> 1;
		if (m_keyPos[mid].t >= time)
			high = mid;
		else
			low = mid + 1;
	} while (low < high);
	if (m_keyPos[mid].t != time)
	{
		if (mid != 0 || m_keyPos[0].t < time)
		{
			int index = mid;
			if (m_keyPos[mid].t > time)
				index = mid - 1;
			float rate = (time - m_keyPos[index].t) /
				(m_keyPos[high].t - m_keyPos[index].t);
			return (m_keyPos[high].pos - m_keyPos[index].pos) * rate +
				m_keyPos[index].pos;
		}
	}
	return m_keyPos[mid].pos;
}

WQuat WBone::GetKeyRot(float time)
{
	if (0)
		Abs(time);
	if (m_keyRot == 0)
		return GetBasicRot();
	int low = 0;
	int high = m_keyRotNum - 1;
	int mid;
	do
	{
		mid = (low + high) >> 1;
		if (m_keyRot[mid].t >= time)
			high = mid;
		else
			low = mid + 1;
	} while (low < high);
	if (m_keyRot[mid].t != time && (mid != 0 || m_keyRot[0].t < time))
	{
		int index = mid;
		if (m_keyRot[mid].t > time)
			index = mid - 1;
		float rate =
			(time - m_keyRot[index].t) / (m_keyRot[high].t - m_keyRot[index].t);
		return WQuaternionSlerp(m_keyRot[index].rot, m_keyRot[high].rot, rate);
	}
	return m_keyRot[mid].rot;
}

void WBone::InsertBoneSetList(float time, WBoneSet* set, int flag)
{
	if ((flag & 4) == 0 || strstr(m_name, "tail") == 0)
	{
		WQuat rot;
		WVector pos;
		float scale;
		if ((flag & 2) != 0)
		{
			rot = GetKeyRot(time);
			pos = GetKeyPos(time);
			set->AddBoneKey(m_motion_bone_name, &rot, &pos, 0, 3);
		}
		else if (m_keyFlag != 0)
		{
			if (m_keyRot != 0)
				rot = GetKeyRot(time);
			if (m_keyPos != 0)
				pos = GetKeyPos(time);
			set->AddBoneKey(m_motion_bone_name, &rot, &pos, &scale, m_keyFlag);
		}
	}
	if ((flag & 1) != 0)
	{
		for (WBone* bone = m_child; bone != 0; bone = bone->m_next)
			bone->InsertBoneSetList(time, set, flag);
	}
}

WVector WBone::GetDeltaVec(float time)
{
	const WVector* src;
	w_keyframe_pos* keys = m_keyPos;
	if (keys != 0 && time > 0.0f)
	{
		WVector pos = GetKeyPos(time) - keys[0].pos;
		src = &pos;
	}
	else
	{
		src = &WVector::ZERO;
	}
	return *src;
}

char* WBone::GetNameSubBip(void)
{
	char* found = strstr(m_name, "Bip");
	if (found == 0)
		found = strstr(m_name, "bip");
	if (found != 0)
	{
		while (found[3] != 0)
		{
			if (found[3] != ' ' && found[2] == ' ')
				break;
			++found;
		}
		return found + 3;
	}
	return m_name;
}

void WBone::ApplyBoneSetList(WBoneSet* set)
{
	WBoneKey* key = set->FindBoneKey(m_motion_bone_name);
	if (key != 0 && key->flags != 0)
	{
		const WVector* pos;
		if ((key->flags & 2) != 0)
			pos = &key->pivot;
		else
			pos = &m_basicMat.pivot;
		const WQuat* quat;
		if ((key->flags & 1) != 0)
			quat = &key->quat;
		else
			quat = &m_basicQuat;
		SetLocalMatrix(*quat, *pos);
	}
	if (m_next != 0)
		m_next->ApplyBoneSetList(set);
	if (m_child != 0)
		m_child->ApplyBoneSetList(set);
}

void WBone::SetKeyframe(w_keyframe* frame, float end, float start)
{
	m_keyPosNum = 0;
	m_keyRotNum = 0;
	for (w_keyframe* key = frame; key != 0; key = key->next)
	{
		if (key->q != 0)
			++m_keyRotNum;
		if (key->v != 0)
			++m_keyPosNum;
		if (key->next == 0)
		{
			if (m_keyRotNum != 0 && (key->q == 0 || key->t < end))
				++m_keyRotNum;
			if (m_keyPosNum != 0 && (key->v == 0 || key->t < end))
				++m_keyPosNum;
		}
	}
	m_keyFlag = 0;
	if (m_keyRotNum != 0)
	{
		m_keyRot = new w_keyframe_rot[m_keyRotNum];
		m_keyFlag |= 1;
	}
	if (m_keyPosNum != 0)
	{
		m_keyPos = new w_keyframe_pos[m_keyPosNum];
		m_keyFlag |= 2;
	}
	int rot = 0;
	int pos = 0;
	for (w_keyframe* key = frame; key != 0; key = key->next)
	{
		if (key->q != 0)
		{
			m_keyRot[rot].t = key->t - start;
			m_keyRot[rot].rot = *key->q;
			++rot;
		}
		if (key->v != 0)
		{
			m_keyPos[pos].t = key->t - start;
			m_keyPos[pos].pos = *key->v;
			++pos;
		}
	}
	if (rot != 0 && rot != m_keyRotNum)
	{
		m_keyRot[rot].t = end - start;
		m_keyRot[rot].rot = m_keyRot[rot - 1].rot;
	}
	if (pos != 0 && pos != m_keyPosNum)
	{
		m_keyPos[pos].t = end - start;
		m_keyPos[pos].pos = m_keyPos[pos - 1].pos;
	}
}

bool WBone::CheckSuitableForKeyframe(float start, float end, float (*range)[2],
	int count, bool last)
{
	for (int i = 0; i < count; ++i)
	{
		if (last == 0)
		{
			if (start <= range[i][0] && range[i][0] <= end)
				return true;
		}
		else if (last == 1)
		{
			if (start <= range[i][1] && range[i][1] <= end)
				return true;
		}
	}
	if (count == 0)
		return true;
	return false;
}

bool __fastcall VECEQU(const WVector& left, const WVector& right)
{
	float diff;
	diff = left.x - right.x;
	if (Abs(diff) <= 0.0001f)
	{
		diff = left.y - right.y;
		if (Abs(diff) <= 0.0001f)
		{
			diff = left.z - right.z;
			if (Abs(diff) <= 0.0001f)
				return true;
		}
	}
	return false;
}

bool __fastcall QUATEQU(const WQuat& left, const WQuat& right)
{
	float diff;
	diff = left.x - right.x;
	if ((diff > 0.0f ? diff : -diff) <= 0.0001f)
	{
		diff = left.y - right.y;
		if ((diff > 0.0f ? diff : -diff) <= 0.0001f)
		{
			diff = left.z - right.z;
			if ((diff > 0.0f ? diff : -diff) <= 0.0001f)
			{
				if (Abs(left.w - right.w) <= 0.0001f)
					return true;
			}
		}
	}
	return false;
}

int WBone::OptimizeRotKey(float (*range)[2], int count, w_keyframe_rot** out)
{
	int* map = new int[m_keyRotNum];
	int num = 0;
	int i;
	int prev;
	WQuat rot;
	for (i = 0; i < m_keyRotNum; ++i)
	{
		if (i > 0 && i < m_keyRotNum - 1)
		{
			float t = (m_keyRot[i].t - m_keyRot[prev].t) /
				(m_keyRot[i + 1].t - m_keyRot[prev].t);
			rot = WQuaternionSlerp(m_keyRot[prev].rot, m_keyRot[i + 1].rot, t);
			if (QUATEQU(rot, m_keyRot[i].rot))
			{
				map[i] = -1;
				continue;
			}
		}
		map[i] = num;
		prev = i;
		++num;
	}
	if (num < m_keyRotNum && out != 0)
	{
		w_keyframe_rot* keys = new w_keyframe_rot[num];
		for (i = 0; i < m_keyRotNum; ++i)
		{
			if (map[i] >= 0)
				memcpy(&keys[map[i]], &m_keyRot[i], sizeof(w_keyframe_rot));
		}
		*out = keys;
	}
	delete[] map;
	return num;
}

int WBone::OptimizePosKey(float (*range)[2], int count, w_keyframe_pos** out)
{
	int* map = new int[m_keyPosNum];
	int i = 0;
	int num = 0;

	int prev;
	WVector pos;
	for (; i < m_keyPosNum; ++i)
	{
		if (i > 0 && i < m_keyPosNum - 1)
		{
			float t = (m_keyPos[i].t - m_keyPos[prev].t) /
				(m_keyPos[i + 1].t - m_keyPos[prev].t);
			pos = (m_keyPos[i + 1].pos - m_keyPos[prev].pos) * t +
				m_keyPos[prev].pos;
			if (VECEQU(pos, m_keyPos[i].pos))
			{
				map[i] = -1;
				continue;
			}
		}
		map[i] = num;
		prev = i;
		++num;
	}
	if (num < m_keyPosNum && out != 0)
	{
		w_keyframe_pos* keys = new w_keyframe_pos[num];
		for (i = 0; i < m_keyPosNum; ++i)
		{
			if (map[i] >= 0)
				memcpy(&keys[map[i]], &m_keyPos[i], sizeof(w_keyframe_pos));
		}
		*out = keys;
	}
	delete[] map;
	return num;
}

void WBone::OptimizeBoneSet(float (*range)[2], int count, int child)
{
	w_keyframe_rot* rot = m_keyRot;
	w_keyframe_pos* pos = m_keyPos;
	if (m_keyRot != 0)
	{
		m_keyRotNum = OptimizeRotKey(range, count, &m_keyRot);
		if (rot != m_keyRot)
			delete[] rot;
	}
	if (m_keyPos != 0)
	{
		m_keyPosNum = OptimizePosKey(range, count, &m_keyPos);
		if (pos != m_keyPos)
			delete[] pos;
	}
	if (m_next != 0 && child != 0)
		m_next->OptimizeBoneSet(range, count, 1);
	if (m_child != 0 && child != 0)
		m_child->OptimizeBoneSet(range, count, 1);
}

void WBone::SetMesh(w_pet_vertex* vertexList, int vtxNum,
	w_pet_tri_point* pointList, w_pet_texture_info** texList, WBone** boneList,
	int faceNum)
{
	int owned = 0;
	int pick;
	for (int face = 0; face < faceNum; ++face)
	{
		int j;
		for (j = 0; j < 3; ++j)
		{
			if (j != 0)
			{
				WBone* held = boneList[face * 3 + pick];
				WBone* other = boneList[face * 3 + j];
				if (held == other)
					continue;
				if (other->FindBone(held->m_name, 0) == 0)
					continue;
			}
			pick = j;
		}
		if (boneList[face * 3 + pick] == this)
			++owned;
	}

	if (m_mesh != 0)
	{
		ClearMesh(m_mesh);
		if (m_mesh != 0)
		{
			delete m_mesh;
			m_mesh = 0;
		}
	}

	if (owned != 0)
	{
		int* faceList = new int[owned];
		int* groupList = new int[owned];
		int face = 0;
		if (faceNum > 0)
		{
			int* write = faceList;
			do
			{
				int j;
				for (j = 0; j < 3; ++j)
				{
					if (j != 0)
					{
						WBone* held = boneList[face * 3 + pick];
						WBone* other = boneList[face * 3 + j];
						if (held == other)
							continue;
						if (other->FindBone(held->m_name, 0) == 0)
							continue;
					}
					pick = j;
				}
				if (boneList[face * 3 + pick] == this)
				{
					*write = face;
					++write;
				}
				++face;
			} while (face < faceNum);
		}
		int done = 0;
		while (done < owned)
		{
			int scan;
			for (scan = 0; scan < owned; ++scan)
			{
				if (faceList[scan] >= 0)
					break;
			}
			w_pet_texture_info* first = texList[faceList[scan]];
			int texHandle = first->texHandle;
			unsigned char group = first->group;
			int i = 0;
			int used = 0;
			for (; i < owned; ++i)
			{
				int index = faceList[i];
				if (index < 0)
					continue;
				w_pet_texture_info* tex = texList[index];
				if (tex->texHandle != texHandle)
					continue;
				if (tex->group != group)
					continue;
				groupList[used] = index;
				++done;
				faceList[i] = -1;
				++used;
			}
			if (used != 0)
			{
				w_mesh* mesh = GetIndexedmesh(vertexList, pointList, boneList,
					texList, used, groupList);
				mesh->group = group;
				mesh->next = m_mesh;
				m_mesh = mesh;
				if (mesh->rigidNum != 0 || mesh->blendedRigidNum != 0)
					m_flag.Enable(4);
			}
		}
		delete[] groupList;
		delete[] faceList;
	}

	if (m_next != 0)
		m_next->SetMesh(vertexList, vtxNum, pointList, texList, boneList,
			faceNum);
	if (m_child != 0)
		m_child->SetMesh(vertexList, vtxNum, pointList, texList, boneList,
			faceNum);
}

void WBone::SetBoneMesh(w_pet_vertex* vertexList, int vtxNum,
	w_pet_tri_point* pointList, w_pet_texture_info** texList, WBone** boneList,
	int faceNum)
{
	if (m_mesh != 0)
	{
		ClearMesh(m_mesh);
		if (m_mesh != 0)
		{
			delete m_mesh;
			m_mesh = 0;
		}
	}
	int* mark = new int[faceNum];
	int* faces = new int[faceNum];
	memset(mark, 0, faceNum * 4);
	int done = 0;
	if (faceNum > 0)
	{
		do
		{
			int i = 0;
			do
			{
				if (mark[i] >= 0)
					break;
				++i;
			} while (i < faceNum);
			int handle = texList[i]->texHandle;
			unsigned char group = texList[i]->group;
			int count = 0;
			for (; i < faceNum; ++i)
			{
				if (texList[i]->texHandle == handle &&
					texList[i]->group == group)
				{
					faces[count] = i;
					mark[i] = -1;
					++count;
				}
			}
			if (count != 0)
			{
				done += count;
				m_flag.Enable(0x10000);
				w_mesh* mesh = GetIndexedmesh(vertexList, pointList, boneList,
					texList, count, faces);
				mesh->group = group;
				mesh->next = m_mesh;
				m_mesh = mesh;
				if (mesh->rigidNum != 0 || mesh->blendedRigidNum != 0)
					m_flag.Enable(4);
			}
		} while (done < faceNum);
	}
	delete[] faces;
	delete[] mark;
}

struct w_temp_vertex_list
{
	w_temp_vertex_list() { }
	short index;
	WVector pos;
	WVector normal;
	float weight;
	WBone* bone;
	float tu;
	float tv;
	w_vertex* gvertex;
	w_pet_vertex* vertex;
	int kind;
};

w_mesh* WBone::GetIndexedmesh(w_pet_vertex* vtxList, w_pet_tri_point* triList,
	WBone** triBoneTable, w_pet_texture_info** texInfo, int faceNum,
	int* faceIndex)
{
	short* table;
	int blendTotal = 0;
	int blendNum = 0;
	int maxBoneNum = 0;
	WVector vec, pos;
	int vtxNum;
	WMatrix invMat;
	invMat = ~m_matrix;

	w_temp_vertex_list* list = new w_temp_vertex_list[faceNum * 3];
	int l = faceNum * 3;
	int i = 0;
	vtxNum = 0;
	int rigidNum = 0;

	int j, k;
	for (; i < l; ++i)
	{
		w_pet_tri_point* point = &triList[faceIndex[i / 3] * 3 + i % 3];
		vec = WVector(vtxList[point->pos].x, vtxList[point->pos].y,
			vtxList[point->pos].z);
		for (j = 0; j < vtxNum; j++)
		{
			if (WisEqual(list[j].pos, vec, 1e-5f) != 0 &&
				list[j].weight == vtxList[point->pos].weight &&
				list[j].tu == point->tu && list[j].tv == point->tv &&
				fabs(list[j].normal.x - point->nx) <= 0.001f &&
				fabs(list[j].normal.y - point->ny) <= 0.001f &&
				fabs(list[j].normal.z - point->nz) <= 0.001f &&
				list[j].bone == triBoneTable[point - triList])
				break;
		}
		list[i].index = (short)j;
		if (j == vtxNum)
		{
			list[j].pos = vec;
			list[j].weight = vtxList[point->pos].weight;
			list[j].tu = point->tu;
			list[j].tv = point->tv;
			list[j].normal = WVector(point->nx, point->ny, point->nz);
			list[j].bone = triBoneTable[point - triList];
			list[j].vertex = &vtxList[point->pos];
			int kind = vtxList[point->pos].list != 0
				? 2
				: (list[j].bone != this ? 1 : 0);
			++vtxNum;
			list[j].kind = kind;
			if (kind > 0)
				++rigidNum;
			if (kind > 1)
			{
				++blendNum;
				blendTotal += list[j].vertex->listNum;
			}
		}
	}

	w_mesh* mesh = new w_mesh;
	memset(mesh, 0, sizeof(w_mesh));
	mesh->originalTexHandle = mesh->texHandle =
		texInfo[faceIndex[0]]->texHandle;
	mesh->originalTu = mesh->tu = texInfo[faceIndex[0]]->pu;
	mesh->originalTv = mesh->tv = texInfo[faceIndex[0]]->pv;
	mesh->originalScaleu = mesh->scaleu = texInfo[faceIndex[0]]->scalex;
	mesh->originalScalev = mesh->scalev = texInfo[faceIndex[0]]->scaley;
	mesh->diffuse = texInfo[faceIndex[0]]->diffuse;
	mesh->group = 0;
	mesh->next = 0;
	mesh->alpha = (float)(texInfo[faceIndex[0]]->alpha / 255.0f);
	mesh->flags = texInfo[faceIndex[0]]->drawFlags |
		(mesh->alpha < 1.0f ? 0x20400000 : 0);
	mesh->xiDrawFlag2 = texInfo[faceIndex[0]]->xulDrawFlag2;
	mesh->bEnv = texInfo[faceIndex[0]]->mapType == 1 ||
			texInfo[faceIndex[0]]->mapType == 2
		? true
		: false;
	mesh->bSpec = texInfo[faceIndex[0]]->mapType == 3;
	mesh->indexNum = l;
	mesh->indexList = new unsigned short[l];

	mesh->rigidNum = rigidNum;
	mesh->vtxColorPtrList =
		rigidNum > 0 ? (ulong**)g_mem.Alloc(rigidNum * 4) : 0;
	mesh->boneList =
		mesh->rigidNum > 0 ? (WBone**)g_mem.Alloc(mesh->rigidNum * 4) : 0;
	if (mesh->boneList != 0)
		memset(mesh->boneList, 0, mesh->rigidNum * 4);
	mesh->blendedRigidNum = blendNum;
	mesh->blendedTotalNum = blendTotal;
	mesh->blendWeightList =
		blendTotal > 0 ? (float*)g_mem.Alloc(blendTotal * 4) : 0;
	mesh->blendedBoneList =
		blendTotal > 0 ? (WBone**)g_mem.Alloc(blendTotal * 4) : 0;
	mesh->blendedVecList = blendTotal > 0 ? new WVector[blendTotal] : 0;
	mesh->blendedNormalList = blendTotal > 0 ? new WVector[blendTotal] : 0;

	mesh->vtxNum = vtxNum;
	mesh->vecList = new WVector[mesh->vtxNum];
	mesh->normList = new WVector[mesh->vtxNum];
	mesh->vtxColorList = new ulong[mesh->vtxNum];
	mesh->originalVtxColorList = new ulong[mesh->vtxNum];
	mesh->myBlendedNormalList = 0;
	mesh->myNormalList = 0;
	if (((((ulong)m_flag) >> 16) & 1) != 0)
		mesh->weightList = new float[mesh->vtxNum];
	else
		mesh->weightList = 0;
	mesh->uvData = new float[mesh->vtxNum][2];
	mesh->uvBackup =
		texInfo[faceIndex[0]]->mapType > 1 ? new float[mesh->vtxNum][2] : 0;
	for (i = 0; i < mesh->vtxNum; ++i)
	{
		mesh->originalVtxColorList[i] = mesh->vtxColorList[i] = mesh->diffuse;
	}
	AllocVTX(mesh->vtxNum);

	table = new short[mesh->vtxNum];
	int n;
	for (n = 0, i = 0, j = 0, k = blendNum, l = rigidNum; i < mesh->indexNum;
		++i)
	{
		if (list[i].index == n)
		{
			switch (list[n].kind)
			{
			case 0:
				table[n] = (short)l++;
				break;
			case 1:
				table[n] = (short)k++;
				break;
			case 2:
				table[n] = (short)j++;
				break;
			}
			++n;
		}
	}

	for (i = 0, k = 0, n = 0; i < mesh->indexNum; ++i)
	{
		mesh->indexList[i] = table[list[i].index];
		if (list[i].index != n)
			continue;
		if (table[n] < blendNum)
		{
			for (j = 0; j < list[n].vertex->listNum; ++j)
			{
				mesh->blendedBoneList[k] = list[n].vertex->list[j].bone;
				mesh->blendWeightList[k] =
					(float)list[n].vertex->list[j].blendWeight / 255.0f;
				mesh->blendedVecList[k] =
					list[n].pos * ~mesh->blendedBoneList[k]->GetMatrix();
				mesh->blendedNormalList[k] = RotVec(list[n].normal,
					~mesh->blendedBoneList[k]->GetMatrix());
				mesh->blendedNormalList[k].Normalize();
				++k;
			}
			if (maxBoneNum < list[n].vertex->listNum)
				maxBoneNum = list[n].vertex->listNum;
		}
		if (table[n] < rigidNum)
		{
			mesh->boneList[table[n]] = list[n].bone;
			mesh->vecList[table[n]] = list[n].pos * ~list[n].bone->m_matrix;
			mesh->vtxColorPtrList[table[n]] = &mesh->vtxColorList[table[n]];
			mesh->normList[table[n]] =
				RotVec(list[n].normal, ~list[n].bone->m_matrix);
		}
		else
		{
			mesh->vecList[table[n]] = list[n].pos * invMat;
			mesh->normList[table[n]] = RotVec(list[n].normal, invMat);
		}
		if (mesh->weightList != 0)
			mesh->weightList[table[n]] = list[n].weight;
		mesh->normList[table[n]].Normalize();
		mesh->uvData[table[n]][0] =
			texInfo[faceIndex[i / 3]]->scalex * list[n].tu +
			texInfo[faceIndex[i / 3]]->pu;
		mesh->uvData[table[n]][1] =
			texInfo[faceIndex[i / 3]]->scaley * list[n].tv +
			texInfo[faceIndex[i / 3]]->pv;
		if (mesh->uvBackup != 0)
		{
			mesh->uvBackup[table[n]][0] = mesh->uvData[table[n]][0];
			mesh->uvBackup[table[n]][1] = mesh->uvData[table[n]][1];
		}
		++n;
	}

	if (maxBoneNum == 0 && mesh->rigidNum > 0)
		maxBoneNum = 1;
	mesh->maxBoneNum = maxBoneNum;
	CalcMeshAABB(*this, *mesh);
	delete[] table;
	delete[] list;
	return mesh;
}

void WBone::SetMesh(w_faces* faces, int count)
{
	m_mesh = 0;
	if (count != 0)
	{
		w_faces** list = new w_faces*[count];
		int i;
		for (i = 0; i < count; ++i)
			list[i] = &faces[i];
		qsort(list, count, 4, SortByTextureHandle);
		int first = 0;
		int last = 0;
		while (last < count)
		{
			while (
				last < count && list[first]->texhandle == list[last]->texhandle)
				++last;
			w_mesh* mesh = GetIndexedmesh(&list[first], last - first);
			mesh->next = m_mesh;
			m_mesh = mesh;
			first = last;
		}
		delete[] list;
	}
}

int __cdecl WBone::SortByTextureHandle(const void* left, const void* right)
{
	w_faces* a = *(w_faces**)left;
	w_faces* b = *(w_faces**)right;
	if (a->texhandle == b->texhandle)
		return a->p[0].bone - b->p[0].bone;
	return a->texhandle - b->texhandle;
}

w_mesh* WBone::GetIndexedmesh(w_faces** faces, int count)
{
	int rigidNum = 0;
	int blendedRigidNum = 0;
	int blendedTotalNum = 0;
	w_temp_vertex_list* list = new w_temp_vertex_list[count * 3];
	int cornerNum = count * 3;

	int i, j, k, l, n;
	WVector vec;
	WMatrix invMat;
	for (i = 0; i < cornerNum; ++i)
	{
		w_faces::w_faces_info* p = &faces[i / 3]->p[i % 3];
		list[i].index = -1;
		list[i].pos = p->object->gvertex[p->vindex].v;
		list[i].normal = p->norm;
		list[i].bone = p->object->gvertex[p->vindex].obj != 0
			? p->object->gvertex[p->vindex].obj->bone
			: p->object->bone;
		list[i].tu = p->tu;
		list[i].tv = p->tv;
		list[i].kind = p->object->gvertex[p->vindex].rigidNum != 0
			? 2
			: (list[i].bone != this);
		list[i].gvertex = &p->object->gvertex[p->vindex];
	}

	for (i = 0, n = 0; i < cornerNum; ++i)
	{
		for (j = 0; j <= i; ++j)
		{
			w_faces::w_faces_info* p = &faces[j / 3]->p[j % 3];
			if (list[i].tu == p->tu && list[i].tv == p->tv &&
				WisEqual(list[i].pos, p->object->gvertex[p->vindex].v, 1e-5f) &&
				list[i].bone ==
					(p->object->gvertex[p->vindex].obj != 0
							? p->object->gvertex[p->vindex].obj->bone
							: p->object->bone) &&
				(list[i].normal - p->norm).Magnitude() <= 0.01f)
			{
				if (i == j)
				{
					list[i].index = (short)n++;
					if (list[i].kind != 0)
						++rigidNum;
					if (list[i].kind == 2)
					{
						blendedTotalNum += list[i].gvertex->rigidNum;
						++blendedRigidNum;
					}
				}
				else
				{
					list[i].index = list[j].index;
				}
			}
		}
	}

	w_mesh* mesh = new w_mesh;
	memset(mesh, 0, sizeof(w_mesh));
	mesh->originalTexHandle = mesh->texHandle = faces[0]->texhandle;
	mesh->originalTu = mesh->tu = 0.0f;
	mesh->originalTv = mesh->tv = 0.0f;
	mesh->originalScaleu = mesh->scaleu = 1.0f;
	mesh->originalScalev = mesh->scalev = 1.0f;
	mesh->diffuse = faces[0]->diffuse;
	mesh->group = 0;
	mesh->next = 0;
	mesh->alpha = (float)((unsigned char*)&faces[0]->diffuse)[3] / 255.0f;
	mesh->flags = (mesh->alpha < 1.0f ? 0x20400000 : 0) | faces[0]->mapFlags;
	mesh->xiDrawFlag2 = faces[0]->xiMapFlags2;
	mesh->bEnv =
		faces[0]->mapType == 1 || faces[0]->mapType == 2 ? true : false;
	mesh->bSpec = faces[0]->mapType == 3;
	mesh->indexNum = cornerNum;
	mesh->indexList = new unsigned short[cornerNum];

	mesh->rigidNum = rigidNum;
	mesh->vtxColorPtrList =
		rigidNum > 0 ? (ulong**)g_mem.Alloc(rigidNum * 4) : 0;
	mesh->boneList =
		mesh->rigidNum > 0 ? (WBone**)g_mem.Alloc(mesh->rigidNum * 4) : 0;
	if (mesh->rigidNum > 0)
		memset(mesh->boneList, 0, mesh->rigidNum * 4);
	mesh->blendedRigidNum = blendedRigidNum;
	mesh->blendWeightList =
		blendedTotalNum > 0 ? (float*)g_mem.Alloc(blendedTotalNum * 4) : 0;
	mesh->blendedBoneList =
		blendedTotalNum > 0 ? (WBone**)g_mem.Alloc(blendedTotalNum * 4) : 0;
	mesh->blendedVecList =
		blendedTotalNum > 0 ? new WVector[blendedTotalNum] : 0;
	mesh->blendedNormalList =
		blendedTotalNum > 0 ? new WVector[blendedTotalNum] : 0;
	mesh->blendedTotalNum = blendedTotalNum;
	mesh->vtxNum = n;
	mesh->vecList = new WVector[mesh->vtxNum];
	mesh->normList = new WVector[mesh->vtxNum];
	mesh->vtxColorList = new ulong[mesh->vtxNum];
	mesh->originalVtxColorList = new ulong[mesh->vtxNum];
	mesh->myBlendedNormalList = 0;
	mesh->myNormalList = 0;
	mesh->weightList = 0;
	mesh->uvData = new float[mesh->vtxNum][2];
	mesh->uvBackup = faces[0]->mapType > 1 ? new float[mesh->vtxNum][2] : 0;
	for (i = 0; i < mesh->vtxNum; ++i)
	{
		mesh->originalVtxColorList[i] = mesh->vtxColorList[i] = mesh->diffuse;
	}
	AllocVTX(mesh->vtxNum);

	short* slotList = new short[mesh->vtxNum];
	for (n = 0, i = 0, j = 0, l = blendedRigidNum, k = rigidNum;
		i < mesh->indexNum; ++i)
	{
		if (list[i].index == n)
		{
			switch (list[i].kind)
			{
			case 0:
				slotList[n] = (short)k++;
				break;
			case 1:
				slotList[n] = (short)l++;
				break;
			case 2:
				slotList[n] = (short)j++;
				break;
			}
			++n;
		}
	}

	for (i = 0, n = 0, k = 0; i < mesh->indexNum; ++i)
	{
		mesh->indexList[i] = slotList[list[i].index];
		if (list[i].index != n)
			continue;
		mesh->vecList[slotList[n]] = list[i].pos;
		if (list[i].gvertex->obj == 0)
		{
			mesh->normList[slotList[n]] = list[i].normal;
		}
		else
		{
			mesh->normList[slotList[n]] = RotVec(list[i].normal,
				m_matrix * ~list[i].gvertex->obj->matrix);
		}
		mesh->normList[slotList[n]].Normalize();
		if (slotList[n] < mesh->blendedRigidNum)
		{
			for (l = 0; l < list[i].gvertex->rigidNum; ++l, ++k)
			{
				WMatrix mInv;
				mInv = ~list[i].gvertex->rigidVtx[l].obj->matrix;
				mesh->blendedBoneList[k] = list[i].gvertex->rigidVtx[l].bone;
				mesh->blendWeightList[k] =
					list[i].gvertex->rigidVtx[l].blendWeight;
				w_3d_object* object = list[i].gvertex->obj;
				mesh->blendedVecList[k] = list[i].pos * object->matrix * mInv;
				const WMatrix& boneMatrix = m_matrix;
				mesh->blendedNormalList[k] =
					RotVec(list[i].normal, boneMatrix * mInv);
				mesh->blendedNormalList[k].Normalize();
			}
		}
		if (slotList[n] < rigidNum)
		{
			mesh->boneList[slotList[n]] = list[i].bone;
			mesh->vtxColorPtrList[slotList[n]] =
				&mesh->vtxColorList[slotList[n]];
		}
		mesh->uvData[slotList[n]][0] = list[i].tu;
		mesh->uvData[slotList[n]][1] = list[i].tv;
		if (mesh->uvBackup != 0)
		{
			mesh->uvBackup[slotList[n]][0] = list[i].tu;
			mesh->uvBackup[slotList[n]][1] = list[i].tv;
		}
		++n;
	}
	CalcMeshAABB(*this, *mesh);
	delete[] slotList;
	delete[] list;
	return mesh;
}

int WBone::GetMeshForFaceAnimation(unsigned char group, w_mesh** list,
	int count)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		if (mesh->group == group)
		{
			if (list != 0)
				list[count] = mesh;
			++count;
		}
	}
	if (m_next != 0)
		count = m_next->GetMeshForFaceAnimation(group, list, count);
	if (m_child != 0)
		count = m_child->GetMeshForFaceAnimation(group, list, count);
	return count;
}

int WBone::GetMeshForFaceAnimation(int handle, w_mesh** list, int count)
{
	static int faceGroup = 1;
	w_mesh* mesh = m_mesh;
	if (mesh != 0)
	{
		int group = faceGroup;
		do
		{
			if (mesh->originalTexHandle == handle || handle == 0)
			{
				if (list != 0)
				{
					list[count] = mesh;
					if (mesh->group == 0 && handle != 0)
					{
						if (count == 0)
							++group;
						mesh->group = group;
					}
				}
				++count;
			}
			mesh = mesh->next;
		} while (mesh != 0);
		faceGroup = group;
	}
	if (m_next != 0)
		count = m_next->GetMeshForFaceAnimation(handle, list, count);
	if (m_child != 0)
		count = m_child->GetMeshForFaceAnimation(handle, list, count);
	return count;
}

void WBone::CombineVtxColor(bool child)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		int i;
		for (i = 0; i < mesh->blendedRigidNum; ++i)
			mesh->vtxColorPtrList[i] = &mesh->vtxColorList[i];
		for (; i < mesh->rigidNum; ++i)
		{
			ulong* color = (ulong*)mesh->boneList[i]->FindVColor(
				mesh->vecList[i], mesh->normList[i]);
			if (color == 0)
				color = &mesh->vtxColorList[i];
			mesh->vtxColorPtrList[i] = color;
		}
	}
	if (child)
	{
		if (m_next != 0)
			m_next->CombineVtxColor(true);
		if (m_child != 0)
			m_child->CombineVtxColor(true);
	}
}

ulong* WBone::FindVColor(const WVector& pos, const WVector& normal)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		for (int i = mesh->rigidNum; i < mesh->vtxNum; ++i)
		{
			if (WisEqual(mesh->vecList[i], pos, 1.0e-5f) &&
				WisEqual(mesh->normList[i], normal, 1.0e-5f))
				return (ulong*)&mesh->vtxColorList[i];
		}
	}
	return 0;
}

void WBone::Transform(const WMatrix& mat, WView* view, float matscale, int flag)
{
	WSphere tr_bound_sphere;
	float scale;
	if (!(flag & NOUPDATEMAT))
	{
		if (m_flag.GetFlag(ROTATEBONE | MOVEBONE | SCALEBONE))
		{
			if (m_flag.GetFlag(ROTATEBONE))
				m_matrix = m_rotateMat * m_localMat;
			else
				m_matrix = m_localMat;
			if (m_flag.GetFlag(SCALEBONE))
				m_matrix.AxisScale(
					WVector(m_bonescale, m_bonescale, m_bonescale));
			if (m_flag.GetFlag(MOVEBONE))
				m_matrix.pivot -= m_moveVec;
			m_matrix = m_matrix * mat;
		}
		else
		{
			m_matrix = m_localMat * mat;
		}
	}
	if (view)
	{
		scale = matscale * m_scale;
		tr_bound_sphere.pos = Transform(m_bound_sphere.pos);
		scale *= m_bound_sphere.radius;
		tr_bound_sphere = WSphere(tr_bound_sphere.pos, scale);
		bool inside = view->InFrustum(tr_bound_sphere) == true ||
			WisZero(tr_bound_sphere.radius, g_EPSILON);
		m_flag.Turn(VISIBLE,
			m_flag.GetFlag(HASRIGIDVTX) || (flag & NOCLIP) || inside);
		if (m_flag.GetFlag(HASRIGIDVTX))
			m_flag.Turn(CLIPPING, !(flag & NOCLIP));
		else
			m_flag.Turn(CLIPPING,
				!(flag & NOCLIP) &&
					(!IsVisible() || !view->InFrustumSafe(tr_bound_sphere)));
	}
	if (m_next)
		m_next->Transform(mat, view, matscale, flag);
	if (m_child)
		m_child->Transform(m_matrix, view, matscale * m_scale, flag);
}

void WBone::DisableFog(void)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
		mesh->drawFlag &= 0xf7ffffffUL;
}

void WBone::EnableAlphaBlend(void)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		if ((mesh->drawFlag & 0x80000000UL) != 0)
			mesh->drawFlag = mesh->drawFlag & 0x7fbfffffUL | 0x00400000UL;
	}
}

void WBone::SetRenderMode(int mode)
{
	m_rendMode = mode;
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		switch (mode)
		{
		case 0:
			mesh->drawFlag =
				(((mesh->flags & 0x1800000) != 0x800000) ? 0x8000000 : 0) |
				(mesh->texHandle & 0x3ffff) | mesh->flags;
			break;
		case 1:
			mesh->drawFlag =
				(mesh->texHandle & 0x3ffff) | mesh->flags | 0x40000000;
			break;
		case 5:
			mesh->drawFlag =
				(((mesh->flags & 0x1800000) != 0x800000) ? 0x8000000 : 0) |
				(mesh->texHandle & 0x3ffff) | mesh->flags | 0x40000;
			break;
		case 2:
			mesh->drawFlag = mesh->flags;
			break;
		case 3:
			mesh->drawFlag = 0x4000000;
			break;
		case 4:
			mesh->drawFlag = (mesh->texHandle & 0x3ffff) | 0x20300000;
			break;
		}
	}
	if (m_next != 0)
		m_next->SetRenderMode(mode);
	if (m_child != 0)
		m_child->SetRenderMode(mode);
}

void WBone::SetAlpha(unsigned char alpha, int recurse)
{
	if (m_alpha != alpha)
	{
		m_alpha = alpha;
		if (alpha == 0)
			m_flag.Enable(1);
		if ((((ulong)m_flag) & 1) != 0 && alpha > 0)
			m_flag.Disable(1);
		for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
		{
			ulong color = (ulong)(int)((float)alpha * mesh->alpha) << 24;
			if (color == 0xff000000)
			{
				int flag = mesh->drawFlag;
				if ((flag & 0x1800000) == 0 && (mesh->flags & 0x400000) == 0)
					flag &= 0xffbfffff;
				else
					flag |= 0x400000;
				flag &= 0xdfffffff;
				mesh->drawFlag = flag;
			}
			else
			{
				mesh->drawFlag |= 0x20400000;
			}
			for (int i = 0; i < mesh->vtxNum; ++i)
			{
				ulong& vtxColor = mesh->vtxColorList[i];
				vtxColor = (vtxColor & 0xffffff) | color;
			}
		}
	}
	if (recurse != 0)
	{
		if (m_next != 0)
			m_next->SetAlpha(alpha, 1);
		if (m_child != 0)
			m_child->SetAlpha(alpha, 1);
	}
}

void WBone::RestoreVtxColor(void)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
		for (int index = 0; index < mesh->vtxNum; ++index)
			mesh->vtxColorList[index] = mesh->originalVtxColorList[index];
}

void WBone::SetLightLine(float value)
{
	WBone* bone = this;
	do
	{
		bone->m_lightLine = value;
		if (bone->m_next != 0)
			bone->m_next->SetLightLine(value);
		bone = bone->m_child;
	} while (bone != 0);
}

void WBone::SetLight(LightSet* light)
{
	memcpy(&m_light, light, sizeof(m_light));
	if (m_next != 0)
		m_next->SetLight(light);
	if (m_child != 0)
		m_child->SetLight(light);
}

void WBone::CalcLight(WScene* scene, int flag)
{
	bool selfIllum = ((((ulong)m_flag) >> 1) & 1) != 0;
	if (!selfIllum && scene != 0)
	{
		WVector pos = m_bound_sphere.pos * m_matrix;
		CalcLight(scene->GetLightSet(pos), false, flag, 0);
	}
	else
	{
		int mode = flag | 0x4000000;
		if ((mode & 0x4000000) != 0 || selfIllum)
			CalcLight_SelfIllum();
		else
			CalcLight_Diffuse_Equals_Ambient(0, mode);
	}
	if (m_next != 0)
		m_next->CalcLight(scene, flag);
	if (m_child != 0)
		m_child->CalcLight(scene, flag);
}

void WBone::CalcLight(LightSet* light, bool child, int flag, WScene* scene)
{
	bool selfIllum = ((((ulong)m_flag) >> 1) & 1) != 0;
	if ((flag & 0x4000000) == 0 && !selfIllum)
	{
		if (light != 0)
		{
			if (light->type == 1)
			{
				if (flag == 0x1000000)
					CalcLight_Point(light, flag, scene);
				else
					CalcLight_Directional_Per_Bone(light, flag, scene);
			}
			else if (light->type == 2)
			{
				CalcLight_Directional(light, flag, scene);
			}
		}
		else
		{
			CalcLight_Diffuse_Equals_Ambient(0, flag);
		}
	}
	else
	{
		CalcLight_SelfIllum();
	}
	if (child)
	{
		if (m_next != 0)
			m_next->CalcLight(light, child, flag, scene);
		if (m_child != 0)
			m_child->CalcLight(light, child, flag, scene);
	}
}

void WBone::CalcLight_SelfIllum(void)
{
	w_mesh* mesh = m_mesh;
	if (mesh != 0)
	{
		do
		{
			ulong alpha = (ulong)(int)((float)m_alpha * mesh->alpha) << 24;
			ulong color;
			if (mesh->texHandle == 0)
				color = Modulate(m_selfillumColor, mesh->diffuse);
			else
				color = m_selfillumColor;
			color |= alpha;
			for (int i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] = color;
			mesh = mesh->next;
		} while (mesh != 0);
	}
	if (m_vtxColor != 0xffffffff)
	{
		for (w_mesh* target = m_mesh; target != 0; target = target->next)
		{
			if ((target->drawFlag & 0x1000000) == 0)
			{
				for (int i = 0; i < target->vtxNum; ++i)
					target->vtxColorList[i] &= m_vtxColor;
			}
		}
	}
}

void WBone::CalcLight_Point(LightSet* lig, int lightmode, WScene* scene)
{
	w_mesh* mesh = m_mesh;
	if (mesh == 0)
		return;

	ulong diffuse, ambient, calcAlpha, c;
	WVector vLocLightPos, vLightDir;
	float t;
	int i, k;
	vLocLightPos = lig->nearone * ~m_matrix;

	for (; mesh != 0; mesh = mesh->next)
	{
		if (mesh->texHandle == 0)
		{
			diffuse = Modulate(lig->diffuse, mesh->diffuse);
			ambient = Modulate(lig->ambient, mesh->diffuse);
		}
		else
		{
			diffuse = lig->diffuse;
			ambient = lig->ambient;
		}
		calcAlpha = (ulong)(int)(m_alpha * mesh->alpha) << 24;

		if ((mesh->xiDrawFlag2 & 0x4) != 0)
		{
			for (i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] = lig->ambient2 | calcAlpha;
			continue;
		}
		if ((mesh->xiDrawFlag2 & 0x10) != 0)
		{
			for (i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] = calcAlpha | 0xffffff;
			continue;
		}
		if ((lightmode & 0x2000000) != 0)
			continue;

		k = 0;
		for (i = 0; i < mesh->blendedRigidNum; ++i)
		{
			WVector v, vN;
			vN.x = vN.y = vN.z = 0.0f;
			v.x = v.y = v.z = 0.0f;
			float fTotal = 0.0f;
			for (; fTotal < 0.999f; ++k)
			{
				v += mesh->blendedBoneList[k]->Transform(
						 mesh->blendedVecList[k]) *
					mesh->blendWeightList[k];
				vN += RotVec(mesh->blendedNormalList[k],
						  mesh->blendedBoneList[k]->GetMatrix()) *
					mesh->blendWeightList[k];
				fTotal += mesh->blendWeightList[k];
			}
			vN.Normalize();
			vLightDir = (lig->nearone - v).Normalize();
			t = (float)(vLightDir * vN) * 255.0f;
			if ((mesh->xiDrawFlag2 & 0x400) != 0 && t < 0.0f)
				t = t * -1.0f;
			c = t <= 0.0f ? ambient
						  : AddDiffuse(ambient, diffuse, (unsigned char)t);
			if ((lightmode & 0x8000000) != 0)
			{
				mesh->vtxColorList[i] =
					AddDiffuse(mesh->vtxColorList[i] & 0xffffff, c) | calcAlpha;
			}
			else
			{
				mesh->vtxColorList[i] = c | calcAlpha;
			}
		}

		for (; i < mesh->rigidNum; ++i)
		{
			WVector v, vN;
			v = mesh->boneList[i]->Transform(mesh->vecList[i]);
			vN = RotVec(mesh->normList[i], mesh->boneList[i]->GetMatrix());
			vN.Normalize();
			vLightDir = (lig->nearone - v).Normalize();
			t = vLightDir * vN * 255.0f;
			if ((mesh->xiDrawFlag2 & 0x400) != 0 && t < 0.0f)
				t = t * -1.0f;
			c = t <= 0.0f ? ambient
						  : AddDiffuse(ambient, diffuse, (unsigned char)t);
			if ((lightmode & 0x8000000) != 0)
			{
				mesh->vtxColorList[i] =
					AddDiffuse(mesh->vtxColorList[i] & 0xffffff, c) | calcAlpha;
			}
			else
			{
				mesh->vtxColorList[i] = c | calcAlpha;
			}
		}

		for (; i < mesh->vtxNum; ++i)
		{
			vLightDir = (vLocLightPos - mesh->vecList[i]).Normalize();
			t = vLightDir * mesh->normList[i] * 255.0f;
			if ((mesh->xiDrawFlag2 & 0x400) != 0 && t < 0.0f)
				t = t * -1.0f;
			c = t <= 0.0f ? ambient
						  : AddDiffuse(ambient, diffuse, (unsigned char)t);
			if ((lightmode & 0x8000000) != 0)
			{
				mesh->vtxColorList[i] =
					AddDiffuse(mesh->vtxColorList[i] & 0xffffff, c) | calcAlpha;
			}
			else
			{
				mesh->vtxColorList[i] = c | calcAlpha;
			}
		}
	}

	if (m_vtxColor != 0xffffffff)
	{
		for (mesh = m_mesh; mesh != 0; mesh = mesh->next)
		{
			if ((mesh->drawFlag & 0x1000000) != 0)
				continue;
			for (i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] &= m_vtxColor;
		}
	}
}

void WBone::CalcLight_Directional_Per_Bone(LightSet* lig, int lightmode,
	WScene* scene)
{
	if (m_mesh == 0)
		return;

	float t;
	ulong diffuse, ambient, calcAlpha;
	WVector vWorLightDir;
	ulong c;
	int i, k;
	w_mesh* mesh;
	WVector vLocLightDir;
	if (m_flag.GetFlag(4))
	{
		vWorLightDir = lig->nearone - m_matrix.pivot;
		vWorLightDir = vWorLightDir.Normalize() * 255.0f;
		vLocLightDir = RotVec(vWorLightDir, ~m_matrix);
		vLocLightDir = vLocLightDir.Normalize() * 255.0f;
	}
	else
	{
		vLocLightDir = RotVec(lig->nearone - m_matrix.pivot, ~m_matrix);
		vLocLightDir.Normalize();
	}

	for (mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		if (mesh->texHandle == 0)
		{
			diffuse = Modulate(lig->diffuse, mesh->diffuse);
			ambient = Modulate(lig->ambient, mesh->diffuse);
		}
		else
		{
			diffuse = lig->diffuse;
			ambient = lig->ambient;
		}
		calcAlpha = (ulong)(int)(m_alpha * mesh->alpha) << 24;

		if ((mesh->xiDrawFlag2 & 0x4) != 0)
		{
			for (i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] = lig->ambient2 | calcAlpha;
			continue;
		}
		if ((mesh->xiDrawFlag2 & 0x10) != 0)
		{
			for (i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] = calcAlpha | 0xffffff;
			continue;
		}
		if ((lightmode & 0x2000000) != 0)
			continue;

		for (k = 0, i = 0; i < mesh->blendedRigidNum; ++i)
		{
			WVector vN;
			float fTotal;
			vN.x = vN.y = vN.z = 0.0f;
			for (fTotal = 0.0f; fTotal < 0.999f; ++k)
			{
				vN += RotVec(mesh->blendedNormalList[k],
						  mesh->blendedBoneList[k]->m_matrix) *
					mesh->blendWeightList[k];
				fTotal += mesh->blendWeightList[k];
			}
			vN.Normalize();
			t = (float)(vN * vWorLightDir);
			if ((mesh->xiDrawFlag2 & 0x400) != 0 && t < 0.0f)
				t = t * -1.0f;
			c = t <= 0.0f ? ambient
						  : AddDiffuse(ambient, diffuse, (unsigned char)t);
			if ((lightmode & 0x8000000) != 0)
			{
				mesh->vtxColorList[i] =
					AddDiffuse(mesh->vtxColorList[i] & 0xffffff, c) | calcAlpha;
			}
			else
			{
				mesh->vtxColorList[i] = c | calcAlpha;
			}
		}

		for (; i < mesh->rigidNum; ++i)
		{
			WVector vN;
			vN = RotVec(mesh->normList[i], mesh->boneList[i]->GetMatrix());
			vN.Normalize();
			t = (float)(vN * vWorLightDir);
			if ((mesh->xiDrawFlag2 & 0x400) != 0 && t < 0.0f)
				t = t * -1.0f;
			c = t <= 0.0f ? ambient
						  : AddDiffuse(ambient, diffuse, (unsigned char)t);
			if ((lightmode & 0x8000000) != 0)
			{
				mesh->vtxColorList[i] =
					AddDiffuse(mesh->vtxColorList[i] & 0xffffff, c) | calcAlpha;
			}
			else
			{
				mesh->vtxColorList[i] = c | calcAlpha;
			}
		}

		for (; i < mesh->vtxNum; ++i)
		{
			t = (float)(vLocLightDir * mesh->normList[i]);
			if ((mesh->xiDrawFlag2 & 0x400) != 0 && t < 0.0f)
				t = t * -1.0f;
			c = t <= 0.0f ? ambient
						  : AddDiffuse(ambient, diffuse, (unsigned char)t);
			if ((lightmode & 0x8000000) != 0)
			{
				mesh->vtxColorList[i] =
					AddDiffuse(mesh->vtxColorList[i] & 0xffffff, c) | calcAlpha;
			}
			else
			{
				mesh->vtxColorList[i] = c | calcAlpha;
			}
		}
	}

	if (m_vtxColor != 0xffffffff)
	{
		for (mesh = m_mesh; mesh != 0; mesh = mesh->next)
		{
			if ((mesh->drawFlag & 0x1000000) != 0)
				continue;
			for (i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] &= m_vtxColor;
		}
	}
}

void WBone::CalcLight_Directional(LightSet* lig, int lightmode, WScene* scene)
{
	if (m_mesh == 0)
		return;

	ulong diffuse;
	float t;
	WVector vWorLightDir;
	ulong ambient;
	WVector vLocLightDir;
	ulong calcAlpha;
	w_mesh* mesh;
	int i, k;
	ulong c;
	if (m_flag.GetFlag(4))
		vWorLightDir = lig->nearone * -255.0f;

	vLocLightDir = RotVec(lig->nearone * -1.0f, ~m_matrix);
	vLocLightDir = vLocLightDir.Normalize() * 255.0f;

	for (mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		WVector* normList;
		WVector vN;
		if (mesh->texHandle == 0)
		{
			diffuse = Modulate(lig->diffuse, mesh->diffuse);
			ambient = Modulate(lig->ambient, mesh->diffuse);
		}
		else
		{
			diffuse = lig->diffuse;
			ambient = lig->ambient;
		}
		calcAlpha = (ulong)(int)(m_alpha * mesh->alpha) << 24;

		if ((mesh->xiDrawFlag2 & 0x4) != 0)
		{
			for (i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] = lig->ambient2 | calcAlpha;
			continue;
		}
		if ((mesh->xiDrawFlag2 & 0x10) != 0)
		{
			for (i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] = calcAlpha | 0xffffff;
			continue;
		}
		if ((lightmode & 0x2000000) != 0)
			continue;

		normList = mesh->myBlendedNormalList ? mesh->myBlendedNormalList
											 : mesh->blendedNormalList;
		if (normList != 0)
		{
			float fTotal;
			for (k = 0, i = 0; i < mesh->blendedRigidNum; ++i)
			{
				vN.Reset();
				for (fTotal = 0.0f;
					fTotal < 0.999999f && k < mesh->blendedTotalNum; ++k)
				{
					WVector tmp = RotVec(normList[k],
						mesh->blendedBoneList[k]->GetMatrix());
					float w = mesh->blendWeightList[k];
					float tz = tmp.z * w;
					float ty = tmp.y * w;
					float tx = tmp.x * w;
					vN += WVector(tx, ty, tz);
					fTotal += mesh->blendWeightList[k];
				}
				vN.Normalize();
				t = (float)(vN.x * vWorLightDir.x + vN.z * vWorLightDir.z +
					vN.y * vWorLightDir.y);
				if ((mesh->xiDrawFlag2 & 0x400) != 0 && t < 0.0f)
					t = t * -1.0f;
				c = t <= 0.0f ? ambient
							  : AddDiffuse(ambient, diffuse, (unsigned char)t);
				if ((lightmode & 0x8000000) != 0)
				{
					mesh->vtxColorList[i] =
						AddDiffuse(mesh->vtxColorList[i] & 0xffffff, c) |
						calcAlpha;
				}
				else
				{
					mesh->vtxColorList[i] = c | calcAlpha;
				}
			}
		}
		else
		{
			i = mesh->blendedRigidNum;
		}

		normList = mesh->myNormalList ? mesh->myNormalList : mesh->normList;
		if (normList == 0)
			continue;

		for (; i < mesh->rigidNum; ++i)
		{
			if (&mesh->vtxColorList[i] != mesh->vtxColorPtrList[i])
				continue;
			vN = RotVec(normList[i], mesh->boneList[i]->GetMatrix());
			vN.Normalize();
			t = (float)(vN.x * vWorLightDir.x + vN.z * vWorLightDir.z +
				vN.y * vWorLightDir.y);
			if ((mesh->xiDrawFlag2 & 0x400) != 0 && t < 0.0f)
				t = t * -1.0f;
			c = t <= 0.0f ? ambient
						  : AddDiffuse(ambient, diffuse, (unsigned char)t);
			if ((lightmode & 0x8000000) != 0)
			{
				mesh->vtxColorList[i] =
					AddDiffuse(mesh->vtxColorList[i] & 0xffffff, c) | calcAlpha;
			}
			else
			{
				mesh->vtxColorList[i] = c | calcAlpha;
			}
		}

		for (; i < mesh->vtxNum; ++i)
		{
			t = vLocLightDir * normList[i];
			if ((mesh->xiDrawFlag2 & 0x400) != 0 && t < 0.0f)
				t = t * -1.0f;
			c = t <= 0.0f ? ambient
						  : AddDiffuse(ambient, diffuse, (unsigned char)t);
			if ((lightmode & 0x8000000) != 0)
			{
				mesh->vtxColorList[i] =
					AddDiffuse(mesh->vtxColorList[i] & 0xffffff, c) | calcAlpha;
			}
			else
			{
				mesh->vtxColorList[i] = c | calcAlpha;
			}
		}
	}

	if (m_vtxColor != 0xffffffff)
	{
		for (mesh = m_mesh; mesh != 0; mesh = mesh->next)
		{
			if ((mesh->drawFlag & 0x1000000) != 0)
				continue;
			for (i = 0; i < mesh->vtxNum; ++i)
				mesh->vtxColorList[i] &= m_vtxColor;
		}
	}
}

void WBone::CalcLight_Diffuse_Equals_Ambient(LightSet* light, int flag)
{
	w_mesh* mesh = m_mesh;
	if (mesh != 0)
	{
		int add = flag & 0x8000000;
		do
		{
			ulong color;
			if (light != 0)
			{
				if (mesh->texHandle == 0)
					color = Modulate(light->diffuse, mesh->diffuse);
				else
					color = light->diffuse;
			}
			else
			{
				color = 0;
			}
			ulong value =
				((ulong)(int)((float)m_alpha * mesh->alpha) << 24) | color;
			int i;
			if (add != 0)
			{
				for (i = 0; i < mesh->vtxNum; ++i)
					mesh->vtxColorList[i] =
						AddDiffuse(mesh->vtxColorList[i], value);
			}
			else
			{
				for (i = 0; i < mesh->vtxNum; ++i)
					mesh->vtxColorList[i] = value;
			}
			mesh = mesh->next;
		} while (mesh != 0);
	}
	if (m_vtxColor != 0xffffffff)
	{
		for (w_mesh* target = m_mesh; target != 0; target = target->next)
		{
			if ((target->drawFlag & 0x1000000) == 0)
			{
				for (int i = 0; i < target->vtxNum; ++i)
					target->vtxColorList[i] &= m_vtxColor;
			}
		}
	}
}

void WBone::ResetLight(void)
{
	WBone* bone = this;
	for (;;)
	{
		for (w_mesh* mesh = bone->m_mesh; mesh != 0; mesh = mesh->next)
			for (int index = 0; index < mesh->vtxNum; ++index)
				mesh->vtxColorList[index] = mesh->diffuse;
		if (bone->m_next != 0)
			bone->m_next->ResetLight();
		WBone* child = bone->m_child;
		if (child == 0)
			break;
		bone = child;
	}
}

void WBone::SetVtxColor(ulong color)
{
	m_vtxColor = color;
	RestoreVtxColor();
	if (m_next != 0)
		m_next->SetVtxColor(color);
	if (m_child != 0)
		m_child->SetVtxColor(color);
}

void WBone::CalcENVCoord(WView* view, w_mesh* m)
{
	int i;
	WMatrix trans;
	WVector norm, vb, va;
	if (m->uvBackup)
	{
		for (i = 0; i < m->vtxNum; ++i)
		{
			va = m->vecList[i] * m_matrix - view->camera.pivot;
			vb = RotVec(m->normList[i], m_matrix);
			norm =
				va - vb * (2 * (va * vb) / (vb.Magnitude() * vb.Magnitude()));
			norm *= 0.5f / norm.Magnitude();
			m->uvData[i][0] = 0.5f - norm.x;
			m->uvData[i][1] = 0.5f - norm.y;
		}
	}
	else
	{
		trans = m_matrix * view->invcamera;
		trans *= 0.5f / WVectorLen(trans.xa);
		for (i = 0; i < m->vtxNum; ++i)
		{
			norm = RotVec(m->normList[i], trans);
			m->uvData[i][0] = norm.x + 0.5f;
			m->uvData[i][1] = 0.5f - norm.y;
		}
	}
}

void WBone::CalcHilightCoord(WView* view, const LightSet& light, w_mesh* mesh)
{
	int i = 0;
	ulong type;
	if (!(type = light.type))
		return;
	WMatrix mInv;
	mInv = ~m_matrix;
	if (type == 1)
		return;
	if (type != 2)
		return;
	{
		WVector vLocalL, vXA, vYA, vZA, vN, v, vLocalE;
		WVector* normList;
		normList = mesh->myBlendedNormalList ? mesh->myBlendedNormalList
											 : mesh->blendedNormalList;
		vLocalE = view->GetCamera().pivot;
		vLocalL = light.nearone;
		vLocalL.Normalize();
		const WVector* direction = &light.nearone;
		const WVector* camera = &view->GetCamera().pivot;
		int k = 0;
		for (i = 0; i < mesh->blendedRigidNum; ++i)
		{
			float fTotal;
			vN.Reset();
			v.Reset();
			for (fTotal = 0.0f; fTotal < 0.999999f; ++k)
			{
				vN +=
					RotVec(normList[k], mesh->blendedBoneList[k]->GetMatrix()) *
					mesh->blendWeightList[k];
				v += (mesh->blendedVecList[k] *
						 mesh->blendedBoneList[k]->GetMatrix()) *
					mesh->blendWeightList[k];
				fTotal += mesh->blendWeightList[k];
			}
			vN.Normalize();
			if ((float)(vLocalL * vN) >= 0.0f)
			{
				mesh->uvBackup[i][0] = mesh->uvBackup[i][1] = 0.75f;
			}
			else
			{
				vZA = (vLocalL + (v - vLocalE).Normalize()).Normalize();
				if ((float)(vZA * vN) >= 0.0f)
				{
					mesh->uvBackup[i][0] = mesh->uvBackup[i][1] = 0.75f;
				}
				else
				{
					if (WisEqual(vZA.y, -1.0f, g_EPSILON) ||
						WisEqual(vZA.y, 1.0f, g_EPSILON))
					{
						vXA =
							WCrossProduct(WVector::UNIT_POS_X, vZA).Normalize();
						vYA = WCrossProduct(vZA, vXA).Normalize();
					}
					else
					{
						vXA =
							WCrossProduct(WVector::UNIT_POS_Y, vZA).Normalize();
						vYA = WCrossProduct(vZA, vXA).Normalize();
					}
					mesh->uvBackup[i][0] = Wabs(vXA * vN);
					mesh->uvBackup[i][1] = Wabs(vYA * vN);
				}
			}
		}
		(void)(vLocalL * vN);
		for (; i < mesh->rigidNum; ++i)
		{
			vLocalL =
				RotVec(*direction, ~mesh->boneList[i]->GetMatrix()).Normalize();
			if (vLocalL * mesh->normList[i] >= 0.0f)
			{
				mesh->uvBackup[i][0] = mesh->uvBackup[i][1] = 0.75f;
			}
			else
			{
				vLocalE = *camera * ~mesh->boneList[i]->GetMatrix();
				vZA = (vLocalL + (mesh->vecList[i] - vLocalE).Normalize())
						  .Normalize();
				if (vZA * mesh->normList[i] >= 0.0f)
				{
					mesh->uvBackup[i][0] = mesh->uvBackup[i][1] = 0.75f;
				}
				else
				{
					if (WisEqual(vZA.y, -1.0f, g_EPSILON) ||
						WisEqual(vZA.y, 1.0f, g_EPSILON))
					{
						vXA =
							WCrossProduct(WVector::UNIT_POS_X, vZA).Normalize();
						vYA = WCrossProduct(vZA, vXA).Normalize();
					}
					else
					{
						vXA =
							WCrossProduct(WVector::UNIT_POS_Y, vZA).Normalize();
						vYA = WCrossProduct(vZA, vXA).Normalize();
					}
					mesh->uvBackup[i][0] = Wabs(vXA * mesh->normList[i]);
					mesh->uvBackup[i][1] = Wabs(vYA * mesh->normList[i]);
				}
			}
		}
		vLocalE = *camera * mInv;
		vLocalL = RotVec(*direction, mInv).Normalize();
		for (; i < mesh->vtxNum; ++i)
		{
			if ((float)(vLocalL * mesh->normList[i]) >= 0.0f)
			{
				mesh->uvBackup[i][0] = mesh->uvBackup[i][1] = 0.75f;
			}
			else
			{
				vZA = (vLocalL + (mesh->vecList[i] - vLocalE).Normalize())
						  .Normalize();
				if (vZA * mesh->normList[i] >= 0.0f)
				{
					mesh->uvBackup[i][0] = mesh->uvBackup[i][1] = 0.75f;
				}
				else
				{
					if (WisEqual(vZA.y, -1.0f, g_EPSILON) ||
						WisEqual(vZA.y, 1.0f, g_EPSILON))
					{
						vXA =
							WCrossProduct(WVector::UNIT_POS_X, vZA).Normalize();
						vYA = WCrossProduct(vZA, vXA).Normalize();
					}
					else
					{
						vXA =
							WCrossProduct(WVector::UNIT_POS_Y, vZA).Normalize();
						vYA = WCrossProduct(vZA, vXA).Normalize();
					}
					mesh->uvBackup[i][0] = Wabs(vXA * mesh->normList[i]);
					mesh->uvBackup[i][1] = Wabs(vYA * mesh->normList[i]);
				}
			}
		}
	}
}

void RenderAABB(WView* view, const Waabb& aabb, const WMatrix& mat, float rlen)
{
	static int box_v[8][3] = {
		{ 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 0, 0, 1 },
		{ 1, 0, 1 },
        { 0, 1, 1 },
        { 1, 1, 1 }
	};
	static int box_p[6][4] = {
		{ 0, 2, 3, 1 },
        { 4, 5, 7, 6 },
        { 0, 1, 5, 4 },
        { 3, 2, 6, 7 },
		{ 1, 3, 7, 5 },
        { 2, 0, 4, 6 }
	};
	WVector vt[8];
	for (int i = 0; i < 8; ++i)
		vt[i] = WVector(box_v[i][0] ? (aabb.max.x + rlen) : (aabb.min.x - rlen),
					box_v[i][1] ? (aabb.max.y + rlen) : (aabb.min.y - rlen),
					box_v[i][2] ? (aabb.max.z + rlen) : aabb.min.z - rlen) *
			mat;
	for (int f = 0; f < 6; ++f)
		for (int k = 0; k < 4; ++k)
			view->DrawLine(vt[box_p[f][(k - 1) & 3]], 0xffff0000,
				vt[box_p[f][k]], 0xffff0000, 0);
}

static void DebugTest(WView* view, const WVector& pos, ulong diffuse,
	float rlen)
{
	view->DrawLine(pos - WVector(rlen, 0.0f, 0.0f), diffuse,
		pos + WVector(rlen, 0.0f, 0.0f), diffuse, 0);
	view->DrawLine(pos - WVector(0.0f, rlen, 0.0f), diffuse,
		pos + WVector(0.0f, rlen, 0.0f), diffuse, 0);
	view->DrawLine(pos - WVector(0.0f, 0.0f, rlen), diffuse,
		pos + WVector(0.0f, 0.0f, rlen), diffuse, 0);
}

void WBone::Render(WView* view, const LightSet& light)
{
	WVector vWorLightDir, vLocLightDir, vLocEyePos;
	int i, k, n;
	float t, w;
	w_mesh* f;
	WVector vec, v;
	WTVertex* pv;
	int d0 = 0;

	if (IsVisible() && !m_flag.GetFlag(INVISIBLE) && !IsHidden())
	{
		vWorLightDir = light.nearone * -1.0f;
		vLocLightDir = RotVec(vWorLightDir, ~GetMatrix());
		vLocEyePos = view->GetCamera().pivot * ~GetMatrix();

		for (f = m_mesh, n = 0; f != 0; f = f->next, ++n)
		{
			if (f->alpha == 0.0f ||
				(view->IsShadowView() && (f->drawFlag & 0x300000) == 0x100000))
				continue;
			if (view->ProcessEffect())
			{
				if (f->bEnv)
					CalcENVCoord(view, f);
				if (f->bSpec)
					CalcHilightCoord(view, light, f);
			}

			i = 0;
			if (f->uvBackup != 0)
			{
				k = 0;
				if (f->blendedVecList != 0)
				{
					for (; i < f->blendedRigidNum; ++i)
					{
						t = 0.0f;
						w = f->blendWeightList[k];
						vec = f->blendedBoneList[k]->Transform(
								  f->blendedVecList[k]) *
							w;
						t += w;
						for (++k; t < 0.999999f && k < f->blendedTotalNum; ++k)
						{
							WBone*& bone = f->blendedBoneList[k];
							w = f->blendWeightList[k];
							vec += bone->Transform(f->blendedVecList[k]) * w;
							t += w;
						}
						pv = GetVtxBuff(i);
						pv->tu = f->uvData[i][0];
						pv->tv = f->uvData[i][1];
						pv->lu = f->uvBackup[i][0];
						pv->lv = f->uvBackup[i][1];
						pv->diffuse = *f->vtxColorPtrList[i];
						pv->SetPosition(vec);
					}
				}
				for (; i < f->rigidNum; ++i)
				{
					pv = GetVtxBuff(i);
					pv->tu = f->uvData[i][0];
					pv->tv = f->uvData[i][1];
					pv->lu = f->uvBackup[i][0];
					pv->lv = f->uvBackup[i][1];
					pv->diffuse = *f->vtxColorPtrList[i];
					pv->SetPosition(f->boneList[i]->Transform(f->vecList[i]));
				}
				if (f->vtxNum - i > 0)
				{
					for (; i < f->vtxNum; ++i)
					{
						pv = GetVtxBuff(i);
						pv->diffuse = f->vtxColorList[i];
						pv->tu = f->uvData[i][0];
						pv->tv = f->uvData[i][1];
						pv->lu = f->uvBackup[i][0];
						pv->lv = f->uvBackup[i][1];
					}
					WTVertex* vertices;
					if (m_flag.GetFlag(PHYSICSMODEL))
					{
						for (k = f->rigidNum; k < f->vtxNum; ++k)
							DebugTest(view, m_physic->list[n][k], 0xffff0000,
								1.5999999f);
						vertices = GetVtxBuff();
						for (i = f->rigidNum; i < f->vtxNum; ++i)
						{
							vertices[i].SetPosition(m_physic->list[n][i]);
						}
					}
					else
					{
						vertices = GetVtxBuff();
						for (i = f->rigidNum; i < f->vtxNum; ++i)
						{
							vertices[i].SetPosition(f->vecList[i] * m_matrix);
						}
					}
				}
			}
			else
			{
				k = 0;
				if (f->blendedVecList != 0)
				{
					for (; i < f->blendedRigidNum; ++i)
					{
						t = 0.0f;
						w = f->blendWeightList[k];
						vec = f->blendedBoneList[k]->Transform(
								  f->blendedVecList[k]) *
							w;
						t += w;
						for (++k; t < 0.999999f && k < f->blendedTotalNum; ++k)
						{
							WBone*& bone = f->blendedBoneList[k];
							w = f->blendWeightList[k];
							vec += bone->Transform(f->blendedVecList[k]) * w;
							t += w;
						}
						pv = GetVtxBuff(i);
						pv->tu = f->uvData[i][0];
						pv->tv = f->uvData[i][1];
						pv->diffuse = *f->vtxColorPtrList[i];
						pv->SetPosition(vec);
					}
				}
				for (; i < f->rigidNum; ++i)
				{
					pv = GetVtxBuff(i);
					pv->tu = f->uvData[i][0];
					pv->tv = f->uvData[i][1];
					pv->diffuse = *f->vtxColorPtrList[i];
					pv->SetPosition(f->boneList[i]->Transform(f->vecList[i]));
				}
				if (f->vtxNum - i > 0)
				{
					for (; i < f->vtxNum; ++i)
					{
						pv = GetVtxBuff(i);
						pv->diffuse = f->vtxColorList[i];
						pv->tu = f->uvData[i][0];
						pv->tv = f->uvData[i][1];
					}
					WTVertex* vertices;
					if (m_flag.GetFlag(PHYSICSMODEL))
					{
						for (k = f->rigidNum; k < f->vtxNum; ++k)
							DebugTest(view, m_physic->list[n][k], 0xffff0000,
								1.5999999f);
						vertices = GetVtxBuff();
						for (k = f->rigidNum; k < f->vtxNum; ++k)
						{
							vertices[k].SetPosition(m_physic->list[n][k]);
						}
					}
					else
					{
						vertices = GetVtxBuff();
						for (i = f->rigidNum; i < f->vtxNum; ++i)
						{
							vertices[i].SetPosition(f->vecList[i] * m_matrix);
						}
					}
				}
			}

			if (!view->IsShadowView() && (f->xiDrawFlag2 & 0x400) != 0)
			{
				if ((f->xiDrawFlag2 & 0x14) == 0)
				{
					WVector pos, normal;
					WVector* normList;
					float lightDot, eyeDot;
					normList = f->myBlendedNormalList ? f->myBlendedNormalList
													  : f->blendedNormalList;
					for (i = k = 0; i < f->blendedRigidNum; ++i)
					{
						normal.Reset();
						pos.Reset();
						for (t = 0.0f; t < 0.999999f; ++k)
						{
							normal += RotVec(normList[k],
										  f->blendedBoneList[k]->GetMatrix()) *
								f->blendWeightList[k];
							pos += f->blendedBoneList[k]->Transform(
									   f->blendedVecList[k]) *
								f->blendWeightList[k];
							t = (float)(t + f->blendWeightList[k]);
						}
						eyeDot = (view->GetCamera().pivot - pos) * normal;
						lightDot = (float)(vWorLightDir * normal);
						if (lightDot * eyeDot < 0.0f)
						{
							WTVertex* vtx = GetVtxBuff(i);
							vtx->diffuse =
								(vtx->diffuse & 0xff000000) | light.ambient;
						}
					}
					normList = f->myNormalList ? f->myNormalList : f->normList;
					for (; i < f->rigidNum; ++i)
					{
						v = view->GetCamera().pivot *
							~f->boneList[i]->GetMatrix();
						eyeDot = (v - f->vecList[i]) * normList[i];
						lightDot = (float)(vWorLightDir *
							RotVec(normList[i], f->boneList[i]->GetMatrix()));
						if (lightDot * eyeDot < 0.0f)
						{
							WTVertex* vtx = GetVtxBuff(i);
							vtx->diffuse =
								(vtx->diffuse & 0xff000000) | light.ambient;
						}
					}
					for (; i < f->vtxNum; ++i)
					{
						eyeDot = (vLocEyePos - f->vecList[i]) * normList[i];
						lightDot = vLocLightDir * normList[i];
						if (lightDot * eyeDot < 0.0f)
						{
							WTVertex* vtx = GetVtxBuff(i);
							vtx->diffuse =
								(vtx->diffuse & 0xff000000) | light.ambient;
						}
					}
				}
			}

			view->DrawIndexedTriangles(GetVtxBuff(), f->vtxNum, f->indexList,
				f->indexNum, f->drawFlag, f->xiDrawFlag2);
		}
	}

	if (m_next != 0)
		m_next->Render(view, light);
	if (m_child != 0)
		m_child->Render(view, light);
}

void WBone::RenderHierarchy(WView* view, float size)
{
	const WVector& pivot = m_matrix.pivot;
	view->DrawLine(pivot, 0xffff0000, m_matrix.xa * size + pivot, 0xffff0000,
		0x300000);
	view->DrawLine(pivot, 0xff00ff00, m_matrix.ya * size + pivot, 0xff00ff00,
		0x300000);
	view->DrawLine(pivot, 0xff0000ff, m_matrix.za * size + pivot, 0xff0000ff,
		0x300000);
	if (m_child != 0)
	{
		view->DrawLine(pivot, 0xffffffff, m_child->m_matrix.pivot, 0xffff00ff,
			0x300000);
		m_child->RenderHierarchy(view, size);
	}
	if (m_next != 0)
	{
		view->DrawLine(pivot, 0xffffffff, m_next->m_matrix.pivot, 0xff00ffff,
			0x300000);
		m_next->RenderHierarchy(view, size);
	}
}

void WBone::RenderNormals(WView* view, float axisLen)
{
	WVector v, n;
	float w, t;
	ulong c;
	int i, k;
	WVector* normList;
	for (w_mesh* m = m_mesh; m; m = m->next)
	{
		i = 0;
		normList = m->myBlendedNormalList ? m->myBlendedNormalList
										  : m->blendedNormalList;
		if (m->blendedVecList && normList)
		{
			k = 0;
			for (; i < m->blendedRigidNum; i++)
			{
				c = WisEqual(normList[k], m->blendedNormalList[k], g_EPSILON)
					? 0xffff0000
					: 0xff00ff00;
				t = 0;
				w = m->blendWeightList[k];
				v = m->blendedBoneList[k]->Transform(m->blendedVecList[k]) * w;
				n = RotVec(normList[k], m->blendedBoneList[k]->GetMatrix()) * w;
				t += w;
				for (k++; t < 0.999999f && k < m->blendedTotalNum; k++)
				{
					w = m->blendWeightList[k];
					v +=
						m->blendedBoneList[k]->Transform(m->blendedVecList[k]) *
						w;
					n += RotVec(normList[k],
							 m->blendedBoneList[k]->GetMatrix()) *
						w;
					t += w;
				}
				n.Normalize();
				view->DrawLine(v, c, v + n * axisLen, c, 0x300000);
			}
		}
		normList = m->myNormalList ? m->myNormalList : m->normList;
		if (normList)
		{
			for (; i < m->rigidNum; i++)
			{
				c = WisEqual(normList[i], m->normList[i], g_EPSILON)
					? 0xffff0000
					: 0xff00ff00;
				v = m->boneList[i]->Transform(m->vecList[i]);
				n = RotVec(normList[i], m->boneList[i]->GetMatrix());
				n.Normalize();
				view->DrawLine(v, c, v + n * axisLen, c, 0x300000);
			}
			for (; i < m->vtxNum; i++)
			{
				c = WisEqual(normList[i], m->normList[i], g_EPSILON)
					? 0xffff0000
					: 0xff00ff00;
				v = m->vecList[i] * m_matrix;
				n = RotVec(normList[i], GetMatrix());
				n.Normalize();
				view->DrawLine(v, c, v + n * axisLen, c, 0x300000);
			}
		}
	}
	if (m_child)
		m_child->RenderNormals(view, axisLen);
	if (m_next)
		m_next->RenderNormals(view, axisLen);
}

void WBone::ApplyScale(float scale)
{
	m_localMat.pivot *= scale;
	m_basicMat.pivot *= scale;
	m_matrix.pivot *= scale;
	if (m_keyPos != 0)
	{
		for (int i = 0; i < m_keyPosNum; ++i)
		{
			WVector& pos = m_keyPos[i].pos;
			pos *= scale;
		}
	}
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		int i;
		for (i = 0; i < mesh->blendedTotalNum; ++i)
		{
			WVector& vec = mesh->blendedVecList[i];
			vec *= scale;
		}
		for (i = 0; i < mesh->vtxNum; ++i)
		{
			WVector& vec = mesh->vecList[i];
			vec *= scale;
		}
	}
	if (m_next != 0)
		m_next->ApplyScale(scale);
	if (m_child != 0)
		m_child->ApplyScale(scale);
}

void WBone::SetBoundBox(Waabb aabb, bool merge)
{
	Waabb bound;
	if (merge)
	{
		bound.min = WVector(Min(m_bound_aabb.min.x, aabb.min.x),
			Min(m_bound_aabb.min.y, aabb.min.y),
			Min(m_bound_aabb.min.z, aabb.min.z));
		bound.max = WVector(Max(m_bound_aabb.max.x, aabb.max.x),
			Max(m_bound_aabb.max.y, aabb.max.y),
			Max(m_bound_aabb.max.z, aabb.max.z));
	}
	else
		bound = aabb;
	m_bound_aabb = bound;
	m_bound_sphere = WSphere((bound.max + bound.min) * 0.5f,
		(float)ceil(WVectorLen(bound.max - bound.min) * 0.5f));
}

void WBone::FixNormal(const WMatrix& matrix)
{
	WMatrix world;
	world = m_localMat * matrix;
	WMatrix inverse;
	inverse = ~world;
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		for (int i = 0; i < mesh->vtxNum; ++i)
		{
			WVector& normal = mesh->normList[i];
			normal = WVector(inverse.xx * normal.x + inverse.xz * normal.z +
					inverse.xy * normal.y,
				inverse.yx * normal.x + inverse.yz * normal.z +
					inverse.yy * normal.y,
				inverse.zx * normal.x + inverse.zz * normal.z +
					inverse.zy * normal.y);
		}
	}
	if (m_next != 0)
		m_next->FixNormal(matrix);
	if (m_child != 0)
		m_child->FixNormal(world);
}

WBone* WBone::FindBone(const char* name, ulong hash)
{
	if (hash == 0)
		hash = GetHashCode(name);
	if (m_hashCode == hash && strcmp(name, m_name) == 0)
		return this;
	if (m_next != 0)
	{
		WBone* found = m_next->FindBone(name, hash);
		if (found != 0)
			return found;
	}
	if (m_child != 0)
	{
		WBone* found = m_child->FindBone(name, hash);
		if (found != 0)
			return found;
	}
	return 0;
}

WBone* WBone::FindBone(int id)
{
	if (m_id == id)
		return this;
	if (m_next != 0)
	{
		WBone* found = m_next->FindBone(id);
		if (found != 0)
			return found;
	}
	if (m_child != 0)
	{
		WBone* found = m_child->FindBone(id);
		if (found != 0)
			return found;
	}
	return 0;
}

WBone* WBone::FindBoneByTail(const char* tail)
{
	if (strstr(m_name, tail) != 0)
		return this;
	if (m_next != 0)
	{
		WBone* found = m_next->FindBoneByTail(tail);
		if (found != 0)
			return found;
	}
	if (m_child != 0)
	{
		WBone* found = m_child->FindBoneByTail(tail);
		if (found != 0)
			return found;
	}
	return 0;
}

WBone* WBone::FindFakeBodyBone(w_pet_vertex* list, int count)
{
	if (strstr(m_name, "Bip") == 0 && strstr(m_name, "Jaw") == 0)
	{
		int i;
		for (i = 0; i < count; ++i)
		{
			if (strcmp(list[i].bonename, m_name) == 0)
				break;
		}
		if (i == count && m_child == 0)
			return this;
	}
	if (m_next != 0)
	{
		WBone* found = m_next->FindFakeBodyBone(list, count);
		if (found != 0)
			return found;
	}
	if (m_child != 0)
	{
		WBone* found = m_child->FindFakeBodyBone(list, count);
		if (found != 0)
			return found;
	}
	return 0;
}

int WBone::GetConvexArea(WBlockModel* model)
{
	int i;
	w_mesh* mesh;
	int total = 0;
	for (mesh = m_mesh; mesh != 0; mesh = mesh->next)
		total += mesh->indexNum;
	WVector* list = new WVector[total];
	int count = 0;
	for (mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		for (i = 0; i < mesh->indexNum; ++i)
		{
			list[count++] = mesh->vecList[mesh->indexList[i]] * GetMatrix();
		}
	}
	model->AddTriangleList(list, total);
	delete list;
	return 1;
}

void WBone::ApplySpecularMap(int handle)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		if (mesh->bSpec == false)
		{
			mesh->bSpec = true;
			mesh->uvBackup = new float[mesh->vtxNum][2];
			mesh->texHandle |= (handle & 0x7f) << 11;
			mesh->drawFlag = (mesh->drawFlag & 0xfffff800) | mesh->texHandle;
		}
	}
	if (m_next != 0)
		m_next->ApplySpecularMap(handle);
	if (m_child != 0)
		m_child->ApplySpecularMap(handle);
}

int __fastcall WriteID(int id, FILE* file)
{
	if (id >= 0xfe)
	{
		if (file != 0)
		{
			fputc(0xfe, file);
			fwrite(&id, 2, 1, file);
		}
		return 3;
	}
	if (file != 0)
		fputc(id, file);
	return 1;
}

int WBone::SaveBoneName(FILE* file, int type, char* name)
{
	int written = 0;
	bool skip = name != 0 && m_mesh != 0 && stricmp(name, m_name) != 0;
	if (file != 0)
	{
		if (!skip)
		{
			if (!((type == 0x82 || type == 0x66) && m_mesh != 0))
			{
				fwrite(m_name, 1, strlen(m_name) + 1, file);
				WriteID(m_parent != 0 ? m_parent->m_id : -1, file);
			}
			if (type == 0xff || (type == 0x82 && m_mesh == 0) ||
				type == 0x11b || type == 6)
			{
				WMatrix matrix;
				matrix = m_basicMat;
				if (type == 6)
				{
					for (WBone* bone = m_parent; bone != 0;
						bone = bone->m_parent)
						matrix = matrix * bone->m_basicMat;
					matrix.xm *= 3.125f;
					matrix.ym *= 3.125f;
					matrix.zm *= 3.125f;
				}
				fwrite(&matrix.xx, 4, 1, file);
				fwrite(&matrix.yx, 4, 1, file);
				fwrite(&matrix.zx, 4, 1, file);
				fwrite(&matrix.xy, 4, 1, file);
				fwrite(&matrix.yy, 4, 1, file);
				fwrite(&matrix.zy, 4, 1, file);
				fwrite(&matrix.xz, 4, 1, file);
				fwrite(&matrix.yz, 4, 1, file);
				fwrite(&matrix.zz, 4, 1, file);
				fwrite(&matrix.xm, 4, 1, file);
				fwrite(&matrix.ym, 4, 1, file);
				fwrite(&matrix.zm, 4, 1, file);
			}
		}
	}
	else
	{
		if (!skip)
		{
			if (!((type == 0x82 || type == 0x66) && m_mesh != 0))
			{
				written = strlen(m_name) +
					WriteID(m_parent != 0 ? m_parent->GetID() : -1, 0) + 1;
			}
			if (type == 0xff || (type == 0x82 && m_mesh == 0) ||
				type == 0x11b || type == 6)
				written += 48;
		}
	}
	if (m_child != 0)
		written += m_child->SaveBoneName(file, type, name);
	if (m_next != 0)
		written += m_next->SaveBoneName(file, type, name);
	return written;
}

int WBone::SaveAnimationKey(FILE* file, int id, float (*range)[2], int count)
{
	int written = 0;
	w_keyframe_pos* posKeys = 0;
	w_keyframe_rot* rotKeys = 0;
	int posNum, rotNum, i;
	float zero;
	if (m_keyPosNum != 0 || m_keyRotNum != 0)
	{
		if (m_keyPosNum > 0)
		{
			posNum = OptimizePosKey(range, count, &posKeys);
			if (m_keyPosNum == posNum)
				posKeys = m_keyPos;
		}
		else
		{
			posNum = 0;
		}
		if (m_keyRotNum > 0)
		{
			rotNum = OptimizeRotKey(range, count, &rotKeys);
			if (m_keyRotNum == rotNum)
				rotKeys = m_keyRot;
		}
		else
		{
			rotNum = 0;
		}
		if (file != 0)
		{
			WriteID(m_id, file);
			fwrite(&posNum, 4, 1, file);
			for (i = 0; i < posNum; ++i)
			{
				fwrite(&posKeys[i].t, 4, 1, file);
				fwrite(&posKeys[i].pos.x, 4, 1, file);
				fwrite(&posKeys[i].pos.y, 4, 1, file);
				fwrite(&posKeys[i].pos.z, 4, 1, file);
			}
			fwrite(&rotNum, 4, 1, file);
			w_keyframe_rot* rot = rotKeys;
			for (i = 0; i < rotNum; ++i)
			{
				fwrite(&rot[i].t, 4, 1, file);
				fwrite(&rot[i].rot.x, 4, 1, file);
				fwrite(&rot[i].rot.y, 4, 1, file);
				fwrite(&rot[i].rot.z, 4, 1, file);
				fwrite(&rot[i].rot.w, 4, 1, file);
			}
			zero = 0.0f;
			fwrite(&zero, 4, 1, file);
		}
		written = WriteID(m_id, 0) + posNum * 16 + rotNum * 20 + 12;
		if (m_keyPosNum > 0 && m_keyPos != posKeys)
			delete[] posKeys;
		if (m_keyRotNum > 0 && m_keyRot != rotKeys)
			delete[] rotKeys;
	}
	if (m_child != 0)
		written += m_child->SaveAnimationKey(file, id, range, count);
	if (m_next != 0)
		written += m_next->SaveAnimationKey(file, id, range, count);
	return written;
}

int WBone::SetID(int id, bool flag, char* name)
{
	if ((flag || m_mesh == 0) &&
		(name == 0 || m_mesh == 0 || stricmp(name, m_name) == 0))
	{
		m_id = id;
		++id;
	}
	if (m_child != 0)
		id = m_child->SetID(id, flag, name);
	if (m_next != 0)
		id = m_next->SetID(id, flag, name);
	return id;
}

void WBone::CountBones(int& count)
{
	WBone* bone = this;
	do
	{
		++count;
		WBone* child = bone->m_child;
		if (child != 0)
			child->CountBones(count);
		bone = bone->m_next;
	} while (bone != 0);
}

float WBone::LoadAnimationKey(cFile* fp)
{
	float t = 0.0f;
	int i;
	float n;
	WQuat rot;
	fp->Read(&m_keyPosNum, 4);
	if (m_keyPosNum != 0)
	{
		m_keyPos = new w_keyframe_pos[m_keyPosNum];
		m_keyFlag |= 2;
		for (i = 0; i < m_keyPosNum; ++i)
		{
			fp->Read(&m_keyPos[i].t, 4);
			fp->Read(&m_keyPos[i].pos.x, 4);
			fp->Read(&m_keyPos[i].pos.y, 4);
			fp->Read(&m_keyPos[i].pos.z, 4);
			if (t < m_keyPos[i].t)
				t = m_keyPos[i].t;
		}
	}
	fp->Read(&m_keyRotNum, 4);
	if (m_keyRotNum != 0)
	{
		m_keyFlag |= 1;
		m_keyRot = new w_keyframe_rot[m_keyRotNum];
		for (i = 0; i < m_keyRotNum; ++i)
		{
			fp->Read(&m_keyRot[i].t, 4);
			fp->Read(&rot.x, 4);
			fp->Read(&rot.y, 4);
			fp->Read(&rot.z, 4);
			fp->Read(&rot.w, 4);
			rot.Normalize();
			m_keyRot[i].rot = rot;
			if (t < m_keyRot[i].t)
				t = m_keyRot[i].t;
		}
	}
	fp->Read(&n, 4);
	return t;
}

void WBone::GetName(char** buffer, int& count)
{
	if (m_parent != 0)
		memcpy(*buffer, m_parent->m_name, strlen(m_parent->m_name));
	*buffer += 0x1e;
	memcpy(*buffer, m_name, strlen(m_name));
	*buffer += 0x1e;
	++count;
	if (m_next != 0)
		m_next->GetName(buffer, count);
	if (m_child != 0)
		m_child->GetName(buffer, count);
}

void WBone::GetBoneInfo(int* totals)
{
	WBone* bone = this;
	for (;;)
	{
		for (w_mesh* mesh = bone->m_mesh; mesh != 0; mesh = mesh->next)
		{
			totals[1] += mesh->vtxNum;
			totals[2] += mesh->indexNum / 3;
		}
		++totals[0];
		if (bone->m_next != 0)
			bone->m_next->GetBoneInfo(totals);
		WBone* child = bone->m_child;
		if (child == 0)
			break;
		bone = child;
	}
}

int WBone::GetOverlapBox(float* box, WBone* bone)
{
	for (w_mesh* mesh = bone->m_mesh; mesh != 0; mesh = mesh->next)
	{
		for (int i = 0; i < mesh->indexNum; i += 3)
		{
			for (int k = 0; k < 3; ++k)
			{
				int index = mesh->indexList[i + k];
				WVector point;
				if (bone != this)
				{
					if (mesh->indexList[i] >= mesh->rigidNum)
						continue;
					if (mesh->boneList[index] != this)
						continue;
				}
				if (mesh->indexList[i] < mesh->rigidNum)
					point = mesh->vecList[index] *
						mesh->boneList[index]->GetMatrix() * ~GetMatrix();
				else
					point = mesh->vecList[index];
				box[0] = box[0] < point.x ? box[0] : point.x;
				box[1] = box[1] > point.x ? box[1] : point.x;
				box[2] = box[2] < point.y ? box[2] : point.y;
				box[3] = box[3] > point.y ? box[3] : point.y;
				box[4] = box[4] < point.z ? box[4] : point.z;
				box[5] = box[5] > point.z ? box[5] : point.z;
			}
		}
	}
	if (bone->m_next != 0)
		GetOverlapBox(box, bone->m_next);
	if (bone->m_child != 0)
		GetOverlapBox(box, bone->m_child);
	return 1;
}

void WBone::GetTotalOverlapBox(WVector* min, WVector* max, WMatrix* matrix,
	bool visibleOnly)
{
	WVector v;
	int i;
	w_mesh* mesh;

	if (matrix)
	{
		for (mesh = m_mesh; mesh; mesh = mesh->next)
		{
			if (visibleOnly && !IsMeshVisible(this, mesh))
				continue;

			for (i = 0; i < mesh->vtxNum; ++i)
			{
				if (i < mesh->rigidNum)
					v = mesh->boneList[i]->Transform(mesh->vecList[i]);
				else
					v = Transform(mesh->vecList[i]);

				v = v * *matrix;

				min->x = min->x < v.x ? min->x : v.x;
				max->x = max->x > v.x ? max->x : v.x;
				min->y = min->y < v.y ? min->y : v.y;
				max->y = max->y > v.y ? max->y : v.y;
				min->z = min->z < v.z ? min->z : v.z;
				max->z = max->z > v.z ? max->z : v.z;
			}
		}
	}
	else
	{
		for (mesh = m_mesh; mesh; mesh = mesh->next)
		{
			if (visibleOnly && !IsMeshVisible(this, mesh))
				continue;

			for (i = 0; i < mesh->vtxNum; ++i)
			{
				if (i < mesh->rigidNum)
					v = mesh->boneList[i]->Transform(mesh->vecList[i]);
				else
					v = Transform(mesh->vecList[i]);

				min->x = min->x < v.x ? min->x : v.x;
				max->x = max->x > v.x ? max->x : v.x;
				min->y = min->y < v.y ? min->y : v.y;
				max->y = max->y > v.y ? max->y : v.y;
				min->z = min->z < v.z ? min->z : v.z;
				max->z = max->z > v.z ? max->z : v.z;
			}
		}
	}

	if (m_next)
		m_next->GetTotalOverlapBox(min, max, matrix, visibleOnly);

	if (m_child)
		m_child->GetTotalOverlapBox(min, max, matrix, visibleOnly);
}

void WBone::Rotate(float angle, int axis, bool reset)
{
	if (reset)
		m_rotateMat.Reset();
	m_rotateMat.Rotate(angle, (char)axis);
	m_flag.Turn(ROTATEBONE, !reset || !WisZero(angle, g_EPSILON));
}

void WBone::Move(WVector& vec, bool child)
{
	m_moveVec = vec;
	m_flag.Enable(0x400);
	if (child)
	{
		if (m_next != 0)
			m_next->Move(vec, child);
		if (m_child != 0)
			m_child->Move(vec, child);
	}
}

void WBone::SetSelfIllum(bool enabled, bool recursive)
{
	WBone* bone = this;
	do
	{
		if (enabled == true)
			bone->m_flag.Enable(2);
		else
			bone->m_flag.Disable(2);
		if (!recursive)
			return;
		WBone* next = bone->m_next;
		if (next != 0)
			next->SetSelfIllum(enabled, recursive);
		bone = bone->m_child;
	} while (bone != 0);
}

void WBone::SetSelfIllumColor(ulong color, bool recursive)
{
	WBone* bone = this;
	bone->m_selfillumColor = color & 0xffffff;
	if (recursive)
	{
		for (;;)
		{
			WBone* next = bone->m_next;
			if (next != 0)
				next->SetSelfIllumColor(color, recursive);
			bone = bone->m_child;
			if (bone == 0)
				break;
			bone->m_selfillumColor = color & 0xffffff;
		}
	}
}

void WBone::SetInvisible(bool recursive, bool invisible)
{
	WBone* bone = this;
	do
	{
		if (invisible)
			bone->m_flag.Enable(0x40000);
		else
			bone->m_flag.Disable(0x40000);
		if (!recursive)
			return;
		WBone* next = bone->m_next;
		if (next != 0)
			next->SetInvisible(recursive, invisible);
		bone = bone->m_child;
	} while (bone != 0);
}

void WBone::HideBone(bool recursive)
{
	WBone* bone = this;
	do
	{
		bone->m_flag.Enable(1);
		if (!recursive)
			return;
		WBone* next = bone->m_next;
		if (next != 0)
			next->HideBone(recursive);
		bone = bone->m_child;
	} while (bone != 0);
}

void WBone::ShowBone(bool recursive)
{
	WBone* bone = this;
	do
	{
		bone->m_flag.Disable(1);
		if (!recursive)
			return;
		WBone* next = bone->m_next;
		if (next != 0)
			next->ShowBone(recursive);
		bone = bone->m_child;
	} while (bone != 0);
}

void WBone::AllocVTX(int count)
{
	if (m_vtx_len < count)
	{
		m_vtx_len = count;
		if (m_vtxList != 0)
			delete[] m_vtxList;
		m_vtxList = new WTVertex[m_vtx_len];
	}
}

void WBone::CountVtxBuff(int count)
{
	m_vtx_count += count;
	if (m_vtx_count == 0)
	{
		if (m_vtxList != 0)
		{
			delete[] m_vtxList;
			m_vtxList = 0;
			m_vtx_len = 0;
		}
	}
}

void WBone::SetRigidVtxFlag(void)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		if (mesh->rigidNum != 0 || mesh->blendedRigidNum != 0)
		{
			m_flag.Enable(4);
			break;
		}
	}
	if (m_next != 0)
		m_next->SetRigidVtxFlag();
	if (m_child != 0)
		m_child->SetRigidVtxFlag();
}

bool WBone::CompareBasicMatrix(WBone* other)
{
	if (((((ulong)m_flag) >> 16) & 1) == 0)
	{
		WBone* found = other->FindBone(m_name, 0);
		if (found == 0)
			return false;
		if (m_basicMat != found->m_basicMat)
			return false;
	}
	if (m_child != 0 && !m_child->CompareBasicMatrix(other))
		return false;
	if (m_next != 0 && !m_next->CompareBasicMatrix(other))
		return false;
	return true;
}

void WBone::xSetAlpha(unsigned char alpha, int recurse)
{
	m_alpha = alpha;
	if (alpha == 0)
		m_flag.Enable(1);
	if ((((ulong)m_flag) & 1) != 0 && alpha > 0)
		m_flag.Disable(1);
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		ulong value = (ulong)(int)((float)alpha * mesh->alpha);
		if ((value & 0xff) == 0xff)
		{
			int flag = mesh->drawFlag;
			if ((flag & 0x1800000) == 0 && (mesh->flags & 0x400000) == 0)
				flag &= 0xffbfffff;
			else
				flag |= 0x400000;
			flag &= 0xdfffffff;
			mesh->drawFlag = flag;
		}
		else
		{
			mesh->drawFlag |= 0x20400000;
		}
	}
	if (recurse != 0)
	{
		if (m_next != 0)
			m_next->xSetAlpha(alpha, 1);
		if (m_child != 0)
			m_child->xSetAlpha(alpha, 1);
	}
}

void WBone::xRender(WView* view, WxBatchState* state, bool setMatrix,
	int handle, int newHandle)
{
	bool visible = IsVisible();
	bool mirror = m_flag.GetFlag(INVISIBLE);
	bool hidden = IsHidden();
	if (visible && !mirror && !hidden)
	{
		for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
		{
			if (mesh->xpBatch == 0)
				continue;
			state->xiBaseVtxIdx = mesh->xpBatch->xiBaseVtxIdx;
			state->xiBaseIdxIdx = mesh->xpBatch->xiBaseIdxIdx;
			state->xnVtxs = mesh->vtxNum;
			state->xnIdxs = mesh->indexNum;
			if (setMatrix)
			{
				state->xmW = m_matrix;
				state->xnmTransfs = 0;
			}
			int draw = mesh->drawFlag;
			if ((draw & 0x7ff) == handle && handle != newHandle)
				state->xiFlag0 = draw ^ ((draw ^ newHandle) & 0x7ff);
			else
				state->xiFlag0 = draw;
			int flag2 = mesh->xiDrawFlag2;
			state->xiFlag1 = flag2;
			if ((flag2 & 4) != 0)
				state->xdwDiffuse =
					((ulong)m_alpha << 24) | view->xGetLight()->ambient2;
			else
				state->xdwDiffuse = ((ulong)m_alpha << 24) | 0xffffff;
			view->xDrawIndexedTriangles(*state);
		}
	}
	if (m_next != 0)
		m_next->xRender(view, state, setMatrix, handle, newHandle);
	if (m_child != 0)
		m_child->xRender(view, state, setMatrix, handle, newHandle);
}

void WBone::ReplaceKeyframe(WBone* bone, float time)
{
	WMatrix other, mine;
	mine = m_localMat;
	other = bone->m_localMat;

	m_keyFlag = 0;
	if (m_keyRot != 0)
	{
		delete[] m_keyRot;
		m_keyRot = 0;
	}
	m_keyRotNum = bone->m_keyRotNum;
	if (m_keyRotNum != 0)
	{
		m_keyRot = new w_keyframe_rot[m_keyRotNum];
		memcpy(m_keyRot, bone->m_keyRot, m_keyRotNum * 20);
		m_keyFlag |= 1;
		m_flag.Disable(0x40);
	}
	else
	{
		WQuat qSrc, qDst;
		qSrc = other;
		qDst = mine;
		if (WisZero(Wabs(qSrc.x - qDst.x), 1e-5f) == 0 ||
			WisZero(Wabs(qSrc.y - qDst.y), 1e-5f) == 0 ||
			WisZero(Wabs(qSrc.z - qDst.z), 1e-5f) == 0 ||
			WisZero(Wabs(qSrc.w - qDst.w), 1e-5f) == 0)
		{
			m_keyRotNum = 2;
			m_keyRot = new w_keyframe_rot[2];
			m_keyRot[0].t = 0;
			m_keyRot[0].rot = qSrc;
			m_keyRot[1].t = time;
			m_keyRot[1].rot = qSrc;
			m_keyFlag |= 1;
			m_flag.Disable(0x40);
		}
	}

	if (m_keyPos != 0)
	{
		delete[] m_keyPos;
		m_keyPos = 0;
	}
	m_keyPosNum = bone->m_keyPosNum;
	if (m_keyPosNum != 0)
	{
		m_keyPos = new w_keyframe_pos[m_keyPosNum];
		memcpy(m_keyPos, bone->m_keyPos, m_keyPosNum * 16);
	}
	else
	{
		WVector vSrc = other.pivot;
		WVector vDst = mine.pivot;
		if (WisEqual(vSrc, vDst, 1e-5f) != 0)
			return;
		m_keyPosNum = 2;
		m_keyPos = new w_keyframe_pos[2];
		m_keyPos[0].t = 0;
		m_keyPos[0].pos = vSrc;
		m_keyPos[1].t = time;
		m_keyPos[1].pos = vSrc;
	}
	m_keyFlag |= 2;
	m_flag.Disable(0x40);
}

WBone* WBone::AttachMeshBone(WBone* bone, int handle, int oldHandle)
{
	m_next = new WBone;
	m_next->SetName(bone->m_name);
	m_next->m_id = 0;
	m_next->SetBasicMatrix(bone->m_basicMat);

	w_mesh* mesh = bone->m_mesh;
	w_mesh** tail = &m_next->m_mesh;
	while (mesh != 0)
	{
		if ((((ulong)m_next->m_flag) & 0x10020) == 0)
			m_next->m_flag.Enable(0x10020);
		*tail = m_next->CopyMesh(mesh);
		if (*tail != 0)
		{
			if (handle != oldHandle && ((*tail)->drawFlag & 0x7ff) == oldHandle)
				(*tail)->drawFlag = ((*tail)->drawFlag & ~0x7ff) | handle;
			bool rigid = ((((ulong)m_next->m_flag) >> 2) & 1) != 0;
			if (rigid == false)
			{
				if ((*tail)->rigidNum != 0 || (*tail)->blendedRigidNum != 0)
					m_next->m_flag.Enable(4);
			}
		}
		mesh = mesh->next;
		if (*tail != 0)
			tail = &(*tail)->next;
	}
	return m_next;
}

void WBone::ChangeTextureByHandle(int handle, int oldHandle, bool next,
	bool child)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		if (mesh->originalTexHandle == oldHandle)
			mesh->drawFlag = (mesh->drawFlag & 0xfffff800) | handle;
	}
	if (next && m_next != 0)
		m_next->ChangeTextureByHandle(handle, oldHandle, next, child);
	if (child && m_child != 0)
		m_child->ChangeTextureByHandle(handle, oldHandle, next, child);
}

bool WBone::ChangeTexture(const char* name, int oldHandle, int handle)
{
	if (name == 0 || oldHandle == 0 || handle == 0)
		return false;
	{
		if (m_hashCode == GetHashCode(name) && strcmp(name, m_name) == 0)
		{
			for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
			{
				if (mesh->originalTexHandle == oldHandle)
					mesh->drawFlag = (mesh->drawFlag & 0xfffff800) | handle;
			}
			return true;
		}
	}
	if (m_next != 0 && m_next->ChangeTexture(name, oldHandle, handle))
		return true;
	if (m_child != 0 && m_child->ChangeTexture(name, oldHandle, handle))
		return true;
	return false;
}

bool WBone::ChangeTexture(const char* name, int handle)
{
	if (name == 0 || handle == 0)
		return false;
	{
		if (m_hashCode == GetHashCode(name) && strcmp(name, m_name) == 0)
		{
			for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
			{
				if (mesh->originalTexHandle != 0)
					mesh->drawFlag = (mesh->drawFlag & 0xfffff800) | handle;
			}
			return true;
		}
	}
	if (m_next != 0 && m_next->ChangeTexture(name, handle))
		return true;
	if (m_child != 0 && m_child->ChangeTexture(name, handle))
		return true;
	return false;
}

void WBone::ResetMyNormalList(void)
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		if (mesh->myNormalList != 0)
			memcpy(mesh->myNormalList, mesh->normList, mesh->vtxNum * 12);
		if (mesh->myBlendedNormalList != 0)
			memcpy(mesh->myBlendedNormalList, mesh->blendedNormalList,
				mesh->blendedTotalNum * 12);
	}
}

void WBone::GatherNormalMergeInfo(w_normalmerge& merge) const
{
	for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
	{
		if (mesh->weightList == 0)
			continue;
		for (int i = 0; i < mesh->vtxNum; ++i)
		{
			if (mesh->weightList[i] != 1.0f)
				merge.AddVertex(mesh, i);
		}
	}
}

void WBone::xBuildTransfMatPtrList(std::vector<const WMatrix*>& list) const
{
	const WBone* bone = this;
	do
	{
		if ((((ulong)bone->m_flag) & 0x10000) == 0)
			list[bone->m_id] = &bone->m_matrix;
		if (bone->m_child != 0)
			bone->m_child->xBuildTransfMatPtrList(list);
		bone = bone->m_next;
	} while (bone != 0);
}

void WBone::SetMirrored(bool mirrored)
{
	WBone* bone = this;
	for (;;)
	{
		if (mirrored)
			bone->m_flag.Enable(0x20000);
		else
			bone->m_flag.Disable(0x20000);
		for (w_mesh* mesh = bone->m_mesh; mesh != 0; mesh = mesh->next)
		{
			mesh->xiDrawFlag2 &= 0xfffff3ffUL;
			int draw = mesh->xiDrawFlag2;
			if (mirrored)
				draw |= 0x800;
			mesh->xiDrawFlag2 = draw;
		}
		if (bone->m_next != 0)
			bone->m_next->SetMirrored(mirrored);
		WBone* child = bone->m_child;
		if (child == 0)
			break;
		bone = child;
	}
}

void WBone::SetScale(float scale)
{
	m_bonescale = scale;
	m_flag.Enable(0x80000);
}

void __fastcall WBone::CalcMeshAABB(WBone& bone, w_mesh& mesh)
{
	int i;
	if (mesh.vtxNum <= 0)
		return;
	if (mesh.rigidNum == 0)
	{
		mesh.aabbBone = &bone;
	}
	else
	{
		std::map<WBone*, float> counts;
		for (i = 0; i < mesh.rigidNum; ++i)
		{
			std::map<WBone*, float>::iterator found;
			found = counts.find(mesh.boneList[i]);
			if (found == counts.end())
				counts.insert(
					std::pair<WBone* const, float>(mesh.boneList[i], 1.0f));
			else
				(*found).second = (*found).second + 1.0f;
		}
		if (i < mesh.vtxNum)
		{
			counts.insert(std::pair<WBone* const, float>(&bone, 1.0f));
			for (; i < mesh.vtxNum; ++i)
				counts[&bone] += 1.0f;
		}
		if (counts.size() > 0)
		{
			std::map<WBone*, float>::const_iterator best, scan;
			best = counts.begin();
			scan = best;
			for (scan++; scan != counts.end(); scan++)
			{
				if ((*scan).second > (*best).second)
					best = scan;
			}
			mesh.aabbBone = (*best).first;
		}
	}

	WMatrix inverse = ~mesh.aabbBone->m_matrix;
	WBone* prevBone = 0;
	WMatrix m;
	WVector v;
	mesh.localAabb.Clear();

	i = 0;
	for (; i < mesh.rigidNum; ++i)
	{
		if (mesh.boneList[i] != mesh.aabbBone)
		{
			if (mesh.boneList[i] != prevBone)
			{
				prevBone = mesh.boneList[i];
				m = mesh.boneList[i]->GetMatrix() * inverse;
			}
			v = mesh.vecList[i] * m;
		}
		else
			v = mesh.vecList[i];
		mesh.localAabb.AddPoint(v);
	}
	if (&bone != mesh.aabbBone)
		m = bone.m_matrix * inverse;
	for (; i < mesh.vtxNum; ++i)
	{
		if (&bone != mesh.aabbBone)
			v = mesh.vecList[i] * m;
		else
			v = mesh.vecList[i];
		mesh.localAabb.AddPoint(v);
	}
}

bool __fastcall WBone::IsMeshVisible(WBone* bone, w_mesh* mesh)
{
	return bone->IsVisible() && !bone->m_flag.GetFlag(INVISIBLE) &&
		!bone->IsHidden() && mesh->alpha > 0.0f;
}

bool WBone::CheckMeshBoneAllVertexRigid(void) const
{
	if (((((ulong)m_flag) >> 16) & 1) != 0)
	{
		for (w_mesh* mesh = m_mesh; mesh != 0; mesh = mesh->next)
		{
			if (mesh->rigidNum < mesh->vtxNum)
				return false;
		}
	}
	if (m_next != 0 && !m_next->CheckMeshBoneAllVertexRigid())
		return false;
	if (m_child != 0 && !m_child->CheckMeshBoneAllVertexRigid())
		return false;
	return true;
}

WBone::w_normalmerge::w_normalmerge(WPuppet& puppet)
{
	WBone* bone;
	for (bone = puppet.FindMeshBone(0); bone != 0;
		bone = puppet.FindMeshBone(bone))
		bone->ResetMyNormalList();
	for (bone = puppet.FindMeshBone(0); bone != 0;
		bone = puppet.FindMeshBone(bone))
		bone->GatherNormalMergeInfo(*this);
	MergeNormal();
}

void WBone::w_normalmerge::AddVertex(w_mesh* mesh, int index)
{
	PLIST& plist = m_wlist[mesh->weightList[index]];
	P* p = Find(plist, mesh->vecList[index]);
	if (p == 0)
	{
		plist.push_back(P(mesh->vecList[index]));
		p = &*plist.rbegin();
	}
	NLIST& nlist = p->m_meshList[mesh];
	N* n = Find(nlist, mesh->normList[index]);
	if (n == 0)
	{
		nlist.push_back(N(mesh->normList[index]));
		n = &*nlist.rbegin();
	}
	n->m_idxList.push_back(index);
}

void WBone::w_normalmerge::MergeNormal(void)
{
	WLIST::const_iterator w;
	for (w = m_wlist.begin(); w != m_wlist.end(); w++)
	{
		PLIST::const_iterator p;
		for (p = (*w).second.begin(); p != (*w).second.end(); p++)
		{
			MLIST::const_iterator m;
			for (m = (*p).m_meshList.begin(); m != (*p).m_meshList.end(); m++)
			{
				w_mesh* mesh = (*m).first;
				if (mesh->myNormalList == 0)
					continue;
				NLIST::const_iterator n;
				for (n = (*m).second.begin(); n != (*m).second.end(); n++)
				{
					int index = *(*n).m_idxList.begin();
					bool merged = false;
					MLIST::const_iterator other = m;
					for (other++; other != (*p).m_meshList.end(); other++)
					{
						w_mesh* mesh2 = (*other).first;
						if (mesh2->myNormalList == 0)
							continue;
						if (stricmp(mesh->mpetName, mesh2->mpetName) == 0)
							continue;
						NLIST::const_iterator n2;
						for (n2 = (*other).second.begin();
							n2 != (*other).second.end(); n2++)
						{
							merged = true;
							int index2 = *(*n2).m_idxList.begin();
							mesh->myNormalList[index] +=
								mesh2->normList[index2];
							mesh2->myNormalList[index2] += (*n).m_n;
							SetMyNormalsEqualToFirstNormal(mesh2,
								(*n2).m_idxList);
						}
					}
					if (merged)
						SetMyNormalsEqualToFirstNormal(mesh, (*n).m_idxList);
				}
			}
		}
	}
	NormalizeMergedNormals();
	ApplyToBlendedNormalList();
}

void WBone::w_normalmerge::NormalizeMergedNormals(void)
{
	WLIST::const_iterator w;
	PLIST::const_iterator p;
	MLIST::const_iterator m;
	NLIST::const_iterator n;

	for (w = m_wlist.begin(); w != m_wlist.end(); w++)
	{
		for (p = (*w).second.begin(); p != (*w).second.end(); p++)
		{
			for (m = (*p).m_meshList.begin(); m != (*p).m_meshList.end(); m++)
			{
				w_mesh* mesh = (*m).first;
				if ((*m).first->myNormalList)
				{
					for (n = (*m).second.begin(); n != (*m).second.end(); n++)
					{
						for (std::vector<int>::const_iterator i =
								 (*n).m_idxList.begin();
							i != (*n).m_idxList.end(); i++)
						{
							if (WisEqual(mesh->myNormalList[*i],
									mesh->normList[*i], 1e-05f) == 0)
								mesh->myNormalList[*i].Normalize();
						}
					}
				}
			}
		}
	}
}

void WBone::w_normalmerge::ApplyToBlendedNormalList(void)
{
	w_mesh* mesh;
	int slot, i;
	float weight;
	std::list<w_mesh*> meshList;

	WLIST::const_iterator w;
	for (w = m_wlist.begin(); w != m_wlist.end(); w++)
	{
		PLIST::const_iterator p;
		for (p = (*w).second.begin(); p != (*w).second.end(); p++)
		{
			MLIST::const_iterator m;
			for (m = (*p).m_meshList.begin(); m != (*p).m_meshList.end(); m++)
			{
				mesh = (*m).first;
				if (std::find(meshList.begin(), meshList.end(), mesh) ==
					meshList.end())
					meshList.push_back(mesh);
			}
		}
	}

	std::list<w_mesh*>::const_iterator it;
	for (it = meshList.begin(); it != meshList.end(); it++)
	{
		mesh = *it;
		const std::vector<WMatrix>& matrixList =
			WPuppet::GetOrginalMatrixList(mesh->mpetName);
		slot = 0;
		for (i = 0; i < mesh->blendedRigidNum; ++i)
		{
			if (mesh->weightList[i] == 1.0f ||
				WisEqual(mesh->myNormalList[i], mesh->normList[i], 1e-05f) != 0)
			{
				for (weight = 0.0f; weight < 0.999f; ++slot)
				{
					weight += mesh->blendWeightList[slot];
				}
			}
			else
			{
				for (weight = 0.0f; weight < 0.999f; ++slot)
				{
					int rigid = mesh->boneList[i]->GetID();
					int blended = mesh->blendedBoneList[slot]->GetID();
					if (rigid >= 0 && rigid < (int)matrixList.size() &&
						blended >= 0 && blended < (int)matrixList.size())
					{
						const WMatrix* matrices = &matrixList[0];
						mesh->myBlendedNormalList[slot] =
							RotVec(mesh->myNormalList[i],
								matrices[rigid] * ~matrixList[blended]);
					}
					weight += mesh->blendWeightList[slot];
				}
			}
		}
	}
}

WBone::w_normalmerge::P* __fastcall WBone::w_normalmerge::Find(PLIST& plist,
	const WVector& p)
{
	PLIST::iterator it;
	it = plist.begin();
	PLIST::iterator last = plist.end();
	while (it != last)
	{
		if (WisEqual((*it).m_p, p, 0.001f))
			return &*it;
		it++;
	}
	return 0;
}

WBone::w_normalmerge::N* __fastcall WBone::w_normalmerge::Find(NLIST& nlist,
	const WVector& n)
{
	NLIST::iterator it;
	it = nlist.begin();
	NLIST::iterator last = nlist.end();
	while (it != last)
	{
		if (WisEqual((*it).m_n, n, 0.001f))
			return &*it;
		it++;
	}
	return 0;
}

void __fastcall WBone::w_normalmerge::SetMyNormalsEqualToFirstNormal(
	w_mesh* mesh, const std::vector<int>& idxList)
{
	if (idxList.size() <= 1)
		return;
	std::vector<int>::const_iterator it = idxList.begin();
	int first = *it;
	for (; it != idxList.end(); it++)
		mesh->myNormalList[*it] = mesh->myNormalList[first];
}
