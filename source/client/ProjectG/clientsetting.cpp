#include "stdafx.h"
#include "mathconsts.h"

#include "networksystem.h"
#include "clientsetting.h"
#include "actor.h"
#include "inputmanager.h"
#include "packet.h"
#include "soundmanager.h"
#include "shadow.h"
#include "wlocalize.h"
#include "wutil.h"
#include "wvideo.h"
#include "wdevmng.h"
#include "wview.h"
#include "shareddoc.h"
#include "taskmanager.h"
#include "golftask.h"
#include "frwnd.h"
#include "projectg.h"
#include "browser.h"
#include "chatmsg.h"
#include "scenemanager.h"
#include "polysoup.h"
#include "golfdoc.h"
#include "frform.h"
#include "fresh.h"
#include "frdesktop.h"
#include "frwndmanager.h"

#include <string>
#include <stdio.h>
#include <string.h>

extern Fresh* g_pFresh;

extern char PY_PUBLIC_VERSION[];

bool sOption::sbClipPlaneSupport;
bool sOption::sbMrtSupport;
bool sOption::sbPsSupport;
bool sOption::sbVsSupport;
bool sOption::sbRtSupport;
bool sOption::sbHwTnlSupport;

bool sOption::IsSupportShadow(void)
{
	return sbRtSupport;
}

bool sOption::IsSupportReflection(void)
{
	return sbRtSupport && sbClipPlaneSupport && sbVsSupport && sbPsSupport;
}

bool sOption::IsSupportLod(void)
{
	return sbRtSupport;
}

sOption::sOption()
{
	vWindowMode = 0;
	vFillMode = 0;
	vWideMode = 0;
	vOverall = -1;
	vUseMinScreen = 0;
	vMinScreenWidth = 800;
	vMinScreenHeight = 600;
	vScreenWidth = 800;
	vScreenHeight = 600;
	vScreenColor = 16;
	vTnLMode = 1;
	vDDSRes = 0;
	vUseMipmap = 0;
	vMaxMipLvl = 1;
	vMipCreateFilter = 2;
	vEffectLevel = 1;
	vProjectionShadow = 1;
	vShadowmap = 1;
	vReflection = 1;
	vLod = 0;
	vNpc = 1;
	vGamma = 1.3f;
	aOverall = 0;
	aMssOn = 1;
	aMssFreq = 44100;
	aMssBits = 16;
	aMssChannels = 2;
	aMssEnableHwSound = 1;
	aMssBalance = 128;
	aMssSpeaker = 0;
	aSfxVolume = 1.0f;
	aBGMVolume = 1.0f;
	gMouseSensitivity = 1.0f;
	gWhisper = 1;
	gInvitation = 1;
	gFriendConfirm = 1;
	gTransChatWin = 1;
	gUiEffect = 1;
	gLastLoginID[0] = 0;
	gIdentity = 0;
	gExtendChatLine = 0;
	gUserSort = 0;
	gPPL_Enable = 1;
	gPPL_Size = 0;
	gPowerGauge = 1;
	gCaptureLogo = 1;
	gRestore = 0;
	gChatWnd = 0;
	gUnderTabBtn = 0;
	gAvatarNewbie = 1;
	gCaptureHideGUI = 0;
	gCaptureHidePI = 0;
	gTerrainTooltip = 1;
	gCutinDisplay = 1;

	for (int i = 0; i < 8; i++)
	{
		mMacro[i][0] = 0;
	}
	mSaveMacros = 1;
}

sOption::~sOption()
{
}

void sOption::vResetByOverall(const char* text)
{
	if (strcmp(text, vGetOverallText(0)) == 0)
	{
		vOverall = 0;
		vUseMinScreen = 0;
		vScreenWidth = 1024;
		vScreenHeight = 768;
		vScreenColor = 32;
		vDDSRes = 0;
		vEffectLevel = 0;
		vProjectionShadow = IsSupportShadow() ? 1 : 0;
		vShadowmap = 1;
		vReflection = IsSupportReflection() ? 1 : 0;
		vLod = 0;
		vNpc = 1;
	}
	else if (strcmp(text, vGetOverallText(1)) == 0)
	{
		vOverall = 1;
		vUseMinScreen = 0;
		vScreenWidth = 800;
		vScreenHeight = 600;
		vScreenColor = 32;
		vDDSRes = 1;
		vEffectLevel = 1;
		vProjectionShadow = IsSupportShadow() ? 1 : 0;
		vShadowmap = 1;
		vReflection = IsSupportReflection() ? 1 : 0;
		vLod = IsSupportLod() ? 1 : 0;
		vNpc = 1;
	}
	else if (strcmp(text, vGetOverallText(2)) == 0)
	{
		vOverall = 2;
		vUseMinScreen = 0;
		vScreenWidth = 640;
		vScreenHeight = 480;
		vScreenColor = 16;
		vDDSRes = 2;
		vEffectLevel = 2;
		vProjectionShadow = 0;
		vShadowmap = 0;
		vReflection = 0;
		vLod = IsSupportLod() ? 1 : 0;
		vNpc = 0;
	}
}

void sOption::vSetOverallByVidOptions(void)
{
	if (vScreenWidth == 1024 && vScreenHeight == 768 && vScreenColor == 32 &&
		vDDSRes == 0 && vEffectLevel == 0 && vProjectionShadow == 1 &&
		vShadowmap == 1 && vReflection == 1 && vLod == 0 && vNpc == 1)
	{
		vOverall = 0;
	}
	else if (vScreenWidth == 800 && vScreenHeight == 600 &&
		vScreenColor == 32 && vDDSRes == 1 && vEffectLevel == 1 &&
		vProjectionShadow == 1 && vShadowmap == 1 && vReflection == 1 &&
		vLod == 1 && vNpc == 1)
	{
		vOverall = 1;
	}
	else if (vScreenWidth == 640 && vScreenHeight == 480 &&
		vScreenColor == 16 && vDDSRes == 2 && vEffectLevel == 2 &&
		vProjectionShadow == 0 && vShadowmap == 0 && vReflection == 0 &&
		vLod == 1 && vNpc == 0)
	{
		vOverall = 2;
	}
	else
	{
		vOverall = -1;
	}
}

const char* sOption::vGetOverallText(int level)
{
	return GetThreeLvlText(level);
}

int sOption::vGetOverallLvl(const char* text)
{
	return vGetLvl3FromText(text);
}

const char* sOption::vGetTnlText(int level)
{
	if (level == 0)
	{
		return K2L_Compatibility(
			"\xbc\xd2\xc7\xc1\xc6\xae\xbf\xfe\xbe\xee TnL");
	}
	else if (level == 1)
	{
		return K2L_Compatibility("\xc7\xcf\xb5\xe5\xbf\xfe\xbe\xee TnL");
	}

	return "";
}

int sOption::vGetTnlLvl(const char* text)
{
	if (strcmp(text,
			K2L_Compatibility(
				"\xbc\xd2\xc7\xc1\xc6\xae\xbf\xfe\xbe\xee TnL")) == 0)
	{
		return 0;
	}
	else if (strcmp(text,
				 K2L_Compatibility("\xc7\xcf\xb5\xe5\xbf\xfe\xbe\xee TnL")) ==
		0)
	{
		return 1;
	}

	return -1;
}

const char* sOption::vGetTexResText(int level)
{
	return GetThreeLvlText(level);
}

int sOption::vGetTexResLvl(const char* text)
{
	return vGetLvl3FromText(text);
}

const char* sOption::vGetParticleText(int level)
{
	return GetThreeLvlText(level);
}

int sOption::vGetParticleLvl(const char* text)
{
	return vGetLvl3FromText(text);
}

const char* sOption::aGetOverallText(int level)
{
	return GetThreeLvlText(level);
}

int sOption::aGetOverallLvl(const char* text)
{
	return vGetLvl3FromText(text);
}

const char* sOption::aGetSpeakerText(int type)
{
	switch (type)
	{
	case 0:
		return K2L_Compatibility("2\xc3\xa4\xb3\xce \xbd\xba\xc7\xc7\xc4\xbf");
	case 1:
		return K2L_Compatibility("\xc7\xec\xb5\xe5\xc6\xf9");
	case 2:
		return K2L_Compatibility("\xbc\xad\xb6\xf3\xbf\xee\xb5\xe5");
	case 3:
		return K2L_Compatibility("4\xc3\xa4\xb3\xce \xbd\xba\xc7\xc7\xc4\xbf");
	case 4:
		return K2L_Compatibility(
			"5.1\xc3\xa4\xb3\xce \xbd\xba\xc7\xc7\xc4\xbf");
	case 5:
		return K2L_Compatibility(
			"7.1\xc3\xa4\xb3\xce \xbd\xba\xc7\xc7\xc4\xbf");
	}

	return "";
}

int sOption::aGetSpeakerLvl(const char* text)
{
	for (int i = 0; i < 6; i++)
	{
		if (strcmp(text, aGetSpeakerText(i)) == 0)
		{
			return i;
		}
	}

	return 0;
}

struct sDisplayModeSize
{
	int width;
	int height;
	int color;
};

static sDisplayModeSize s_normalModes[12] = {
	{ 640,  480,  16 },
	{ 640,  480,  32 },
	{ 800,  600,  16 },
	{ 800,  600,  32 },
	{ 1024, 768,  16 },
	{ 1024, 768,  32 },
	{ 1280, 960,  16 },
	{ 1280, 960,  32 },
	{ 1280, 1024, 16 },
	{ 1280, 1024, 32 },
	{ 1600, 1200, 16 },
	{ 1600, 1200, 32 },
};

static sDisplayModeSize s_wideModes[28] = {
	{ 1024, 576,  16 },
	{ 1024, 576,  32 },
	{ 1280, 720,  16 },
	{ 1280, 720,  32 },
	{ 1280, 768,  16 },
	{ 1280, 768,  32 },
	{ 1280, 800,  16 },
	{ 1280, 800,  32 },
	{ 1366, 768,  16 },
	{ 1366, 768,  32 },
	{ 1400, 900,  16 },
	{ 1400, 900,  32 },
	{ 1440, 990,  16 },
	{ 1440, 990,  32 },
	{ 1400, 1050, 16 },
	{ 1400, 1050, 32 },
	{ 1440, 900,  16 },
	{ 1440, 900,  32 },
	{ 1440, 1050, 16 },
	{ 1440, 1050, 32 },
	{ 1600, 900,  16 },
	{ 1600, 900,  32 },
	{ 1680, 1050, 16 },
	{ 1680, 1050, 32 },
	{ 1920, 1080, 16 },
	{ 1920, 1080, 32 },
	{ 1920, 1200, 16 },
	{ 1920, 1200, 32 },
};

const char* sOption::GetThreeLvlText(int level)
{
	switch (level)
	{
	case 0:
		return K2L_Compatibility("\xb3\xf4\xc0\xbd");
	case 1:
		return K2L_Compatibility("\xc1\xdf\xb0\xa3");
	case 2:
		return K2L_Compatibility("\xb3\xb7\xc0\xbd");
	}

	return K2L_Compatibility("\xbb\xe7\xbf\xeb\xc0\xda");
}

const char* sOption::GetTwoLvlText(int level)
{
	switch (level)
	{
	case 0:
		return K2L_Compatibility("\xb3\xf4\xc0\xbd");
	case 1:
		return K2L_Compatibility("\xb3\xb7\xc0\xbd");
	}

	return K2L_Compatibility("\xbb\xe7\xbf\xeb\xc0\xda");
}

int sOption::vGetLvl3FromText(const char* text)
{
	for (int i = 0; i < 3; i++)
	{
		if (strcmp(text, vGetOverallText(i)) == 0)
		{
			return i;
		}
	}

	return -1;
}

const char* COption::ms_szCaptureFmt[] = { "jpg", "bmp", "avi", NULL };

#define MACRO1_DEFAULT \
	K2L_Compatibility( \
		"\xb8\xb8\xb3\xaa\xbc\xad \xb9\xdd\xb0\xa9\xbd\xc0\xb4\xcf\xb4\xd9.(\xc0\xce\xbb\xe7)")
#define MACRO2_DEFAULT \
	K2L_Compatibility( \
		"\xc7\xcf\xc0\xcc\xb7\xee~ \xb9\xdd\xb0\xa1\xbf\xf6\xbf\xe4~(\xc0\xae\xc5\xa9)")
#define MACRO3_DEFAULT \
	K2L_Compatibility( \
		"\xb7\xb9\xb5\xf0\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4~(\xc8\xad\xb3\xb2)")
#define MACRO4_DEFAULT \
	K2L_Compatibility("\xb3\xaa\xc0\xcc\xbd\xba~ \xbc\xa6!(\xb9\xda\xbc\xf6)")
#define MACRO5_DEFAULT \
	K2L_Compatibility( \
		"\xb5\xe9\xbe\xee\xb0\xa1\xb6\xf3~ \xb5\xe9\xbe\xee\xb0\xa1\xb6\xf3~ \xb5\xe9\xbe\xee\xb0\xa1\xb6\xf3~(\xb1\xe4\xc0\xe5)")
#define MACRO6_DEFAULT \
	K2L_Compatibility( \
		"\xc6\xc4\xc0\xcc\xc6\xc3~ \xc8\xfb\xb3\xbb\xbc\xbc\xbf\xe4~(\xc7\xec\xc7\xec)")
#define MACRO7_DEFAULT \
	K2L_Compatibility("\xbe\xd1! \xbd\xc7\xbc\xf6..(\xbe\xfb\xbe\xfb)")
#define MACRO8_DEFAULT \
	K2L_Compatibility( \
		"\xbc\xf6\xb0\xed\xc7\xcf\xbc\xcc\xbe\xee\xbf\xe4~(\xc0\xce\xbb\xe7)")

const char* COption::ms_szReg[][2] = {
	{ "Ver",						   "1.xx"         },
	{ "Option\\vWindowMode",           "0"            },
	{ "Option\\vFillMode",             "0"            },
	{ "Option\\vWideMode",             "0"            },
	{ "Option\\vOverall",              "-1"           },
	{ "Option\\vScreenWidth",          "800"          },
	{ "Option\\vScreenHeight",         "600"          },
	{ "Option\\vScreenColor",          "32"           },
	{ "Option\\vTnLMode",              "1"            },
	{ "Option\\Mip\\vDDSRes",          "0"            },
	{ "Option\\Mip\\vUseMipmap",       "0"            },
	{ "Option\\Mip\\vMaxMipLvl",       "1"            },
	{ "Option\\Mip\\vMipCreateFilter", "2"            },
	{ "Option\\vEffectLevel",          "1"            },
	{ "Option\\vProjectionShadow",     "1"            },
	{ "Option\\vShadowmap",            "1"            },
	{ "Option\\vReflection",           "0"            },
	{ "Option\\vLod",                  "0"            },
	{ "Option\\vNpc",                  "1"            },
	{ "Option\\vCoordX",               "0"            },
	{ "Option\\vCoordY",               "0"            },
	{ "Option\\vGamma",                "1.3"          },
	{ "Option\\aMssOn",                "1"            },
	{ "Option\\aOverall",              "0"            },
	{ "Option\\aMssFreq",              "44100"        },
	{ "Option\\aMssBits",              "16"           },
	{ "Option\\aMssChannels",          "2"            },
	{ "Option\\aMssEnableHwSound",     "1"            },
	{ "Option\\aMssBalance",           "128"          },
	{ "Option\\aMssSpeaker",           "0"            },
	{ "Option\\aSfxVolume",            "1.00"         },
	{ "Option\\aBGMVolume",            "1.00"         },
	{ "Option\\gMouseSensitivity",     "1.00"         },
	{ "Option\\gWhisper",              "1"            },
	{ "Option\\gInvitation",           "1"            },
	{ "Option\\gFriendConfirm",        "1"            },
	{ "Option\\gTransChatWin",         "1"            },
	{ "Option\\gUiEffect",             "1"            },
	{ "Option\\gLastLoginID",          ""             },
	{ "Option\\gIdentity",             "0"            },
	{ "Option\\gExtendChatLine",       "0"            },
	{ "Option\\gUserSort",             "0"            },
	{ "Option\\gPPL_Enable",           "1"            },
	{ "Option\\gPPL_Size",             "0"            },
	{ "Option\\gPowerGauge",           "1"            },
	{ "Option\\gCaptureLogo",          "1"            },
	{ "Option\\gRestore",              "0"            },
	{ "Option\\gChatWnd",              "0"            },
	{ "Option\\gUnderTabBtn",          "0"            },
	{ "Option\\gAvatarNewbie",         "1"            },
	{ "Option\\gCaptureHideGUI",       "0"            },
	{ "Option\\gCaptureHidePI",        "0"            },
	{ "Option\\gTerrainTooltip",       "1"            },
	{ "Option\\gCutinDisplay",         "1"            },
	{ "Option\\mMacro1",               MACRO1_DEFAULT },
	{ "Option\\mMacro2",               MACRO2_DEFAULT },
	{ "Option\\mMacro3",               MACRO3_DEFAULT },
	{ "Option\\mMacro4",               MACRO4_DEFAULT },
	{ "Option\\mMacro5",               MACRO5_DEFAULT },
	{ "Option\\mMacro6",               MACRO6_DEFAULT },
	{ "Option\\mMacro7",               MACRO7_DEFAULT },
	{ "Option\\mMacro8",               MACRO8_DEFAULT },
	{ "Option\\mSaveMacros",           "1"            },
};

COption::COption(HWND hWnd, WDeviceManager* deviceManager,
	WResourceManager* resourceManager)
	: m_hWnd(hWnd),
	  m_pDeviceManager(deviceManager),
	  m_pResourceManager(resourceManager)
{
	InitVars();
	Init();
}

COption::~COption()
{
	SaveReg();

	m_displayModeList.clear();
}

void COption::SaveReg(void)
{
	WriteReg_I(1, m_opt.vWindowMode);
	WriteReg_I(2, m_opt.vFillMode);

	WriteReg_I(3, m_opt.vWideMode);
	WriteReg_I(4, m_opt.vOverall);
	WriteReg_I(5, m_opt.vScreenWidth);
	WriteReg_I(6, m_opt.vScreenHeight);
	WriteReg_I(7, m_opt.vScreenColor);
	WriteReg_I(8, m_opt.vTnLMode);
	WriteReg_I(9, m_opt.vDDSRes);
	WriteReg_I(10, m_opt.vUseMipmap);
	WriteReg_I(11, m_opt.vMaxMipLvl);
	WriteReg_I(12, m_opt.vMipCreateFilter);
	WriteReg_I(13, m_opt.vEffectLevel);
	WriteReg_I(14, m_opt.vProjectionShadow);
	WriteReg_I(15, m_opt.vShadowmap);
	WriteReg_I(16, m_opt.vReflection);
	WriteReg_I(17, m_opt.vLod);
	WriteReg_I(18, m_opt.vNpc);
	WriteReg_I(19, m_opt.vCoordX);
	WriteReg_I(20, m_opt.vCoordY);

	WriteReg_I(22, m_opt.aMssOn);
	WriteReg_I(23, m_opt.aOverall);
	WriteReg_I(24, m_opt.aMssFreq);
	WriteReg_I(25, m_opt.aMssBits);
	WriteReg_I(26, m_opt.aMssChannels);
	WriteReg_I(27, m_opt.aMssEnableHwSound);
	WriteReg_I(28, m_opt.aMssBalance);
	WriteReg_I(29, m_opt.aMssSpeaker);
	WriteReg_F(30, m_opt.aSfxVolume);
	WriteReg_F(31, m_opt.aBGMVolume);

	WriteReg_F(32, m_opt.gMouseSensitivity);
	WriteReg_I(33, m_opt.gWhisper);
	WriteReg_I(34, m_opt.gInvitation);
	WriteReg_I(35, m_opt.gFriendConfirm);
	WriteReg_I(36, m_opt.gTransChatWin);
	WriteReg_I(37, m_opt.gUiEffect);
	WriteReg_S(38, m_opt.gLastLoginID);
	WriteReg_I(39, m_opt.gIdentity);
	WriteReg_I(40, m_opt.gExtendChatLine);
	WriteReg_I(41, m_opt.gUserSort);
	WriteReg_I(42, m_opt.gPPL_Enable);
	WriteReg_I(43, m_opt.gPPL_Size);
	WriteReg_I(44, m_opt.gPowerGauge);
	WriteReg_I(45, m_opt.gCaptureLogo);
	WriteReg_I(46, m_opt.gRestore);
	WriteReg_I(47, m_opt.gChatWnd);
	WriteReg_I(48, m_opt.gUnderTabBtn);
	WriteReg_I(49, m_opt.gAvatarNewbie);
	WriteReg_I(50, m_opt.gCaptureHideGUI);
	WriteReg_I(51, m_opt.gCaptureHidePI);
	WriteReg_I(52, m_opt.gTerrainTooltip);
	WriteReg_I(53, m_opt.gCutinDisplay);

	WriteReg_S(54, m_opt.mMacro[0]);
	WriteReg_S(55, m_opt.mMacro[1]);
	WriteReg_S(56, m_opt.mMacro[2]);
	WriteReg_S(57, m_opt.mMacro[3]);
	WriteReg_S(58, m_opt.mMacro[4]);
	WriteReg_S(59, m_opt.mMacro[5]);
	WriteReg_S(60, m_opt.mMacro[6]);
	WriteReg_S(61, m_opt.mMacro[7]);
	WriteReg_I(62, m_opt.mSaveMacros);
}

void COption::InitVars(void)
{
	m_pVideoDev = NULL;
	m_pAudioManager = NULL;
	m_pInputManager = NULL;
	m_bPendingChange = false;
	m_fBarY = m_opt.vScreenHeight - 62.0f;
	m_bSwapMouseButton = false;
	m_bQuitWindowed = false;
	m_nCaptureFmt = 0;
	m_nDefaultCharacter = 0;
	m_nDefaultCaddie = 0;
	m_nAviWidth = 320;
	m_nAviHeight = 240;
	m_nAviFps = 10;
	m_nStartMode = 0;
	m_nLobbyCourse = -1;
	m_nMap = 0;
	m_nStartHole = 1;
	m_nReplayMode = 0;
	m_nNumPlayers = 1;
	m_nGameTime = 1;
	m_nWeather = 0;
	m_nClickSystem = 0;
	m_bVtxClrTest = 0;
	m_bBaseMeshTest = 0;
}

bool COption::ReadReg(int index, char* buffer, int size)
{
	if (index < 0 || index >= 63)
	{
		return false;
	}

	return m_reg.Read((char*)ms_szReg[index][0], buffer, size);
}

bool COption::ScanReg_I(int index, int* value)
{
	if (index < 0 || index >= 63 || !ms_szReg[index][0] ||
		strlen(ms_szReg[index][0]) == 0)
	{
		return false;
	}

	return m_reg.Scan((char*)ms_szReg[index][0], "%d", value) ? true : false;
}

bool COption::ScanReg_F(int index, float* value)
{
	if (index < 0 || index >= 63 || !ms_szReg[index][0] ||
		strlen(ms_szReg[index][0]) == 0)
	{
		return false;
	}

	return m_reg.Scan((char*)ms_szReg[index][0], "%f", value) ? true : false;
}

bool COption::ScanReg_S(int index, char* value)
{
	if (index < 0 || index >= 63 || !ms_szReg[index][0] ||
		strlen(ms_szReg[index][0]) == 0)
	{
		value[0] = 0;
		return false;
	}

	if (m_reg.Scan((char*)ms_szReg[index][0], "%s", value))
	{
		return true;
	}

	value[0] = 0;
	return false;
}

void COption::WriteReg(int index, const char* value)
{
	if (index >= 0 && index < 63 && ms_szReg[index][0] &&
		strlen(ms_szReg[index][0]) != 0)
	{
		m_reg.Write((char*)ms_szReg[index][0], value);
	}
}

bool COption::WriteReg_I(int index, int value)
{
	static char szValue[256];

	if (index < 0 || index >= 63 || !ms_szReg[index][0] ||
		strlen(ms_szReg[index][0]) == 0)
	{
		return false;
	}

	sprintf(szValue, "%d", value);
	m_reg.Write((char*)ms_szReg[index][0], szValue);

	return true;
}

bool COption::WriteReg_F(int index, float value)
{
	static char szValue[256];

	if (index < 0 || index >= 63 || !ms_szReg[index][0] ||
		strlen(ms_szReg[index][0]) == 0)
	{
		return false;
	}

	sprintf(szValue, "%.2f", value);
	m_reg.Write((char*)ms_szReg[index][0], szValue);

	return true;
}

bool COption::WriteReg_S(int index, const char* value)
{
	static char szValue[256];

	if (index < 0 || index >= 63 || !ms_szReg[index][0] ||
		strlen(ms_szReg[index][0]) == 0)
	{
		return false;
	}

	sprintf(szValue, "%s", value);
	m_reg.Write((char*)ms_szReg[index][0], szValue);

	return true;
}

void COption::Init(void)
{
	char szLocation[256];
	szLocation[0] = '\0';

	if (IsLocalContent((localContentType_t)0x10))
	{
		char* cmdLine = GetCommandLineA();

		if (strstr(cmdLine, "-godhfma"))
		{
			sprintf(szLocation, "%s\\%s", "HKEY_LOCAL_MACHINE",
				"Software\\Ntreev\\Pangya_QA");
		}
		else if (strstr(cmdLine, "-vkfks"))
		{
			strcpy(szLocation,
				"HKEY_LOCAL_MACHINE\\Software\\ParanGame\\Pangya");
		}
		else
		{
			sprintf(szLocation, "%s\\%s", "HKEY_LOCAL_MACHINE",
				"Software\\Ntreev\\Pangya_QA");
		}
	}
	else
	{
		sprintf(szLocation, "%s\\%s", "HKEY_LOCAL_MACHINE",
			"Software\\Ntreev\\Pangya_QA");
	}

	m_reg.SetLocation(szLocation);

	char szValue[128] = { 0 };
	for (unsigned char i = 0; i < 63; i++)
	{
		if (strlen(ms_szReg[i][0]) != 0 &&
			!ReadReg(i, szValue, sizeof(szValue)))
		{
			WriteReg(i, ms_szReg[i][1]);
		}
	}

	ScanReg_S(0, PY_PUBLIC_VERSION);

	ScanReg_I(1, &m_opt.vWindowMode);
	ScanReg_I(2, &m_opt.vFillMode);
	ScanReg_I(3, &m_opt.vWideMode);
	ScanReg_I(4, &m_opt.vOverall);
	ScanReg_I(5, &m_opt.vScreenWidth);
	ScanReg_I(6, &m_opt.vScreenHeight);
	ScanReg_I(7, &m_opt.vScreenColor);
	ScanReg_I(8, &m_opt.vTnLMode);
	m_opt.vTnLMode = 1;
	ScanReg_I(9, &m_opt.vDDSRes);
	ScanReg_I(10, &m_opt.vUseMipmap);
	ScanReg_I(11, &m_opt.vMaxMipLvl);
	ScanReg_I(12, &m_opt.vMipCreateFilter);
	ScanReg_I(13, &m_opt.vEffectLevel);
	ScanReg_I(14, &m_opt.vProjectionShadow);
	ScanReg_I(15, &m_opt.vShadowmap);
	ScanReg_I(16, &m_opt.vReflection);
	ScanReg_I(17, &m_opt.vLod);
	ScanReg_I(18, &m_opt.vNpc);
	ScanReg_I(19, &m_opt.vCoordX);
	ScanReg_I(20, &m_opt.vCoordY);
	ScanReg_F(21, &m_opt.vGamma);

	ScanReg_I(22, &m_opt.aMssOn);
	ScanReg_I(23, &m_opt.aOverall);
	ScanReg_I(24, &m_opt.aMssFreq);
	ScanReg_I(25, &m_opt.aMssBits);
	ScanReg_I(26, &m_opt.aMssChannels);
	ScanReg_I(27, &m_opt.aMssEnableHwSound);
	ScanReg_I(28, &m_opt.aMssBalance);
	ScanReg_I(29, &m_opt.aMssSpeaker);
	ScanReg_F(30, &m_opt.aSfxVolume);
	ScanReg_F(31, &m_opt.aBGMVolume);

	ScanReg_F(32, &m_opt.gMouseSensitivity);
	ScanReg_I(33, &m_opt.gWhisper);
	ScanReg_I(34, &m_opt.gInvitation);
	ScanReg_I(35, &m_opt.gFriendConfirm);
	ScanReg_I(36, &m_opt.gTransChatWin);
	ScanReg_I(37, &m_opt.gUiEffect);
	ScanReg_S(38, m_opt.gLastLoginID);
	ScanReg_I(39, &m_opt.gIdentity);
	ScanReg_I(40, &m_opt.gExtendChatLine);
	ScanReg_I(41, &m_opt.gUserSort);
	ScanReg_I(42, &m_opt.gPPL_Enable);
	ScanReg_I(43, &m_opt.gPPL_Size);
	ScanReg_I(44, &m_opt.gPowerGauge);
	ScanReg_I(45, &m_opt.gCaptureLogo);
	ScanReg_I(46, &m_opt.gRestore);
	ScanReg_I(47, &m_opt.gChatWnd);
	ScanReg_I(48, &m_opt.gUnderTabBtn);
	ScanReg_I(49, &m_opt.gAvatarNewbie);
	ScanReg_I(50, &m_opt.gCaptureHideGUI);
	ScanReg_I(51, &m_opt.gCaptureHidePI);
	ScanReg_I(52, &m_opt.gTerrainTooltip);
	ScanReg_I(53, &m_opt.gCutinDisplay);

	char szMacro[8][260] = { 0 };
	ScanReg_S(54, szMacro[0]);
	ScanReg_S(55, szMacro[1]);
	ScanReg_S(56, szMacro[2]);
	ScanReg_S(57, szMacro[3]);
	ScanReg_S(58, szMacro[4]);
	ScanReg_S(59, szMacro[5]);
	ScanReg_S(60, szMacro[6]);
	ScanReg_S(61, szMacro[7]);
	ScanReg_I(62, &m_opt.mSaveMacros);

	wnsprintfA(m_opt.mMacro[0], 64, "%s", szMacro[0]);
	wnsprintfA(m_opt.mMacro[1], 64, "%s", szMacro[1]);
	wnsprintfA(m_opt.mMacro[2], 64, "%s", szMacro[2]);
	wnsprintfA(m_opt.mMacro[3], 64, "%s", szMacro[3]);
	wnsprintfA(m_opt.mMacro[4], 64, "%s", szMacro[4]);
	wnsprintfA(m_opt.mMacro[5], 64, "%s", szMacro[5]);
	wnsprintfA(m_opt.mMacro[6], 64, "%s", szMacro[6]);
	wnsprintfA(m_opt.mMacro[7], 64, "%s", szMacro[7]);

	m_opt.vWindowMode = m_opt.vWindowMode != 0;

	m_opt.vFillMode = m_opt.vFillMode != 0;

	m_opt.vWideMode = m_opt.vWideMode != 0;
	m_opt.vOverall = Between(0, m_opt.vOverall, 2);
	m_fBarY = m_opt.vScreenHeight - 62.0f;
	m_opt.vTnLMode = Between(0, m_opt.vTnLMode, 2);
	m_opt.vDDSRes = Max(0, m_opt.vDDSRes);
	m_opt.vUseMipmap = m_opt.vUseMipmap != 0;
	m_opt.vMaxMipLvl = Max(0, m_opt.vMaxMipLvl);
	m_opt.vMipCreateFilter = Between(0, m_opt.vMipCreateFilter, 5);
	m_opt.vEffectLevel = Between(0, m_opt.vEffectLevel, 2);
	m_opt.vProjectionShadow = m_opt.vProjectionShadow != 0;
	m_opt.vShadowmap = m_opt.vShadowmap != 0;
	m_opt.vReflection = m_opt.vReflection != 0;
	m_opt.vLod = m_opt.vLod != 0;
	m_opt.vNpc = m_opt.vNpc != 0;

	m_opt.vCoordX = Between(0, m_opt.vCoordX,
		GetSystemMetrics(SM_CXSCREEN) - m_opt.vScreenWidth);
	m_opt.vCoordY = Between(0, m_opt.vCoordX,
		GetSystemMetrics(SM_CYSCREEN) - m_opt.vScreenHeight);
	m_opt.vGamma = Between(0.5f, m_opt.vGamma, 3.0f);

	m_nTnLMode = m_opt.vTnLMode;

	m_opt.aMssOn = m_opt.aMssOn != 0;
	m_opt.aOverall = Between(0, m_opt.aOverall, 2);
	m_opt.aMssFreq = m_opt.aMssFreq == 44100 ? 44100 : 1;
	m_opt.aMssBits = m_opt.aMssBits == 16 ? 16 : 8;
	m_opt.aMssChannels = m_opt.aMssChannels == 2 ? 2 : 1;
	m_opt.aMssEnableHwSound = m_opt.aMssEnableHwSound != 0;
	m_opt.aMssBalance = Between(0, m_opt.aMssBalance, 256);
	m_opt.aMssSpeaker = Between(0, m_opt.aMssSpeaker, 5);
	m_opt.aSfxVolume = Between(0.0f, m_opt.aSfxVolume, 1.0f);
	m_opt.aBGMVolume = Between(0.0f, m_opt.aBGMVolume, 1.0f);

	m_opt.gMouseSensitivity = Between(0.5f, m_opt.gMouseSensitivity, 2.0f);
	m_opt.gWhisper = m_opt.gWhisper != 0;
	m_opt.gInvitation = m_opt.gInvitation != 0;
	m_opt.gTransChatWin = m_opt.gTransChatWin != 0;
	m_opt.gUiEffect = m_opt.gUiEffect != 0;

	m_opt.gLastLoginID[sizeof(m_opt.gLastLoginID) - 1] = '\0';
	m_opt.gExtendChatLine = m_opt.gExtendChatLine != 0;
	m_opt.gPowerGauge = m_opt.gPowerGauge != 0;
	m_opt.gCaptureLogo = m_opt.gCaptureLogo != 0;
	m_opt.gCutinDisplay = m_opt.gCutinDisplay != 0;

	for (int i = 0; i < 8; i++)
	{
		std::string macro(m_opt.mMacro[i]);

		if (macro.length() == 0 ||
			macro.find_first_not_of(' ') == std::string::npos)
		{
			sprintf(m_opt.mMacro[i],
				"\xc3\xa4\xc6\xc3 \xb8\xc5\xc5\xa9\xb7\xce\xb8\xa6 \xbc\xb3\xc1\xa4\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
		}

		LimitText(m_opt.mMacro[i], 64);
	}
	m_opt.mSaveMacros = m_opt.mSaveMacros != 0;

	WRegistry mouseRegistry;

	mouseRegistry.SetLocation("HKEY_CURRENT_USER\\Control Panel\\Mouse");

	char szSwap[128];
	if (mouseRegistry.Read("SwapMouseButtons", szSwap, sizeof(szSwap)))
	{
		int swap;

		sscanf(szSwap, "%d", &swap);
		m_bSwapMouseButton = swap != 0;
	}

	FrForm::EnableTail(m_opt.gUiEffect != 0);

	char szCmdLine[260];
	strcpy(szCmdLine, GetCommandLineA());
	strlwr(szCmdLine);

	if (strstr(szCmdLine, "-safe"))
	{
		m_opt.vWindowMode = 1;
		m_opt.vWideMode = 0;
		m_opt.vOverall = 2;
		vChangeGameScreenSize(800, 600, 16, false);
		m_opt.vTnLMode = 0;
		m_opt.vDDSRes = 1;
		m_opt.vUseMipmap = 0;
		m_opt.vMaxMipLvl = 1;
		m_opt.vMipCreateFilter = 2;
		m_opt.vEffectLevel = 2;
		m_opt.vProjectionShadow = 0;
		m_opt.vShadowmap = 0;
		m_opt.vReflection = 0;
		m_opt.vLod = 0;
		m_opt.vNpc = 0;
		m_opt.vCoordX = (GetSystemMetrics(SM_CXSCREEN) - 800) / 2;
		m_opt.vCoordY = (GetSystemMetrics(SM_CYSCREEN) - 600) / 2;

		m_nTnLMode = m_opt.vTnLMode;
	}

	strcpy(m_opt.mPCBangMascotMsg, "PangYa!");
}

void COption::CheckDebugRegValid(void)
{
	m_nCaptureFmt = Between(0, m_nCaptureFmt, 2);
	m_nDefaultCharacter = Max(0, m_nDefaultCharacter);
	m_nDefaultCaddie = Max(0, m_nDefaultCaddie);
	m_nAviWidth =
		(m_nAviWidth == 640 || m_nAviWidth == 800) ? m_nAviWidth : 320;
	m_nAviHeight = m_nAviWidth == 640 ? 480 : (m_nAviWidth == 800 ? 600 : 240);
	m_nAviFps = Max(1, m_nAviFps);

	m_nStartMode = Between(0, m_nStartMode, 3);

	m_nLobbyCourse = Max(-1, m_nLobbyCourse);
	m_nMap = Max(0, m_nMap);
	m_nStartHole = Between(1, m_nStartHole, 18);
	m_nNumPlayers = Between(1, m_nNumPlayers, 30);
	m_nGameTime = Max(0, m_nGameTime);
	m_nWeather = Between(0, m_nWeather, 4);
	m_nChatGender = m_nChatGender != 0;
	m_bVtxClrTest = m_bVtxClrTest != 0;
	m_bBaseMeshTest = m_bBaseMeshTest != 0;
	m_nClickSystem = m_nClickSystem != 0;
	m_nReplayMode = Between(0, m_nReplayMode, 3);
	m_nGameType = Between(0, m_nGameType, 16);
	m_bVersionInfo = m_bVersionInfo != 0;
}

void COption::SetVideoDevice(WVideoDev* videoDevice)
{
	int caps;

	m_pVideoDev = videoDevice;
	m_pVideoDev->Command(WX_VDEV_GET_CAPS, 0, (intptr_t)&caps);
	sOption::sbHwTnlSupport = caps != 0;
	sOption::sbRtSupport = m_pVideoDev->SupportRenderTargetFormat();

	sOption::sbVsSupport = m_pVideoDev->IsSupportVS();
	sOption::sbPsSupport = m_pVideoDev->IsSupportPS();
	sOption::sbMrtSupport = m_pVideoDev->IsSupportMRT();
	sOption::sbClipPlaneSupport = m_pVideoDev->IsSupportClipPlane();

	m_pVideoDev->Command(WX_VDEV_GET_CAPS, 1, (intptr_t)&caps);
	m_nTnLMode = m_opt.vTnLMode = caps < 0 ? 0 : (caps > 2 ? 2 : caps);
	m_pVideoDev->Command(W_VDEV_SET_CAPTURE_MODE, m_opt.gPowerGauge,
		m_opt.vWindowMode);

	m_pDeviceManager->ResetVideoDevice(true, 800, 600, m_opt.vScreenColor,
		0xC40000, 0);

	if (!sOption::sbRtSupport)
	{
		m_opt.vProjectionShadow = 0;
	}

	if (!sOption::sbRtSupport || !sOption::sbClipPlaneSupport ||
		!sOption::sbVsSupport || !sOption::sbPsSupport)
	{
		m_opt.vReflection = 0;
	}

	if (!sOption::sbRtSupport)
	{
		m_opt.vLod = 0;
	}

	m_opt.vSetOverallByVidOptions();

	CheckDisplayModeList(videoDevice, m_opt.vWindowMode != 0,
		m_opt.vWideMode != 0);

	vApplyMipmap();
}

void COption::SetGlobalView(WView* view)
{
	m_pGlobalView = view;
}

void COption::SetAudioManager(CSoundManager* audioManager)
{
	m_pAudioManager = audioManager;
}

void COption::SetInputManager(CInputManager* inputManager)
{
	m_pInputManager = inputManager;

	if (m_pInputManager)
	{
		m_pInputManager->SwapMouseButton(m_bSwapMouseButton);
	}

	gSetMouseSensitivity(m_opt.gMouseSensitivity);
}

void COption::vSetMipmap(int level, bool enable, int param1, int param2)
{
	m_opt.vDDSRes = Max(0, level);
	m_opt.vUseMipmap = enable ? 1 : 0;
	m_opt.vMaxMipLvl = Max(0, param1);
	m_opt.vMipCreateFilter = Max(0, param2);
}

void COption::vApplyMipmap(void)
{
	m_pVideoDev->Command(W_VDEV_SET_DDS_RES, m_opt.vDDSRes, 0);
	m_pVideoDev->Command(W_VDEV_USE_MIPMAP, m_opt.vUseMipmap, 0);
	m_pVideoDev->Command(W_VDEV_MAX_MIPLVL, m_opt.vMaxMipLvl, 0);
	m_pVideoDev->Command(W_VDEV_MIP_CREATE_FILTER, m_opt.vMipCreateFilter, 0);
}

void COption::vSetMinimumScreenSize(int enable, int width, int height)
{
	m_opt.vUseMinScreen = enable;
	m_opt.vMinScreenWidth = width;
	m_opt.vMinScreenHeight = height;
}

void COption::vChangeGameScreenSize(int width, int height, int bpp, bool check)
{
	if (check)
	{
		vCheckScreenSize(width, height, bpp);
	}
	else
	{
		m_opt.vScreenWidth = width;
		m_opt.vScreenHeight = height;
		m_opt.vScreenColor = bpp;
	}

	m_fBarY = m_opt.vScreenHeight - 62.0f;
}

void COption::vApplyGameScreenSize(void)
{
	if (m_opt.vUseMinScreen)
	{
		if (m_opt.vScreenWidth < m_opt.vMinScreenWidth ||
			m_opt.vScreenHeight < m_opt.vMinScreenHeight)
		{
			vChangeScreenSize(m_opt.vMinScreenWidth, m_opt.vMinScreenHeight,
				m_opt.vScreenColor);
			return;
		}
	}

	vChangeScreenSize(m_opt.vScreenWidth, m_opt.vScreenHeight,
		m_opt.vScreenColor);
}

void COption::vApplyLobbyScreenSize(void)
{
	if (CShadowManager::IsInstantiated())
	{
		CShadowManager::Instance()->Reset();
	}

	vChangeScreenSize(800, 600, m_opt.vScreenColor);
}

bool COption::vChangeScreenSize(int width, int height, int bpp)
{
	int captureMode;
	bool captureChanged = false;

	m_pVideoDev->Command(W_VDEV_GET_CAPTURE_MODE, (intptr_t)&captureMode, 0);

	if (GetGameType() == 0xB || GetGameType() == 0xC)
	{
		if (captureMode != 0)
		{
			captureChanged = true;
		}
	}
	else
	{
		if (captureMode != m_opt.gPowerGauge)
		{
			captureChanged = true;
		}
	}

	if (m_pVideoDev->IsWindowed() == (m_opt.vWindowMode != 0 ? true : false) &&
		m_pVideoDev->GetWidth() == width &&
		m_pVideoDev->GetHeight() == height &&
		m_pVideoDev->IsFillScreenMode() ==
			(m_opt.vFillMode != 0 ? true : false) &&
		(m_opt.vWindowMode || m_pVideoDev->GetBackBufferBpp() == bpp) &&
		!captureChanged)
	{
		return true;
	}

	WView* view = m_pGlobalView;
	float clipNear = view->GetClipNearValue();
	float fov = view->GetFOV();
	float clipFar = view->GetClipFarValue();

	CProjectG::Instance()->SetWindowed(m_opt.vWindowMode != 0);

	m_pVideoDev->Command(W_VDEV_SET_CAPTURE_MODE,
		GetGameType() == 0xB || GetGameType() == 0xC ? 0 : m_opt.gPowerGauge,
		m_opt.vWindowMode);

	bool result;
	if (m_opt.vWindowMode)
	{
		long style = m_opt.vFillMode ? 0x90080000 : 0x00C40000;
		result = m_pDeviceManager->ResetVideoDevice(true, width, height, bpp,
			style, m_opt.vFillMode);
	}
	else
	{
		result = m_pDeviceManager->ResetVideoDevice(false, width, height, bpp,
			0x90080000, 0);
	}

	m_pGlobalView->SetViewport((float)m_pVideoDev->GetWidth(),
		(float)m_pVideoDev->GetHeight());
	m_pGlobalView->SetClip(clipNear, clipFar, false);
	m_pGlobalView->SetFOV(fov);
	m_pGlobalView->UpdateCamera();

	return result;
}

void COption::vSetParticleLevel(int level)
{
	m_opt.vEffectLevel = Between<int>(0, level, 2);
}

void COption::vEnableShadow(bool enable)
{
	m_opt.vProjectionShadow = enable ? 1 : 0;
}

void COption::vEnableShadowmap(bool enable)
{
	m_opt.vShadowmap = enable ? 1 : 0;
}

bool COption::vIsWindowed(void)
{
	return m_opt.vWindowMode != 0 ? true : false;
}

bool COption::vIsWideMode(void)
{
	return m_opt.vWideMode != 0 ? true : false;
}

bool COption::vIsShadowEnabled(void)
{
	return m_opt.vProjectionShadow != 0 ? true : false;
}

bool COption::vIsShadowmapEnabled(void)
{
	return m_opt.vShadowmap != 0 ? true : false;
}

bool COption::vIsReflectionEnabled(void)
{
	return m_opt.vReflection != 0 ? true : false;
}

bool COption::vIsLodEnabled(void)
{
	return m_opt.vLod != 0 ? true : false;
}

bool COption::vIsNpcEnabled(void)
{
	return m_opt.vNpc != 0 ? true : false;
}

void COption::vToggleFullscreen(void)
{
	m_newOpt = m_opt;
	m_newOpt.vWindowMode = !m_opt.vWindowMode;
	ApplyChange(m_newOpt, false);
}

void COption::aApplySfxVolume(float volume)
{
	if (m_pAudioManager)
	{
		m_pAudioManager->SetVolume(Between<float>(0.0f, volume, 1.0f));
	}
}

void COption::aApplyBgmVolume(float volume)
{
	if (m_pAudioManager)
	{
		m_pAudioManager->SetBGMVolume(Between<float>(0.0f, volume, 1.0f));
	}
}

void COption::aSetSfxVolume(float volume)
{
	m_opt.aSfxVolume = Between<float>(0.0f, volume, 1.0f);
}

void COption::aSetBgmVolume(float volume)
{
	m_opt.aBGMVolume = Between<float>(0.0f, volume, 1.0f);
}

void COption::aSetSpeakerType(int type)
{
	m_opt.aMssSpeaker = Between<int>(0, type, 5);
}

bool COption::aIsMssEnabled(void)
{
	return m_opt.aMssOn != 0 ? true : false;
}

bool COption::aIsMssHwSoundEnabled(void)
{
	return m_opt.aMssEnableHwSound != 0 ? true : false;
}

WMilesSoundSystem::w_speaker_type COption::aGetMssSpeaker(void)
{
	return (WMilesSoundSystem::w_speaker_type)m_opt.aMssSpeaker;
}

void COption::aApplyAudioSetting(void)
{
	g_audio->SetSpeakerType(m_opt.aMssSpeaker);
}

void COption::gSetMouseSensitivity(float sensitivity)
{
	m_opt.gMouseSensitivity = Between<float>(0.5f, sensitivity, 2.0f);
	m_pInputManager->SetSensitivity(sensitivity);
}

bool COption::gIsWhisperAllowed(void)
{
	return m_opt.gWhisper != 0 ? true : false;
}

bool COption::gIsInvitationAllowed(void)
{
	return m_opt.gInvitation != 0 ? true : false;
}

bool COption::gIsFriendConfirmNeeded(void)
{
	return m_opt.gFriendConfirm != 0 ? true : false;
}

bool COption::gIsChatWinTransparent(void)
{
	return m_opt.gTransChatWin != 0 ? true : false;
}

bool COption::gIsChatLineEntended(void)
{
	return m_opt.gExtendChatLine != 0 ? true : false;
}

bool COption::gIsPPLEnable(void)
{
	return m_opt.gPPL_Enable != 0 ? true : false;
}

void COption::gSetPPLEnable(bool enable)
{
	m_opt.gPPL_Enable = enable;
}

void COption::gSetPPLSize(int size)
{
	m_opt.gPPL_Size = size;
}

int COption::gGetRestore(void)
{
	if (m_opt.gRestore == 0)
	{
		return 0;
	}

	char check = (char)(m_opt.gRestore >> 30);
	check = (char)(check << 2 | ((m_opt.gRestore >> 22) & 3));
	check = (char)(check << 2 | ((m_opt.gRestore >> 14) & 3));
	check = (char)(check << 2 | ((m_opt.gRestore >> 6) & 3));

	unsigned int len = strlen(MyId());
	char sum = 0;
	for (unsigned int i = 0; i < len; i++)
	{
		sum += MyId()[i];
	}

	if (check != sum)
	{
		gResetRestore();
		return 0;
	}

	unsigned char halfMinute = (unsigned char)(m_opt.gRestore >> 24) & 0x3F;
	return (halfMinute << 25) | (m_opt.gRestore & 0x3F3F3F);
}

void COption::gSetRestore(void)
{
	if (!((__rtti_obj = AfxGetTask()) != 0
				? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI)
				: false))
	{
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 10)
	{
		return;
	}

	unsigned int len = strlen(MyId());
	char sum = 0;
	for (unsigned int i = 0; i < len; i++)
	{
		sum += MyId()[i];
	}

	SYSTEMTIME time;
	GetLocalTime(&time);
	m_opt.gRestore = ((((((((sum >> 6) & 3) << 6 | (time.wMinute / 2)) << 2 |
							 ((sum >> 4) & 3))
								<< 6 |
							time.wHour)
							   << 2 |
						   ((sum >> 2) & 3))
							  << 6 |
						  time.wDay)
							 << 2 |
						 (sum & 3))
			<< 6 |
		time.wMonth;

	WriteReg_I(46, m_opt.gRestore);
}

void COption::gResetRestore(void)
{
	m_opt.gRestore = 0;
	WriteReg_I(46, m_opt.gRestore);
}

void COption::dApplyDebugSetting(void)
{
	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
			WNetworkSystem::NET_GAME))
	{
		return;
	}

	SetCurMap((unsigned char)m_nMap);

	SetHoleType(0);
	SetGameTimeLimit(0);

	switch (m_nGameTime)
	{
	case 0:
		SetShotTimeLimit(0);
		break;

	case 1:
		SetShotTimeLimit(40000);
		break;

	default:
		SetShotTimeLimit(m_nGameTime * 1000);
		break;
	}
}

void COption::dSetNumPlayers(int numPlayers)
{
	m_nNumPlayers = Between<int>(1, numPlayers, 30);
}

const char* COption::dGetCaptureFormat(void)
{
	return ms_szCaptureFmt[m_nCaptureFmt];
}

void COption::dSetCaptureFormat(int format)
{
	m_nCaptureFmt = format;
}

int COption::dGetDefaultCharacter(void)
{
	return m_nDefaultCharacter;
}

int COption::dGetDefaultCaddie(void)
{
	return m_nDefaultCaddie;
}

int COption::dGetDefaultClub(void)
{
	return m_nDefaultClub;
}

int COption::dGetDefaultBall(void)
{
	return m_nDefaultBall;
}

int COption::dGetMap(void)
{
	return m_nMap;
}

int COption::dGetAviWidth(void)
{
	return m_nAviWidth;
}

int COption::dGetAviHeight(void)
{
	return m_nAviHeight;
}

int COption::dGetAviFps(void)
{
	return m_nAviFps;
}

int COption::dGetStartMode(void)
{
	return m_nStartMode;
}

int COption::dGetLobbyCourse(void)
{
	return m_nLobbyCourse;
}

int COption::dGetNumPlayers(void)
{
	return m_nNumPlayers;
}

unsigned char COption::dGetChatGender(void)
{
	return m_nChatGender;
}

bool COption::dIsVtxClrTestEnabled(void)
{
	return m_bVtxClrTest != 0 ? true : false;
}

bool COption::dIsBaseMeshTestEnabled(void)
{
	return m_bBaseMeshTest != 0 ? true : false;
}

int COption::dGetClickSystem(void)
{
	return m_nClickSystem;
}

int COption::dGetReplayMode(void)
{
	return m_nReplayMode;
}

void COption::dSetReplayMode(int mode)
{
	m_nReplayMode = mode;
}

int COption::dGetGameType(void)
{
	return m_nGameType;
}

int COption::dGetWeather(void)
{
	return m_nWeather;
}

unsigned char COption::dGetStartHole(void)
{
	return m_nStartHole;
}

bool COption::dIsVersionInfoEnabled(void)
{
	return m_bVersionInfo != 0 ? true : false;
}

float COption::gGetBar_Y(void)
{
	return m_fBarY;
}

void COption::ApplyChange(sOption& option, bool flag)
{
	m_bPendingChange = true;
	m_newOpt = option;
	m_bGameScreen = flag;
}

const sOption* COption::GetOption(void)
{
	return &m_opt;
}

void COption::CheckPendingChanges(void)
{
	if (!m_bPendingChange)
	{
		return;
	}

	m_bPendingChange = false;

	if (m_opt.vWindowMode != m_newOpt.vWindowMode ||
		m_opt.vScreenWidth != m_newOpt.vScreenWidth ||
		m_opt.vScreenHeight != m_newOpt.vScreenHeight ||
		m_opt.vScreenColor != m_newOpt.vScreenColor ||
		m_opt.vFillMode != m_newOpt.vFillMode)
	{
		if (CBrowser::IsInstantiated())
		{
			CBrowser::Instance()->ResetCount();
		}

		if (m_bGameScreen)
		{
			int oldBpp = m_opt.vScreenColor;
			int oldWidth = m_opt.vScreenWidth;
			int oldHeight = m_opt.vScreenHeight;

			m_opt.vFillMode = m_newOpt.vFillMode;
			m_opt.vWideMode = m_newOpt.vWideMode;
			m_opt.vWindowMode = m_newOpt.vWindowMode;
			vChangeGameScreenSize(m_newOpt.vScreenWidth, m_newOpt.vScreenHeight,
				m_newOpt.vScreenColor, true);
			vApplyGameScreenSize();

			if (CSceneManager::IsInstantiated())
			{
				CSceneManager::Instance()->UpdateFog(false);
			}

			if (oldWidth != m_opt.vScreenWidth ||
				oldHeight != m_opt.vScreenHeight ||
				oldBpp != m_opt.vScreenColor)
			{
				MsgObject msg;
				msg.message = 0x271B;
				msg.sender = 0;
				msg.param1 = 0;
				msg.param2 = 0;
				msg.param3 = 0;
				msg.time = 0;
				AfxGetTask()->SendGlobalMsg(msg);

				if (g_pFresh)
				{
					g_pFresh->GetDesktop()->ResetRect();
				}
			}
		}
		else
		{
			m_opt.vFillMode = m_newOpt.vFillMode;
			m_opt.vWideMode = m_newOpt.vWideMode;
			m_opt.vWindowMode = m_newOpt.vWindowMode;
			vChangeGameScreenSize(m_newOpt.vScreenWidth, m_newOpt.vScreenHeight,
				m_newOpt.vScreenColor, true);
			if (CShadowManager::IsInstantiated())
			{
				CShadowManager::Instance()->Reset();
			}
			vChangeScreenSize(800, 600, m_opt.vScreenColor);
		}

		if (CChatMsg::IsInstantiated())
		{
			CChatMsg::Instance()->ResetUiPosition();
		}
	}

	m_opt.vOverall = m_newOpt.vOverall;
	m_opt.vTnLMode = m_newOpt.vTnLMode;
	m_opt.vDDSRes = m_newOpt.vDDSRes;
	m_opt.vUseMipmap = m_newOpt.vUseMipmap;
	m_opt.vMaxMipLvl = m_newOpt.vMaxMipLvl;
	m_opt.vMipCreateFilter = m_newOpt.vMipCreateFilter;
	m_opt.vEffectLevel = m_newOpt.vEffectLevel;

	if (!WisEqual(m_opt.vGamma, m_newOpt.vGamma, g_EPSILON))
	{
		m_opt.vGamma = m_newOpt.vGamma;
	}

	if (m_opt.vProjectionShadow != m_newOpt.vProjectionShadow)
	{
		m_opt.vProjectionShadow = m_newOpt.vProjectionShadow;

		if (CShadowManager::IsInstantiated())
		{
			CShadowManager::Instance()->Enable(m_opt.vProjectionShadow != 0);
		}
	}

	if (m_opt.vShadowmap != m_newOpt.vShadowmap)
	{
		m_opt.vShadowmap = m_newOpt.vShadowmap;

		if ((__rtti_obj = AfxGetTask()) != 0
				? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI)
				: false)
		{
			if (GOLFDOC() && GOLFDOC()->m_pPolySoup)
			{
				GOLFDOC()->m_pPolySoup->EnableShadowmap(m_opt.vShadowmap != 0);
			}
		}
	}

	if (m_opt.vReflection != m_newOpt.vReflection)
	{
		m_opt.vReflection = m_newOpt.vReflection;

		if (CSceneManager::IsInstantiated())
		{
			CSceneManager::Instance()->SetReflection(m_opt.vReflection != 0);
		}
	}

	if (m_opt.vLod != m_newOpt.vLod)
	{
		m_opt.vLod = m_newOpt.vLod;

		if (CSceneManager::IsInstantiated())
		{
			CSceneManager::Instance()->SetLOD(m_opt.vLod != 0, true);
		}
	}

	m_opt.vNpc = m_newOpt.vNpc;

	m_opt.aOverall = m_newOpt.aOverall;

	if (m_opt.aMssOn != m_newOpt.aMssOn ||
		m_opt.aMssFreq != m_newOpt.aMssFreq ||
		m_opt.aMssBits != m_newOpt.aMssBits ||
		m_opt.aMssChannels != m_newOpt.aMssChannels ||
		m_opt.aMssEnableHwSound != m_newOpt.aMssEnableHwSound ||
		m_opt.aMssBalance != m_newOpt.aMssBalance ||
		m_opt.aMssSpeaker != m_newOpt.aMssSpeaker)
	{
		m_opt.aMssOn = m_newOpt.aMssOn;
		m_opt.aMssFreq = m_newOpt.aMssFreq;
		m_opt.aMssBits = m_newOpt.aMssBits;
		m_opt.aMssChannels = m_newOpt.aMssChannels;
		m_opt.aMssEnableHwSound = m_newOpt.aMssEnableHwSound;
		m_opt.aMssBalance = m_newOpt.aMssBalance;
		m_opt.aMssSpeaker = m_newOpt.aMssSpeaker;

		g_audio->SetSpeakerType(m_opt.aMssSpeaker);
	}

	if (m_opt.aMssOn)
	{
		if (!WisZero(m_opt.aSfxVolume - m_newOpt.aSfxVolume, g_EPSILON))
		{
			aSetSfxVolume(m_newOpt.aSfxVolume);
		}

		if (!(Wabs(m_opt.aBGMVolume - m_newOpt.aBGMVolume) < g_EPSILON))
		{
			aSetBgmVolume(m_newOpt.aBGMVolume);
		}
	}

	if (!(Wabs(m_opt.gMouseSensitivity - m_newOpt.gMouseSensitivity) <
			g_EPSILON))
	{
		gSetMouseSensitivity(m_newOpt.gMouseSensitivity);
	}

	if (m_opt.gWhisper != m_newOpt.gWhisper)
	{
		m_opt.gWhisper = m_newOpt.gWhisper;

		WSendPacket packet((enumClientPacket)0x55);
		packet.Encode1((unsigned char)m_opt.gWhisper);
		packet.Send(TO_GAME);
	}

	m_opt.gInvitation = m_newOpt.gInvitation;

	if (m_opt.gFriendConfirm != m_newOpt.gFriendConfirm)
	{
		m_opt.gFriendConfirm = m_newOpt.gFriendConfirm;
	}

	m_opt.gTransChatWin = m_newOpt.gTransChatWin;

	if (m_opt.gUiEffect != m_newOpt.gUiEffect)
	{
		m_opt.gUiEffect = m_newOpt.gUiEffect;
		FrForm::EnableTail(m_opt.gUiEffect != 0);
	}

	if (m_opt.gExtendChatLine != m_newOpt.gExtendChatLine)
	{
		m_opt.gExtendChatLine = m_newOpt.gExtendChatLine;
	}

	if (m_opt.gPowerGauge != m_newOpt.gPowerGauge)
	{
		m_opt.gPowerGauge = m_newOpt.gPowerGauge;
		m_pVideoDev->Command(W_VDEV_SET_CAPTURE_MODE, m_opt.gPowerGauge,
			m_opt.gPowerGauge);

		if (m_opt.vWindowMode)
		{
			long style = m_opt.vFillMode ? 0x90080000 : 0x00C40000;
			m_pDeviceManager->ResetVideoDevice(true, m_pVideoDev->GetWidth(),
				m_pVideoDev->GetHeight(), m_opt.vScreenColor, style,
				m_opt.vFillMode);
		}
		else
		{
			m_pDeviceManager->ResetVideoDevice(false, m_pVideoDev->GetWidth(),
				m_pVideoDev->GetHeight(), m_opt.vScreenColor, 0x90080000, 0);
		}
	}

	m_opt.gCaptureHideGUI = m_newOpt.gCaptureHideGUI;
	m_opt.gCaptureHidePI = m_newOpt.gCaptureHidePI;
	m_opt.gTerrainTooltip = m_newOpt.gTerrainTooltip;
	m_opt.gCutinDisplay = m_newOpt.gCutinDisplay;

	for (int i = 0; i < 8; i++)
	{
		std::string text(m_newOpt.mMacro[i]);

		if (text.length() == 0 ||
			text.find_first_not_of(' ', 0) == std::string::npos)
		{
			sprintf(m_opt.mMacro[i],
				"\xc3\xa4\xc6\xc3 \xb8\xc5\xc5\xa9\xb7\xce\xb8\xa6 \xbc\xb3\xc1\xa4\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
		}
		else
		{
			strcpy(m_opt.mMacro[i], m_newOpt.mMacro[i]);
		}
	}

	m_opt.mSaveMacros = m_newOpt.mSaveMacros;

	SaveReg();
}

const char* COption::vGetScrResText(int width, int height, int bpp,
	bool windowed)
{
	const char* text = NULL;

	std::list<sDisplayMode>::iterator it;
	for (it = m_displayModeList.begin(); it != m_displayModeList.end(); ++it)
	{
		if (width == (*it).width && height == (*it).height)
		{
			if (windowed)
			{
				text = (*it).text;
				if (bpp == (*it).color)
				{
					return text;
				}
			}
			else if (bpp == (*it).color)
			{
				return (*it).text;
			}
		}
	}

	return text ? text : "";
}

const char* COption::vGetScrResText(int index)
{
	int i = 0;

	for (std::list<sDisplayMode>::iterator it = m_displayModeList.begin();
		it != m_displayModeList.end(); ++it)
	{
		if (i == index)
		{
			return (*it).text;
		}
		i++;
	}

	return "";
}

int COption::vGetScrResNum(void)
{
	return m_displayModeList.size();
}

void COption::vGetScrResLvl(const char* text, int* width, int* height, int* bpp)
{
	for (std::list<sDisplayMode>::iterator it = m_displayModeList.begin();
		it != m_displayModeList.end(); ++it)
	{
		if (strcmpi(text, (*it).text) == 0)
		{
			*width = (*it).width;
			*height = (*it).height;
			*bpp = (*it).color;
			return;
		}
	}
}

void COption::vCheckScreenSize(int width, int height, int bpp)
{
	for (std::list<sDisplayMode>::iterator it = m_displayModeList.begin();
		it != m_displayModeList.end(); ++it)
	{
		if (width == (*it).width && height == (*it).height)
		{
			if (m_opt.vWindowMode)
			{
				m_opt.vScreenWidth = width;
				m_opt.vScreenHeight = height;
				m_opt.vScreenColor = (*it).color;
				return;
			}

			if (bpp == (*it).color)
			{
				m_opt.vScreenWidth = width;
				m_opt.vScreenHeight = height;
				m_opt.vScreenColor = bpp;
				return;
			}
		}
	}
}

void COption::CheckDisplayModeList(WVideoDev* videoDevice, bool windowed,
	bool wide)
{
	if (!videoDevice)
	{
		return;
	}

	m_displayModeList.clear();

	if (wide)
	{
		for (int i = 0; i < sizeof(s_wideModes) / sizeof(s_wideModes[0]); i++)
		{
			sDisplayMode mode;
			mode.width = s_wideModes[i].width;
			mode.height = s_wideModes[i].height;
			mode.color = s_wideModes[i].color;

			sprintf(mode.text, "%dx%dx%d", mode.width, mode.height, mode.color);

			if (videoDevice->IsSupportedDisplayMode(windowed, mode.width,
					mode.height, mode.color))
			{
				m_displayModeList.push_back(mode);
			}
		}
	}
	else
	{
		for (int i = 0; i < sizeof(s_normalModes) / sizeof(s_normalModes[0]);
			i++)
		{
			sDisplayMode mode;
			mode.width = s_normalModes[i].width;
			mode.height = s_normalModes[i].height;
			mode.color = s_normalModes[i].color;

			sprintf(mode.text, "%dx%dx%d", mode.width, mode.height, mode.color);

			if (videoDevice->IsSupportedDisplayMode(windowed, mode.width,
					mode.height, mode.color))
			{
				m_displayModeList.push_back(mode);
			}
		}
	}
}

void COption::ChangeGameOption(sOption& option, sGameOptionInfo& info)
{
	for (int i = 0; i < 8; i++)
	{
		strcpy(info.macro[i], option.mMacro[i]);
	}
}

void COption::ChangeGameOption(sGameOptionInfo& info, sOption& option)
{
	for (int i = 0; i < 8; i++)
	{
		strcpy(option.mMacro[i], info.macro[i]);
	}
}

void COption::SendUpdateGameOptionPacket(void)
{
	sGameOptionInfo info;

	COption::Instance()->ChangeGameOption(m_opt, info);

	WSendPacket packet((enumClientPacket)0x69);
	packet.EncodeBuffer(&info, sizeof(info));
	packet.Send(TO_GAME);
}
