#include <math.h>
#include <new>
#include "wmath.h"

struct w_mesh
{
	int vtxNum;
	int rigidNum;
	int blendedRigidNum;
	int blendedTotalNum;
	WVector* vecList;
	char unknown_14[0x1c];
	int indexNum;
	ushort* indexList;
	char unknown_38[0x2c];
	w_mesh* next;
};

class WBone
{
public:
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

	void UpdatePhysicsModel(float delta, const WVector& accel);
	void ResetPhysicsModel();
	void SetPhysicsModel(int type);

protected:
	void ReleasePhysicsModel();
	int FindLink(int body, w_mesh* mesh,
		w_bone_physic_link::w_bone_physic_link_element* out, int offset);

private:
	w_bone_physic_link* m_physic;
	w_mesh* m_mesh;
	char unknown_08[0xe8];
	ulong m_flag;
	char unknown_f4[0x30];
	WMatrix m_matrix;
};

void WBone::SetPhysicsModel(int type)
{
	int cnt = 0;
	int offset = 0;
	w_mesh* mesh;
	for (mesh = m_mesh; mesh; mesh = mesh->next)
	{
		cnt += mesh->vtxNum;
		++offset;
	}

	m_flag |= 0x2000;
	m_physic = new w_bone_physic_link;
	m_physic->vtxnum = cnt;
	m_physic->pos = new WVector[cnt];
	m_physic->vel = new WVector[cnt];
	m_physic->linkoffset = new int[cnt];
	m_physic->list = new WVector*[offset];

	mesh = m_mesh;
	offset = 0;
	cnt = 0;
	int meshIndex = 0;
	for (; mesh; mesh = mesh->next, ++meshIndex)
	{
		for (int i = mesh->rigidNum; i < mesh->vtxNum; ++i)
			cnt += FindLink(i, mesh, 0, offset);
		m_physic->list[meshIndex] = m_physic->pos + offset;
		offset += mesh->vtxNum;
	}

	m_physic->link = new w_bone_physic_link::w_bone_physic_link_element[cnt];
	mesh = m_mesh;
	offset = 0;
	cnt = 0;
	for (; mesh; mesh = mesh->next)
	{
		int i;
		for (i = 0; i < mesh->rigidNum; ++i)
			m_physic->linkoffset[offset + i] = -1;
		for (i = mesh->rigidNum; i < mesh->vtxNum; ++i)
		{
			m_physic->linkoffset[offset + i] = cnt;
			cnt += FindLink(i, mesh, m_physic->link + cnt, offset);
		}
		offset += mesh->vtxNum;
	}
	ResetPhysicsModel();
}

void WBone::ReleasePhysicsModel()
{
	if (m_physic)
	{
		delete m_physic->link;
		delete m_physic->linkoffset;
		delete m_physic->pos;
		delete m_physic;
	}
}

void WBone::UpdatePhysicsModel(float delta, const WVector& accel)
{
	int i;
	for (i = 0; i < m_physic->vtxnum; ++i)
	{
		if (m_physic->linkoffset[i] >= 0)
			m_physic->vel[i] += accel * delta;
	}

	for (i = 0; i < m_physic->vtxnum; ++i)
	{
		if (m_physic->linkoffset[i] >= 0)
		{
			w_bone_physic_link::w_bone_physic_link_element* link =
				m_physic->link + m_physic->linkoffset[i];
			WVector pos;
			pos = m_physic->pos[i] + m_physic->vel[i] * delta;
			WVector gap = WVector::ZERO;
			for (int j = 0; link[j].offset >= 0; ++j)
			{
				WVector total;
				total = m_physic->pos[link[j].offset] - pos;
				float magnitude = total.Magnitude();
				float ext = magnitude - link[j].length;
				gap += total * (ext / magnitude);
			}
			m_physic->vel[i] += gap;
		}
	}

	for (i = 0; i < m_physic->vtxnum; ++i)
		m_physic->pos[i] += m_physic->vel[i] * delta;
}

int WBone::FindLink(int body, w_mesh* mesh,
	w_bone_physic_link::w_bone_physic_link_element* out, int offset)
{
	int temp[16];
	int cnt = 0;
	for (int i = 0; i < mesh->indexNum; i += 3)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (mesh->indexList[i + j] == body)
			{
				for (int k = 0; k < 3; ++k)
				{
					if (j != k)
					{
						int index = mesh->indexList[i + k];
						int n;
						for (n = 0; n < cnt; ++n)
						{
							if (temp[n] == index)
								break;
						}
						if (n == cnt)
							temp[cnt++] = index;
					}
				}
			}
		}
	}

	if (out)
	{
		int i;
		for (i = 0; i < cnt; ++i)
		{
			out[i].offset = temp[i] + offset;
			out[i].length =
				(mesh->vecList[body] - mesh->vecList[temp[i]]).Magnitude();
		}
		out[i].offset = -1;
	}
	return cnt + 1;
}

void WBone::ResetPhysicsModel()
{
	int i = 0;
	for (w_mesh* mesh = m_mesh; mesh; mesh = mesh->next)
	{
		for (int j = 0; j < mesh->vtxNum; ++j, ++i)
		{
			m_physic->pos[i] = mesh->vecList[j] * m_matrix;
			m_physic->vel[i] = WVector::ZERO;
		}
	}
}
