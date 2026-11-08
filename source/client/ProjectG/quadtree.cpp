#include "wind.h"
#include "quadtree.h"
#include "golfball.h"
#include "club.h"
#include "contentsdoc.h"
#include "clientsetting.h"
#include "slicestr.h"
#include "fx.h"
#include "wbone.h"
#include "ntavatar.h"
#include "mathconsts.h"

extern char g_colTex[16][32];
extern int g_colNum;

const char* GetStrConst(const char* source, char* dest);

namespace intersection
{
	WVector closestPointOnLine(const WVector& a, const WVector& b,
		const WVector& p)
	{
		WVector c = p - a;
		WVector V = (b - a).Normalize();
		float d = (a - b).Magnitude();
		float t = V * c;

		if (t < 0.0f)
			return a;

		if (t > d)
			return b;

		V = V * t;
		return a + V;
	}

	WVector closestPointOnTriangle(const WVector& p, const WVector& a,
		const WVector& b, const WVector& c)
	{
		WVector Rab = closestPointOnLine(a, b, p);
		WVector Rbc = closestPointOnLine(b, c, p);
		WVector Rca = closestPointOnLine(c, a, p);

		float dAB = (Rab - p).Magnitude();
		float dBC = (Rbc - p).Magnitude();
		float dCA = (Rca - p).Magnitude();

		if (dAB < dBC)
		{
			if (dAB < dCA)
				return Rab;

			return Rca;
		}
		else
		{
			if (dBC < dCA)
				return Rbc;
		}

		return Rca;
	}

	float intersectPolygon(const WVector& p, const WVector& from,
		const WVector& dir, const float& radius)
	{
		WVector v = dir * -1.0f;

		WVector q = p - from;
		float a = v * v;
		float b = v * q;
		float c = q * q - radius * radius;
		float disc = b * b - c * a;

		if (disc < 0.0f)
			return 1.0f;

		float t[2];
		t[0] = (b * -1.0f - sqrtf(disc)) * (1.0f / a);
		t[1] = (sqrtf(disc) - b) * (1.0f / a);

		if (t[0] < 0.0f)
		{
			if (t[1] > 0.0f && t[1] < 1.0f)
				return t[1];
		}
		else if (t[0] < 1.0f)
		{
			return t[0];
		}

		return 1.0f;
	}

	float IntersectRayTriangle(const WVector& from, const WVector& dir,
		const WVector* tri)
	{
		WVector edge1, edge2, tvec, pvec, qvec;
		edge1 = tri[1] - tri[0];
		edge2 = tri[2] - tri[0];
		pvec = WCrossProduct(dir, edge2);
		float det = pvec * edge1;

		if (Wabs(det) < g_EPSILON)
			return 1.0f;

		float invDet = 1.0f / det;

		tvec = from - tri[0];

		float u = tvec * (pvec * invDet);
		if (u < 0.0f || u > 1.0f)
			return 1.0f;

		qvec = WCrossProduct(tvec, edge1);

		float v = dir * (qvec * invDet);
		if (v < 0.0f || v > 1.0f)
			return 1.0f;

		float t = edge2 * (qvec * invDet);

		return Min(t, 1.0f);
	}

	bool SameSide(const WVector& p1, const WVector& p2, const WVector& a,
		const WVector& b)
	{
		WVector cp1, cp2;
		cp1 = WCrossProduct(b - a, p1 - a);
		cp2 = WCrossProduct(b - a, p2 - a);

		if (cp1 * cp2 > 0.0f || WisEqual(cp1, WVector::ZERO, g_EPSILON) ||
			WisEqual(cp2, WVector::ZERO, g_EPSILON))
			return true;

		return false;
	}

	bool PointInTriangle(const WVector& p, const WVector& a, const WVector& b,
		const WVector& c)
	{
		if (SameSide(p, a, b, c) && SameSide(p, b, a, c) &&
			SameSide(p, c, a, b))
			return true;

		return false;
	}

	float IntersectPlane(const WVector& from, const WVector& dir,
		const WPlane& plane, bool bDoubleSide)
	{
		float dist0 = plane.normal * from + plane.dis;
		WVector to = from + dir;
		float dist1 = to.z * plane.normal.z + to.x * plane.normal.x +
			to.y * plane.normal.y + plane.dis;

		if (plane.normal * dir > 0.0f)
			return 1.0f;

		if (bDoubleSide && dist0 < 0.0f)
			return 0.0f;

		if (dist0 * dist1 <= 0.0f)
			return Abs(dist0) / (Abs(dist0) + Abs(dist1));

		return 1.0f;
	}
}

void ReflectVector(const WVector& normal, WVector& v)
{
	v = v - normal * (2.0f * (normal * v));
}

static bool IntersectOBB(const Wobb& box0, const Wobb& box1)
{
	double afEA[3];
	double afEB[3];
	afEA[0] = (box0.extend[0].Magnitude());
	afEA[1] = (box0.extend[1].Magnitude());
	afEA[2] = (box0.extend[2].Magnitude());
	afEB[0] = (box1.extend[0].Magnitude());
	afEB[1] = (box1.extend[1].Magnitude());
	afEB[2] = (box1.extend[2].Magnitude());

	static const double fCutoff = 0.999999;
	double aafC[3][3];
	double aafAbsC[3][3];
	double afAD[3];
	double fR0, fR1, fR;
	double fR01;
	int i;

	bool bExistsParallelPair = false;

	WVector kD = box1.center - box0.center;
	WVector akA[3];
	WVector akB[3];

	for (i = 0; i < 3; i++)
	{
		akA[i] = box0.extend[i];
		akA[i].Normalize();
	}

	for (i = 0; i < 3; i++)
	{
		akB[i] = box1.extend[i];
		akB[i].Normalize();
	}

	for (i = 0; i < 3; i++)
	{
		aafC[0][i] = akA[0] * akB[i];
		aafAbsC[0][i] = fabs(aafC[0][i]);
		if (aafAbsC[0][i] > fCutoff)
			bExistsParallelPair = true;
	}

	afAD[0] = akA[0] * kD;
	fR = fabs(afAD[0]);

	fR1 = afEB[0] * aafAbsC[0][0] + afEB[1] * aafAbsC[0][1] +
		afEB[2] * aafAbsC[0][2];
	fR01 = afEA[0] + fR1;
	if (fR > fR01)
		return false;

	for (i = 0; i < 3; i++)
	{
		aafC[1][i] = akA[1] * akB[i];
		aafAbsC[1][i] = fabs(aafC[1][i]);
		if (aafAbsC[1][i] > fCutoff)
			bExistsParallelPair = true;
	}

	afAD[1] = akA[1] * kD;
	fR = fabs(afAD[1]);

	fR1 = afEB[0] * aafAbsC[1][0] + afEB[1] * aafAbsC[1][1] +
		afEB[2] * aafAbsC[1][2];
	fR01 = afEA[1] + fR1;
	if (fR > fR01)
		return false;

	for (i = 0; i < 3; i++)
	{
		aafC[2][i] = akA[2] * akB[i];
		aafAbsC[2][i] = fabs(aafC[2][i]);
		if (aafAbsC[2][i] > fCutoff)
			bExistsParallelPair = true;
	}

	afAD[2] = akA[2] * kD;
	fR = fabs(afAD[2]);

	fR0 = afEB[0] * aafAbsC[2][0] + afEB[1] * aafAbsC[2][1] +
		afEB[2] * aafAbsC[2][2];
	fR01 = afEA[2] + fR0;
	if (fR > fR01)
		return false;

	fR = fabs(akB[0] * kD);
	fR0 = afEA[0] * aafAbsC[0][0] + afEA[1] * aafAbsC[1][0] +
		afEA[2] * aafAbsC[2][0];
	fR01 = fR0 + afEB[0];

	if (fR > fR01)
		return false;

	fR = fabs(akB[1] * kD);
	fR0 = afEA[0] * aafAbsC[0][1] + afEA[1] * aafAbsC[1][1] +
		afEA[2] * aafAbsC[2][1];
	fR01 = fR0 + afEB[1];

	if (fR > fR01)
		return false;

	fR = fabs(akB[2] * kD);
	fR0 = afEA[0] * aafAbsC[0][2] + afEA[1] * aafAbsC[1][2] +
		afEA[2] * aafAbsC[2][2];
	fR01 = fR0 + afEB[2];

	if (fR > fR01)
		return false;

	if (bExistsParallelPair)
		return true;

	fR = fabs(afAD[2] * aafC[1][0] - afAD[1] * aafC[2][0]);
	fR0 = afEA[1] * aafAbsC[2][0] + afEA[2] * aafAbsC[1][0];
	fR1 = afEB[1] * aafAbsC[0][2] + afEB[2] * aafAbsC[0][1];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	fR = fabs(afAD[2] * aafC[1][1] - afAD[1] * aafC[2][1]);
	fR0 = afEA[1] * aafAbsC[2][1] + afEA[2] * aafAbsC[1][1];
	fR1 = afEB[0] * aafAbsC[0][2] + afEB[2] * aafAbsC[0][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	fR = fabs(afAD[2] * aafC[1][2] - afAD[1] * aafC[2][2]);
	fR0 = afEA[1] * aafAbsC[2][2] + afEA[2] * aafAbsC[1][2];
	fR1 = afEB[0] * aafAbsC[0][1] + afEB[1] * aafAbsC[0][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	fR = fabs(afAD[0] * aafC[2][0] - afAD[2] * aafC[0][0]);
	fR0 = afEA[0] * aafAbsC[2][0] + afEA[2] * aafAbsC[0][0];
	fR1 = afEB[1] * aafAbsC[1][2] + afEB[2] * aafAbsC[1][1];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	fR = fabs(afAD[0] * aafC[2][1] - afAD[2] * aafC[0][1]);
	fR0 = afEA[0] * aafAbsC[2][1] + afEA[2] * aafAbsC[0][1];
	fR1 = afEB[0] * aafAbsC[1][2] + afEB[2] * aafAbsC[1][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	fR = fabs(afAD[0] * aafC[2][2] - afAD[2] * aafC[0][2]);
	fR0 = afEA[0] * aafAbsC[2][2] + afEA[2] * aafAbsC[0][2];
	fR1 = afEB[0] * aafAbsC[1][1] + afEB[1] * aafAbsC[1][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	fR = fabs(afAD[1] * aafC[0][0] - afAD[0] * aafC[1][0]);

	fR0 = afEA[0] * aafAbsC[1][0] + afEA[1] * aafAbsC[0][0];
	fR1 = afEB[1] * aafAbsC[2][2] + afEB[2] * aafAbsC[2][1];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	fR = fabs(afAD[1] * aafC[0][1] - afAD[0] * aafC[1][1]);

	fR0 = afEA[0] * aafAbsC[1][1] + afEA[1] * aafAbsC[0][1];
	fR1 = afEB[0] * aafAbsC[2][2] + afEB[2] * aafAbsC[2][0];
	if (fR > fR0 + fR1)
		return false;

	fR = fabs(afAD[1] * aafC[0][2] - afAD[0] * aafC[1][2]);

	fR0 = afEA[0] * aafAbsC[1][2] + afEA[1] * aafAbsC[0][2];
	fR1 = afEB[0] * aafAbsC[2][1] + afEB[1] * aafAbsC[2][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	return true;
}

int CPVSPool::ms_InstanceIdx;

CQuadTree& CPVSPool::GetInstance()
{
	return m_instance[ms_InstanceIdx];
}

void CPVSPool::SetInstanceIdx(int idx)
{
	ms_InstanceIdx = idx;
}

void SetPVSIndex(int idx)
{
	CPVSPool::SetInstanceIdx(idx);
}

CQuadTree& GetPVS()
{
	static CPVSPool s_pool;
	return s_pool.GetInstance();
}

void CQuadTree::AddWater(Waabb aabb, int type)
{
	sWater water;
	water.aabb = aabb;
	water.type = type;
	m_water.push_back(water);
}

void CQuadTree::ResetHoleData()
{
	m_greenArea.Reset();

	for (int i = 0; i < m_pPolySoup->GetAreaNum(); i++)
	{
		if (m_pPolySoup->GetArea(i)->type == 7)
		{
			m_greenArea = m_pPolySoup->GetArea(i)->aabb;
			break;
		}
	}

	WPolySoup::sPoint* point = m_pPolySoup->FindPoint("\xc1\xdf\xbd\xc9");

	if (point)
		m_greenCenter = GetGroundPoint(point->pos, false, NULL, false);
	else
		m_greenCenter = GetGroundPoint(GetHoleCupPos(), false, NULL, false);

	WVector pin = GOLFDOC()->GetHoleData().pin;
	m_holeBottom = pin.y - GolfBall().GetRadius() * 6.0f;

	m_frameTime = 0.0f;
	m_frame = 0;
	m_randSeed = 0;
	m_paintIndex = -1;

	m_gravityModifier = 1.0f;

	ResetVariable();
}

void CQuadTree::ResetVariable()
{
	m_colList.clear();

	g_colNum = 0;

	m_caddieCol = 0;
	m_waterBounceNum = 0;
	m_holeCount = 0;
	m_bMeshOverflow = false;
	m_bWind = true;
}

CQuadTree::CQuadTree()
	: m_pPolySoup(NULL), m_pBaseModel(NULL)
{
	m_quad = NULL;
	m_slopeFactor = 1.1f;
	m_bounceFactor = 1.0f;
	m_rollFactor = 1.0f;
	m_greenFactor = 0.2f;

	g_colNum = 0;

	m_holeCupRadius = GolfBall().GetRadius() * 3.0f;

	m_baseVtxColor = WVector(255.0f, 255.0f, 255.0f);
	m_objVtxColor = WVector(255.0f, 255.0f, 255.0f);

	m_vtxColorMode = 0;

	m_gravityModifier = 1.0f;
}

CQuadTree::~CQuadTree()
{
	for (int i = 0; i < m_quadNumX * m_quadNumZ; i++)
	{
		if (m_quad[i].meshIndex)
		{
			delete[] m_quad[i].meshIndex;
			m_quad[i].meshIndex = NULL;
		}
	}

	if (m_quad)
	{
		delete[] m_quad;
		m_quad = NULL;
	}

	m_water.clear();
	m_colList.clear();

	for (i = 0; i < m_aniObj.size(); i++)
	{
		if (m_aniObj[i])
		{
			delete m_aniObj[i];
			m_aniObj[i] = NULL;
		}
	}
	m_aniObj.clear();

	if (m_pReserved0)
	{
		delete[] m_pReserved0;
		m_pReserved0 = NULL;
	}
	if (m_pReserved1)
	{
		delete[] m_pReserved1;
		m_pReserved1 = NULL;
	}
	if (m_pReserved1)
	{
		delete[] m_pReserved1;
		m_pReserved1 = NULL;
	}
}

void CQuadTree::Reserve(short num)
{
	for (int i = 0; i < m_aniObj.size(); i++)
	{
		if (m_aniObj[i])
		{
			delete m_aniObj[i];
			m_aniObj[i] = NULL;
		}
	}
	m_aniObj.clear();

	if (num > 0)
		m_aniObj.reserve(num);
}

unsigned long CQuadTree::MakeRand(unsigned long range)
{
	while (m_randSeed < range)
		m_randSeed = m_randSeed * m_randSeed + 1;

	m_randSeed %= range;

	return m_randSeed;
}

void CQuadTree::MakeObjStructure(short index, int modelIndex, bool bRebuild)
{
	int i;
	int k;
	int n;
	WPolySoup::sModel* model;

	if (IsLocalContent(S4_REAL_MYROOM))
	{
		if (modelIndex != -1)
			model = m_pPolySoup->m_model[modelIndex];
		else
			model = m_pPolySoup->m_model[index];
	}
	else
	{
		model = m_pPolySoup->m_model[index];
	}

	if (!model->pet)
	{
		for (i = 0; i < m_aniObj.size(); i++)
		{
			if (m_aniObj[i] && m_aniObj[i]->m_modelId == model->id)
			{
				if (m_aniObj[i])
				{
					delete m_aniObj[i];
					m_aniObj[i] = NULL;
				}
				return;
			}
		}
		return;
	}

	if (model->type)
	{
		CAniObj* obj = NULL;

		if (bRebuild)
		{
			for (i = 0; i < m_aniObj.size(); i++)
			{
				if (m_aniObj[i] && m_aniObj[i]->m_modelId == model->id)
				{
					obj = m_aniObj[i];
					break;
				}
			}
		}

		unsigned long frames;
		if (model->pet->GetAnimationLength() > 0.0)
		{
			frames = (unsigned long)(50.0f * model->pet->GetAnimationLength());
		}
		else
		{
			frames = 50;
		}

		switch (model->type)
		{
		case 1:
		{
			CMoveObj* move;
			if (bRebuild)
				move = (CMoveObj*)obj;
			else
				move = new CMoveObj;

			move->m_standTime = 0.0f;
			move->m_moveTime = 1.0f;
			move->m_start = model->mat.pivot;
			move->m_end = model->mat.pivot;

			if (model->option[0])
			{
				char node[16];
				WSliceStr slice(model->option);
				slice.Get("*node", "%s", node);
				slice.Get("*stand", "%f", &move->m_standTime);
				slice.Get("*move", "%f", &move->m_moveTime);

				CNodeContainer* pNode = m_pPolySoup->FindNode(node);
				if (pNode)
				{
					move->m_start = pNode->GetList()[0];
					move->m_end = pNode->GetList()[1];
					model->mat.pivot = pNode->GetList()[0];
				}
			}

			obj = move;
		}
		break;

		case 2:
		{
			CRotObj* rot;
			if (bRebuild)
				rot = (CRotObj*)obj;
			else
				rot = new CRotObj;

			float size = Max(Max(model->aabb.max.x - model->aabb.min.x,
								 model->aabb.max.y - model->aabb.min.y),
				model->aabb.max.z - model->aabb.min.z);
			rot->m_invSize = 1.0f / size;
			obj = rot;
		}
		break;

		case 3:
		{
			CVtxObj* vtx;
			if (bRebuild)
				vtx = (CVtxObj*)obj;
			else
				vtx = new CVtxObj;

			obj = vtx;
		}
		break;

		case 4:
		{
			CTranObj* tran;
			if (bRebuild)
				tran = (CTranObj*)obj;
			else
				tran = new CTranObj;

			obj = tran;
		}
		break;

		default:
			break;
		}

		if (obj)
		{
			obj->m_startFrame = MakeRand(frames);
			obj->m_type = model->type;
			obj->m_modelId = index;

			if (!bRebuild)
				m_aniObj.push_back(obj);
		}
	}

	if (model->noCollision == 3)
		return;

	Waabb aabb;

	if (model->type != 1 && model->type != 3)
	{
		RECT rc;
		rc.left = Between(0, (int)((model->aabb.min.x - m_origin.x) * 0.05f),
			m_quadNumX - 1);
		rc.right = Between(0, (int)((model->aabb.max.x - m_origin.x) * 0.05f),
			m_quadNumX - 1);
		rc.top = Between(0, (int)((model->aabb.min.z - m_origin.z) * 0.05f),
			m_quadNumZ - 1);
		rc.bottom = Between(0, (int)((model->aabb.max.z - m_origin.z) * 0.05f),
			m_quadNumZ - 1);

		for (n = rc.top; n <= rc.bottom; n++)
		{
			for (i = rc.left; i <= rc.right; i++)
			{
				int q = n * m_quadNumX + i;

				if (m_quad[q].objNum == 0)
				{
					m_quad[q].objIndex = new short[4];
					m_quad[q].objIndex[0] = index;
					m_quad[q].objNum = 1;
					continue;
				}

				for (k = 0; k < m_quad[q].objNum; k++)
				{
					if (m_quad[q].objIndex[k] == index)
						break;
				}

				if (k < m_quad[q].objNum)
					continue;

				if (m_quad[q].objNum % 4 == 0)
				{
					short* temp = new short[m_quad[q].objNum];
					memcpy(temp, m_quad[q].objIndex,
						m_quad[q].objNum * sizeof(short));

					delete[] m_quad[q].objIndex;
					m_quad[q].objIndex = new short[m_quad[q].objNum + 4];
					memcpy(m_quad[q].objIndex, temp,
						m_quad[q].objNum * sizeof(short));

					delete[] temp;
				}

				m_quad[q].objIndex[m_quad[q].objNum] = index;
				m_quad[q].objNum++;
			}
		}
	}

	if (model->noCollision & 2)
		return;

	if (bRebuild)
	{
		delete[] model->tri;
		delete[] model->quad;
	}

	int faceNum = model->pet->m_faceNum;
	model->tri = new WPolySoup::sTriangle[faceNum];

	WBone* abv = model->pet->FindBoneByTail("abv");
	bool bAbv = abv != NULL;
	n = 0;

	for (w_bound_box* box = model->pet->m_bbList.Start(); box;
		box = model->pet->m_bbList.Next())
	{
		WBone* bone = box->bone;

		if (strcmpi(box->info->name, bone->GetBoneName()))
			continue;

		if (bAbv && !strstr(_strlwr(box->info->name), "abv"))
			continue;

		for (w_mesh* mesh = bone->GetMesh(); mesh; mesh = mesh->next)
		{
			for (i = 0; i < mesh->indexNum; i += 3)
			{
				WVector normal;
				int ofs = n * sizeof(WPolySoup::sTriangle);
				if (model->type == 0 || model->type == 4)
				{
					for (k = 0; k < 3; k++)
						model->tri[n].v[k] =
							mesh->vecList[mesh->indexList[i + k]] *
							bone->GetMatrix();

					WVector* v = (WVector*)((char*)model->tri + ofs);

					normal = WCrossProduct(v[2] - v[1], v[0] - v[1]);
					normal.Normalize();

					model->tri[n].plane.normal = normal;
					model->tri[n].plane.dis =
						model->tri[n].plane.normal * -1.0f * v[0];
				}

				model->tri[n].bone = bone;
				model->tri[n].bDoubleSide = (mesh->xiDrawFlag2 & 0x400) != 0;
				model->tri[n].propIndex =
					m_pPolySoup
						->m_texture[m_pPolySoup
								->m_texIndex[mesh->texHandle & 0x7ff]]
						.propIndex;
				model->tri[n].texHandle =
					m_pPolySoup
						->m_texture[m_pPolySoup
								->m_texIndex[mesh->texHandle & 0x7ff]]
						.texHandle;

				if (++n >= faceNum)
					goto build_quad;
			}
		}
	}

build_quad:
	if (model->pet->m_faceNum < 512 || model->type == 2 || model->type == 1 ||
		model->type == 3)
	{
		model->quad = new WPolySoup::sQuad[1];
		model->bSingleQuad = true;

		model->quad->meshIndex = new int[model->pet->m_faceNum];
		model->quad->meshNum = model->pet->m_faceNum;

		for (i = 0; i < model->pet->m_faceNum; i++)
			model->quad->meshIndex[i] = i;
		return;
	}

	model->quad = new WPolySoup::sQuad[8];
	model->bSingleQuad = false;

	WVector& pos = model->pos;
	pos.Reset();

	n = 0;
	box = model->pet->m_bbList.Start();
	while (box)
	{
		WBone* bone = box->bone;

		if (strcmpi(box->info->name, bone->GetBoneName()) == 0)
		{
			for (w_mesh* mesh = bone->GetMesh(); mesh; mesh = mesh->next)
			{
				for (i = 0; i < mesh->vtxNum; i++)
				{
					pos += mesh->vecList[i] * bone->GetMatrix();
					n++;
				}
			}
		}

		box = model->pet->m_bbList.Next();
	}

	model->pos /= (float)n;

	for (n = 0; n < model->pet->m_faceNum; n++)
	{
		WPolySoup::sTriangle* tri = &model->tri[n];

		aabb.min.x = Min(Min(tri->v[0].x, tri->v[1].x), tri->v[2].x);
		aabb.min.y = Min(Min(tri->v[0].y, tri->v[1].y), tri->v[2].y);
		aabb.min.z = Min(Min(tri->v[0].z, tri->v[1].z), tri->v[2].z);

		aabb.max.x = Max(Max(tri->v[0].x, tri->v[1].x), tri->v[2].x);
		aabb.max.y = Max(Max(tri->v[0].y, tri->v[1].y), tri->v[2].y);
		aabb.max.z = Max(Max(tri->v[0].z, tri->v[1].z), tri->v[2].z);

		int flag[8];
		for (k = 0; k < 8; k++)
			flag[k] = 1;

		int bit = 1;
		for (i = 0; i < 3; i++)
		{
			if (aabb.max.p[i] < model->pos.p[i])
			{
				for (k = 0; k < 8; k++)
				{
					if ((bit & k) == bit)
						flag[k] = 0;
				}
			}
			else if (aabb.min.p[i] > model->pos.p[i])
			{
				for (k = 0; k < 8; k++)
				{
					if (!(bit & k))
						flag[k] = 0;
				}
			}

			bit *= 2;
		}

		for (i = 0; i < 8; i++)
		{
			if (flag[i] == 1)
			{
				if (model->quad[i].meshNum == 0)
				{
					model->quad[i].meshIndex = new int[64];
					model->quad[i].meshIndex[0] = n;
					model->quad[i].meshNum = 1;
					continue;
				}

				if (model->quad[i].meshNum % 64 == 0)
				{
					int* temp = new int[model->quad[i].meshNum];
					memcpy(temp, model->quad[i].meshIndex,
						model->quad[i].meshNum * sizeof(int));

					delete[] model->quad[i].meshIndex;
					model->quad[i].meshIndex =
						new int[model->quad[i].meshNum + 64];
					memcpy(model->quad[i].meshIndex, temp,
						model->quad[i].meshNum * sizeof(int));

					delete[] temp;
				}

				model->quad[i].meshIndex[model->quad[i].meshNum] = n;
				model->quad[i].meshNum++;
			}
		}
	}
}

unsigned char CQuadTree::GetObjQuad(const WVector& center, const WVector& pos)
{
	unsigned char quad = 0;
	if (center.x > pos.x)
		quad = 1;
	if (center.y > pos.y)
		quad |= 2;
	if (center.z > pos.z)
		quad |= 4;

	return quad;
}

bool CQuadTree::ObjColTest(const WVector& dir, short objIndex, WPlane* plane,
	float& t, int& index)
{
	WPolySoup::sModel* model = m_pPolySoup->m_model[objIndex];
	const WVector& pos = model->pos;
	bool bCol = false;
	const int maxMesh = 2048;

	if (model->bSingleQuad)
	{
		m_meshNum = model->quad->meshNum;
		memcpy(m_meshIndex, model->quad->meshIndex, m_meshNum * sizeof(int));
	}
	else
	{
		const WVector& ballPos = GolfBall().m_pos;
		int from = GetObjQuad(ballPos, pos);
		int to = GetObjQuad(GolfBall().m_pos + dir, pos);

		if (from == to)
		{
			m_meshNum = model->quad[from].meshNum;
			memcpy(m_meshIndex, model->quad[from].meshIndex,
				m_meshNum * sizeof(int));
		}
		else
		{
			m_meshNum = 0;
			int a = 0;
			int b = 0;
			while (model->quad[from].meshNum && model->quad[to].meshNum)
			{
				if (model->quad[from].meshIndex[a] <
					model->quad[to].meshIndex[b])
				{
					m_meshIndex[m_meshNum] = model->quad[from].meshIndex[a];
					a++;
				}
				else if (model->quad[to].meshIndex[b] <
					model->quad[from].meshIndex[a])
				{
					m_meshIndex[m_meshNum] = model->quad[to].meshIndex[b];
					b++;
				}
				else
				{
					m_meshIndex[m_meshNum] = model->quad[from].meshIndex[a];
					a++;
					b++;
				}

				m_meshNum++;

				if (a == model->quad[from].meshNum)
				{
					if (b == model->quad[to].meshNum)
						break;

					int num =
						Min(model->quad[to].meshNum - b, maxMesh - m_meshNum);
					if (num < 0)
					{
						m_bMeshOverflow = true;
						return false;
					}

					memcpy(&m_meshIndex[m_meshNum],
						&model->quad[to].meshIndex[b], num * sizeof(int));
					m_meshNum += num;
					break;
				}

				if (b == model->quad[to].meshNum)
				{
					int num = maxMesh - m_meshNum;
					num = Min(model->quad[from].meshNum - a, num);
					if (num < 0)
					{
						m_bMeshOverflow = true;
						return false;
					}

					memcpy(&m_meshIndex[m_meshNum],
						&model->quad[from].meshIndex[a], num * sizeof(int));
					m_meshNum += num;
					break;
				}
			}

			if (m_meshNum > maxMesh)
				m_bMeshOverflow = true;
		}
	}

	float bottom, top;
	float dy = dir.y;
	if (dy > 0.0f)
	{
		bottom = GolfBall().m_pos.y - GolfBall().GetRadius();
		top = GolfBall().m_pos.y + dir.y + GolfBall().GetRadius();
	}
	else
	{
		bottom = GolfBall().m_pos.y - GolfBall().GetRadius() + dir.y;
		top = GolfBall().m_pos.y + GolfBall().GetRadius();
	}

	for (int i = 0; i < m_meshNum; i++)
	{
		if (objIndex == GolfBall().m_colObjIndex &&
			i == GolfBall().m_colTriIndex)
			continue;

		int tri = m_meshIndex[i], prop;

		if (model->tri[tri].bone->IsInvisible())
			continue;

		if (m_pPolySoup->GetProperty(prop = model->tri[tri].propIndex).pass & 1)
		{
			if ((GolfBall().m_special & 0x10) ||
				((GolfBall().m_special & 0x20) &&
					GolfBall().m_cobraFrame < 0) ||
				WisEqual(
					m_pPolySoup->GetProperty(prop = model->tri[tri].propIndex)
						.bound,
					1.0f, g_EPSILON))
				continue;

			if (GolfBall().m_special & 0x40)
				continue;
		}

		int above = 0, below = 0;
		WVector v[3];
		model->tri[tri].bEdge = false;

		for (int j = 0; j < 3; j++)
		{
			v[j] = model->tri[tri].v[j];

			if (v[j].y > top)
				above++;
			else if (v[j].y < bottom)
				below++;

			if (GolfBall().m_bounceFrame < 0 && GolfBall().m_nearObjFrame < 0 &&
				(GolfBall().m_pos - v[j]).Magnitude() < 3.0f)
				GolfBall().m_nearObjFrame = GolfBall().m_renderNum;
		}

		if (below == 3 || above == 3)
			continue;

		WPlane triPlane;
		triPlane = model->tri[tri].plane;
		if (model->tri[tri].bDoubleSide && triPlane * GolfBall().m_pos < 0.0f)
		{
			WVector& n = triPlane.normal;
			n.x = -n.x;
			n.y = -n.y;
			n.z = -n.z;
			triPlane.dis *= -1.0f;
		}

		WVector offset = triPlane.normal * GolfBall().GetRadius();
		WVector start;
		start = GolfBall().m_pos - offset;
		float dist = intersection::IntersectPlane(start, dir, triPlane, false);

		WVector hit;
		hit = start + dir * dist;

		if (dist < t)
		{
			if (intersection::PointInTriangle(hit, v[0], v[1], v[2]))
			{
				t = dist;
				*plane = model->tri[tri].plane;
				index = tri;
				bCol = true;
				if (model->tri[tri].bDoubleSide &&
					GolfBall().m_pos * model->tri[tri].plane < 0.0f)
				{
					plane->x = -plane->x;
					plane->y = -plane->y;
					plane->z = -plane->z;
					plane->dis *= -1.0f;
				}
			}
			else
			{
				WVector closest;
				closest = closestPointOnTriangle(hit, v[0], v[1], v[2]);
				float edge = intersectPolygon(closest, GolfBall().m_pos, dir);
				if (edge < t)
				{
					model->tri[tri].bEdge = true;
					t = edge;
					*plane = model->tri[tri].plane;
					index = tri;
					bCol = true;
					if (model->tri[tri].bDoubleSide &&
						GolfBall().m_pos * model->tri[tri].plane < 0.0f)
					{
						plane->x = -plane->x;
						plane->y = -plane->y;
						plane->z = -plane->z;
						plane->dis *= -1.0f;
					}
				}
			}
		}
	}

	return bCol;
}

void CQuadTree::MakeBaseStructure(WPolySoup::sModel* model, bool bRebuild)
{
	ResetWater();

	m_pBaseModel = model;

	if (m_quad)
	{
		delete[] m_quad;
		m_quad = NULL;
	}

	m_lowest = 100000.0f;
	m_origin =
		WVector(m_pBaseModel->aabb.min.x, 0.0f, m_pBaseModel->aabb.min.z);

	m_quadNumX = 1 -
		(int)((m_pBaseModel->aabb.max.x - m_pBaseModel->aabb.min.x) * -0.05f);
	m_quadNumZ = 1 -
		(int)((m_pBaseModel->aabb.max.z - m_pBaseModel->aabb.min.z) * -0.05f);

	if (bRebuild)
	{
		if (m_quad)
			delete[] m_quad;
		m_quad = NULL;
	}

	m_quad = new WPolySoup::sQuad[m_quadNumX * m_quadNumZ];

	for (int i = 0; i < m_pBaseModel->pet->GetFaceNum(); i++)
	{
		WPolySoup::sTriangle& tri = m_pBaseModel->tri[i];

		if (m_pBaseModel->tri[i].bDoubleSide)
		{
			if (Abs(m_pBaseModel->tri[i].plane.y) > cos(20.0f * g_DEGTORAD))
				m_lowest =
					Min(Min(m_lowest, tri.v[0].y), Min(tri.v[1].y, tri.v[2].y));
		}
		else
		{
			if (m_pBaseModel->tri[i].plane.y > cos(20.0f * g_DEGTORAD))
				m_lowest =
					Min(Min(m_lowest, tri.v[0].y), Min(tri.v[1].y, tri.v[2].y));
		}

		WRect rc;
		rc.x = Min(tri.v[0].x, Min(tri.v[1].x, tri.v[2].x));
		rc.w = Max(tri.v[0].x, Max(tri.v[1].x, tri.v[2].x));
		rc.y = Min(tri.v[0].z, Min(tri.v[1].z, tri.v[2].z));
		rc.h = Max(tri.v[0].z, Max(tri.v[1].z, tri.v[2].z));

		rc.x -= m_origin.x;
		rc.w -= m_origin.x;
		rc.y -= m_origin.z;
		rc.h -= m_origin.z;

		RECT rect;
		rect.left = (int)(rc.x * 0.05f);
		rect.right = (int)(rc.w * 0.05f);
		rect.top = (int)(rc.y * 0.05f);
		rect.bottom = (int)(rc.h * 0.05f);

		for (int z = rect.top; z <= rect.bottom; z++)
		{
			for (int x = rect.left; x <= rect.right; x++)
			{
				int idx = z * m_quadNumX + x;

				if (m_quad[idx].meshNum == 0)
				{
					m_quad[idx].meshIndex = new int[16];
					m_quad[idx].meshIndex[0] = i;
					m_quad[idx].meshNum = 1;

					continue;
				}

				int j;
				for (j = 0; j < m_quad[idx].meshNum; j++)
				{
					if (m_quad[idx].meshIndex[j] == i)
						break;
				}

				if (j < m_quad[idx].meshNum)
					continue;

				if (m_quad[idx].meshNum % 16 == 0)
				{
					int* temp = new int[m_quad[idx].meshNum];
					memcpy(temp, m_quad[idx].meshIndex,
						m_quad[idx].meshNum * sizeof(int));

					delete[] m_quad[idx].meshIndex;
					m_quad[idx].meshIndex = new int[m_quad[idx].meshNum + 16];
					memcpy(m_quad[idx].meshIndex, temp,
						m_quad[idx].meshNum * sizeof(int));

					delete[] temp;
				}

				m_quad[z * m_quadNumX + x]
					.meshIndex[m_quad[z * m_quadNumX + x].meshNum] = i;
				m_quad[z * m_quadNumX + x].meshNum++;
			}
		}
	}

	m_lowest = Max(m_lowest, m_pBaseModel->aabb.min.y) - 1.0f;
}

WVector CQuadTree::ColTest(const WVector& pos, bool bObject, int* index,
	bool bDoubleSide)
{
	WVector ground = pos;
	bool bFound = false;
	ground.y = -100000.0f;

	for (int i = 0; i < m_meshNum; i++)
	{
		int tri = m_meshIndex[i];

		if (!bDoubleSide && m_pBaseModel->tri[tri].bDoubleSide)
			continue;

		if (!m_pBaseModel->tri[tri].bDoubleSide &&
			m_pBaseModel->tri[tri].plane.y < 0.0f)
			continue;

		float slope = m_pBaseModel->tri[tri].plane.y;
		if ((slope > 0.0f ? slope : -slope) < 0.1f)
			continue;
		WVector v[3];
		for (int j = 0; j < 3; j++)
		{
			v[j] = m_pBaseModel->tri[tri].v[j];
			v[j].y = ground.y;
		}

		if (intersection::PointInTriangle(ground, v[0], v[1], v[2]))
		{
			float y = (m_pBaseModel->tri[tri].plane.x * ground.x +
						  m_pBaseModel->tri[tri].plane.z * ground.z +
						  m_pBaseModel->tri[tri].plane.dis) /
				m_pBaseModel->tri[tri].plane.y * -1.0f;

			if (y > ground.y)
			{
				ground.y = y;
				bFound = true;
				if (index)
					*index = tri;
			}
		}
	}

	if (!bFound)
	{
		if (bObject)
		{
			if (index)
				*index = -1;

			return pos;
		}

		if (GolfBall().m_pos.y < m_pBaseModel->aabb.min.y)
			ground.y = m_pBaseModel->aabb.min.y;
	}

	return ground;
}

WVector CQuadTree::ColObject(const WVector& pos, bool bObject, int* index,
	bool bDoubleSide)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return pos;

	int meshNum = 0;
	WVector ground = pos;

	WPolySoup::sModel* model = m_pPolySoup->m_model[0];
	meshNum = model->quad->meshNum;

	for (int i = 0; i < meshNum; i++)
	{
		int tri = model->quad->meshIndex[i];

		if (!bDoubleSide && model->tri->bDoubleSide)
			continue;

		const WPolySoup::sTriangle& t = model->tri[tri];
		if (!t.bDoubleSide && t.plane.y < 0.0f)
			continue;

		float slope = t.plane.y;
		if ((slope > 0.0f ? slope : -slope) < 0.1f)
			continue;

		WVector v[3];
		for (int j = 0; j < 3; j++)
			v[j] = model->tri[tri].v[j];

		if (intersection::PointInTriangle(ground, v[0], v[1], v[2]))
		{
			float y =
				(t.plane.x * ground.x + t.plane.z * ground.z + t.plane.dis) /
				t.plane.y * -1.0f;
			if (y > ground.y)
			{
				ground.y = y;

				return ground;
			}
		}
	}

	if (bObject)
	{
		if (index)
			*index = -1;

		return pos;
	}

	if (pos.y < m_pBaseModel->aabb.min.y)
		ground.y = m_pBaseModel->aabb.min.y;

	return ground;
}

WVector CQuadTree::TestCameraCollision(const WVector& pos)
{
	WVector camPos = m_pPolySoup->GetCameraPos(pos, GolfBall().m_pos);

	camPos = GetGroundPoint(camPos, true, NULL, true);
	camPos.y = (camPos.y < pos.y - GOLFDOC()->m_cameraData.clearance
					   ? pos.y - GOLFDOC()->m_cameraData.clearance
					   : camPos.y) +
		GOLFDOC()->m_cameraData.clearance;

	return camPos;
}

int CQuadTree::GetMeshIndex(const WVector& pos, bool bObject)
{
	int index = -1;
	std::vector<int> list;
	float minDist = 100000.0f;
	GetMeshList(pos);

	int i;
	for (i = 0; i < m_meshNum; i++)
	{
		int tri = m_meshIndex[i];
		if (!m_pBaseModel->tri[tri].bDoubleSide &&
			m_pBaseModel->tri[tri].plane.y < 0.0f)
			continue;

		WVector v[3];
		for (int j = 0; j < 3; j++)
		{
			v[j] = m_pBaseModel->tri[tri].v[j];
			v[j].y = pos.y;
		}

		if (intersection::PointInTriangle(pos, v[0], v[1], v[2]))
			list.push_back(m_meshIndex[i]);
	}

	if (list.size() == 1)
	{
		index = list[0];
	}
	else
	{
		int num = list.size();
		for (int i = 0; i < num; i++)
		{
			const WPolySoup::sTriangle& t = m_pBaseModel->tri[list[i]];
			float dist = t.plane.normal * pos + t.plane.dis;
			if (dist < 0.0f && t.bDoubleSide)
				dist *= -1.0f;

			if (fabs(dist) < fabs(minDist))
			{
				minDist = dist;
				index = list[i];
			}
		}
	}

	if (bObject)
	{
		for (i = 0; i < (int)m_water.size(); i++)
		{
			Waabb aabb = m_water[i].aabb;
			if (pos.x > aabb.min.x && pos.x < aabb.max.x &&
				pos.z > aabb.min.z && pos.z < aabb.max.z)
			{
				if (ColTest(pos, true, NULL, true).y < aabb.max.y)
				{
					index = -2;
					break;
				}
			}
		}
	}

	return index;
}

int CQuadTree::GetObjIndex(const WVector& pos, bool bObject)
{
	int index = -1;
	std::vector<int> list;
	float minDist = 100000.0f;

	if (!IsLocalContent(S4_REAL_MYROOM))
		return -1;

	GetObjList(pos, pos);

	for (int i = 0; i < m_objNum; i++)
	{
		if (!m_pPolySoup->m_model[m_objIndex[i]]->tri->bDoubleSide &&
			m_pPolySoup->m_model[m_objIndex[i]]->tri->plane.y < 0.0f)
			continue;

		WVector v[3];
		v[0] = m_pPolySoup->m_model[m_objIndex[i]]->tri->v[0];
		v[0].y = pos.y;
		v[1] = m_pPolySoup->m_model[m_objIndex[i]]->tri->v[1];
		v[1].y = pos.y;
		v[2] = m_pPolySoup->m_model[m_objIndex[i]]->tri->v[2];
		v[2].y = pos.y;

		if (intersection::PointInTriangle(pos, v[0], v[1], v[2]))
			list.push_back(m_objIndex[i]);
	}

	if (list.size() == 1)
	{
		index = m_objIndex[0];
	}
	else
	{
		int num = list.size();
		for (int i = 0; i < num; i++)
		{
			const WPolySoup::sTriangle* t = m_pPolySoup->m_model[list[i]]->tri;
			float dist = pos * t->plane.normal + t->plane.dis;
			if (dist < 0.0f && t->bDoubleSide)
				dist *= -1.0f;

			if (fabs(dist) < fabs(minDist))
			{
				minDist = dist;
				index = list[i];
			}
		}
	}

	return index;
}

WVector CQuadTree::GetGroundPoint(WVector pos, bool bObject, int* index,
	bool bWater)
{
	GetMeshList(pos);

	int meshIndex = -1;

	pos = ColTest(pos, bObject, &meshIndex, true);

	if (bObject && meshIndex == -1)
		pos.y = m_pBaseModel->aabb.min.y;

	if (index)
		*index = meshIndex;

	if (bWater)
	{
		for (int i = 0; i < (int)m_water.size(); i++)
		{
			Waabb aabb = m_water[i].aabb;

			if (pos.x > aabb.min.x && pos.x < aabb.max.x &&
				pos.z > aabb.min.z && pos.z < aabb.max.z)
			{
				if (pos.y < aabb.max.y)
				{
					pos.y = aabb.max.y;

					if (index)
						*index = -2;
					break;
				}
			}
		}
	}

	if (pos.y < m_pBaseModel->aabb.min.y)
	{
		pos.y = m_pBaseModel->aabb.min.y;

		if (index)
			*index = min(*index, -1);
	}

	return pos;
}

float CQuadTree::GetHighestGround(WVector pos, int* index)
{
	GetMeshList(pos);

	float highest = -100000.0f;
	bool bFound = false;

	for (int i = 0; i < m_meshNum; i++)
	{
		int meshIndex = m_meshIndex[i];

		if (Abs(m_pBaseModel->tri[meshIndex].plane.y) < 0.1f)
			continue;

		if (!m_pBaseModel->tri[meshIndex].bDoubleSide &&
			m_pBaseModel->tri[meshIndex].plane.y < 0.0f)
			continue;

		WVector v[3];
		for (int j = 0; j < 3; j++)
		{
			v[j] = m_pBaseModel->tri[meshIndex].v[j];
			v[j].y = pos.y;
		}

		if (PointInTriangle(pos, v[0], v[1], v[2]))
		{
			float height = (m_pBaseModel->tri[meshIndex].plane.x * pos.x +
							   m_pBaseModel->tri[meshIndex].plane.z * pos.z +
							   m_pBaseModel->tri[meshIndex].plane.dis) /
				m_pBaseModel->tri[meshIndex].plane.y * -1.0f;

			if (Abs(height - pos.y) < Abs(highest - pos.y))
			{
				bFound = true;
				if (index)
					*index = meshIndex;
				highest = height;
			}
		}
	}

	if (!bFound)
	{
		if (index)
			*index = -1;
		return pos.y;
	}

	return highest;
}

float CQuadTree::GetLowestGround(WVector pos)
{
	GetMeshList(pos);

	float lowest = 100000.0f;
	bool bFound = false;

	for (int i = 0; i < m_meshNum; i++)
	{
		int meshIndex = m_meshIndex[i];

		if (Abs(m_pBaseModel->tri[meshIndex].plane.y) < 0.1f)
			continue;

		if (!m_pBaseModel->tri[meshIndex].bDoubleSide &&
			m_pBaseModel->tri[meshIndex].plane.y < 0.0f)
			continue;

		WVector v[3];
		for (int j = 0; j < 3; j++)
		{
			v[j] = m_pBaseModel->tri[meshIndex].v[j];
			v[j].y = pos.y;
		}

		if (PointInTriangle(pos, v[0], v[1], v[2]))
		{
			bFound = true;
			lowest = Min(lowest,
				(m_pBaseModel->tri[meshIndex].plane.x * pos.x +
					m_pBaseModel->tri[meshIndex].plane.z * pos.z +
					m_pBaseModel->tri[meshIndex].plane.dis) /
					m_pBaseModel->tri[meshIndex].plane.y * -1.0f);
		}
	}

	if (!bFound)
		return pos.y;

	return lowest;
}

void CQuadTree::GetMeshList(WVector pos)
{
	m_meshNum = 0;
	pos -= m_origin;
	int z = Between(0, (int)(pos.z * 0.05f), m_quadNumZ - 1);
	int x = Between(0, (int)(pos.x * 0.05f), m_quadNumX - 1);

	for (int i = 0; i < m_quad[z * m_quadNumX + x].meshNum; i++)
	{
		int j;
		for (j = 0; j < m_meshNum; j++)
		{
			if (m_quad[z * m_quadNumX + x].meshIndex[i] == m_meshIndex[j])
				break;
		}

		if (j == m_meshNum)
		{
			m_meshIndex[m_meshNum] = m_quad[z * m_quadNumX + x].meshIndex[i];
			m_meshNum++;
		}
	}
}

void CQuadTree::GetMeshList(WVector pos, const WVector& dir)
{
	m_meshNum = 0;
	pos -= m_origin;

	RECT rc;
	rc.left = Between(0, (int)((dir.x > 0.0f ? pos.x : pos.x + dir.x) * 0.05f),
		m_quadNumX - 1);
	rc.right = Between(0, (int)((dir.x < 0.0f ? pos.x : pos.x + dir.x) * 0.05f),
		m_quadNumX - 1);
	rc.top = Between(0, (int)((dir.z > 0.0f ? pos.z : pos.z + dir.z) * 0.05f),
		m_quadNumZ - 1);
	rc.bottom = Between(0,
		(int)((dir.z < 0.0f ? pos.z : pos.z + dir.z) * 0.05f), m_quadNumZ - 1);

	int h = rc.bottom - rc.top + 1;
	int w = rc.right - rc.left + 1;
	int* flag = new int[w * h];

	if (Wabs(dir.x) < g_EPSILON)
	{
		for (int i = 0; i < h; i++)
			flag[i] = 1;
	}
	else
	{
		int* bound = new int[w + 1];

		for (int i = 0; i < w + 1; i++)
			bound[i] = -(int)(-(((float)(rc.left + i) * 20.0f - pos.x) *
									  (dir.z / dir.x) +
								  pos.z) *
						   0.05f) -
				rc.top;

		for (int x = 0; x < w; x++)
		{
			for (int z = 0; z < h; z++)
				flag[z * w + x] = (z >= Min(bound[x], bound[x + 1]) &&
									  z <= Max(bound[x], bound[x + 1]))
					? 1
					: 0;
		}

		delete[] bound;
	}

	for (int z = 0; z < h; z++)
	{
		for (int x = 0; x < w; x++)
		{
			if (flag[z * w + x])
			{
				for (int i = 0;
					i < m_quad[(rc.top + z) * m_quadNumX + rc.left + x].meshNum;
					i++)
				{
					int j;
					for (j = 0; j < m_meshNum; j++)
					{
						if (m_quad[(rc.top + z) * m_quadNumX + rc.left + x]
								.meshIndex[i] == m_meshIndex[j])
							break;
					}

					if (j == m_meshNum)
					{
						m_meshIndex[m_meshNum] =
							m_quad[(rc.top + z) * m_quadNumX + rc.left + x]
								.meshIndex[i];
						m_meshNum++;
					}
				}
			}
		}
	}

	delete[] flag;
}

void CQuadTree::GetMeshList(Waabb aabb)
{
	m_meshNum = 0;

	aabb.min -= m_origin;
	aabb.max -= m_origin;

	m_minX = Between(0, (int)(aabb.min.x * 0.05f), m_quadNumX - 1);
	m_maxX = Between(0, (int)(aabb.max.x * 0.05f), m_quadNumX - 1);
	m_minZ = Between(0, (int)(aabb.min.z * 0.05f), m_quadNumZ - 1);
	m_maxZ = Between(0, (int)(aabb.max.z * 0.05f), m_quadNumZ - 1);

	for (int z = m_minZ; z <= m_maxZ; z++)
	{
		for (int x = m_minX; x <= m_maxX; x++)
		{
			for (int i = 0; i < m_quad[z * m_quadNumX + x].meshNum; i++)
			{
				int j;
				for (j = 0; j < m_meshNum; j++)
				{
					if (m_quad[z * m_quadNumX + x].meshIndex[i] ==
						m_meshIndex[j])
						break;
				}

				if (j == m_meshNum)
				{
					m_meshIndex[m_meshNum] =
						m_quad[z * m_quadNumX + x].meshIndex[i];
					m_meshNum++;
				}
			}
		}
	}
}

void CQuadTree::GetObjList(Waabb aabb)
{
	m_objNum = 0;

	aabb.min -= m_origin;
	aabb.max -= m_origin;

	m_minX = Between(0, (int)(aabb.min.x * 0.05f), m_quadNumX - 1);
	m_maxX = Between(0, (int)(aabb.max.x * 0.05f), m_quadNumX - 1);
	m_minZ = Between(0, (int)(aabb.min.z * 0.05f), m_quadNumZ - 1);
	m_maxZ = Between(0, (int)(aabb.max.z * 0.05f), m_quadNumZ - 1);

	for (int z = m_minZ; z <= m_maxZ; z++)
	{
		for (int x = m_minX; x <= m_maxX; x++)
		{
			for (int i = 0; i < m_quad[z * m_quadNumX + x].objNum; i++)
			{
				int j;
				for (j = 0; j < m_objNum; j++)
				{
					if (m_quad[z * m_quadNumX + x].objIndex[i] == m_objIndex[j])
						break;
				}

				if (j == m_objNum)
				{
					m_objIndex[m_objNum] =
						m_quad[z * m_quadNumX + x].objIndex[i];
					m_objNum++;
				}
			}
		}
	}

	for (unsigned int i = 0; i < m_aniObj.size(); i++)
	{
		if (!m_aniObj[i])
			continue;

		if (m_aniObj[i]->m_type != 1 && m_aniObj[i]->m_type != 3)
			continue;

		if (m_pPolySoup->m_modelNum == m_pPolySoup->m_rmrModelNum)
		{
			m_objIndex[m_objNum] = m_aniObj[i]->m_modelId;
			m_objNum++;
		}
		else
		{
			for (int j = 0; j < m_pPolySoup->m_modelNum; j++)
			{
				if (m_aniObj[i]->m_modelId == m_pPolySoup->m_model[j]->id)
				{
					m_objIndex[m_objNum] = j;
					m_objNum++;
					break;
				}
			}
		}
	}
}

void CQuadTree::GetObjList(WVector pos, const WVector& dir)
{
	m_objNum = 0;
	pos -= m_origin;

	RECT rc;
	rc.left = Between(0, (int)((dir.x > 0.0f ? pos.x : pos.x + dir.x) * 0.05f),
		m_quadNumX - 1);
	rc.right = Between(0, (int)((dir.x < 0.0f ? pos.x : pos.x + dir.x) * 0.05f),
		m_quadNumX - 1);
	rc.top = Between(0, (int)((dir.z > 0.0f ? pos.z : pos.z + dir.z) * 0.05f),
		m_quadNumZ - 1);
	rc.bottom = Between(0,
		(int)((dir.z < 0.0f ? pos.z : pos.z + dir.z) * 0.05f), m_quadNumZ - 1);

	int h = rc.bottom - rc.top + 1;
	int w = rc.right - rc.left + 1;
	int* flag = new int[w * h];

	if (Wabs(dir.x) < g_EPSILON)
	{
		for (int i = 0; i < h; i++)
			flag[i] = 1;
	}
	else
	{
		int* bound = new int[w + 1];

		for (int i = 0; i < w + 1; i++)
		{
			bound[i] = -(int)(-(((float)(rc.left + i) * 20.0f - pos.x) *
									  (dir.z / dir.x) +
								  pos.z) *
						   0.05f) -
				rc.top;
		}

		if (dir.z / dir.x >= 0.0f)
		{
			for (int x = 0; x < w; x++)
			{
				for (int z = 0; z < h; z++)
					flag[z * w + x] =
						(z >= bound[x] && z <= bound[x + 1]) ? 1 : 0;
			}
		}
		else
		{
			for (int x = 0; x < w; x++)
			{
				for (int z = 0; z < h; z++)
					flag[z * w + x] =
						(z >= bound[x + 1] && z <= bound[x]) ? 1 : 0;
			}
		}

		delete[] bound;
	}

	for (int z = 0; z < h; z++)
	{
		for (int x = 0; x < w; x++)
		{
			if (flag[z * w + x])
			{
				for (int i = 0;
					i < m_quad[(rc.top + z) * m_quadNumX + rc.left + x].objNum;
					i++)
				{
					int j;
					for (j = 0; j < m_objNum; j++)
					{
						if (m_quad[(rc.top + z) * m_quadNumX + rc.left + x]
								.objIndex[i] == m_objIndex[j])
							break;
					}

					if (j == m_objNum)
					{
						m_objIndex[m_objNum] =
							m_quad[(rc.top + z) * m_quadNumX + rc.left + x]
								.objIndex[i];
						m_objNum++;
					}
				}
			}
		}
	}

	delete[] flag;

	for (unsigned int i = 0; i < m_aniObj.size(); i++)
	{
		if (!m_aniObj[i])
			continue;

		if (m_aniObj[i]->m_type != 1 && m_aniObj[i]->m_type != 3)
			continue;

		if (m_pPolySoup->m_modelNum == m_pPolySoup->m_rmrModelNum)
		{
			m_objIndex[m_objNum] = m_aniObj[i]->m_modelId;
			m_objNum++;
		}
		else
		{
			for (int j = 0; j < m_pPolySoup->m_modelNum; j++)
			{
				if (m_aniObj[i]->m_modelId == m_pPolySoup->m_model[j]->id)
				{
					m_objIndex[m_objNum] = j;
					m_objNum++;
					break;
				}
			}
		}
	}
}

float CQuadTree::GetGroundSlope(int index) const
{
	if (index < 0)
		return 0.0f;

	const WPolySoup::sTriangle& tri = m_pBaseModel->tri[index];
	const WPlane& plane = tri.plane;
	WVector normal = plane.normal;
	if (tri.bDoubleSide && normal.y < 0.0f)
		normal.y = -normal.y;

	float angle =
		acos(normal.y < -1.0f ? -1.0f : (normal.y > 1.0f ? 1.0f : normal.y));

	return Abs(angle);
}

WVector CQuadTree::GetMeshNormal(int index)
{
	if (index < 0)
		return GetMeshNormal(GolfBall().m_pos);

	const WPolySoup::sTriangle& tri = m_pBaseModel->tri[index];
	WVector normal = tri.plane.normal;
	if (tri.bDoubleSide && normal.y < 0.0f)
	{
		normal.x = -normal.x;
		normal.y = -normal.y;
		normal.z = -normal.z;
	}

	return normal;
}

WVector CQuadTree::GetMeshNormal(const WVector& pos)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return WVector::ZERO;

	int index = GetMeshIndex(pos, false);

	if (index < 0)
		return WVector::ZERO;

	const WPolySoup::sTriangle& tri = m_pBaseModel->tri[index];
	WVector normal = tri.plane.normal;
	if (tri.bDoubleSide && normal.y < 0.0f)
	{
		normal.x = -normal.x;
		normal.y = -normal.y;
		normal.z = -normal.z;
	}

	return normal;
}

WVector CQuadTree::GetMeshObjcet(const WVector& pos)
{
	int index = GetObjIndex(pos, false);

	if (index < 0)
		return WVector::ZERO;

	WVector normal = m_pPolySoup->m_model[m_objIndex[index]]->pos;

	if (m_pPolySoup->m_model[m_objIndex[index]]->tri->bDoubleSide &&
		normal.y < 0.0f)
	{
		normal.x = -normal.x;
		normal.y = -normal.y;
		normal.z = -normal.z;
	}

	return normal;
}

bool CQuadTree::PointInTriangle(const WVector& p, const WVector& a,
	const WVector& b, const WVector& c) const
{
	return intersection::PointInTriangle(p, a, b, c);
}

bool CQuadTree::SameSide(const WVector& p1, const WVector& p2, const WVector& a,
	const WVector& b) const
{
	return intersection::SameSide(p1, p2, a, b);
}

float CQuadTree::IntersectPlane(const WVector& from, const WVector& dir,
	const WPlane& plane, bool bDoubleSide)
{
	return intersection::IntersectPlane(from, dir, plane, bDoubleSide);
}

float CQuadTree::IntersectObj(const WVector& pos, const WVector& dir,
	WPlane* plane)
{
	float t = 1.0f;
	short objIndex = -1;
	int triIndex = -1;
	WVector radius = GolfBall().GetRadius() * WVector::ONE;
	WVector ext = radius + WVector(Abs(dir.x), Abs(dir.y), Abs(dir.z));

	Waabb aabb;
	aabb.min = pos - ext;
	aabb.max = pos + ext;
	GetObjList(aabb);

	for (unsigned int i = 0; i < m_objNum; i++)
	{
		WPolySoup::sModel* model = m_pPolySoup->m_model[m_objIndex[i]];

		Waabb box;

		box.min = model->aabb.min - ext;
		box.max = model->aabb.max + ext;

		if (!box.IsInclude(pos))
			continue;

		if (model->type == 2 || model->type == 1 || model->type == 3)
			MakeCollisionData(m_objIndex[i]);

		if (ObjColTest(dir, m_objIndex[i], plane, t, triIndex))
			objIndex = m_objIndex[i];
	}

	if (objIndex >= 0)
	{
		WPolySoup::sModel* model = m_pPolySoup->m_model[objIndex];

		if (GolfBall().m_special & 0x10)
			model->hitCount++;

		if ((GolfBall().m_special & 0x40) && GolfBall().m_topFrame >= 0 &&
			GolfBall().m_renderNum > GolfBall().m_topFrame)
			model->hitCount++;

		m_colData.objIndex = objIndex;
		m_colData.triIndex = triIndex;
		m_colData.type = model->type;
		m_colData.frame = GolfBall().m_renderNum;

		strcpy(g_colTex[g_colNum],
			FindTexture(model->tri[triIndex].texHandle & 0x7ff));
		g_colNum++;
		if (g_colNum >= 16)
			g_colNum = 15;

		GolfBall().m_attrIndex = model->tri[triIndex].propIndex;
	}

	return t;
}

int CQuadTree::GetTextureHandle(const WVector& pos)
{
	int index = GetMeshIndex(pos, false);
	if (index < 0)
		return 0;

	return m_pBaseModel->tri[index].texHandle;
}

const char* CQuadTree::GetTextureName(const WVector& pos)
{
	int index = GetMeshIndex(pos, false);
	if (index < 0)
		return NULL;

	return FindTexture(m_pBaseModel->tri[index].texHandle & 0x7ff);
}

void CQuadTree::MakeCollisionData(short index)
{
	WPolySoup::sModel* model = m_pPolySoup->m_model[index];
	WVector normal;

	if (model->pet)
	{
		int count = 0;
		for (w_bound_box* box = model->pet->m_bbList.Start(); box;
			box = model->pet->m_bbList.Next())
		{
			WBone* bone = box->bone;
			if (strcmpi(box->info->name, bone->GetBoneName()) == 0)
			{
				for (w_mesh* mesh = bone->GetMesh(); mesh; mesh = mesh->next)
				{
					for (int i = 0; i < mesh->indexNum; i += 3)
					{
						for (int j = 0; j < 3; j++)
							model->tri[count].v[j] =
								mesh->vecList[mesh->indexList[i + j]] *
								bone->GetMatrix();

						WVector* v = model->tri[count].v;

						normal = WCrossProduct(v[2] - v[1], v[0] - v[1]);
						normal.Normalize();

						model->tri[count].plane.normal = normal;
						model->tri[count].plane.dis =
							(model->tri[count].plane.normal * -1.0f) * v[0];
						count++;
					}
				}
			}
		}
	}
}

void CQuadTree::CalcObjMeshData(short index, int tri)
{
	WPolySoup::sModel* model = m_pPolySoup->m_model[index];

	if (model->pet)
	{
		for (w_bound_box* box = model->pet->m_bbList.Start(); box;
			box = model->pet->m_bbList.Next())
		{
			WBone* bone = box->bone;
			if (strcmpi(box->info->name, bone->GetBoneName()) == 0)
			{
				int count = 0;
				for (w_mesh* mesh = bone->GetMesh(); mesh; mesh = mesh->next)
				{
					for (int i = 0; i < mesh->indexNum; i += 3, count++)
					{
						if (count == tri)
						{
							for (int j = 0; j < 3; j++)
								model->tri[count].v[j] =
									mesh->vecList[mesh->indexList[i + j]] *
									bone->GetMatrix();
							return;
						}
					}
				}
			}
		}
	}
}

void CQuadTree::PreProcess(float dt, bool bOneFrame)
{
	if (bOneFrame)
	{
		m_frame++;
	}
	else
	{
		m_frameTime += dt;

		if (m_frameTime < 0.02f || Wabs(dt) < g_EPSILON)
			return;

		unsigned long frame = (int)(m_frameTime * 50.0f);

		m_frameTime -= frame * 0.02f;
		m_frame += frame;
	}

	if (Wind().HasSpecialWind())
	{
		unsigned long sec = m_frame / 50 % 170;
		if (IsLocalContent(S4_REPLAY_SYSTEM))
		{
			if (Doc()->m_playingMode != 2)
				Wind().SetSpecialWind(sec >= 80);
		}
		else
		{
			Wind().SetSpecialWind(sec >= 80);
		}

		if (!bOneFrame)
		{
			if (Wind().IsActiveSpecialWind())
			{
				if (!Wind().GetSpWindStartFrame())
					Wind().SetSpWindStartFrame(m_frame);
			}
			else
			{
				if (Wind().GetSpWindStartFrame())
					Wind().SetSpWindStartFrame(0);
			}

			Wind().GenerateFx(sec >= 60);
		}
	}

	WPolySoup::sModel* model;
	for (unsigned int i = 0; i < m_aniObj.size(); i++)
	{
		CAniObj* obj = m_aniObj[i];

		if (obj == NULL)
			continue;

		int id = obj->m_modelId;
		if (m_pPolySoup->GetModelNum() == m_pPolySoup->GetRmrModelNum())
		{
			model = m_pPolySoup->m_model[id];
		}
		else
		{
			for (int j = 0; j < m_pPolySoup->GetModelNum(); j++)
			{
				if (m_pPolySoup->m_model[j]->id == id)
				{
					model = m_pPolySoup->m_model[j];
					break;
				}

				model = NULL;
			}
		}

		if (model == NULL || model->pet == NULL)
			continue;

		if (!bOneFrame && obj->m_sound[0])
		{
			char name[16];
			sprintf(name, "sound%d", i);

			if (g_audio->IsPlaying(name))
			{
				float volume = 1.0f -
					(model->mat.pivot - g_camera.pivot).Magnitude() /
						(obj->m_soundDist * (100.0f * g_CM_TO_WU * 0.1f));
				g_audio->SetSFXVolumeWhilePlaying(name, Max(0.0f, volume));
			}
			else
			{
				obj->m_sound[0] = 0;
			}
		}

		if (model->type == 4 && bOneFrame)
			continue;

		obj->m_time = (m_frame + obj->m_startFrame) * 0.02f;

		switch (model->type)
		{
		case 2:
		{
			CRotObj* rot = (CRotObj*)obj;
			if (model->windFactor > 0.0f)
				rot->m_angle = Wind().GetGlobalIntensity() * rot->m_invSize *
					model->windFactor * obj->m_time * g_PI;
			else
				rot->m_angle = obj->m_time * model->windFactor * -3.14159265f;

			model->pet->ApplyBones(NULL, RotMat(rot->m_angle, 2) * model->mat);
			model->pet->Transform(NULL, 1.0f, 0);
			continue;
		}
		case 1:
		{
			CMoveObj* move = (CMoveObj*)obj;
			float time = obj->m_time -
				floor(obj->m_time / (move->m_standTime + move->m_moveTime)) *
					(move->m_standTime + move->m_moveTime);

			int dir = (int)floor(obj->m_time /
						  (move->m_standTime + move->m_moveTime)) %
				2;

			if (time > move->m_moveTime)
			{
				model->mat.pivot = (&move->m_start)[1 - dir];
			}
			else
			{
				model->mat.pivot = (&move->m_start)[dir] +
					((&move->m_start)[1 - dir] - (&move->m_start)[dir]) *
						(time / move->m_moveTime);
			}
			break;
		}
		case 3:
		{
			CVtxObj* vtx = (CVtxObj*)obj;
			break;
		}
		case 4:
		{
			CTranObj* tran = (CTranObj*)obj;
			break;
		}
		}

		if (model->pet && model->pet->GetAnimationLength() > 0.0f)
		{
			float time = Max(0.0f,
				obj->m_time -
					floor(obj->m_time / model->pet->GetAnimationLength()) *
						model->pet->GetAnimationLength());

			if (!bOneFrame)
				ScriptProcess(i, time - dt, time);

			model->pet->ApplyBones(
				model->pet->GetBones(NULL, time, 0, NULL, false), model->mat);
		}
		else
		{
			model->pet->ApplyBones(NULL, model->mat);
		}

		model->pet->Transform(NULL, 1.0f, 0);
	}
}

void CQuadTree::ScriptProcess(int index, float from, float to)
{
	WPuppet* pet = m_pPolySoup->m_model[m_aniObj[index]->m_modelId]->pet;

	if (from == to)
		return;

	from = Max(0.0f, from) * 30.0f;
	to *= 30.0f;
	float epsilon = pet->GetFrameCount()
		? ((pet->GetFrameDataTime(1) - pet->GetFrameDataTime(0) < 0.0f)
				  ? 0.0f
				  : pet->GetFrameDataTime(1) - pet->GetFrameDataTime(0))
		: 0.3f;
	int first = Ceil(from, epsilon);
	int last = (to == floor(to)) ? (int)to - 1 : (int)floor(to);

	if (first <= last)
	{
		int start, end;
		if (pet->CheckFrameData(first, last, start, end))
		{
			for (int i = start; i <= end; i++)
				ParsePuppetScript(index, pet->GetFrameData(i));
		}
	}
}

static const char* s_scriptCommand[4] = { "*fx(", "*snd(", "*hide", "*show" };

void CQuadTree::ParsePuppetScript(int index, const char* script)
{
	static char s_scriptToken[256];
	WPuppet* pet = m_pPolySoup->m_model[m_aniObj[index]->m_modelId]->pet;

	if (script == NULL)
		return;

	const char* p = script;
	while (p)
	{
		p = strchr(p, '*');
		if (p == NULL)
			return;

		int len = 0;
		while (p[len])
		{
			if (len >= 64)
				LogOut(0, "ParsePuppetScript : token too long\n");
			if (p[++len] == ' ')
				break;
			if (p[len] == '(')
			{
				len++;
				break;
			}
		}

		if (len <= 1)
			return;

		strncpy(s_scriptToken, p, len);
		s_scriptToken[len] = 0;
		p += len;

		for (unsigned int i = 0; i < 4; i++)
		{
			if (strcmpi(s_scriptToken, s_scriptCommand[i]) != 0)
				continue;

			switch (i)
			{
			case 0:
			{
				char name[64];
				name[0] = 0;
				p = GetStrConst(p, name);
				if (strlen(name) == 0 || strlen(name) >= 64)
					break;

				char filename[64];
				if (name[0] == '@')
				{
					p = GetStrConst(p, filename);
					if (strlen(filename) == 0 || strlen(filename) >= 64)
						break;
				}
				else
				{
					strcpy(filename, name);
				}

				if (pet)
				{
					WVector pivot = pet->GetPivot(NULL);
					WMatrix mat = pet->GetMatrix();
					unsigned char type =
						GetGroundType(pivot * mat, false, NULL);
					if ((type == 10 || type == 14) && strstr(filename, "dust"))
					{
						char ext[256] = { 0 };
						WSliceStr slice(filename);
						slice.Get("dust", "%s", ext);
						std::string strExt(ext);
						std::string strName(filename);
						std::string base = strName.substr(0,
							strName.length() - strExt.length() - 4);
						strName = base + "water" + strExt;
						strcpy(filename, strName.c_str());
					}
				}

				Attacher attacher;
				int j;
				for (j = 0; j < sizeof(attacher.mat) / sizeof(attacher.mat[0]);
					j++)
				{
					char bone[64];
					p = GetStrConst(p, bone);
					if (strlen(bone) == 0 || strlen(bone) >= 64)
						break;

					WBone* found = pet->FindBone(bone);
					if (found)
					{
						attacher.mat[j] = &found->GetMatrix();
					}
					else
					{
						w_bound_box* box = pet->m_bbList.Find(bone);
						if (box)
						{
							if (box->bone)
								attacher.mat[j] = &box->bone->GetMatrix();

							attacher.pos[j] = &box->info->spherePivot;
						}
					}
				}

				if (j > 0)
					CFx::Instance()->Attach(attacher, filename, true, NULL);
				break;
			}
			case 1:
			{
				char name[16];
				sprintf(name, "sound%d", index);

				char text[128];
				p = GetStrConst(p, text);

				int loop = 0;
				sscanf(text, "%s %d", m_aniObj[index]->m_sound, &loop);

				if (g_audio->IsPlaying(name))
					g_audio->StopSfx(name);

				WVector dist = m_pPolySoup->m_model[m_aniObj[index]->m_modelId]
								   ->mat.pivot -
					g_camera.pivot;

				g_audio->PlaySfx(m_aniObj[index]->m_sound, NULL, loop != 0,
					name);
				g_audio->SetSFXVolumeWhilePlaying(name,
					Max(0.0f,
						1.0f -
							dist.Magnitude() /
								(m_aniObj[index]->m_soundDist *
									(100.0f * g_CM_TO_WU * 0.1f))));
				break;
			}
			}
		}
	}
}

void CQuadTree::BallProcess(float dt)
{
	if (IsLocalContent(S3_ROOKIE_CHANNEL) && Doc()->m_golfGame.gameType != 11 &&
		Doc()->m_golfGame.gameType != 12)
	{
		if (Doc()->m_curChannel.Type & 0x800)
			m_holeCupRadius = GolfBall().GetRadius() * 6.0f;
		else
			m_holeCupRadius = GolfBall().GetRadius() * 3.0f;
	}

	CGolfBall& ball = GolfBall();

	m_bUseLowest = false;

	if (ball.m_state != CGolfBall::BALL_SPIN)
	{
		WVector delta =
			ball.PreData(ball.m_renderNum - 2 > 0 ? ball.m_renderNum - 2 : 0)
				->pos -
			ball.PreData(ball.m_renderNum - 1 > 0 ? ball.m_renderNum - 1 : 0)
				->pos;
		if (WisEqual(delta, WVector::ZERO, 0.00001f))
		{
			++ball.m_stayCount;
		}
		else
		{
			delta = ball.m_pos - GetHoleCupPos();
			if (sqrtf(delta.x * delta.x + delta.z * delta.z) < m_holeCupRadius)
			{
				if (++ball.m_stayCount == 100)
				{
					ball.m_pos = GetHoleCupPos();
					ball.m_pos.y = m_holeBottom + ball.GetRadius();
					HoleIn();
					return;
				}
			}
			else
			{
				ball.m_stayCount = 0;
			}
		}
	}

	ball.m_groundType = GetGroundType(ball.m_pos, true, NULL);

	WVector dist = ball.m_pos - GetHoleCupPos();
	dist.y = 0.0f;

	if (sqrtf(dist.x * dist.x + dist.z * dist.z) < m_holeCupRadius &&
		m_holeBottom + ball.GetRadius() > ball.m_pos.y)
	{
		ball.m_pos.y = m_holeBottom + ball.GetRadius();
		HoleIn();
		return;
	}

	if (ball.m_pos.y < m_pBaseModel->aabb.min.y)
	{
		ball.m_vel.x = ball.m_vel.y = ball.m_vel.z = 0.0f;
		ball.m_bOB = true;
		ball.m_state = CGolfBall::BALL_STOP;
		return;
	}

	if (ball.m_renderNum < 100 && !m_caddieCol)
	{
		WVector start = ball.m_pos;
		start.y = (m_caddieBox.min + m_caddieBox.max).y * 0.5f;

		if (RayIntersectAABB(m_caddieBox, start,
				WVector(dt * ball.m_vel.x, 0.0f, dt * ball.m_vel.z), NULL,
				0) < 1.0f)
		{
			if (ball.m_pos.y < m_caddieBox.min.y)
				m_caddieCol = 2;
			else if (ball.m_pos.y > m_caddieBox.max.y)
				m_caddieCol = 1;
			else
				m_caddieCol = 3;
		}
	}

	m_holeDist = DistanceBetweenRayNSegment(GetHoleCupPos() -
			WVector(0.0f, GolfBall().GetRadius() * 6.0f, 0.0f),
		WVector::UNIT_POS_Y, ball.m_pos, ball.m_vel * dt);

	if (ball.m_cupFrame < 0 && ball.m_rollFrame < 0 &&
		m_holeCupRadius - ball.GetRadius() > m_holeDist.z &&
		(ball.m_special & 0x20 ? ball.m_vel.y < 0.0f : true))
	{
		ball.m_cupFrame = ball.m_renderNum;
		ball.m_pos += m_holeDist.y * ball.m_vel * dt;

		dist = GetHoleCupPos() - ball.m_pos;
		dist.y = 0.0f;
		float factor =
			dist.Magnitude() * 0.4f / (m_holeCupRadius - ball.GetRadius()) +
			0.1f;

		ball.m_vel = WVector(factor * ball.m_vel.x, Abs(ball.m_vel.y) * -2.0f,
			factor * ball.m_vel.z);
		return;
	}

	WVector next = ball.m_pos + ball.m_vel * dt;
	dist = next - GetHoleCupPos();

	if (ball.GetRadius() + m_holeCupRadius > m_holeDist.z)
	{
		if (ball.m_state == CGolfBall::BALL_ROLL &&
			ball.m_vel.Magnitude() > 12.0f)
		{
			if (Abs(CalcDeltaAngle(WVector(dist.x, 0.0f, dist.z) * -1.0f,
					ball.m_vel)) < 1.0471976f)
			{
				ball.m_curve = ball.m_spin = 0.0f;
				ball.m_vel.y = Min(ball.m_vel.Magnitude() * 0.5f, 5.0f);
				ball.m_pos += ball.m_vel * dt;
				ball.m_state = CGolfBall::BALL_BOUNCE;
				ball.m_meshIndex = -1;

				CGolfBall::sExpected* data =
					ball.PreDataToWrite(ball.m_renderNum);
				data->effect = 2;
				data->event = 3;
				data->normal = WVector::UNIT_POS_Y;
				strcpy(data->sound, "\xc3\xe6\xb5\xb9_wood");
				ball.m_holeOutFrame = ball.m_renderNum;
				return;
			}
		}

		WVector ground = GetGroundPoint(next, false, NULL, false);
		if (ground.y + ball.GetRadius() + 0.01f < next.y)
		{
			ball.m_meshIndex = -1;
			ball.m_state = CGolfBall::BALL_BOUNCE;
		}
		else if (m_holeDist.z < m_holeCupRadius)
		{
			ball.m_state = CGolfBall::BALL_HOLE;

			if (ball.m_holeFrame < 0)
				ball.m_holeFrame = ball.m_renderNum;
		}
	}
	else
	{
		m_holeCount = 0;

		if (ball.m_state == CGolfBall::BALL_HOLE)
		{
			if (GetGroundPoint(next, false, NULL, false).y + ball.GetRadius() +
					0.01f <
				next.y)
			{
				ball.m_state = CGolfBall::BALL_BOUNCE;
			}
			else
			{
				WVector ground = GetGroundPoint(ball.m_pos, false, NULL, false);
				ball.m_state = CGolfBall::BALL_ROLL;
				ball.m_pos.y = ground.y + ball.GetRadius();
			}

			ball.m_holeOutFrame = ball.m_renderNum;
		}
	}

	switch (ball.m_state)
	{
	case CGolfBall::BALL_BOUNCE:
		BounceProcess(dt);
		break;

	case CGolfBall::BALL_SPIN:
		SpinProcess(dt);
		break;

	case CGolfBall::BALL_ROLL:
		RollProcess(dt);
		break;

	case CGolfBall::BALL_HOLE:
		HoleProcess(dt);
		break;
	}
}

bool CQuadTree::IsValid(const WVector& pos)
{
	return true;

	GetMeshList(pos);

	int count = 0;
	for (int i = 0; i < m_meshNum; i++)
	{
		const WPolySoup::sTriangle& tri = m_pBaseModel->tri[m_meshIndex[i]];

		WVector v[3];
		for (int j = 0; j < 3; j++)
		{
			v[j] = tri.v[j];
			v[j].y = pos.y;
		}

		if (!PointInTriangle(pos, v[0], v[1], v[2]))
			continue;

		if ((tri.plane * pos) * tri.plane.b > 0.0f)
			continue;

		if (tri.plane.b > 0.0f)
			count++;
		else if (tri.plane.b < 0.0f)
			count--;
	}

	return count % 2 == 0;
}

WVector CQuadTree::ApplyForce()
{
	WVector force(0.0f, 0.0f, 0.0f);

	if (!(Wabs(GolfBall().m_curve) < g_EPSILON) && GolfBall().m_cobraFrame < 0)
		force += GolfClub().GetCurve() * GolfBall().m_curve * 0.75f *
			WVector(GolfBall().m_vel.z * -1.0f, 0.0f, GolfBall().m_vel.x)
				.Normalize();
	else if (!(Wabs(GolfBall().m_curve) < g_EPSILON) &&
		GolfBall().m_spikeFrame < 0)
		force += GolfClub().GetCurve() * GolfBall().m_curve * 0.75f *
			WVector(GolfBall().m_vel.z * -1.0f, 0.0f, GolfBall().m_vel.x)
				.Normalize();

	if ((GolfBall().m_special & 0x20) && GolfBall().m_cobraFrame < 0)
		return force;

	if ((GolfBall().m_special & 0x40) && GolfBall().m_spikeFrame < 0)
		return force;

	if (m_bWind)
	{
		if (GolfBall().m_special & 0x40)
			force += Wind().GetWind(GolfBall().m_pos) * 0.01f;
		else
			force += Wind().GetWind(GolfBall().m_pos) * 0.02f;
	}

	float mass = GolfBall().GetMass();
	force.y -= m_gravityModifier * mass * 34.295296f;
	if (!(Wabs(GolfBall().m_spin) < g_EPSILON))
		force.y += GolfClub().GetSpin() * GolfBall().m_spin * 3.0f;
	force -= GolfBall().m_vel.Magnitude() * 0.00008f * GolfBall().m_vel;
	return force;
}

void CQuadTree::SetPenaltyPos(const Waabb& area)
{
	if (GolfBall().m_water == CGolfBall::WATER_SEA)
	{
		m_penaltyPos = GolfBall().PreData(0)->pos;
		return;
	}

	WVector dir = GolfBall().PreData(0)->pos - GolfBall().m_pos;
	dir.y = 0.0f;
	dir.Normalize();
	dir *= 16.0f;

	WVector ground;
	WVector pos[2];
	int count = 0;
	for (int i = 0; i < 2; ++i)
	{
		pos[count] = GolfBall().m_pos;

		while (true)
		{
			pos[count] += dir;
			ground = GetGroundPoint(pos[count], false, NULL, false);
			if (GetMeshIndex(ground, false) == -1)
				break;

			WVector top = ColTest(ground, true, NULL, true);
			Waabb aabb;
			if (!IsFallenIntoWater(ground, &aabb, true) && top.y > area.max.y &&
				!m_pPolySoup->IsOutOfBound(ground))
			{
				float limit = cos(20.0 * g_DEGTORAD);
				if (GetMeshNormal(ground).y > limit)
				{
					int j = 0;
					float angle = 0.0f;
					for (; j < 18; ++j)
					{
						ground =
							GetGroundPoint(pos[count] + dir * RotMat(angle, 1),
								false, NULL, false);
						if (GetMeshNormal(ground).y < limit)
							break;
						angle += 20.0f * g_DEGTORAD;
					}

					if (!OverlappedWithObject(ground, 1.6f) && j == 18)
					{
						++count;
						break;
					}
				}
			}
		}

		dir *= -1.0f;
	}

	switch (count)
	{
	case 0:
		m_penaltyPos = GolfBall().PreData(0)->pos;
		break;

	case 1:
		m_penaltyPos = pos[0];
		m_penaltyPos = GetGroundPoint(m_penaltyPos, false, NULL, false);
		m_penaltyPos.y += GolfBall().GetRadius();
		break;

	case 2:
	{
		WVector diff0 = WVector(GOLFDOC()->GetHoleData().pin) - pos[0];
		WVector diff1 = WVector(GOLFDOC()->GetHoleData().pin) - pos[1];
		m_penaltyPos = diff0.Magnitude() > diff1.Magnitude() ? pos[0] : pos[1];
		m_penaltyPos = GetGroundPoint(m_penaltyPos, false, NULL, false);
		m_penaltyPos.y += GolfBall().GetRadius();
		break;
	}

	default:
		m_penaltyPos = GolfBall().PreData(0)->pos;
		break;
	}
}

void CQuadTree::BounceProcess(float dt)
{
	bool bNoObj = false;

	CGolfBall& ball = GolfBall();
	CGolfBall::sExpected* pre = ball.PreDataToWrite(ball.m_renderNum);

	if ((GolfBall().m_special & 0x40) && GolfBall().m_topFrame >= 0 &&
		GolfBall().m_topFrame + 60 > GolfBall().m_renderNum)
		return;

	ball.m_vel += (1.0f / ball.GetMass()) * ApplyForce() * dt;

	if (GolfBall().m_special & 0x40)
		GolfBall().m_vel *= 1.0f;

	if (ball.m_colTriIndex != -1)
	{
		int triIndex = ball.m_colTriIndex;
		WVector normal = m_pPolySoup->m_model[ball.m_colObjIndex]
							 ->tri[triIndex]
							 .plane.normal;

		float d = normal * ball.m_vel;
		ball.m_vel -= normal * d;

		if (ball.m_vel.y > 0.0f)
		{
			ball.m_vel.Reset();
			ball.m_state = CGolfBall::BALL_STOP;
		}
	}

	ball.m_rot.x += dt * ball.m_curve * 12.566371f;
	if (GolfBall().m_special & 3)
		ball.m_rot.y += dt * ball.m_initSpin * 62.831856f;
	else
		ball.m_rot.y += dt * ball.m_spin * 25.132742f;

	if (ball.m_meshIndex >= 0 &&
		ball.PreDataToWrite(ball.m_renderNum - 1)->normal * ball.m_vel < 0.0f)
	{
		ball.m_vel -= ball.PreDataToWrite(ball.m_renderNum - 1)->normal *
			(ball.m_vel * ball.PreDataToWrite(ball.m_renderNum - 1)->normal);

		ball.m_pos = GetPVS().GetGroundPoint(ball.m_pos, false, NULL, false);
		ball.m_pos.y += ball.GetRadius();

		if (ball.m_special & 2)
		{
			MakeAfterBurner();
		}
		else
		{
			ball.m_rollFrame = ball.m_renderNum;
			ball.m_state = CGolfBall::BALL_ROLL;
		}

		const char* sound =
			GetRollSound(m_pBaseModel->tri[ball.m_meshIndex].propIndex);
		const float minSpeed = 20.0f;
		const WVector& burner = ball.m_afterBurner;
		float speed = (ball.m_vel + burner).Magnitude();
		if (sound[0] && speed > minSpeed)
			strcpy(pre->sound, sound);

		if (GetGroundType(ball.m_pos, true, NULL) == 2 && speed > minSpeed)
			ball.m_greenFrame = ball.m_renderNum;

		switch (ball.m_state)
		{
		case CGolfBall::BALL_SPIN:
			SpinProcess(dt);
			break;
		case CGolfBall::BALL_ROLL:
			RollProcess(dt);
			break;
		}
		return;
	}

	if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000013)
		bNoObj = true;

	WVector dir = ball.m_vel * dt;
	float t = CheckCollision(ball.m_pos, dir, &ball.m_meshIndex, false);

	Waabb aabb;
	WVector next = dir + ball.m_pos;
	const WVector& d = dir;

	if (d.y < 0.0f && IsFallenIntoWater(next, &aabb, false))
	{
		float waterT = (ball.m_pos.y - aabb.max.y) / d.y;
		if (waterT * -1.0f < t)
		{
			ball.m_pos += dir * (waterT * -1.0f);
			if (ball.m_bounceFrame < 0)
				ball.m_bounceFrame = ball.m_renderNum;

			ball.m_special &= ~0x20;

			const float maxAngle = 80.0f * g_DEGTORAD;
			const float minAngle = 60.0f * g_DEGTORAD;
			float angle = acos(Between(0.0f,
				ball.m_vel.y / ball.m_vel.Magnitude() * -1.0f, 1.0f));
			if (!(ball.m_special & 0x50) && angle < maxAngle &&
				ball.m_vel.y < -10.0f &&
				WVector(ball.m_vel.x, 0.0f, ball.m_vel.z).Magnitude() > 10.0f &&
				(angle > minAngle || (ball.m_special & 1)))
			{
				++m_waterBounceNum;

				ball.m_water = CGolfBall::WATER_NONE;
				ball.m_vel *= 1.0f - ball.m_initSpin * 0.1f;
				ball.m_vel.y =
					-((ball.m_initSpin * 0.1f + 0.75f) * ball.m_vel.y);

				pre->event = 4;
				pre->effect = 4;
				pre->normal = WVector::UNIT_POS_Y;
				strcpy(pre->sound, "\xc3\xe6\xb5\xb9_water");
			}
			else
			{
				ball.m_vel.Reset();
				ball.m_state = CGolfBall::BALL_STOP;

				ball.m_bOB = true;
				pre->effect = 4;
				strcpy(pre->sound, "\xc3\xe6\xb5\xb9_water");

				SetPenaltyPos(aabb);
			}
			return;
		}

		GolfBall().m_water = CGolfBall::WATER_NONE;
	}

	if (t < 1.0f)
	{
		WPolySoup::sTriangle& tri = m_pBaseModel->tri[ball.m_meshIndex];
		WVector normal;
		if (tri.bEdge)
		{
			WVector point = intersection::closestPointOnTriangle(
				ball.m_pos + dir * t, tri.v[0], tri.v[1], tri.v[2]);
			normal = ball.m_pos + dir * t - point;
			normal.Normalize();
		}
		else
		{
			normal = m_pBaseModel->tri[ball.m_meshIndex].plane.normal;
			if (tri.bDoubleSide && normal * dir > 0.0f)
			{
				WVector& n = normal;
				n.x = -n.x;
				n.y = -n.y;
				n.z = -n.z;
			}
		}

		const WVector& n = normal;
		ball.m_attrIndex = tri.propIndex;
		ball.m_pos += dir * t;
		BounceResponse(normal);

		CVectorSlideTestContainer* pTest =
			(CVectorSlideTestContainer*)CContentsDoc::Instance()->GetContainer(
				(localContentType_t)162);
		if (pTest && pTest->IsNewTypeVectorSlide())
		{
			WVector up(0.0f, 1.0f, 0.0f);
			if (normal * up < cos(45.0f * g_DEGTORAD) && ball.m_groundType == 8)
				ball.m_vel.y = 0.0f;
		}

		if (!PLAYER(GOLFDOC()->m_currentPlayer)->bunker)
			pre->effect = cos(45.0f * g_DEGTORAD) < n.y ? 1 : 2;
		else
			pre->effect = 3;

		pre->normal = normal;
		pre->event = 1;

		if (m_pPolySoup->GetProperty(ball.m_attrIndex).ground == 8)
		{
			const float& speed =
				min(500.0f,
					m_pPolySoup->GetProperty(GolfBall().m_attrIndex).max[1]) *
				0.01f;

			CVectorSlideTestContainer* pSlide =
				(CVectorSlideTestContainer*)CContentsDoc::Instance()
					->GetContainer((localContentType_t)162);
			if (pSlide)
			{
				int count = pSlide->GetLimitVectorSlideCount();
				float height = (float)pSlide->GetBoundHeight();
				float limit = (float)pSlide->GetLimitSpeed();
				MakeVectorSlide(speed, limit, height, count);
			}
		}
		else
		{
			strcpy(g_colTex[g_colNum],
				FindTexture(
					m_pBaseModel->tri[ball.m_meshIndex].texHandle & 0x7ff));
			g_colNum++;
			if (g_colNum >= 16)
				g_colNum = 15;

			m_colData.frame = ball.m_renderNum;
			m_colData.objIndex = -1;
			m_colData.triIndex = ball.m_meshIndex;
			m_colData.type = 0;
			m_colData.pos = ball.m_pos;
			m_colData.vel = ball.m_vel;
			m_colList.push_back(m_colData);
		}
		return;
	}

	WPlane plane;
	t = IntersectObj(ball.m_pos, dir, &plane);

	if (t < 1.0f && !bNoObj)
	{
		if (m_colData.objIndex >= 0 &&
			m_pPolySoup->GetProperty(ball.m_attrIndex).ground == 12)
		{
			WPolySoup::sModel* model = m_pPolySoup->m_model[m_colData.objIndex];

			ball.m_pos += dir;

			pre->event = 5;

			float a, b;
			if (sscanf(model->option, "%f %f", &a, &b) < 2)
				return;

			float velZ = ball.m_vel.z;
			ball.m_vel = model->mat.ya * (fabs(ball.m_vel.y) + a);
			ball.m_vel += model->mat.za * (fabs(velZ) + b);
			return;
		}

		if (m_colData.objIndex >= 0 &&
			m_pPolySoup->GetProperty(ball.m_attrIndex).ground == 13)
		{
			WPolySoup::sModel* model = m_pPolySoup->m_model[m_colData.objIndex];

			ball.m_pos += dir;

			pre->event = 5;

			float a, b;
			if (sscanf(model->option, "%f %f", &a, &b) < 2)
				return;

			float velY = ball.m_vel.y;
			float velZ = ball.m_vel.z;

			a *= 0.33333334f;
			b += a;
			if (a < 0.1f)
				a = 1.0f;

			ball.m_vel = model->mat.ya * (fabs(velY) + a);
			ball.m_vel += model->mat.za * (fabs(velZ) + b);
			return;
		}

		sColResponse response(GolfBall(), m_colData.pos, m_colData.vel);
		ObjResponse(dir, t, plane.normal, response);
		CommitResponse(GolfBall(), response, GolfBall().m_renderNum);

		pre->effect = 2;
		pre->event = 2;

		ball.m_curve = 0.0f;
		ball.m_spin = 0.0f;
		ball.m_special = 0;

		CVectorSlideTestContainer* pTest = NULL;
		CContentsDoc::Instance()->GetContainer((localContentType_t)162, pTest);
		if (pTest && pTest->IsNewTypeVectorSlide())
		{
			if (plane.normal * WVector(0.0f, 1.0f, 0.0f) <
					cos(45.0f * g_DEGTORAD) &&
				ball.m_groundType == 8)
				ball.m_vel.y = 0.0f;
		}
	}
	else
	{
		ball.m_pos += dir;
		if (ball.m_colTriIndex != -1)
		{
			WVector v[3];
			for (int i = 0; i != 3; i++)
			{
				v[i] = m_pPolySoup->m_model[ball.m_colObjIndex]
						   ->tri[ball.m_colTriIndex]
						   .v[i];
				v[i].y = ball.m_pos.y;
			}

			if (!PointInTriangle(ball.m_pos, v[0], v[1], v[2]))
			{
				ball.m_colObjIndex = -1;
				ball.m_colTriIndex = -1;
			}
		}
	}
}

float CQuadTree::CheckCollision(const WVector& pos, const WVector& dir,
	int* index, bool bDoubleSide)
{
	WVector v[3];

	if (IsLocalContent(S3_ROOKIE_CHANNEL) && Doc()->m_golfGame.gameType != 11 &&
		Doc()->m_golfGame.gameType != 12)
	{
		if (Doc()->m_curChannel.Type & 0x800)
			m_holeCupRadius = GolfBall().GetRadius() * 6.0f;
		else
			m_holeCupRadius = GolfBall().GetRadius() * 3.0f;
	}

	CGolfBall& ball = GolfBall();

	Waabb box;
	box.min = pos - WVector(Abs(dir.x), Abs(dir.y), Abs(dir.z)) +
		WVector::ONE * ball.GetRadius();
	box.max = pos + WVector(Abs(dir.x), Abs(dir.y), Abs(dir.z)) +
		WVector::ONE * ball.GetRadius();
	GetMeshList(box);

	float minT = 1.0f;
	if (index)
		*index = -1;

	for (int i = 0; i < m_meshNum; i++)
	{
		int tri = m_meshIndex[i];

		for (int j = 0; j < 3; j++)
			v[j] = m_pBaseModel->tri[tri].v[j];

		m_pBaseModel->tri[tri].bEdge = false;

		WPlane plane;
		plane = m_pBaseModel->tri[tri].plane;
		if (m_pBaseModel->tri[tri].bDoubleSide && plane * pos < 0.0f)
		{
			WVector& n = plane.normal;
			n.x = -n.x;
			n.y = -n.y;
			n.z = -n.z;
			plane.dis *= -1.0f;
		}

		WVector from;
		from = pos - plane.normal * ball.GetRadius();
		bool bDouble =
			m_pPolySoup->GetProperty(m_pBaseModel->tri[tri].propIndex).ground ==
			2;
		float t = intersection::IntersectPlane(from, dir, plane, bDouble);

		if (t < minT)
		{
			WVector hit;
			hit = from + dir * t;
			if (!bDoubleSide)
			{
				if ((ball.m_special & 0x20) && ball.m_cobraFrame < 0 &&
					plane.normal.y > (float)cos(20.0 * g_DEGTORAD))
				{
					m_bUseLowest = true;
					continue;
				}

				if ((ball.m_special & 0x40) && ball.m_spikeFrame < 0 &&
					plane.normal.y > (float)cos(20.0 * g_DEGTORAD))
				{
					m_bUseLowest = true;
					continue;
				}

				WVector diff;
				WVector cur = pos + dir * t;
				diff = WVector(GOLFDOC()->GetHoleData().pin) - cur;

				if (sqrtf(diff.z * diff.z + diff.x * diff.x) < m_holeCupRadius)
					continue;
			}

			if (intersection::PointInTriangle(hit, v[0], v[1], v[2]))
			{
				minT = t;
				if (index)
					*index = tri;
			}
			else
			{
				WVector point =
					intersection::closestPointOnTriangle(hit, v[0], v[1], v[2]);
				float t2 = intersection::intersectPolygon(point, pos, dir,
					GolfBall().GetRadius());

				if (t2 < minT)
				{
					minT = t2;
					m_pBaseModel->tri[tri].bEdge = true;
					if (index)
						*index = tri;
				}
			}
		}
	}

	return minT;
}

void CQuadTree::CalcGreenPlane()
{
	if (WisEqual(m_greenArea.max, m_greenArea.min, g_EPSILON))
		return;

	WVector dir = PLAYER(GOLFDOC()->m_currentPlayer)->pos - m_greenCenter;

	m_greenPlane.normal = WVector::UNIT_POS_Y +
		WVector(dir.x / (m_greenArea.max.x - m_greenArea.min.x), 0.0f,
			dir.z / (m_greenArea.max.z - m_greenArea.min.z)) *
			m_greenFactor;
	m_greenPlane.normal.Normalize();

	m_greenPlane.d = m_greenPlane.normal * -1.0f * m_greenCenter;
}

float CQuadTree::GetGreenPos(const WVector& pos)
{
	if (!WisEqual(m_greenArea.max, m_greenArea.min, g_EPSILON) &&
		GetGroundType(pos, true, NULL) == 2)
	{
		return (m_greenPlane.a * pos.x + m_greenPlane.c * pos.z +
				   m_greenPlane.d) /
			m_greenPlane.b * -1.0f;
	}

	return pos.y;
}

WVector CQuadTree::GetFriction()
{
	WVector normal;

	GolfBall().m_attrIndex =
		m_pBaseModel->tri[GolfBall().m_meshIndex].propIndex;

	if (!WisEqual(m_greenArea.max, m_greenArea.min, g_EPSILON) &&
		GetGroundType(GolfBall().m_pos, true, NULL) == 2)
	{
		normal = m_greenPlane.normal;
	}
	else
	{
		normal = m_pBaseModel->tri[GolfBall().m_meshIndex].plane.normal;

		if (normal.y < 0.0f &&
			m_pBaseModel->tri[GolfBall().m_meshIndex].bDoubleSide)
		{
			WVector& n = normal;
			n.x = -n.x;
			n.y = -n.y;
			n.z = -n.z;
		}
	}

	const WVector& up = WVector::UNIT_POS_Y;
	float cosine = up * normal;

	float clamped = Between(-1.0f, cosine, 1.0f);

	WVector slope = normal * cosine + WVector::UNIT_NEG_Y;
	slope.Normalize();
	slope *= sinf(acosf(clamped)) * m_slopeFactor * GolfBall().GetMass() *
		GetGravity();
	WVector force(0.0f, 0.0f, 0.0f);
	force += slope;

	WVector dir = -1.0f * GolfBall().m_vel;
	dir.Normalize();
	float roll = GetRollCoeff(GolfBall().m_attrIndex);
	dir *= GetWeatherRollCoeff(GolfBall().m_attrIndex) * m_rollFactor *
		GolfBall().GetMass() * GetGravity() * roll * cosine;

	force += dir;

	if (GolfBall().m_vel.Magnitude() <
		dir.Magnitude() / GolfBall().GetMass() * 0.02f)
		GolfBall().m_state = CGolfBall::BALL_STOP;

	return force;
}

void CQuadTree::SpinProcess(float dt)
{
	CGolfBall& ball = GolfBall();

	ball.m_rot.y += dt * ball.m_initSpin * 62.831856f;
	if (ball.m_renderNum == ball.m_afterBurnerFrame + 50)
	{
		ball.m_vel += ball.m_afterBurner;

		ball.m_rollFrame = ball.m_renderNum;
		ball.m_state = CGolfBall::BALL_ROLL;
		RollProcess(dt);
	}
}

float CQuadTree::IntersectRollingBallGround(const WVector& pos,
	const WVector& dir, const float& radius, int& index, WVector& normal,
	bool bRolling, bool bCheckWall, bool bDoubleSide)
{
	GetMeshList(pos, dir);

	float t = 1.0f;
	for (int i = 0; i < m_meshNum; i++)
	{
		const WPolySoup::sTriangle* tri = &m_pBaseModel->tri[m_meshIndex[i]];
		WPlane plane;
		plane = tri->plane;
		if (tri->bDoubleSide && plane * pos < 0.0f)
		{
			WVector& n = plane.normal;
			n.x = -n.x;
			n.y = -n.y;
			n.z = -n.z;
			plane.d *= -1.0f;
		}

		if (bRolling && bCheckWall &&
			(Abs(plane.y) > 0.5f || plane.normal * dir > 0.0f ||
				(tri->v[0].y < pos.y && tri->v[1].y < pos.y &&
					tri->v[2].y < pos.y)))
			continue;

		normal = plane.normal;

		WVector from = pos - plane.normal * radius;
		t = intersection::IntersectPlane(from, dir, plane, bDoubleSide);

		if (t < 1.0f)
		{
			if (intersection::PointInTriangle(from + dir * t, tri->v[0],
					tri->v[1], tri->v[2]))
			{
				index = i;
				break;
			}

			WVector closest = intersection::closestPointOnTriangle(
				from + dir * t, tri->v[0], tri->v[1], tri->v[2]);
			t = intersectPolygon(closest, pos, dir);

			if (t < 1.0f)
			{
				index = i;
				break;
			}
		}
	}

	return t;
}

void CQuadTree::RollingBallGroundResponse(CGolfBall& ball, float t,
	const WVector& dir, int index, const WVector& normal)
{
	if (IsLocalContent(S4_BALL_SLIDE))
	{
		ball.m_spin = 0.0f;
		ball.m_curve = 0.0f;
		ball.m_pos += dir * t;
		ball.m_pos.y = GetHighestGround(ball.m_pos, &ball.m_meshIndex);
		ball.m_pos.y += ball.GetRadius();
		ball.m_attrIndex = m_pBaseModel->tri[index].propIndex;

		ReflectVector(normal, ball.m_vel);
		float bounce = GetBounceCoeff(ball.m_attrIndex) *
			GetWeatherBounceCoeff(ball.m_attrIndex);
		ball.m_vel *= bounce;
		ball.PreDataToWrite(ball.m_renderNum)->normal = normal;
	}
	else
	{
		ball.m_spin = 0.0f;
		ball.m_curve = 0.0f;

		ball.m_pos += dir * t;

		ball.m_pos.y += ball.GetRadius();
		ball.m_attrIndex = m_pBaseModel->tri[index].propIndex;
		ball.m_vel *= GetBounceCoeff(ball.m_attrIndex) *
			GetWeatherBounceCoeff(ball.m_attrIndex) * -1.0f;

		strcpy(ball.PreDataToWrite(ball.m_renderNum)->sound,
			"\xc3\xe6\xb5\xb9_wood");
		ball.PreDataToWrite(ball.m_renderNum)->normal = normal;
	}
}

void CQuadTree::RollProcess(float dt)
{
	if (IsLocalContent(S3_ROOKIE_CHANNEL) && GetGameType() != 11 &&
		GetGameType() != 12)
	{
		if (Doc()->m_curChannel.Type & 0x800)
			m_holeCupRadius = GolfBall().GetRadius() * 6.0f;
		else
			m_holeCupRadius = GolfBall().GetRadius() * 3.0f;
	}

	CGolfBall& ball = GolfBall();

	ball.m_meshIndex = GetMeshIndex(ball.m_pos, false);

	WVector& vel = ball.m_vel;

	if (ball.m_meshIndex == -1)
	{
		const float step = 0.02f;
		vel = ball.m_vel * -1.0f * (1.0f / vel.Magnitude()) * step;
		ball.m_pos += vel * step;
		if (vel.Magnitude() < step / ball.GetMass())
		{
			vel.x = vel.y = vel.z = 0.0f;
			ball.m_bOB = true;
			ball.m_state = CGolfBall::BALL_STOP;
		}
	}
	else
	{
		float mass = ball.GetMass();
		vel += (1.0f / mass) * GetFriction() * dt;

		WVector move;
		if (ball.m_state == CGolfBall::BALL_STOP)
		{
			move = ball.m_pos - GetHoleCupPos();
			move.y = 0.0f;

			if (sqrtf(move.x * move.x + move.z * move.z) < m_holeCupRadius)
			{
				ball.m_pos = GetHoleCupPos();
				ball.m_pos.y = m_holeBottom + ball.GetRadius();
				HoleIn();
			}
			else
			{
				vel.x = vel.y = vel.z = 0.0f;

				Waabb aabb;
				if (IsFallenIntoWater(ball.m_pos, &aabb, false))
				{
					ball.m_bOB = true;
					SetPenaltyPos(aabb);
				}
				else if (ball.m_special & 1)
					MakeAfterBurner();
			}
		}
		else
		{
			move = dt * ball.m_vel;
			WPlane plane;
			float t = IntersectObj(ball.m_pos, move, &plane);
			float tOffset = 0.0f;

			sColResponse response(GolfBall(), m_colData.pos, m_colData.vel);
			WVector pos;
			if (t < 1.0f)
			{
				ObjResponse(move, t, plane.normal, response);
				if (response.bRadiusOffset)
				{
					tOffset = response.ball.GetRadius() / move.Magnitude() + t;
				}

				pos = response.ball.pos;
				move = pos - ball.m_pos;
			}
			else
			{
				pos = move + ball.m_pos;
			}

			float height = GetHighestGround(pos, &ball.m_meshIndex);
			if (ball.m_meshIndex == -1)
			{
				const float bottom = -100000.0f;
				pos.y = bottom;
			}
			else
			{
				pos.y = height + ball.GetRadius();

				const float rollScale = 0.3f;
				ball.m_rot.y -= (pos - ball.m_pos).Magnitude() /
					ball.GetRadius() * rollScale;
			}

			int index = -1;
			WVector normal;
			float tGround = IntersectRollingBallGround(ball.m_pos, move,
				GolfBall().GetRadius(), index, normal, true, true, false);
			if (tGround < 1.0f)
			{
				if (response.bRadiusOffset && t / tOffset < tGround)
				{
					response.ball.pos = GolfBall().m_pos;
					CommitResponse(GolfBall(), response,
						GolfBall().m_renderNum);
				}

				RollingBallGroundResponse(ball, tGround, move, index, normal);
				return;
			}

			if (t < 1.0f)
			{
				CommitResponse(GolfBall(), response, GolfBall().m_renderNum);
			}

			const float half = 0.5f;
			if ((ball.m_pos + move).y > pos.y + ball.GetRadius() * half)
			{
				ball.m_pos += move;
				ball.m_spin = 0.0f;
				ball.m_curve = 0.0f;
				ball.m_state = CGolfBall::BALL_BOUNCE;
				ball.m_meshIndex = -1;
				if (ball.m_bounceFrame < 0)
					ball.m_bounceFrame = ball.m_renderNum;
			}
			else
			{
				if (ball.PreData(ball.m_renderNum - 1)->state ==
					CGolfBall::BALL_ROLL)
				{
					move = pos - ball.PreData(ball.m_renderNum - 1)->pos;

					if (WisZero(sqrtf(move.x * move.x + move.z * move.z)))
					{
						vel.x = vel.y = vel.z = 0.0f;
						ball.m_state = CGolfBall::BALL_STOP;
						if (ball.m_special & 1)
							MakeAfterBurner();
					}
					else
					{
						float slope = sin(60.0f * g_DEGTORAD);
						if (Abs(move.y) / move.Magnitude() < slope)
							ball.m_vel.y = sqrtf(ball.m_vel.x * ball.m_vel.x +
											   ball.m_vel.z * ball.m_vel.z) /
								sqrtf(move.x * move.x + move.z * move.z) *
								move.y;
					}
				}

				ball.m_pos = pos;
			}

			const float maxCount = 75.0f;
			if (!(m_holeDist.z > m_holeCupRadius + ball.GetRadius()) &&
				(float)m_holeCount < maxCount)
			{
				m_holeCount++;
				move = ball.m_pos - GetHoleCupPos();
				if (WVector(move.x, 0.0f, move.z) * ball.m_vel > 0.0f)
				{
					WVector dir = GetHoleCupPos() - ball.m_pos;
					dir.y = 0.0f;
					dir.Normalize();
					dir *= 80.0f;
					vel += dir * dt;
				}
			}
		}
	}
}

void CQuadTree::BounceResponse(const WVector& normal)
{
	CGolfBall& ball = GolfBall();
	CGolfBall::sExpected* expected = ball.PreDataToWrite(ball.m_renderNum);

	const char* sound = GetBounceSound(ball.m_attrIndex);

	if (sound[0])
	{
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
				0x14000005 ||
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
				0x14000080)
		{
			strcpy(expected->sound, "wb_bounce");
		}
		else if (Doc()
					 ->m_userInfo[GOLFDOC()->m_currentPlayer]
					 .userEquip.tidBall == 0x1400001d)
		{
			strcpy(expected->sound, "HVA_bnound");
		}
		else if (Doc()
					 ->m_userInfo[GOLFDOC()->m_currentPlayer]
					 .userEquip.tidBall == 0x1400001c)
		{
			strcpy(expected->sound, "nagashima_Bounce");
		}
		else if (Doc()
					 ->m_userInfo[GOLFDOC()->m_currentPlayer]
					 .userEquip.tidBall == 0x1400000b)
		{
			strcpy(expected->sound, "HG_Bounce_1");
		}
		else
		{
			const char* texName = GetTextureName(ball.m_pos);

			if (texName &&
				(!stricmp(texName, "icegrass07_03.dds") ||
					!stricmp(texName, "igrass03.dds") ||
					!stricmp(texName, "igrass06.dds")))
			{
				const char* iceSound[4] = { "ball_ice\xc0\xfd\xba\xae"
											"3",
					"ball_ice\xc0\xfd\xba\xae"
					"3-1",
					"ball_ice\xc0\xfd\xba\xae"
					"3-2",
					"ball_ice\xc0\xfd\xba\xae"
					"3-3" };

				strcpy(expected->sound, iceSound[rand() % 4]);
			}
			else
			{
				strcpy(expected->sound, sound);
			}
		}
	}

	bool bBounce = true;
	if (ball.m_bounceFrame < 0)
	{
		ball.m_bounceFrame = ball.m_renderNum;
		ball.m_curve = 0.0f;
		if (normal.y > (float)cos(20.0 * 0.017453292f))
		{
			if (ball.m_special & 0x10 || StuckInBunker(normal))
				bBounce = false;
			else if (ball.m_special & 0x40)
				bBounce = false;
		}
		else
		{
			if ((ball.m_special & 0x40) && ball.m_topFrame >= 0 &&
				ball.m_renderNum > ball.m_topFrame)
				ball.m_vel *= 0.5f;
		}

		ball.m_special &= ~0x70;

		if (!bBounce)
		{
			if (ball.m_special & 3)
			{
				MakeAfterBurner();
				return;
			}

			if (GetGroundType(ball.m_pos, true, NULL) == 3)
			{
				if (ball.m_vel.Magnitude() > 150.0f ||
					(ball.m_initSpecial & 0x50))
					PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 2;
				else
					PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 1;
			}
			else
				PLAYER(GOLFDOC()->m_currentPlayer)->bunker = 0;

			ball.m_state = CGolfBall::BALL_STOP;
			ball.m_vel.x = ball.m_vel.y = ball.m_vel.z = 0.0f;
			return;
		}
	}

	ball.m_spin = Min(0.0f, ball.m_spin);

	WVector& vel = ball.m_vel;
	const WVector& n = normal;
	WVector perp = (ball.m_vel * n) * -1.0f * n;
	WVector tangent = vel + perp;
	float spin = ball.m_initSpin * 0.1f + 0.3f;

	vel = tangent *
			(1.0f -
				(1.0f -
					GetWeatherBounceCoeff(ball.m_attrIndex) * m_bounceFactor *
						GetBounceCoeff(ball.m_attrIndex)) *
					spin) +
		perp *
			(GetWeatherBounceCoeff(ball.m_attrIndex) * m_bounceFactor *
				GetBounceCoeff(ball.m_attrIndex));
}

bool CQuadTree::StuckInBunker(const WVector& normal)
{
	float cosine =
		GolfBall().m_vel * -1.0f * normal / GolfBall().m_vel.Magnitude();

	if (GetGroundType(GolfBall().m_pos, true, NULL) == 3 &&
		GolfBall().m_vel.Magnitude() > 50.0f &&
		cosine > (float)cos(45.0 * 0.017453292f))
		return true;

	if (GetGroundType(GolfBall().m_pos, true, NULL) == 11 &&
		GolfBall().m_vel.Magnitude() > 50.0f &&
		cosine > (float)cos(45.0 * 0.017453292f))
		return true;

	if (GetGroundType(GolfBall().m_pos, true, NULL) == 14 &&
		GolfBall().m_vel.Magnitude() > 50.0f &&
		cosine > (float)cos(45.0 * 0.017453292f))
		return true;

	return false;
}

void CQuadTree::ObjResponse(const WVector& dir, float t, const WVector& normal,
	sColResponse& response)
{
	response.normal = normal;

	const char* sound =
		m_pPolySoup->GetProperty(response.ball.GetAttrIndex()).boundSound;

	if (sound[0])
	{
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
			0x14000004)
			sprintf(response.sound, "t_Ex4-%d", rand() % 4 + 1);
		else
			strcpy(response.sound, sound);
	}

	if (m_pPolySoup->GetProperty(response.ball.GetAttrIndex()).pass & 1)
	{
		if (!WisEqual(dir, WVector::ZERO, g_EPSILON))
		{
			response.bRadiusOffset = true;

			response.ball.pos +=
				dir * (response.ball.GetRadius() / dir.Magnitude() + t);
			response.ball.vel *= GetBounceCoeff(response.ball.GetAttrIndex());
			response.pos = response.ball.pos;
			response.vel = response.ball.vel;
			response.bCommit = true;
		}
	}
	else if (response.ball.state == CGolfBall::BALL_ROLL)
	{
		response.ball.pos += dir * t;
		ReflectVector(normal, response.ball.vel);
		response.ball.vel *= GetBounceCoeff(response.ball.GetAttrIndex());

		response.pos = response.ball.pos;
		response.vel = response.ball.vel;
		response.bCommit = true;
	}
	else
	{
		if (response.ball.colTriIndex == -1)
		{
			const WVector& n = normal;
			WVector vn = (n * response.ball.vel) * -1.0f * n;
			WVector vt = response.ball.vel + vn;
			float spin = response.ball.GetInitSpin();

			response.ball.vel = vt *
					(1.0f -
						(spin * 0.1f + 0.3f) *
							(1.0f -
								GetBounceCoeff(response.ball.GetAttrIndex()))) +
				vn * GetBounceCoeff(response.ball.GetAttrIndex());

			if (response.ball.GetSpecial() & 0x10)
				response.ball.vel *= 0.25f;

			if ((response.ball.GetSpecial() & 0x40) &&
				response.ball.GetTopFrame() >= 0 &&
				response.ball.GetRenderNum() > response.ball.GetTopFrame())
				response.ball.vel *= 0.25f;

			response.ball.pos += dir * t;

			response.pos = response.ball.pos;
			response.vel = response.ball.vel;

			response.bCommit = true;
		}
		else
		{
			response.ball.colObjIndex = m_colList.back().objIndex;
			response.ball.colTriIndex = m_colList.back().triIndex;

			WPolySoup::sModel* model =
				m_pPolySoup->m_model[response.ball.colObjIndex];
			const WPolySoup::sTriangle& tri =
				model->tri[response.ball.colTriIndex];

			if (Wabs(tri.plane.b) < g_EPSILON)
			{
				response.ball.vel.Reset();
				response.ball.state = CGolfBall::BALL_STOP;
			}
			else
			{
				response.ball.pos.y =
					(tri.plane.a * response.ball.pos.x +
						tri.plane.c * response.ball.pos.z + tri.plane.d) /
					tri.plane.b * -1.0f;
				response.ball.pos.y += response.ball.GetRadius();
			}
		}

		if ((response.ball.vel - WVector(0.0f, GetGravity() * 0.02f, 0.0f)) *
				normal <
			0.0f)
		{
			if (normal.y > cos(20.0f * g_DEGTORAD))
			{
				response.ball.vel.Reset();
				response.ball.state = CGolfBall::BALL_STOP;
			}
			else
			{
				if (response.ball.vel.y > 0.0f)
				{
					response.ball.vel.x = -response.ball.vel.x;
					response.ball.vel.y = -response.ball.vel.y;
					response.ball.vel.z = -response.ball.vel.z;
				}

				response.ball.colObjIndex = m_colList.back().objIndex;
				response.ball.colTriIndex = m_colList.back().triIndex;
			}
		}
	}
}

CQuadTree::sColResponse::sColResponse(const CGolfBall& ball, const WVector& pos,
	const WVector& vel)
	: ball(ball), pos(pos), vel(vel), bRadiusOffset(false), bCommit(false)
{
	sound[0] = 0;
}

CQuadTree::sColResponse::GolfBallClone::GolfBallClone(const CGolfBall& ball)
	: pos(ball.m_pos),
	  vel(ball.m_vel),
	  colObjIndex(ball.m_colObjIndex),
	  colTriIndex(ball.m_colTriIndex),
	  state(ball.m_state),
	  pBall(&ball)
{
}

void CQuadTree::CommitResponse(CGolfBall& ball, const sColResponse& response,
	int sample)
{
	ball.m_pos = response.ball.pos;
	ball.m_vel = response.ball.vel;
	ball.m_colObjIndex = response.ball.colObjIndex;
	ball.m_colTriIndex = response.ball.colTriIndex;
	ball.m_state = (CGolfBall::eBallState)response.ball.state;

	CGolfBall::sExpected* expected = ball.PreDataToWrite(sample);
	expected->normal = response.normal;
	strcpy(expected->sound, response.sound);

	m_colData.pos = response.pos;
	m_colData.vel = response.vel;
	if (response.bCommit)
	{
		m_colList.push_back(m_colData);
	}
}

void CQuadTree::Display()
{
	WTVertex v[3];
	WTVertex* vl[4] = { &v[0], &v[1], &v[2], NULL };

	if (m_paintIndex >= 0)
	{
		for (int i = 0; i < 3; ++i)
		{
			v[i].SetPosition(m_pBaseModel->tri[m_paintIndex].v[i]);
			v[i].diffuse = 0xffffffff;
		}

		g_view->DrawPolygonFan(vl, 0x4300000, 0x400, false);
	}

	if (COption::Instance()->dIsVtxClrTestEnabled())
	{
		TextOut(g_view, 470.0f, 60.0f,
			m_vtxColorMode == 0 ? "<<BASE COLOR MODE>>" : "<<OBJ COLOR MODE>>");
		TextOut(g_view, 470.0f, 72.0f,
			MakeStr("Base : %.0f %.0f %.0f", m_baseVtxColor.x, m_baseVtxColor.y,
				m_baseVtxColor.z));
		TextOut(g_view, 470.0f, 84.0f,
			MakeStr("Obj  : %.0f %.0f %.0f", m_objVtxColor.x, m_objVtxColor.y,
				m_objVtxColor.z));
	}
}

WVector CQuadTree::closestPointOnTriangle(const WVector& a, const WVector& b,
	const WVector& c, const WVector& p) const
{
	return intersection::closestPointOnTriangle(a, b, c, p);
}

WVector CQuadTree::closestPointOnLine(const WVector& a, const WVector& b,
	const WVector& p) const
{
	return intersection::closestPointOnLine(a, b, p);
}

float CQuadTree::intersectPolygon(const WVector& p, const WVector& from,
	const WVector& dir) const
{
	return intersection::intersectPolygon(p, from, dir, GolfBall().GetRadius());
}

void CQuadTree::HoleProcess(float dt)
{
	if (IsLocalContent(S3_ROOKIE_CHANNEL) && Doc()->m_golfGame.gameType != 11 &&
		Doc()->m_golfGame.gameType != 12)
	{
		if (Doc()->m_curChannel.Type & 0x800)
			m_holeCupRadius = GolfBall().GetRadius() * 6.0f;
		else
			m_holeCupRadius = GolfBall().GetRadius() * 3.0f;
	}

	CGolfBall& ball = GolfBall();
	CGolfBall::sExpected* expected = ball.PreDataToWrite(ball.m_renderNum);

	ball.m_vel.y -= dt * m_gravityModifier * 34.295296f;
	WVector step;
	step = ball.m_vel * dt;

	float height = ball.m_pos.y - ball.GetRadius() - m_holeBottom;
	float nextHeight = step.y + height;

	float t[3];
	if (height * nextHeight < 0.0f && ball.m_vel.y < 0.0f &&
		!(Wabs(height) < g_EPSILON))
		t[0] = height / (height - nextHeight);
	else
		t[0] = 1.0f;

	step *= t[0];

	t[2] = CheckCollision(ball.m_pos, step, &ball.m_meshIndex, false);
	if (ball.m_meshIndex >= 0)
		ball.m_attrIndex = m_pBaseModel->tri[ball.m_meshIndex].propIndex;

	WVector diff;
	diff = ball.m_pos - GetHoleCupPos();
	diff.y = 0.0f;
	float dist[2];
	float cupR = m_holeCupRadius - ball.GetRadius();
	dist[0] = sqrtf((float)(diff * diff));
	dist[0] -= cupR;
	diff = ball.m_pos + step - GetHoleCupPos();
	diff.y = 0.0f;
	dist[1] = diff.Magnitude() - (m_holeCupRadius - ball.GetRadius());

	if (ball.PreData(ball.m_renderNum - 1)->state == CGolfBall::BALL_BOUNCE &&
		dist[0] > 0.0f)
	{
		if (dist[1] > ball.GetRadius())
		{
			ball.m_pos += step * t[2];
			if (t[2] < 1.0f)
				BounceResponse(GetMeshNormal(ball.m_meshIndex));
		}
		else
		{
			ball.m_pos += step;

			if (dist[1] > 0.0f)
			{
				WVector ground = GetGroundPoint(ball.m_pos, false, NULL, false);

				float y = sqrtf(ball.GetRadius() * ball.GetRadius() -
							  (ball.GetRadius() - dist[1]) *
								  (ball.GetRadius() - dist[1])) +
					ground.y;

				WVector dir = ball.m_pos - GetHoleCupPos();
				dir.y = 0.0f;
				dir.Normalize();
				dir *= m_holeCupRadius;
				dir += GetHoleCupPos();
				WVector normal = (ball.m_pos - dir).Normalize();

				if (ball.m_pos.y < y)
				{
					ball.m_special &= 0xffffff8c;
					ball.m_curve = ball.m_spin = 0.0f;
					ball.m_pos.y = y;
					ball.m_vel += normal * (Abs(normal * ball.m_vel) * 1.3f);

					expected->effect = 0;
					expected->event = 3;
					expected->normal = normal;
					strcpy(expected->sound, "\xc3\xe6\xb5\xb9_pole");
					return;
				}
			}
		}
	}
	else if (dist[1] < 0.0f)
	{
		ball.m_pos += step;

		if (height * nextHeight < 0.0f && ball.m_vel.y < 0.0f)
			HoleIn();
	}
	else
	{
		t[1] = IntersectPoint(step);
		if (t[1] < t[2])
		{
			ball.m_pos += step * t[2];

			if (t[2] < 1.0f)
				BounceResponse(GetMeshNormal(ball.m_meshIndex));
		}
		else if (t[1] < 1.0f)
		{
			ball.m_special &= 0xffffff8c;
			ball.m_pos += step * t[1];

			WVector dir = GetHoleCupPos() - ball.m_pos;
			dir.y = 0.0f;
			dir.Normalize();

			ball.m_vel = dir * (ball.m_vel.Magnitude() * 0.3f);

			expected->effect = 0;
			expected->event = 3;
			expected->normal = dir;
			strcpy(expected->sound, "\xc3\xe6\xb5\xb9_pole");
		}
		else
		{
			ball.m_pos += step;
		}
	}

	diff = GetHoleCupPos() - ball.m_pos;
	diff.y = 0.0f;

	float holeDist = diff.Magnitude();
	float y;
	if (holeDist > m_holeCupRadius - ball.GetRadius())
	{
		if (ball.m_rollFrame < 0)
		{
			if (ball.m_pos.y < GetHoleCupPos().y)
			{
				if (t[0] < 1.0f)
				{
					ball.m_pos = GetHoleCupPos();
					ball.m_pos.y = m_holeBottom + ball.GetRadius();
					HoleIn();
					return;
				}

				y = ball.m_pos.y;
				ball.m_pos = GetHoleCupPos() -
					(m_holeCupRadius - ball.GetRadius()) * diff.Normalize();
				ball.m_pos.y = y;
				ball.m_vel *= -0.3f;
				ball.m_vel.y *= -1.0f;
				ball.m_special &= 0xffffff8c;
				return;
			}

			if (GetHoleCupPos().y + ball.GetRadius() > ball.m_pos.y)
			{
				if (holeDist > m_holeCupRadius)
				{
					WVector ground =
						GetGroundPoint(ball.m_pos, false, NULL, false);

					float y = ground.y + ball.GetRadius();
					if (y > ball.m_pos.y)
					{
						ball.m_special &= 0xffffff8f;
						ball.m_pos.y = y;
						ball.m_vel.y =
							GetBounceCoeff(m_pBaseModel
									->tri[GetMeshIndex(ball.m_pos, false)]
									.propIndex) *
							ball.m_vel.y * -1.0f;

						expected->effect = 1;
						expected->event = 1;
						expected->normal = WVector::UNIT_POS_Y;

						const char* sound = GetBounceSound(
							m_pBaseModel->tri[GetMeshIndex(ball.m_pos, false)]
								.propIndex);
						if (sound[0])
							strcpy(expected->sound, sound);
					}
				}
				else
				{
					WVector ground =
						GetGroundPoint(ball.m_pos, false, NULL, false);

					float r = ball.GetRadius();
					float y = sqrtf(r * r -
								  (m_holeCupRadius - holeDist) *
									  (m_holeCupRadius - holeDist)) +
						ground.y;
					if (ball.m_pos.y < y)
					{
						WVector dir = ball.m_vel;
						dir.Normalize();
						float dirY = -dir.y;
						dir.y = 0.0f;

						WVector cupDir = GetHoleCupPos() - ball.m_pos;
						cupDir.y = 0.0f;
						cupDir.Normalize();

						dir = cupDir + dir;
						dir.Normalize();

						dir = cupDir + dir;
						dir.Normalize();

						dir.y = dirY;
						dir.Normalize();

						ball.m_pos.y = y;
						ball.m_vel = dir * (ball.m_vel.Magnitude() * 0.3f);
						ball.m_special &= 0xffffff8c;
					}
				}
			}
		}
		else
		{
			y = ball.m_pos.y;
			ball.m_pos = GetHoleCupPos() -
				(m_holeCupRadius - ball.GetRadius()) * diff.Normalize();
			ball.m_pos.y = y;

			float speed = ball.m_vel.Magnitude();
			ball.m_vel = (speed * 0.3f) * diff.Normalize();
		}
	}
}

float CQuadTree::IntersectPoint(WVector dir)
{
	if (IsLocalContent(S3_ROOKIE_CHANNEL) && Doc()->m_golfGame.gameType != 11 &&
		Doc()->m_golfGame.gameType != 12)
	{
		if (Doc()->m_curChannel.Type & 0x800)
			m_holeCupRadius = GolfBall().GetRadius() * 6.0f;
		else
			m_holeCupRadius = GolfBall().GetRadius() * 3.0f;
	}

	WVector diff = GolfBall().m_pos - GetHoleCupPos();

	dir.y = 0.0f;
	diff.y = 0.0f;
	float a = dir * dir;
	float b = dir * diff;
	float radius = GolfBall().GetRadius();
	float rr = m_holeCupRadius - GolfBall().GetRadius();
	float c = diff * diff;
	float d = b * b - a * (c - rr * (m_holeCupRadius - radius));
	if (d < 0.0f)
		return 1.0f;

	float sq = sqrtf(d);
	float inv = 1.0f / a;
	float t[2];
	t[0] = (b * -1.0f - sq) * inv;
	t[1] = (sq - b) * inv;

	float r = m_holeCupRadius - GolfBall().GetRadius();
	float result = 1.0f;
	if (r == sqrtf(c))
		return 0.0f;
	if (r > sqrtf(c))
	{
		if (t[1] < 1.0f)
			result = t[1];
	}
	else
	{
		result = t[0];
	}
	return result;
}

float CQuadTree::GetWeatherRollCoeff(int index) const
{
	switch (Doc()->m_golfGame.weather)
	{
	case 0:
	case 1:
		break;
	case 2:
	case 3:
		switch (m_pPolySoup->GetProperty(index).ground)
		{
		case 0:
			return 1.3f;
		case 3:
			return 1.5f;
		case 2:
			return 1.13f;
		case 7:
		case 8:
			break;
		default:
			return 1.8f;
		}
		break;
	}
	return 1.0f;
}

float CQuadTree::GetWeatherBounceCoeff(int index) const
{
	switch (Doc()->m_golfGame.weather)
	{
	case 0:
	case 1:
		break;
	case 2:
	case 3:
		switch (m_pPolySoup->GetProperty(index).ground)
		{
		case 0:
			return 0.8f;
		case 3:
			return 0.7f;
		case 2:
			return 0.9f;
		case 7:
		case 8:
			break;
		default:
			return 0.8f;
		}
		break;
	}
	return 1.0f;
}

void CQuadTree::SetCondition()
{
	switch (Doc()->m_golfGame.weather)
	{
	case 0:
	case 1:
		m_condition = 0.0f;
		break;
	case 2:
	case 3:
		switch (GolfBall().m_groundType)
		{
		case 0:
			m_condition = -2.0f;
			break;
		case 1:
			m_condition = -2.0f;
			break;
		case 2:
			m_condition = 0.0f;
			break;
		case 3:
			m_condition = -2.0f;
			break;
		case 4:
			m_condition = -3.0f;
			break;
		default:
			m_condition = -3.0f;
			break;
		}
		break;
	}
}

float CQuadTree::GetRollCoeff(int index) const
{
	return m_pPolySoup->GetProperty(index).roll;
}

float CQuadTree::GetBounceCoeff(int index) const
{
	return m_pPolySoup->GetProperty(index).bound;
}

unsigned char CQuadTree::GetGroundType(WVector pos, bool bCheckTee,
	CGolfBall::eWater* water)
{
	if (bCheckTee && !GOLFDOC()->m_bNoStat &&
		WisEqual(pos.x, GetTeeShotPos().x, g_EPSILON) &&
		WisEqual(pos.z, GetTeeShotPos().z, g_EPSILON))
		return 0;

	GetMeshList(pos);

	int index;
	pos = ColTest(pos, true, &index, true);

	if (index < 0)
		return 10;

	bool bWater = false;
	for (unsigned int i = 0; i < m_water.size(); ++i)
	{
		const sWater& w = m_water[i];
		if (pos.x > w.aabb.min.x && pos.x < w.aabb.max.x &&
			pos.z > w.aabb.min.z && pos.z < w.aabb.max.z &&
			pos.y < w.aabb.max.y)
		{
			if (water)
				*water = (CGolfBall::eWater)w.type;
			bWater = true;
		}
	}

	if (bWater)
		return 10;

	return m_pPolySoup->GetProperty(m_pBaseModel->tri[index].propIndex).ground;
}

bool CQuadTree::IsOBArea(const WVector& pos)
{
	int index = GetMeshIndex(pos, false);
	if (index < 0)
		return true;

	return (m_pPolySoup->GetProperty(m_pBaseModel->tri[index].propIndex).pass &
			   2)
		? true
		: false;
}

bool CQuadTree::IsPenetrable(int index) const
{
	if (m_pPolySoup->GetProperty(index).pass & 1)
		return true;
	return false;
}

float CQuadTree::GetMaxPowerOnGround() const
{
	if (IsLocalContent(S3_SAFETEE_ITEM))
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
			return (m_pPolySoup->GetProperty(1)
						   .max[PLAYER(GOLFDOC()->m_currentPlayer)->bunker] +
					   m_condition) *
				0.01f;
	}

	return (m_pPolySoup
				   ->GetProperty(
					   m_pBaseModel->tri[GolfBall().m_meshIndex].propIndex)
				   .max[PLAYER(GOLFDOC()->m_currentPlayer)->bunker] +
			   m_condition) *
		0.01f;
}

float CQuadTree::GetMinPowerOnGround() const
{
	if (IsLocalContent(S3_SAFETEE_ITEM))
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
			return (m_pPolySoup->GetProperty(1)
						   .min[PLAYER(GOLFDOC()->m_currentPlayer)->bunker] +
					   m_condition) *
				0.01f;
	}

	return (m_pPolySoup
				   ->GetProperty(
					   m_pBaseModel->tri[GolfBall().m_meshIndex].propIndex)
				   .min[PLAYER(GOLFDOC()->m_currentPlayer)->bunker] +
			   m_condition) *
		0.01f;
}

float CQuadTree::GetMaxPowerOnGround(const WVector& pos)
{
	if (IsLocalContent(S3_SAFETEE_ITEM))
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
			return (m_pPolySoup->GetProperty(1)
						   .max[PLAYER(GOLFDOC()->m_currentPlayer)->bunker] +
					   m_condition) *
				0.01f;
	}

	return (m_pPolySoup
				   ->GetProperty(
					   m_pBaseModel->tri[GetMeshIndex(pos, false)].propIndex)
				   .max[PLAYER(GOLFDOC()->m_currentPlayer)->bunker] +
			   m_condition) *
		0.01f;
}

float CQuadTree::GetMinPowerOnGround(const WVector& pos)
{
	if (IsLocalContent(S3_SAFETEE_ITEM))
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
			return (m_pPolySoup->GetProperty(1)
						   .min[PLAYER(GOLFDOC()->m_currentPlayer)->bunker] +
					   m_condition) *
				0.01f;
	}

	return (m_pPolySoup
				   ->GetProperty(
					   m_pBaseModel->tri[GetMeshIndex(pos, false)].propIndex)
				   .min[PLAYER(GOLFDOC()->m_currentPlayer)->bunker] +
			   m_condition) *
		0.01f;
}

const char* CQuadTree::GetBounceSound(int index) const
{
	return m_pPolySoup->GetProperty(index).boundSound;
}

const char* CQuadTree::GetRollSound(int index) const
{
	return m_pPolySoup->GetProperty(index).rollSound;
}

const char* CQuadTree::GetStepSound(int index) const
{
	return m_pPolySoup->GetProperty(index).stepSound;
}

const char* CQuadTree::GetStepSound(const WVector& pos)
{
	return GetStepSound(m_pBaseModel->tri[GetMeshIndex(pos, false)].propIndex);
}

const char* CQuadTree::FindTexture(int texHandle)
{
	if (m_pPolySoup->m_texIndex.find(texHandle) ==
		m_pPolySoup->m_texIndex.end())
	{
		LogOut(0, "FindTexture %d : %s\n", texHandle, m_pPolySoup->m_mapName);
	}
	return m_pPolySoup->m_texture[m_pPolySoup->m_texIndex[texHandle]].name;
}

void CQuadTree::UpdateProperty()
{
	IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(
		((GetCurMap() > 127 && GetCurMap() != 253) ? GetCurMap() - 128
												   : GetCurMap()) |
		0x28000000);

	if (course && course->TexProp[0])
		m_pPolySoup->m_pDoc->LoadXml(course->TexProp,
			GOLFDOC()->m_currentHole - 1, true);
	else
		m_pPolySoup->m_pDoc->LoadXml(
			MakeStr("%s_property.xml", GetCurMapName()),
			GOLFDOC()->m_currentHole - 1, true);

	for (int i = 0; i < m_pPolySoup->m_header.textureNum; i++)
		m_pPolySoup->m_texture[i].propIndex =
			m_pPolySoup->GetPropertyIndex(m_pPolySoup->m_texture[i].name);
}

void CQuadTree::ResetWater()
{
	m_water.clear();
	m_water.reserve(32);
}

void CQuadTree::HoleIn()
{
	CGolfBall& ball = GolfBall();
	CGolfBall::sExpected* pData = ball.PreDataToWrite(ball.m_renderNum);

	ball.m_vel.Reset();
	ball.m_state = CGolfBall::BALL_STOP;
	ball.SetHoleIn(1);
	pData->event = 3;
	pData->effect = 0;

	unsigned long tid =
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall;
	if (tid == 0x14000005 || tid == 0x14000080)
		strcpy(pData->sound, "wb_holein");
	else
		strcpy(pData->sound, "\xb0\xf8_\xc8\xa6\xc0\xce");
}

WVector CQuadTree::GetCollisionPoint(const WVector& normal)
{
	WVector point = GolfBall().m_pos - normal * GolfBall().GetRadius();
	return point;
}

bool CQuadTree::OverlappedWithObject(WVector pos, float radius)
{
	for (unsigned char i = 0; i < m_objNum; i++)
	{
		WPolySoup::sModel** pm = &m_pPolySoup->m_model[m_objIndex[i]];
		WPolySoup::sModel* model = *pm;
		if (model->noCollision & 1)
			continue;

		int count = 0;
		w_bound_box* bb = model->pet->m_bbList.Find("char0");
		if (bb)
		{
			do
			{
				Wobb obb;
				obb = bb->bone->GetMatrix();
				obb *= (bb->info->aabb.max - bb->info->aabb.min) * 0.5f +
					WVector::ONE * radius;
				obb.center = bb->info->spherePivot * bb->bone->m_matrix;

				if (obb.IsInclude(pos))
					return true;

				bb = (m_pPolySoup->m_model[m_objIndex[i]])
						 ->pet->m_bbList.Find(MakeStr("char%d", ++count));
			} while (bb);
		}
		else
		{
			if (pos.y > model->bbox.max.y || model->bbox.min.y - 10.0f > pos.y)
				continue;

			Waabb aabb;
			aabb = model->bbox;
			aabb.min -= WVector(1.0f, 0.0f, 1.0f) * radius;
			aabb.max += WVector(1.0f, 0.0f, 1.0f) * radius;

			if (pos.x > aabb.min.x && pos.x < aabb.max.x &&
				pos.z > aabb.min.z && pos.z < aabb.max.z)
				return true;
		}
	}

	return false;
}

void CQuadTree::UpdateVtxColor(float dt)
{
	if (g_input->GetDown("\xc5\xd7\xbd\xba\xc6\xae", true))
		m_vtxColorMode = (m_vtxColorMode + 1) % 2;

	WVector color;
	switch (m_vtxColorMode)
	{
	case 0:
		color = m_baseVtxColor;
		break;
	case 1:
		color = m_objVtxColor;
		break;
	}

	int key[3] = { 0, 0, 0 };
	if (g_input->Get('Q'))
		key[0] = 1;

	if (g_input->Get('W'))
		key[1] = 1;

	if (g_input->Get('E'))
		key[2] = 1;

	for (int i = 0; i < 3; i++)
	{
		if (key[i])
		{
			if (g_input->Get('='))
				color.p[i] = Between(0.0f, color.p[i] + dt * 64.0f, 255.0f);

			if (g_input->Get('-'))
				color.p[i] = Between(0.0f, color.p[i] - dt * 64.0f, 255.0f);
		}
	}

	switch (m_vtxColorMode)
	{
	case 0:
		if (!WisEqual(m_baseVtxColor, color, g_EPSILON))
			SetBaseVtxColor(0xff000000 | ((unsigned char)color.x << 16) |
				((unsigned char)color.y << 8) | (unsigned char)color.z);
		break;
	case 1:
		if (!WisEqual(m_objVtxColor, color, g_EPSILON))
			SetObjVtxColor(0xff000000 | ((unsigned char)color.x << 16) |
				((unsigned char)color.y << 8) | (unsigned char)color.z);
		break;
	}
}

void CQuadTree::SetBaseVtxColor(unsigned long color)
{
	m_baseVtxColor.x = (float)((color >> 16) & 0xff);
	m_baseVtxColor.y = (float)((color >> 8) & 0xff);
	m_baseVtxColor.z = (float)(color & 0xff);

	if (!m_pBaseModel || !m_pBaseModel->pet)
		return;

	m_pBaseModel->pet->SetVtxColor(color);
	m_pBaseModel->pet->UpdateLightSource(&g_lightset, false, NULL);
}

void CQuadTree::SetObjVtxColor(unsigned long color)
{
	m_objVtxColor.x = (float)((color >> 16) & 0xff);
	m_objVtxColor.y = (float)((color >> 8) & 0xff);
	m_objVtxColor.z = (float)(color & 0xff);

	if (m_pPolySoup)
	{
		for (int i = 0; i < m_pPolySoup->m_modelNum; i++)
		{
			if (m_pPolySoup->m_model[i]->pet)
			{
				m_pPolySoup->m_model[i]->pet->SetVtxColor(color);
				m_pPolySoup->m_model[i]->pet->UpdateLightSource(&g_lightset,
					false, NULL);
			}
		}
	}
}

WVector CQuadTree::DistanceBetweenRayNSegment(const WVector& from,
	const WVector& dir, const WVector& start, const WVector& seg)
{
	WVector w = from - start;
	float a = dir * dir;
	float b = dir * seg;
	float c = seg * seg;
	float d = dir * w;
	float e = seg * w;

	float D = a * c - b * b;
	float sc, sN, sD;
	float tc, tN, tD;

	if (Wabs(D) < g_EPSILON)
	{
		sN = 0.0f;
		sD = 1.0f;
		tN = e;
		tD = c;
	}
	else
	{
		sN = b * e - c * d;
		tN = a * e - b * d;
	}

	sD = D;
	if (sN < 0.0f)
	{
		sN = 0.0f;
		tN = e;
		tD = c;
	}
	else
	{
		tD = D;
	}

	if (tN < 0.0f)
	{
		tN = 0.0f;
		if (-d < 0.0f)
		{
			sN = 0.0f;
		}
		else
		{
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD)
	{
		tN = tD;
		if (b - d < 0.0f)
			sN = 0.0f;
		else
		{
			sN = b - d;
			sD = a;
		}
	}

	if (Wabs(sN) < g_EPSILON)
		sc = 0.0f;
	else
		sc = sN / sD;

	if (Wabs(tN) < g_EPSILON)
		tc = 0.0f;
	else
		tc = tN / tD;

	WVector diff = (from + dir * sc) - (start + seg * tc);
	return WVector(sc, tc, diff.Magnitude());
}

bool CQuadTree::SimpleColTest(const WVector& from, const WVector& dir)
{
	GetObjList(from, dir);

	for (unsigned char i = 0; i < m_objNum; i++)
	{
		if (RayIntersectAABB(m_pPolySoup->m_model[m_objIndex[i]]->aabb, from,
				dir, NULL, 0.0f) < 1.0f)
			return true;
	}

	return false;
}

float CQuadTree::RayIntersectAll(const WVector& from, const WVector& dir,
	bool bAll)
{
	int i;
	float tMin = 1.0f;
	WVector v[3];
	WPlane plane;
	float t;

	GetObjList(from, dir);

	for (i = 0; i < m_objNum; i++)
	{
		WPolySoup::sModel* model = m_pPolySoup->m_model[m_objIndex[i]];
		if (!model->pet)
			continue;

		int num = model->pet->GetFaceNum();
		for (int j = 0; j < num; j++)
		{
			if (!bAll && IsPenetrable(model->tri[j].propIndex))
				continue;

			for (int k = 0; k < 3; k++)
				v[k] = model->tri[j].v[k];
			plane = model->tri[j].plane;
			if (model->tri[j].bDoubleSide &&
				(plane.z * from.z + plane.y * from.y) + plane.x * from.x +
						plane.dis <
					0.0f)
			{
				plane.normal.x = -plane.normal.x;
				plane.normal.y = -plane.normal.y;
				plane.normal.z = -plane.normal.z;
				plane.dis *= -1.0f;
			}

			t = intersection::IntersectPlane(from, dir, plane, false);
			if (t < tMin)
			{
				if (intersection::PointInTriangle(from + dir * t, v[0], v[1],
						v[2]))
					tMin = t;
			}
		}
	}

	GetMeshList(from, dir);

	for (i = 0; i < m_meshNum; i++)
	{
		int index = m_meshIndex[i];
		if (!bAll && IsPenetrable(m_pBaseModel->tri[index].propIndex))
			continue;

		plane = m_pBaseModel->tri[index].plane;
		if (m_pBaseModel->tri[index].bDoubleSide &&
			(plane.z * from.z + plane.y * from.y) + plane.x * from.x +
					plane.dis <
				0.0f)
		{
			plane.normal.x = -plane.normal.x;
			plane.normal.y = -plane.normal.y;
			plane.normal.z = -plane.normal.z;
			plane.dis *= -1.0f;
		}

		t = intersection::IntersectPlane(from, dir, plane, false);
		if (t < tMin)
		{
			for (int k = 0; k < 3; k++)
				v[k] = m_pBaseModel->tri[index].v[k];
			if (intersection::PointInTriangle(from + dir * t, v[0], v[1], v[2]))
				tMin = t;
		}
	}

	return tMin;
}

void CQuadTree::ClipDecalPolygon(WVector* poly, int num, int plane)
{
	WVector front[1024];
	float dists[1024];
	char sides[1024];
	int i;
	WVector* v;

	if (num > 1022)
		return;

	if (plane == 6)
	{
		if (num > 2)
		{
			if (m_decalVtxNum + num >= m_decalVtxMax)
				num = m_decalVtxMax - m_decalVtxNum;

			WVector* p = &m_decalVtx[m_decalVtxNum];
			WVector* v = poly;
			for (int i = 0; i < num; i++)
				p[i] = *v++;

			m_decalVtxNum += num;
		}
		return;
	}

	WPlane* pPlane = &m_decalPlane[plane];
	bool bFront = false;

	for (i = 0; i < num; i++)
	{
		float dist = poly[i] * pPlane->normal - pPlane->dis;
		if (dist > 0.03125f)
		{
			bFront = true;
			sides[i] = 0;
		}
		else if (dist < -0.03125f)
		{
			sides[i] = 1;
		}
		else
		{
			sides[i] = 2;
		}

		dists[i] = dist;
	}

	if (!bFront)
		return;

	sides[i] = sides[0];
	dists[i] = dists[0];
	poly[i] = poly[0];

	int count = 0;
	for (i = 0, v = poly; i < num; i++, v++)
	{
		if (sides[i] == 0 || sides[i] == 2)
		{
			front[count] = *v;
			count++;
		}

		if (sides[i] != 2 && sides[i + 1] != 2)
		{
			if (sides[i] != sides[i + 1])
			{
				float d = dists[i] / (dists[i] - dists[i + 1]);
				WVector dir;
				dir = v[1] - v[0];
				front[count] = v[0] + dir * d;
				count++;
			}
		}
	}

	ClipDecalPolygon(front, count, plane + 1);
}

void CQuadTree::ClipDecalTriangle(const WPolySoup::sTriangle* tri, WVector dir)
{
	if (!tri->bDoubleSide)
	{
		if (tri->plane.normal * dir < 0.01f)
			return;
	}

	float poly[4][3];
	memcpy(poly, tri->v, sizeof(WVector) * 3);
	ClipDecalPolygon((WVector*)poly, 3, 0);
}

static int fragmentsCacheIndex = 0;
static fragmentsCache_s fragmentsCache[32];

static fragmentsCache_s* FindFragmentsCache(const WVector* points,
	const WVector& normal)
{
	int i = fragmentsCacheIndex;
	int last = fragmentsCacheIndex - 1;
	if (last < 0)
		last = 31;

	while (i != last)
	{
		fragmentsCache_s* cache = &fragmentsCache[i];

		if (cache->used)
		{
			if (WisEqual(cache->points[0], points[0], 0.15f) &&
				WisEqual(cache->points[1], points[1], 0.15f) &&
				WisEqual(cache->points[2], points[2], 0.15f) &&
				WisEqual(cache->points[3], points[3], 0.15f) &&
				WisEqual(cache->normal, normal, 0.15f))
				return cache;
		}

		if (++i == 32)
			i = 0;
	}

	return NULL;
}

static void AddFragmentsCache(const WVector* points, const WVector& normal,
	int numVerts, const WVector* verts, int numIndices,
	const unsigned short* indices)
{
	fragmentsCache_s* cache = &fragmentsCache[fragmentsCacheIndex++];
	if (fragmentsCacheIndex == 32)
		fragmentsCacheIndex = 0;

	cache->used = true;

	cache->points[0] = points[0];
	cache->points[1] = points[1];
	cache->points[2] = points[2];
	cache->points[3] = points[3];
	cache->normal = normal;

	cache->numVerts = numVerts;
	memcpy(cache->verts, verts, numVerts * sizeof(WVector));
	cache->numIndices = numIndices;
	memcpy(cache->indices, indices, numIndices * sizeof(unsigned short));
}

void CQuadTree::DecalFragments(const WVector* points, const WVector& normal,
	WVector* vtx, int* vtxNum, int vtxMax, unsigned short* idx, int* idxNum,
	int idxMax, bool bObject)
{
	int i, j, index;
	Waabb box;
	WVector dir, side, up;

	fragmentsCache_s* cache = FindFragmentsCache(points, normal);
	if (cache)
	{
		memcpy(vtx, cache->verts, cache->numVerts * sizeof(WVector));
		memcpy(idx, cache->indices, cache->numIndices * sizeof(unsigned short));
		*vtxNum = cache->numVerts;
		*idxNum = cache->numIndices;
		return;
	}

	WVector sum;
	sum.Reset();
	for (short n = 0; n < 4; n++)
		sum += points[n];
	sum *= 0.25f;

	WVector pos = sum;
	GetMeshList(sum);
	pos = ColTest(pos, false, &index, true);
	if (pos.y < m_pBaseModel->aabb.min.y)
		pos.y = m_pBaseModel->aabb.min.y;
	WVector center = pos;

	dir = -normal;
	dir.Normalize();

	side = points[0] - points[1];
	side.Normalize();

	up = WCrossProduct(dir, side);
	up.Normalize();

	float width = (points[1] - points[0]).Magnitude() * 0.5f;
	float height = (points[2] - points[1]).Magnitude() * 0.5f;
	float depth = width > height ? width : height;

	float dis = center * side;
	m_decalPlane[0] = WPlane(side, dis - width);
	m_decalPlane[1] = WPlane(-side, -dis - width);

	dis = center * up;
	m_decalPlane[2] = WPlane(up, dis - height);
	m_decalPlane[3] = WPlane(-up, -dis - height);

	dis = center * dir;
	m_decalPlane[4] = WPlane(dir, dis - depth);
	m_decalPlane[5] = WPlane(-dir, -dis - depth);

	box.Clear();
	for (i = 0; i < 4; i++)
		box.AddPoint(points[i]);
	box.min.y -= depth;
	box.max.y += depth;

	GetMeshList(box);

	m_decalVtx = vtx;
	m_decalVtxNum = 0;
	m_decalVtxMax = vtxMax;

	int numIndices = 0;
	for (i = 0; i < m_meshNum; i++)
	{
		int start = m_decalVtxNum;
		ClipDecalTriangle(&m_pBaseModel->tri[m_meshIndex[i]], dir);

		int num = m_decalVtxNum - start - 2;
		unsigned short* p = &idx[numIndices];
		for (j = 0; j < num; j++)
		{
			if (numIndices + 3 > idxMax)
				break;

			*p++ = start;
			*p++ = start + j + 1;
			*p++ = start + j + 2;
		}

		numIndices += j * 3;
	}

	if (bObject)
	{
		GetObjList(box);

		for (i = 0; i < m_objNum; i++)
		{
			WPolySoup::sModel* model = m_pPolySoup->m_model[m_objIndex[i]];

			int faceNum = model->pet->GetFaceNum();
			for (int k = 0; k < faceNum; k++)
			{
				int start = m_decalVtxNum;
				ClipDecalTriangle(&model->tri[k], dir);

				int num = m_decalVtxNum - start - 2;
				unsigned short* p = &idx[numIndices];
				for (j = 0; j < num; j++)
				{
					if (numIndices + 3 > idxMax)
						break;

					*p++ = start;
					*p++ = start + j + 1;
					*p++ = start + j + 2;
				}

				numIndices += j * 3;
			}
		}
	}

	AddFragmentsCache(points, normal, m_decalVtxNum, vtx, numIndices, idx);

	*vtxNum = m_decalVtxNum;
	*idxNum = numIndices;
}

void CQuadTree::DrawDecal(int texHandle, const WVector& pos,
	const WVector& right, const WVector& up, float alpha, float scale,
	enumPolyBlend blend, bool bObject)
{
	static WVector verts[1024];
	static unsigned short indices[1536];
	WTVertex vertices[1024];
	int numVerts, numIndices;

	WVector r = right;
	r.Normalize();
	WVector u = up;
	u.Normalize();

	WVector normal;
	normal = WCrossProduct(r, u);
	normal.Normalize();

	WVector points[4];
	points[0] = pos - right - up;
	points[1] = pos + right - up;
	points[2] = pos + right + up;
	points[3] = pos - right + up;

	DecalFragments(points, normal, verts, &numVerts, 1024, indices, &numIndices,
		1536, bObject);

	float width = sqrtf((float)(right * right));
	float height = up.Magnitude();
	float radius = (width + height) * 0.5f;
	float sScale = alpha / (width * 2.0f);
	float tScale = alpha / (height * 2.0f);

	WVector* v = verts;
	for (int i = 0; i < numVerts; i++)
	{
		vertices[i].SetPosition(v[i]);
		vertices[i].tu = ((v[i] - pos) * r) * sScale + 0.5f;
		vertices[i].tv = 0.5f - ((v[i] - pos) * u) * tScale;

		float dist =
			fabs(pos.y - v[i].y) < radius ? fabs(pos.y - v[i].y) : radius;
		vertices[i].diffuse =
			((unsigned char)((radius - dist) * (1.0f / radius) * scale * 255.0f)
				<< 24) |
			0xffffff;
	}

	g_view->DrawIndexedTriangles(vertices, numVerts, indices, numIndices,
		(texHandle & 0x7ff) | blend | W_POLY_NO_DEPTH_WRITE |
			W_POLY_TYPE_NORMAL_CLAMP | W_POLY_ZFUNC_MOSTLESSEQUAL,
		0x400);
}

void CQuadTree::TraceAABB(const WVector& from, const WVector& to,
	const Waabb& aabb, trace_s* trace)
{
	Waabb src, dest, box;
	WVector direction;
	WPlane plane, bestPlane;

	memset(trace, 0, sizeof(trace_s));

	if (WisEqual(from, to, 0.00001f))
	{
		trace->endpos = to;
		return;
	}

	src = aabb + from;
	dest = aabb + to;

	box = ExpandAABB(src, dest);
	box.min -= WVector::ONE;
	box.max += WVector::ONE;

	direction = to - from;

	float fraction = 1.0f;

	GetObjList(box);

	for (int i = 0; i < m_objNum; i++)
	{
		WPolySoup::sModel* model = m_pPolySoup->m_model[m_objIndex[i]];

		if (model->noCollision & 1)
			continue;

		int count = 0;
		w_bound_box* bb = model->pet->FindBoundBox("char1");
		if (bb)
		{
			bb = model->pet->FindBoundBox("char0");
			while (bb)
			{
				count++;
				Waabb charBox = bb->info->aabb + model->pet->GetMatrix().pivot;
				float t = AabbIntersect(src, direction, charBox, &plane);
				if (t < fraction)
				{
					fraction = t;
					bestPlane = plane;
				}

				bb = model->pet->FindBoundBox(MakeStr("char%d", count));
			}
		}
		else
		{
			float t = AabbIntersect(src, direction, model->bbox, &plane);
			if (t < fraction)
			{
				fraction = t;
				bestPlane = plane;
			}
		}
	}

	if (fraction == 1.0f)
		trace->endpos = to;
	else
		trace->endpos = from + WVector::UNIT_POS_Y + direction * fraction;

	trace->fraction = fraction;
	trace->plane = bestPlane;
	trace->startsolid = false;
}

void CQuadTree::TraceOBB(const WVector& from, const WVector& to,
	const Waabb& aabb, trace_s* trace)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	Waabb src, dest, box;
	WVector direction;
	WPlane plane, bestPlane;

	memset(trace, 0, sizeof(trace_s));

	if (WisEqual(from, to, 0.00001f))
	{
		trace->endpos = to;
		return;
	}

	src = aabb + from;
	dest = aabb + to;

	box = ExpandAABB(src, dest);
	box.min -= WVector::ONE;
	box.max += WVector::ONE;

	direction = to - from;

	float fraction = 1.0f;

	GetObjList(box);
	for (int i = 0; i < m_objNum; i++)
	{
		WPolySoup::sModel* model = m_pPolySoup->m_model[m_objIndex[i]];

		if (model->noCollision & 1)
			continue;

		int count = 0;
		w_bound_box* bb = model->pet->FindBoundBox("char0");
		if (bb)
		{
			bb = model->pet->FindBoundBox("char0");
			while (bb)
			{
				count++;

				Wobb obb = bb->bone->GetMatrix();
				obb *= (bb->info->aabb.max - bb->info->aabb.min) * 0.5f;
				obb.center = bb->info->spherePivot * bb->bone->GetMatrix();
				float t = ObbIntersect(src, direction, obb, &plane);
				if (t < fraction)
				{
					fraction = t;
					bestPlane = plane;
				}

				bb = model->pet->FindBoundBox(MakeStr("char%d", count));
			}
		}
	}

	if (fraction == 1.0f)
		trace->endpos = to;
	else
		trace->endpos = from + WVector::UNIT_POS_Y + direction * fraction;

	trace->fraction = fraction;
	trace->plane = bestPlane;
	trace->startsolid = false;
}

void CQuadTree::DrawDebugTraceOBB(const WVector& from, const WVector& to,
	const Waabb& aabb)
{
	Waabb src, dest, box;

	src = aabb + from;
	dest = aabb + to;

	box = ExpandAABB(src, dest);
	box.min -= WVector::ONE;
	box.max += WVector::ONE;

	GetObjList(box);
	for (int i = 0; i < m_objNum; i++)
	{
		WPolySoup::sModel* model = m_pPolySoup->m_model[m_objIndex[i]];

		if (model->noCollision & 1)
			continue;

		WPuppet* pet = model->pet;
		if (!pet)
			continue;

		int count = 0;
		w_bound_box* bb = pet->FindBoundBox("char0");
		while (bb)
		{
			Wobb obb = bb->bone->GetMatrix();
			count++;
			obb *= (bb->info->aabb.max - bb->info->aabb.min) * 0.5f;
			w_bound_box_info* info = bb->info;
			obb.center = info->spherePivot * bb->bone->GetMatrix();
			g_view->DrawOBB(obb, 0xff00ff00);

			bb = model->pet->FindBoundBox(MakeStr("char%d", count));
		}
	}
}

void CQuadTree::DrawDebugTraceAABB(const WVector& from, const WVector& to,
	const Waabb& aabb)
{
	Waabb src, dest, box;

	src = aabb + from;
	dest = aabb + to;

	box = ExpandAABB(src, dest);
	box.min -= WVector::ONE;
	box.max += WVector::ONE;

	GetObjList(box);
	for (int i = 0; i < m_objNum; i++)
	{
		WPolySoup::sModel* model = m_pPolySoup->m_model[m_objIndex[i]];

		if (model->noCollision & 1)
			continue;

		if (!model->pet)
			continue;

		int count = 0;
		if (model->pet->FindBoundBox("char1"))
		{
			w_bound_box* bb = model->pet->FindBoundBox("char0");
			while (bb)
			{
				count++;
				Waabb charBox = bb->info->aabb + model->pet->GetMatrix().pivot;
				g_view->DrawAABB(charBox, 0xffff0000, false, 0);

				bb = model->pet->FindBoundBox(MakeStr("char%d", count));
			}
		}
		else
		{
			g_view->DrawAABB(model->bbox, 0xffff0000, false, 0);
		}
	}
}

int CQuadTree::GetObjColNum()
{
	int num = 0;
	std::vector<sColData>::iterator it;
	for (it = m_colList.begin(); it != m_colList.end(); ++it)
	{
		if ((*it).objIndex >= 0)
			num++;
	}

	return num;
}

bool CQuadTree::IsFallenIntoWater(WVector pos, Waabb* aabb, bool bCheck)
{
	bool bFallen = false;
	for (unsigned int i = 0; i < m_water.size(); i++)
	{
		sWater& water = m_water[i];

		if (pos.x > water.aabb.min.x && pos.x < water.aabb.max.x &&
			pos.z > water.aabb.min.z && pos.z < water.aabb.max.z &&
			pos.y < water.aabb.max.y)
		{
			if (aabb)
				*aabb = water.aabb;

			if (!bCheck)
				GolfBall().m_water = (CGolfBall::eWater)water.type;

			bFallen = true;
		}
	}

	return bFallen;
}

bool CQuadTree::IsOverWater(WVector pos)
{
	GetMeshList(pos);

	for (unsigned int i = 0; i < m_water.size(); i++)
	{
		sWater& water = m_water[i];

		pos.y = (water.aabb.min.y + water.aabb.max.y) * 0.5f;

		if (water.aabb.IsInclude(pos) &&
			water.aabb.max.y > ColTest(pos, true, NULL, true).y)
			return true;
	}

	return false;
}

float CQuadTree::GetShadowIntensity(const WVector& pos)
{
	if (!COption::Instance()->vIsShadowmapEnabled() || !GOLFDOC()->m_bDay)
		return 1.0f;

	WVector dir = g_lightset.nearone;
	dir.y = 0.0f;
	dir.Normalize();
	dir *= -320.0f;

	GetObjList(pos, dir);

	float intensity = 1.0f;
	Waabb box;
	unsigned char shadow = (unsigned char)(g_lightset.ambient >> 17);

	for (int i = 0; i < m_objNum; i++)
	{
		WPolySoup::sModel* model;
		if (m_pPolySoup->GetModelNum() != m_pPolySoup->GetRmrModelNum())
		{
			model = NULL;
			for (int k = 0; k < m_pPolySoup->GetModelNum(); k++)
			{
				if (m_pPolySoup->m_model[k]->id == m_objIndex[k])
					model = m_pPolySoup->m_model[k];
			}
		}
		else
		{
			model = m_pPolySoup->m_model[m_objIndex[i]];
		}

		if (!model)
			continue;

		if (model->aabb.max.y - model->aabb.min.y < 16.0f)
			continue;

		if (model->aabb.IsInclude(pos) && (pos - model->pos) * dir < 0.0f)
			return shadow;

		WVector size;
		size = model->aabb.max - model->pos;
		box.min = model->pos - WVector(size.x * 0.75f, size.y, size.z * 0.75f);
		box.max =
			WVector(size.x * 0.75f, size.y * 0.5f, size.z * 0.75f) + model->pos;

		float t = RayIntersectAABBSolid(box, pos, g_lightset.nearone * -320.0f,
			NULL, 0.0f);
		if (t < intensity)
			intensity = t;
	}

	return intensity;
}

WVector CQuadTree::GetRayIntersection(WVector from, WVector to)
{
	to.Normalize();
	to *= 1.6f;

	float t = 1.0f;

	for (int i = 0; i < 1500; i++)
	{
		if (GetGroundPoint(from + to, true, NULL, false).y > (from + to).y)
		{
			GetMeshList(from, to);

			if (m_meshNum > 0)
			{
				for (int j = 0; j < m_meshNum; j++)
					t = Min(IntersectRayTriangle(from, to,
								m_pBaseModel->tri[m_meshIndex[j]].v),
						t);

				if (t < 1.0f)
					break;
			}
		}

		from += to;
	}

	return GetGroundPoint(from + to * t, false, NULL, false);
}

WVector CQuadTree::GetRayIntersectionObj(WVector from, WVector to)
{
	int count = 0;
	bool bHit = false;
	WVector dir = to;

	if (!IsLocalContent(S4_REAL_MYROOM))
		return WVector::ONE;

	dir.Normalize();
	dir *= 1.6f;

	float t = 1.0f;
	WVector tri[3];
	WMatrix mat;
	const int corner[4][3] = {
		{ -1, 1,  -1 },
		{ -1, -1, -1 },
		{ 1,  -1, -1 },
		{ 1,  1,  -1 },
	};

	int j;

	for (int i = 0; i < 1500; i++)
	{
		GetObjList(from, dir);
		for (j = 0; j < m_objNum; j++)
		{
			WPuppet* pet = m_pPolySoup->m_model[m_objIndex[j]]->pet;
			if (pet)
			{
				w_bound_box* center = pet->FindBoundBox("center");
				if (center)
				{
					w_bound_box* box = pet->FindBoundBox("char0");
					while (box)
					{
						Wobb obb = box->bone->GetMatrix();
						count++;
						obb *=
							(box->info->aabb.max - box->info->aabb.min) * 0.5f;
						obb.center =
							box->info->spherePivot * box->bone->GetMatrix();

						mat.pivot = obb.center;
						mat.xa = obb.extend[0];
						mat.ya = obb.extend[1];
						mat.za = obb.extend[2];

						WVector v[4];
						for (int k = 0; k < 4; k++)
							v[k] = WVector(corner[k][0], corner[k][1],
									   corner[k][2]) *
								mat;
						tri[0] = v[0];
						tri[1] = v[1];
						tri[2] = v[2];
						t = Min(IntersectRayTriangle(from, dir, tri), t);
						if (t < 1.0f)
							bHit = true;

						tri[0] = v[1];
						tri[1] = v[2];
						tri[2] = v[3];
						t = Min(IntersectRayTriangle(from, dir, tri), t);
						if (t < 1.0f)
							bHit = true;

						if (bHit)
							break;

						box = m_pPolySoup->m_model[m_objIndex[j]]
								  ->pet->FindBoundBox(MakeStr("char%d", count));
					}
				}
			}

			if (bHit)
				break;
		}

		if (bHit)
			break;

		from += dir;
	}

	if (!bHit)
		return GetRayIntersection(from, to);

	WPolySoup::sModel* model = m_pPolySoup->m_model[m_objIndex[j]];
	int meshNum = model->quad->meshNum;

	t = 1.0f;
	for (int k = 0; k < meshNum; k++)
	{
		t = Min(IntersectRayTriangle(from, dir, model->tri[k].v), t);

		if (t < 1.0f)
			break;
	}

	return from + dir * t;
}

WVector CQuadTree::GetRayIntersectionWall(WVector from, WVector to)
{
	WVector dir = to;

	dir.Normalize();
	dir *= 1.6f;

	to = GetRayIntersection(from, to);

	Waabb aabb;
	GetPVS().GetAABB(aabb);

	if (aabb.IsInclude(to))
		return to;

	if (!m_pPolySoup->m_model[0]->pet)
		return from;

	float t = 1.0f;

	for (int i = 0; i < 1500; i++)
	{
		int faceNum = m_pPolySoup->m_model[0]->pet->GetFaceNum();

		if (faceNum > 0)
		{
			for (int j = 0; j < faceNum; j++)
			{
				float r = Min(IntersectRayTriangle(from, dir,
								  m_pPolySoup->m_model[0]->tri[j].v),
					t);

				if (r > 0.0f)
					t = r;
			}

			if (t < 1.0f)
				break;
		}

		from += dir;
	}

	to = from + dir * t;
	return to;
}

float CQuadTree::IntersectRayTriangle(const WVector& from, const WVector& dir,
	const WVector* tri)
{
	return intersection::IntersectRayTriangle(from, dir, tri);
}

void CQuadTree::MakeAfterBurner()
{
	WVector vel = GolfBall().PreData(0)->vel;

	vel.y = 0.0f;
	GolfBall().m_afterBurner =
		vel * (GolfBall().m_initSpin * -40.0f) * (1.0f / GolfClub().GetPower());

	GolfBall().m_spin = GolfBall().m_initSpin * 1.5f;
	GolfBall().m_afterBurnerFrame = GolfBall().m_renderNum;
	GolfBall().m_state = CGolfBall::BALL_SPIN;
	GolfBall().m_vel.Reset();

	GolfBall().m_special &= ~0x73;
}

void CQuadTree::MakeVectorSlide(float x, float y, float z, int index)
{
	if (GolfBall().m_initSpecial & 0x50)
		return;

	GolfBall().m_state = CGolfBall::BALL_BOUNCE;

	bool bLimit = false;
	if (GolfBall().GetVectorSlideCount() >= index)
		bLimit = true;

	if (GolfBall().m_vel.Magnitude() < y)
		GolfBall().m_vel *= x;

	WVector pos = GetGroundPoint(GolfBall().m_pos + GolfBall().m_vel * 0.02f,
		true, NULL, true);

	WVector dir = pos - GolfBall().m_pos;
	dir.Normalize();
	dir *= GolfBall().m_vel.Magnitude();

	if (!bLimit)
		GolfBall().m_vel.y = dir.y + z;
	else
		GolfBall().m_vel.y = dir.y;

	GolfBall().AddVectorSlideCount();

	GolfBall().PreDataToWrite(GolfBall().m_renderNum)->event = 6;
}

int CQuadTree::GetBounceCount()
{
	int count = m_waterBounceNum;
	int frame = -1;

	for (std::vector<sColData>::iterator it = m_colList.begin();
		it != m_colList.end(); ++it)
	{
		if ((*it).frame - frame > 5)
		{
			count++;
			frame = (*it).frame;
		}
	}

	return count;
}

float CQuadTree::GetBasePosY(WVector pos, float height)
{
	for (unsigned char i = 0; i < m_water.size(); i++)
	{
		Waabb aabb = m_water[i].aabb;

		if (pos.x > aabb.min.x && pos.x < aabb.max.x && pos.z > aabb.min.z &&
			pos.z < aabb.max.z && pos.y < aabb.max.y)
		{
			pos.y = aabb.max.y;
			break;
		}
	}

	if (m_bUseLowest && GolfBall().m_renderNum > 0)
		return GetLowestGround(pos) + height;

	float t[2];
	t[0] = CheckCollision(pos, WVector(0.0f, 100.0f, 0.0f), NULL, true);
	t[1] = CheckCollision(WVector(pos.x, pos.y + 100.0f, pos.z),
		WVector(0.0f, -100.0f, 0.0f), NULL, true);

	float d = 1.0f - t[1];
	int index;
	if (t[0] < d)
		index = 0;
	else
		index = 1;

	float y;
	if (t[index] < 1.0f)
	{
		if (index == 0)
		{
			float t2 =
				CheckCollision(pos, WVector(0.0f, -100.0f, 0.0f), NULL, true);
			if (t2 < 1.0f)
				y = pos.y - t2 * 100.0f + height;
			else
				y = pos.y;
		}
		else
			y = d * 100.0f + pos.y + height;
	}
	else
	{
		WVector down;
		down.z = 0.0f;
		down.y = -100.0f;
		down.x = 0.0f;
		float t2 = CheckCollision(pos, down, NULL, true);
		if (t2 < 1.0f)
			y = pos.y - t2 * 100.0f + height;
		else
			y = pos.y;
	}

	return Max(y, height + m_lowest);
}

float CQuadTree::GetGravity()
{
	return m_gravityModifier * 34.295296f;
}

void CQuadTree::GravityModifier(float modifier)
{
	m_gravityModifier = modifier;
}

void CQuadTree::TraceOBB_New(const WVector& from, const WVector& to,
	const Waabb& aabb, trace_s* trace)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	Waabb fromBox;
	Waabb toBox;
	Waabb box;
	Waabb objBox;
	WVector pos = to;
	bool bHit = false;
	memset(trace, 0, sizeof(trace_s));

	const float epsilon = 0.00001f;
	if (WisEqual(from, to, epsilon))
	{
		trace->endpos = to;
		return;
	}

	fromBox = aabb + from;
	toBox = aabb + to;

	box = ExpandAABB(fromBox, toBox);
	box.min -= WVector::ONE;
	box.max += WVector::ONE;

	GetObjList(box);
	for (int i = 0; i < m_objNum; i++)
	{
		WPolySoup::sModel** pm = &m_pPolySoup->m_model[m_objIndex[i]];
		WPolySoup::sModel* model = *pm;
		if (model->noCollision & 1)
			continue;

		int count = 0;
		if (model->pet)
		{
			w_bound_box* bbox = model->pet->FindBoundBox("char0");

			if (bbox)
			{
				while (bbox)
				{
					Wobb obb;
					obb = bbox->bone->GetMatrix();
					const float radius = 1.0f;
					const float half = 0.5f;
					obb *=
						(bbox->info->aabb.max - bbox->info->aabb.min) * half +
						WVector::ONE * radius;
					const WMatrix& mat = bbox->bone->GetMatrix();
					obb.center = bbox->info->spherePivot * mat;

					if (obb.IsInclude(pos))
					{
						bHit = true;
						break;
					}

					bbox =
						m_pPolySoup->m_model[m_objIndex[i]]->pet->FindBoundBox(
							MakeStr("char%d", ++count));
				}
			}
			else
			{
				const float margin = 10.0f;
				if (pos.y > model->bbox.max.y ||
					model->bbox.min.y - margin > pos.y)
					continue;

				objBox = model->bbox;

				objBox.min -= WVector(1.0f, 0.0f, 1.0f);
				objBox.max += WVector(1.0f, 0.0f, 1.0f);

				if (pos.x > objBox.min.x && pos.x < objBox.max.x &&
					pos.z > objBox.min.z && pos.z < objBox.max.z)
				{
					bHit = true;
					break;
				}
			}
		}
	}

	if (!bHit)
	{
		trace->endpos = to;
	}
	else
	{
		trace->endpos = from;
	}

	const float one = 1.0f;
	trace->fraction = one;

	trace->startsolid = false;
}

bool CQuadTree::TraceAABB_BackGround(const WVector& from, const WVector& to,
	const Waabb& aabb, trace_s* trace, WPolySoup::sModel* model)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return false;

	if (WisEqual(from, to, 0.00001f))
	{
		trace->endpos = to;
		return false;
	}
	else
	{
		memset(trace, 0, sizeof(trace_s));

		WVector dir;
		dir = to - from;

		const Waabb& base = m_pBaseModel->aabb;
		Waabb box(base.min, base.max);
		box.min.x -= 1.0f;
		box.min.y -= 1.0f;
		box.min.z -= 1.0f;

		box.max += WVector::ONE;
		box.max.y = m_pPolySoup->m_model[0]->aabb.max.y;

		Waabb moved(aabb.min, aabb.max);

		moved.min.x += dir.x;
		moved.min.z += dir.z;
		moved.max.x += dir.x;
		moved.max.z += dir.z;

		bool bInclude;
		if (box.IsInclude(moved))
			bInclude = true;
		else
			bInclude = false;

		trace->fraction = 1.0f;
		trace->startsolid = false;

		return bInclude;
	}
}

int CQuadTree::CheckCamera()
{
	int camera = -1;
	int index = 0;

	if (!IsLocalContent(S4_REAL_MYROOM))
		return -1;

	if (m_pPolySoup->m_model[0]->pet)
	{
		w_bound_box* center =
			m_pPolySoup->m_model[0]->pet->FindBoundBox("center");

		if (center)
		{
			w_bound_box* bb =
				m_pPolySoup->m_model[0]->pet->FindBoundBox("camera0");
			while (bb)
			{
				WVector pos[4];

				pos[0] = bb->info->aabb.min;
				pos[1] = bb->info->aabb.min;
				pos[2] = bb->info->aabb.min;
				pos[3] = bb->info->aabb.min;
				pos[0].y = bb->info->aabb.max.y;
				pos[1].x = bb->info->aabb.max.x;
				pos[1].y = bb->info->aabb.max.y;
				pos[2].x = bb->info->aabb.max.x;

				bool bIn = false;
				for (int i = 0; i < 4; i++)
				{
					bIn = g_view->InFrustum(pos[i]);

					if (bIn)
					{
						camera = index;
						break;
					}
				}

				if (bIn)
					break;

				bb = m_pPolySoup->m_model[0]->pet->FindBoundBox(
					MakeStr("camera%d", ++index));
			}
		}
	}

	return camera;
}

void CQuadTree::SetAvatarPosition(const WVector& pos, float size)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	m_avatarBox.Reset();

	m_avatarBox.min = pos;
	m_avatarBox.max = pos;

	for (int i = 0; i < 3; i++)
	{
		m_avatarBox.min.p[i] -= size;
		m_avatarBox.max.p[i] += size;
	}
	m_avatarBox.min.y = -2.0f;
	m_avatarBox.max.y = 2.0f;
}

void CQuadTree::TraceOBB_Avatar_genPosition(const WVector& from,
	const WVector& to, trace_s* trace, WPolySoup::sModel* model)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	memset(trace, 0, sizeof(trace_s));

	int index = 0;
	bool bHit = false;
	WVector pos;
	if (WisEqual(from, to, 0.00001f))
	{
		trace->endpos = from;
		return;
	}

	WVector dir;
	dir = to - from;

	if (!model->pet)
	{
		trace->endpos = from;
		return;
	}

	w_bound_box* center = model->pet->FindBoundBox("center");
	if (center)
	{
		Wobb obb;
		obb = center->bone->GetMatrix();
		obb *= (m_avatarBox.max - m_avatarBox.min) * 0.5f;
		obb.center.x = (m_avatarBox.max.x + m_avatarBox.min.x) * 0.5f;
		obb.center.y = (m_avatarBox.max.y + m_avatarBox.min.y) * 0.5f;
		obb.center.z = (m_avatarBox.max.z + m_avatarBox.min.z) * 0.5f;

		// HACK!
		do
		{
			w_bound_box* bb = model->pet->FindBoundBox("char0");
			while (bb)
			{
				WBone* bone = bb->bone;
				Wobb charObb;
				charObb = bone->GetMatrix();
				charObb *= (bb->info->aabb.max - bb->info->aabb.min) * 0.5f;
				charObb.center = bb->info->spherePivot * bone->GetMatrix();

				dir.y = 0.0f;
				Wobb moved = charObb;
				moved += dir;

				if (IntersectOBB(obb, moved))
				{
					WVector n = dir;
					n.Normalize();
					n.y = 0.0f;

					moved = charObb;
					moved += n;

					if (IntersectOBB(obb, moved))
					{
						pos = from;
					}
					else
					{
						pos = from + n;
					}

					bHit = true;
					break;
				}

				bb = model->pet->FindBoundBox(MakeStr("char%d", ++index));
			}
		} while (0);
	}

	if (bHit)
		trace->endpos = pos;
	else
		trace->endpos = to;
}

void CQuadTree::TraceOBB_WallSliding(const WVector& from, const WVector& to,
	trace_s* trace, WPolySoup::sModel* model, int index, bool bCheck)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	memset(trace, 0, sizeof(trace_s));

	bool bCollide = false;
	bool bFound = false;
	Waabb src;
	Waabb dest;
	WPlane plane;
	Wobb obb;
	Wobb charObb;
	Wobb moved;
	w_bound_box* window;
	WVector direction;

	WVector result = from;
	int roomIndex = 0;
	int charIndex = 0;
	WVector target = to;

	if (WisEqual(from, to, 0.00001f) && !bCheck)
	{
		trace->endpos = from;
		return;
	}

	direction = to - from;
	float fraction = 1.0f;

	WPolySoup::sModel* room = m_pPolySoup->m_model[0];
	if (!strcmp(room->name, model->name))
	{
		trace->endpos = from;
		return;
	}

	if (!room->pet)
	{
		trace->endpos = from;
		return;
	}

	if (!model->pet)
	{
		trace->endpos = from;
		return;
	}

	w_bound_box* center = model->pet->FindBoundBox("center");
	if (!center)
	{
		trace->endpos = from;
		return;
	}

	w_bound_box* roomCenter = room->pet->FindBoundBox("center");

	if (index)
	{
		window = room->pet->FindBoundBox("center_window");
		if (!window)
			return;
	}

	src = model->aabb;

	if (!roomCenter)
	{
		trace->endpos = from;
		return;
	}

	WPolySoup::sModel*& room0 = m_pPolySoup->m_model[0];
	w_bound_box* bb = room0->pet->FindBoundBox("char0");
	while (bb)
	{
		obb = bb->bone->GetMatrix();
		obb *= (bb->info->aabb.max - bb->info->aabb.min) * 0.5f;
		obb.center = bb->info->spherePivot * bb->bone->GetMatrix();

		WPolySoup::sModel*& roomModel = m_pPolySoup->m_model[0];
		dest = bb->info->aabb + roomModel->pet->GetMatrix().pivot;

		w_bound_box* charBox = model->pet->FindBoundBox("char0");
		while (charBox)
		{
			bFound = true;

			charObb = charBox->bone->GetMatrix();
			WVector half =
				(charBox->info->aabb.max - charBox->info->aabb.min) * 0.5f;
			charObb.extend[0] *= half.x;
			charObb.extend[1] *= half.y;
			charObb.extend[2] *= half.z;
			charObb.center =
				charBox->info->spherePivot * charBox->bone->GetMatrix();

			src = model->aabb;

			if (index > 0)
			{
				float t = AabbIntersect(src, direction, dest, &plane);

				if (t < fraction)
				{
					fraction = t;
				}
			}
			else
			{
				moved = charObb;
				if (index == 0)
					direction.y = 0.0f;
				if (index == 1)
				{
					direction.z = 0.0f;
					src = charBox->info->aabb;
					src.max += charObb.center + direction;
					src.min += charObb.center + direction;

					if (src.max.y > window->info->aabb.max.y &&
						src.max.y - window->info->aabb.max.y > 0.001f)
					{
						direction.y = to.y -
							(src.max.y - window->info->aabb.max.y) - from.y;
						target.y = from.y + direction.y;
					}

					if (src.min.y < window->info->aabb.min.y)
					{
						direction.y = window->info->aabb.min.y - src.min.y +
							to.y - from.y;
						target.y = from.y + direction.y;
					}
				}

				moved += direction;

				if (IntersectOBB(obb, moved))
				{
					bCollide = true;

					float dz = GetObbPos(moved, true, 2) - bb->info->aabb.min.z;
					float dx;
					if (target.x > 0.0f)
						dx = GetObbPos(moved, true, 0) - bb->info->aabb.min.x;
					else
						dx = GetObbPos(moved, false, 0) - bb->info->aabb.max.x;

					if (fabs(bb->info->aabb.max.x - bb->info->aabb.min.x) >
						fabs(bb->info->aabb.max.z - bb->info->aabb.min.z))
					{
						target.z -= dz;
					}
					else
					{
						if (target.x > 0.0f)
							target.x -= dx;
						else if (target.x < 0.0f)
							target.x -= dx;
						else
						{
							target.z = obb.center.z - dz;
							target.x = obb.center.x - dx;
						}
					}

					if (index == 1)
					{
						direction.x = target.x - from.x;
						direction.z = target.z - from.z;
					}
					else
					{
						direction = target - from;
					}

					result = target;
				}
			}

			charBox = model->pet->FindBoundBox(MakeStr("char%d", ++charIndex));
		}

		charIndex = 0;
		bb = m_pPolySoup->m_model[0]->pet->FindBoundBox(
			MakeStr("char%d", ++roomIndex));
	}

	if (index > 0)
	{
		if (fraction == 1.0f)
		{
			trace->endpos = to;
			return;
		}

		if (fraction != 0.0f)
			trace->endpos = from + WVector::UNIT_POS_Y + direction * fraction;
		else
			trace->endpos = from + WVector::UNIT_POS_Y + direction;

		WPolySoup::sModel*& roomModel = m_pPolySoup->m_model[0];
		dest = window->info->aabb + roomModel->pet->GetMatrix().pivot;

		trace->endpos.y = to.y;

		if (to.y + (model->aabb.max.y - model->aabb.min.y) * 0.5f > dest.max.y)
		{
			trace->endpos.y =
				dest.max.y - (model->aabb.max.y - model->aabb.min.y) * 0.5f;
		}

		if (to.y - (model->aabb.max.y - model->aabb.min.y) * 0.5f < dest.min.y)
		{
			trace->endpos.y =
				dest.min.y + (model->aabb.max.y - model->aabb.min.y) * 0.5f;
		}
	}
	else
	{
		if (bCollide)
		{
			trace->endpos = result;
			return;
		}

		if (bFound)
			trace->endpos = to;
		else
			trace->endpos = from;
	}
}

bool CQuadTree::CheckPickRoom(WVector pos)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return false;

	Waabb box;

	if (!m_pPolySoup->m_model[0]->pet)
		return false;

	w_bound_box* bb =
		m_pPolySoup->m_model[0]->pet->FindBoundBox("center_window");
	if (!bb)
	{
		return false;
	}

	box = bb->info->aabb + m_pPolySoup->m_model[0]->pet->GetMatrix().pivot;

	return box.IsInclude(pos);
}

int CQuadTree::Get_WallSlidingAngle(const WVector& pos,
	WPolySoup::sModel* model, WVector& normal)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return -1;

	WVector dir = pos;
	Waabb box;
	Waabb centerBox;
	WVector cand[3];
	memset(cand, 0, sizeof(cand));
	int index = 0;
	WPuppet* pet = m_pPolySoup->m_model[0]->pet;
	if (!pet)
		return -1;

	w_bound_box* center = pet->FindBoundBox("center");

	pet = model->pet;
	w_bound_box* bb = pet->FindBoundBox("center");
	if (!bb)
		return -1;

	centerBox = bb->info->aabb + pet->GetMatrix().pivot;

	if (!center)
		return -1;

	w_bound_box* col = m_pPolySoup->m_model[0]->pet->FindBoundBox("char_col0");
	while (col)
	{
		box = col->info->aabb + m_pPolySoup->m_model[0]->pet->GetMatrix().pivot;

		if (box.IsInclude(pos))
		{
			WVector p1;
			WVector p2;
			switch (index)
			{
			case 0:
			{
				p1.z = pos.z;
				p2.z = pos.z;
				p1.x = pos.x - (centerBox.max.x - centerBox.min.x) * 0.5f;
				p1.y = pos.y - (centerBox.max.y - centerBox.min.y) * 0.5f;
				p2.x = pos.x + (centerBox.max.x - centerBox.min.x) * 0.5f;
				p2.y = pos.y + (centerBox.max.y - centerBox.min.y) * 0.5f;
			}
			break;
			case 1:
			case 2:
			{
				p1.x = pos.x;
				p2.x = pos.x;
				p1.y = pos.y - (centerBox.max.y - centerBox.min.y) * 0.5f;
				p1.z = pos.z - (centerBox.max.x - centerBox.min.x) * 0.5f;
				p2.y = pos.y + (centerBox.max.y - centerBox.min.y) * 0.5f;
				p2.z = pos.z + (centerBox.max.x - centerBox.min.x) * 0.5f;
			}
			break;
			}

			if (box.IsInclude(p1) && box.IsInclude(p2))
			{
				dir.Normalize();
				normal = (p1 + p2) * 0.5f + dir;
				cand[index] = normal;
			}
		}

		col = m_pPolySoup->m_model[0]->pet->FindBoundBox(
			MakeStr("char_col%d", ++index));
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (cand[i].p[j] != 0.0f)
			{
				WVector best;
				best = normal + normal;

				for (int k = 0; k < 3; k++)
				{
					WVector v = cand[k];

					double len = sqrt(v.x * v.x + v.z * v.z);
					if (len < sqrt(best.x * best.x + best.z * best.z) &&
						0.0 != len)
					{
						best = cand[k];
						index = k;
					}
				}

				normal = cand[index];
				return index;
			}
		}
	}

	return -1;
}

void CQuadTree::OBBToAABB(Wobb obb, Waabb& aabb, WVector& center)
{
	WPolySoup::sModel* model = GOLFDOC()->m_pPolySoup->FindModel(NULL);

	Waabb base = model->aabb;
	WVector vmax = base.min;
	vmax.y = 0.0f;
	WVector vmin = base.max;
	vmin.y = 30.0f;

	const Wobb& o = obb;
	WMatrix mat;
	mat.pivot = o.center;
	mat.xa = o.extend[0];
	mat.ya = o.extend[1];
	mat.za = o.extend[2];

	WVector box[8] = {
		WVector(-1.0f, 1.0f, -1.0f),
		WVector(-1.0f, -1.0f, -1.0f),
		WVector(1.0f, -1.0f, -1.0f),
		WVector(1.0f, 1.0f, -1.0f),
		WVector(-1.0f, 1.0f, 1.0f),
		WVector(-1.0f, -1.0f, 1.0f),
		WVector(1.0f, -1.0f, 1.0f),
		WVector(1.0f, 1.0f, 1.0f),
	};
	WVector vertex[8];
	WVector junk[8];
	for (int k = 0; k < 8; k++)
		junk[k] = box[k] * mat;
	for (int k = 0; k < 8; k++)
		vertex[k] = box[k] * mat;
	WVector sum(0.0f, 0.0f, 0.0f);
	for (int k = 0; k < 8; k++)
		sum += vertex[k] + junk[k];

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (!(vmax.p[j] > vertex[i].p[j]))
				vmax.p[j] = vertex[i].p[j];
		}
		for (int j = 0; j < 3; j++)
		{
			if (!(vmin.p[j] < vertex[i].p[j]))
				vmin.p[j] = vertex[i].p[j];
		}
	}

	aabb.max = vmax;
	aabb.min = vmin;
}

float CQuadTree::GetObbPos(Wobb& obb, bool bMax, int axis)
{
	float ret;
	if (bMax)
		ret = -1000.0f;
	else
		ret = 1000.0f;

	WMatrix mat;
	mat.pivot = obb.center;
	mat.xa = obb.extend[0];
	mat.ya = obb.extend[1];
	mat.za = obb.extend[2];

	WVector box[8] = {
		WVector(-1.0f, 1.0f, -1.0f),
		WVector(-1.0f, -1.0f, -1.0f),
		WVector(1.0f, -1.0f, -1.0f),
		WVector(1.0f, 1.0f, -1.0f),
		WVector(-1.0f, 1.0f, 1.0f),
		WVector(-1.0f, -1.0f, 1.0f),
		WVector(1.0f, -1.0f, 1.0f),
		WVector(1.0f, 1.0f, 1.0f),
	};
	WVector vertex[8];
	for (int k = 0; k < 8; k++)
	{
		WVector v = box[k] * mat;
		v.z = box[k].x * mat.zx + box[k].y * mat.zy;
		v.z += box[k].z * mat.zz + mat.zm;
		vertex[k] = v;
	}

	if (bMax)
	{
		for (int i = 0; i < 8; i++)
		{
			if (!(ret > vertex[i].p[axis]))
				ret = vertex[i].p[axis];
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if (!(ret < vertex[i].p[axis]))
				ret = vertex[i].p[axis];
		}
	}

	return ret;
}

void CQuadTree::TraceOBB_Wall(const WVector& from, const WVector& to,
	trace_s* trace, WPolySoup::sModel* model, int index, bool bCheck)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;

	memset(trace, 0, sizeof(trace_s));

	bool bHit = false, bFound = false;
	WVector result = from;
	int i = 0;
	int j = 0;

	const float epsilon = 0.00001f;
	if (WisEqual(from, to, epsilon) && !bCheck)
	{
		trace->endpos = from;
		return;
	}

	WVector dir;
	dir = to - from;

	WPolySoup::sModel*& first = m_pPolySoup->m_model[0];
	WPolySoup::sModel* base = first;
	if (!strcmp(base->name, model->name))
	{
		trace->endpos = from;
		return;
	}
	if (!base->pet)
	{
		trace->endpos = from;
		return;
	}
	if (!model->pet)
	{
		trace->endpos = from;
		return;
	}

	const char* centerName = "center";
	if (model->pet->FindBoundBox(centerName) &&
		base->pet->FindBoundBox(centerName))
	{
		const char* boxName = "char0";
		w_bound_box* box1 = base->pet->FindBoundBox(boxName);
		while (box1)
		{
			const float half = 0.5f;
			Wobb obb1;
			obb1 = box1->bone->GetMatrix();
			obb1 *= (box1->info->aabb.max - box1->info->aabb.min) * half;
			obb1.center = box1->info->spherePivot * box1->bone->GetMatrix();

			w_bound_box* box2 = model->pet->FindBoundBox("char0");
			if (box2)
				bFound = true;
			while (box2)
			{
				Wobb tmp;
				tmp = box2->bone->GetMatrix();
				tmp *= (box2->info->aabb.max - box2->info->aabb.min) * 0.5f;
				tmp.center = box2->info->spherePivot * box2->bone->GetMatrix();

				Wobb obb2;
				obb2 = tmp;
				if (index == 0)
					dir.y = 0.0f;
				else if (index == 1)
					dir.z = 0.0f;

				obb2 += dir;
				if (IntersectOBB(obb1, obb2))
				{
					bHit = true;
					WVector move = dir;
					move.Normalize();
					if (index == 0)
						move.y = 0.0f;
					else if (index == 1)
						move.z = 0.0f;

					obb2 = tmp;
					obb2 += move;
					if (IntersectOBB(obb1, obb2))
					{
						result = from;
						break;
					}

					dir = move;
					result = from + move;
					break;
				}

				box2 = model->pet->FindBoundBox(MakeStr("char%d", ++j));
			}

			j = 0;
			box1 = m_pPolySoup->m_model[0]->pet->FindBoundBox(
				MakeStr("char%d", ++i));
		}
	}
	else
	{
		trace->endpos = from;
		return;
	}

	if (bHit)
	{
		trace->endpos = result;
		return;
	}
	if (bFound)
	{
		trace->endpos = to;
		return;
	}
	trace->endpos = from;
}

bool CQuadTree::TraceOBB_Object(const WVector& from, const WVector& to,
	const Waabb& aabb, trace_s* trace, WPolySoup::sModel* model, bool bCheck)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return false;

	memset(trace, 0, sizeof(trace_s));

	bool bHit = false;
	int i = 0;
	int j = 0;
	Waabb box;
	Waabb fromBox;
	Waabb toBox;
	WVector dir;
	dir = to - from;

	if (!m_pPolySoup->m_model[0]->pet)
	{
		trace->endpos = from;
		return true;
	}
	if (!strcmp(m_pPolySoup->m_model[0]->name, model->name))
	{
		trace->endpos = from;
		return true;
	}

	fromBox = aabb + from;
	toBox = aabb + to;
	box = ExpandAABB(fromBox, toBox);
	box.min -= WVector::ONE;
	box.max += WVector::ONE;
	GetObjList(box);

	if (!model->pet)
	{
		trace->endpos = from;
		return true;
	}
	if (!model->pet->FindBoundBox("center"))
	{
		trace->endpos = from;
		return true;
	}

	for (int k = 0; k < m_objNum; k++)
	{
		if (!bCheck && m_objIndex[k] <= 0)
			continue;

		WPolySoup::sModel* obj = m_pPolySoup->m_model[m_objIndex[k]];
		if (!strcmp(obj->name, model->name))
			continue;
		if (!obj->pet)
			continue;

		w_bound_box* center = obj->pet->FindBoundBox("center");
		char petName[256] = "poster_a.pet";
		if (!stricmp(m_pPolySoup->m_model[m_objIndex[k]]->pet->GetPuppetName(),
				petName))
			continue;
		if (!center)
			continue;

		w_bound_box* box1 =
			m_pPolySoup->m_model[m_objIndex[k]]->pet->FindBoundBox("char0");
		while (box1)
		{
			Wobb obb1;
			obb1 = box1->bone->GetMatrix();
			obb1 *= (box1->info->aabb.max - box1->info->aabb.min) * 0.5f;
			obb1.center = box1->info->spherePivot * box1->bone->GetMatrix();

			w_bound_box* box2 = model->pet->FindBoundBox("char0");
			while (box2)
			{
				Wobb tmp;
				tmp = box2->bone->GetMatrix();
				tmp *= (box2->info->aabb.max - box2->info->aabb.min) * 0.5f;
				tmp.center = box2->info->spherePivot * box2->bone->GetMatrix();

				box2 = model->pet->FindBoundBox(MakeStr("char%d", ++j));

				Wobb obb2 = tmp;
				obb2.center.x = obb2.center.x + dir.x;
				obb2.center.y = obb2.center.y + dir.y;
				obb2.center.z = obb2.center.z + dir.z;
				if (IntersectOBB(obb1, obb2))
				{
					WVector dir2 = dir;
					dir2.Normalize();

					obb2 = tmp;
					obb2 += WVector(dir2.x, 0.0f, dir2.z);
					if (IntersectOBB(obb1, obb2))
					{
						bHit = true;
						break;
					}

					trace->endpos =
						WVector(from.x + dir2.x, from.y, from.z + dir2.z);
					return false;
				}
			}

			if (bHit)
				break;

			j = 0;
			box1 = m_pPolySoup->m_model[m_objIndex[k]]->pet->FindBoundBox(
				MakeStr("char%d", ++i));
		}
	}

	if (bHit)
		trace->endpos = from;
	else if (bCheck)
		trace->endpos = from + to;
	else
		trace->endpos = to;

	return bHit;
}
