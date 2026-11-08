#include "skinnedtheme.h"
#include "frgroupbox.h"
#include "frarea.h"

class CSkinnedThemeBase : public _ISkinnedTheme
{
public:
	virtual const char* GetSkinedThemeClassName()
	{
		return "CSkinnedThemeBase";
	}

	virtual const char* GetWindOutterCircleImgName()
	{
		return "[right_wind_b.jpg";
	}
	virtual const char* GetWindInnerCircleImgName()
	{
		return "[right_wind_s.jpg";
	}

	virtual const char* GetRoomListBackGroundImgName()
	{
		return "roomlist_background.jpg";
	}

	virtual void GetOverBarImgName(THEME_OVERBAR& overbar)
	{
		strncpy(overbar.bar[0], "over_bar_01", sizeof(overbar.bar[0]) - 1);
		strncpy(overbar.bar[1], "over_bar_02", sizeof(overbar.bar[1]) - 1);
		strncpy(overbar.bar[2], "over_bar_03", sizeof(overbar.bar[2]) - 1);
	}

	virtual void GetOverBarFrameImgName(THEME_OVERBAR_FRAME& frame)
	{
		strncpy(frame.frame[0], "over_frame_1", sizeof(frame.frame[0]) - 1);
		strncpy(frame.frame[1], "over_frame_2", sizeof(frame.frame[1]) - 1);
		strncpy(frame.frame[2], "over_frame_3", sizeof(frame.frame[2]) - 1);
	}

	virtual void GetMakeRoomButtonImgName(THEME_BUTTONS& buttons)
	{
		strncpy(buttons.disable, "btn_makeroom_d", sizeof(buttons.disable) - 1);
		strncpy(buttons.normal, "btn_makeroom_n", sizeof(buttons.normal) - 1);
		strncpy(buttons.over, "btn_makeroom_o", sizeof(buttons.over) - 1);
		strncpy(buttons.press, "btn_makeroom_d", sizeof(buttons.press) - 1);
	}

	virtual void GetQuickStartButtonImgName(THEME_BUTTONS& buttons)
	{
		strncpy(buttons.disable, "btn_ingameguild_d",
			sizeof(buttons.disable) - 1);
		strncpy(buttons.normal, "btn_ingameguild_n",
			sizeof(buttons.normal) - 1);
		strncpy(buttons.over, "btn_ingameguild_o", sizeof(buttons.over) - 1);
		strncpy(buttons.press, "btn_ingameguild_d", sizeof(buttons.press) - 1);
	}
};

class CClassicTheme : public _ISkinnedTheme
{
public:
	virtual const char* GetSkinedThemeClassName() { return "CClassicTheme"; }

	virtual const char* GetWindOutterCircleImgName()
	{
		return "[classic_right_wind_b.jpg";
	}
	virtual const char* GetWindInnerCircleImgName()
	{
		return "[classic_right_wind_s.jpg";
	}

	virtual const char* GetRoomListBackGroundImgName()
	{
		return "classic_roomlist_background.jpg";
	}

	virtual void GetOverBarImgName(THEME_OVERBAR& overbar)
	{
		strncpy(overbar.bar[0], "classic_over_bar_01",
			sizeof(overbar.bar[0]) - 1);
		strncpy(overbar.bar[1], "classic_over_bar_02",
			sizeof(overbar.bar[1]) - 1);
		strncpy(overbar.bar[2], "classic_over_bar_03",
			sizeof(overbar.bar[2]) - 1);
	}

	virtual void GetOverBarFrameImgName(THEME_OVERBAR_FRAME& frame)
	{
		strncpy(frame.frame[0], "classic_over_frame_1",
			sizeof(frame.frame[0]) - 1);
		strncpy(frame.frame[1], "over_frame_2", sizeof(frame.frame[1]) - 1);
		strncpy(frame.frame[2], "classic_over_frame_3",
			sizeof(frame.frame[2]) - 1);
	}

	virtual void GetMakeRoomButtonImgName(THEME_BUTTONS& buttons)
	{
		strncpy(buttons.disable, "classic_btn_makeroom_d",
			sizeof(buttons.disable) - 1);
		strncpy(buttons.normal, "classic_btn_makeroom_n",
			sizeof(buttons.normal) - 1);
		strncpy(buttons.over, "classic_btn_makeroom_o",
			sizeof(buttons.over) - 1);
		strncpy(buttons.press, "classic_btn_makeroom_d",
			sizeof(buttons.press) - 1);
	}
	virtual void GetQuickStartButtonImgName(THEME_BUTTONS& buttons)
	{
		strncpy(buttons.disable, "btn_ingameguild_ntr_d",
			sizeof(buttons.disable) - 1);
		strncpy(buttons.normal, "btn_ingameguild_ntr_n",
			sizeof(buttons.normal) - 1);
		strncpy(buttons.over, "btn_ingameguild_ntr_o",
			sizeof(buttons.over) - 1);
		strncpy(buttons.press, "btn_ingameguild_ntr_d",
			sizeof(buttons.press) - 1);
	}
};

class CSkinnedThemeName
{
public:
	static CSkinnedThemeName* Instance()

	{
		static CSkinnedThemeName _inst;

		return &_inst;
	}

	void SetSkinedThemeName(const char* name) { m_name = name; }
	const std::string& GetSkinnedThemeName() const { return m_name; }

private:
	CSkinnedThemeName()
		: m_name("CSkinnedThemeBase")
	{
	}

	std::string m_name;
};

namespace S5
{
	namespace THEME
	{
		void GetServerSelectButton(unsigned long flag, THEME_BUTTONS& buttons)

		{
			if (flag & 0x80)
			{
				strncpy(buttons.disable, "classic_server_select_btn_d",
					sizeof(buttons.disable) - 1);
				strncpy(buttons.normal, "classic_server_select_btn_n",
					sizeof(buttons.normal) - 1);
				strncpy(buttons.over, "classic_server_select_btn_o",
					sizeof(buttons.over) - 1);
				strncpy(buttons.press, "classic_server_select_btn_d",
					sizeof(buttons.press) - 1);
			}
			else
			{
				strncpy(buttons.disable, "server_select_btn_d",
					sizeof(buttons.disable) - 1);
				strncpy(buttons.normal, "server_select_btn_n",
					sizeof(buttons.normal) - 1);
				strncpy(buttons.over, "server_select_btn_o",
					sizeof(buttons.over) - 1);
				strncpy(buttons.press, "server_select_btn_d",
					sizeof(buttons.press) - 1);
			}
		}

		void GetChannelSelectButton(unsigned long flag, THEME_BUTTONS& buttons)
		{
			if (flag & 0x80)
			{
				strncpy(buttons.disable, "classic_channel_select_btn_d",
					sizeof(buttons.disable) - 1);
				strncpy(buttons.normal, "classic_channel_select_btn_n",
					sizeof(buttons.normal) - 1);
				strncpy(buttons.over, "classic_channel_select_btn_o",
					sizeof(buttons.over) - 1);
				strncpy(buttons.press, "classic_channel_select_btn_p",
					sizeof(buttons.press) - 1);
			}
			else
			{
				strncpy(buttons.disable, "channel_select_btn_d",
					sizeof(buttons.disable) - 1);
				strncpy(buttons.normal, "channel_select_btn_n",
					sizeof(buttons.normal) - 1);
				strncpy(buttons.over, "channel_select_btn_o",
					sizeof(buttons.over) - 1);
				strncpy(buttons.press, "channel_select_btn_p",
					sizeof(buttons.press) - 1);
			}
		}

		static CSkinnedThemeBase s_skinnedThemeBase;
		static CClassicTheme s_classicTheme;

		void ChangeGroupBoxChildAreaBgImg(FrGroupBox* pGroupBox,
			const char* areaName, const char* imgName)

		{
			if (!pGroupBox || !areaName || !imgName)
				return;

			FrWnd* pWnd = pGroupBox->FindChildByName(areaName);
			FrArea* pArea = DYNAMIC_CAST(FrArea, pWnd);
			if (pArea)
				pArea->SetBgImg(imgName);
		}

		void SetSkinnedTheme(const char* name)

		{
			CSkinnedThemeName::Instance()->SetSkinedThemeName(name);
		}

		void SetSkinnedTheme(unsigned long flag)
		{
			if (flag & 0x80)
				CSkinnedThemeName::Instance()->SetSkinedThemeName(
					"CClassicTheme");
			else

				CSkinnedThemeName::Instance()->SetSkinedThemeName(
					"CSkinnedThemeBase");
		}

		_ISkinnedTheme* GetCurrentSkinnedTheme()
		{
			const std::string& name =
				CSkinnedThemeName::Instance()->GetSkinnedThemeName();
			if (name == "CSkinnedThemeBase")
				return &s_skinnedThemeBase;

			if (name == "CClassicTheme")
				return &s_classicTheme;

			return &s_skinnedThemeBase;
		}

		void ChangeOverbarTheme(_ISkinnedTheme* pTheme, FrGroupBox* pGroupBox)
		{
			if (!pGroupBox)
				return;

			std::string name;
			pGroupBox->GetWindowName(name);
			if (name != "obar")
				return;

			THEME_OVERBAR overbar;
			pTheme->GetOverBarImgName(overbar);
			THEME_OVERBAR_FRAME frame;
			pTheme->GetOverBarFrameImgName(frame);

			ChangeGroupBoxChildAreaBgImg(pGroupBox, "bar1", overbar.bar[0]);
			ChangeGroupBoxChildAreaBgImg(pGroupBox, "bar2", overbar.bar[1]);
			ChangeGroupBoxChildAreaBgImg(pGroupBox, "bar3", overbar.bar[2]);

			ChangeGroupBoxChildAreaBgImg(pGroupBox, "frame_1", frame.frame[0]);
			ChangeGroupBoxChildAreaBgImg(pGroupBox, "frame_2", frame.frame[1]);
			ChangeGroupBoxChildAreaBgImg(pGroupBox, "frame_3", frame.frame[2]);
		}
	}
}
