#pragma once
#include "wmath.h"
#include "wresource.h"
#include "wscene.h"
#include "wlist.h"
#include <vector>
#include <string>
#include <map>

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
	struct w_frame
	{
		int num;
		int check;
		char* data;
	};

	char* petName;
	WBone* rootbone;
	float length;
	int faceNum;
	int numFramedata;
	w_frame* frame;
	Waabb bound;
	WList<w_bound_box*>* bbList;
	WList<w_motion_data*>* mdList;
	WList<w_face_animation*>* fanimList;
};

class WPuppet : public WResource
{
public:
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

	WPuppet();
	virtual ~WPuppet();
	void Share(w_share_pet_data* shareData);
	virtual WPuppet* MakeClone(bool needBuffer);
	virtual int LoadPET(char* name, bool flag, int loadFlags);
	virtual void UpdateLightSource(LightSet* light, bool addLight,
		WScene* scene);
	virtual void Render(WView* view, bool transform, float scale, bool flag0,
		bool flag1, bool software);
	virtual void xUpdateTnLBuffers(bool) { }
	virtual void EndMeshBoneWork();
	virtual void UpdateMeshBonePtr(WPuppet* source);

	void Transform(WView* view, float scale, int flags);
	const char* GetPuppetName() const { return m_petName; }
	WBone* GetRootBone() { return m_rootbone; }
	int GetLightMode() { return m_lightmode; }
	const WMatrixPtrList& xGetTransfMatPtrList() const
	{
		return m_xTransfMatPtrList;
	}
	static const WMatrixPtrList& GetOrginalMatrixPtrList(
		const std::string& name);

	__forceinline const WSphere& GetBoundSphere() const
	{
		return m_boundSphere;
	}

	void SetRenderMode(int mode);
	void SetLightLine(float value);
	void UpdateLightSource(WScene* scene);
	void RenderHierarchy(WView* view, bool transform, float value, float scale);
	void RenderNormals(WView* view, bool transform, float value, float scale);
	void ApplyBones(WBoneSet* set);
	void ApplyBones(WBoneSet* set, const WMatrix& matrix);
	void AddCollisionObject(const char* name, WBlockModel* model);
	void SetAlpha(unsigned char alpha);
	void SetAlpha(char* name, unsigned char alpha);
	unsigned char GetAlpha(char* name);
	void SetMirrored(bool mirrored);
	WBone* FindBone(int id);
	WBone* FindBone(const char* name);
	WBone* FindBoneByTail(const char* tail);
	void GetBoneName(char** names, int& count);
	WMatrix GetRootMatrix();
	WVector GetPivot(const char* name);
	void SetVtxColor(unsigned long color);
	void SetSelfIllum(char* name, bool enabled);
	void SetSelfIllumColor(char* name, unsigned long color);
	void ToggleSelfIllum(char* name);
	void ShowBone(char* name);
	void HideBone(char* name);
	void ScaleBone(char* name, float scale);
	void RotateBone(char* name, float angle, int axis);
	void MoveBone(char* name, WVector& vector);
	void SetPhysicsModel(const char* name, int model);
	void UpdatePhysics(float delta, const WVector& accel);
	void DisableFog();
	void EnableAlphaBlend();
	void OptimizeBone();
	bool CompareBasicMatrix(WPuppet* other) const;
	void ChangeTexture(char* name, int handle);
	bool ChangeTexturePart(int handle, char* name, Bitmap** bitmap,
		tagRECT* rect);
	void ChangeTexturePart(int handle, Bitmap* bitmap, const tagRECT& rect);
	bool ChangeTexturePart(char* name, char* part, Bitmap** bitmap,
		tagRECT* rect);
	void ChangeTexturePart(char* name, Bitmap* bitmap, const tagRECT& rect);
	void ChangeTextureByHandle(int from, int to, WPuppet* other);
	bool ChangeMesh(char* name);
	WBone* FindMeshBone(WBone* previous);
	void ReplaceMeshBones(WPuppet* oldPuppet, WPuppet* newPuppet, int handle,
		int oldHandle);
	void ApplyFaceAnimation(char* name, float time);
	w_face_animation* FindFaceAnim(char* name);
	char* FindTexAniList(char* name);
	w_motion_data* FindMotionData(const char* name);
	float GetMotionLength(const char* name);
	char* GetNextMotionName(const char* name);
	int GetMotionCountIncludeName(const char* include, const char* exclude);
	w_motion_data* GetFirstMotionData() { return m_mdList->Start(); }
	bool AddMotion(const char* name, char* alias);
	void DelAddMotion(const char* name);
	void ClearAddMotion();
	WBoneSet* GetBones(const char* name, float time, int mode, char* bone,
		bool flag);
	WVector GetDeltaVec(float time);
	WVector GetDeltaVec(char* name, float time);
	WVector GetDeltaVec(char* name, float from, float to);
	bool CheckFrameData(int first, int last, int& from, int& to);
	float GetCamFOV(char* name, WBoneSet* set);
	const WMatrix& GetCamMatrix(char* name);
	void CalcBound(Waabb* box, bool flag);
	void CalcBound(bool reset, WPuppet* puppet, bool mode, char* name,
		Waabb* box);
	void UpdateBound(bool transform, bool flag);
	void UpdateBSphere();
	Waabb GetTransformedBoundBox() const;
	bool GetBoundBoxPlane(char* name, WPlane* plane);
	bool GetAlignBoundBoxPlane(char* name, WPlane* plane, Waabb* box);
	bool DotContact(char* name, const WVector& dot);
	void RenderBoundBox(const char* name, WView* view, unsigned long color,
		float size);
	void xBuildTransfMatPtrList();
	static bool HasOrginalMatrixList(const std::string& name);
	static const std::vector<WMatrix>& GetOrginalMatrixList(
		const std::string& name);
	static void AddOrginalMatrixPtrList(const std::string& name,
		const WMatrixPtrList& matPtrList);

protected:
	virtual void DetachMeshBone(WPuppet* source);
	void AttachMeshBone(WPuppet* other, int handle, int oldHandle);
	void FindMeshBone(WBone** found, WBone** previous, const char* name);
	void MergeNormal();
	void SetDefaultFaceAnimation();
	void SetBoundBox(char* name, Waabb* aabb, char* boneName);
	WBoneSet* GetFreeBoneSetList(bool flag);

private:
	void LoadPET(cFile* file, bool flag, char* directory, int type);
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
	static int SeekChunkFromPET(cFile* file, char* tag);
	void GetDirectory(char* path, char* directory);
	void GetName(char* path, char* name);
	void FixNormal();
	bool CheckStaticBone(WBone* bone);
	bool ReduceBone(WBone* bone);
	WBone* MergeBone(WBone* target, WBone* source);
	void MergeRootMotion(WList<w_motion_data*>* list, WBone* bone);
	void MakeTemporaryCenterBox();
	void SetTexPiece(char* name, int handle, float pu, float pv, float scalex,
		float scaley);
	w_tex_piece* GetTexPiece(char* name);
	void ClearTexPiece();
	static WList<w_tex_piece*>* m_tex_piece;
	static unsigned char ms_lockedBoneSetPos;
	static unsigned char ms_boneSetPos;
	static int box_v[8][3];
	static int box_p[6][4];

public:
	int m_lightmode;
	WList<WBone*> m_phybone;
	WMatrix m_mat;
	WList<w_motion_addition*> m_addedMotionList;
	int m_rendMode;
	bool m_bClone;
	bool m_bHaveCenter;
	w_share_pet_data::w_frame* m_frame;
	int m_framenum;
	char m_petName[0x40];
	int m_faceNum;
	Waabb m_bound;
	WSphere m_boundSphere;
	w_bound_box m_BBox;
	WList<int>* m_storeTexList;
	WList<WPuppet::w_tex_piece*> m_use_tex_piece;
	WList<w_bound_box*> m_bbList;
	WList<w_motion_data*>* m_mdList;
	WList<w_face_animation*> m_fanimList;
	WList<WBone*> m_boneList;
	float m_aniLen;
	int m_iPetType;
	WBone* m_rootbone;
	WMatrixPtrList m_xTransfMatPtrList;

protected:
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
};
