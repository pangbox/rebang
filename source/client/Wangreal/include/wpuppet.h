#pragma once
#include "wmath.h"
#include "wresource.h"
#include "wscene.h"
#include "wlist.h"
#include <vector>
#include <string>
#include <map>

enum wPuppetSaveFlag
{
	W_PET_SV_TEXT = 1,
	W_PET_SV_BONE = 2,
	W_PET_SV_ANIM = 4,
	W_PET_SV_MESH = 8,
	W_PET_SV_FANM = 16,
	W_PET_SV_FRAM = 32,
	W_PET_SV_MOTI = 64,
	W_PET_SV_COLL = 128,
	W_PET_SV_ALL = 255,
	W_PET_SV_SKINLIST = 0x100,
};

enum wPuppetRenderMode
{
	W_PUPPET_RENDER_NORMAL = 0,
	W_PUPPET_RENDER_TEXTURE_ONLY = 1,
	W_PUPPET_RENDER_DIFFUSE_ONLY = 2,
	W_PUPPET_RENDER_WIREFRAME = 3,
	W_PUPPET_RENDER_OVERLAY = 4,
	W_PUPPET_RENDER_TWO_TEXTURE_ADD = 5,
};

typedef std::vector<const WMatrix*> WMatrixPtrList;

class WBone;
class WBoneSet;
class WBlockModel;
class Bitmap;
class cFile;
class WView;
class WScene;
struct w_mesh;
struct w_pet_texture_info;
struct tagRECT;

struct w_bound_box_info
{
	Waabb aabb;
	WVector spherePivot;
	float sphereLength;
	char* option;
	char name[1];
};

struct w_motion_data
{
	float s;
	float e;
	float end;
	int option1;
	float option2;
	char* applybone;
	char* name;
	char* nextmotion;
	char ptr[1];
};

struct w_face_animation
{
	w_mesh** mesh;
	int meshNum;
	int group;
	char name[32];
	char filename[32];
	float tu;
	float tv;
	float du;
	float dv;
	float scalex;
	float scaley;
	int texHandle;
};

struct w_bound_box
{
	WBone* bone;
	w_bound_box_info* info;
};

struct w_motion_addition
{
	WList<w_motion_data*>* mdList;
	WBone* rootbone;
	float s;
	float e;
	float length;
	float h_gap;
	char name[1];
};

struct w_share_pet_data
{
	char* petName;
	WBone* rootbone;
	float length;
	int faceNum;
	int numFramedata;
	struct w_frame
	{
		int num;
		int check;
		char* data;
	};
	w_frame* frame;
	Waabb bound;
	WList<w_bound_box*>* bbList;
	WList<w_motion_data*>* mdList;
	WList<w_face_animation*>* fanimList;
};

class WPuppet : public WResource
{
public:
	WPuppet();
	virtual ~WPuppet();
	void Share(w_share_pet_data* shareData);
	virtual WPuppet* MakeClone(bool needBuffer);
	virtual int LoadPET(char* name, bool flag, int loadFlags);

private:
	void LoadPET(cFile* file, bool flag, char* directory, int type);

public:
	void SetRenderMode(int mode);
	void SetLightLine(float value);
	enum
	{
		W_PUPPET_LIGHT_COLOR_PER_VERTEX = 0x01000000,
		W_PUPPET_CHECK_SHADOW = 0x02000000,
		W_LIGHT_SELFILLUM = 0x04000000,
		W_LIGHT_ADD = 0x08000000,
		W_LIGHT_SELFCOLOR_MASK = 0x00ffffff
	};
	void SetLightMode(unsigned long mode) { m_lightmode = mode; }
	int GetLightMode() { return m_lightmode; }
	int m_lightmode;
	void Transform(WView* view, float scale, int flags);
	virtual void UpdateLightSource(LightSet* light, bool addLight,
		WScene* scene);
	void UpdateLightSource(WScene* scene);
	void ApplyFaceAnimation(char* name, float time);
	virtual void Render(WView* view, bool transform, float scale, bool flag0,
		bool flag1, bool software);
	void RenderHierarchy(WView* view, bool transform, float value, float scale);
	void RenderNormals(WView* view, bool transform, float value, float scale);
	WVector InvTransform(const WVector& point) { return point * ~m_mat; }
	void ChangeTexture(char* name, int handle);
	bool ChangeTexturePart(char* name, char* part, Bitmap** bitmap,
		tagRECT* rect);
	void ChangeTexturePart(char* name, Bitmap* bitmap, const tagRECT& rect);
	bool ChangeTexturePart(int handle, char* name, Bitmap** bitmap,
		tagRECT* rect);
	void ChangeTexturePart(int handle, Bitmap* bitmap, const tagRECT& rect);
	void InstantTexChange(int, int);
	void DisableFog();
	void EnableAlphaBlend();
	enum w_get_bone_flag
	{
		FIXEDPIVOT = 1,
		ADDFORCE = 2,
		EXCEPTPARENT = 4,
		EX_PONYTAIL = 8
	};
	WBoneSet* GetBones(const char* name, float time, int mode, char* bone,
		bool flag);
	float GetMotionLength(const char* name);
	char* GetNextMotionName(const char* name);
	void ApplyBones(WBoneSet* set, const WMatrix& matrix);
	void ApplyBones(WBoneSet* set);
	bool ChangeMesh(char* name);
	bool AddMotion(const char* name, char* alias);
	void DelAddMotion(const char* name);
	void ClearAddMotion();
	void OptimizeBone();
	bool HaveCenterBoundBox() { return m_bHaveCenter; }
	bool GetBoundBoxPlane(char* name, WPlane* plane);
	bool GetAlignBoundBoxPlane(char* name, WPlane* plane, Waabb* box);
	bool DotContact(char* name, const WVector& dot);
	w_bound_box* FindBoundBox(const char* name) { return m_bbList.Find(name); }
	char* FindTexAniList(char* name);
	w_face_animation* FindFaceAnim(char* name);
	w_motion_data* FindMotionData(const char* name);
	int GetMotionCountIncludeName(const char* include, const char* exclude);
	w_motion_data* GetFirstMotionData() { return m_mdList->Start(); }
	w_motion_data* GetNextMotionData() { return m_mdList->Next(); }
	w_motion_addition* GetFirstAddedMotionList();
	w_motion_addition* GetNextAddedMotionList();
	void GetBoneName(char** names, int& count);
	WVector GetPivot(const char* name);
	WMatrix GetRootMatrix();
	WVector GetDeltaVec(float time);
	WVector GetDeltaVec(char* name, float time);
	WVector GetDeltaVec(char* name, float from, float to);
	WBone* FindBone(const char* name);
	WBone* FindBone(int id);
	WBone* FindBoneByTail(const char* tail);
	bool CheckFrameData(int first, int last, int& from, int& to);
	float GetAnimationLength() { return m_aniLen; }
	unsigned int GetFrameCount() { return m_framenum; }
	char* GetFrameData(int index) { return m_frame[index].data; }
	float GetFrameDataTime(int index) { return m_frame[index].num / 30.0f; }
	bool GetFrameCheck(int index) { return m_frame[index].check != 0; }
	WMatrix& GetMatrix() { return m_mat; }
	const WMatrix& GetCamMatrix(char* name);
	float GetCamFOV(char* name, WBoneSet* set);
	WBone* GetRootBone() { return m_rootbone; }
	int GetFaceNum() { return m_faceNum; }
	void SetVtxColor(unsigned long color);
	void SetSelfIllum(char* name, bool enabled);
	void SetSelfIllumColor(char* name, unsigned long color);
	void ShowBone(char* name);
	void HideBone(char* name);
	unsigned char GetAlpha(char* name);
	void SetAlpha(unsigned char alpha);
	void SetAlpha(char* name, unsigned char alpha);
	void ToggleSelfIllum(char* name);
	void SetMirrored(bool mirrored);
	void AddCollisionObject(const char* name, WBlockModel* model);
	void UpdatePhysics(float delta, const WVector& accel);
	void SetPhysicsModel(const char* name, int model);
	WList<WBone*> m_phybone;
	void RotateBone(char* name, float angle, int axis);
	void MoveBone(char* name, WVector& vector);
	void CalcBound(Waabb* box, bool flag);
	void CalcBound(bool reset, WPuppet* puppet, bool mode, char* name,
		Waabb* box);
	const Waabb& GetBoundBox() { return m_bound; }
	Waabb GetTransformedBoundBox() const;
	void UpdateBound(bool transform, bool flag);
	void UpdateBSphere();
	const WSphere& GetBoundSphere() const { return m_boundSphere; }
	void LockBoneSetPos(unsigned char pos) { ms_lockedBoneSetPos = pos; }
	unsigned char GetBoneSetPos() const { return ms_boneSetPos; }
	void RenderBoundBox(const char* name, WView* view, unsigned long color,
		float size);
	const char* GetPuppetName() const { return m_petName; }
	int GetRenderMode() const { return m_rendMode; }

protected:
	void SetDefaultFaceAnimation();
	void SetBoundBox(char* name, Waabb* aabb, char* boneName);
	WBoneSet* GetFreeBoneSetList(bool flag);
	WMatrix m_mat;

private:
	void FreeBone(WBone*);
	void FixNormal();
	void MakeTemporaryCenterBox();
	WList<w_motion_addition*> m_addedMotionList;

public:
	enum
	{
		LOAD_BONE = 1,
		LOAD_ANIMATIONDATA = 2,
		LOAD_TEXTURE_MESH_FACEANIMATION = 4,
		LOAD_MISC_DATA = 8,
		LOAD_ALL = 15
	};

protected:
	int m_rendMode;
	bool m_bClone;

private:
	w_pet_texture_info* LoadPET_Texture(cFile* file, bool flag, char* directory,
		int* count);
	void LoadPET_Mesh(cFile* file, WBone* bone, w_pet_texture_info* texture,
		bool flag);
	WBone* LoadPET_Bone(cFile* file, int version);
	float LoadPET_Animation(cFile* file, WBone* bone);
	void LoadPET_FaceAnim(cFile* file, w_pet_texture_info* texture, WBone* bone,
		char* name);
	void LoadPET_Frame(cFile* file);
	WList<w_motion_data*>* LoadPET_Motion(cFile* file, float scale);
	void LoadPET_Collision(cFile* file);
	void MergeRootMotion(WList<w_motion_data*>* list, WBone* bone);
	static int SeekChunkFromPET(cFile* fp, char* str);
	bool CheckStaticBone(WBone* bone);
	WBone* MergeBone(WBone* target, WBone* source);
	bool ReduceBone(WBone* bone);
	void GetDirectory(char* path, char* directory);
	void GetName(char* path, char* name);
	bool m_bHaveCenter;

protected:
	w_share_pet_data::w_frame* m_frame;
	int m_framenum;
	char m_petName[0x40];

public:
	int m_faceNum;

protected:
	Waabb m_bound;
	WSphere m_boundSphere;
	w_bound_box m_BBox;

private:
	WList<int>* m_storeTexList;
	struct w_tex_piece
	{
		float pu;
		float pv;
		float scalex;
		float scaley;
		int texHandle;
		int count;
		char name[1];
	};
	void SetTexPiece(char* name, int handle, float pu, float pv, float scalex,
		float scaley);
	w_tex_piece* GetTexPiece(char* name);
	void ClearTexPiece();
	static WList<w_tex_piece*>* m_tex_piece;
	WList<WPuppet::w_tex_piece*> m_use_tex_piece;

public:
	WList<w_bound_box*> m_bbList;

protected:
	WList<w_motion_data*>* m_mdList;
	WList<w_face_animation*> m_fanimList;
	WList<WBone*> m_boneList;
	float m_aniLen;
	int m_iPetType;
	WBone* m_rootbone;
	WMatrixPtrList m_xTransfMatPtrList;
	struct w_original_matrix_set
	{
		WMatrixPtrList matPtrList;
		std::vector<WMatrix> matList;

		w_original_matrix_set() { }
		w_original_matrix_set(const w_original_matrix_set& other)
		{
			if (other.matPtrList.size() > 0)
			{
				matPtrList.resize(other.matPtrList.size());
				matList = other.matList;
				for (int i = 0; i < (int)other.matPtrList.size(); ++i)
					matPtrList[i] = &matList[i];
			}
		}
	};
	static std::map<std::string, w_original_matrix_set> ms_xOrginalMatrixList;
	struct w_pet_chunk;

private:
	static int box_v[8][3];
	static int box_p[6][4];
	static unsigned char ms_lockedBoneSetPos;
	static unsigned char ms_boneSetPos;

public:
	virtual void xUpdateTnLBuffers(bool) { }
	void xBuildTransfMatPtrList();
	const WMatrixPtrList& xGetTransfMatPtrList() const
	{
		return m_xTransfMatPtrList;
	}
	void ReplaceMeshBones(WPuppet* oldPuppet, WPuppet* newPuppet, int handle,
		int oldHandle);
	void ChangeTextureByHandle(int from, int to, WPuppet* other);
	virtual void EndMeshBoneWork();
	virtual void UpdateMeshBonePtr(WPuppet* source);
	WBone* FindMeshBone(WBone* previous);

protected:
	void FindMeshBone(WBone** found, WBone** previous, const char* name);

public:
	void ScaleBone(char* name, float scale);
	bool CompareBasicMatrix(WPuppet* other) const;
	static void AddOrginalMatrixPtrList(const std::string& name,
		const WMatrixPtrList& matPtrList);
	static bool HasOrginalMatrixList(const std::string& name);
	static const std::vector<WMatrix>& GetOrginalMatrixList(
		const std::string& name);
	static const WMatrixPtrList& GetOrginalMatrixPtrList(
		const std::string& name);

protected:
	void AttachMeshBone(WPuppet* other, int handle, int oldHandle);
	virtual void DetachMeshBone(WPuppet* source);
	void MergeNormal();
};
