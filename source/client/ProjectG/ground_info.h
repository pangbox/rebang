#pragma once

#include <list>
#include "fx.h"

class CBase
{
protected:
	struct sBall
	{
		IFF_STRUCT::sBall* info;
		WPuppet* puppet;
		sBall()
			: info(NULL), puppet(NULL)
		{
		}
	};
	struct sFindBall
	{
		unsigned long typeId;
		sFindBall(unsigned long id)
			: typeId(id)
		{
		}
		bool operator()(sBall* ball) { return ball->info->c.TypeId == typeId; }
	};

public:
	CBase();
	~CBase();
	void Load();
	void ResetUiPosition();
	void Process(float delta);
	void Display();
	void SetNormal(const WVector& normal);
	void SetAngle(float angle) { m_angle = angle; }
	WMatrix GetBaseMatrix();
	sBall* ChangeBall(unsigned long typeId);
	void ChangeTurn();

protected:
	void DrawRing();
	sBall* AddBall(unsigned long typeId);
	sBall* FindBall(unsigned long typeId);
	void AttachFx();
	std::list<sBall*> m_balls;
	sBall* m_ball;
	float m_x, m_y;
	float m_unused18;
	WVector m_normal;
	WMatrix m_camera;
	LightSet m_light;
	float m_angle;
	int m_ringTexture, m_numberTexture;
	int m_ringCount, m_penalty;
	WVector2D m_spin;
	WMatrix m_matrix;
	float m_ringAngle;
	CPvsFxBox m_fx;
	WTVertex* m_vtx[5];
	WTVertex m_vertices[4];
};
