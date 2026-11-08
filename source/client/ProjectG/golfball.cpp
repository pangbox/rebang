#include "golfball.h"
#include "hackingmanager.h"

CGolfBall::CGolfBall()
{
	m_pExtra = NULL;
	m_radius = 0.14698039f;
	m_mass = 0.045927f;
}

void CGolfBall::ClearBuffer()
{
	m_extraNum = 0;
	if (m_pExtra)
	{
		delete[] m_pExtra;
		m_pExtra = NULL;
	}
}

void CGolfBall::AddCrypticValue()
{
	if (HackingManager::Instance())
	{
		HackingManager::Instance()->AddCrypticIntValue(&m_holeIn);
	}
}

void CGolfBall::Reset(WVector pos)
{
	memset(m_expected, 0, sizeof(m_expected));

	ClearBuffer();

	m_pos = pos;
	m_expected[0].pos = pos;

	float* rot = &m_rot.x;

	m_vel.Reset();
	rot[0] = rot[1] = 0.0f;
	m_state = BALL_STOP;

	m_initSpecial = 0;
	m_special = 0;

	m_bOB = false;

	m_initCurve = 0.0f;
	m_initSpin = 0.0f;
	m_curve = 0.0f;
	m_spin = 0.0f;

	m_water = WATER_NONE;
	m_groundType = 1;

	m_holeIn = 0;

	m_attrIndex = -1;
	m_standGround = 0;
	m_stayCount = 0;

	m_renderNum = 0;
	m_playFrame = -1;
	m_bounceFrame = -1;
	m_topFrame = -1;
	m_nearObjFrame = -1;
	m_holeFrame = -1;
	m_holeOutFrame = -1;
	m_greenFrame = -1;
	m_cupFrame = -1;
	m_afterBurnerFrame = -1;
	m_rollFrame = -1;
	m_cobraFrame = -1;
	m_tomahawkFrame = -1;
	m_spikeFrame = -1;
	m_spikeDiveFrame = -1;
	m_spikeEndFrame = -1;
	m_vectorSlideFrame = -1;

	m_prevColTriIndex = -1;
	m_prevColObjIndex = -1;
	m_colTriIndex = -1;
	m_colObjIndex = -1;

	ResetVectorSlideCount();
}

void CGolfBall::DbgTraceOutput(const char* msg, float value)
{
	DbgTraceToFile(msg, m_pos);
}

void CGolfBall::DbgTraceToFile(const char* msg, const WVector& pos)
{
}
