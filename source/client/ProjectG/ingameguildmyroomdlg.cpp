#include "ingameguildmyroomdlg.h"

extern Fresh* g_pFresh;

#include "actor.h"
#include "guildactor.h"
#include "guilddefine.h"
#include "netresourcemanager.h"
#include "ucclibrary.h"
#include "uwin.h"
#include "wresrcmng.h"
#include "clientsetting.h"
#include <commdlg.h>

extern WResourceManager* g_resrcmng;
extern WInputDev* g_mouse;
extern HWND g_hwnd;

namespace _guild
{

	IMPLEMENT_OBJECT(FrCreateGuildKit, FrForm)

	BEGIN_FRESH_MSGMAP(FrCreateGuildKit, FrForm)

	ON_FRESH_VI("exist", FRCMD_INIT,
		FrCreateGuildKit::OnInitExistGuildNameButton)
	ON_FRESH_VV("exist", FRCMD_LBUTTONUP,
		FrCreateGuildKit::OnLButtonUpExistGuildNameButton)
	ON_FRESH_VI("guildname", FRCMD_ENTERKEY,
		FrCreateGuildKit::OnExistGuildName_EnterKey)
	ON_FRESH_VI("confirm", FRCMD_INIT,
		FrCreateGuildKit::OnGuildRegister_ButtonInit)
	ON_FRESH_VV("confirm", FRCMD_LBUTTONUP,
		FrCreateGuildKit::OnGuildRegister_LButtonUp)
	ON_FRESH_VI("cancel", FRCMD_INIT, FrCreateGuildKit::OnCancel_ButtonInit)
	ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrCreateGuildKit::OnCancel_LButtonUp)
	ON_FRESH_VI("guildname", FRCMD_INIT,
		FrCreateGuildKit::OnInitInputGuildNameEdit)
	ON_FRESH_VI("introduce", FRCMD_INIT,
		FrCreateGuildKit::OnInitInputGuildIntroduceEdit)

	END_FRESH_MSGMAP()

	FrCreateGuildKit::FrCreateGuildKit()
	{
		InitControl();
	}

	FrCreateGuildKit::~FrCreateGuildKit()
	{
	}

	void FrCreateGuildKit::SetConfirmGuildName(const char* name)
	{
		if (!m_confirmedName.empty())
			m_confirmedName.clear();
		m_confirmedName = name;
		if (m_pEdit[0])
		{
			m_pEdit[0]->ClearLine();
			m_pEdit[0]->AddLine(name, false, false);
			m_pEdit[1]->SetKeyFocus(true);
		}
	}

	void FrCreateGuildKit::InitControl()
	{
		memset(m_pButton, 0, sizeof(m_pButton));
		memset(m_pEdit, 0, sizeof(m_pEdit));
	}

	void FrCreateGuildKit::OnInitExistGuildNameButton(int param)
	{
		m_pButton[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrCreateGuildKit::OnLButtonUpExistGuildNameButton()
	{
		std::string name(m_pEdit[0]->GetLine(1, false));
		if (name.empty())
		{
			AfxGetTask()->GetActor(s_guildActor[1])
				<< MsgObject(NULL, 611, 26, 1000, 0, 0, 0);
			return;
		}
		int result = Doc()->m_chatManager.FilteringGuildString(name.c_str(), 4,
			20, true);
		if (result != 1)
		{
			IActor* actor = AfxGetTask()->GetActor(s_guildActor[1]);
			if (actor && result > 0)
				actor << MsgObject(NULL, 611, 26, result, 0, 0, 0);
			return;
		}
		WSendPacket packet((enumClientPacket)255);
		packet.EncodeStr(name);
		packet.Send(TO_GAME);
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}

	void FrCreateGuildKit::OnExistGuildName_EnterKey(int)
	{
		OnLButtonUpExistGuildNameButton();
	}

	void FrCreateGuildKit::OnGuildRegister_ButtonInit(int param)
	{
		m_pButton[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrCreateGuildKit::OnGuildRegister_LButtonUp()
	{
		if (m_confirmedName.empty())
		{
			std::string name(m_pEdit[0]->GetLine(1, false));
			if (name.empty())
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 26, 1000, 0, 0, 0);
			else
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 26, 1028, 0, 0, 0);
			return;
		}
		const char* line = m_pEdit[0]->GetLine(1, false);
		if (m_confirmedName != line)
		{
			AfxGetTask()->GetActor(s_guildActor[1])
				<< MsgObject(NULL, 611, 26, 1028, 0, 0, 0);
			return;
		}
		std::string introduce;
		int lines = m_pEdit[1]->GetLineNum();
		for (int i = 1; i < lines + 1; ++i)
		{
			introduce += m_pEdit[1]->GetLine(i, false);
			if (i != lines)
				introduce += s_guildLineFeed;
		}
		if (introduce.empty())
		{
			AfxGetTask()->GetActor(s_guildActor[1])
				<< MsgObject(NULL, 611, 26, 1001, 0, 0, 0);
			return;
		}
		WSendPacket packet((enumClientPacket)254);
		packet.EncodeStr(m_confirmedName);
		packet.EncodeStr(introduce);
		packet.Send(TO_GAME);
		Close(true);
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}

	void FrCreateGuildKit::OnCancel_ButtonInit(int param)
	{
		m_pButton[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrCreateGuildKit::OnCancel_LButtonUp()
	{
		Close(true);
	}

	void FrCreateGuildKit::OnInitInputGuildNameEdit(int param)
	{
		m_pEdit[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	}

	void FrCreateGuildKit::OnInitInputGuildIntroduceEdit(int param)
	{
		m_pEdit[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	}

	IMPLEMENT_OBJECT(FrRegisterGuildMark, FrForm)

	BEGIN_FRESH_MSGMAP(FrRegisterGuildMark, FrForm)

	ON_FRESH_VI("search", FRCMD_INIT,
		FrRegisterGuildMark::OnMarkSearch_ButtonInit)
	ON_FRESH_VV("search", FRCMD_LBUTTONUP,
		FrRegisterGuildMark::OnMarkSearch_LButtonUp)
	ON_FRESH_VI("confirm", FRCMD_INIT,
		FrRegisterGuildMark::OnConfirm_ButtonInit)
	ON_FRESH_VV("confirm", FRCMD_LBUTTONUP,
		FrRegisterGuildMark::OnConfirm_LButtonUp)
	ON_FRESH_VI("cancel", FRCMD_INIT, FrRegisterGuildMark::OnCancel_ButtonInit)
	ON_FRESH_VV("cancel", FRCMD_LBUTTONUP,
		FrRegisterGuildMark::OnCancel_LButtonUp)
	ON_FRESH_VI("mark_viewer", FRCMD_INIT,
		FrRegisterGuildMark::OnMarkViewer_AreaInit)
	ON_FRESH_VI("mark_viewer", FRCMD_OWNERDRAW,
		FrRegisterGuildMark::OnMarkViewer_OwnerDraw)
	ON_FRESH_VI("mark_path", FRCMD_INIT,
		FrRegisterGuildMark::OnMarkPath_EditInit)

	END_FRESH_MSGMAP()

	FrRegisterGuildMark::FrRegisterGuildMark()
	{
		memset(m_pButton, 0, sizeof(m_pButton));
		m_pMarkPath = NULL;
		m_pMarkViewer = NULL;
		m_pMarkBitmap = NULL;
		m_windowMode = 0;
		m_bMarkSelected = false;
	}

	FrRegisterGuildMark::~FrRegisterGuildMark()
	{
		SetCurrentDirectory(g_executeDirectory);
		if (!m_windowMode && COption::Instance())
		{
			sOption option = *COption::Instance()->GetOption();
			option.vWindowMode = 0;
			COption::Instance()->ApplyChange(option, false);
		}
		ClearTexCache(m_pMarkBitmap);
		delete m_pMarkBitmap;
		m_pMarkBitmap = NULL;
	}

	bool FrRegisterGuildMark::OnInit()
	{
		COption::Instance()->vApplyLobbyScreenSize();
		sOption option = *COption::Instance()->GetOption();
		m_windowMode = option.vWindowMode;
		if (!option.vWindowMode)
		{
			option.vWindowMode = 1;
			COption::Instance()->ApplyChange(option, false);
		}
		return FrForm::OnInit();
	}

	void FrRegisterGuildMark::RequestUpload(int guildIdx,
		const std::string& path)
	{
		sResource resource;
		resource.type = RESOURCE_GUILD_EMBLEM;
		resource.filename = m_markPath;
		resource.id = guildIdx;
		sprintf(resource.arg, "%s", path.c_str());
		NetResManager()->Upload(resource,
			"http://qa.contents.pangya.gametree.co.kr:50006/Guild/upload.asp");
		Close(FrOK, true);
	}

	void FrRegisterGuildMark::OnMarkSearch_ButtonInit(int param)
	{
		m_pButton[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrRegisterGuildMark::OnMarkSearch_LButtonUp()
	{
		static const char* filter = "png Files\0*.png\0";
		char path[MAX_PATH] = { 0 };
		OPENFILENAME ofn;
		memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = g_hwnd;
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = path;
		ofn.nMaxFile = MAX_PATH;
		ClearTexCache(m_pMarkBitmap);
		delete m_pMarkBitmap;
		m_pMarkBitmap = NULL;
		m_markPath.clear();
		m_bMarkSelected = false;
		class CGuard
		{
		public:
			CGuard() { g_mouse->InitDevice(g_hwnd, false); }
			~CGuard() { g_mouse->InitDevice(g_hwnd, true); }
		} guard;
		if (GetOpenFileName(&ofn))
		{
			char filename[_MAX_FNAME] = { 0 };
			char ext[_MAX_EXT] = { 0 };
			_splitpath(ofn.lpstrFile, NULL, NULL, filename, ext);
			if (strcmp(".png", ext))
			{
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 26, 1052, 0, 0, 0);
				return;
			}
			m_markPath = ofn.lpstrFile;
			HANDLE file =
				CreateFile(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file == INVALID_HANDLE_VALUE)
			{
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 26, 1057, 0, 0, 0);
				return;
			}
			unsigned int size = UWIN::GetFileSize(ofn.lpstrFile);
			if (!size || size > 0x2000000)
			{
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 26, 1058, 0, 0, 0);
				return;
			}
			char* data = new char[size];
			DWORD bytesRead = 0;
			ReadFile(file, data, size, &bytesRead, NULL);
			if (!bytesRead)
			{
				if (data)
					delete[] data;
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 26, 1056, 0, 0, 0);
				return;
			}
			m_pMarkBitmap = g_resrcmng->LoadPNG(data, true, false);
			if (!m_pMarkBitmap)
			{
				if (data)
					delete[] data;
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 26, 1056, 0, 0, 0);
				return;
			}
			if (m_pMarkBitmap->bi->bmiHeader.biBitCount != 32)
			{
				if (data)
					delete[] data;
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 26, 1059, 0, 0, 0);
				return;
			}
			bool tooWide = false;
			if (m_pMarkBitmap->Width() > 22)
				tooWide = true;
			bool oversized = m_pMarkBitmap->Height() > 20 || tooWide == true;
			if (oversized == true)
			{
				ClearTexCache(m_pMarkBitmap);
				delete m_pMarkBitmap;
				m_pMarkBitmap = NULL;
				if (data)
					delete[] data;
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 26, 1051, 0, 0, 0);
				return;
			}
			std::string name(filename);
			name += ext;
			m_pMarkPath->ClearLine();
			m_pMarkPath->AddLine(name.c_str(), false, false);
			if (data)
				delete[] data;
			m_bMarkSelected = true;
		}
	}

	void FrRegisterGuildMark::OnConfirm_ButtonInit(int param)
	{
		m_pButton[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrRegisterGuildMark::OnConfirm_LButtonUp()
	{
		if (m_bMarkSelected == true)
		{
			if (!m_markPath.empty())
			{
				unsigned int guild = 0;
				AfxGetTask()->GetActor(s_guildActor[1])
					<< MsgObject(NULL, 611, 32, (int)&guild, 0, 0, 0);
				WSendPacket packet((enumClientPacket)274);
				packet.Encode4(guild);
				packet.Send(TO_GAME);
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			}
		}
		else
		{
			AfxGetTask()->GetActor(s_guildActor[1])
				<< MsgObject(NULL, 611, 26, 1050, 0, 0, 0);
			m_bMarkSelected = false;
			m_pMarkPath->ClearLine();
		}
	}

	void FrRegisterGuildMark::OnCancel_ButtonInit(int param)
	{
		m_pButton[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrRegisterGuildMark::OnCancel_LButtonUp()
	{
		Close(FrCANCEL, true);
	}

	void FrRegisterGuildMark::OnMarkViewer_AreaInit(int param)
	{
		m_pMarkViewer = DYNAMIC_CAST(FrArea, (FrWnd*)param);
		if (m_pMarkViewer)
			m_pMarkBitmap = g_resrcmng->LoadPNG("guildmark", false, false);
	}

	void FrRegisterGuildMark::OnMarkViewer_OwnerDraw(int)
	{
		if (m_pMarkBitmap)
		{
			const WRect& area = m_pMarkViewer->GetRect();
			g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pMarkBitmap,
				WRect(area.x, area.y, (float)m_pMarkBitmap->Width(),
					(float)m_pMarkBitmap->Height()),
				0xffffffff, 0);
		}
	}

	void FrRegisterGuildMark::OnMarkPath_EditInit(int param)
	{
		m_pMarkPath = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
		if (m_pMarkPath)
			m_pMarkPath->SetReadOnly(true);
	}

	IMPLEMENT_OBJECT(FrChangeGuildName, FrForm)

	BEGIN_FRESH_MSGMAP(FrChangeGuildName, FrForm)

	ON_FRESH_VI("original", FRCMD_INIT,
		FrChangeGuildName::OnOriginalName_StaticInit)
	ON_FRESH_VI("exist", FRCMD_INIT,
		FrChangeGuildName::OnInitExistGuildNameButton)
	ON_FRESH_VV("exist", FRCMD_LBUTTONUP,
		FrChangeGuildName::OnLButtonUpExistGuildNameButton)
	ON_FRESH_VI("confirm", FRCMD_INIT,
		FrChangeGuildName::OnGuildNameConfirm_ButtonInit)
	ON_FRESH_VV("confirm", FRCMD_LBUTTONUP,
		FrChangeGuildName::OnGuildNameConfirm_LButtonUp)
	ON_FRESH_VI("cancel", FRCMD_INIT,
		FrChangeGuildName::OnGuildNameCancel_ButtonInit)
	ON_FRESH_VV("cancel", FRCMD_LBUTTONUP,
		FrChangeGuildName::OnGuildNameCancel_LButtonUp)
	ON_FRESH_VI("guildname", FRCMD_INIT,
		FrChangeGuildName::OnInitChangeGuildNameEdit)

	END_FRESH_MSGMAP()

	FrChangeGuildName::FrChangeGuildName()
	{
		memset(m_pButton, 0, sizeof(m_pButton));
		m_pOriginalName = NULL;
		m_pGuildNameEdit = NULL;
	}

	FrChangeGuildName::~FrChangeGuildName()
	{
	}

	bool FrChangeGuildName::IsHaveGuild()
	{
		IActor* actor = AfxGetTask()->GetActor(s_guildActor[1]);
		GUILD_INFO* info = NULL;
		actor << MsgObject(NULL, 611, 33, (int)&info, 0, 0, 0);
		if (!info->guildUID)
		{
			actor << MsgObject(NULL, 611, 26, 1040, 0, 0, 0);
			return false;
		}
		return true;
	}

	void FrChangeGuildName::SetConfirmedName(const char* name)
	{
		m_confirmedName = name;
	}

	void FrChangeGuildName::OnOriginalName_StaticInit(int param)
	{
		m_pOriginalName = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
		if (m_pOriginalName)
		{
			IActor* actor = AfxGetTask()->GetActor(s_guildActor[1]);
			GUILD_INFO* info = NULL;
			actor << MsgObject(NULL, 611, 33, (int)&info, 0, 0, 0);
			if (!info->guildUID)
				Close(true);
			else
				m_pOriginalName->SetCaption(MakeStr("%s", info->guildName));
		}
	}

	void FrChangeGuildName::OnInitExistGuildNameButton(int param)
	{
		m_pButton[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrChangeGuildName::OnLButtonUpExistGuildNameButton()
	{
		std::string name;
		name = m_pGuildNameEdit->GetLine(1, false);
		if (name.empty())
		{
			AfxGetTask()->GetActor(s_guildActor[1])
				<< MsgObject(NULL, 611, 26, 1000, 0, 0, 0);
			return;
		}
		IActor* actor = AfxGetTask()->GetActor(s_guildActor[1]);
		if (!actor)
			return;
		int result = Doc()->m_chatManager.FilteringGuildString(name.c_str(), 4,
			20, true);
		if (result != 1)
		{
			IActor* errorActor = AfxGetTask()->GetActor(s_guildActor[1]);
			if (errorActor && result > 0)
				errorActor << MsgObject(NULL, 611, 26, result, 0, 0, 0);
			return;
		}
		unsigned int guild;
		actor << MsgObject(NULL, 611, 32, (int)&guild, 0, 0, 0);
		WSendPacket packet((enumClientPacket)255);
		packet.EncodeStr(name);
		packet.Send(TO_GAME);
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}

	void FrChangeGuildName::OnGuildNameConfirm_ButtonInit(int param)
	{
		m_pButton[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrChangeGuildName::OnGuildNameConfirm_LButtonUp()
	{
		std::string name;
		name = m_pGuildNameEdit->GetLine(1, false);
		if (name.empty())
		{
			AfxGetTask()->GetActor(s_guildActor[1])
				<< MsgObject(NULL, 611, 26, 1000, 0, 0, 0);
			return;
		}
		std::string original(m_pOriginalName->GetCaption());
		if (m_confirmedName == original)
		{
			AfxGetTask()->GetActor(s_guildActor[1])
				<< MsgObject(NULL, 611, 26, 54003, 0, 0, 0);
			return;
		}
		if (m_confirmedName != name)
		{
			AfxGetTask()->GetActor(s_guildActor[1])
				<< MsgObject(NULL, 611, 26, 1028, 0, 0, 0);
			return;
		}
		IActor* actor = AfxGetTask()->GetActor(s_guildActor[1]);
		if (!actor)
			return;
		unsigned int guild;
		actor << MsgObject(NULL, 611, 32, (int)&guild, 0, 0, 0);
		WSendPacket packet((enumClientPacket)256);
		packet.Encode4(guild);
		packet.EncodeStr(name);
		packet.Send(TO_GAME);
		Close(FrOK, true);
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}

	void FrChangeGuildName::OnGuildNameCancel_ButtonInit(int param)
	{
		m_pButton[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrChangeGuildName::OnGuildNameCancel_LButtonUp()
	{
		Close(true);
	}

	void FrChangeGuildName::OnInitChangeGuildNameEdit(int param)
	{
		m_pGuildNameEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	}
}
