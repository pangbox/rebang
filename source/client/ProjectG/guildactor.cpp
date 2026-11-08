#include "stdafx.h"
#include "ingameguildlobbydlg.h"
#include "contentsdoc.h"
#include "ingameguildmyroomdlg.h"
#include "guildactor.h"
#include "guilddefine.h"

template std::list<GUILD_LIST>::size_type std::list<GUILD_LIST>::size() const;
extern Fresh* g_pFresh;
namespace _guild
{
	class CGuildActor : public IActor, public FrCmdTarget
	{
		DECLARE_OBJECT(CGuildActor)

	protected:
		CGuildActor();

	public:
		virtual ~CGuildActor();

		virtual void OnInit();
		virtual void HandleMsg(const MsgObject& msg);

		void RequestGuildUserList(int page);

		bool OnCreateGuildFormResult(int result, FrForm* form);
		bool OnRegisterGuildMarkResult(int result, FrForm* form);
		bool OnChangeGuildNameResult(int result, FrForm* form);
		bool OnGuildJoinFormResult(int result, FrForm* form);
		bool OnMainFormCloseResult(int result, FrForm* form);
		bool OnGuildMasterDelegate(int result, FrForm* form);
		bool OnGuildShutDownResult(int result, FrForm* form);
		bool OnGuildOutResult(int result, FrForm* form);

	private:
		void InitControl();
		void ExecGuildMessage(const MsgObject& msg);
		void OpenGuileMainForm();
		void OpenGuildItemForm(unsigned long itemTypeID);
		void OpenGuildJoinForm(int guildUID);
		void OpenGuildQuestionForm(const char* msg, int type);
		void ReInitMyGuildInfo_SharedDoc(GUILD_USER_INFO* pInfo);
		std::pair<bool, bool> CheckMyGuildState(GUILD_USER_INFO* pInfo);
		void ChangeGuildEmblem(GUILD_USER_INFO* pInfo);
		void ChangeGuildName(GUILD_INFO* pInfo);
		const char* SystemMsg(int index);

		FrCreateGuildKit* m_pCreateGuildKit;
		FrRegisterGuildMark* m_pRegisterGuildMark;
		FrChangeGuildName* m_pChangeGuildName;
		FrGuildJoin* m_pGuildJoin;
		CGuildMainForm* m_pMainForm;
		CGuildInfo* m_pGuildInfo;
	};

	IMPLEMENT_ACTOR(CGuildActor, IActor)

	CGuildActor::CGuildActor()
	{
		(void)&g_EPSILON;
		InitControl();
	}

	CGuildActor::~CGuildActor()
	{
	}

	void CGuildActor::InitControl()
	{
		m_pCreateGuildKit = NULL;
		m_pRegisterGuildMark = NULL;
		m_pChangeGuildName = NULL;
		m_pGuildJoin = NULL;
		m_pMainForm = NULL;
		m_pGuildInfo = NULL;
	}

	bool CGuildActor::OnCreateGuildFormResult(int result, FrForm* form)
	{
		m_pCreateGuildKit = NULL;
		return true;
	}

	bool CGuildActor::OnRegisterGuildMarkResult(int result, FrForm* form)
	{
		m_pRegisterGuildMark = NULL;
		return true;
	}

	bool CGuildActor::OnChangeGuildNameResult(int result, FrForm* form)
	{
		m_pChangeGuildName = NULL;
		return true;
	}

	bool CGuildActor::OnGuildJoinFormResult(int result, FrForm* form)
	{
		m_pGuildJoin = NULL;
		return true;
	}

	bool CGuildActor::OnMainFormCloseResult(int result, FrForm* form)
	{
		m_pMainForm = NULL;
		return true;
	}

	bool CGuildActor::OnGuildMasterDelegate(int result, FrForm* form)
	{
		if (result == FrOK && m_pMainForm)
			m_pMainForm->RequestChangeClass((GUILD_CLASS_IDX)1);
		return true;
	}

	void CGuildActor::OnInit()
	{
		CContentsDoc::Instance()->GetContainer((localContentType_t)151,
			m_pGuildInfo);
	}

	void CGuildActor::RequestGuildUserList(int page)
	{
		if (page >= 1 && m_pGuildInfo->IsGuildMember())
		{
			WSendPacket packet((enumClientPacket)271);
			packet.Encode4(m_pGuildInfo->GetMyGuildInfo().guildUID);
			packet.Encode4(page);
			packet.Send(TO_GAME);
		}
	}

	const char* CGuildActor::SystemMsg(int index)
	{
		const char* text = m_pGuildInfo->GuildMsg(index);
		if (!text)
			return K2L_Compatibility(
				"\xc1\xa4\xc0\xc7 \xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xbf\xa1\xb7\xaf \xb8\xde\xbd\xc3\xc1\xf6\xc0\xd4\xb4\xcf\xb4\xd9.");
		return text;
	}

	void CGuildActor::OpenGuileMainForm()
	{
		if (!m_pMainForm)
		{
			m_pMainForm = CreateForm<CGuildMainForm>(g_pFresh->GetManager(),
				this, "guildmain");
			m_pMainForm->EnableDrag(false);
			m_pMainForm->SetLinkGuildInfo(m_pGuildInfo);
			m_pMainForm->Open(
				(FRESH_PFN_RESULT)&CGuildActor::OnMainFormCloseResult,
				FrESCAPE);
		}
	}

	void CGuildActor::OpenGuildItemForm(unsigned long itemTypeID)
	{
		switch (itemTypeID)
		{
		case 436207919:
			if (!m_pCreateGuildKit)
			{
				m_pCreateGuildKit = CreateForm<FrCreateGuildKit>(
					g_pFresh->GetManager(), this, "create_guild");
				m_pCreateGuildKit->EnableDrag(false);
				m_pCreateGuildKit->SetCaption(SystemMsg(1005));
				m_pCreateGuildKit->Open(
					(FRESH_PFN_RESULT)&CGuildActor::OnCreateGuildFormResult,
					FrESCAPE);
			}
			break;
		case 436207920:
			if (!m_pRegisterGuildMark)
			{
				if (!m_pGuildInfo->IsManager())
				{
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 35, (int)SystemMsg(54507), 0, 0, 0, 0));
					break;
				}
				m_pRegisterGuildMark = CreateForm<FrRegisterGuildMark>(
					g_pFresh->GetManager(), this, "register_guild_mark");
				m_pRegisterGuildMark->EnableDrag(false);
				m_pRegisterGuildMark->SetCaption(SystemMsg(1006));
				m_pRegisterGuildMark->Open(
					(FRESH_PFN_RESULT)&CGuildActor::OnRegisterGuildMarkResult,
					FrESCAPE);
			}
			break;
		case 436207921:
			if (!m_pChangeGuildName)
			{
				if (!m_pGuildInfo->IsManager())
				{
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 35, (int)SystemMsg(54505), 0, 0, 0, 0));
					break;
				}
				m_pChangeGuildName = CreateForm<FrChangeGuildName>(
					g_pFresh->GetManager(), this, "change_guild_name");
				if (m_pChangeGuildName->IsHaveGuild())
				{
					m_pChangeGuildName->EnableDrag(false);
					m_pChangeGuildName->SetCaption(SystemMsg(1007));
					m_pChangeGuildName->Open(
						(FRESH_PFN_RESULT)&CGuildActor::OnChangeGuildNameResult,
						FrESCAPE);
				}
			}
			break;
		}
	}

	void CGuildActor::OpenGuildJoinForm(int guildUID)
	{
		if (!m_pGuildJoin)
		{
			m_pGuildJoin = CreateForm<FrGuildJoin>(g_pFresh->GetManager(), this,
				"requestjoinform");
			m_pGuildJoin->SetFixed(true);
			m_pGuildJoin->EnableDrag(false);
			m_pGuildJoin->SetGuildNumber(guildUID);
			m_pGuildJoin->Open(
				(FRESH_PFN_RESULT)&CGuildActor::OnGuildJoinFormResult,
				FrESCAPE);
		}
	}

	void CGuildActor::ReInitMyGuildInfo_SharedDoc(GUILD_USER_INFO* pInfo)
	{
		sPangYaUserInfo& info =
			const_cast<sPangYaUserInfo&>(Doc()->GetPangyaUserInfo());
		info.dwGuildId = pInfo->guildUID;
		info.dwEmblemVer = pInfo->guildPang;
		strncpy(info.sGuild, pInfo->guildName, 20);
		strncpy(info.szEmblemName, pInfo->guildMark, 11);
	}

	void CGuildActor::ChangeGuildName(GUILD_INFO* pInfo)
	{
		if (pInfo->guildUID)
		{
			m_pGuildInfo->SetGuildName(pInfo);
			strncpy(Doc()->m_myInfo.info.sGuild, pInfo->guildName, 20);
		}
	}

	bool CGuildActor::OnGuildShutDownResult(int result, FrForm* form)
	{
		if (result == FrOK)
		{
			WSendPacket packet((enumClientPacket)260);
			packet.Encode4(m_pGuildInfo->GetMyGuildInfo().guildUID);
			packet.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
		return true;
	}

	bool CGuildActor::OnGuildOutResult(int result, FrForm* form)
	{
		if (result == FrOK)
		{
			WSendPacket packet((enumClientPacket)272);
			packet.Encode4(m_pGuildInfo->GetMyGuildInfo().guildUID);
			packet.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
		return true;
	}

	void CGuildActor::OpenGuildQuestionForm(const char* msg, int type)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");
		form->SetMessage(msg, false);
		form->SetTopmost(true);
		form->FindNextTopFocus(false);
		FRESH_PFN_RESULT callback = NULL;
		switch (type)
		{
		case 0:
			callback = (FRESH_PFN_RESULT)&CGuildActor::OnGuildShutDownResult;
			break;
		case 1:
			callback = (FRESH_PFN_RESULT)&CGuildActor::OnGuildMasterDelegate;
			break;
		case 2:
			callback = (FRESH_PFN_RESULT)&CGuildActor::OnGuildOutResult;
			break;
		}
		if (callback)
			form->Open(callback, FrESCAPE);
	}

	void CGuildActor::ChangeGuildEmblem(GUILD_USER_INFO* pInfo)
	{
		if (pInfo->guildUID)
		{
			m_pGuildInfo->SetGuildEmblem(pInfo);
			const char* mark = pInfo->guildMark;
			strncpy(Doc()->m_myInfo.info.szEmblemName, mark, 11);
			CSharedDoc* doc = Doc();
			std::map<unsigned long, sBriefUserInfo>::iterator it;
			unsigned long guid = doc->m_myInfo.info.dwGuid;
			if (doc->FindUserInfo(guid, it) == 1)
				strncpy(it->second.szEmblemName, mark, 11);
			strncpy(Doc()->m_myInfo.info.szEmblemName, mark, 11);
		}
	}

	void CGuildActor::ExecGuildMessage(const MsgObject& msg)
	{
		switch (msg.param1)
		{
		case 1:
			OpenGuileMainForm();
			return;
		case 27:
			OpenGuildItemForm(msg.param2);
			return;
		case 29:
			OpenGuildJoinForm(msg.param2);
			return;
		case 30:
			OpenGuildQuestionForm((const char*)msg.param2, msg.param3);
			return;
		case 3:
		{
			std::string* name = (std::string*)msg.param2;
			if (m_pCreateGuildKit)
				m_pCreateGuildKit->SetConfirmGuildName(name->c_str());
			if (m_pChangeGuildName)
				m_pChangeGuildName->SetConfirmedName(name->c_str());
		}
			return;
		case 4:
		{
			m_pGuildInfo->ClearAllData();
			GUILD_INFO* info = (GUILD_INFO*)msg.param2;
			m_pGuildInfo->SetMyGuildInfo(*info);
			ReInitMyGuildInfo_SharedDoc(info);
			if (m_pMainForm)
			{
				{
					WSendPacket packet((enumClientPacket)263);
					packet.Send(TO_GAME);
				}
				m_pMainForm->Refresh();
				m_pMainForm->RequestMember_Page(0);
			}
		}
			return;
		case 5:
		{
			GUILD_USER_INFO* info = (GUILD_USER_INFO*)msg.param2;
			std::pair<bool, bool> change = CheckMyGuildState(info);
			if (change.first)
			{
				m_pGuildInfo->ClearAllData();
				Doc()->ClearMyGuildInfo();
				if (m_pMainForm)
					m_pMainForm->ClearForm();
			}
			if (change.second)
			{
				m_pGuildInfo->ClearMemberPage();
				m_pGuildInfo->SetMyGuildUserInfo(*info);
				ReInitMyGuildInfo_SharedDoc(info);
				if (m_pMainForm)
				{
					m_pMainForm->Refresh();
					m_pMainForm->RequestMember_Page(0);
				}
			}
		}
			return;
		case 11:
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 35, (int)SystemMsg(1025), 0, 0, 0, 0));
			return;
		case 12:
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 35, (int)SystemMsg(1021), 0, 0, 0, 0));
			return;
		case 6:
		{
			m_pGuildInfo->ClearAllData();
			GUILD_USER_INFO* info = (GUILD_USER_INFO*)msg.param2;
			m_pGuildInfo->SetMyGuildUserInfo(*info);
			ReInitMyGuildInfo_SharedDoc(info);
		}
			return;
		case 7:
		{
			int count = msg.param4;
			m_pGuildInfo->SetMemberPage(msg.param3,
				(std::list<GUILD_USER_LIST>*)msg.param2);
			m_pGuildInfo->SetAllMemberCount(count);
			if (m_pMainForm)
				m_pMainForm->RefreshGuildMember();
		}
			return;
		case 8:
		{
			std::list<GUILD_LIST>* list = (std::list<GUILD_LIST>*)msg.param2;
			int page = msg.param3;
			int count = msg.param4;
			if (list->size() == 0)
			{
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 35, (int)SystemMsg(1035), 0, 0, 0, 0));
				if (m_pMainForm)
					m_pMainForm->InitSearchMode();
			}
			else
			{
				m_pGuildInfo->SetSearchList(page, list);
				m_pGuildInfo->SetAllGuildCount(count);
				if (m_pMainForm)
					m_pMainForm->RefreshGuildSearch();
			}
		}
			return;
		case 9:
			m_pGuildInfo->SetHistoryList((std::list<GUILD_HISTORY>*)msg.param2);
			if (m_pMainForm)
				m_pMainForm->RefreshMyGuildHistory();
			return;
		case 10:
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 35, (int)SystemMsg(1016), 0, 0, 0, 0));
			return;
		case 14:
			if (m_pMainForm)
				m_pMainForm->DisableJoinButton();
			if (m_pGuildJoin)
				m_pGuildJoin->SetSuccessRequest();
			return;
		case 15:
			if (m_pMainForm)
				m_pMainForm->RequestMember_Page(0);
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 35, (int)SystemMsg(1019), 0, 0, 0, 0));
			return;
		case 16:
			if (m_pMainForm)
				m_pMainForm->RequestMember_Page(0);
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 35, (int)SystemMsg(1018), 0, 0, 0, 0));
			return;
		case 17:
			if (m_pMainForm && m_pGuildJoin)
			{
				m_pGuildJoin->Close(FrCANCEL, true);
				int code = msg.param2;
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 35, (int)SystemMsg(code), 0, 0, 0, 0));
			}
			return;
		case 13:
			if (m_pMainForm)
				m_pMainForm->RequestMember_Page(0);
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 35, (int)SystemMsg(1017), 0, 0, 0, 0));
			return;
		case 18:
			if (m_pMainForm)
			{
				m_pMainForm->RequestMember_Page(0);
				m_pMainForm->UnLockScrollMemberList();
			}
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 35, (int)SystemMsg(1030), 0, 0, 0, 0));
			return;
		case 19:
		{
			if (m_pMainForm)
				m_pMainForm->RequestMember_Page(0);
			const char* text;
			switch (msg.param2)
			{
			case 1:
				text =
					MakeStr(SystemMsg(1033), SystemMsg(1077), SystemMsg(1079));
				break;
			case 2:
				text = MakeStr(SystemMsg(1034), SystemMsg(1078));
				break;
			case 3:
				text = MakeStr(SystemMsg(1034), SystemMsg(1079));
				break;
			default:
				return;
			}
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 35, (int)text, 0, 0, 0, 0));
		}
			return;
		case 32:
		{
			unsigned long* uid = (unsigned long*)msg.param2;
			if (m_pGuildInfo->IsGuildMember())
				*uid = m_pGuildInfo->GetMyGuildInfo().guildUID;
			else
				*uid = 0;
		}
			return;
		case 33:
		{
			const GUILD_INFO** info = (const GUILD_INFO**)msg.param2;
			*info = &m_pGuildInfo->GetMyGuildInfo();
		}
			return;
		case 34:
		{
			GUILD_CLASS_IDX* index = (GUILD_CLASS_IDX*)msg.param2;
			*index = m_pGuildInfo->GetClassIndex();
		}
			return;
		case 21:
			if (m_pRegisterGuildMark)
				m_pRegisterGuildMark->RequestUpload(msg.param2,
					*(std::string*)msg.param3);
			return;
		case 22:
			ChangeGuildEmblem((GUILD_USER_INFO*)msg.param2);
			return;
		case 23:
			ChangeGuildName((GUILD_INFO*)msg.param2);
			return;
		case 24:
		{
			WSendPacket packet(42);
			packet.Encode4(m_pGuildInfo->GetGuildNumber());
			packet.Send(TO_MSN);
		}
			return;
		case 25:
		{
			WSendPacket packet(43);
			packet.Encode4(m_pGuildInfo->GetGuildNumber());
			packet.Send(TO_MSN);
		}
			return;
		case 26:
		{
			int code = msg.param2;
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 35, (int)SystemMsg(code), 0, 0, 0, 0));
		}
			return;
		}
	}

	std::pair<bool, bool> CGuildActor::CheckMyGuildState(GUILD_USER_INFO* pInfo)
	{
		if (!m_pGuildInfo)
			return std::pair<bool, bool>(true, false);
		GUILD_CLASS_IDX previous = m_pGuildInfo->GetClassIndex();
		switch (pInfo->classIdx)
		{
		case 0:
			if (previous == 1 || previous == 3 || previous == 2)
				return std::pair<bool, bool>(true, false);
			if (previous == 9)
				return std::pair<bool, bool>(false, true);
			break;
		case 9:
			if (previous == 0)
				return std::pair<bool, bool>(false, true);
			break;
		case 3:
			if (previous == 9)
			{
				ChangeGuildEmblem(pInfo);
				return std::pair<bool, bool>(false, true);
			}
			else if (previous == 2)
				return std::pair<bool, bool>(false, true);
			else if (previous == 1)
				return std::pair<bool, bool>(false, true);
			break;
		case 2:
			if (previous == 3)
				return std::pair<bool, bool>(false, true);
			break;
		case 1:
			if (previous == 2 || previous == 0)
				return std::pair<bool, bool>(false, true);
			break;
		}
		return std::pair<bool, bool>(false, false);
	}

	void CGuildActor::HandleMsg(const MsgObject& msg)
	{
		if (msg.message == 611)
			ExecGuildMessage(msg);
	}
}
