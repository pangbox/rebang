#pragma once
#include "wmath.h"

class WBone;
struct WxBatch;
struct WxBatchGrp;

struct w_mesh
{
	int vtxNum, rigidNum, blendedRigidNum, blendedTotalNum;
	WVector* vecList;
	WVector* normList;
	WVector* blendedVecList;
	float* blendWeightList;
	WVector* blendedNormalList;
	ulong* originalVtxColorList;
	int maxBoneNum;
	float* weightList;
	int indexNum;
	ushort* indexList;
	int originalTexHandle;
	float originalTu, originalTv, originalScaleu, originalScalev;
	ulong diffuse;
	float (*uvBackup)[2];
	float (*uvData)[2];
	bool bEnv, bSpec;
	int group, flags;
	w_mesh* next;
	float alpha;
	WBone** boneList;
	WBone** blendedBoneList;
	ulong* vtxColorList;
	ulong** vtxColorPtrList;
	WVector* myNormalList;
	WVector* myBlendedNormalList;
	int texHandle;
	float tu, tv, scaleu, scalev;
	int clipFlag, drawFlag;
	WxBatchGrp* xpBatchGrp;
	WxBatch* xpBatch;
	int xiBaseVtxIdx, xiBaseIdxIdx, xiDrawFlag2;
	WBone* aabbBone;
	Waabb localAabb;
	char mpetName[64];
};
