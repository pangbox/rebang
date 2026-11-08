#pragma once

#include <vector>
#include "wvideo.h"
#include "polysoup.h"
#include "aniobj.h"
#include "golfball.h"

struct trace_s;

class CQuadTree
{
public:
	struct sWater
	{
		Waabb aabb;
		int type;
	};

	struct sColData
	{
		unsigned char type;
		short objIndex;
		int triIndex;
		int frame;
		WVector pos;
		WVector vel;
	};

	CQuadTree();
	~CQuadTree();

	void Display();
	float GetGravity();
	void GravityModifier(float modifier);

	void SetSlopeFactor(float factor) { m_slopeFactor = factor; }

	void MakeBaseStructure(WPolySoup::sModel* model, bool bRebuild);
	void MakeObjStructure(short index, int modelIndex, bool bRebuild);
	void Reserve(short num);

	void SetPolySoup(WPolySoup* pPolySoup) { m_pPolySoup = pPolySoup; }
	void UpdateProperty();
	void ResetHoleData();
	void ResetVariable();
	void ResetWater();
	void AddWater(Waabb aabb, int type);
	void CalcGreenPlane();

	void GetMeshList(WVector pos);
	void GetMeshList(WVector pos, const WVector& dir);
	void GetMeshList(Waabb aabb);
	void GetObjList(WVector pos, const WVector& dir);
	void GetObjList(Waabb aabb);
	int GetMeshIndex(const WVector& pos, bool bObject);
	int GetObjIndex(const WVector& pos, bool bObject);
	WVector GetMeshNormal(const WVector& pos);
	WVector GetMeshNormal(int index);
	WVector GetMeshObjcet(const WVector& pos);
	int GetTextureHandle(const WVector& pos);
	const char* GetTextureName(const WVector& pos);
	unsigned char GetGroundType(WVector pos, bool bCheckTee,
		CGolfBall::eWater* water);
	float GetGroundSlope(int index) const;
	WVector GetGroundPoint(WVector pos, bool bObject, int* index, bool bWater);
	WVector ColObject(const WVector& pos, bool bObject, int* index,
		bool bDoubleSide);
	float GetHighestGround(WVector pos, int* index);
	float GetLowestGround(WVector pos);

	float GetLowest() const { return m_lowest; }
	float GetBasePosY(WVector pos, float height);
	float GetGreenPos(const WVector& pos);
	float GetShadowIntensity(const WVector& pos);
	bool IsOBArea(const WVector& pos);
	bool IsOverWater(WVector pos);
	bool IsFallenIntoWater(WVector pos, Waabb* aabb, bool bCheck);
	bool OverlappedWithObject(WVector pos, float radius);
	bool SimpleColTest(const WVector& from, const WVector& dir);
	float RayIntersectAll(const WVector& from, const WVector& dir, bool bAll);
	WVector GetRayIntersection(WVector from, WVector to);
	WVector GetRayIntersectionObj(WVector from, WVector to);
	WVector GetRayIntersectionWall(WVector from, WVector to);
	WVector TestCameraCollision(const WVector& pos);
	int CheckCamera();
	bool CheckPickRoom(WVector pos);
	float GetObbPos(Wobb& obb, bool bMax, int axis);
	void OBBToAABB(Wobb obb, Waabb& aabb, WVector& center);
	float IntersectRollingBallGround(const WVector& pos, const WVector& dir,
		const float& radius, int& index, WVector& normal, bool bRolling,
		bool bCheckWall, bool bDoubleSide);

	const char* GetBounceSound(int index) const;
	const char* GetRollSound(int index) const;
	const char* GetStepSound(int index) const;
	const char* GetStepSound(const WVector& pos);
	float GetMaxPowerOnGround() const;
	float GetMinPowerOnGround() const;
	float GetMaxPowerOnGround(const WVector& pos);
	float GetMinPowerOnGround(const WVector& pos);

	void SetBounceFactor(float factor) { m_bounceFactor = factor; }
	void SetRollFactor(float factor) { m_rollFactor = factor; }
	void SetCondition(float condition) { m_condition = condition; }
	void SetCondition();

	float GetGreenFactor() const { return m_greenFactor; }
	void SetGreenFactor(float factor) { m_greenFactor = factor; }

	void SetBaseVtxColor(unsigned long color);
	void SetObjVtxColor(unsigned long color);
	void UpdateVtxColor(float dt);

	void GetAABB(Waabb& aabb) { aabb = m_pBaseModel->aabb; }

	void SetAvatarPosition(const WVector& pos, float size);

	void PaintIndex(int index) { m_paintIndex = index; }

	void DecalFragments(const WVector* points, const WVector& normal,
		WVector* vtx, int* vtxNum, int vtxMax, unsigned short* idx, int* idxNum,
		int idxMax, bool bObject);
	void DrawDecal(int texHandle, const WVector& pos, const WVector& right,
		const WVector& up, float alpha, float scale, enumPolyBlend blend,
		bool bObject);

	void TraceAABB(const WVector& from, const WVector& to, const Waabb& aabb,
		trace_s* trace);
	void TraceOBB(const WVector& from, const WVector& to, const Waabb& aabb,
		trace_s* trace);
	void TraceOBB_New(const WVector& from, const WVector& to, const Waabb& aabb,
		trace_s* trace);
	bool TraceAABB_BackGround(const WVector& from, const WVector& to,
		const Waabb& aabb, trace_s* trace, WPolySoup::sModel* model);
	bool TraceOBB_Object(const WVector& from, const WVector& to,
		const Waabb& aabb, trace_s* trace, WPolySoup::sModel* model,
		bool bCheck);
	void TraceOBB_Wall(const WVector& from, const WVector& to, trace_s* trace,
		WPolySoup::sModel* model, int index, bool bCheck);
	void TraceOBB_WallSliding(const WVector& from, const WVector& to,
		trace_s* trace, WPolySoup::sModel* model, int index, bool bCheck);
	void TraceOBB_Avatar_genPosition(const WVector& from, const WVector& to,
		trace_s* trace, WPolySoup::sModel* model);
	int Get_WallSlidingAngle(const WVector& dir, WPolySoup::sModel* model,
		WVector& normal);
	void DrawDebugTraceAABB(const WVector& from, const WVector& to,
		const Waabb& aabb);
	void DrawDebugTraceOBB(const WVector& from, const WVector& to,
		const Waabb& aabb);

	struct sColResponse
	{
		struct GolfBallClone
		{
			GolfBallClone(const CGolfBall& ball);

			float GetRadius() const { return pBall->GetRadius(); }
			unsigned long GetSpecial() const { return pBall->m_special; }
			float GetInitSpin() const { return pBall->m_initSpin; }
			int GetRenderNum() const { return pBall->m_renderNum; }
			int GetTopFrame() const { return pBall->m_topFrame; }
			int GetAttrIndex() const { return pBall->m_attrIndex; }

			WVector pos;
			WVector vel;
			short colObjIndex;
			int colTriIndex;
			int state;
			const CGolfBall* pBall;
		};

		sColResponse(const CGolfBall& ball, const WVector& pos,
			const WVector& vel);

		GolfBallClone ball;
		unsigned char unused28[32];
		char sound[64];
		unsigned char unused88[4];
		WVector normal;
		unsigned char unused98[4];
		WVector pos;
		WVector vel;
		bool bRadiusOffset;
		bool bCommit;
	};

	void PreProcess(float dt, bool bOneFrame);
	void BallProcess(float dt);
	void HoleIn();
	int GetObjColNum();
	int GetBounceCount();

	WVector GetPenaltyPos() { return m_penaltyPos; }

	void SetCaddieBox(const Waabb& box) { m_caddieBox = box; }
	unsigned char CollideBetweenBallandCaddie() { return m_caddieCol; }
	float GetHoleCupRadius() const { return m_holeCupRadius; }

	void SetFrame(unsigned long frame)
	{
		m_frame = frame;
		m_frameTime = 0.0f;
	}

	unsigned long GetFrame() { return m_frame; }

	void SetRandSeed(unsigned long seed) { m_randSeed = seed; }

	const std::vector<sColData>& GetColList() { return m_colList; }

	const std::vector<WPolySoup::sTriangle*>* GetTriArray(int texHandle) const
	{
		return m_pPolySoup->GetTriArray(texHandle);
	}

protected:
	bool IsValid(const WVector& pos);
	float IntersectPlane(const WVector& from, const WVector& dir,
		const WPlane& plane, bool bDoubleSide);
	WVector ColTest(const WVector& pos, bool bObject, int* index,
		bool bDoubleSide);
	float CheckCollision(const WVector& pos, const WVector& dir, int* index,
		bool bDoubleSide);
	bool ObjColTest(const WVector& dir, short objIndex, WPlane* plane, float& t,
		int& index);
	float IntersectObj(const WVector& pos, const WVector& dir, WPlane* plane);
	void ObjResponse(const WVector& dir, float t, const WVector& normal,
		sColResponse& response);
	void CommitResponse(CGolfBall& ball, const sColResponse& response,
		int sample);
	void RollingBallGroundResponse(CGolfBall& ball, float t, const WVector& dir,
		int index, const WVector& normal);
	void BounceResponse(const WVector& normal);
	void BounceProcess(float dt);
	void RollProcess(float dt);
	void SpinProcess(float dt);
	void ScriptProcess(int index, float from, float to);
	void ParsePuppetScript(int index, const char* script);

private:
	unsigned long MakeRand(unsigned long range);
	unsigned char GetObjQuad(const WVector& center, const WVector& pos);
	void MakeCollisionData(short index);
	void CalcObjMeshData(short index, int tri);
	float GetRollCoeff(int index) const;
	float GetBounceCoeff(int index) const;
	float GetWeatherRollCoeff(int index) const;
	float GetWeatherBounceCoeff(int index) const;
	bool IsPenetrable(int index) const;
	const char* FindTexture(int texHandle);
	bool PointInTriangle(const WVector& p, const WVector& a, const WVector& b,
		const WVector& c) const;
	bool SameSide(const WVector& p1, const WVector& p2, const WVector& a,
		const WVector& b) const;
	WVector closestPointOnTriangle(const WVector& a, const WVector& b,
		const WVector& c, const WVector& p) const;
	WVector closestPointOnLine(const WVector& a, const WVector& b,
		const WVector& p) const;
	float intersectPolygon(const WVector& p, const WVector& from,
		const WVector& dir) const;
	float IntersectRayTriangle(const WVector& from, const WVector& dir,
		const WVector* tri);
	float IntersectPoint(WVector dir);
	WVector DistanceBetweenRayNSegment(const WVector& from, const WVector& dir,
		const WVector& start, const WVector& seg);
	WVector GetCollisionPoint(const WVector& normal);
	WVector ApplyForce();
	WVector GetFriction();
	bool StuckInBunker(const WVector& pos);
	void MakeAfterBurner();
	void MakeVectorSlide(float x, float y, float z, int index);
	void HoleProcess(float dt);
	void SetPenaltyPos(const Waabb& area);
	void ClipDecalPolygon(WVector* poly, int num, int plane);
	void ClipDecalTriangle(const WPolySoup::sTriangle* tri, WVector dir);

	WPolySoup* m_pPolySoup;
	WPolySoup::sModel* m_pBaseModel;
	int m_minX;
	int m_minZ;
	int m_maxX;
	int m_maxZ;
	int m_meshIndex[2048];
	int m_meshNum;
	short m_objIndex[32];
	unsigned char m_objNum;
	int m_quadNumX;
	int m_quadNumZ;
	WVector m_origin;
	std::vector<sWater> m_water;
	WVector m_penaltyPos;
	float m_rollFactor;
	float m_bounceFactor;
	float m_condition;
	Waabb m_greenArea;
	WVector m_greenCenter;
	float m_greenFactor;
	WPlane m_greenPlane;
	int m_paintIndex;
	int m_waterBounceNum;
	WVector* m_decalVtx;
	int m_decalVtxNum;
	int m_decalVtxMax;
	WPlane m_decalPlane[6];
	Waabb m_avatarBox;
	WPolySoup::sQuad* m_quad;
	Waabb m_caddieBox;
	unsigned char m_caddieCol;
	float m_holeCupRadius;
	float m_holeBottom;
	int m_holeCount;
	WVector m_holeDist;
	float m_lowest;
	bool m_bUseLowest;
	bool m_bWind;
	bool m_bMeshOverflow;
	WVector m_baseVtxColor;
	WVector m_objVtxColor;
	int m_vtxColorMode;
	float m_frameTime;
	unsigned long m_frame;
	std::vector<CAniObj*> m_aniObj;
	unsigned long m_randSeed;
	float m_slopeFactor;
	sColData m_colData;
	std::vector<sColData> m_colList;
	unsigned char m_reserved[16];
	char* m_pReserved0;
	char* m_pReserved1;
	float m_gravityModifier;
};

class CPVSPool
{
public:
	CQuadTree& GetInstance();
	static void SetInstanceIdx(int idx);

	static int ms_InstanceIdx;

private:
	CQuadTree m_instance[2];
};

CQuadTree& GetPVS();
void SetPVSIndex(int idx);

struct fragmentsCache_s
{
	bool used;
	WVector points[4];
	WVector normal;
	int numVerts;
	WVector verts[1024];
	int numIndices;
	unsigned short indices[1536];
};
