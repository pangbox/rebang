#include "wresrcmng.h"
#include "wpuppet.h"
#include "wbone.h"
#include "rectmng.h"
#include "wboneset.h"
#include "wpetfile.h"
#include "wmesh.h"
#include "wview.h"
#include "bitmap.h"
#include <stdio.h>

unsigned char WPuppet::ms_lockedBoneSetPos = 0xff;
int WPuppet::box_v[8][3] = {
	{ 0, 0, 0 },
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 1, 1, 0 },
    { 0, 0, 1 },
	{ 1, 0, 1 },
    { 0, 1, 1 },
    { 1, 1, 1 }
};
int WPuppet::box_p[6][4] = {
	{ 0, 2, 3, 1 },
    { 4, 5, 7, 6 },
    { 0, 1, 5, 4 },
    { 1, 3, 7, 5 },
	{ 3, 2, 6, 7 },
    { 2, 0, 4, 6 }
};
WList<WPuppet::w_tex_piece*>* WPuppet::m_tex_piece;
unsigned char WPuppet::ms_boneSetPos;
WMemFillBlock g_mem(0x4800, 5);

WPuppet::WPuppet()
	: m_addedMotionList(4, 0),
	  m_bbList(8, 8),
	  m_fanimList(4, 4),
	  m_boneList(4, 16)
{
	m_bClone = false;
	memset(m_petName, 0, sizeof(m_petName));
	m_frame = 0;
	m_framenum = 0;
	m_storeTexList = 0;
	m_rendMode = 0;
	m_aniLen = 0.0f;
	m_rootbone = 0;
	m_lightmode = 0;
	m_mdList = 0;
	m_bound = Waabb(WVector::ZERO, WVector::ZERO);
	m_boundSphere = WSphere(WVector::ZERO, 0.0f);
	m_BBox.bone = 0;
	m_BBox.info = 0;
	m_faceNum = 0;
	m_mat.Reset();
}

WPuppet::~WPuppet()
{
	for (w_face_animation* anim = m_fanimList.Start(); anim;
		anim = m_fanimList.Next())
	{
		g_mem.Free(anim->mesh);
		delete anim;
	}
	for (w_bound_box* box = m_bbList.Start(); box; box = m_bbList.Next())
	{
		if (!m_bClone)
		{
			if (box->info->option)
				g_mem.Free(box->info->option);
			g_mem.Free(box->info);
		}
		g_mem.Free(box);
	}
	if (!m_bClone)
	{
		if (m_mdList)
		{
			for (w_motion_data* data = m_mdList->Start(); data;
				data = m_mdList->Next())
				delete[] data;
			if (m_mdList)
			{
				delete m_mdList;
				m_mdList = 0;
			}
		}
		if (m_framenum > 0)
		{
			for (int i = 0; i < m_framenum; ++i)
				g_mem.Free(m_frame[i].data);
			g_mem.Free(m_frame);
		}
	}
	if (m_rootbone)
	{
		delete m_rootbone;
		m_rootbone = 0;
	}
	if (m_storeTexList)
	{
		for (int handle = m_storeTexList->Start(); handle;
			handle = m_storeTexList->Next())
			GetResrcManager()->Release(handle);
		delete m_storeTexList;
	}
	ClearAddMotion();
	ClearTexPiece();
	if (m_tex_piece && !m_tex_piece->Start())
	{
		delete m_tex_piece;
		m_tex_piece = 0;
	}
}

void WPuppet::Share(w_share_pet_data* data)
{
	m_bClone = true;
	strcpy(m_petName, data->petName);
	m_frame = data->frame;
	m_framenum = data->numFramedata;
	m_mdList = data->mdList;
	m_bound = data->bound;
	m_faceNum = data->faceNum;
	m_aniLen = data->length;
	m_rootbone = data->rootbone->MakeClone(0);
	m_rootbone->ResetMeshBone(m_rootbone, true);
	m_rootbone->CombineVtxColor(true);

	for (w_bound_box* box = data->bbList->Start(); box != 0;
		box = data->bbList->Next())
	{
		w_bound_box* copy = (w_bound_box*)g_mem.Alloc(sizeof(w_bound_box));
		copy->info = box->info;
		copy->bone = box->bone != 0
			? m_rootbone->FindBone(box->bone->GetBoneName(), 0)
			: 0;
		m_bbList.AddItem(copy, copy->info->name, false);
	}

	w_bound_box* found = data->bbList->Find("bound");
	if (found == 0)
		found = data->bbList->Find("center");
	if (found != 0)
	{
		m_BBox.bone = found->bone;
		m_BBox.info = found->info;
	}
	if (m_BBox.bone != 0)
		m_BBox.bone = m_rootbone->FindBone(m_BBox.bone->GetBoneName(), 0);

	for (w_face_animation* anim = data->fanimList->Start(); anim != 0;
		anim = data->fanimList->Next())
	{
		w_face_animation* copy = new w_face_animation;
		*copy = *anim;
		copy->mesh = (w_mesh**)g_mem.Alloc(anim->meshNum * 4 + 4);
		copy->mesh[anim->meshNum] = 0;
		if (anim->meshNum > 0)
			m_rootbone->GetMeshForFaceAnimation((unsigned char)copy->group,
				copy->mesh, 0);
		m_fanimList.AddItem(copy, copy->name, false);
	}

	SetRenderMode(m_rendMode);
}

const WMatrix& WPuppet::GetCamMatrix(char* name)
{
	static WMatrix convMat(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f,
		0.0f);
	static WMatrix mat;

	mat = convMat * FindBone(name)->GetMatrix();
	return mat;
}

float WPuppet::GetCamFOV(char* name, WBoneSet* set)
{
	WBoneKey* key = set->FindBoneKey(name);
	if (key && (key->flags & 4) != 0)
		return key->scale;
	return 0.0f;
}

WPuppet* WPuppet::MakeClone(bool clone)
{
	w_share_pet_data data;
	data.petName = m_petName;
	data.rootbone = m_rootbone;
	data.length = m_aniLen;
	data.faceNum = m_faceNum;
	data.numFramedata = m_framenum;
	data.frame = m_frame;
	data.bound = m_bound;
	data.bbList = &m_bbList;
	data.mdList = m_mdList;
	data.fanimList = &m_fanimList;

	WPuppet* pet = new WPuppet();
	pet->m_iPetType = m_iPetType;
	pet->m_rendMode = m_rendMode;
	pet->Share(&data);
	return pet;
}

void WPuppet::SetRenderMode(int mode)
{
	m_rendMode = mode;
	if (m_rootbone)
		m_rootbone->SetRenderMode(mode);
}

void WPuppet::Transform(WView* view, float value, int flag)
{
	m_rootbone->Transform(m_mat, view, value, flag);
}

void WPuppet::UpdateLightSource(WScene* scene)
{
	m_rootbone->CalcLight(scene, m_lightmode);
}

void WPuppet::UpdateLightSource(LightSet* light, bool enable, WScene* scene)
{
	m_rootbone->SetLight(light);
	int mode = m_lightmode;
	if (enable)
		mode = mode | 0x8000000;
	m_rootbone->CalcLight(light, true, mode, scene);
}

void WPuppet::Render(WView* view, bool transform, float value, bool light,
	bool shadow, bool unused)
{
	if (transform)
	{
		int flag = (light != false ? 1 : 0) | (shadow ? 0 : 2);
		m_rootbone->Transform(m_mat, view, value, flag);
	}
	m_rootbone->Render(view, m_rootbone->m_light);
}

void WPuppet::RenderHierarchy(WView* view, bool transform, float value,
	float scale)
{
	if (transform)
		m_rootbone->Transform(m_mat, view, value, 0);
	m_rootbone->RenderHierarchy(view, scale);
}

void WPuppet::RenderNormals(WView* view, bool transform, float value,
	float scale)
{
	if (transform)
		m_rootbone->Transform(m_mat, view, value, 0);
	m_rootbone->RenderNormals(view, scale);
}

float WPuppet::GetMotionLength(const char* name)
{
	if (name != 0)
	{
		w_motion_data* data = FindMotionData(name);
		if (data == 0)
			return -1.0f;
		return data->end - data->s;
	}
	return m_aniLen;
}

WBoneSet* WPuppet::GetBones(const char* name, float time, int mode, char* bone,
	bool flag)
{
	if (name != 0)
	{
		w_motion_data* data = FindMotionData(name);
		if (bone == 0 && *data->applybone != 0)
			bone = data->applybone;
		time = time + data->s;
		if (!(time > data->e))
		{
			flag = false;
		}
		else
		{
			float ratio = (time - data->e) / (data->end - data->e);
			switch (data->option1)
			{
			case 2:
				return BlendBoneSet(GetBones(0, time, mode, bone, false),
					GetBones(data->nextmotion, time - data->e, mode, bone,
						false),
					ratio);
			case 1:
				return BlendBoneSet(GetBones(0, data->e, mode, bone, false),
					GetBones(data->nextmotion, 0.0f, mode, bone, false), ratio);
			}
		}
	}
	if (time > m_aniLen)
		time = m_aniLen;
	WBoneSet* set = GetFreeBoneSetList(flag);
	for (w_motion_addition* add = m_addedMotionList.Start(); add != 0;
		add = m_addedMotionList.Next())
	{
		if (add->s <= time && time <= add->e)
		{
			if (bone != 0)
			{
				WBone* found = add->rootbone->FindBone(bone, 0);
				if ((mode & 4) == 0)
				{
					found->InsertBoneSetList(time - add->s, set, 1);
				}
				else
				{
					for (WBone* child = found->m_child; child != 0;
						child = child->m_next)
						child->InsertBoneSetList(time - add->s, set, 1);
				}
			}
			else
			{
				for (WBone* item = add->rootbone; item != 0;
					item = item->m_next)
					item->InsertBoneSetList(time - add->s, set, 1);
			}
			goto keyed;
		}
	}
	if (bone != 0)
	{
		WBone* found = m_boneList.Find(bone);
		if (found == 0)
		{
			found = m_rootbone->FindBone(bone, 0);
			if (found != 0)
				m_boneList.AddItem(found, found->m_name, false);
		}
		if ((mode & 4) == 0)
		{
			found->InsertBoneSetList(time, set, 1);
		}
		else
		{
			for (WBone* child = found->m_child; child != 0;
				child = child->m_next)
				child->InsertBoneSetList(time, set, 1);
		}
	}
	else
	{
		for (WBone* item = m_rootbone; item != 0; item = item->m_next)
			item->InsertBoneSetList(time, set, 1);
	}
keyed:
	if (mode & 1)
	{
		WBoneKey* key = set->FindBoneKey(m_rootbone->GetBoneMotionName());
		if (key != 0 && (key->flags & 2) != 0)
			key->pivot = m_rootbone->GetBasicMatrix().pivot;
	}
	return set;
}

w_motion_data* WPuppet::FindMotionData(const char* name)
{
	if (m_mdList == 0)
		return 0;

	w_motion_data* data = m_mdList->Find(name);
	if (data == 0)
	{
		for (w_motion_addition* add = m_addedMotionList.Start();
			add != 0 && data == 0; add = m_addedMotionList.Next())
			data = add->mdList->Find(name);
	}
	return data;
}

int WPuppet::GetMotionCountIncludeName(const char* include, const char* exclude)
{
	if (m_mdList == 0)
		return 0;

	int count = 0;
	w_motion_data* md;
	for (md = m_mdList->Start(); md != 0; md = m_mdList->Next())
	{
		if (strstr(md->name, include) != 0 && strstr(md->name, exclude) == 0)
			++count;
	}
	if (count == 0)
	{
		for (w_motion_addition* add = m_addedMotionList.Start();
			add != 0 && md == 0; add = m_addedMotionList.Next())
		{
			for (md = m_mdList->Start(); md != 0; md = m_mdList->Next())
			{
				if (strstr(md->name, include) != 0 &&
					strstr(md->name, exclude) == 0)
					++count;
			}
		}
	}
	return count;
}

char* WPuppet::GetNextMotionName(const char* name)
{
	w_motion_data* data = FindMotionData(name);
	if (data->s == data->e && strcmp(data->name, data->nextmotion) == 0)
		return 0;
	return data->nextmotion;
}

void WPuppet::ApplyBones(WBoneSet* set)
{
	if (m_rootbone)
		m_rootbone->ApplyBoneSetList(set);
}

void WPuppet::ApplyBones(WBoneSet* set, const WMatrix& matrix)
{
	m_mat = matrix;
	if (set && m_rootbone)
		m_rootbone->ApplyBoneSetList(set);
}

WVector WPuppet::GetDeltaVec(float time)
{
	for (w_motion_addition* add = m_addedMotionList.Start(); add != 0;
		add = m_addedMotionList.Next())
	{
		if (add->s <= time && time <= add->e)
		{
			return strcmp(add->rootbone->GetBoneMotionName(),
					   m_rootbone->GetBoneMotionName()) == 0
				? add->rootbone->GetDeltaVec(time - add->s) +
					WVector(0.0f, add->h_gap, 0.0f)
				: WVector::ZERO;
		}
	}
	return m_rootbone->GetDeltaVec(time);
}

WVector WPuppet::GetDeltaVec(char* name, float time)
{
	w_motion_data* data = FindMotionData(name);
	return GetDeltaVec(time + data->s);
}

WVector WPuppet::GetDeltaVec(char* name, float from, float to)
{
	if (from == to)
		return WVector::ZERO;
	w_motion_data* data;
	if (name != 0)
	{
		data = FindMotionData(name);
		float length = data->e - data->s;
		float span = length > to ? to : length;
		if (to > span)
		{
			if (from < span)
				return GetDeltaVec(name, from, span) +
					GetDeltaVec(name, span, to);
			if (from >= span)
			{
				float ratio = (to - data->e) / (data->end - data->e);
				switch (data->option1)
				{
				case 1:
					return WVector::ZERO;
				case 2:
					return GetDeltaVec(0, from + data->s, to + data->s) *
						(1.0f - ratio) +
						GetDeltaVec(data->nextmotion, from - data->e,
							to - data->e) *
						ratio;
				}
			}
		}
	}
	return name ? GetDeltaVec(to + data->s) - GetDeltaVec(from + data->s)
				: GetDeltaVec(to) - GetDeltaVec(from);
}

WBoneSet* WPuppet::GetFreeBoneSetList(bool flag)
{
	static WBoneSet boneSetList[8] = { WBoneSet(false) };
	ms_boneSetPos = (ms_boneSetPos + 1) & 7;
	if (ms_boneSetPos == ms_lockedBoneSetPos)
		ms_boneSetPos = (ms_boneSetPos + 1) & 7;
	if (flag)
		ms_lockedBoneSetPos = ms_boneSetPos;
	boneSetList[ms_boneSetPos].Clear();
	return &boneSetList[ms_boneSetPos];
}

void WPuppet::SetDefaultFaceAnimation(void)
{
	w_face_animation* anim = new w_face_animation;
	strcpy(anim->name, "\261\342\272\273");
	anim->texHandle = 0;
	anim->tu = 0.0f;
	anim->tv = 0.0f;
	anim->mesh = 0;
	anim->group = 0;
	anim->meshNum = m_rootbone->GetMeshForFaceAnimation(0, 0, 0);
	anim->mesh = (w_mesh**)g_mem.Alloc(anim->meshNum * 4 + 4);
	m_rootbone->GetMeshForFaceAnimation(0, anim->mesh, 0);
	anim->mesh[anim->meshNum] = 0;
	m_fanimList.AddItem(anim, anim->name, false);
}

char* WPuppet::FindTexAniList(char* name)
{
	char* buffer = 0;
	if (m_fanimList.Find(name) == 0)
		return 0;

	int group = m_fanimList.Find(name)->group;
	for (int pass = 0, len = 0; pass < 2; ++pass)
	{
		if (pass == 1)
		{
			buffer = new char[len + pass];
			*(int*)buffer = 0;
		}
		for (w_face_animation* anim = m_fanimList.Start(); anim != 0;
			anim = m_fanimList.Next())
		{
			if (anim->group != group)
				continue;
			if (strcmpi(anim->name, name) != 0)
			{
				if (pass == 0)
				{
					len += (int)strlen(anim->name) + 1;
				}
				else
				{
					strcat(buffer, anim->name);
					strcat(buffer, " ");
				}
			}
		}
	}
	return buffer;
}

void WPuppet::AddCollisionObject(const char* name, WBlockModel* model)
{
	WBone* bone = m_rootbone->FindBone(name, 0);
	bone->GetConvexArea(model);
}

void WPuppet::SetSelfIllum(char* name, bool enabled)
{
	if (name != 0 && strcmpi(name, "Recursive") != 0)
	{
		WBone* bone = FindBone(name);
		if (bone)
			bone->SetSelfIllum(enabled, false);
		return;
	}
	m_rootbone->SetSelfIllum(enabled, true);
}

void WPuppet::SetSelfIllumColor(char* name, unsigned long color)
{
	if (name != 0 && strcmpi(name, "Recursive") != 0)
	{
		WBone* bone = FindBone(name);
		if (bone)
			bone->SetSelfIllumColor(color, false);
		return;
	}
	m_rootbone->SetSelfIllumColor(color, true);
}

void WPuppet::ShowBone(char* name)
{
	if (name != 0 && strcmp(name, "Recursive") != 0)
	{
		WBone* bone = FindBone(name);
		if (bone)
			bone->ShowBone(false);
		return;
	}
	m_rootbone->ShowBone(true);
}

void WPuppet::HideBone(char* name)
{
	if (name != 0 && strcmp(name, "Recursive") != 0)
	{
		WBone* bone = FindBone(name);
		if (bone)
			bone->HideBone(false);
		return;
	}
	m_rootbone->HideBone(true);
}

unsigned char WPuppet::GetAlpha(char* name)
{
	if (name != 0)
		return FindBone(name)->m_alpha;
	return m_rootbone->m_alpha;
}

void WPuppet::SetAlpha(unsigned char alpha)
{
	m_rootbone->SetAlpha(alpha, 1);
}

void WPuppet::SetLightLine(float value)
{
	m_rootbone->SetLightLine(value);
}

void WPuppet::SetAlpha(char* name, unsigned char alpha)
{
	if (strcmpi(name, "Recursive") == 0)
	{
		m_rootbone->SetAlpha(alpha, 1);
		return;
	}
	WBone* bone = FindBone(name);
	if (bone)
		bone->SetAlpha(alpha, 0);
}

void WPuppet::ToggleSelfIllum(char* name)
{
	if (name != 0)
	{
		FindBone(name)->ToggleSelfIllum();
	}
}

void WPuppet::SetMirrored(bool mirrored)
{
	if (m_rootbone)
		m_rootbone->SetMirrored(mirrored);
}

void WPuppet::SetBoundBox(char* name, Waabb* aabb, char* bonename)
{
	w_bound_box* box = (w_bound_box*)g_mem.Alloc(8);
	box->info = (w_bound_box_info*)g_mem.Alloc((int)strlen(name) + 48);
	box->bone = m_rootbone->FindBone(bonename, 0);
	box->info->aabb = *aabb;
	if (box->info->aabb.max.x - box->info->aabb.min.x < 0.001f)
		box->info->aabb.max.x += 0.001f;
	if (box->info->aabb.max.y - box->info->aabb.min.y < 0.001f)
		box->info->aabb.max.y += 0.001f;
	if (box->info->aabb.max.z - box->info->aabb.min.z < 0.001f)
		box->info->aabb.max.z += 0.001f;
	box->info->option = 0;
	box->info->spherePivot = (aabb->min + aabb->max) * 0.5f;
	box->info->sphereLength = (aabb->max - aabb->min).Magnitude() * 0.5f;
	strcpy(box->info->name, name);
	m_bbList.AddItem(box, box->info->name, false);
}

WBone* WPuppet::FindBone(const char* name)
{
	WBone* bone = m_boneList.Find(name);
	if (bone == 0)
	{
		bone = m_rootbone->FindBone(name, 0);
		if (bone)
			m_boneList.AddItem(bone, bone->m_name, false);
	}
	return bone;
}

WBone* WPuppet::FindBone(int id)
{
	return m_rootbone->FindBone(id);
}

bool WPuppet::GetBoundBoxPlane(char* name, WPlane* plane)
{
	w_bound_box* box = m_bbList.Find(name);
	if (box)
	{
		MakePlaneEq(plane, box->info->aabb, &box->bone->m_matrix);
		return true;
	}
	return false;
}

bool WPuppet::GetAlignBoundBoxPlane(char* name, WPlane* plane, Waabb* box)
{
	Waabb aabb;
	aabb.min.x = aabb.min.y = aabb.min.z = 0.0f;
	aabb.max.x = aabb.max.y = aabb.max.z = 0.0f;

	w_bound_box* bound = m_bbList.Find(name);
	if (bound != 0)
	{
		WVector corner = WVector(bound->info->aabb.max.x,
							 bound->info->aabb.max.y, bound->info->aabb.max.z) *
			bound->bone->m_matrix;
		aabb.min = aabb.max = corner;

		for (int i = 1; i < 8; ++i)
		{
			float z = (i & 4) != 0 ? bound->info->aabb.min.z
								   : bound->info->aabb.max.z;
			float y = (i & 2) != 0 ? bound->info->aabb.min.y
								   : bound->info->aabb.max.y;
			float x = (i & 1) != 0 ? bound->info->aabb.min.x
								   : bound->info->aabb.max.x;
			WVector vec;
			vec = WVector(x, y, z) * bound->bone->m_matrix;
			aabb += WVector(vec);
		}

		if (plane != 0)
			MakePlaneEq(plane, aabb, 0);
		if (box != 0)
			*box = aabb;
		return true;
	}
	return false;
}

bool WPuppet::DotContact(char* name, const WVector& dot)
{
	w_bound_box* box = m_bbList.Find(name);
	WVector corner[8];
	int i;
	for (i = 0; i < 8; ++i)
	{
		float z = box_v[i][2] ? box->info->aabb.max.z : box->info->aabb.min.z;
		float y = box_v[i][1] ? box->info->aabb.max.y : box->info->aabb.min.y;
		float x = box_v[i][0] ? box->info->aabb.max.x : box->info->aabb.min.x;
		corner[i] = WVector(x, y, z) * box->bone->m_matrix;
	}
	for (int f = 0; f < 6; ++f)
	{
		if (WPlane(WCrossProduct(corner[box_p[f][0]] - corner[box_p[f][1]],
					   corner[box_p[f][1]] - corner[box_p[f][2]]),
				corner[box_p[f][3]]) *
				dot >
			0.05f)
			return false;
	}
	return true;
}

void WPuppet::GetBoneName(char** names, int& count)
{
	m_rootbone->GetName(names, count);
}

WVector WPuppet::GetPivot(const char* name)
{
	if (name != 0)
	{
		WBone* bone = FindBone(name);
		if (bone)
			return bone->GetPivot();
	}
	return m_rootbone->GetPivot();
}

WMatrix WPuppet::GetRootMatrix()
{
	return m_rootbone->m_localMat;
}

WBone* WPuppet::FindBoneByTail(const char* tail)
{
	if (m_rootbone)
		return m_rootbone->FindBoneByTail(tail);
	return 0;
}

bool WPuppet::CheckFrameData(int first, int last, int& from, int& to)
{
	int index;
	for (index = 0; index < m_framenum; ++index)
		if (first <= m_frame[index].num)
			break;
	if (index < m_framenum && m_frame[index].num <= last)
	{
		from = index;
		to = index;
		while (to + 1 < m_framenum && m_frame[to + 1].num <= last)
			++to;
		return true;
	}
	return false;
}

void WPuppet::MakeTemporaryCenterBox(void)
{
	Waabb box;
	box.min.x = box.min.y = box.min.z = 0.0f;
	box.max.x = box.max.y = box.max.z = 0.0f;

	if (m_bbList.Find("center") == 0)
	{
		box = m_bound;
		SetBoundBox("center", &box, m_rootbone->m_name);
		m_bHaveCenter = false;
		if (m_BBox.bone != 0)
		{
			w_bound_box* found = m_bbList.Find("center");
			m_BBox.bone = found->bone;
			m_BBox.info = found->info;
		}
	}
	else
	{
		m_bHaveCenter = true;
	}
}

void WPuppet::OptimizeBone(void)
{
	for (WBone* bone = m_rootbone; bone; bone = bone->m_next)
		ReduceBone(bone);
}

bool WPuppet::ReduceBone(WBone* bone)
{
	int reduced = CheckStaticBone(bone) ? 1 : 0;
	WBone* child = bone->m_child;
	while (child != 0)
	{
		if (ReduceBone(child) == true)
		{
			child = MergeBone(bone, child);
		}
		else
		{
			child = child->m_next;
			reduced = false;
		}
	}
	return (bool)reduced;
}

bool WPuppet::CheckStaticBone(WBone* bone)
{
	if (bone->HasAnimation() || bone->m_parent == 0)
		return false;

	for (w_motion_data* motion = m_mdList->Start(); motion != 0;
		motion = m_mdList->Next())
	{
		if (strcmp(motion->applybone, bone->m_name) == 0)
			return false;
	}
	for (w_bound_box* box = m_bbList.Start(); box != 0; box = m_bbList.Next())
	{
		if (box->bone == bone)
			return false;
	}
	if (m_rootbone->CheckRigidVtx(bone))
		return false;
	return true;
}

WBone* WPuppet::MergeBone(WBone* target, WBone* source)
{
	WBone* next = source->m_next;
	WBone* parent = source->m_parent;
	parent->ReleaseChild(source);

	WSphere sphere;
	sphere = target->GetBoundSphere();
	float radius = sphere.radius;
	sphere.pos = parent->Transform(sphere.pos);
	Waabb box;
	box = Waabb(WVector(sphere.pos.x - radius, sphere.pos.y - radius,
					sphere.pos.z - radius),
		WVector(radius + sphere.pos.x, radius + sphere.pos.y,
			radius + sphere.pos.z));

	target->MergeMesh(source, m_rootbone);
	target->SetBoundBox(box, true);
	source->ReleaseLink();
	delete source;
	return next;
}

void WPuppet::CalcBound(Waabb* box, bool flag)
{
	WVector min;
	WVector max;
	min.x = 99999.0f;
	min.y = 99999.0f;
	min.z = 99999.0f;
	max.x = -99999.0f;
	max.y = -99999.0f;
	max.z = -99999.0f;
	if (box == 0)
	{
		WMatrix mat;
		mat.Reset();
		if (m_mdList != 0)
		{
			for (w_motion_data* data = m_mdList->Start(); data != 0;
				data = m_mdList->Next())
			{
				WBoneSet* set = GetBones(0, data->s, flag, 0, false);
				ApplyBones(set, mat);
				m_rootbone->Transform(m_mat, 0, 1.0f, 0);
				m_rootbone->GetTotalOverlapBox(&min, &max, 0, false);
				set = GetBones(0, data->e, flag, 0, false);
				ApplyBones(set, mat);
				m_rootbone->Transform(m_mat, 0, 1.0f, 0);
				m_rootbone->GetTotalOverlapBox(&min, &max, 0, false);
			}
		}
		WBoneSet* set = GetBones(0, 0.0f, flag, 0, false);
		ApplyBones(set, mat);
		m_rootbone->Transform(m_mat, 0, 1.0f, 0);
		m_rootbone->GetTotalOverlapBox(&min, &max, 0, false);
		m_bound = Waabb(min, max);
	}
	else
	{
		m_bound = *box;
	}
	m_boundSphere.pos = (m_bound.min + m_bound.max) * 0.5f;
	m_boundSphere.radius =
		(float)sqrt((m_boundSphere.pos - m_bound.min).SquareMagnitude());
}

void WPuppet::CalcBound(bool reset, WPuppet* puppet, bool mode, char* name,
	Waabb* box)
{
	WVector min;
	WVector max;
	min.x = 99999.0f;
	min.y = 99999.0f;
	min.z = 99999.0f;
	max.x = -99999.0f;
	max.y = -99999.0f;
	max.z = -99999.0f;
	if (reset)
	{
		m_bound.min.x = m_bound.min.y = m_bound.min.z = 0.0f;
		m_bound.max.x = m_bound.max.y = m_bound.max.z = 0.0f;
	}
	if (box != 0)
	{
		box->min = box->max = WVector::ZERO;
	}
	WMatrix mat;
	mat.Reset();
	WPuppet* src;
	WList<w_motion_data*>* list;
	if (puppet != 0)
	{
		src = puppet;
		list = puppet->m_mdList;
	}
	else
	{
		src = this;
		list = m_mdList;
	}
	if (list != 0)
	{
		for (w_motion_data* data = list->Start(); data != 0;
			data = list->Next())
		{
			if (name != 0 && strcmp(data->name, name) != 0)
				continue;
			WBoneSet* set = src->GetBones(0, data->s, mode, 0, false);
			m_mat = mat;
			if (set != 0 && m_rootbone != 0)
				m_rootbone->ApplyBoneSetList(set);
			m_rootbone->Transform(m_mat, 0, 1.0f, 0);
			m_rootbone->GetTotalOverlapBox(&min, &max, 0, false);
			if (data->e != data->s)
			{
				set = src->GetBones(0, data->e, mode, 0, false);
				m_mat = mat;
				if (set != 0 && m_rootbone != 0)
					m_rootbone->ApplyBoneSetList(set);
				m_rootbone->Transform(m_mat, 0, 1.0f, 0);
				m_rootbone->GetTotalOverlapBox(&min, &max, 0, false);
				if (name != 0)
					break;
			}
		}
	}
	if (name == 0)
	{
		WBoneSet* set = src->GetBones(0, 0.0f, mode, 0, false);
		m_mat = mat;
		if (set != 0 && m_rootbone != 0)
			m_rootbone->ApplyBoneSetList(set);
		m_rootbone->Transform(m_mat, 0, 1.0f, 0);
		m_rootbone->GetTotalOverlapBox(&min, &max, 0, false);
	}
	if (box == 0)
	{
		min.x = Min(m_bound.min.x, min.x);
		max.x = Max(m_bound.max.x, max.x);
		min.y = Min(m_bound.min.y, min.y);
		max.y = Max(m_bound.max.y, max.y);
		min.z = Min(m_bound.min.z, min.z);
		max.z = Max(m_bound.max.z, max.z);
		m_bound.min = min;
		m_bound.max = max;
		m_boundSphere.pos = (max + min) * 0.5f;
		m_boundSphere.radius =
			(float)sqrt((m_boundSphere.pos - min).SquareMagnitude());
	}
	else
	{
		box->min = min;
		box->max = max;
	}
}

Waabb WPuppet::GetTransformedBoundBox(void) const
{
	Waabb bound;
	WVector vtx[8] = { WVector(m_bound.min.x, m_bound.min.y, m_bound.min.z),
		WVector(m_bound.max.x, m_bound.min.y, m_bound.min.z),
		WVector(m_bound.min.x, m_bound.max.y, m_bound.min.z),
		WVector(m_bound.min.x, m_bound.min.y, m_bound.max.z),
		WVector(m_bound.min.x, m_bound.max.y, m_bound.max.z),
		WVector(m_bound.max.x, m_bound.min.y, m_bound.max.z),
		WVector(m_bound.max.x, m_bound.max.y, m_bound.min.z),
		WVector(m_bound.max.x, m_bound.max.y, m_bound.max.z) };
	for (int i = 0; i < 8; ++i)
		vtx[i] = vtx[i] * m_mat;
	bound.min = WVector(3.402823466e38F, 3.402823466e38F, 3.402823466e38F);
	bound.max = WVector(-3.402823466e38F, -3.402823466e38F, -3.402823466e38F);
	for (int j = 0; j < 8; ++j)
	{
		if (vtx[j].x < bound.min.x)
			bound.min.x = vtx[j].x;
		if (vtx[j].y < bound.min.y)
			bound.min.y = vtx[j].y;
		if (vtx[j].z < bound.min.z)
			bound.min.z = vtx[j].z;
		if (vtx[j].x > bound.max.x)
			bound.max.x = vtx[j].x;
		if (vtx[j].y > bound.max.y)
			bound.max.y = vtx[j].y;
		if (vtx[j].z > bound.max.z)
			bound.max.z = vtx[j].z;
	}
	return bound;
}

void WPuppet::UpdateBound(bool transform, bool flag)
{
	WVector vMin(99999.0f, 99999.0f, 99999.0f);
	WVector vMax(-99999.0f, -99999.0f, -99999.0f);
	if (transform)
		m_rootbone->Transform(m_mat, 0, 1.0f, 0);
	m_rootbone->GetTotalOverlapBox(&vMin, &vMax, 0, flag);
	m_bound = Waabb(vMin, vMax);
	m_boundSphere.pos = (m_bound.max + m_bound.min) * 0.5f;
	m_boundSphere.radius =
		(float)sqrt((m_bound.max - m_bound.min).SquareMagnitude()) * 0.5f;
}

void WPuppet::UpdateBSphere(void)
{
	WBone* bone = m_BBox.bone;
	if (bone)
	{
		WMatrix* matrix = &bone->m_matrix;
		w_bound_box_info* info = m_BBox.info;
		WVector min;
		WVector max;
		min = info->aabb.min * *matrix;
		max = info->aabb.max * *matrix;
		m_boundSphere.pos = (min + max) * 0.5f;
		m_boundSphere.radius = (max - m_boundSphere.pos).Magnitude();
	}
}

void WPuppet::SetVtxColor(unsigned long color)
{
	m_rootbone->SetVtxColor(color);
}

void WPuppet::UpdatePhysics(float delta, const WVector& accel)
{
	for (WBone* bone = m_phybone.Start(); bone; bone = m_phybone.Next())
		bone->UpdatePhysicsModel(delta, accel);
}

void WPuppet::SetPhysicsModel(const char* name, int model)
{
	WBone* bone = m_boneList.Find(name);
	if (bone == 0)
	{
		bone = m_rootbone->FindBone(name, 0);
		if (bone != 0)
			m_boneList.AddItem(bone, bone->m_name, false);
	}
	bone->SetPhysicsModel(model);
	m_phybone += bone;
}

void WPuppet::RotateBone(char* name, float angle, int axis)
{
	char* key = *name == '@' ? name + 1 : name;
	WBone* bone = m_boneList.Find(key);
	if (bone == 0)
	{
		bone = m_rootbone->FindBone(key, 0);
		if (bone == 0)
			return;
		m_boneList.AddItem(bone, bone->m_name, false);
	}
	bone->Rotate(angle, axis, *name != '@');
}

void WPuppet::MoveBone(char* name, WVector& vector)
{
	if (strcmpi(name, "Recursive") == 0)
	{
		m_rootbone->Move(vector, true);
		return;
	}
	WBone* bone = FindBone(name);
	if (bone)
		bone->Move(vector, false);
}

void WPuppet::RenderBoundBox(const char* name, WView* view, unsigned long color,
	float size)
{
	static unsigned long box_color[6] = { 0xffffff00, 0xff00ff00, 0xffffff00,
		0xffff0000, 0xff0000ff, 0xffffff00 };
	static WVector emptyMin(99999.0f, 99999.0f, 99999.0f);
	static WVector emptyMax(-99999.0f, -99999.0f, -99999.0f);
	static WVector under(-0.01f, -0.01f, -0.01f);
	static WVector over(0.01f, 0.01f, 0.01f);

	w_bound_box* box = m_bbList.Find(name);
	if (box == 0)
		return;
	WVector v[8];
	WVector a, b, normal, center;
	int i;
	if (WisEqual(box->info->aabb.min, emptyMin, 0.00001f) != 0 &&
		WisEqual(box->info->aabb.max, emptyMax, 0.00001f) != 0)
	{
		for (i = 0; i < 8; ++i)
			v[i] = box->bone->Transform(WVector(
				box_v[i][0] ? (float)(over.x + size) : (float)(under.x - size),
				box_v[i][1] ? (float)(over.y + size) : (float)(under.y - size),
				box_v[i][2] ? (float)(over.z + size)
							: (float)(under.z - size)));
	}
	else
	{
		for (i = 0; i < 8; ++i)
		{
			WVector p(box_v[i][0] ? (float)(size + box->info->aabb.max.x)
								  : (float)(box->info->aabb.min.x - size),
				box_v[i][1] ? (float)(size + box->info->aabb.max.y)
							: (float)(box->info->aabb.min.y - size),
				box_v[i][2] ? (float)(size + box->info->aabb.max.z)
							: (float)(box->info->aabb.min.z - size));
			v[i] = p * box->bone->m_matrix;
		}
	}
	for (int f = 0; f < 6; ++f)
	{
		center = WVector::ZERO;
		if (WisEqual(box->info->aabb.min, emptyMin, 0.00001f) != 0 &&
			WisEqual(box->info->aabb.max, emptyMax, 0.00001f) != 0)
		{
			for (i = 0; i < 4; ++i)
				center += v[box_p[f][i]];
		}
		else
		{
			for (i = 0; i < 4; ++i)
			{
				view->DrawLine(v[box_p[f][(i + 1) % 4]], color, v[box_p[f][i]],
					color, 0);
				center += v[box_p[f][i]];
			}
		}
		a = v[box_p[f][0]] - v[box_p[f][1]];
		b = v[box_p[f][1]] - v[box_p[f][2]];
		a.Normalize();
		b.Normalize();
		float dot = a * b;
		if (dot > -0.999f && dot < 0.999f)
		{
			normal = WCrossProduct(a, b);
			normal.Normalize();
			center *= 0.25f;
			view->DrawLine(center, box_color[f],
				center + normal * (15.0f * g_CM_TO_WU), box_color[f], 0);
			view->DrawLine(center + normal * (15.0f * g_CM_TO_WU), box_color[f],
				center + normal * (10.0f * g_CM_TO_WU) +
					a * (3.0f * g_CM_TO_WU),
				box_color[f], 0);
			view->DrawLine(center + normal * (15.0f * g_CM_TO_WU), box_color[f],
				center + normal * (10.0f * g_CM_TO_WU) -
					a * (3.0f * g_CM_TO_WU),
				box_color[f], 0);
		}
	}
}

void WPuppet::ChangeTexture(char* name, int handle)
{
	w_face_animation* anim = m_fanimList.Find(name);
	if (anim == 0)
		return;

	w_mesh** list = anim->mesh;
	for (int i = 0; list[i] != 0; ++i)
	{
		float su = anim->scalex / list[i]->scaleu;
		float sv = anim->scaley / list[i]->scalev;
		float du = anim->tu - su * list[i]->tu;
		float dv = anim->tv - sv * list[i]->tv;
		list[i]->texHandle = handle;
		int tex = list[i]->texHandle;
		list[i]->drawFlag ^= (tex ^ list[i]->drawFlag) & 0x7ff;
		list[i]->tu = anim->tu;
		list[i]->tv = anim->tv;
		list[i]->scaleu = anim->scalex;
		list[i]->scalev = anim->scaley;
		if (Wabs(du - 0.0f) < 0.00001f && Wabs(dv - 0.0f) < 0.00001f &&
			Wabs(su - 1.0f) < 0.00001f && Wabs(sv - 1.0f) < 0.00001f)
			continue;
		for (int v = 0; v < list[i]->vtxNum; ++v)
		{
			list[i]->uvData[v][0] = su * list[i]->uvData[v][0] + du;
			list[i]->uvData[v][1] = sv * list[i]->uvData[v][1] + dv;
		}
	}
}

bool WPuppet::ChangeTexturePart(char* name, char* part, Bitmap** bitmap,
	tagRECT* rect)
{
	w_face_animation* anim = m_fanimList.Find(name);
	if (anim == 0)
		return false;
	WResourceManager* mng = GetResrcManager();
	return mng->ChangeTexturePart(anim->filename, part, bitmap, rect);
}

void WPuppet::ChangeTexturePart(char* name, Bitmap* bitmap, const tagRECT& rect)
{
	w_face_animation* anim = m_fanimList.Find(name);
	if (anim != 0)
		GetResrcManager()->ChangeTexturePart(anim->filename, bitmap, rect);
}

bool WPuppet::ChangeTexturePart(int handle, char* name, Bitmap** bitmap,
	tagRECT* rect)
{
	return GetResrcManager()->ChangeTexturePart(handle, name, bitmap, rect);
}

void WPuppet::ChangeTexturePart(int handle, Bitmap* bitmap, const tagRECT& rect)
{
	GetResrcManager()->ChangeTexturePart(handle, bitmap, rect);
}

void WPuppet::ApplyFaceAnimation(char* name, float time)
{
	float su, sv, du, dv;
	if (name != 0)
	{
		w_face_animation* anim = m_fanimList.Find(name);
		if (anim == 0)
			return;
		w_mesh** list = anim->mesh;
		for (int i = 0; list[i] != 0; ++i)
		{
			su = anim->scalex / list[i]->scaleu;
			sv = anim->scaley / list[i]->scalev;
			du = anim->tu - su * list[i]->tu;
			dv = anim->tv - sv * list[i]->tv;
			list[i]->texHandle = anim->texHandle;
			int tex = list[i]->texHandle;
			list[i]->drawFlag ^= (tex ^ list[i]->drawFlag) & 0x7ff;
			list[i]->tu = anim->tu;
			list[i]->tv = anim->tv;
			list[i]->scaleu = anim->scalex;
			list[i]->scalev = anim->scaley;
			if (WisEqual(du, 0.0f, 0.00001f) != 0 &&
				WisEqual(dv, 0.0f, 0.00001f) != 0 &&
				WisEqual(su, 1.0f, 0.00001f) != 0 &&
				WisEqual(sv, 1.0f, 0.00001f) != 0)
				continue;
			for (int v = 0; v < list[i]->vtxNum; ++v)
			{
				list[i]->uvData[v][0] = su * list[i]->uvData[v][0] + du;
				list[i]->uvData[v][1] = sv * list[i]->uvData[v][1] + dv;
			}
		}
	}
	else
	{
		w_face_animation* anim = m_fanimList.Find("\261\342\272\273");
		if (anim == 0)
			return;
		w_mesh** list = anim->mesh;
		for (int i = 0; list[i] != 0; ++i)
		{
			su = list[i]->originalTu / list[i]->scaleu;
			sv = list[i]->originalTv / list[i]->scalev;
			du = list[i]->originalTu - su * list[i]->tu;
			dv = list[i]->originalTv - sv * list[i]->tv;
			list[i]->texHandle = list[i]->originalTexHandle;
			int tex = list[i]->texHandle;
			list[i]->drawFlag ^= (tex ^ list[i]->drawFlag) & 0x7ff;
			list[i]->tu = list[i]->originalTu;
			list[i]->tv = list[i]->originalTv;
			list[i]->scaleu = list[i]->originalScaleu;
			list[i]->scalev = list[i]->originalScalev;
			if (WisEqual(du, 0.0f, 0.00001f) != 0 &&
				WisEqual(dv, 0.0f, 0.00001f) != 0 &&
				WisEqual(su, 1.0f, 0.00001f) != 0 &&
				WisEqual(sv, 1.0f, 0.00001f) != 0)
				continue;
			for (int v = 0; v < list[i]->vtxNum; ++v)
			{
				list[i]->uvData[v][0] = su * list[i]->uvData[v][0] + du;
				list[i]->uvData[v][1] = sv * list[i]->uvData[v][1] + dv;
			}
		}
	}
}

int WPuppet::SeekChunkFromPET(cFile* file, char* tag)
{
	int pos = 0;
	while (pos < file->m_nLen)
	{
		file->Seek(pos, 0);
		char head[8];
		file->Read(head, 8);
		if (memcmp(tag, head, 4) == 0)
		{
			file->Seek(pos + 8, 0);
			return 1;
		}
		pos += *(int*)(head + 4) + 8;
	}
	return 0;
}

inline int ReadID(cFile* file);

WBone* WPuppet::LoadPET_Bone(cFile* file, int version)
{
	char name[256];
	WBone* root;
	WBone** bones;
	int count;
	WMatrix mat;
	WMatrix* mats;

	root = 0;
	if (SeekChunkFromPET(file, "BONE") == 0)
		return 0;
	mat.Reset();
	count = file->GetByte();
	if (count == 0)
		file->Read(&count, 2);
	bones = new WBone*[count];
	mats = new WMatrix[count];
	for (int i = 0; i < count; ++i)
	{
		name[0] = (char)file->GetByte();
		if (name[0])
		{
			for (char* p = name; *p; *++p = (char)file->GetByte())
			{
			}
		}
		bones[i] = new WBone;
		bones[i]->SetName(name);
		bones[i]->SetIDNumber(i);
		int parent = ReadID(file);
		if (root != 0)
		{
			if (parent != -1)
				bones[i]->SetParent(root->FindBone(parent));
			else
				root->SetNext(bones[i], true);
		}
		else
		{
			root = bones[i];
		}
		if (version == 0xff || version == 0x82 || version == 0x11b ||
			version == 6)
		{
			file->Read(&mat.xx, 4);
			file->Read(&mat.yx, 4);
			file->Read(&mat.zx, 4);
			file->Read(&mat.xy, 4);
			file->Read(&mat.yy, 4);
			file->Read(&mat.zy, 4);
			file->Read(&mat.xz, 4);
			file->Read(&mat.yz, 4);
			file->Read(&mat.zz, 4);
			file->Read(&mat.xm, 4);
			file->Read(&mat.ym, 4);
			file->Read(&mat.zm, 4);
			WVectorLen(mat.xa);
			if (version == 6)
			{
				mat.xm = mat.xm * 0.32f;
				mat.ym = mat.ym * 0.32f;
				mat.zm = mat.zm * 0.32f;
				mats[i] = mat;
				if (parent >= 0)
					mat = mats[i] * ~mats[parent];
			}
		}
		bones[i]->SetBasicMatrix(mat);
	}
	delete[] mats;
	delete[] bones;
	for (WBone* previous = root; previous->m_next != 0;
		previous = previous->m_next)
	{
		if (strstr(previous->m_next->GetBoneName(), "Bip") != 0 ||
			strstr(previous->m_next->GetBoneName(), "Bone") != 0 ||
			strstr(previous->m_next->GetBoneName(), "Dummy") != 0)
		{
			WBone* next = previous->m_next;
			previous->SetNext(0, true);
			next->SetNext(root, true);
			root = next;
			break;
		}
	}
	return root;
}

inline int ReadID(cFile* file)
{
	int id = file->GetByte();
	if (id == 0xff)
		id = -1;
	else if (id == 0xfe)
		file->Read(&id, 2);
	return id;
}

bool WPuppet::ChangeMesh(char* name)
{
	char directory[256];

	cFile* file = GetResrcManager()->GetCFile(name, 0xffff);
	if (file != 0)
	{
		GetDirectory(name, directory);
		w_pet_texture_info* texture =
			LoadPET_Texture(file, false, directory[0] != 0 ? directory : 0, 0);
		WBone* bone = LoadPET_Bone(file, 0xff);
		bone->Transform(m_mat, 0, 1.0f, 0);
		LoadPET_Mesh(file, bone, texture, false);
		LoadPET_FaceAnim(file, texture, bone, directory);
		if (texture != 0)
			delete[] texture;
		m_rootbone->CombineVtxColor(true);
		CloseCFile(file);
		delete bone;
		SetRenderMode(m_rendMode);
		SetDefaultFaceAnimation();
		return true;
	}
	return false;
}

bool WPuppet::AddMotion(const char* name, char* alias)
{
	char motion[64];

	cFile* file = GetResrcManager()->GetCFile(name, 0xffff);
	if (file == 0)
		return false;

	if (alias != 0)
		strcpy(motion, alias);
	else
		strcpy(motion, strchr(name, '/') != 0 ? strchr(name, '/') + 1 : name);

	WMatrix mat;
	mat.Reset();

	bool mtn = strlen(name) > 3 && strcmpi(name + strlen(name) - 3, "mtn") == 0;

	w_motion_addition* add = (w_motion_addition*)new char[strlen(motion) +
		sizeof(w_motion_addition)];
	strcpy(add->name, motion);
	add->rootbone = LoadPET_Bone(file, mtn);
	add->rootbone->Transform(mat, 0, 1.0f, 0);
	add->length = LoadPET_Animation(file, add->rootbone);
	add->s = m_aniLen + 1.0f;
	add->e = add->s + add->length;
	add->mdList = mtn == false ? LoadPET_Motion(file, add->s) : 0;
	add->h_gap =
		add->rootbone->GetKeyPos(0.0f).y - m_rootbone->GetKeyPos(0.0f).y;
	CloseCFile(file);

	if (add->mdList == 0)
	{
		add->mdList = new WList<w_motion_data*>(2, 2);
		w_motion_data* data =
			(w_motion_data*)new char[strlen(motion) + strlen(motion) +
				strlen(add->rootbone->m_name) + sizeof(w_motion_data) + 3];
		data->s = add->s;
		data->end = data->e = add->length + add->s;
		data->option1 = 1;
		data->option2 = 0.0f;
		data->name = data->ptr;
		data->nextmotion = data->name + strlen(motion) + 1;
		data->applybone = data->nextmotion + strlen(motion) + 1;
		strcpy(data->name, motion);
		data->nextmotion[0] = 0;
		strcpy(data->applybone, add->rootbone->m_name);
		add->mdList->AddItem(data, data->name, false);
	}

	m_addedMotionList += add;
	m_aniLen = add->e;
	MergeRootMotion(add->mdList, add->rootbone);
	return false;
}

void WPuppet::DelAddMotion(const char* name)
{
	w_motion_addition* add;
	for (add = m_addedMotionList.Start(); add != 0;
		add = m_addedMotionList.Next())
	{
		if (strcmp(add->name, name) == 0)
			break;
	}
	if (add == 0)
		return;

	m_aniLen = add->s;
	for (w_motion_addition* other = m_addedMotionList.Start(); other != 0;
		other = m_addedMotionList.Next())
	{
		if (other->s >= add->e)
		{
			other->s -= add->length;
			other->e -= add->length;
			if (m_aniLen < other->e)
				m_aniLen = other->e;
		}
	}

	m_addedMotionList -= add;
	for (w_motion_data* data = add->mdList->Start(); data != 0;
		data = add->mdList->Next())
		delete[] data;
	delete add->mdList;
	delete add->rootbone;
	delete[] add;
}

void WPuppet::ClearAddMotion(void)
{
	for (w_motion_addition* add = m_addedMotionList.Start(); add != 0;
		add = m_addedMotionList.Next())
	{
		if (m_aniLen > add->s)
			m_aniLen = add->s;
		for (w_motion_data* data = add->mdList->Start(); data != 0;
			data = add->mdList->Next())
			delete[] data;
		delete add->mdList;
		delete add->rootbone;
		delete[] add;
	}
	m_addedMotionList.Reset();
}

void WPuppet::SetTexPiece(char* name, int handle, float pu, float pv,
	float scalex, float scaley)
{
	char* part = name;
	if (strchr(name, '/') != 0)
		part = strchr(name, '/') + 1;

	if (m_tex_piece == 0)
	{
		WList<w_tex_piece*>* list = new WList<w_tex_piece*>(0x20, 8);
		m_tex_piece = list;
	}

	w_tex_piece* piece =
		(w_tex_piece*)g_mem.Alloc((int)strlen(part) + sizeof(w_tex_piece));
	piece->pu = pu;
	piece->pv = pv;
	piece->count = 1;
	piece->scalex = scalex;
	piece->scaley = scaley;
	piece->texHandle = handle;
	strcpy(piece->name, part);
	m_tex_piece->AddItem(piece, piece->name, false);
	m_use_tex_piece += piece;
}

WPuppet::w_tex_piece* WPuppet::GetTexPiece(char* name)
{
	char* part = strchr(name, '/') != 0 ? strchr(name, '/') + 1 : name;
	if (m_tex_piece != 0)
	{
		w_tex_piece* piece = m_tex_piece->Find(part);
		if (piece != 0)
		{
			piece->count++;
			m_use_tex_piece.AddItem(piece, 0, false);
			return piece;
		}
	}
	return 0;
}

void WPuppet::ClearTexPiece(void)
{
	w_tex_piece* piece = m_use_tex_piece.Start();
	w_tex_piece* found;
	for (; piece != 0; piece = m_use_tex_piece.Next())
	{
		if (--piece->count != 0)
			continue;
		if (m_tex_piece != 0)
		{
			*m_tex_piece -= piece;
			for (found = m_tex_piece->Start(); found != 0;
				found = m_tex_piece->Next())
				if (found->texHandle == piece->texHandle)
					break;
		}
		if (found == 0)
			GetResrcManager()->Release(piece->texHandle);
		g_mem.Free(piece);
	}
	m_use_tex_piece.Reset();
}

void WPuppet::LoadPET(cFile* file, bool flag, char* directory, int type)
{
	w_pet_texture_info* texture = 0;
	int count = 0;

	m_iPetType = type;
	if ((type & 1) != 0)
		texture = LoadPET_Texture(file, flag, directory, &count);

	WMatrix mat;
	mat.Reset();
	m_rootbone = LoadPET_Bone(file, type);
	m_rootbone->Transform(mat, 0, 1.0f, 0);

	if ((type & 4) != 0)
		m_aniLen = LoadPET_Animation(file, m_rootbone);
	if ((type & 8) != 0)
		LoadPET_Mesh(file, m_rootbone, texture, type == 0x11b);
	if ((type & 0x10) != 0)
		LoadPET_FaceAnim(file, texture, m_rootbone, directory);
	if ((type & 0x20) != 0)
		LoadPET_Frame(file);
	if ((type & 0x40) != 0)
		m_mdList = LoadPET_Motion(file, 0.0f);
	if ((type & 0x80) != 0)
		LoadPET_Collision(file);

	for (int i = 0; i < count; ++i)
	{
		if (texture[i].mapType == 3)
		{
			m_rendMode = 5;
			m_rootbone->ApplySpecularMap(texture[i].texHandle);
			break;
		}
	}
	if (texture != 0)
		delete[] texture;
}

float WPuppet::LoadPET_Animation(cFile* file, WBone* root)
{
	float length = 0.0f;
	if (!SeekChunkFromPET(file, "ANIM"))
		return 0.0f;
	while (true)
	{
		int id = ReadID(file);
		if (id < 0)
			break;
		length = root->FindBone(id)->LoadAnimationKey(file);
	}
	return length;
}

WList<w_motion_data*>* WPuppet::LoadPET_Motion(cFile* file, float scale)
{
	char text[4][512];
	int length;
	float option2;
	WList<w_motion_data*>* list;
	int i;
	int count;
	int startFrame;
	float e;
	float s;
	int endFrame;

	if (SeekChunkFromPET(file, "MOTI") == 0)
		return 0;
	list = new WList<w_motion_data*>(8, 8);
	file->Read(&count, 4);
	for (i = 0; i < count; ++i)
	{
		file->Read(&length, 4);
		file->Read(text[0], length);
		text[0][length] = 0;
		file->Read(&startFrame, 4);
		file->Read(&endFrame, 4);
		s = (float)startFrame / 30.0f;
		e = (float)endFrame / 30.0f;
		file->Read(&length, 4);
		file->Read(text[1], length);
		text[1][length] = 0;
		file->Read(&length, 4);
		file->Read(text[2], length);
		text[2][length] = 0;
		int option1 = strcmpi(text[2], "blending") != 0;
		file->Read(&option2, 4);
		file->Read(&length, 4);
		file->Read(text[3], length);
		text[3][length] = 0;
		w_motion_data* data = (w_motion_data*)new char[(int)strlen(text[0]) +
			(int)strlen(text[1]) + (int)strlen(text[3]) + 0x27];
		data->s = s;
		data->end = e;
		data->e = e;
		if (option1 == 2)
			data->e = data->e - option2;
		if (option2 == 1.0f)
			data->end = option2 + data->end;
		data->option1 = option1;
		data->option2 = option2;
		data->name = data->ptr;
		data->nextmotion = data->name + (int)strlen(text[0]) + 1;
		data->applybone = data->nextmotion + (int)strlen(text[1]) + 1;
		strcpy(data->name, text[0]);
		strcpy(data->nextmotion, text[1]);
		strcpy(data->applybone, text[3]);
		list->AddItem(data, data->name, false);
	}
	return list;
}

void WPuppet::LoadPET_Collision(cFile* file)
{
	char name[4][512];
	Waabb aabb;
	aabb.min.z = 0.0f;
	aabb.min.y = 0.0f;
	aabb.min.x = 0.0f;
	aabb.max.z = 0.0f;
	aabb.max.y = 0.0f;
	aabb.max.x = 0.0f;
	if (SeekChunkFromPET(file, "COLL") == 0)
		return;
	int count;
	file->Read(&count, 4);
	for (int i = 0; i < count; ++i)
	{
		int length;
		float value[6];
		int head[2];
		file->Read(&head[0], 4);
		file->Read(&head[1], 4);
		for (int n = 0; n <= 3; ++n)
		{
			memset(name[n], 0, 512);
			file->Read(&length, 4);
			file->Read(name[n], length);
			name[n][length] = 0;
		}
		for (int k = 0; k < 6; ++k)
			file->Read(&value[k], 4);
		aabb.min.y = value[1];
		aabb.min.z = value[2];
		aabb.min.x = value[0];
		aabb.max.y = value[4];
		aabb.max.z = value[5];
		aabb.max.x = value[3];
		SetBoundBox(name[0], &aabb, name[1]);
		if (strlen(name[3]) > 1 || strlen(name[2]) > 1)
		{
			w_bound_box* box = m_bbList.Find(name[0]);
			box->info->option = (char*)g_mem.Alloc(
				(int)strlen(name[3]) + (int)strlen(name[2]) + 2);
			sprintf(box->info->option, "%s %s", name[2], name[3]);
		}
		if (m_rootbone->FindBone(name[0], 0))
			m_rootbone->FindBone(name[1], 0)->SetBoundBox(aabb, false);
	}
	w_bound_box* box = m_bbList.Find("bound");
	if (box)
	{
		m_BBox.bone = box->bone;
		m_BBox.info = box->info;
		return;
	}
	box = m_bbList.Find("center");
	if (box)
	{
		m_BBox.bone = box->bone;
		m_BBox.info = box->info;
	}
}

void WPuppet::LoadPET_Frame(cFile* file)
{
	char name[1024];
	char text[4][1024];
	int count;
	int check;
	if (SeekChunkFromPET(file, "FRAM") == 0)
		return;
	file->Read(&count, 4);
	if (count <= 0)
		return;
	if (count > 10000)
		return;
	m_framenum = count;
	m_frame = (w_share_pet_data::w_frame*)g_mem.Alloc(m_framenum * 12);
	for (int i = 0; i < count; ++i)
	{
		int length;
		int num;
		int head[2];
		file->Read(&head[0], 4);
		num = head[0];
		char* line = text[0];
		for (int k = 0; k < 3; ++k)
		{
			memset(line, 0, 1024);
			file->Read(&length, 4);
			file->Read(line, length);
			line += 1024;
		}
		memset(name, 0, 1024);
		memcpy(name, text[0], strlen(text[0]) < 1024 ? strlen(text[0]) : 1024);
		if (count <= 1 && strcmp(name, "") == 0)
			return;
		file->Read(&check, 4);
		if (check == 0)
			check = -1;
		m_frame[i].num = num;
		m_frame[i].check = check;
		m_frame[i].data = (char*)g_mem.Alloc((int)strlen(name) + 1);
		strcpy(m_frame[i].data, name);
		char* found = strstr(m_frame[i].data, "msg");
		while (found != 0)
		{
			int c = 0;
			if (*found != 0)
			{
				do
				{
					if (found[c] == ')')
					{
						found[c] = ' ';
						break;
					}
					++c;
				} while (found[c] != 0);
			}
			found = strstr(found + 3, "msg");
		}
	}
}

w_pet_texture_info* WPuppet::LoadPET_Texture(cFile* file, bool alphaTest,
	char* directory, int* textureCount)
{
	int i, j;
	int count = 0;
	if (!SeekChunkFromPET(file, "TEXT"))
		return 0;
	int texNum;
	file->Read(&texNum, sizeof(texNum));
	if (textureCount)
		*textureCount = texNum;
	w_pet_texture* texture = new w_pet_texture[texNum];
	file->Read(texture, texNum * sizeof(w_pet_texture));
	w_pet_texture_info* info = new w_pet_texture_info[texNum];
	Bitmap** bitmap = new Bitmap*[texNum];
	if (!m_storeTexList)
		m_storeTexList = new WList<int>;
	char filename[260];
	for (i = 0; i < texNum; ++i)
	{
		strcpy(info[i].filename, texture[i].filename);
		info[i].group = texture[i].group;
		info[i].bBind = false;
		info[i].mapType = 0;
		info[i].alpha = (unsigned char)(texture[i].diffuse >> 24);
		info[i].diffuse = texture[i].diffuse;
		info[i].xulDrawFlag2 = 0;
		info[i].drawFlags = 0;
		info[i].texHandle = 0;
		info[i].pu = info[i].pv = 0.0f;
		info[i].scalex = info[i].scaley = 1.0f;
		bitmap[i] = 0;
		if (!strcmp(texture[i].filename, "null"))
			continue;
		if (strstr(texture[i].filename, "2#"))
			info[i].xulDrawFlag2 = 0x400;
		for (j = 0; j < 3; ++j)
		{
			switch (texture[i].filename[j])
			{
			case '+':
				if (alphaTest)
					info[i].drawFlags |= 0x80000000;
				else
					info[i].drawFlags |= 0x400000;
				info[i].xulDrawFlag2 |= 0x10;
				break;
			case '[':
				if (alphaTest)
					info[i].drawFlags |= 0x80000000;
				else
					info[i].drawFlags |= 0x400000;
				break;
			case ']':
				info[i].drawFlags |= 0x80000000;
				info[i].xulDrawFlag2 |= 4;
				break;
			case '{':
				info[i].drawFlags |= 0x80000000;
				break;
			case '~':
				info[i].drawFlags |= 0x20800000;
				if (alphaTest)
					info[i].drawFlags |= 0x80000000;
				else
					info[i].drawFlags |= 0x400000;
				if (!texture[i].flag)
					info[i].bBind = true;
				break;
			case '}':
				info[i].drawFlags |= 0x21800000;
				if (alphaTest)
					info[i].drawFlags |= 0x80000000;
				else
					info[i].drawFlags |= 0x400000;
				break;
			case '@':
				info[i].mapType = 1;
				break;
			case '!':
				info[i].mapType = 3;
				break;
			default:
				if (j == 0 && !texture[i].flag)
					info[i].bBind = true;
				break;
			}
		}
		strcpy(filename, texture[i].filename);
		if (strstr(filename, ".dds") || info[i].mapType == 3)
			info[i].bBind = false;
		if (texNum > 1 && info[i].bBind == true &&
			!GetResrcManager()->CheckTexture(filename))
		{
			w_tex_piece* piece = GetTexPiece(texture[i].filename);
			if (piece)
			{
				info[i].pu = piece->pu;
				info[i].pv = piece->pv;
				info[i].scalex = piece->scalex;
				info[i].scaley = piece->scaley;
				info[i].texHandle = piece->texHandle;
				info[i].bBind = false;
				bitmap[i] = 0;
			}
			else
			{
				bitmap[i] = GetResrcManager()->LoadBitmap(filename, 1, false);
				if (!bitmap[i])
					bitmap[i] = new Bitmap(32, 32, 8);
				if (bitmap[i]->bi->bmiHeader.biWidth > 256 ||
					bitmap[i]->bi->bmiHeader.biHeight > 256 ||
					(bitmap[i]->bi->bmiHeader.biWidth == 256 &&
						bitmap[i]->bi->bmiHeader.biHeight == 256))
				{
					if (strcmp(texture[i].filename, "null"))
					{
						bitmap[i]->Update();
						info[i].texHandle = GetResrcManager()->UploadTexture(
							filename, bitmap[i], 0, 0);
						delete bitmap[i];
						*m_storeTexList += info[i].texHandle;
						bitmap[i] = 0;
					}
				}
				else
					++count;
			}
		}
		else
		{
			bitmap[i] = 0;
			info[i].texHandle =
				GetResrcManager()->LoadTexture(filename, 0, 0, 0);
			*m_storeTexList += info[i].texHandle;
		}
	}
	WRectManager page[8];
	Bitmap surface;
	int* bindIndex = new int[texNum];
	static int fixed_wh[8][2] = {
		{ 256, 256 },
        { 256, 128 },
        { 128, 128 },
        { 128, 64  },
        { 64,  128 },
		{ 64,  64  },
        { 32,  64  },
        { 32,  32  }
	};
	while (count > 0)
	{
		for (i = 0; i < 8; ++i)
			page[i].Clear(fixed_wh[i][0], fixed_wh[i][1]);
		i = 0;
		int max = 8;
		int level;
		int x, y;
		for (; i < texNum; ++i)
		{
			if (bitmap[i] &&
				!page[0].Alloc(bitmap[i]->bi->bmiHeader.biWidth,
					bitmap[i]->bi->bmiHeader.biHeight, x, y))
			{
				bindIndex[i] = 1;
				level = 0;
				for (j = 1; j < max; ++j)
				{
					if (page[j].Alloc(bitmap[i]->bi->bmiHeader.biWidth,
							bitmap[i]->bi->bmiHeader.biHeight, x, y))
					{
						max = level + 1;
						break;
					}
					++level;
				}
			}
			else
				bindIndex[i] = 0;
		}
		page[level].Clear(fixed_wh[level][0], fixed_wh[level][1]);
		surface.Create(fixed_wh[level][0], fixed_wh[level][1], 24);
		int handle =
			GetResrcManager()->UploadTexture(0, &surface, 0x10000000, 0);
		for (i = 0; i < texNum; ++i)
		{
			if (bindIndex[i])
			{
				bitmap[i]->Update();
				page[level].Alloc(bitmap[i]->bi->bmiHeader.biWidth,
					bitmap[i]->bi->bmiHeader.biHeight, x, y);
				info[i].pu = (float)x / (float)surface.bi->bmiHeader.biWidth;
				info[i].pv = (float)y / (float)surface.bi->bmiHeader.biHeight;
				info[i].scalex = (float)bitmap[i]->bi->bmiHeader.biWidth /
					(float)surface.bi->bmiHeader.biWidth;
				info[i].scaley = (float)bitmap[i]->bi->bmiHeader.biHeight /
					(float)surface.bi->bmiHeader.biHeight;
				info[i].texHandle = handle;
				bitmap[i]->Update();
				for (j = 0; j < (int)(bitmap[i]->bi->bmiHeader.biWidth *
									bitmap[i]->bi->bmiHeader.biHeight);
					++j)
				{
					unsigned char r, g, b;
					int px = j % bitmap[i]->bi->bmiHeader.biWidth;
					int py = j / bitmap[i]->bi->bmiHeader.biWidth;
					bitmap[i]->GetPixel(px, py, r, g, b);
					surface.vram[(y + py) * surface.pitch + (x + px) * 3] = b;
					surface.vram[(y + py) * surface.pitch + (x + px) * 3 + 1] =
						g;
					surface.vram[(y + py) * surface.pitch + (x + px) * 3 + 2] =
						r;
				}
				--count;
				delete bitmap[i];
				bitmap[i] = 0;
				SetTexPiece(texture[i].filename, info[i].texHandle, info[i].pu,
					info[i].pv, info[i].scalex, info[i].scaley);
			}
		}
		GetResrcManager()->FixTexture(handle, &surface, 0, 0);
	}
	delete[] bindIndex;
	delete[] bitmap;
	delete[] texture;
	return info;
}

void WPuppet::LoadPET_FaceAnim(cFile* file, w_pet_texture_info* texture,
	WBone* bone, char* name)
{
	w_pet_face_animation record;
	int i;
	w_face_animation* anim;
	int count;

	if (SeekChunkFromPET(file, "FANM") == 0)
		return;
	for (anim = m_fanimList.Start(); anim != 0; anim = m_fanimList.Next())
	{
		g_mem.Free(anim->mesh);
		delete anim;
	}
	m_fanimList.Reset();
	file->Read(&count, 4);
	for (i = 0; i < count; ++i)
	{
		file->Read(&record, 65);
		int n = 0;
		while (strcmpi(texture[n].filename, record.fName) != 0)
			++n;
		anim = new w_face_animation;
		strcpy(anim->name, record.animName);
		strcpy(anim->filename, record.fName);
		anim->texHandle = texture[n].texHandle;
		anim->tu = texture[n].pu;
		anim->tv = texture[n].pv;
		anim->scalex = texture[n].scalex;
		anim->scaley = texture[n].scaley;
		anim->mesh = 0;
		anim->group = record.group;
		anim->meshNum = m_rootbone->GetMeshForFaceAnimation(
			(unsigned char)anim->group, 0, 0);
		anim->mesh = (w_mesh**)g_mem.Alloc(anim->meshNum * 4 + 4);
		m_rootbone->GetMeshForFaceAnimation((unsigned char)anim->group,
			anim->mesh, 0);
		anim->mesh[anim->meshNum] = 0;
		m_fanimList.AddItem(anim, anim->name, false);
	}
}

void WPuppet::LoadPET_Mesh(cFile* file, WBone* root,
	w_pet_texture_info* texture, bool meshPet)
{
	WVector position, normal, va, vb, vc;
	int i;
	if (!SeekChunkFromPET(file, "MESH"))
		return;
	w_skin_info* skins = 0;
	int skinCount;
	if (meshPet)
	{
		skinCount = file->GetByte();
		skins = new w_skin_info[skinCount];
		file->Read(skins, skinCount * sizeof(w_skin_info));
	}
	int vertexCount;
	file->Read(&vertexCount, sizeof(vertexCount));
	w_pet_vertex* vertices = new w_pet_vertex[vertexCount];
	for (i = 0; i < vertexCount; ++i)
	{
		file->Read(&position, sizeof(position));
		if (meshPet)
			file->Read(&vertices[i].weight, sizeof(float));
		else
			vertices[i].weight = 1.0f;
		short blend[16][2];
		int total = 0;
		int count = 0;
		do
		{
			blend[count][0] = file->GetByte();
			int id = ReadID(file);
			blend[count][1] = id;
			total += blend[count][0];
			++count;
		} while (total < 255);
		normal = root->FindBone(blend[0][1])->Transform(position);
		vertices[i].x = normal.x;
		vertices[i].y = normal.y;
		vertices[i].z = normal.z;
		vertices[i].blendWeight = (unsigned char)blend[0][0];
		vertices[i].bonename = root->FindBone(blend[0][1])->GetBoneName();
		vertices[i].list = 0;
		if (count > 1)
		{
			vertices[i].list = new w_pet_vertex::w_blended[count];
			vertices[i].listNum = count;
			for (int j = 0; j < count; ++j)
			{
				WBone* blended = root->FindBone(blend[j][1]);
				vertices[i].list[j].blendWeight = (unsigned char)blend[j][0];
				vertices[i].list[j].bone =
					m_rootbone->FindBone(blended->GetBoneName(), 0);
			}
		}
		else
		{
			file->GetByte();
			file->GetByte();
		}
	}
	int triangleCount;
	file->Read(&triangleCount, sizeof(triangleCount));
	w_pet_tri_point* triangles = new w_pet_tri_point[triangleCount * 3];
	w_pet_texture_info** textures = new w_pet_texture_info*[triangleCount];
	file->Read(triangles, triangleCount * 3 * sizeof(w_pet_tri_point));
	for (i = 0; i < triangleCount; ++i)
		textures[i] = &texture[file->GetByte()];
	m_faceNum = triangleCount;
	WBone** bones;
	if (!meshPet)
	{
		WBone* fake = root->FindFakeBodyBone(vertices, vertexCount);
		for (i = 0; i < triangleCount * 3; ++i)
		{
			if (Wabs(triangles[i].nx) < g_EPSILON &&
				Wabs(triangles[i].ny) < g_EPSILON &&
				Wabs(triangles[i].nz) < g_EPSILON)
			{
				int j = i / 3 * 3;
				WBone* bone =
					root->FindBone(vertices[triangles[j].pos].bonename, 0);
				va = bone->Transform(WVector(vertices[triangles[j].pos].x,
					vertices[triangles[j].pos].y,
					vertices[triangles[j].pos].z));
				vb = bone->Transform(WVector(vertices[triangles[j + 1].pos].x,
					vertices[triangles[j + 1].pos].y,
					vertices[triangles[j + 1].pos].z));
				vc = bone->Transform(WVector(vertices[triangles[j + 2].pos].x,
					vertices[triangles[j + 2].pos].y,
					vertices[triangles[j + 2].pos].z));
				normal = WCrossProduct(va - vb, vc - vb);
			}
			else
			{
				WBone* bone =
					root->FindBone(vertices[triangles[i].pos].bonename, 0);
				if (fake &&
					(strstr(vertices[triangles[i].pos].bonename, "Bip") ||
						strstr(vertices[triangles[i].pos].bonename, "Bone") ||
						strstr(vertices[triangles[i].pos].bonename, "Jaw")))
					normal = RotVec(WVector(triangles[i].nx, triangles[i].ny,
										triangles[i].nz),
						fake->GetMatrix());
				else
					normal = RotVec(WVector(triangles[i].nx, triangles[i].ny,
										triangles[i].nz),
						bone->GetMatrix());
			}
			normal.Normalize();
			triangles[i].nx = normal.x;
			triangles[i].ny = normal.y;
			triangles[i].nz = normal.z;
		}
		bones = new WBone*[triangleCount * 3];
		for (i = 0; i < triangleCount * 3; ++i)
		{
			bones[i] =
				m_rootbone->FindBone(vertices[triangles[i].pos].bonename, 0);
			if (!bones[i])
				bones[i] = m_rootbone->FindBone(
					root->FindBone(vertices[triangles[i].pos].bonename, 0)
						->GetParent()
						->GetBoneName(),
					0);
		}
		m_rootbone->SetMesh(vertices, vertexCount, triangles, textures, bones,
			triangleCount);
	}
	else
	{
		int end;
		for (i = 0; i < skinCount; ++i)
		{
			end = (skins[i].iSTriIdx + skins[i].nTris) * 3;
			for (int j = skins[i].iSTriIdx * 3; j < end; ++j)
			{
				if (Wabs(triangles[j].nx) < g_EPSILON &&
					Wabs(triangles[j].ny) < g_EPSILON &&
					Wabs(triangles[j].nz) < g_EPSILON)
				{
					int k = j / 3 * 3;
					w_pet_vertex* a =
						&vertices[triangles[k].pos + skins[i].iSVtxIdx];
					w_pet_vertex* b =
						&vertices[triangles[k + 1].pos + skins[i].iSVtxIdx];
					w_pet_vertex* c =
						&vertices[triangles[k + 2].pos + skins[i].iSVtxIdx];
					WBone* bone = root->FindBone(a->bonename, 0);
					va = bone->Transform(WVector(a->x, a->y, a->z));
					vb = bone->Transform(WVector(b->x, b->y, b->z));
					vc = bone->Transform(WVector(c->x, c->y, c->z));
					normal = WCrossProduct(va - vb, vc - vb);
					normal.Normalize();
				}
				else
					normal = WVector(triangles[j].nx, triangles[j].ny,
						triangles[j].nz);
				triangles[j].nx = normal.x;
				triangles[j].ny = normal.y;
				triangles[j].nz = normal.z;
			}
		}
		bones = new WBone*[triangleCount * 3];
		for (i = 0; i < skinCount; ++i)
		{
			end = (skins[i].iSTriIdx + skins[i].nTris) * 3;
			int j = skins[i].iSTriIdx * 3;
			if (j < end)
			{
				do
				{
					w_pet_vertex* vertex =
						&vertices[triangles[j].pos + skins[i].iSVtxIdx];
					bones[j] = m_rootbone->FindBone(vertex->bonename, 0);
					if (!bones[j])
						bones[j] = m_rootbone->FindBone(
							root->FindBone(vertex->bonename, 0)
								->GetParent()
								->GetBoneName(),
							0);
					++j;
				} while (j < end);
			}
		}
		for (i = 0; i < skinCount; ++i)
		{
			WBone* bone = m_rootbone->FindBone(skins[i].ucBoneID);
			bone->SetBoneMesh(vertices + skins[i].iSVtxIdx, skins[i].nVtxs,
				triangles + skins[i].iSTriIdx * 3, textures + skins[i].iSTriIdx,
				bones + skins[i].iSTriIdx * 3, skins[i].nTris);
			for (w_mesh* mesh = bone->m_mesh; mesh; mesh = mesh->next)
				strcpy(mesh->mpetName, m_petName);
		}
	}
	for (i = 0; i < vertexCount; ++i)
		if (vertices[i].list)
			delete[] vertices[i].list;
	delete[] bones;
	delete[] textures;
	delete[] triangles;
	delete[] vertices;
	if (meshPet)
		delete[] skins;
}

void WPuppet::GetDirectory(char* path, char* directory)
{
	char* src = path;
	char* dst = directory;
	char value;
	do
	{
		value = *src;
		*dst = value;
		++src;
		++dst;
	} while (value != 0);
	char* slash = strrchr(directory, '/');
	if (slash)
		*slash = 0;
	else
		*directory = 0;
}

void WPuppet::GetName(char* path, char* name)
{
	char* slash = strrchr(path, '/');
	char* src = slash + 1;
	if (slash == 0)
		src = path;
	char value;
	do
	{
		value = *src;
		*name = value;
		++src;
		++name;
	} while (value != 0);
}

void WPuppet::MergeRootMotion(WList<w_motion_data*>* list, WBone* bone)
{
	for (w_motion_data* data = list->Start(); data != 0; data = list->Next())
	{
		for (WBone* b = bone; b != 0; b = b->m_next)
		{
			if (data->applybone[0] == 0)
				break;
			if (strcmp(b->m_name, data->applybone) == 0)
				data->applybone[0] = 0;
		}
	}
}

int WPuppet::LoadPET(char* name, bool flag, int type)
{
	char hint[260];
	char directory[256];
	char petname[256];

	cFile* file = GetResrcManager()->GetCFile(name, -1);
	if (file == 0)
		return 1;

	GetDirectory(name, directory);
	char* slash = strrchr(name, '/');
	strcpy(petname, slash != 0 ? slash + 1 : name);
	strcpy(m_petName, petname);

	LoadPET(file, flag, directory[0] != 0 ? directory : 0, type);
	CloseCFile(file);

	if (m_mdList != 0)
		MergeRootMotion(m_mdList, m_rootbone);
	m_rootbone->CombineVtxColor(true);
	SetRenderMode(m_rendMode);
	SetDefaultFaceAnimation();
	MakeTemporaryCenterBox();

	WMatrix mat;
	mat.Reset();
	m_mat = mat;
	m_rootbone->Transform(m_mat, 0, 1.0f, 0);

	if (m_iPetType == 0xff || m_iPetType == 0x82)
		xBuildTransfMatPtrList();

	sprintf(hint, "WPuppet:%s", name);
	SetLeakHint(hint);

	if (type == 0x11b && m_rootbone->CheckMeshBoneAllVertexRigid() == false)
		return 1;
	return 0;
}

void WPuppet::FixNormal(void)
{
	WMatrix matrix;
	matrix.Reset();
	m_rootbone->FixNormal(matrix);
}

void WPuppet::xBuildTransfMatPtrList(void)
{
	m_xTransfMatPtrList.clear();

	int count = 0;
	m_rootbone->CountBones(count);
	if (count > 0)
	{
		m_xTransfMatPtrList.resize(count);
		for (int i = 0; i < count; ++i)
			m_xTransfMatPtrList[i] = 0;
		m_rootbone->xBuildTransfMatPtrList(m_xTransfMatPtrList);
		AddOrginalMatrixPtrList(m_petName, m_xTransfMatPtrList);
	}
}

void WPuppet::AttachMeshBone(WPuppet* other, int handle, int oldHandle)
{
	WBone* tail;
	for (tail = m_rootbone; tail->m_next != 0; tail = tail->m_next)
	{
	}

	WBone* bone = 0;
	while ((bone = other->FindMeshBone(bone)) != 0)
	{
		tail = tail->AttachMeshBone(bone, handle, oldHandle);
		tail->ResetMeshBone(m_rootbone, false);
		tail->CombineVtxColor(false);
	}

	if (!HasOrginalMatrixList(other->m_petName))
	{
		std::vector<const WMatrix*> list;
		int count = 0;
		other->m_rootbone->CountBones(count);
		if (count > 0)
		{
			std::vector<const WMatrix*> mine;
			list.resize(count);
			int i;
			for (i = 0; i < count; ++i)
				list[i] = 0;
			other->m_rootbone->xBuildTransfMatPtrList(list);
			mine.resize(m_xTransfMatPtrList.size());
			for (i = 0; i < (int)m_xTransfMatPtrList.size(); ++i)
				mine[i] = 0;
			for (i = 0; i < count; ++i)
			{
				if (list[i] == 0)
					continue;
				WBone* found = other->m_rootbone->FindBone(i);
				WBone* mapped = m_boneList.Find(found->m_name);
				if (mapped == 0)
				{
					mapped = m_rootbone->FindBone(found->m_name, 0);
					if (mapped == 0)
						continue;
					m_boneList.AddItem(mapped, mapped->m_name, false);
				}
				int id = mapped->GetID();
				if (id < 0)
					continue;
				if (id < (int)mine.size())
					mine[id] = list[i];
			}
			AddOrginalMatrixPtrList(other->m_petName, mine);
		}
	}
}

void WPuppet::DetachMeshBone(WPuppet* other)
{
	WBone* bone = 0;
	while ((bone = other->FindMeshBone(bone)) != 0)
	{
		WBone* found;
		WBone* previous;
		FindMeshBone(&found, &previous, bone->m_name);
		if (found != 0)
		{
			if (previous != 0)
				previous->SetNext(found->m_next, false);
			else
				m_rootbone = found->m_next;
			found->SetNext(0, true);
			delete found;
		}
	}
}

void WPuppet::ReplaceMeshBones(WPuppet* oldPuppet, WPuppet* newPuppet,
	int handle, int oldHandle)
{
	if (oldPuppet)
		DetachMeshBone(oldPuppet);
	if (newPuppet)
		AttachMeshBone(newPuppet, handle, oldHandle);
}

void WPuppet::ChangeTextureByHandle(int from, int to, WPuppet* other)
{
	if (other == 0)
	{
		m_rootbone->ChangeTextureByHandle(from, to, true, true);
		return;
	}
	WBone* bone = 0;
	while ((bone = other->FindMeshBone(bone)) != 0)
	{
		WBone* root = m_rootbone;
		WBone* found = 0;
		if (stricmp(root->m_name, bone->m_name) == 0)
		{
			found = root;
		}
		else
		{
			while (root->m_next != 0)
			{
				if (stricmp(root->m_next->m_name, bone->m_name) == 0)
				{
					found = root->m_next;
					break;
				}
				root = root->m_next;
			}
		}
		if (found != 0)
			found->ChangeTextureByHandle(from, to, false, false);
	}
}

void WPuppet::EndMeshBoneWork(void)
{
	MergeNormal();
}

void WPuppet::MergeNormal(void)
{
	WBone::w_normalmerge merge(*this);
}

void WPuppet::FindMeshBone(WBone** found, WBone** previous, const char* name)
{
	if (found)
		*found = 0;
	if (previous)
		*previous = 0;
	WBone* bone = m_rootbone;
	if (stricmp(bone->m_name, name) == 0)
	{
		*found = bone;
		return;
	}
	WBone* next = bone->m_next;
	while (next != 0)
	{
		if (stricmp(next->m_name, name) == 0)
		{
			*found = bone->m_next;
			if (previous)
				*previous = bone;
			return;
		}
		bone = bone->m_next;
		next = bone->m_next;
	}
}

WBone* WPuppet::FindMeshBone(WBone* previous)
{
	if (previous == 0)
	{
		WBone* first = m_rootbone;
		while (first != 0 && first->m_mesh == 0)
			first = first->m_next;
		return first;
	}
	WBone* bone = m_rootbone;
	while (bone != 0 && bone != previous)
		bone = bone->m_next;
	if (bone != 0)
	{
		bone = bone->m_next;
		while (bone != 0 && bone->m_mesh == 0)
			bone = bone->m_next;
	}
	return bone;
}

void WPuppet::UpdateMeshBonePtr(WPuppet* other)
{
	for (WBone* bone = m_rootbone; bone != 0; bone = bone->m_next)
	{
		if (bone->m_mesh != 0)
			bone->ResetMeshBone(other->m_rootbone, false);
	}
}

w_face_animation* WPuppet::FindFaceAnim(char* name)
{
	return m_fanimList.Find(name);
}

void WPuppet::DisableFog(void)
{
	for (WBone* bone = m_rootbone; bone; bone = bone->m_next)
		bone->DisableFog();
}

void WPuppet::EnableAlphaBlend(void)
{
}

void WPuppet::ScaleBone(char* name, float scale)
{
	WBone* bone = m_boneList.Find(name);
	if (bone == 0)
	{
		bone = m_rootbone->FindBone(name, 0);
		if (bone == 0)
			return;
		m_boneList.AddItem(bone, bone->m_name, false);
	}
	bone->SetScale(scale);
}

bool WPuppet::CompareBasicMatrix(WPuppet* other) const
{
	if (m_rootbone && other->m_rootbone)
		return m_rootbone->CompareBasicMatrix(other->m_rootbone);
	return false;
}

std::map<std::string, WPuppet::w_original_matrix_set>
	WPuppet::ms_xOrginalMatrixList;

void WPuppet::AddOrginalMatrixPtrList(const std::string& name,
	const std::vector<const WMatrix*>& matPtrList)
{
	std::map<std::string, w_original_matrix_set>::iterator found =
		ms_xOrginalMatrixList.find(name);
	if (found != ms_xOrginalMatrixList.end())
		return;
	w_original_matrix_set& set = ms_xOrginalMatrixList[name];
	set.matPtrList.resize(matPtrList.size());
	set.matList.resize(matPtrList.size());
	for (int i = 0; i < (int)matPtrList.size(); ++i)
	{
		set.matPtrList[i] = &set.matList[i];
		if (matPtrList[i])
			set.matList[i] = *matPtrList[i];
		else
			set.matList[i].Reset();
	}
}

bool WPuppet::HasOrginalMatrixList(const std::string& name)
{
	std::map<std::string, w_original_matrix_set>::iterator found =
		ms_xOrginalMatrixList.find(name);
	return found != ms_xOrginalMatrixList.end() ? true : false;
}

const std::vector<WMatrix>& WPuppet::GetOrginalMatrixList(
	const std::string& name)
{
	std::map<std::string, w_original_matrix_set>::const_iterator found =
		ms_xOrginalMatrixList.find(name);
	return (*found).second.matList;
}

const std::vector<const WMatrix*>& WPuppet::GetOrginalMatrixPtrList(
	const std::string& name)
{
	std::map<std::string, w_original_matrix_set>::const_iterator found =
		ms_xOrginalMatrixList.find(name);
	return (*found).second.matPtrList;
}

// HACK(?): Necessary to preserve emission order
#include "wmemblock.inl"
