#include "stdafx.h"
#include "ghostdata.h"

class CGhostDocument;

class CGhostRecorder : public IActor
{
public:
	DECLARE_OBJECT(CGhostRecorder)
	CGhostRecorder();
	virtual ~CGhostRecorder() { }
	virtual void OnLoad();
	virtual void OnProcess(float delta) { }
	virtual void OnDisplay() { }
	virtual void OnReload();
	virtual void OnInit();
	virtual void OnDestroy();

protected:
	virtual void HandleMsg(const MsgObject& msg);

private:
	bool IsSaveAvailable();
	CGhostDocument* m_pDocument;
};

#include "ghosthandler.h"
#include "ghostmanager.h"
#include "ghostloader.h"
#include "ghostdocument.h"
#include "ghostgamedlg.h"
#include "golfdoc.h"
#include "mousecursor.h"
#include "tinyxml.h"

extern Fresh* g_pFresh;

IMPLEMENT_ACTOR(CGhostRecorder, IActor)

CGhostRecorder::CGhostRecorder()
	: m_pDocument(NULL)
{
}

bool CGhostRecorder::IsSaveAvailable()
{
	if (Doc()->m_golfGame.gameType != 4 && Doc()->m_golfGame.gameType != 0)
		return false;
	if (Doc()->m_holeType == 4)
		return false;
	if (Doc()->m_roomInfo.realGameType == 14)
		return false;
	if (Doc()->m_golfGame.holes != 18)
		return false;
	return true;
}

void CGhostRecorder::HandleMsg(const MsgObject& msg)
{
	if (!m_pDocument)
		return;
	if (msg.message != 11)
		m_pDocument->RecordHoleData(msg);
	else
		CGhostManager::Instance()->ReleaseDocument();
}

void CGhostRecorder::OnLoad()
{
	CGhostManager::Instance()->ReleaseDocument();
	if (OnlinePlay() && IsSaveAvailable())
	{
		m_pDocument = CGhostManager::Instance()->GetDocument();
		m_pDocument->RecordUserData();
		m_pDocument->RecordGameData();
	}
}

void CGhostRecorder::OnReload()
{
}

void CGhostRecorder::OnInit()
{
}

void CGhostRecorder::OnDestroy()
{
}

CGhostLoader::CGhostLoader()
{
}

bool CGhostLoader::LoadFromMemory(unsigned char* data, unsigned int size)
{
	CGhostManager::Instance()->ReleaseDocument();
	CGhostDocument* doc = CGhostManager::Instance()->GetDocument();
	return doc->Import(data, size);
}

bool CGhostLoader::LoadFromFile(const std::string& name)
{
	CGhostManager::Instance()->ReleaseDocument();
	CGhostDocument* doc = CGhostManager::Instance()->GetDocument();
	return doc->Import(name);
}

namespace
{
	const char* const NOTIFY_MSG_SERVICE_STOP = K2L_Compatibility(
		"\xb0\xed\xbd\xba\xc6\xae \xbc\xad\xba\xf1\xbd\xba \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xcc\xbf\xeb\xbf\xa1 \xba\xd2\xc6\xed\xc0\xbb \xb5\xe5\xb7\xc1\xbc\xad \xc1\xcb\xbc\xdb\xc7\xd5\xb4\xcf\xb4\xd9.");
	const char* const RECORD_DLG_LOADING_FAIL = K2L_Compatibility(
		"\xb7\xce\xb5\xf9\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
	const char* const CANNOT_UNDERSTAND_MSG = K2L_Compatibility(
		"\xb4\xd4\xc0\xc7 \xb0\xed\xbd\xba\xc6\xae\xb0\xa1 \xbe\xee\xb8\xae\xb5\xd5\xc0\xfd\xc7\xd8 \xc7\xd5\xb4\xcf\xb4\xd9.");
	const char* const NOTIFY_MSG_GHOST_NOSAVE = K2L_Compatibility(
		"\xc7\xf6\xc0\xe7 \xc0\xaf\xc0\xfa\xbf\xa1\xb0\xd4 \xc0\xfa\xc0\xe5\xb5\xc8 \xb0\xed\xbd\xba\xc6\xae\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
	const char* const NOTIFY_MSG_GHOST_NODATA = K2L_Compatibility(
		"\xb0\xed\xbd\xba\xc6\xae \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xc1\xb8\xc0\xe7 \xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
	const char* const NOTIFY_MSG_GHOST_SAVED = K2L_Compatibility(
		"\xb0\xed\xbd\xba\xc6\xae \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xb0\xbb\xbd\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
	const char* const NOTIFY_MSG_UNKNOWN_ERROR = K2L_Compatibility(
		"\xbd\xc3\xbd\xba\xc5\xdb \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
	const char* const NOTIFY_MSG_DATA_ERROR = K2L_Compatibility(
		"\xbc\xd5\xbb\xf3\xb5\xc8 \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xb9\xdf\xb0\xdf\xb5\xc7\xbe\xee \xb4\xf5\xc0\xcc\xbb\xf3 \xb0\xd4\xc0\xd3\xc0\xbb \xc1\xf8\xc7\xe0\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
	const char* const NOTIFY_MSG_NETWAIT_OVER = K2L_Compatibility(
		"\xbc\xad\xb9\xf6\xbf\xa1 \xbf\xe4\xc3\xbb\xc0\xcc \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
	const char* const GHOST_ROOM_JOIN_WELCOME = K2L_Compatibility(
		"(\xbe\xcb\xb8\xb2)\134c0xffffffff\134c%s\134c0xffff7878\134c\xb4\xd4\xc0\xcc \xc0\xd4\xc0\xe5\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.");
	const char* const NOTIFY_MSG_DISABLE_GAME = K2L_Compatibility(
		"\xbc\xd5\xbb\xf3\xb5\xc8 \xb5\xa5\xc0\xcc\xc5\xcd\xbf\xcd\xb4\xc2 \xc0\xe7\xb4\xeb\xb0\xe1 \xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xb8\xa5 \xb0\xed\xbd\xba\xc6\xae\xbf\xcd \xb4\xeb\xc0\xfc\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.");
	const char* const NOTIFY_MSG_NOT_DEFINED = K2L_Compatibility(
		"\xc1\xa4\xc0\xc7\xb5\xc8 \xbf\xc0\xb7\xf9 \xb8\xde\xbc\xbc\xc1\xf6\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");

	class CEventGhostXmlFile
	{
	public:
		bool Load()
		{
			m_gameList.clear();
			TiXmlDocument doc;
			if (!doc.LoadFileEx("event_ghost.xml"))
				return false;
			std::string uid, file;
			TiXmlNode* active = doc.FirstChild("active");
			if (!active)
				return false;
			uid = active->ToElement()->Attribute("uid");
			if (uid == "-1")
				return false;
			TiXmlNode* element = active->FirstChild("element");
			TiXmlElement* item;
			while (element)
			{
				item = element->ToElement();
				if (uid == item->Attribute("uid"))
				{
					m_nick = item->Attribute("nick");
					TiXmlNode* param = element->FirstChild("param");
					while (param)
					{
						file = param->ToElement()->Attribute("file");
						if (!file.empty())
						{
							GHOST_GAME_BRIEF brief;
							memset(&brief, 0, sizeof(brief));
							GetGhostGameBriefFromFileName(&brief, file.c_str());
							brief.recordCount = 1;
							m_gameList.push_back(brief);
						}
						param = param->NextSibling();
					}
					return !m_gameList.empty();
				}
				element = element->NextSibling();
			}
			return false;
		}
		const std::string& GetNick() const { return m_nick; }
		const std::list<GHOST_GAME_BRIEF>& GetGameList() const
		{
			return m_gameList;
		}

	private:
		std::string m_nick;
		std::list<GHOST_GAME_BRIEF> m_gameList;
	};
}

CGhostHandler::CGhostHandler()
{
}

CGhostHandler::~CGhostHandler()
{
}

bool CGhostHandler::IsAvailableGhostSystem()
{
	if (!IsLocalContent(S4_GHOST))
		return false;
	if (Doc()->m_curChannel.Type & 0x800)
		return false;
	return true;
}

void CGhostHandler::OpenNotifyDlg(int type)
{
	FrForm* dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
	dlg->SetTopmost(true);
	dlg->FindNextTopFocus(NULL);
	dlg->SetTimeLimit(7.0f);
	switch (type)
	{
	case 7:
		dlg->SetMessage(NOTIFY_MSG_SERVICE_STOP, false);
		break;
	case 0:
		dlg->SetMessage(NOTIFY_MSG_GHOST_NOSAVE, false);
		break;
	case 1:
		dlg->SetMessage(NOTIFY_MSG_GHOST_NODATA, false);
		break;
	case 2:
		dlg->SetMessage(RECORD_DLG_LOADING_FAIL, false);
		break;
	case 4:
		dlg->SetMessage(NOTIFY_MSG_UNKNOWN_ERROR, false);
		break;
	case 5:
		dlg->SetMessage(NOTIFY_MSG_DATA_ERROR, false);
		break;
	case 6:
		dlg->SetMessage(NOTIFY_MSG_NETWAIT_OVER, false);
		break;
	case 8:
		dlg->SetMessage(NOTIFY_MSG_DISABLE_GAME, false);
		break;
	default:
		dlg->SetMessage(NOTIFY_MSG_NOT_DEFINED, false);
		break;
	}
	dlg->Open(NULL, 0);
}

bool CGhostHandler::IsExistEventGhost()
{
	CEventGhostXmlFile file;
	return file.Load();
}

bool CGhostHandler::OpenEventGhostGameDlg()
{
	CEventGhostXmlFile file;
	if (!file.Load())
		return false;
	OpenGhostGameDlg(-1, file.GetNick(), file.GetGameList(),
		(FRESH_PFN_RESULT)&CGhostHandler::OnResultEventGhostGameDlg);
	return true;
}

void CGhostHandler::OpenGhostGameDlg(unsigned long uid, const std::string& nick,
	const std::list<GHOST_GAME_BRIEF>& list, FRESH_PFN_RESULT callback)
{
	if (g_pFresh->GetDesktop()->FindChildForm("GHOST_GAMEMAP"))
		return;
	FrGhostGameMapDlg* dlg = CreateForm<FrGhostGameMapDlg>(
		g_pFresh->GetManager(), this, "GHOST_GAMEMAP");
	dlg->SetTopmost(true);
	dlg->FindNextTopFocus(NULL);
	dlg->Init(uid, nick, list);
	dlg->EnableDrag(false);
	dlg->SetFlag(FrNOTAIL);
	dlg->Open(callback, 1);
}

void CGhostHandler::RecordComplete()
{
	if (GOLFDOC())
	{
		int score = PLAYER(MyIndex())->score;
		int pang = PLAYER(MyIndex())->totalPang;
		score += (int)PLAYER(MyIndex())->stroke[GOLFDOC()->m_currentHole - 1] -
			GOLFDOC()->GetHoleData().par;
		unsigned char character =
			(unsigned char)Doc()->m_userInfo[MyIndex()].charInfo.tid;
		if (score < 0)
		{
			WSendPacket packet((enumClientPacket)186);
			packet.Encode1(4);
			packet.Encode1(GetCurMap());
			packet.Encode1(character);
			packet.Encode4(abs(score));
			packet.Encode4(pang);
			packet.Send((eSendTo)0);
			return;
		}
	}
	CGhostManager::Instance()->ReleaseDocument();
}

void CGhostHandler::LoadingComplete(bool success)
{
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
	if (success)
	{
		CMouseCursor::Instance()->SetActive(false);
		CTaskManager::Instance()->ChangeTask("CGhostTask", "",
			CTaskManager::Instance()->GetPreservedTask() != NULL);
		CTaskManager::Instance()->PostMsg(NULL, "Ghost", 0, (int)"GHOST", 0, 0,
			0);
	}
	else
	{
		OpenNotifyDlg(2);
		SendErrorReport("GHOST FILE LOAD FAILED",
			CGhostManager::Instance()->GetLatestFileName());
	}
}

void CGhostHandler::UploadComplete(bool success, const std::string& name)
{
	ClearLocalGhostFiles();
	if (success)
	{
		WSendPacket packet((enumClientPacket)186);
		packet.Encode1(5);
		packet.EncodeStr(name);
		packet.Send((eSendTo)0);
	}
	else
		SendErrorReport("GHOST FILE UPLOAD FAILED.", name);
}

void CGhostHandler::OnGhostPacket(WReceivedPacket& packet)
{
	if (!IsAvailableGhostSystem())
		return;
	switch (packet.Decode1())
	{
	case 12:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
		int count = packet.Decode4();
		unsigned long uid = packet.Decode4();
		if (!count)
		{
			OpenNotifyDlg(0);
			break;
		}
		{
			std::list<GHOST_GAME_BRIEF> games;
			std::string nick;
			for (int i = 0; i < count; ++i)
			{
				unsigned long values[5] = { 0 };
				for (int j = 0; j < 5; ++j)
					values[j] = packet.Decode4();
				packet.DecodeStr(nick);
				unsigned long recordCount = packet.Decode4();
				GHOST_GAME_BRIEF brief;
				brief.owner = uid;
				brief.course = values[0];
				brief.itemId = values[1];
				brief.score = values[2];
				brief.pang = values[3];
				brief.recordId = values[4];
				brief.recordCount = recordCount;
				games.push_back(brief);
			}
			OpenGhostGameDlg(uid, nick, games,
				(FRESH_PFN_RESULT)&CGhostHandler::OnResultGhostGameDlg);
		}
	}
	break;
	case 13:
	{
		std::string name;
		packet.DecodeStr(name);
		CGhostManager::Instance()->SaveDocument(name);
	}
	break;
	case 14:
		CGhostManager::Instance()->ReleaseDocument();
		ClearLocalGhostFiles();
		break;
	case 16:
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
		switch (packet.Decode1())
		{
		case 25:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			OpenNotifyDlg(1);
			break;
		case 28:
			OpenNotifyDlg(7);
			break;
		case 26:
			OpenNotifyDlg(6);
			break;
		}
		break;
	case 19:
	{
		packet.Decode1();
		unsigned long typeId = packet.Decode4();
		unsigned long itemId = packet.Decode4();
		packet.Decode1();
		if (!(typeId & 0x2000000))
			AfxGetTask()->GetActor("Item")
				<< MsgObject(NULL, 415, -1, typeId, itemId, 0, 0);
	}
	break;
	case 22:
	{
		FrWnd* wnd = g_pFresh->GetDesktop()->FindChildByName("GHOST_HISTORY");
		if (!wnd && !IS_KINDOF(FrRecentMatch, wnd))
			break;
		FrRecentMatch* dlg = (FrRecentMatch*)wnd;

		{
			if (packet.Decode1() != 255)
			{
				unsigned long count = packet.Decode4();
				for (unsigned long i = 0; i < count; ++i)
				{
					FrRecentMatch::sRecentMatchForm record;
					record.sex = packet.Decode1();
					record.nick = packet.DecodeStr();
					record.win = packet.Decode4();
					packet.DecodeBuffer(&record.date, sizeof(record.date));
					record.pang = packet.Decode4();
					record.grade = packet.Decode1();
					dlg->AddHistory(record);
				}
				dlg->AddFinish();
			}
			else
				dlg->Exception();
		}
	}
	break;
	}
}

bool CGhostHandler::OnResultGhostGameDlg(int result, FrForm* form)
{
	if (result == FrOK)
	{
		FrGhostGameMapDlg* dlg = DYNAMIC_CAST(FrGhostGameMapDlg, form);
		GHOST_GAME_BRIEF* brief = (GHOST_GAME_BRIEF*)dlg->GetSelectItem();
		if (brief)
		{
			std::string name = MakeGhostGameFileName(brief);
			CGhostManager::Instance()->LoadDocument(name);
			CGhostManager::Instance()->SetRecentSelectedGhostNick(
				dlg->GetGhostNick());
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
		else
			OpenNotifyDlg(4);
	}
	return true;
}

bool CGhostHandler::OnResultEventGhostGameDlg(int result, FrForm* form)
{
	if (result == FrOK)
	{
		FrGhostGameMapDlg* dlg = DYNAMIC_CAST(FrGhostGameMapDlg, form);
		GHOST_GAME_BRIEF* brief = (GHOST_GAME_BRIEF*)dlg->GetSelectItem();
		if (brief)
		{
			std::string name = MakeGhostGameFileName(brief);
			CGhostManager::Instance()->SetRecentSelectedGhostNick(
				dlg->GetGhostNick());
			CGhostManager::Instance()->LoadDocumentFromLocalFile(name);
		}
		else
			OpenNotifyDlg(4);
	}
	return true;
}

void CGhostHandler::ClearLocalGhostFiles()
{
	WIN32_FIND_DATA data;
	HANDLE find = FindFirstFile(".\\temporary_save\\*.GST", &data);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			DeleteFile(data.cFileName);
		} while (FindNextFile(find, &data));
		FindClose(find);
	}
}

void CGhostHandler::SendErrorReport(const std::string& error,
	const std::string& name)
{
	WSendPacket packet((enumClientPacket)186);
	packet.Encode1(8);
	packet.EncodeStr(name);
	packet.EncodeStr(error);
	packet.Send((eSendTo)0);
}

void CGhostHandler::ExceptionFinishGame()
{
	CGhostManager::Instance()->ReleaseDocument();
	CProjectG::Instance()->FromGolfToLobbySettings();
	OpenNotifyDlg(5);
	CTaskManager::Instance()->ChangeTask("CGhostTask", "", false);
	CTaskManager::Instance()->PostMsg(NULL, "Ghost", 0, (int)"GHOST", 1, 0, 0);
}

void CGhostHandler::RequestGhostGameList(unsigned long uid)
{
	if (!IsAvailableGhostSystem())
		return;
	if (IsLocalContent(S4_MATCHING_SYSTEM))
	{
		WSendPacket packet((enumClientPacket)185);
		packet.Encode1(2);
		packet.Send((eSendTo)0);
	}
	WSendPacket packet((enumClientPacket)186);
	packet.Encode1(0);
	packet.Encode4(uid);
	packet.Send((eSendTo)0);
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
}
