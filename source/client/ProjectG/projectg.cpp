#include "wproc.h"
#include "miles.h"
#include "clientsetting.h"
#include "fpsmonitor.h"
#include "dxdiaginfo.h"
#include "networkmonitor.h"
#include "projectg.h"
#include "gamedatadb.h"
#include "voiceitem.h"
#include "contentsdoc.h"
#include "matchhistory.h"
#include "thread.h"
#include "noticeboard.h"
#include "effectmanager.h"
#include "buddymanager.h"
#include "netresourcemanager.h"
#include "rankinginfo.h"
#include "onelineboard.h"
#include "messengerdlg.h"
#include "gmtoolkit.h"
#include "replaydlg.h"
#include "dlgquickstartsetting.h"
#include "pangfbi.h"
#include "eventthread.h"
#include "messagemanager.h"
#include "winnernotice.h"
#include "topiconinterface.h"
#include "topiconmanager.h"
#include "ghostmanager.h"
#include "ghosthandler.h"
#include "shufflebonus.h"
#include "intrusion.h"
#include "powergauge.h"
#include "lobbybg.h"
#include "mathconsts.h"
#include "../../shared/LuaSystem/luascriptmanager.h"
#include "md5.h"
#include "fx.h"
#include "mousecursor.h"
#include "user_info.h"
#include "logininfo.h"
#include "browser.h"
#include "cardsystem.h"
#include "headicon.h"
#include "treasure.h"
#include "integritycheck.h"
#include "chatmsg.h"
#include "golftask.h"
#include "registry.h"
#include <float.h>
extern "C" __declspec(dllimport) int __cdecl mkdir(const char*);
#include <io.h>
#include <stdarg.h>

extern HWND g_hwnd;
extern bool g_bQuit;
extern WResourceManager* g_resrcmng;
extern WMilesSoundSystem* g_miles;
extern WInputDev* g_keyboard;
extern WInputDev* g_mouse;
extern WFont* g_font;

void InitSingletonClasses();
void DeleteSingletonClasses();
void ExecuteStartMode();
void __cdecl ReleasePAK();
bool __stdcall GetCRCCheckList(
	std::list<std::pair<std::string, unsigned long> >& list);
void RegisterTitles(TitleManager* manager);

static char captureMessage[256] = { 0 };
char g_colTex[16][32] = { 0 };
int g_last_screen_height = 0;
int g_last_screen_width = 0;
TitleManager* g_TitleManager = 0;
WFont* g_font = 0;
Fresh* g_pFresh = 0;
unsigned long g_CurrentTime = 0;
char g_executeDirectory[MAX_PATH] = { 0 };
WInputDev* g_keyboard = 0;
WInputDev* g_mouse = 0;
WInputDev* g_ime = 0;
WView* g_view = 0;
HWND g_hwnd = 0;
WResourceManager* g_resrcmng = 0;
CSoundManager* g_audio = 0;
WMilesSoundSystem* g_miles = 0;
static int processCount = 0;
static int drawCount = 0;
int g_nDrawPoly = 0;
int g_nModel = 0;
int g_nDrawModel = 0;
int g_colNum = 0;
unsigned long* g_pZuffHistogram = 0;
bool g_bAviCapture = 0;
bool g_bQuit = 0;
static unsigned long captureMessageTime = 0;
int g_iSendPacketType = 0;
int g_iRecvPacketType = 0;
int g_iRecvDisconCode = 0;
int g_iSendFailType = 0;
WMatrix g_camera;
CFpsMonitor g_FpsMonitor;
CFpsMonitor g_VramMonitor;
bool g_bFocus = true;
bool g_bOldFocus = true;
struct UIFileChecksum
{
	const char* filename;
	unsigned char digest[16];
};
static UIFileChecksum uiChecks[] = {
	{ "[left_ht.jpg",
     { 0x5b, 0xa3, 0x60, 0x07, 0x2f, 0x73, 0x87, 0x21, 0xef, 0xbd, 0xcd,
			0xd4, 0x45, 0x23, 0xfc, 0xfa } },
	{ "[middle.jpg",
     { 0x7b, 0x95, 0xfc, 0xe6, 0x44, 0xab, 0xd1, 0x81, 0xa8, 0x53, 0xd1,
			0x56, 0xf9, 0xf4, 0xa2, 0x8f } },
	{ "[right_wind_b.jpg",
     { 0xed, 0xdd, 0x43, 0xfd, 0x5e, 0x65, 0x91, 0x86, 0x15, 0x1c, 0x80,
			0x79, 0x32, 0x41, 0x77, 0x1b } }
};
static int captureCountdown = -1;
LightSet g_lightset;

extern char PY_PUBLIC_VERSION[];

void __cdecl TextOut(WView* view, float x, float y, const char* format, ...)
{
	char text[2048];
	va_list args;
	va_start(args, format);
	if (_vsnprintf(text, sizeof(text) - 1, format, args) == -1)
		text[sizeof(text) - 1] = 0;
	va_end(args);
	g_font->Print(view, x, y, text, 0, 0xffffffff, NULL);
}

CTmpLogo::CTmpLogo()
	: m_pLogo(NULL)
{
}

CTmpLogo::~CTmpLogo()
{
	delete m_pLogo;
}

static CTmpLogo captureLogo;

static bool SaveScreenShot(const char* filename)
{
	Bitmap* image = new Bitmap(g_last_screen_width, g_last_screen_height, 24);
	if (g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURE_SCREEN, 0,
			(int)image) == 1)
	{
		if (COption::Instance()->GetOption()->gCaptureLogo == 1 &&
			Doc()->m_playingMode != 2)
		{
			if (!captureLogo.m_pLogo)
				captureLogo.m_pLogo = g_resrcmng->LoadTGA("pangya_s3.tga");
			if (captureLogo.m_pLogo)
			{
				Bitmap* logo = captureLogo.m_pLogo;
				int height = logo->bi->bmiHeader.biHeight;
				int width = logo->bi->bmiHeader.biWidth;
				int x = image->bi->bmiHeader.biWidth - width;
				int y = image->bi->bmiHeader.biHeight - height;
				if (x > 0 && y > 0)
				{
					for (int row = 0; row < height; ++row)
					{
						for (int col = 0; col < width; ++col)
						{
							unsigned char r2, g2, b2, a;
							unsigned char r, g, b;
							image->GetPixel(col + x, row + y, r, g, b);
							logo->GetPixel(col, row, r2, g2, b2, a);
							int alpha = a;
							int inverseAlpha = 255 - a;
							image->SetPixel(col + x, row + y,
								(inverseAlpha * r + alpha * r2) / 255,
								(inverseAlpha * g + alpha * g2) / 255,
								(inverseAlpha * b + alpha * b2) / 255, 255);
						}
					}
				}
			}
		}
		if (strrchr(filename, '.'))
		{
			if (!strcmpi(strrchr(filename, '.'), ".jpg"))
			{
				SaveJPG(filename, image, 100);
				delete image;
				return true;
			}
			if (!strcmpi(strrchr(filename, '.'), ".bmp"))
			{
				SaveBMP(filename, image);
				delete image;
				return true;
			}
		}
	}
	delete image;
	return false;
}

void InitSingletonClasses()
{
	new CPowerGauge;
	new CMatchHistory;
	new CGhostManager;
	new CGhostHandler;
	new CSoundItemManager;
	new CShuffleBonus;
	new CIntrusion;
}

void DeleteSingletonClasses()
{
	if (CIntrusion::IsInstantiated())
		delete CIntrusion::Instance();
	if (CShuffleBonus::IsInstantiated())
		delete CShuffleBonus::Instance();
	if (CSoundItemManager::IsInstantiated())
		delete CSoundItemManager::Instance();
	if (CGhostHandler::IsInstantiated())
		delete CGhostHandler::Instance();
	if (CGhostManager::IsInstantiated())
		delete CGhostManager::Instance();
	if (CMatchHistory::IsInstantiated())
		delete CMatchHistory::Instance();
	if (CPowerGauge::IsInstantiated())
		delete CPowerGauge::Instance();
}

CProjectG::CProjectG(HWND hwnd)
	: m_inputList(16, 16)
{
	g_CurrentTime = timeGetTime();
	srand(g_CurrentTime);
	m_threadId = GetCurrentThreadId();
	m_pDeviceManager = NULL;
	m_pProcManager = NULL;
	m_bMoveLoginServer = false;
	m_bLostFocus = false;
	m_pDeviceManager = new WDeviceManager(NULL, 9);
	m_pProcManager = new WProcManager;
	g_resrcmng = new WResourceManager;
	WFile::SetResourceManager(g_resrcmng);
	m_baseTime = g_CurrentTime;
	m_reserved = 4;
	new _systemmsg::CMessageManager;
	new CSharedDoc;
	new COption(hwnd, m_pDeviceManager, g_resrcmng);
	new HackingManager;
	ResetTimer();
	m_refreshRate = 60.0f;
	new CTaskManager;
	new BuddyManager;
	new CGMToolkit;
	if (IsLocalContent((localContentType_t)79))
		new CReplayControlManager;
	if (IsLocalContent((localContentType_t)81))
		new CCardManager;
	new CPangFBI;
	m_bHidePrivacy = false;
	m_bHideGUI = false;
	m_bHidePI = false;
	m_pDxDiagInfo = NULL;
	m_pDxDiagInfo = new CDxDiagInfo;
	g_TitleManager = new TitleManager;
	RegisterTitles(g_TitleManager);
	new CEventThread;
	m_prevCamera.Reset();
	InitSingletonClasses();
}

CProjectG::~CProjectG()
{
	while (!m_recvPacketQueue.empty())
	{
		delete m_recvPacketQueue.front();
		m_recvPacketQueue.pop();
	}
	if (g_view)
	{
		g_resrcmng->Release(g_view);
		g_view = NULL;
	}
	if (g_font)
	{
		g_resrcmng->Release(g_font);
		g_font = NULL;
	}
	DeleteSingletonClasses();
	if (CPangFBI::IsInstantiated())
		delete CPangFBI::Instance();
	if (IsLocalContent((localContentType_t)88))
		if (CTHunter::IsInstantiated())
			delete CTHunter::Instance();
	if (CMatchingSystem::IsInstantiated())
		delete CMatchingSystem::Instance();
	if (CGMToolkit::IsInstantiated())
		delete CGMToolkit::Instance();
	if (BuddyManager::IsInstantiated())
		delete BuddyManager::Instance();
	if (CHeadIcon::IsInstantiated())
		delete CHeadIcon::Instance();
	if (CTaskManager::IsInstantiated())
		delete CTaskManager::Instance();
	if (CChatMsg::IsInstantiated())
		delete CChatMsg::Instance();
	if (CNetworkMonitor::IsInstantiated())
		delete CNetworkMonitor::Instance();
	if (WNetworkSystem::IsInstantiated())
		delete WNetworkSystem::Instance();
	if (CMouseCursor::IsInstantiated())
		delete CMouseCursor::Instance();
	if (CUserInfo::IsInstantiated())
		delete CUserInfo::Instance();
	if (CMessengerInfo::IsInstantiated())
		delete CMessengerInfo::Instance();
	if (CSharedDoc::IsInstantiated())
		delete Doc();
	if (CContentsDoc::IsInstantiated())
		delete CContentsDoc::Instance();
	if (COption::IsInstantiated())
		delete COption::Instance();
	if (HackingManager::IsInstantiated())
		delete HackingManager::Instance();
	if (CNoticeBoard::IsInstantiated())
		delete CNoticeBoard::Instance();
	if (CWinnerNotice::IsInstantiated())
		delete CWinnerNotice::Instance();
	if (CFx::IsInstantiated())
		delete CFx::Instance();
	if (CAztecHoleinEffect::IsInstantiated())
		delete CAztecHoleinEffect::Instance();
	if (NetResourceManager::IsInstantiated())
		delete NetResourceManager::Instance();
	if (COneLineBoard::IsInstantiated())
		delete COneLineBoard::Instance();
	if (CRankingInfo::IsInstantiated())
		delete CRankingInfo::Instance();
	if (IsLocalContent((localContentType_t)79))
		if (CReplayControlManager::IsInstantiated())
			delete CReplayControlManager::Instance();
	if (IsLocalContent((localContentType_t)81))
		if (CCardManager::IsInstantiated())
			delete CCardManager::Instance();
	if (IsLocalContent((localContentType_t)85))
		if (CAztecHoleinEffect::IsInstantiated())
			delete CAztecHoleinEffect::Instance();
	if (CGameDataDB::IsInstantiated())
		delete CGameDataDB::Instance();
	if (CIconManager::IsInstantiated())
		delete CIconManager::Instance();
	if (g_pFresh)
	{
		g_pFresh->GetManager()->DestroyToolTip();
		if (g_pFresh)
		{
			delete g_pFresh;
			g_pFresh = NULL;
		}
	}
	if (g_input)
	{
		delete g_input;
		g_input = NULL;
	}
	if (g_audio)
	{
		delete g_audio;
		g_audio = NULL;
	}
	if (g_miles)
	{
		delete g_miles;
		g_miles = NULL;
	}
	if (g_resrcmng)
	{
		delete g_resrcmng;
		g_resrcmng = NULL;
	}
	if (m_pDeviceManager)
	{
		delete m_pDeviceManager;
		m_pDeviceManager = NULL;
	}
	if (m_pProcManager)
	{
		delete m_pProcManager;
		m_pProcManager = NULL;
	}
	if (m_pDxDiagInfo)
	{
		delete m_pDxDiagInfo;
		m_pDxDiagInfo = NULL;
	}
	if (g_TitleManager)
	{
		delete g_TitleManager;
		g_TitleManager = NULL;
	}
	if (CEventThread::IsInstantiated())
		delete CEventThread::Instance();
	if (_systemmsg::CMessageManager::IsInstantiated())
		delete _systemmsg::CMessageManager::Instance();
	if (CVoiceItem::Instance())
	{
		CVoiceItem::Instance()->Destroy();
		if (CVoiceItem::IsInstantiated())
			delete CVoiceItem::Instance();
	}
	if (m_avi.IsOpened())
		m_avi.Close();
	ReleasePAK();
}

void CALLBACK MyTimerProc(HWND hwnd, UINT message, UINT timer, DWORD time)
{
}

void CProjectG::FPU_Check()
{
	m_fpuControl = _controlfp(0, 0);
	if (m_fpuControl != 0xa001f)
		m_fpuControl = _controlfp(_PC_24, _MCW_PC);
}

void CProjectG::SetMoveLoginServer(const char* id, const char* password)
{
	m_bMoveLoginServer = true;
	m_moveLoginId = id;
	m_moveLoginPassword = password;
}

bool CProjectG::Ready()
{
	FPU_Check();
	SetCursor(NULL);
	if (IsLocalContent(S3_ENCRYPTION_LV1))
	{
		if (!InitPakedFile2())
			return false;
	}
	else
		InitPakedFile();
	Doc()->LoadItemDb();
	Doc()->InitGolfDoc();
	Doc()->LoadChatFilter();
	Doc()->LoadVisGroup("visgroup.def");
	new CContentsDoc;
	if (!CContentsDoc::Instance()->Initialize())
		return false;
	DeleteFile("GameGuard/GameGuard.err");
	char commandLine[MAX_PATH];
	strcpy(commandLine, GetCommandLine());
	strlwr(commandLine);
	if (strstr(commandLine, "-integrity"))
	{
		CIntegrityCheck check;
		if (!check.Report("ResrcIntegrity.txt"))
			g_bQuit = true;
	}
	GetCurrentDirectory(MAX_PATH, g_executeDirectory);
	g_view = g_resrcmng->GetView(0);
	g_input = new CInputManager;
	COption::Instance()->CheckDebugRegValid();
	COption::Instance()->SetVideoDevice(g_resrcmng->VideoReference());
	COption::Instance()->SetGlobalView(g_view);
	COption::Instance()->SetAudioManager(g_audio);
	COption::Instance()->SetInputManager(g_input);
	UploadShaderSource();
	g_font = g_resrcmng->GetTexFont("data/Misc/font.bmp");
	LoadKeyLayout();
	new CChatMsg;
	new WNetworkSystem;
	new CNetworkMonitor;
	new CMouseCursor;
	new CHeadIcon;
	new CUserInfo;
	new CMessengerInfo;
	new CNoticeBoard;
	new CWinnerNotice;
	new CFx;
	new CAztecHoleinEffect;
	new NetResourceManager;
	new COneLineBoard;
	new CRankingInfo;
	if (IsLocalContent((localContentType_t)85))
		new CAztecHoleinEffect;
	if (IsLocalContent(S4_TREASURE_HUNTER))
	{
		new CTHunter;
		CTHunter::Instance()->Initialize(20);
	}
	if (IsLocalContent(S4_MATCHING_SYSTEM))
		new CMatchingSystem;
	new CGameDataDB;
	new CIconManager;
	new CVoiceItem;
	CVoiceItem::Instance()->Initialize();
	g_pFresh = new Fresh;
	g_pFresh->Init("all.xml", true, false);
	g_pFresh->GetManager()->CreateToolTip();
	COption::Instance()->dApplyDebugSetting();
	CNetworkMonitor::Instance()->Init();
	g_FpsMonitor.Init(200, 200, WRect(10, 300, 200, 100));
	g_VramMonitor.Init(200, 600, WRect(10, 300, 200, 100));
	g_VramMonitor.SetColor(0, 0xff0000ff);
	int course = COption::Instance()->dGetLobbyCourse();
	if (course >= 0)
	{
		LOBBYBG_MAP = course;
		int map = course;
		if (course > 127 && course != 253)
			map = course - 128;
		CItemManager* items = &Doc()->m_itemManager;
		LOBBYBG_MAPNAME = items->FindCourse(map | 0x28000000)->Data;
	}
	ExecuteStartMode();
	SYSTEM_INFO systemInfo;
	memset(&systemInfo, 0, sizeof(systemInfo));
	GetSystemInfo(&systemInfo);
	if (systemInfo.dwNumberOfProcessors > 1)
	{
		std::list<std::pair<std::string, unsigned long> > checks;
		if (!GetCRCCheckList(checks))
			return false;
		for (std::list<std::pair<std::string, unsigned long> >::iterator it =
				 checks.begin();
			it != checks.end(); ++it)
			m_crc.AddWork((*it).first.c_str(), (*it).second);
		m_crc.SetSleepTime(1);
		m_crc.Begin(g_hwnd);
	}
	return true;
}

void ExecuteStartMode()
{
	CTaskManager::Instance()->CreateTask("CLobbyTask", "CGolfDoc", false);
	CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"LOGIN", 0, 0, 0);
}

void CProjectG::LoadKeyLayout()
{
	struct KeyBinding
	{
		const char* name;
		int key;
		unsigned long flags;
		int type;
	};
	KeyBinding bindings[] = {
		{ "\xbb\xf3", 6 },
		{ "\xc7\xcf", 7 },
		{ "\xc1\xc2", 4 },
		{ "\xbf\xec", 5 },
		{ "\xbb\xf3", 87 },
		{ "\xc7\xcf", 83 },
		{ "\xc1\xc2", 65 },
		{ "\xbf\xec", 68 },
		{ "LSHIFT", 129, 1 },
		{ "RSHIFT", 130, 1 },
		{ "\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", 32 },
		{ "PAGE_DOWN", 3, 1 },
		{ "PAD_PLUS", 140 },
		{ "PAGE_UP", 2, 1 },
		{ "PAD_MINUS", 136 },
		{ "\xb5\xf0\xb9\xf6\xb1\xd7\xc5\xb0", 8 },
		{ "HOME", 12 },
		{ "END", 26 },
		{ "PAD_0", 144 },
		{ "NUM_0", 48 },
		{ "PAD_1", 141 },
		{ "NUM_1", 49 },
		{ "PAD_2", 142 },
		{ "NUM_2", 50 },
		{ "PAD_3", 143 },
		{ "NUM_3", 51 },
		{ "PAD_4", 137 },
		{ "NUM_4", 52 },
		{ "PAD_5", 138 },
		{ "NUM_5", 53 },
		{ "PAD_6", 139 },
		{ "NUM_6", 54 },
		{ "PAD_7", 133 },
		{ "NUM_7", 55 },
		{ "PAD_8", 134 },
		{ "NUM_8", 56 },
		{ "PAD_9", 135 },
		{ "NUM_9", 57 },
		{ "ITEMSLOT", 73 },
		{ "\xc1\xa4\xc1\xf6", 80 },
		{ "LALT", 28, 1 },
		{ "RALT", 29, 1 },
		{ "LCONTROL", 30 },
		{ "RCONTROL", 31 },
		{ "TAB", 9, 1 },
		{ "FASTFORWARD", 32 },
		{ "\xc5\xd7\xbd\xba\xc6\xae", 84 },
		{ "PARAM_UPDATE", 85 },
		{ "CONSOLE", 10, 1 },
		{ "F15", 102, 1 },
		{ "FUNC_1", 14, 1 },
		{ "FUNC_2", 15, 1 },
		{ "FUNC_3", 16, 1 },
		{ "FUNC_4", 17, 1 },
		{ "FUNC_5", 18, 1 },
		{ "FUNC_6", 19, 1 },
		{ "FUNC_7", 20, 1 },
		{ "FUNC_8", 21, 1 },
		{ "FUNC_9", 22, 1 },
		{ "FUNC_10", 23, 1 },
		{ "FUNC_11", 24, 1 },
		{ "FUNC_12", 25, 1 },
		{ "FUNC_15", 102, 1 },
		{ "INSERT", 127, 1 },
		{ "ESCAPE", 27, 1 },
		{ "ENTER", 13, 1 },
		{ "PADENTER", 145, 1 },
		{ "HIDE", 72 },
		{ "JUKEBOX", 77 },
		{ "REPLAY", 82 },
		{ "SKIP", 83 },
		{ "CANCEL", 67 },
		{ "FPS_SHOT", 70 },
		{ "BALL_TRACE", 71 },
		{ "PARTICLE_1", 46 },
		{ "PARTICLE_2", 44 },
		{ "\xb8\xc5\xc5\xa9\xb7\xce\xc3\xa2", 96 },
		{ "\xc1\xa4\xc1\xf6"
		  "2", 86 },
		{ "EMOTICON", 69 },
		{ "BANISH", 1, 1 },
		{ "RANK", 81 },
		{ "ROTATE_CW", 81 },
		{ "MESSENGER", 127, 1 },
		{ "ROTATE_CCW", 69 },
		{ "DEBUG_INFO", 146, 1 },
		{ "PLAYER_INFO", 147, 0, 0 },
		{ "CALIPERS_RIGHT", 88 },
		{ "CALIPERS_LEFT", 90 },
		{ "\xb0\xed\xbc\xd3", 78 },
		{ "\xc0\xfa\xbc\xd3", 66 },
		{ "UNDO", 90 },
		{ "REDO", 89 },
		{ "FLIP", 70 },
		{ "SAVE", 83 },
		{ "PAINT", 80 },
		{ "GRID", 71 },
		{ "DRAG", 68 },
		{ NULL },
	};
	for (int i = 0; bindings[i].name; ++i)
		g_input->Register(bindings[i].name, bindings[i].key, bindings[i].flags,
			bindings[i].type == 1 ? 3 : 2);
	g_input->ExcludeKey("BALL_TRACE");
	g_input->ExcludeKey("PARAM_UPDATE");
	g_input->ExcludeKey("FPS_SHOT");
	g_input->ExcludeKey("\xc5\xd7\xbd\xba\xc6\xae");
	g_input->ExcludeKey("\xb5\xf0\xb9\xf6\xb1\xd7\xc5\xb0");
	g_input->ExcludeKey("PARTICLE_1");
	g_input->ExcludeKey("PARTICLE_2");
	g_input->ExcludeKey("\xc1\xa4\xc1\xf6");
	g_input->ExcludeKey("\xc1\xa4\xc1\xf6"
						"2");
	g_input->ExcludeKey("\xb0\xed\xbc\xd3");
	g_input->ExcludeKey("\xc0\xfa\xbc\xd3");
}

int MainLoop()
{
	int beingDebugged = 1;
	__asm
	{
		mov eax, fs:[18h]
		mov eax, [eax + 30h]
		movzx eax, byte ptr [eax + 2]
		mov beingDebugged, eax
	}
	return beingDebugged;
}

int CProjectG::MainLoop(int flags)
{
	FPU_Check();
	g_CurrentTime = timeGetTime();
	WNetworkSystem::Instance()->Process();
	ProcessRecvPacketQueue();
	WFlags captureFlags = m_flag;
	int mainFlags = m_mainFlags;
	if (!bool(captureFlags & 2))
		return Main(mainFlags | 0x80);
	return Main(mainFlags);
}

void CProjectG::CheckScreenShot()
{
	if (g_input->GetDown("FUNC_11", true) || Doc()->GetReplayState() == 6)
	{
		if (IsLocalContent(S4_REPLAY_SYSTEM) &&
			(Doc()->GetReplayState() == 6 || Doc()->GetReplayState() == 9))
		{
			__int64 freeSpace = CalcHddFreeSpace();
			if (!freeSpace)
			{
				CChatMsg::Instance()->AddChatMsg(
					"\xc7\xcf\xb5\xe5\xc0\xc7 \xbf\xeb\xb7\xae\xc0\xbb \xc3\xbc\xc5\xa9 \xc7\xd2 \xbc\xf6 \xbe\xf8\xc0\xb8\xb9\xc7\xb7\xce \xb5\xbf\xbf\xb5\xbb\xf3 \xc6\xc4\xc0\xcf\xc0\xbb \xbb\xfd\xbc\xba \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xffff7878, true, false);
				return;
			}
			if (freeSpace < 0x40000000)
			{
				CChatMsg::Instance()->AddChatMsg(
					"\xc7\xcf\xb5\xe5\xc0\xc7 \xbf\xeb\xb7\xae\xc0\xcc \xba\xce\xc1\xb7\xc7\xcf\xbf\xa9 avi\xc6\xc4\xc0\xcf\xc0\xbb \xbb\xfd\xbc\xba\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xffff7878, true, false);
				return;
			}
			COption::Instance()->dSetCaptureFormat(2);
			if (Doc()->GetReplayState() == 6)
				Doc()->SetReplayState(7);
		}
		else
			COption::Instance()->dSetCaptureFormat(0);
		m_bHideGUI = COption::Instance()->gGetCaptureHideGUI() == 1;
		m_bHidePI = COption::Instance()->gGetCaptureHidePI() == 1;
		if (g_input->Get("LCONTROL", false) ||
			g_input->Get("RCONTROL", false) || m_bHidePI)
		{
			m_bHidePrivacy = true;
			g_pFresh->HidePrivacy(true);
		}
		captureCountdown = 3;
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 505, 0, 0, 0, 0, 0));
	}
	if (captureCountdown > 0)
		--captureCountdown;
	if (captureCountdown == 0)
	{
		m_bHidePrivacy = false;
		captureCountdown = -1;
		g_pFresh->HidePrivacy(false);
		char suffix[4] = "";
		char description[64] = "";
		if (m_bHideGUI)
		{
			strcat(suffix, "G");
			strcat(description, "(GUI");
			if (!m_bHidePI)
				strcat(description, "\xc1\xa6\xb0\xc5)");
		}
		if (m_bHidePI)
		{
			strcat(suffix, "U");
			if (m_bHideGUI)
				strcat(description, ", ");
			else
				strcat(description, "(");
			strcat(description,
				"\xb0\xb3\xc0\xce\xc1\xa4\xba\xb8\xc1\xa6\xb0\xc5)");
		}
		m_bHideGUI = false;
		m_bHidePI = false;
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 505, 0, 0, 0, 0, 0));
		const char* format = COption::Instance()->dGetCaptureFormat();
		mkdir("capture");
		char filename[64];
		do
		{
			sprintf(filename, "capture/pangya%s_%03d.%s", suffix,
				m_captureNum++, format);
		} while (_access(filename, 0) != -1);
		if (!stricmp(format, "avi"))
		{
			Doc()->SetReplayState(7);
			if (m_avi.IsOpened())
			{
				m_avi.Close();
				m_mainFlags.Disable(0x22);
				m_flag.Disable(2);
				SetFPS(m_refreshRate);
				if (IsLocalContent(S4_REPLAY_SYSTEM))
					Doc()->SetReplayState(0);
				SetGameSpeed(1.0f);
				COption::Instance()->dSetCaptureFormat(0);
			}
			else
			{
				int width = COption::Instance()->dGetAviWidth();
				int height = COption::Instance()->dGetAviHeight();
				int fps = COption::Instance()->dGetAviFps();
				if (m_avi.Open(filename, width, height, fps, g_hwnd))
				{
					g_input->SetFakeInput(
						"\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", false);
					SetFPS((float)fps);
					m_flag.Enable(2);
					m_mainFlags.Enable(0x22);
				}
			}
		}
		else
		{
			if (!COption::Instance()->gGetCaptureHidePI() &&
				Doc()->m_playingMode != 2)
			{
				FILETIME fileTime;
				SystemTimeToFileTime(&Doc()->GetServerTime(), &fileTime);
				ULARGE_INTEGER ticks;
				ticks.LowPart = fileTime.dwLowDateTime;
				ticks.HighPart = fileTime.dwHighDateTime;
				ULARGE_INTEGER now;
				now.QuadPart = ticks.QuadPart +
					10000LL * (GetTickCount() - Doc()->m_serverTimeTick);
				fileTime.dwLowDateTime = now.LowPart;
				fileTime.dwHighDateTime = now.HighPart;
				SYSTEMTIME time;
				FileTimeToSystemTime(&fileTime, &time);
				char stamp[128];
				sprintf(stamp, "%d-%02d-%02d %02d:%02d:%02d", time.wYear,
					time.wMonth, time.wDay, time.wHour, time.wMinute,
					time.wSecond);
				if (Doc()->m_loginServerUID && Doc()->m_curGameServer.name[0])
				{
					strcat(stamp, " (");
					strcat(stamp, Doc()->m_curGameServer.name);
					strcat(stamp, ")");
				}
				Draw();
				FrGraphicInterface* graphics = g_pFresh->GetManager()->GetGDI();
				if (graphics)
				{
					graphics->SetTextColor(0xffffffff, 0x88888888);
					graphics->SetTextStyle(2);
					graphics->Print(WPoint(5, g_view->GetHeight() - 13), 0,
						stamp);
					graphics->SetTextStyle(0);
				}
				g_view->EndScene();
				g_resrcmng->VideoReference()->EndScene();
			}
			if (SaveScreenShot(filename))
				sprintf(captureMessage,
					"\xc8\xad\xb8\xe9%s\xc0\xcc %s\xbf\xa1 \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					description, filename);
			else
				sprintf(captureMessage,
					"\xbd\xba\xc5\xa9\xb8\xb0\xbc\xa6 \xc0\xfa\xc0\xe5\xc0\xcc \xbd\xc7\xc6\xd0 \xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
			captureMessageTime = timeGetTime();
		}
	}
}

void CheckQuickEscape()
{
}

void CheckDebugKey()
{
}

void CProjectG::OpenDisconDlg(const char* text, bool gameServer)
{
	if (g_pFresh)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		if (form)
		{
			form->SetMessage(text, false);
			if (gameServer == true)
				form->Open((FRESH_PFN_RESULT)&CProjectG::OnGameServerResult,
					515);
			else
				form->Open((FRESH_PFN_RESULT)&CProjectG::OnLoginServerResult,
					515);
		}
	}
	else
	{
		MessageBox(NULL, text, "\xbe\xcb\xb8\xb2", 0);
		if (gameServer == true)
			OnGameServerResult(1, NULL);
		else
			OnLoginServerResult(1, NULL);
	}
}

void CProjectG::FromGolfToLobbySettings()
{
	if (g_input)
	{
		g_input->FreeKey();
		g_input->FreeMouse();
	}
	if (g_audio)
		g_audio->StopAllSound();
	if (CFx::Instance())
		CFx::Instance()->ClearAll(false);
	if (CUserInfo::Instance())
		CUserInfo::Instance()->Close();
	if (CMouseCursor::Instance())
		CMouseCursor::Instance()->SetMode(CMouseCursor::SCREEN_LOBBY);
	g_resrcmng->VideoReference()->Command(W_VDEV_RELEASE_CAPTURERESOURCE, 0, 0);
}

void CProjectG::Process(float delta)
{
	g_input->Update();
	if (!WisEqual(GetGameSpeed(), 1.0f, g_EPSILON))
	{
		if (!IS_KINDOF(CGolfTask, AfxGetTask()))
			SetGameSpeed(1.0f);
	}
	CNetworkMonitor::Instance()->Process(delta);
	g_FpsMonitor.Trace(delta == 0.0f ? -1 : (int)(1.0f / delta));
	g_FpsMonitor.Process(0.0f);
	int available =
		g_resrcmng->VideoReference()->Command(W_VDEV_GETAVAIL_VRAM, 0, 0);
	static int initialVram = -1;
	if (initialVram == -1)
		initialVram = available;
	g_VramMonitor.Trace((initialVram - available) / 20000);
	g_VramMonitor.Process(0.0f);
	delta *= GetGameSpeed();
	if (IsLocalContent(S4_REPLAY_SYSTEM) && bool(m_flag & 2))
	{
		static float captureTime;
		captureTime += delta;
		if (delta > 1.0f / 15.0f)
		{
			delta = 1.0f / 15.0f;
			SetGameSpeed(0.0f);
			captureTime = 0.0f;
			g_bAviCapture = true;
		}
		else if (captureTime > 1.0f / 15.0f)
		{
			delta -= captureTime - 1.0f / 15.0f;
			SetGameSpeed(0.0f);
			captureTime = 0.0f;
			g_bAviCapture = true;
		}
	}
	CTaskManager::Instance()->PreProcess();
	if (Doc()->m_replayState != 7 && g_input->GetDown("MESSENGER", true) &&
		NET()->GetSocket(WNetworkSystem::NET_GAME)->m_bConnected)
		CMessengerInfo::Instance()->Open(true);
	CMouseCursor::Instance()->Process(delta);
	CMessengerInfo::Instance()->Process(delta);
	CTaskManager::Instance()->ProcessTask(delta);
	CUserInfo::Instance()->Process(delta);
	CRankingInfo::Instance()->Process(delta);
	CNoticeBoard::Instance()->Process(delta);
	CWinnerNotice::Instance()->Process(delta);
	CFx::Instance()->Process(delta);
	CBrowser::Instance()->Process(delta);
	COneLineBoard::Instance()->Process(delta);
	lua_system::lua_script_manger::GetInstance()->Update(delta);
	g_audio->Process(delta, g_view);
	if (g_miles)
		g_miles->Process();
	COption::Instance()->CheckPendingChanges();
	CheckScreenShot();
	if (m_pDxDiagInfo && !m_pDxDiagInfo->IsGathered())
		m_pDxDiagInfo->GatherInfo();
	if (g_bQuit)
		Quit();
	++processCount;
	if (g_bOldFocus != g_bFocus)
	{
		g_bOldFocus = g_bFocus;
		if (!COption::Instance()->GetOption()->vWindowMode)
			g_mouse->InitDevice(g_hwnd, g_bFocus);
	}
	if (!g_bFocus)
	{
		if (NET()->GetSocket(WNetworkSystem::NET_GAME)->GetRcvCnt() < 5000)
			Sleep(50);
	}
	else if (CBrowser::Instance()->IsOpen())
		Sleep(10);
}

void CProjectG::ReportError(int code, const char* format, ...)
{
	char text[256] = "";
	va_list args;
	va_start(args, format);
	if (_vsnprintf(text, sizeof(text) - 1, format, args) != -1)
	{
		char report[512] = "";
		const char* nick = MyNick();
		_snprintf(report, sizeof(report) - 1, "%d %s %s %s %s", code,
			strlen(nick) ? nick : "unknown", PY_PUBLIC_VERSION, "645.00", text);
		CPangFBI::Instance()->ProcessReport("app_log", report);
	}
	va_end(args);
}

void CProjectG::GameGuardLog(const char* format, ...)
{
	char text[256];
	va_list args;
	va_start(args, format);
	if (_vsnprintf(text, sizeof(text) - 1, format, args) == -1)
		text[sizeof(text) - 1] = 0;
	va_end(args);
	SYSTEMTIME time;
	GetLocalTime(&time);
	FILE* file = fopen("GameGuard/GameGuard.err", "a+");
	if (file)
	{
		fprintf(file, "%d-%02d-%02d %02d:%02d:%02d.%03d %s\n", time.wYear,
			time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
			time.wMilliseconds, text);
		fclose(file);
	}
}

bool CProjectG::HidePrivacy()
{
	return m_bHidePrivacy;
}

bool CProjectG::HideGUI()
{
	return m_bHideGUI;
}

bool CProjectG::HidePI()
{
	return m_bHidePI;
}

static bool CheckFileMD5(const char* filename, const unsigned char* expected)
{
	cFile* file = g_resrcmng->GetCFile(filename, 0xffff);
	if (!file)
		return false;
	unsigned int size = file->Length();
	unsigned char* data = new unsigned char[size];
	file->Read(data, size);
	CloseCFile(file);
	MD5Context context;
	unsigned char digest[16];
	MD5Init(&context);
	MD5Update(&context, data, size);
	MD5Final(digest, &context);
	delete[] data;
	for (int i = 0; i < 16; ++i)
		if (expected[i] != digest[i])
			return false;
	return true;
}

bool CProjectG::CheckUIHack()
{
	UIFileChecksum* check = uiChecks;
	for (unsigned int i = 0; i < sizeof(uiChecks) / sizeof(uiChecks[0]);
		++i, ++check)
		if (!CheckFileMD5(check->filename, check->digest))
			return false;
	return true;
}

void CProjectG::ResetCaptuerNum()
{
	m_captureNum = 0;
}

void CProjectG::Draw()
{
	if (CBrowser::Instance()->IsOpen())
		return;
	WMatrix camera = g_camera;
	unsigned char flash = 0;
	if (CFx::Instance())
		CFx::Instance()->GetOutput(camera.pivot, flash);
	g_view->SetCamera(camera);
	g_view->BeginScene();
	g_view->Clear(0, 2);
	g_view->SetPrevCamera(m_prevCamera);
	m_prevCamera = g_view->GetCamera();
	CTaskManager::Instance()->DisplayTask();
	CMouseCursor::Instance()->Display();
	CNoticeBoard::Instance()->Display();
	CWinnerNotice::Instance()->Display();
	if (!CSceneManager::Instance())
		CFx::Instance()->Display();
	CBrowser::Instance()->Display();
	if (IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->m_playingMode == 1)
	{
		if (IS_EXACTKINDOF(CGolfTask, AfxGetTask()) &&
			(Doc()->m_replayPlayType == 1 || Doc()->m_replayPlayType == 2) &&
			Doc()->m_roomInfo.realGameType != 14)
		{
			FrGraphicInterface* graphics = g_pFresh->GetManager()->GetGDI();
			graphics->SetTextStyle(2);
			graphics->SetTextColor(0xffff0000, 0xffffffff);
			if (Doc()->m_replayPlayType == 1)
				graphics->Print(WPoint(300, g_view->GetHeight() - 40), 0,
					"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc\xb0\xa1 \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.(\xbc\xa6)");
			else if (Doc()->m_replayPlayType == 2)
				graphics->Print(WPoint(300, g_view->GetHeight() - 40), 0,
					"\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc\xb0\xa1 \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
			graphics->SetTextStyle(0);
		}
	}
	if (timeGetTime() - captureMessageTime < 1000 && CChatMsg::Instance())
	{
		WFont* font = CChatMsg::Instance()->GetMaskedFont();
		font->Print(g_view,
			g_view->GetWidth() * 0.5f -
				font->GetTextWidth(g_view, captureMessage) * 0.5f,
			g_view->GetHeight() - 25.0f, captureMessage, 0, 0xffff0000, NULL);
		CChatMsg::Instance()->GetMaskedFont()->Flush(g_view);
	}
	if (flash)
	{
		WRect rect(0, 0, g_view->GetWidth(), g_view->GetHeight());
		WOverlay::DrawBox(g_view, rect, 0, (flash << 24) | 0xffffff, 0.001f);
	}
	++drawCount;
}

void CProjectG::Paint()
{
	if (!CBrowser::Instance()->IsOpen())
	{
		g_view->EndScene();
		g_view->Render();
		g_last_screen_width = (int)g_view->GetWidth();
		g_last_screen_height = (int)g_view->GetHeight();
		WVideoDev* video = g_view->GetVideoDevice();
		if (video)
		{
			int state = video->GetDeviceState();
			if (state == 1 && g_bFocus)
			{
				g_bFocus = false;
				m_bLostFocus = true;
			}
			else if (m_bLostFocus && state == 0 && !g_bFocus)
			{
				g_bFocus = true;
				m_bLostFocus = false;
			}
		}
		if (bool(m_flag & 2) && g_bAviCapture)
		{
			static Bitmap frame(g_last_screen_width, g_last_screen_height, 24);
			if (g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURE_SCREEN, 0,
					(int)&frame) == 1)
			{
				m_avi.Write(&frame);
				SetGameSpeed(1.0f);
				g_bAviCapture = false;
			}
		}
	}
}

long __stdcall CProjectG::WinProc(HWND hwnd, unsigned int message,
	unsigned int wParam, long lParam)
{
	switch (message)
	{
	case 44468:
		if (m_crc.IsWorking())
			m_crc.Resume();
		break;
	case 44467:
		if (m_crc.IsWorking())
			m_crc.Pause();
		break;
	case 44466:
		if (!wParam)
		{
			std::string filename = (const char*)lParam;
			MessageBox(NULL,
				MakeStr(
					"%s\xc6\xc4\xc0\xcf\xbf\xa1 \xbc\xd5\xbb\xf3\xc0\xcc \xb9\xdf\xb0\xdf\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc7\xc7\xe0\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
					filename.c_str()),
				"\xc6\xce\xbe\xdf\xbf\xa1\xb7\xaf", MB_ICONEXCLAMATION);
			PostQuitMessage(0);
		}
		break;
	case WM_USER + 10:
	{
		if (!NET())
			return 0;
		WClientSocket* socket = NET()->GetSocket(WNetworkSystem::NET_GAME);
		if (!socket)
			return 0;
		int reason;
		if (!WSAGETSELECTERROR(lParam))
		{
			switch (WSAGETSELECTEVENT(lParam))
			{
			case FD_READ:
				if (socket->TryRead())
					goto DefaultProcessing;
				socket->Close();
				reason = 1;
				break;
			case FD_WRITE:
				socket->Flush();
				goto DefaultProcessing;
			case FD_CLOSE:
				reason = 2;
				break;
			default:
				goto DefaultProcessing;
			}
		}
		else
			reason = 3;
		if (g_iRecvDisconCode)
			g_iRecvDisconCode = 0;
		else if (AfxGetTask()->GetActor("GolfRule"))
		{
			ReportError(4, "%d_%d_%d_%d_%d_%d(%d_%d)", reason,
				WSAGETSELECTERROR(lParam), g_iSendFailType, WSAGetLastError(),
				g_iSendPacketType, g_iRecvPacketType,
				Doc()->m_roomInfo.roomGuid, MyGuid(false));
		}
		else
			ReportError(4, "%d_%d_%d_%d_%d_%d", reason,
				WSAGETSELECTERROR(lParam), g_iSendFailType, WSAGetLastError(),
				g_iSendPacketType, g_iRecvPacketType);
		if (IsLocalContent((localContentType_t)90))
		{
			sGameServerInfo selected;
			memset(&selected, 0, sizeof(selected));
			bool found = false;
			int users = 2000;
			for (std::list<sGameServerInfo>::iterator it =
					 Doc()->m_gameServerList.begin();
				it != Doc()->m_gameServerList.end(); ++it)
			{
				sGameServerInfo server = *it;
				if (Doc()->m_curGameServer.guid != server.guid)
				{
					users = server.curUser;
					selected = server;
					found = true;
				}
				else if (found != true)
				{
					continue;
				}

				std::list<unsigned long>& visited = Doc()->m_guildMemberList;
				for (std::list<unsigned long>::iterator visit = visited.begin();
					visit != visited.end(); ++visit)
				{
					if (*visit == server.guid)
					{
						memset(&selected, 0, sizeof(selected));
						found = false;
						goto NextServer;
					}
				}
				if (server.curUser < users &&
					Doc()->m_curGameServer.guid != server.guid)
				{
					users = server.curUser;
					selected = server;
				}
NextServer:;
			}
			if (!found)
			{
				socket->Close();
				Doc()->m_guildMemberList.clear();
				SetMoveLoginServer(MyId(), Doc()->m_password.c_str());
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"LOGIN", 0, 0, 0);
				break;
			}
			Doc()->m_gameServerAddr = selected.addr;
			Doc()->m_gameServerPort = selected.port;
			Doc()->m_gameServerUID = selected.guid;
			if (NET()->IsConnected(WNetworkSystem::NET_GAME))
				NET()->ForceShutDown(WNetworkSystem::NET_GAME);
			NET()->SendMessage(WNetworkSystem::NET_GAME,
				NetworkUnit::eEVENT_NEXT);
			Doc()->m_curGameServer = selected;
			Doc()->SetDirectMoveRoomInfo(Doc()->m_curChannel.Uid, 0xffff,
				0xffffffff);
			Doc()->m_guildMemberList.push_back(selected.guid);
			return 1;
		}
		else
		{
			OpenDisconDlg(
				"\xb0\xd4\xc0\xd3 \xbc\xad\xb9\xf6\xbf\xcd \xbf\xac\xb0\xe1\xc0\xcc \xb2\xf7\xbe\xee\xc1\xb3\xbd\xc0\xb4\xcf\xb4\xd9.",
				true);
			return 1;
		}
	}
	case WM_USER + 12:
		if (WSAGETSELECTERROR(lParam))
		{
			if (WSAGETSELECTEVENT(lParam) == FD_CONNECT)
			{
				if (WSAGETSELECTERROR(lParam) != WSAEWOULDBLOCK)
					NET()->SendMessage(WNetworkSystem::NET_MSN,
						NetworkUnit::eEVENT_FAIL);
				return 0;
			}
			CMessengerInfo::Instance()->Close(true);
			NET()->ForceShutDown(WNetworkSystem::NET_MSN);
			return 0;
		}
		else
		{
			if (WSAGETSELECTEVENT(lParam) == FD_CONNECT)
			{
				NET()->GetSocket(WNetworkSystem::NET_MSN)->m_bConnected = true;
				NET()->SendMessage(WNetworkSystem::NET_MSN,
					NetworkUnit::eEVENT_NEXT);
			}
			else if (WSAGETSELECTEVENT(lParam) == FD_READ)
			{
				if (NET()->GetSocket(WNetworkSystem::NET_MSN) &&
					!NET()->GetSocket(WNetworkSystem::NET_MSN)->TryRead())
				{
					CMessengerInfo::Instance()->Close(true);
					NET()->ForceShutDown(WNetworkSystem::NET_MSN);
				}
			}
			else if (WSAGETSELECTEVENT(lParam) == FD_WRITE)
			{
				WClientSocket* socket =
					NET()->GetSocket(WNetworkSystem::NET_MSN);
				if (socket)
					socket->Flush();
			}
			else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
			{
				CMessengerInfo::Instance()->Close(true);
				NET()->ForceShutDown(WNetworkSystem::NET_MSN);
			}
		}
		return 0;
	case WM_USER + 13:
		if (WSAGETSELECTERROR(lParam))
		{
			NET()->GetSocket(WNetworkSystem::NET_LOGIN)->m_bConnected = false;
			OpenDisconDlg(
				"\xb7\xce\xb1\xd7\xc0\xce \xbc\xad\xb9\xf6\xbf\xcd \xbf\xac\xb0\xe1\xc0\xcc \xb2\xf7\xbe\xee\xc1\xb3\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			return 0;
		}
		if (WSAGETSELECTEVENT(lParam) == FD_READ)
		{
			if (NET()->GetSocket(WNetworkSystem::NET_LOGIN) &&
				!NET()->GetSocket(WNetworkSystem::NET_LOGIN)->TryRead())
				OpenDisconDlg(
					"\xb7\xce\xb1\xd7\xc0\xce \xbc\xad\xb9\xf6\xbf\xcd \xbf\xac\xb0\xe1\xc0\xcc \xb2\xf7\xbe\xee\xc1\xb3\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_WRITE)
		{
			WClientSocket* socket = NET()->GetSocket(WNetworkSystem::NET_LOGIN);
			if (socket)
				socket->Flush();
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
			OpenDisconDlg(
				"\xb7\xce\xb1\xd7\xc0\xce \xbc\xad\xb9\xf6\xbf\xcd \xbf\xac\xb0\xe1\xc0\xcc \xb2\xf7\xbe\xee\xc1\xb3\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
		return 0;
	case WM_USER + 14:
	{
		if (WSAGETSELECTERROR(lParam))
		{
			NET()->GetSocket(WNetworkSystem::NET_RANK)->m_bConnected = false;
			CMouseCursor::Instance()->SetActive(false);
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
				false);
			CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"ROOMLIST",
				0, 0, 0);
			CTaskManager::Instance()->PostMsg(NULL, "Lobby", 26, 0, 0, 0, 0);
			return 0;
		}
		if (WSAGETSELECTEVENT(lParam) == FD_READ)
		{
			if (NET()->GetSocket(WNetworkSystem::NET_RANK) &&
				!NET()->GetSocket(WNetworkSystem::NET_RANK)->TryRead())
			{
				CMouseCursor::Instance()->SetActive(false);
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"ROOMLIST", 0, 0, 0);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 26, 0, 0, 0,
					0);
			}
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
		{
			CMouseCursor::Instance()->SetActive(false);
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
				false);
			CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"ROOMLIST",
				0, 0, 0);
			CTaskManager::Instance()->PostMsg(NULL, "Lobby", 26, 0, 0, 0, 0);
		}
		return 0;
	}

	case WM_SETFOCUS:
		if (g_hwnd == GetForegroundWindow())
			g_bFocus = true;
		break;
	case WM_KILLFOCUS:
		if (!CBrowser::Instance() || !CBrowser::Instance()->IsOpen())
			g_bFocus = false;
		break;
	case WM_CLOSE:
		Quit();
		return 0;
	case WM_NCHITTEST:
		if (!bool(m_flag & 4))
			return HTCLIENT;
		break;
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_SIZE:
		case SC_MOVE:
		case SC_MAXIMIZE:
		case SC_KEYMENU:
		case SC_MONITORPOWER:
			if (!bool(m_flag & 4))
				return 1;
		}
		break;
	case WM_ACTIVATEAPP:
		if (!bool(m_flag & 4))
		{
			if (wParam)
			{
				SetWindowLong(g_hwnd, GWL_STYLE,
					GetWindowLong(g_hwnd, GWL_STYLE) & ~WS_SYSMENU);
				ShowWindow(g_hwnd, SW_RESTORE);
			}
			else
			{
				SetWindowLong(g_hwnd, GWL_STYLE,
					GetWindowLong(g_hwnd, GWL_STYLE) & ~WS_SYSMENU);
				ShowWindow(g_hwnd, SW_MINIMIZE);
			}
			return 1;
		}
		if (g_miles)
			g_miles->Activate(wParam != 0);
		break;
	case WM_PAINT:
		if (CBrowser::Instance() && CBrowser::Instance()->IsOpen())
			return DefWindowProc(g_hwnd, message, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_F4)
		{
			if (IS_KINDOF(CGolfTask, AfxGetTask()))
			{
				if (IsLocalContent(S4_REPLAY_SYSTEM))
				{
					if (Doc()->m_playingMode == 1)
						return 1;
				}
				else
					return 1;
			}
		}
		break;
	}
DefaultProcessing:
	if (m_pProcManager)
		return m_pProcManager->WinProc(g_hwnd, message, wParam, lParam);
	return 0;
}

bool CProjectG::Init()
{
	m_pDeviceManager->SetHWND(g_hwnd);
	if (!SetVideoDevice())
		return false;
	if (!SetInputDevice())
		return false;
	SetAudioDevice();
	UpdateWindow(g_hwnd);
	ShowWindow(g_hwnd, SW_SHOW);
	InvalidateRect(g_hwnd, NULL, FALSE);
	UpdateWindow(g_hwnd);
	m_captureNum = 0;
	return true;
}

bool CProjectG::SetVideoDevice()
{
	char caption[64] = "";
	int tnl = COption::Instance()->vGetTnLMode();
	RECT rect = { 0, 0, 800, 600 };
	AdjustWindowRectEx(&rect, WS_CAPTION | WS_THICKFRAME, FALSE, 0);
	int x = (rect.left - rect.right + GetSystemMetrics(SM_CXSCREEN)) / 2;
	int y = (rect.top - rect.bottom + GetSystemMetrics(SM_CYSCREEN)) / 2;
	OffsetRect(&rect, x, y);
	MoveWindow(g_hwnd, x, y, rect.right - rect.left, rect.bottom - rect.top,
		FALSE);
	m_flag.Enable(4);
	WVideoDev* video = m_pDeviceManager->CreateVideoDevice(NULL, "Window", tnl);
	if (!video)
	{
		MessageBox(g_hwnd,
			"\xb4\xd9\xc0\xbd\xb0\xfa \xb0\xb0\xc0\xba \xb9\xae\xc1\xa6\xb7\xce \xbd\xc7\xc7\xe0\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.\012\012* DirectX 9.0c \xc0\xcc\xbb\xf3\xc0\xcc \xbc\xb3\xc4\xa1 \xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xb0\xc5\xb3\xaa\012* \xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc \xbe\xee\xb4\xf0\xc5\xcd(\xba\xf1\xb5\xf0\xbf\xc0\xc4\xab\xb5\xe5)\xc0\xc7 \xb5\xe5\xb6\xf3\xc0\xcc\xb9\xf6\xb0\xa1 \xc3\xd6\xbd\xc5 \xb9\xf6\xc0\xfc\xc0\xcc \xbe\xc6\xb4\xcf\xb0\xc5\xb3\xaa\012* \xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc \xbe\xee\xb4\xf0\xc5\xcd(\xba\xf1\xb5\xf0\xbf\xc0\xc4\xab\xb5\xe5)\xc0\xc7 \xbc\xba\xb4\xc9\xbf\xa1 \xb9\xae\xc1\xa6\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\012\012\xc6\xce\xbe\xdf \xbd\xc7\xc7\xe0\xbd\xc3 \xb3\xaa\xbf\xc0\xb4\xc2 \xc3\xb9 \xc8\xad\xb8\xe9\xbf\xa1\xbc\xad [\xbd\xc7\xc7\xe0\xb9\xae\xc1\xa6\xba\xb8\xb0\xed]\xb8\xa6 \xb4\xad\xb7\xaf \xc4\xc4\xc7\xbb\xc5\xcd \xc1\xa4\xba\xb8\xb8\xa6 \xb5\xee\xb7\xcf\xc7\xcf\xbd\xc5 \xc8\xc4,  \012\xc6\xce\xbe\xdf \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\xc0\xc7 [\xb0\xed\xb0\xb4\xc1\xf6\xbf\xf8->FAQ]\xb8\xa6 \xc0\xd0\xbe\xee\xba\xb8\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
			caption, MB_ICONERROR);
		return false;
	}
	video->SetMainThreadId(m_threadId);
	SetProc(video);
	g_resrcmng->SetVideoReference(video);
	m_refreshRate = (float)floor(video->GetMonitorSupportFPS());
	SetFPS(m_refreshRate);
	return true;
}

bool CProjectG::SetAudioDevice()
{
	bool enabled = COption::Instance()->aIsMssEnabled();
	int frequency = COption::Instance()->aGetMssFrequency();
	int bits = COption::Instance()->aGetMssBits();
	int channels = COption::Instance()->aGetMssChannels();
	bool hardware = COption::Instance()->aIsMssHwSoundEnabled();
	float sfx = COption::Instance()->aGetSfxVolume();
	float bgm = COption::Instance()->aGetBgmVolume();
	g_miles = new WMilesSoundSystem(g_hwnd, "mss", enabled, frequency, bits,
		channels);
	g_audio = new CSoundManager;
	if (!g_miles || !g_miles->Init(g_hwnd, frequency, bits))
	{
		g_audio->SetMSS(NULL);
		return false;
	}
	WMilesSoundSystem::w_speaker_type speaker =
		COption::Instance()->aGetMssSpeaker();
	int balance = COption::Instance()->aGetMssBalance();
	g_miles->SpeakerType(speaker);
	g_miles->SetBalance(balance);
	g_miles->SetDistance(400.0f * g_CM_TO_WU, 3000.0f * g_CM_TO_WU);
	g_audio->SetMSS(g_miles);
	if (enabled)
	{
		g_audio->SetVolume(Between(0.0f, sfx, 1.0f));
		g_audio->SetBGMVolume(Between(0.0f, bgm, 1.0f));
	}
	else
	{
		g_audio->SetVolume(0.0f);
		g_audio->SetBGMVolume(0.0f);
	}
	return true;
}

void CProjectG::UploadShaderSource()
{
	WVideoDev* video = g_resrcmng->VideoReference();
	if (video)
	{
		cFile* file = g_resrcmng->GetCFile("pangya.fx", -1);
		if (file)
		{
			char* source = new char[file->Length() + 1];
			file->Read(source, file->Length());
			source[file->Length()] = 0;
			video->SetShaderSource(source);
			delete[] source;
			CloseCFile(file);
		}
		else
			video->SetShaderSource(NULL);
	}
}

bool CProjectG::SetInputDevice()
{
	g_keyboard = m_pDeviceManager->CreateInputDevice("DirectInput", "keyboard");
	SetProc(g_keyboard);
	AddInputDev(g_keyboard);
	g_mouse = m_pDeviceManager->CreateInputDevice("DirectInput", "mouse");
	SetProc(g_mouse);
	AddInputDev(g_mouse);
	g_ime = m_pDeviceManager->CreateInputDevice("Ime", "");
	SetProc(g_ime);
	AddInputDev(g_ime);
	return true;
}

void CProjectG::SetProc(WDevice* device)
{
	WProc* proc = device->ExternProc();
	if (proc)
		proc->SetProc(m_pProcManager, g_hwnd);
}

void CProjectG::Update(int time)
{
	for (WInputDev* input = m_inputList.Start(); input;
		input = m_inputList.Next())
		input->Update(g_CurrentTime);
}

void CProjectG::SetWindowed(bool windowed)
{
	if (windowed)
		m_flag.Enable(4);
	else
		m_flag.Disable(4);
}

void CProjectG::OnMsnPacket(WReceivedPacket& packet)
{
}

void CProjectG::ProcessRecvPacketQueue()
{
	if (CTaskManager::Instance())
	{
		CTask* task = CTaskManager::Instance()->GetCurrentTask();
		if (task && task->IsInited() &&
			!CTaskManager::Instance()->IsTaskChanging())
		{
			while (!m_recvPacketQueue.empty())
			{
				WReceivedPacket* packet = m_recvPacketQueue.front();
				if (!task->OnPacket(*packet))
				{
					packet->Reset();
					ProcessPacket(*packet);
				}
				delete packet;
				m_recvPacketQueue.pop();
			}
		}
	}
}

void CProjectG::OnPacket(WReceivedPacket& packet)
{
	if (CTaskManager::Instance())
	{
		CTask* preserved = CTaskManager::Instance()->GetPreservedTask();
		CTask* task = CTaskManager::Instance()->GetCurrentTask();
		if (task)
		{
			if (task->IsInited() && !CTaskManager::Instance()->IsTaskChanging())
			{
				while (!m_recvPacketQueue.empty())
				{
					WReceivedPacket* pending = m_recvPacketQueue.front();
					if (!task->OnPacket(*pending))
					{
						if (preserved)
						{
							pending->Reset();
							if (!preserved->OnPacket(*pending))
							{
								pending->Reset();
								ProcessPacket(*pending);
							}
						}
						else
						{
							pending->Reset();
							ProcessPacket(*pending);
						}
					}
					delete pending;
					m_recvPacketQueue.pop();
				}
				if (!task->OnPacket(packet))
				{
					if (preserved)
					{
						packet.Reset();
						if (!preserved->OnPacket(packet))
						{
							packet.Reset();
							ProcessPacket(packet);
						}
					}
					else
					{
						packet.Reset();
						ProcessPacket(packet);
					}
				}
			}
			else if (packet.Decode2() == 96)
			{
				packet.Reset();
				ProcessPacket(packet);
			}
			else
			{
				WReceivedPacket* pending = new WReceivedPacket;
				packet.Copy(pending);
				m_recvPacketQueue.push(pending);
			}
			return;
		}
	}
	ProcessPacket(packet);
}

void CProjectG::ProcessPacket(WReceivedPacket& packet)
{
	packet.Decode2();
}

void CProjectG::OnUDPPacket(WReceivedPacket& packet)
{
}

bool CProjectG::OnGameServerResult(int result, FrForm* form)
{
	if (CLoginInfo::Instance()->IsWebLogin())
	{
		g_bQuit = true;
		return true;
	}
	if (result)
	{
		FromGolfToLobbySettings();
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"SERVERLIST",
			0, 0, 0);
		CMouseCursor::Instance()->SetActive(true);
		CSharedDoc* doc = Doc();
		if (doc)
		{
			std::list<sGameServerInfo>::iterator it = std::find_if(
				doc->m_gameServerList.begin(), doc->m_gameServerList.end(),
				_FindByGuid(doc->m_curGameServer.guid));
			if (it != doc->m_gameServerList.end())
				doc->m_gameServerList.erase(it);
			Doc()->m_channelList.clear();
		}
	}
	return true;
}

bool CProjectG::OnLoginServerResult(int result, FrForm* form)
{
	g_bQuit = true;
	return true;
}

__int64 CProjectG::CalcHddFreeSpace()
{
	__int64 freeSpace = 0;
	char* drive = GetCurrentHdd();
	typedef BOOL(WINAPI * DiskSpaceProc)(LPCSTR, PULARGE_INTEGER,
		PULARGE_INTEGER, PULARGE_INTEGER);
	DiskSpaceProc getSpace = (DiskSpaceProc)GetProcAddress(
		GetModuleHandle("kernel32.dll"), "GetDiskFreeSpaceExA");
	if (getSpace)
	{
		ULARGE_INTEGER available, total, free;
		if (getSpace(drive, &available, &total, &free))
			freeSpace = free.QuadPart;
		else
			return 0;
	}
	return freeSpace;
}

char* CProjectG::GetCurrentHdd()
{
	CRegistry registry;
	registry.SetRootKey(HKEY_LOCAL_MACHINE);
	registry.SetKey("Software\\Ntreev\\Pangya_QA", true);
	WTL::CString directory = "";
	directory = registry.ReadString("Install_Dir", "");
	static char drive[255];
	sprintf(drive, "%s", (const char*)directory);
	drive[2] = 0;
	return drive;
}
