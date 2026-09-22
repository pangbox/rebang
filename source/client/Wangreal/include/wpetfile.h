#pragma once

class WBone;

struct w_pet_vertex
{
	struct w_blended
	{
		unsigned char blendWeight;
		WBone* bone;
	};

	float x;
	float y;
	float z;
	float weight;
	unsigned char blendWeight;
	char* bonename;
	w_blended* list;
	int listNum;
};

struct w_pet_tri_point
{
	int pos;
	float nx;
	float ny;
	float nz;
	float tu;
	float tv;
};

struct w_skin_info
{
	unsigned char ucBoneID;
	int iSVtxIdx;
	int nVtxs;
	int iSTriIdx;
	int nTris;
};

struct w_pet_texture
{
	char filename[32];
	char flag;
	unsigned char group;
	unsigned long diffuse;
	int handle;
};

struct w_pet_texture_info
{
	char filename[64];
	unsigned char alpha;
	unsigned long diffuse;
	unsigned long drawFlags;
	unsigned long xulDrawFlag2;
	int texHandle;
	int mapType;
	bool bBind;
	float pu;
	float pv;
	float scalex;
	float scaley;
	unsigned char group;
};

struct w_pet_face_animation
{
	unsigned char group;
	char animName[32];
	char fName[32];
};
