#include "christamseventdlg.h"
#include "wresrcmng.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrChristmasEventDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrChristmasEventDlg, FrForm)

ON_FRESH_VI("close", FRCMD_INIT, FrChristmasEventDlg::OnCloseBtnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrChristmasEventDlg::OnCloseBtnUp)
ON_FRESH_VI("caption", FRCMD_INIT, FrChristmasEventDlg::OnCaptionInit)
ON_FRESH_VI("xmas_btn1", FRCMD_INIT, FrChristmasEventDlg::OnXmasBtn1Init)
ON_FRESH_VI("xmas_btn2", FRCMD_INIT, FrChristmasEventDlg::OnXmasBtn2Init)
ON_FRESH_VI("xmas_btn3", FRCMD_INIT, FrChristmasEventDlg::OnXmasBtn3Init)
ON_FRESH_VI("xmas_ex1", FRCMD_INIT, FrChristmasEventDlg::OnXmasEx1Init)
ON_FRESH_VI("xmas_ex2", FRCMD_INIT, FrChristmasEventDlg::OnXmasEx2Init)
ON_FRESH_VI("xmas_ex3", FRCMD_INIT, FrChristmasEventDlg::OnXmasEx3Init)
ON_FRESH_VI("xmas_socks1", FRCMD_INIT, FrChristmasEventDlg::OnSocks1AreaInit)
ON_FRESH_VI("xmas_socks2", FRCMD_INIT, FrChristmasEventDlg::OnSocks2AreaInit)
ON_FRESH_VV("snow_falling", FRCMD_OWNERDRAW,
	FrChristmasEventDlg::OnSnowFallDraw)

END_FRESH_MSGMAP()

namespace
{
	class cSnow
	{
	public:
		cSnow() { m_bAlive = true; }
		~cSnow() { }
		static void SetArea(const WRect& rc) { rcArea = rc; }
		const Bitmap* Img() const { return m_pImg; }
		WRect GetRect() const
		{
			return WRect(m_pos.x, m_pos.y, m_pImg->Width(), m_pImg->Height());
		}
		unsigned long Color() const
		{
			return ((unsigned char)m_alpha << 24) | 0x00ffffff;
		}
		void Create();
		void InitialCreate();
		void Process(float delta)
		{
			m_pos += m_dir * m_speed * delta;
			if (m_bBrighten)
			{
				m_alpha += delta * 64;
				if (m_alpha > 255)
				{
					m_alpha = 255;
					m_bBrighten = false;
				}
			}
			else
			{
				m_alpha -= delta * 64;
				if (m_alpha < 128)
				{
					m_alpha = 128;
					m_bBrighten = true;
				}
			}
		}
		bool IsAlive()
		{
			if (m_pos.x - m_pImg->Width() < rcArea.x ||
				m_pos.x + m_pImg->Width() > rcArea.x + rcArea.w ||
				m_pos.y + m_pImg->Height() + 30 > rcArea.y + rcArea.h)
				return false;
			return true;
		}

	private:
		static WRect rcArea;
		WVector2D m_pos;
		WVector2D m_dir;
		const Bitmap* m_pImg;
		float m_alpha;
		bool m_bBrighten;
		float m_speed;
		bool m_bAlive;
	};

	void cSnow::Create()
	{
		int left = (int)rcArea.x;
		int right = (int)(rcArea.x + rcArea.w);
		m_pos.x = (float)(left + rand() % right);
		m_pos.y = rcArea.y + 30;
		m_dir.x = (float)(rand() % 10 - 5);
		m_dir.y = (float)(rand() % 50 + 1);
		m_dir.Normalize();
		m_speed = (float)(rand() % 100 + 10);
		int choice = rand() % 100;
		if (choice < 20)
			m_pImg = g_pFresh->GetBitmap("snow1");
		else if (choice < 60)
			m_pImg = g_pFresh->GetBitmap("snow2");
		else
			m_pImg = g_pFresh->GetBitmap("snow3");
		m_alpha = (float)(rand() % 255 + 128);
		m_bBrighten = rand() % 2 ? true : false;
	}

	void cSnow::InitialCreate()
	{
		Create();
		int top = (int)rcArea.y + 30;
		int bottom = (int)(rcArea.y + rcArea.h);
		m_pos.y = (float)(top + rand() % bottom);
	}

	WRect cSnow::rcArea;
	std::vector<cSnow*> snow;
}

FrChristmasEventDlg::FrChristmasEventDlg()
{
	char* textures[1];
	textures[0] = "[font_hole_skins_red.jpg";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_titleFontInfo.filename = textures;
	m_titleFontInfo.flag = 0;
	m_titleFontInfo.fonth = 16;
	m_titleFontInfo.fontw = 16;
	m_titleFontInfo.numPages = 1;
	m_titleFontInfo.texw = 128;
	m_titleFontInfo.texh = 32;
	m_titleFontInfo.pCharSet = "1234567890-,";
	m_pTitleFont->Create(&m_titleFontInfo);
	for (int i = 0; i < 3; ++i)
	{
		m_pXmasBtn[i] = NULL;
		m_pXmasEx[i] = NULL;
	}
	snow.clear();
}

FrChristmasEventDlg::~FrChristmasEventDlg()
{
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
	std::for_each(snow.begin(), snow.end(), Delete_Object());
	snow.clear();
}

bool FrChristmasEventDlg::OnInit()
{
	FrForm::OnInit();
	EnableDrag(false);
	m_bXmasExShown[0] = false;
	m_bXmasExShown[1] = false;
	m_bXmasExShown[2] = false;
	cSnow::SetArea(GetRect());
	for (int i = 0; i < 100; ++i)
	{
		cSnow* flake = new cSnow;
		flake->InitialCreate();
		snow.push_back(flake);
	}
	return true;
}

void FrChristmasEventDlg::OnCloseBtnInit(int param)
{
	DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrChristmasEventDlg::OnCloseBtnUp()
{
	OnFreshOkay();
}

void FrChristmasEventDlg::OnCaptionInit(int param)
{
	DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrChristmasEventDlg::OnXmasBtn1Init(int param)
{
	m_pXmasBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrChristmasEventDlg::OnXmasBtn2Init(int param)
{
	m_pXmasBtn[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrChristmasEventDlg::OnXmasBtn3Init(int param)
{
	m_pXmasBtn[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrChristmasEventDlg::OnXmasEx1Init(int param)
{
	m_pXmasEx[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pXmasEx[0])
		m_pXmasEx[0]->SetVisible(false);
}

void FrChristmasEventDlg::OnXmasEx2Init(int param)
{
	m_pXmasEx[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pXmasEx[1])
		m_pXmasEx[1]->SetVisible(false);
}

void FrChristmasEventDlg::OnXmasEx3Init(int param)
{
	m_pXmasEx[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pXmasEx[2])
		m_pXmasEx[2]->SetVisible(false);
}

void FrChristmasEventDlg::OnSocks1AreaInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (area)
		area->SetVisible(false);
}

void FrChristmasEventDlg::OnSocks2AreaInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (area)
		area->SetVisible(false);
}

void FrChristmasEventDlg::OnProc(const float delta)
{
	if (m_pXmasBtn[0]->GetStatus() == 1)
	{
		if (!m_bXmasExShown[0])
		{
			m_bXmasExShown[0] = true;
			m_pXmasEx[0]->SetVisible(true);
		}
	}
	else if (m_bXmasExShown[0] == true)
	{
		m_bXmasExShown[0] = false;
		m_pXmasEx[0]->SetVisible(false);
	}
	if (m_pXmasBtn[1]->GetStatus() == 1)
	{
		if (!m_bXmasExShown[1])
		{
			m_bXmasExShown[1] = true;
			m_pXmasEx[1]->SetVisible(true);
		}
	}
	else if (m_bXmasExShown[1] == true)
	{
		m_bXmasExShown[1] = false;
		m_pXmasEx[1]->SetVisible(false);
	}
	if (m_pXmasBtn[2]->GetStatus() == 1)
	{
		if (!m_bXmasExShown[2])
		{
			m_bXmasExShown[2] = true;
			m_pXmasEx[2]->SetVisible(true);
		}
	}
	else if (m_bXmasExShown[2] == true)
	{
		m_bXmasExShown[2] = false;
		m_pXmasEx[2]->SetVisible(false);
	}
	if (snow.size() < 100)
	{
		cSnow* flake = new cSnow;
		flake->Create();
		snow.push_back(flake);
	}
	std::vector<cSnow*>::iterator it = snow.begin();
	while (it != snow.end())
	{
		cSnow* flake = *it;
		flake->Process(delta);
		if (!flake->IsAlive())
		{
			delete flake;
			it = snow.erase(it);
		}
		else
			++it;
	}
}

void FrChristmasEventDlg::OnDraw()
{
}

void FrChristmasEventDlg::OnSnowFallDraw()
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	for (std::vector<cSnow*>::iterator it = snow.begin(); it != snow.end();
		++it)
	{
		cSnow* flake = *it;
		gdi->DrawTexture(flake->Img(), flake->GetRect(), flake->Color(), 0);
	}
}
