#include "projectg.h"
#include "petal.h"
#include "petbody.h"
#include "wind.h"

extern WMatrix g_camera;

const float PI = 3.14159265358979323846f;
const int ntFlutter::MAX_FLUTTER_PET = 10;

ntFlutter::ntFlutter(int num, const std::string& petName, int type)
	: m_frag(NULL), m_petName(petName)
{
	memset(m_pet, 0, sizeof(m_pet));

	m_area.min = WVector(-112.0f, -112.0f * 0.7f, -112.0f);
	m_area.max = WVector(112.0f, 112.0f * 0.3f, 112.0f);

	Reset(num, 1.0f);
}

ntFlutter::~ntFlutter()
{
	ReleasePets();

	if (m_frag)
	{
		delete[] m_frag;
		m_frag = NULL;
	}
}

void ntFlutter::Reset(int num, float rate)
{
	m_num = num;

	ReleasePets();
	if (m_frag)
	{
		delete[] m_frag;
		m_frag = NULL;
	}

	m_frag = new sFrag[m_num];

	m_rate = rate;

	char szName[260];
	sprintf(szName, "*pet %s", MakeStr(m_petName.c_str(), rand() % 2 + 1));

	int count = m_num > MAX_FLUTTER_PET ? MAX_FLUTTER_PET : m_num;
	for (int i = 0; i < count; i++)
	{
		m_pet[i] = new CPetBody(szName, false);
		w_motion_data* motion = m_pet[i]->GetPet()->GetFirstMotionData();
		if (motion)
			m_pet[i]->SetMotion(motion->name, false, 0.0f, false);
		m_pet[i]->Process(Random(0.0f, 2.0f));
		m_pet[i]->ActiveGlobalLight(true);
	}

	for (int j = 0; j < m_num; j++)
	{
		m_frag[j].yaw = Random(0.0f, PI * 2.0f);
		m_frag[j].swing = Random(0.0f, PI);

		for (int k = 0; k < 3; k++)
			m_frag[j].center.p[k] =
				Random(-1.0f, 1.0f) * 112.0f + g_camera.pivot.p[k];

		m_frag[j].pos = m_frag[j].center + m_frag[j].offset;
	}
}

void ntFlutter::Process(float dt, float gravity)
{
	int i, j;
	Waabb area = m_area;
	area.min += g_camera.pivot;
	area.max += g_camera.pivot;
	WVector size = m_area.max - m_area.min;

	for (i = 0; i < m_num; i++)
	{
		m_frag[i].center += Wind().GetWind(m_frag[i].center) * dt;
		m_frag[i].center.y -= dt * gravity * 34.295296f;
		m_frag[i].swing += dt * (PI / 2.0f);

		float s = sinf(m_frag[i].swing);
		m_frag[i].offset =
			WVector(cosf(m_frag[i].yaw), 0.0f, sinf(m_frag[i].yaw)) * 4.0f * s;

		for (j = 0; j < 3; j++)
		{
			if (m_frag[i].center.p[j] > area.max.p[j])
				m_frag[i].center.p[j] -=
					ceilf((m_frag[i].center.p[j] - area.max.p[j]) / size.p[j]) *
					size.p[j];
			else if (m_frag[i].center.p[j] < area.min.p[j])
				m_frag[i].center.p[j] +=
					ceilf((area.min.p[j] - m_frag[i].center.p[j]) / size.p[j]) *
					size.p[j];
		}

		m_frag[i].pos = m_frag[i].center + m_frag[i].offset;
		m_frag[i].bVisible =
			(m_frag[i].pos - g_camera.pivot) * g_camera.za > 0.0f;
	}

	for (j = 0; j < MAX_FLUTTER_PET; j++)
	{
		if (m_pet[j])
			m_pet[j]->Process(dt);
	}
}

void ntFlutter::Display()
{
	g_view->GetWidth();
	for (int i = 0; i < m_num; i++)
	{
		if (m_frag[i].bVisible)
		{
			m_pet[i % MAX_FLUTTER_PET]->SetPos(m_frag[i].pos);
			m_pet[i % MAX_FLUTTER_PET]->SetLight();
			m_pet[i % MAX_FLUTTER_PET]->Display();
		}
	}
}

void ntFlutter::DisplayFullScreenOverlay()
{
	if (Doc()->m_golfGame.weather)
	{
		WRect rect(0.0f, 0.0f, g_view->GetWidth(), g_view->GetHeight());
		WOverlay::DrawBox(g_view, rect, 0, 0x4c3c3c3c, 0.001f);
	}
}

void ntFlutter::ReleasePets()
{
	for (int i = 0; i < MAX_FLUTTER_PET; i++)
	{
		if (m_pet[i])
		{
			delete m_pet[i];
			m_pet[i] = NULL;
		}
	}
}
