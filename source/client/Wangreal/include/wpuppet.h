#pragma once
#include "wmath.h"
#include "wresource.h"
#include "wscene.h"
#include "wlist.h"
#include <vector>

class WBone;
class WView;
class WScene;

struct w_bound_box_info;
struct w_motion_data;
struct w_face_animation;

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
	virtual void xUpdateTnLBuffers(bool rebuild);
	virtual void EndMeshBoneWork();
	virtual void UpdateMeshBonePtr(WPuppet* source);

	void Transform(WView* view, float scale, int flags);

	__forceinline const WSphere& GetBoundSphere() const
	{
		return m_boundSphere;
	}

protected:
	virtual void DetachMeshBone(WPuppet* source);

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
	std::vector<WMatrix const*> m_xTransfMatPtrList;
};
