#include "stdafx.h"
#include "mathconsts.h"
#include "shotpreview.h"
#include "golfball.h"
#include "club.h"
#include "quadtree.h"
#include "3dfont.h"
#include "wind.h"
#include "cardsystem.h"
#include "fx.h"
#include "fxsequence.h"

extern LightSet g_lightset;

WMatrix operator*(const WMatrix& m, float f);

class ShotPreview::PangCoin
{
public:
	PangCoin(const WVector& pos, float size, bool visible);
	~PangCoin(void);

	void SetVisible(bool visible);
	void Restore(void);
	void Update(float delta);
	void Render(void);

private:
	static int _PetReferenceCounter;

	WPuppet* m_pPuppet;
	std::string m_name;
	WMatrix m_matrix;
	float m_angle;
	float m_size;
	bool m_visible;
	float m_radius;
	WVector m_pos;
};

class ShotPreview::UpdateSensor
{
public:
	UpdateSensor(void);

	void Update(void);
	bool Equal(const UpdateSensor& other) const;
	bool operator==(const UpdateSensor& other) const;

private:
	int m_playerShot;
	int m_spin[2];
	unsigned char m_clubIndex;

	WVector m_wind;
	int m_powerShotType;
	unsigned long m_shotItemTypeId;
	float m_calipers;
};

int ShotPreview::PangCoin::_PetReferenceCounter = 0;

ShotPreview::PangCoin::PangCoin(const WVector& pos, float size, bool visible)
{
	m_visible = true;
	m_size = size;
	m_angle = (rand() % 314) * 0.01f;
	m_pPuppet = g_resrcmng->GetPuppet("coin.pet", false, true, false);
	m_name = MakeStr("\xc6\xce\xc4\xda\xc0\xce_%d", ++_PetReferenceCounter);
	m_matrix.Reset();

	CSceneManager::Instance()->RegisterElement(OBJ_ACTOR, m_pPuppet, 0,
		m_name.c_str(), 8, 0);

	m_matrix.pivot = pos;
	m_pos = pos;
	m_pPuppet->CalcBound(0, true);
	m_radius = size * m_pPuppet->GetBoundSphere().radius;

	if (visible)
	{
		m_matrix.pivot.y -= m_radius;
	}
	else
	{
		m_pos.y += m_radius;
	}
}

ShotPreview::PangCoin::~PangCoin(void)
{
	--_PetReferenceCounter;

	if (m_pPuppet)
	{
		CSceneManager::Instance()->DeleteElement(m_name.c_str());
		if (g_resrcmng && m_pPuppet)
		{
			g_resrcmng->Release(m_pPuppet, false);
			m_pPuppet = 0;
		}
	}
}

void ShotPreview::PangCoin::Update(float delta)
{
	m_angle += delta * g_PI;

	if (!m_pPuppet || !m_visible)
	{
		return;
	}

	if (!(GOLFDOC()->GameMode() & 0x80))
	{
		return;
	}

	WVector d = GolfBall().m_pos - m_pos;
	if (!(d.Magnitude() <= m_radius))
	{
		return;
	}

	CFx::Instance()->OpenSequence("coin_pang.seq", true)->Pos() = m_pos;
	CSceneManager::Instance()->DeleteElement(m_name.c_str());
	if (g_resrcmng && m_pPuppet)
	{
		g_resrcmng->Release(m_pPuppet, false);
		m_pPuppet = 0;
	}
	g_audio->PlaySfx(MakeStr("pang_coin_%d", rand() % 3 + 1), 0, 0, 0, 0, 0.5f,
		200.0f);
}

void ShotPreview::PangCoin::SetVisible(bool visible)
{
	m_visible = visible;
	CSceneManager::Instance()->SetVisible(m_name.c_str(), visible);
}

void ShotPreview::PangCoin::Restore(void)
{
	if (m_pPuppet != 0)
	{
		return;
	}

	m_pPuppet = g_resrcmng->GetPuppet("coin.pet", false, true, false);
	CSceneManager::Instance()->RegisterElement(OBJ_ACTOR, m_pPuppet, 0,
		m_name.c_str(), 8, 0);
}

void ShotPreview::PangCoin::Render(void)
{
	if (!m_pPuppet || !m_visible)
	{
		return;
	}

	WVector pivot = m_matrix.pivot;
	m_matrix = RotMat(WVector(0.0f, 1.0f, 0.0f), m_angle);
	m_matrix.pivot = pivot;

	m_pPuppet->UpdateLightSource(&g_lightset, false, 0);
	m_pPuppet->ApplyBones(0, m_matrix * m_size);
	m_pPuppet->UpdateBSphere();

	m_pPuppet->Transform(0, 1.0f, 0);
}

ShotPreview::UpdateSensor::UpdateSensor(void)
{
	memset(this, 0, sizeof(UpdateSensor));
}

void ShotPreview::UpdateSensor::Update(void)
{
	AfxGetTask()->GetActor("Player")
		<< MsgObject(0, 0x110, (int)this, 0, 0, 0, 0);
	AfxGetTask()->GetActor("Spin")
		<< MsgObject(0, 0x161, (int)&m_spin[1], (int)&m_spin[0], 0, 0, 0);

	m_clubIndex = GolfClub().GetIndex();
	m_wind = Wind().GetGlobalWind();
	m_powerShotType = GolfClub().PowerShotType();
	m_shotItemTypeId = PLAYER(GOLFDOC()->m_currentPlayer)->item;
	m_calipers = GOLFDOC()->GetBar().calipers;
}

bool ShotPreview::UpdateSensor::Equal(const UpdateSensor& other) const
{
	return *this == other;
}

bool ShotPreview::UpdateSensor::operator==(const UpdateSensor& other) const
{
	int result = memcmp(this, &other, sizeof(UpdateSensor));
	return result == 0;
}

IMPLEMENT_ACTOR(ShotPreview, IActor)

ShotPreview::ShotPreview(void)
	: m_predictType((PredictType::Enum)0), m_visible(1)
{
	SetOptionFlag(10);
	m_updateElapsed = 0.0f;
	m_pBall = new CGolfBall;
	m_pSensor = new UpdateSensor;
	m_indicatorTexture = g_resrcmng->LoadTexture("{indicator.bmp", 0, 0, 0);

	m_dispPriority = DISP_PRIORITY_2;
	CSceneManager::Instance()->RegisterElement(OBJ_EFFECT, 0, this,
		"ShotPreview", 0, 0);
	m_pFont = new C3dFont;
	m_pFont->LoadTexture("[font_bar.jpg", 16, 16, "1234567890ym%-.:", 6.0f,
		-25.0f);
}

std::string ShotPreview::TwAddOption(const tweaker_param& param)
{
	unsigned int flag = GetOptionFlag();
	SetOptionFlag(flag | param[0].to_int());
	return std::string("TwAddOption Processed");
}

std::string ShotPreview::TwSubOption(const tweaker_param& param)
{
	unsigned int flag = GetOptionFlag();
	SetOptionFlag(flag & ~param[0].to_int());
	return std::string("TwSubOption Processed");
}

ShotPreview::~ShotPreview(void)
{
	std::for_each(m_pangCoins.begin(), m_pangCoins.end(), Delete_Object());
	m_pangCoins.clear();

	if (m_pFont)
	{
		delete m_pFont;
		m_pFont = 0;
	}

	if (m_pBall)
	{
		delete m_pBall;
		m_pBall = 0;
	}

	if (m_pSensor)
	{
		delete m_pSensor;
		m_pSensor = 0;
	}

	if (g_resrcmng && m_indicatorTexture)
	{
		g_resrcmng->Release(m_indicatorTexture);
		m_indicatorTexture = 0;
	}
}

bool ShotPreview::IsSkipState(void)
{
	if (!(GOLFDOC()->GameMode() & 0x3F8))
	{
		return true;
	}

	if (GolfClub().GetType() == 3)
	{
		return true;
	}

	return m_visible == 0;
}

void ShotPreview::OnInit(void)
{
	m_path.clear();
}

void ShotPreview::OnLoad(void)
{
	m_predictButtons.clear();

	FrButton* buttons[4] = {
		0,
	};
	SendMsg(this, "GolfRule", 0x24A, (int)"predict_normal", (int)&buttons[0], 0,
		0);
	SendMsg(this, "GolfRule", 0x24A, (int)"predict_tomahwak", (int)&buttons[1],
		0, 0);
	SendMsg(this, "GolfRule", 0x24A, (int)"predict_cobra", (int)&buttons[2], 0,
		0);
	SendMsg(this, "GolfRule", 0x24A, (int)"predict_spike", (int)&buttons[3], 0,
		0);

	for (int i = 0; i < 4; ++i)
	{
		if (buttons[i])
		{
			FrButton* pButton = DYNAMIC_CAST(FrButton, buttons[i]);
			if (pButton)
			{
				m_predictButtons.push_back(pButton);
			}
		}
	}
}

void ShotPreview::OnReload(void)
{
	OnLoad();
}

void ShotPreview::OnProcess(float delta)
{
	m_updateElapsed += delta;

	if (g_input->Get("LSHIFT", true) && g_input->GetDown("NUM_9", true))
	{
		PLAYER(GOLFDOC()->m_currentPlayer)->gauge =
			Between(0.0f, PLAYER(GOLFDOC()->m_currentPlayer)->gauge + 33.0f,
				Doc()->GetComboGaugeLimit(GOLFDOC()->m_currentPlayer));
	}

	if (g_input->Get("LSHIFT", true) && g_input->GetDown("REPLAY", true))
	{
		switch (GetOptionFlag())
		{
		case 10:
			SetOptionFlag(11);
			break;
		case 11:
			SetOptionFlag(0);
			break;
		case 0:
			SetOptionFlag(10);
			break;
		}

		std::for_each(m_pangCoins.begin(), m_pangCoins.end(),
			std::bind2nd(std::mem_fun(&PangCoin::SetVisible),
				((GetOptionFlag() >> 4) & 1) != 0));
	}

	if (GetOptionFlag() == 0)
	{
		std::for_each(m_predictButtons.begin(), m_predictButtons.end(),
			std::bind2nd(std::mem_fun(&FrWnd::SetVisible), false));
	}
	else
	{
		std::for_each(m_predictButtons.begin(), m_predictButtons.end(),
			std::bind2nd(std::mem_fun(&FrWnd::SetVisible), true));
	}

	if (m_updateElapsed > 0.5f)
	{
		m_updateElapsed = 0.0f;

		if (!IsSkipState())
		{
			UpdateSensor sensor;
			sensor.Update();
			if (!(*m_pSensor == sensor))
			{
				m_path.clear();
				m_path2.clear();

				int count = -1;
				Predict(m_path, m_path2, &count);

				std::for_each(m_pangCoins.begin(), m_pangCoins.end(),
					Delete_Object());
				m_pangCoins.clear();

				if (count != -1 && !m_path.empty() && (GetOptionFlag() & 0x10))
				{
					std::vector<WVector> coins;

					coins.assign(m_path.begin(), m_path.begin() + count);

					std::vector<WVector>::iterator it;
					while (coins.size() > 20)
					{
						int n = 0;
						for (it = coins.begin(); it != coins.end() - 1;)
						{
							if (++n & 1)
							{
								it = coins.erase(it);
							}
							else
							{
								++it;
							}
						}
					}

					for (it = coins.begin(); it != coins.end() - 1; ++it)
					{
						m_pangCoins.push_back(new PangCoin(*it, 3.0f, true));
					}

					m_pangCoins.push_back(new PangCoin(*it, 8.0f, false));

					std::for_each(m_pangCoins.begin(), m_pangCoins.end(),
						std::bind2nd(std::mem_fun(&PangCoin::SetVisible),
							((GetOptionFlag() >> 4) & 1) != 0));
				}

				m_pSensor->Update();
			}
		}
	}

	std::for_each(m_pangCoins.begin(), m_pangCoins.end(),
		std::bind2nd(std::mem_fun(&PangCoin::Update), delta));
}

namespace
{

	unsigned long GetGuideLineColor(const WVector& pos)
	{
		WVector ballPos = pos;
		WVector cupPos = GOLFDOC()->GetHoleData().pin;

		WVector d;
		d.x = cupPos.x - ballPos.x;
		d.z = cupPos.z - ballPos.z;

		return sqrtf(d.x * d.x + d.z * d.z) < GetPVS().GetHoleCupRadius()
			? 0xA80000FF
			: 0xA8FFFF00;
	}

	WVector GetProjectionVector(const WVector& v)
	{
		WVector pos = v;
		WTVertex vtx;
		vtx.x = pos.x;
		vtx.y = pos.y;
		vtx.z = pos.z;

		g_view->Projection2(&vtx, pos);

		pos.x = vtx.x;
		pos.y = vtx.y;
		pos.z = vtx.z;

		return pos;
	}

}

void ShotPreview::DrawIndicator(const WVector& pos, unsigned long color,
	float size)
{
	size *= 20.0f;

	WVector ground = GetPVS().GetGroundPoint(pos, false, 0, false);

	WTVertex* vl[5] = {
		0,
	};
	WTVertex screen;
	WTVertex vtx[4];

	g_view->Projection2(&screen, ground);
	if (screen.z <= 0.0f || screen.z >= 1.0f)
	{
		return;
	}

	vtx[0].x = screen.x - size * 0.5f;
	vtx[0].z = screen.z;
	vl[0] = &vtx[0];
	vtx[1].x = screen.x - size * 0.5f;
	vtx[1].z = vtx[0].z;
	vl[1] = &vtx[1];
	vtx[1].y = screen.y - size;
	vtx[2].x = size * 0.5f + screen.x;
	vtx[2].z = vtx[1].z;
	vl[2] = &vtx[2];
	vtx[2].y = screen.y - size;
	vtx[3].x = size * 0.5f + screen.x;
	vtx[3].z = vtx[2].z;
	vl[3] = &vtx[3];
	vtx[0].y = screen.y;
	vtx[0].rhw = 1.0f;
	vtx[0].diffuse = color;
	vtx[1].rhw = 1.0f;
	vtx[1].diffuse = color;
	vtx[2].rhw = 1.0f;
	vtx[2].diffuse = color;
	vtx[3].rhw = 1.0f;
	vtx[3].diffuse = color;

	vtx[0].tu = 0.0f;
	vtx[0].tv = 1.0f;
	vtx[1].tu = 0.0f;
	vtx[1].tv = 0.0f;
	vtx[2].tu = 1.0f;
	vtx[2].tv = 0.0f;
	vtx[3].tu = 1.0f;
	vtx[3].tv = 1.0f;
	vtx[3].y = vtx[0].y;

	if (g_view->GetWidth() - 0.5f < vtx[0].x || vtx[3].x < 0.5f ||
		vtx[0].y < 0.5f || g_view->GetHeight() - 0.5f < vtx[1].y)
	{
		return;
	}

	int flags = (m_indicatorTexture & 0x7FF) | 0x2400000;
	if (GOLFDOC()->GameMode() & 0x200)
	{
		flags |= 0x300000;
		m_pFont->SetProjectionStyle(C3dFont::ProjectionStyle::Screen);
	}
	else
	{
		m_pFont->SetProjectionStyle(C3dFont::ProjectionStyle::Perspective);
	}

	g_view->DrawPolygonFan(vl, flags, 0, true);

	if (!(GetOptionFlag() & 8))
	{
		return;
	}

	WVector ballPos = pos;
	WVector cupPos = GetHoleCupPos();

	WVector d(ballPos.x - cupPos.x, 0.0f, ballPos.z - cupPos.z);

	m_pFont->SetPos(ground);
	if (color == 0xA80000FF)
	{
		m_pFont->SetColor(0xFF80FFFF);
		m_pFont->Printf(g_view, -7, "0y");
	}
	else
	{
		m_pFont->SetColor(-1);
		float dist = d.Magnitude();
		float yard = dist * 0.3125f;
		m_pFont->Printf(g_view, -7, "%.1fy", yard);
	}
}

void ShotPreview::Display(void)
{
	if (IsSkipState() || m_path.size() == 0)
	{
		return;
	}

	std::for_each(m_pangCoins.begin(), m_pangCoins.end(),
		std::mem_fun(&PangCoin::Render));

	int count = m_path.size();

	unsigned long color = GetGuideLineColor(m_path.back());

	if (m_path2.size())
	{
		if (m_path2.size() > 2)
		{
			m_path2.erase(m_path2.begin() + 1, m_path2.end() - 1);

			if (GetOptionFlag() & 4)
			{
				WVector& marker = m_path2[1];
				DrawIndicator(marker, color, 0.8f);
			}

			if (GetOptionFlag() & 2)
			{
				DrawIndicator(*m_path2.begin(), color, 0.8f);
			}
		}
		else if (GetOptionFlag() & 6)
		{
			DrawIndicator(*m_path2.begin(), color, 0.8f);
		}
	}

	if (!(GetOptionFlag() & 1))
	{
		return;
	}

	if (GOLFDOC()->GameMode() & 0x200)
	{
		for (int i = 0; i < count - 1; ++i)
		{
			WVector a = GetPVS().GetGroundPoint(m_path[i], false, 0, false);
			WVector b = GetPVS().GetGroundPoint(m_path[i + 1], false, 0, false);

			g_view->DrawLine2D(a, b, 0xA8FFFF00, 1.0f);
		}
	}
	else
	{
		WTVertex vtx[4];
		WTVertex* vl[5] = { &vtx[0], &vtx[1], &vtx[2], &vtx[3], 0 };
		vtx[0].diffuse = color;
		vtx[1].diffuse = color;
		vtx[2].diffuse = color;
		vtx[3].diffuse = color;

		for (int i = 1; i < count; ++i)
		{
			WVector& prev = m_path[i - 1];
			WVector& cur = m_path[i];

			WVector side;
			side = WCrossProduct(cur - prev, WVector(0.0f, 1.0f, 0.0f));
			side.Normalize();
			side *= GolfBall().m_radius;

			WVector a;
			a = prev - side;
			WVector b;
			b = cur - side;
			WVector c;
			c = cur + side;
			WVector d;
			d = prev + side;

			vtx[0].x = a.x;
			vtx[0].y = a.y;
			vtx[0].z = a.z;
			vtx[1].x = b.x;
			vtx[1].y = b.y;
			vtx[1].z = b.z;
			vtx[2].x = c.x;
			vtx[2].y = c.y;
			vtx[2].z = c.z;
			vtx[3].x = d.x;
			vtx[3].y = d.y;
			vtx[3].z = d.z;

			g_view->DrawPolygonFan(vl, 0xC00000, 0x400, false);
		}
	}
}

void ShotPreview::HandleMsg(const MsgObject& msg)
{
	if (msg.message != -1)
	{
		return;
	}

	std::string name((const char*)msg.param1);
	if (name == "GET_PREDICTED_FIRSTBOUND_POS")
	{
		if (m_path2.size())
		{
			*(int*)msg.param2 = 1;
			*(WVector*)msg.param3 = m_path2[0];
		}
		else
		{
			*(int*)msg.param2 = 0;
		}
	}

	if (name == "GET_PREDICT_POSITIONS")
	{
		std::vector<WVector>* positions = (std::vector<WVector>*)msg.param2;
		positions->assign(m_path.begin(), m_path.end());
	}
	else if (name == "CHANGE_PREDICT_TYPE")
	{
		PredictType::Enum type = (PredictType::Enum)msg.param2;
		if (type != m_predictType)
		{
			m_predictType = type;
			m_path.clear();
			m_path2.clear();
			Predict(m_path, m_path2, 0);
			m_pSensor->Update();
			m_updateElapsed = 0.0f;
		}
	}
	else if (name == "ENABLE_DISPLAY")
	{
		m_visible = msg.param2;
	}
	else if (name == "PM_REPLAY")
	{
		std::for_each(m_pangCoins.begin(), m_pangCoins.end(),
			std::mem_fun(&PangCoin::Restore));
	}
}

unsigned short ShotPreview::GetBallInitSpecialValueFromPredictType(
	PredictType::Enum type)
{
	if (type == 1)
	{
		return 0x10;
	}

	if (type == 2)
	{
		return 0x20;
	}

	return type == 3 ? 0x40 : 0;
}

bool ShotPreview::Predict(std::vector<WVector>& path,
	std::vector<WVector>& bounces, int* count)
{
	float rate = (GOLFDOC()->GetBar().calipers - s_barStart) / s_powerBarWidth;

	if (Wabs(rate) < g_EPSILON)
	{
		rate = 1.0f;
	}

	float power = GolfClub().GetPower();
	power *= sqrtf(rate);

	CClub& club = GolfClub();

	m_pBall->Reset(WVector());
	*m_pBall = GolfBall();

	CGolfBall& ball = GolfBall();
	ball.Reset(ball.m_pos);

	float spot;
	SendMsg(this, "Spot", 0xFA, (int)&spot, 0, 0, 0);
	float angle;
	SendMsg(this, "Player", 0x110, (int)&angle, 0, 0, 0);
	angle += spot;

	CGolfBall::sExpected* first = ball.PreDataToWrite(0);
	first->pos = ball.m_pos;
	first->event = 0;
	first->state = CGolfBall::BALL_BOUNCE;

	power *= sqrtf(GetPVS().GetMaxPowerOnGround());

	ball.m_initSpecial = GetBallInitSpecialValueFromPredictType(m_predictType);

	if (ball.m_initSpecial == 0x40 || ball.m_initSpecial == 0x10)
	{
		power *= 1.3f;
	}

	SendMsg(this, "Spin", 0x161, (int)&ball.m_initCurve, (int)&ball.m_initSpin,
		0, 0);

	float spinAngle = -(ball.m_initCurve * (20 * g_DEGTORAD));

	if (PLAYER(GOLFDOC()->m_currentPlayer)->item != 0x18000028)
	{
		WVector screen;
		SendMsg(this, "Screen", 0x1C1, (int)&screen, 0, 0, 0);

		if (ball.m_initSpecial & 0x40)
		{
			ball.m_initCurve -= screen.x * 0.15f;
		}
		else
		{
			ball.m_initCurve -= screen.x * 0.5f;
		}
	}

	WVector velocity;
	velocity = (Abs(ball.m_initCurve) * 0.1f + 1.0f) * power *
		RotMat(GolfClub().GetLie(), 0).za * RotMat(spinAngle + angle, 1);
	first->vel = velocity;

	float frameOffset;
	SendMsg(this, "Player", 0x126, (int)&frameOffset, 0, 0, 0);
	unsigned long frame = GetPVS().GetFrame();
	GetPVS().SetFrame(frame - (int)(frameOffset * -50.0f) + 50);

	ball.m_vel = first->vel;
	ball.m_state = first->state;
	ball.m_special = ball.m_initSpecial;
	ball.m_spin = 0.0f;
	ball.m_tomahawkFrame = -1;
	GetPVS().ResetVariable();

	unsigned char groundType = ball.m_groundType;

	if (GolfClub().GetApproachMode() == 4)
	{
		float aux =
			Doc()->GetAuxPartProperty(GOLFDOC()->m_currentPlayer, 0) * 0.5f;
		if (IsLocalContent((localContentType_t)0x70))
		{
			float range =
				(float)Doc()->GetBuffValue(GOLFDOC()->m_currentPlayer, 0x10);
			if (GolfClub().PowerShotType())
			{
				range += (float)Doc()->GetBuffValue(GOLFDOC()->m_currentPlayer,
					0x11);
			}
			aux += range;
		}
		else
		{
			CCardManager::Instance()->SetPlayerIndex(
				GOLFDOC()->m_currentPlayer);
			float range = CCardManager::Instance()->GetCardRangeUp();
			if (GolfClub().PowerShotType())
			{
				range += CCardManager::Instance()->GetCardPowerRangeUp();
			}
			aux += range;
		}
		float bonus = ((int)PLAYER(GOLFDOC()->m_currentPlayer)->power + aux) *
			(1.0f / 170.0f);
		bonus = Max(0.0f, bonus);
		ball.m_spin = (bonus + 1.5f) * rate * rate;
	}

	ball.m_curve = rate * ball.m_initCurve;
	while (ball.m_state)
	{
		CGolfBall::sExpected* pre = ball.PreDataToWrite(++ball.m_renderNum);

		pre->sound[0] = 0;
		pre->event = 0;
		pre->effect = 0;
		GetPVS().PreProcess(0.02f, true);
		GetPVS().BallProcess(0.02f);

		if (GolfClub().GetApproachMode() == 4)
		{
			if (ball.m_special & 0x20)
			{
				if (ball.m_cobraFrame < 0)
				{
					ball.m_pos.y = Max(ball.m_pos.y,
						GetPVS().GetBasePosY(ball.m_pos, ball.m_radius * 2.0f));
				}
				else
				{
					ball.m_spin -= 0.02f;
				}
			}
			else if (ball.m_special & 0x40)
			{
				if (ball.m_spikeFrame < 0)
				{
					ball.m_pos.y = Max(ball.m_pos.y,
						GetPVS().GetBasePosY(ball.m_pos, ball.m_radius * 2.0f));
				}
				else
				{
					ball.m_spin -= 0.02f;
				}
			}
			else
			{
				if (ball.m_bounceFrame < 0 && GetPVS().GetObjColNum() == 0)
				{
					ball.m_spin -= (0.5f - ball.m_initSpin * 0.1f) * 0.02f;
				}
				else
				{
					ball.m_spin -= 0.002f;
				}
			}
		}

		if (ball.m_renderNum >= 3000 ||
			(ball.m_renderNum >= 1500 &&
				(WVectorLen(ball.m_vel) < 0.1f ||
					GetPVS().GetObjColNum() >= 10 ||
					GetPVS().GetMeshIndex(ball.m_pos, false) == -1)))
		{
			ball.m_vel.Reset();
			ball.m_pos = GetPVS().GetGroundPoint(ball.m_pos, false, 0, 0);
			ball.m_pos.y += ball.m_radius;
			ball.m_state = CGolfBall::BALL_STOP;
		}

		pre->pos = ball.m_pos;
		pre->vel = ball.m_vel;
		pre->rot.x = ball.m_rot.x;
		pre->rot.y = ball.m_rot.y;
		pre->state = ball.m_state;

		if (ball.m_state == 0)
		{
			ball.m_special &= ~0x70;
		}

		if (ball.m_state == 1 && ball.m_vel.y < 0.0f && ball.m_topFrame < 0)
		{
			ball.m_topFrame = ball.m_renderNum;
		}

		if (ball.m_tomahawkFrame < 0 && (ball.m_initSpecial & 0x10) &&
			!(ball.m_special & 0x10))
		{
			ball.m_tomahawkFrame = ball.m_renderNum;
		}

		if (ball.m_special & 0x20)
		{
			if (ball.m_renderNum == 1)
			{
				ball.m_vel.y = 0.0f;
				ball.m_vel = power * ball.m_vel.Normalize();
			}

			const CGolfBall::sExpected* start = ball.PreData(0);
			float dx = ball.m_pos.x - start->pos.x;
			float dz = ball.m_pos.z - start->pos.z;
			if (ball.m_cobraFrame < 0)
			{
				float dist = sqrtf(dz * dz + dx * dx);
				if (dist >
					(GolfClub().GetRange() * sqrtf(rate) - 100.0f) * 3.2f)
				{
					ball.m_cobraFrame = ball.m_renderNum;
					ball.m_vel.Normalize();

					switch (GolfClub().GetID())
					{
					case 0:
						ball.m_vel *= 74.0f;
						break;
					case 1:
						ball.m_vel *= 76.0f;
						break;
					case 2:
						ball.m_vel *= 80.0f;
						break;
					}

					ball.m_vel *= sqrtf(rate);
					ball.m_spin = 2.5f;
				}
			}
		}

		if (ball.m_spikeEndFrame < 0 && (ball.m_initSpecial & 0x40) &&
			!(ball.m_special & 0x40) && ball.m_spikeDiveFrame > 0)
		{
			ball.m_spikeEndFrame = ball.m_renderNum;
		}

		if (ball.m_special & 0x40)
		{
			if (ball.m_renderNum == 1)
			{
				ball.m_vel.y = 0.0f;
				ball.m_vel = power * ball.m_vel.Normalize();
			}

			const CGolfBall::sExpected* start = ball.PreData(0);
			float dx = ball.m_pos.x - start->pos.x;
			float dz = ball.m_pos.z - start->pos.z;
			if (ball.m_spikeFrame < 0)
			{
				if (sqrtf(dz * dz + dx * dx) > 0.0f)
				{
					ball.m_spikeFrame = ball.m_renderNum;
					ball.m_vel.Normalize();
					ball.m_vel *= 72.5f;
					ball.m_vel *= sqrtf(rate) * 2.0f;
					ball.m_spin = 3.1f;
				}
			}

			if (ball.m_topFrame >= 0 &&
				ball.m_renderNum > ball.m_topFrame + 60 &&
				ball.m_spikeDiveFrame < 0)
			{
				ball.m_spikeDiveFrame = ball.m_renderNum;

				switch (GolfClub().GetID())
				{
				case 0:
					ball.m_vel.y = -8.0f -
						(344.0f -
							min(GolfClub().GetPower() * sqrtf(rate), 344.0f)) *
							0.191964284f;
					break;
				case 1:
					ball.m_vel.y = -10.2999992f -
						(306.0f -
							min(GolfClub().GetPower() * sqrtf(rate), 306.0f)) *
							0.195238099f;
					break;
				case 2:
					ball.m_vel.y = -10.7999992f -
						(273.0f -
							min(GolfClub().GetPower() * sqrtf(rate), 273.0f)) *
							0.202000007f;
					break;
				}

				ball.m_vel *= sqrtf(rate) * 7.0f;
				ball.m_spin = ball.m_initSpin;
			}
		}
	}

	ball.m_groundType = groundType;
	ball.m_special = ball.m_initSpecial;

	bool firstBounce = true;
	for (int i = 0; i < ball.m_renderNum; ++i)
	{
		path.push_back(ball.PreData(i)->pos);
		if (ball.PreData(i)->event > 0)
		{
			bounces.push_back(ball.PreData(i)->pos);
			if (firstBounce)
			{
				firstBounce = false;
				if (count)
				{
					*count = i;
				}
			}
		}
	}

	path.push_back(ball.PreData(ball.m_renderNum)->pos);
	bounces.push_back(ball.PreData(ball.m_renderNum)->pos);

	if (count && firstBounce)
	{
		*count = path.size() - 1;
	}

	ball.Reset(WVector());

	GolfBall() = *m_pBall;
	GetPVS().SetFrame(frame - 1);
	GetPVS().PreProcess(0.02f, true);

	return true;
}
