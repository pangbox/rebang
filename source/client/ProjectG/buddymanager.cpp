#include "buddymanager.h"
#include "imessage.h"
#include "chatmsg.h"
#include "lobbytask.h"

BuddyManager* BuddyMGR = NULL;

void ProcMsg(sockaddr_in& addr, const char* packet, int size)
{
	BuddyMGR->ProcCallBackMsg(addr, packet, size);
}

void ProcErr(const char* err)
{
	BuddyMGR->ProcCallBackErr(err);
}

BuddyManager::BuddyManager()
	: m_bInit(false)
{
	Init();
	BuddyMGR = this;
}

BuddyManager::~BuddyManager()
{
	UnRegisterDNS();
}

bool BuddyManager::Init()
{
	IsInit();
	if (false)
	{
		void* pfnMsg = ProcMsg;
		void* pfnErr = ProcErr;
	}

	return m_bInit;
}

void BuddyManager::Close()
{
	// Compiled out
}

void BuddyManager::UnRegisterDNS()
{
	ISendMsg msg(6);
	msg.EncodeStr(MyId());
	msg.MakePacketComplete();
	SendDNS((const char*)msg.GetBuffer(), msg.GetLength());
}

void BuddyManager::CheckBuddyList()
{
}

void BuddyManager::QueryBuddy(const char* nick)
{
	ISendMsg msg(2);
	msg.EncodeStr(MyId());
	msg.EncodeStr(nick);
	msg.MakePacketComplete();
}

void BuddyManager::ClearBuddy()
{
	m_buddyList.clear();
}

void BuddyManager::SendDNS(const char* addr, int port)
{
	// Compiled out
}

void BuddyManager::AddBuddy(sFriend* pFriend)
{
	if (pFriend)
	{
		Doc()->m_chatManager.FilteringHack(pFriend->szAlias, false);

		m_buddyList[pFriend->Uid] = *pFriend;
	}
}

void BuddyManager::DeleteBuddy(unsigned long uid)
{
	m_buddyList.erase(uid);
}

void BuddyManager::SendChat(const char* nick, const char* msg)
{
	ISendMsg sendMsg(4);
	sendMsg.EncodeStr(MyId());
	sendMsg.EncodeStr(nick);

	sendMsg.EncodeStr(msg);
	sendMsg.MakePacketComplete();
}

int BuddyManager::GetBuddySize()
{
	return m_buddyList.size();
}

std::map<unsigned long, sFriend>* BuddyManager::GetBuddyList()
{
	return &m_buddyList;
}

void BuddyManager::UpdateBuddyInfo(sFriend* pFriend)
{
	sFriend* pBuddy = GetBuddy(pFriend->Uid);

	if (pBuddy)
	{
		pBuddy->Channel = pFriend->Channel;
		pBuddy->State = pFriend->State;
	}
	else
	{
		Doc()->m_chatManager.FilteringHack(pFriend->szAlias, false);
		m_buddyList[pFriend->Uid] = *pFriend;
	}
}

void BuddyManager::BuddyLogOn(unsigned long uid)
{
	sFriend* pBuddy = GetBuddy(uid);

	if (pBuddy)
	{
		pBuddy->IsLogOn = 1;
		if (pBuddy->IsAgree)
		{
			DisplayString(MakeStr(
				"\xc4\xa3\xb1\xb8 (%s) \xb4\xd4\xc0\xcc \xc6\xce\xbe\xdf\xbf\xa1 \xc1\xa2\xbc\xd3 \xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
				pBuddy->NickName));
		}
	}
}

void BuddyManager::BuddyLogOut(unsigned long uid)
{
	sFriend* pBuddy = GetBuddy(uid);

	if (pBuddy)
	{
		if (pBuddy->IsLogOn && pBuddy->IsAgree)
		{
			DisplayString(MakeStr(
				"\xc4\xa3\xb1\xb8 (%s) \xb4\xd4\xc0\xcc \xc6\xce\xbe\xdf\xb8\xa6 \xc1\xbe\xb7\xe1 \xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
				pBuddy->NickName));
		}

		pBuddy->IsLogOn = 0;
	}
}

sFriend* BuddyManager::GetBuddyByNick(const char* nick)
{
	std::map<unsigned long, sFriend>::iterator it;

	for (it = m_buddyList.begin(); it != m_buddyList.end(); ++it)
	{
		const char* pNick = it->second.NickName;
		if (stricmp(pNick, nick) == 0)
		{
			return &it->second;
		}
	}

	return NULL;
}

sFriend* BuddyManager::GetBuddy(unsigned long uid)
{
	std::map<unsigned long, sFriend>::iterator it;

	for (it = m_buddyList.begin(); it != m_buddyList.end(); ++it)
	{
		if (it->second.Uid == uid)
		{
			return &it->second;
		}
	}

	return NULL;
}

void BuddyManager::SetRecentPlayer()
{
	// Compiled out
}

void BuddyManager::Process()
{
	static DWORD s_checkTime = 0;
	static DWORD s_registerTime = 0;
	static DWORD s_curTime = 0;

	s_curTime = GetTickCount();

	if (!m_bInit)
		return;
	if (s_curTime - s_registerTime > 60000)
	{
		GetBuddySize();
		s_registerTime = s_curTime;
	}

	if (s_curTime - s_checkTime > 60000)
	{
		s_checkTime = s_curTime;
	}
}

void BuddyManager::DisplayString(const char* str)
{
	if (IS_KINDOF(CLobbyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 3, (int)str, 0xffff7878, 0, 0, 0);
	else
		CChatMsg::Instance()->AddChatMsg(str, 0xffff0000, false, false);
}

void BuddyManager::ProcCallBackMsg(sockaddr_in& addr, const char* packet,
	int size)
{
	IRecvMsg msg;
	msg.SetRcvPacket(packet, size);

	msg.Decode2();
	unsigned short type = msg.Decode2();

	switch (type)
	{
	case 4:
	{
		std::string nick = msg.DecodeStr();
		std::string chat = msg.DecodeStr();

		std::string str = nick;
		str += " : ";
		str += chat;

		DisplayString(str.c_str());
	}
	break;

	case 2:
	{
		sFriend info;
		msg.DecodeBuffer(&info, sizeof(sFriend));
		UpdateBuddyInfo(&info);
	}
	break;

	case 7:
	{
		std::string nick = msg.DecodeStr();
	}
	break;

	case 8:
	{
		std::string nick = msg.DecodeStr();
	}
	break;

	case 9:
	{
		std::string nick = msg.DecodeStr();
	}
	break;
	}
}

void BuddyManager::ProcCallBackErr(const char* err)
{
	// Compiled out
}
