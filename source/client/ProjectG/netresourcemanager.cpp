#include "stdafx.h"
#include "netresourcemanager.h"
#include "chttp.h"
#include "generichttpclient.h"
#include "uccmanager.h"
#include "ucclibrary.h"
#include "securitykeymanager.h"
#include "guildactor.h"
#include "guilddefine.h"
#include "commonutil.h"
#include <process.h>

std::string GenerateTextureName_F(unsigned long, const char*);
std::string GenerateTextureName_B(unsigned long, const char*);
bool UnzipUccClothes(unsigned char*, unsigned long, Bitmap*, Bitmap*, Bitmap*,
	sUccHeader*);
bool LoadWarningClothes(unsigned int, Bitmap*, Bitmap*, Bitmap*);

void G_CreateDirectory(const char* name)
{
	std::string path = name;
	if (path.length() > 0)
	{
		std::string part = path.substr(0, path.find("\\", 0));
		path = path.substr(path.find("\\", 0) + 1, std::string::npos);
		std::string directory;
		directory += part;
		directory += "\\";
		CreateDirectory(directory.c_str(), NULL);
		std::string::size_type pos = path.find("\\", 0);
		while (pos != std::string::npos)
		{
			part = path.substr(0, pos);
			path = path.substr(pos + 1, std::string::npos);
			directory += part;
			directory += "\\";
			CreateDirectory(directory.c_str(), NULL);
			pos = path.find("\\", 0);
		}
		directory += path;
		CreateDirectory(directory.c_str(), NULL);
	}
}

unsigned int __stdcall CreateDownloadThreadFunc(void* param)
{
	((NetResourceManager*)param)->WorkDownloadThread();
	return 0;
}

unsigned int __stdcall CreateUploadThreadFunc(void* param)
{
	((NetResourceManager*)param)->WorkUploadThread();
	return 0;
}

NetResourceManager::NetResourceManager()
{
	Init();
}

NetResourceManager::~NetResourceManager()
{
	Close();
}

void NetResourceManager::Init()
{
	strcpy(m_download[RESOURCE_GUILD_EMBLEM].dir, "emblem");
	strcpy(m_download[RESOURCE_GUILD_EMBLEM].url,
		"http://qa.contents.pangya.gametree.co.kr:50006/_Files/GuildMark/");
	strcpy(m_uploadAddress[RESOURCE_GUILD_EMBLEM],
		"http://qa.contents.pangya.gametree.co.kr:50006/Guild/upload.asp");
	m_download[RESOURCE_GUILD_EMBLEM].reserved = 32;
	m_download[RESOURCE_UCC_CLOTHES].dir[0] = 0;
	strcpy(m_download[RESOURCE_UCC_CLOTHES].url,
		"http://qa.contents.pangya.gametree.co.kr:50006/UCC/UCC_ONE/clothes/");
	strcpy(m_uploadAddress[RESOURCE_UCC_CLOTHES],
		"http://qa.contents.pangya.gametree.co.kr:50006/UCC/upload_one.asp");
	strcpy(m_download[RESOURCE_UCC_TEMP_CLOTHES].url,
		m_download[RESOURCE_UCC_CLOTHES].url);
	strcpy(m_uploadAddress[RESOURCE_UCC_TEMP_CLOTHES],
		m_uploadAddress[RESOURCE_UCC_CLOTHES]);
	strcpy(m_uploadAddress[RESOURCE_BUG_REPORT],
		"http://qagame.pangya.ntreev.com:50000/post/BugReport.asp");
	m_downloadList.clear();
	m_bRunning = true;
	m_bClearGarbage = false;
	InitializeCriticalSection(&m_csDownload);
	InitializeCriticalSection(&m_csUpload);
	unsigned int downloadId, uploadId;
	m_hDownloadThread = (HANDLE)_beginthreadex(NULL, 0,
		CreateDownloadThreadFunc, this, 0, &downloadId);
	m_hUploadThread = (HANDLE)_beginthreadex(NULL, 0, CreateUploadThreadFunc,
		this, 0, &uploadId);
	if (m_hDownloadThread == (HANDLE)-1 || m_hUploadThread == (HANDLE)-1)
		return;
	memset(m_rootDir, 0, sizeof(m_rootDir));
	strcpy(m_rootDir, g_executeDirectory);
	strcat(m_rootDir, "\\");
	char path[1024];
	memset(path, 0, sizeof(path));
	sprintf(path, "%s\\%s", m_rootDir, m_download[RESOURCE_GUILD_EMBLEM].dir);
	G_CreateDirectory(path);
}

void NetResourceManager::Close()
{
	m_bRunning = false;
	WaitForSingleObject(m_hDownloadThread, 3000);
	CloseHandle(m_hDownloadThread);
	WaitForSingleObject(m_hUploadThread, 3000);
	CloseHandle(m_hUploadThread);
	EnterCriticalSection(&m_csDownload);
	m_downloadList.clear();
	LeaveCriticalSection(&m_csDownload);
	EnterCriticalSection(&m_csUpload);
	m_uploadList.clear();
	LeaveCriticalSection(&m_csUpload);
	for (std::map<std::string, Bitmap*>::iterator it = m_emblem.begin();
		it != m_emblem.end(); it++)
		delete it->second;
	m_emblem.clear();
	DeleteCriticalSection(&m_csDownload);
	DeleteCriticalSection(&m_csUpload);
}

int NetResourceManager::WorkDownloadThread()
{
	while (m_bRunning)
	{
		if (m_bClearGarbage)
		{
			m_bClearGarbage = false;
			ClearGarbage();
		}
		EnterCriticalSection(&m_csDownload);
		if (m_downloadList.empty())
		{
			LeaveCriticalSection(&m_csDownload);
			Sleep(1000);
			continue;
		}
		sResource resource = m_downloadList.front();
		m_downloadList.pop_front();
		LeaveCriticalSection(&m_csDownload);
		bool download = true;
		switch (resource.type)
		{
		case RESOURCE_GUILD_EMBLEM:
		{
			char path[256];
			sprintf(path, "%s%s\\%s", m_rootDir, m_download[resource.type].dir,
				resource.filename.c_str());
			WIN32_FIND_DATA data;
			HANDLE file = FindFirstFile(path, &data);
			if (file != INVALID_HANDLE_VALUE)
			{
				FindClose(file);
				LoadBitmapA(resource);
				download = false;
			}
			break;
		}
		}
		if (!download || !_RealDownLoad(resource))
			continue;
		switch (resource.type)
		{
		case RESOURCE_UCC_CLOTHES:
		case RESOURCE_UCC_AZTEC:
			if (UccManager()->SetDownloadComplete(resource.id, resource.key))
				UccManager()->AddRefreshItem(resource.id, resource.key);
			break;
		case RESOURCE_GUILD_EMBLEM:
			LoadBitmapA(resource);
			break;
		}
	}
	return 0;
}

int NetResourceManager::WorkUploadThread()
{
	while (m_bRunning)
	{
		EnterCriticalSection(&m_csUpload);
		if (m_uploadList.empty())
		{
			LeaveCriticalSection(&m_csUpload);
			Sleep(1000);
			continue;
		}
		sResource resource = m_uploadList.front();
		m_uploadList.pop_front();
		LeaveCriticalSection(&m_csUpload);
		GenericHTTPClient client;
		std::string filename;
		if (resource.type != RESOURCE_GUILD_EMBLEM)
		{
			char current[1024] = { 0 };
			GetCurrentDirectory(1024, current);
			filename = current;
			filename += "\\";
			filename += resource.filename;
		}
		else
			filename = resource.filename;
		client.InitilizePostArguments();
		if (resource.type == RESOURCE_UCC_CLOTHES ||
			resource.type == RESOURCE_UCC_TEMP_CLOTHES)
		{
			sItemInfo* item = GetMyItemInfo(resource.id, resource.key, 0);
			if (!item)
			{
				if (AfxGetTask()->GetActor("RealMyRoom"))
					AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 35,
						(int)MakeStr(
							"\xbe\xf7\xb7\xce\xb5\xe5\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.(\xc4\xda\xb5\xe5:%d)",
							6),
						0, 0, 0, 0);
				continue;
			}
			std::string key = SecurityKeyManager()->GetKey(SK_UCC, item->guid);
			client.AddPostArguments("ucctype", "clothes", 0);
			client.AddPostArguments("uccfile", filename.c_str(), 1);
			client.AddPostArguments("userid", Doc()->m_myInfo.info.sID, 0);
			client.AddPostArguments("type", 1UL);
			client.AddPostArguments("item_id", item->guid);
			client.AddPostArguments("key", key.c_str(), 0);
			unsigned long uid = Doc()->m_myInfo.info.dwUID;
			client.AddPostArguments("uid", uid);
		}
		else if (resource.type == RESOURCE_GUILD_EMBLEM)
		{
			IActor* actor = AfxGetTask()->GetActor(s_guildActor[1]);
			if (actor)
			{
				unsigned long* guild = NULL;
				actor << MsgObject(NULL, 611, 33, (int)&guild, 0, 0, 0);
				client.AddPostArguments("EMBLEM_IDX", resource.id);
				client.AddPostArguments("GUILD_IDX", *guild);
				unsigned long uid = Doc()->m_myInfo.info.dwUID;
				client.AddPostArguments("UID", uid);
				client.AddPostArguments("EMBLEM", resource.arg, 0);
				client.AddPostArguments("FILENAME", resource.filename.c_str(),
					1);
			}
		}
		else if (resource.type == RESOURCE_BUG_REPORT)
		{
			if (m_postArgument.find(resource.arg) != m_postArgument.end())
			{
				const std::list<sPostArgument>& args =
					m_postArgument[resource.arg];
				for (std::list<sPostArgument>::const_iterator it = args.begin();
					it != args.end(); ++it)
					client.AddPostArguments(it->name.c_str(), it->value.c_str(),
						it->bBinary);
				m_postArgument.erase(resource.arg);
			}
		}
		else
		{
			client.AddPostArguments("arg1", resource.arg, 0);
			client.AddPostArguments("arg2", filename.c_str(), 1);
		}
		int result =
			client.Request(NetResManager()->GetUploadAddress(resource.type), 3,
				"MERONG(0.9/;p)");
		const char* response = client.QueryHTTPResponse();
		if (result && strstr(response, "PANGYA_UPDATE_OK") == response)
		{
			switch (resource.type)
			{
			case RESOURCE_UCC_CLOTHES:
			case RESOURCE_UCC_AZTEC:
			case RESOURCE_GUILD_EMBLEM:
			case RESOURCE_UCC_TEMP_CLOTHES:
				if (SetUploadStatus(resource.id, resource.key, resource.type))
				{
					EnterCriticalSection(&m_csUpload);
					std::list<sUploadStatus>::iterator it, end;
					it = m_uploadStatus.begin();
					end = m_uploadStatus.end();
					for (; it != end; it++)
						if ((*it).type == resource.type &&
							(*it).id == resource.id &&
							!strcmp((*it).key, resource.key))
							break;
					if (it != m_uploadStatus.end())
						m_uploadStatus.erase(it);
					LeaveCriticalSection(&m_csUpload);
					switch (resource.type)
					{
					case RESOURCE_GUILD_EMBLEM:
					{
						IActor* actor = AfxGetTask()->GetActor(s_guildActor[1]);
						if (actor)
							actor << MsgObject(NULL, 611, 24, 0, 0, 0, 0);
						WSendPacket packet((enumClientPacket)275);
						packet.Send(TO_GAME);
						AfxGetTask()->SendMsgToMainActor(
							MsgObject(NULL, 1, 0, 0, 0, 0, 0));
						break;
					}
					case RESOURCE_UCC_CLOTHES:
					{
						WSendPacket packet((enumClientPacket)177);
						packet.Encode1(0);
						packet.Encode4(resource.id);
						packet.EncodeStr(resource.key);
						packet.EncodeStr(resource.arg);
						packet.Send(TO_GAME);
						break;
					}
					case RESOURCE_UCC_TEMP_CLOTHES:
					{
						WSendPacket packet((enumClientPacket)177);
						packet.Encode1(3);
						packet.Encode4(resource.id);
						packet.EncodeStr(resource.key);
						packet.Send(TO_GAME);
						break;
					}
					}
				}
				break;
			}
		}
		else
		{
			switch (resource.type)
			{
			case RESOURCE_UCC_CLOTHES:
			case RESOURCE_UCC_AZTEC:
			case RESOURCE_UCC_TEMP_CLOTHES:
			{
				char error[1024] = { 0 };
				strncpy(error, response, 1023);
				if (AfxGetTask()->GetActor("RealMyRoom"))
				{
					AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 531,
						(int)&resource.id, (int)resource.key, (int)error, 0, 0);
					EnterCriticalSection(&m_csUpload);
					for (std::list<sResource>::iterator it =
							 m_uploadList.begin();
						it != m_uploadList.end();)
					{
						if (resource.id == (*it).id &&
							!strcmp(resource.key, (*it).key))
						{
							DeleteFile((*it).filename.c_str());
							it = m_uploadList.erase(it);
						}
						else
							it++;
					}
					LeaveCriticalSection(&m_csUpload);
				}
				break;
			}
			case RESOURCE_GUILD_EMBLEM:
			{
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 2, 0, 0, 0, 0, 0));
				IActor* actor = AfxGetTask()->GetActor(s_guildActor[1]);
				if (actor)
				{
					if (strstr(response, "PANGYA_UPDATE_BLOCK"))
						actor << MsgObject(NULL, 611, 26, 1055, 0, 0, 0);
					else
						actor << MsgObject(NULL, 611, 26, 1054, 0, 0, 0);
				}
				break;
			}
			}
		}
		DeleteFile(resource.filename.c_str());
	}
	return 0;
}

bool NetResourceManager::_RealDownLoad(const sResource& resource)
{
	bool result = false;
	cHttp http;
	switch (resource.type)
	{
	case RESOURCE_GHOST:
	{
		std::string url = m_download[RESOURCE_GHOST].url;
		url += resource.arg[0];
		url += "/";
		url += resource.arg;
		url += "/";
		CreateDirectory(m_download[resource.type].dir, NULL);
		unsigned long size = 0;
		unsigned char* data = http.DownLoad<unsigned char>(
			resource.filename.c_str(), url.c_str(), size);
		if (data)
		{
			delete[] data;
			result = true;
		}
		break;
	}
	case RESOURCE_GUILD_EMBLEM:
	{
		char filename[256];
		memset(filename, 0, sizeof(filename));
		sprintf(filename, "%s\\%s", m_download[resource.type].dir,
			resource.filename.c_str());
		if (http.DownLoad(filename, resource.filename.c_str(), m_rootDir,
				m_download[resource.type].url))
			result = true;
		break;
	}
	case RESOURCE_UCC_CLOTHES:
	case RESOURCE_UCC_AZTEC:
	{
		std::string url = m_download[resource.type].url;
		url += resource.key[0];
		url += "/";
		unsigned long size = 0;
		unsigned char* data = http.DownLoad<unsigned char>(
			resource.filename.c_str(), url.c_str(), size);
		int width = 0, height = 0;
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(resource.id);
		if (part)
		{
			Bitmap* bitmap = g_resrcmng->LoadBitmap(part->Tex[0], 0, 0);
			if (bitmap)
			{
				width = bitmap->Width();
				height = bitmap->Height();
				delete bitmap;
			}
		}
		Bitmap front, back;
		front.Create(width, height, 24);
		back.Create(width, height, 24);
		Bitmap* icon = new Bitmap(64, 84, 32);
		sUccHeader header;
		header.Initialize();
		if (!UnzipUccClothes(data, size, &front, &back, icon, &header))
		{
			delete icon;
			delete[] data;
			return false;
		}
		delete[] data;
		if (header.flags & 1)
			LoadWarningClothes(resource.id, &front, &back, icon);
		std::string frontName =
			GenerateTextureName_F(resource.id, resource.key);
		std::string backName = GenerateTextureName_B(resource.id, resource.key);
		g_resrcmng->UploadTexture(frontName.c_str(), &front, 0, 0);
		g_resrcmng->UploadTexture(backName.c_str(), &back, 0, 0);
		std::string iconName = GenerateIconFilename(resource.id, resource.key);
		UCC_ICON_IT it = UccManager()->m_icon.find(iconName);
		if (it == UccManager()->m_icon.end())
		{
			bool addIcon = false;
			if (resource.type == RESOURCE_UCC_CLOTHES)
			{
				sUccClothes* item =
					UccManager()->FindClothes(resource.id, resource.key);
				if (item)
				{
					item->icon = icon;
					addIcon = true;
				}
				else
				{
					ClearTexCache(icon);
					delete icon;
				}
			}
			else
			{
				sUccAztec* item =
					UccManager()->FindAztec(resource.id, resource.key);
				if (item)
				{
					item->icon = icon;
					addIcon = true;
				}
				else
				{
					ClearTexCache(icon);
					delete icon;
				}
			}
			if (!addIcon)
				goto iconComplete;
			UccManager()->m_icon.insert(
				std::pair<const std::string, Bitmap*>(iconName, icon));
		}
		else
			delete icon;
iconComplete:
		result = true;
		break;
	}
	case RESOURCE_UCC_TEMP_CLOTHES:
	{
		std::string url = m_download[RESOURCE_UCC_TEMP_CLOTHES].url;
		url += resource.key[0];
		url += "/";
		unsigned long size = 0;
		unsigned char* data = http.DownLoad<unsigned char>(
			resource.filename.c_str(), url.c_str(), size);
		int width = 0, height = 0;
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(resource.id);
		if (part)
		{
			Bitmap* bitmap = g_resrcmng->LoadBitmap(part->Tex[0], 0, 0);
			if (bitmap)
			{
				width = bitmap->Width();
				height = bitmap->Height();
				delete bitmap;
			}
		}
		Bitmap front, back;
		front.Create(width, height, 32);
		back.Create(width, height, 32);
		if (!UnzipUccClothes(data, size, &front, &back, NULL, NULL))
		{
			delete[] data;
			return false;
		}
		delete[] data;
		UccManager()->SetUccTempClothes(resource.id, resource.key, &front,
			&back);
		result = true;
		break;
	}
	}
	return result;
}

bool NetResourceManager::LoadBitmapA(const sResource& resource)
{
	EnterCriticalSection(&m_csDownload);
	switch (resource.type)
	{
	case RESOURCE_GUILD_EMBLEM:
	{
		char filename[256] = { 0 };
		sprintf(filename, "%s%s\\%s", m_rootDir, m_download[resource.type].dir,
			resource.filename.c_str());
		Bitmap* bitmap = g_resrcmng->LoadBitmap(filename, 0, 1);
		if (!bitmap)
		{
			DeleteFile(filename);
			LeaveCriticalSection(&m_csDownload);
			return false;
		}
		m_emblem[resource.filename] = bitmap;
		break;
	}
	}
	LeaveCriticalSection(&m_csDownload);
	return true;
}

const Bitmap* NetResourceManager::AddToDownloadList(const sResource& resource,
	int load, bool front)
{
	const Bitmap* bitmap = NULL;
	EnterCriticalSection(&m_csDownload);
	switch (resource.type)
	{
	case RESOURCE_GUILD_EMBLEM:
		if (!load)
		{
			LeaveCriticalSection(&m_csDownload);
			return NULL;
		}
		{
			std::map<std::string, Bitmap*>::iterator it;
			it = m_emblem.find(resource.filename);
			if (it == m_emblem.end() || !(*it).second)
			{
				if (!IsDownloadRequested(resource))
				{
					if (front)
						m_downloadList.push_front(resource);
					else
						m_downloadList.push_back(resource);
					m_emblem[resource.filename] = NULL;
				}
				bitmap = NULL;
			}
			else
				bitmap = (*it).second;
		}
		break;
	case RESOURCE_UCC_CLOTHES:
	case RESOURCE_UCC_AZTEC:
	case RESOURCE_UCC_TEMP_CLOTHES:
		if (!IsDownloadRequested(resource))
		{
			if (front)
				m_downloadList.push_front(resource);
			else
				m_downloadList.push_back(resource);
		}
		break;
	default:
		if (front)
			m_downloadList.push_front(resource);
		else
			m_downloadList.push_back(resource);
		break;
	}
	LeaveCriticalSection(&m_csDownload);
	return bitmap;
}

void NetResourceManager::AddToUploadList(const sResource& resource)
{
	if (!IsUploadRequested(resource))
	{
		EnterCriticalSection(&m_csUpload);
		m_uploadList.push_back(resource);
		LeaveCriticalSection(&m_csUpload);
	}
}

const Bitmap* NetResourceManager::GetEmblemByName(const char* name)
{
	if (!strlen(name))
		return NULL;
	char base[13];
	memset(base, 0, sizeof(base));
	strncpy(base, name, 12);
	sResource resource;
	resource.filename = MakeStr("%s.png", base);
	resource.type = RESOURCE_GUILD_EMBLEM;
	return AddToDownloadList(resource, 1, false);
}

void NetResourceManager::DownloadUccClothes(unsigned long id, const char* key,
	unsigned short flag, bool front)
{
	sResource resource;
	resource.filename = GenerateTextureName_F(id, key);
	resource.type = RESOURCE_UCC_CLOTHES;
	resource.id = id;
	resource.flag = flag;
	strncpy(resource.key, key, 9);
	AddToDownloadList(resource, 0, false);
}

void NetResourceManager::DownloadUccTempClothes(unsigned long id,
	const char* key, unsigned short flag, bool front)
{
	sResource resource;
	resource.filename = GenerateTextureName_F(id, key);
	resource.type = RESOURCE_UCC_TEMP_CLOTHES;
	resource.id = id;
	resource.flag = flag;
	strncpy(resource.key, key, 9);
	AddToDownloadList(resource, 0, false);
}

void NetResourceManager::Download(const sResource& resource, const char* url)
{
	if (url)
	{
		m_download[resource.type].reserved = 0;
		strcpy(m_download[resource.type].dir, "./ntsaved/");
		strcpy(m_download[resource.type].url, url);
	}
	AddToDownloadList(resource, 0, false);
}

bool NetResourceManager::IsDownloadRequested(const sResource& resource)
{
	for (std::list<sResource>::iterator it = m_downloadList.begin();
		it != m_downloadList.end(); it++)
	{
		if ((*it).type == resource.type && (*it).filename == resource.filename)
			return true;
	}
	return false;
}

bool NetResourceManager::IsDownloadRequested(eResourceType type,
	unsigned long id, const char* key)
{
	sResource resource;
	resource.type = type;
	resource.id = id;
	strncpy(resource.key, key, 9);
	return IsDownloadRequested(resource);
}

bool NetResourceManager::IsUploadRequested(const sResource& resource)
{
	for (std::list<sResource>::iterator it = m_uploadList.begin();
		it != m_uploadList.end(); it++)
	{
		if ((*it).type == resource.type && (*it).filename == resource.filename)
			return true;
	}
	return false;
}

void NetResourceManager::UploadUccClothes(unsigned long id, const char* key,
	const char* arg)
{
	sResource resource;
	resource.filename = GenerateTextureFullPath(0, id, key);
	resource.type = RESOURCE_UCC_CLOTHES;
	resource.id = id;
	strcpy(resource.arg, arg);
	strncpy(resource.key, key, 9);
	AddToUploadList(resource);
}

void NetResourceManager::UploadUccTempClothes(unsigned long id, const char* key)
{
	sResource resource;
	resource.filename = GenerateTextureFullPath(0, id, key);
	resource.type = RESOURCE_UCC_TEMP_CLOTHES;
	resource.id = id;
	strncpy(resource.key, key, 9);
	AddToUploadList(resource);
}

bool NetResourceManager::SetUploadStatus(unsigned long id, const char* key,
	eResourceType type)
{
	int count = 0;
	bool found = false;
	EnterCriticalSection(&m_csUpload);
	std::list<sUploadStatus>::iterator it, end;
	it = m_uploadStatus.begin();
	end = m_uploadStatus.end();
	for (; it != end; it++)
		if ((*it).id == id && !strcmp((*it).key, key) && (*it).type == type)
		{
			found = true;
			break;
		}
	if (!found)
	{
		sUploadStatus status;
		status.type = type;
		status.count = 1;
		status.id = id;
		strncpy(status.key, key, 9);
		m_uploadStatus.push_back(status);
		count = 1;
	}
	else
		count = ++(*it).count;
	LeaveCriticalSection(&m_csUpload);
	if (!(type == RESOURCE_UCC_CLOTHES || type == RESOURCE_UCC_TEMP_CLOTHES ||
			type == RESOURCE_UCC_AZTEC || type == RESOURCE_GUILD_EMBLEM) ||
		count == 1)
		return true;
	return false;
}

void NetResourceManager::ClearGarbage()
{
	std::list<sResource> resources;
	if (IsLocalContent((localContentType_t)95))
	{
		for (std::map<unsigned long, sRoomSlot>::iterator it =
				 Doc()->m_roomSlotMap.begin();
			it != Doc()->m_roomSlotMap.end(); it++)
		{
			sRoomSlot* character = &(*it).second;
			if (character)
				for (int i = 0; i < 24; i++)
					if (IsUccClothes(character->partTidList.m_tid[i]))
					{
						sItemInfo* item =
							GetMyItemInfo(character->partTidList.m_uid[i]);
						if (item && strlen(item->UccIndex))
						{
							sResource resource;
							resource.id = character->partTidList.m_tid[i];
							strncpy(resource.key, item->UccIndex, 9);
							resources.push_back(resource);
						}
					}
		}
	}
	EnterCriticalSection(&m_csDownload);
	for (std::list<sResource>::iterator it = m_downloadList.begin();
		it != m_downloadList.end();)
	{
		bool keep = false;
		for (std::list<sResource>::iterator j = resources.begin();
			j != resources.end(); j++)
			if ((*it).id == (*j).id && !strcmp((*it).key, (*j).key))
			{
				keep = true;
				break;
			}
		if (!keep)
			it = m_downloadList.erase(it);
		else
			it++;
	}
	LeaveCriticalSection(&m_csDownload);
}

bool NetResourceManager::IsFailedToUpload(unsigned long id, const char* key,
	eResourceType type)
{
	bool missing = false, unqueued = true;
	if (type == RESOURCE_UCC_CLOTHES)
		missing = UccManager()->FindClothes(id, key) == NULL;
	else if (type == RESOURCE_UCC_AZTEC)
		missing = UccManager()->FindAztec(id, key) == NULL;
	else if (type == RESOURCE_UCC_TEMP_CLOTHES)
	{
		Bitmap front, back;
		missing = !UccManager()->GetUccTempClothes(id, key, front, back);
	}
	EnterCriticalSection(&m_csUpload);
	std::list<sResource>::iterator it, end;
	it = m_uploadList.begin();
	end = m_uploadList.end();
	for (; it != end; it++)
		if ((*it).type == type && (*it).id == id && !strcmp((*it).key, key))
			unqueued = false;
	LeaveCriticalSection(&m_csUpload);
	return missing && unqueued;
}

bool NetResourceManager::Upload(const sResource& resource, const char* url)
{
	if (resource.type < RESOURCE_NUM)
	{
		if (url && strlen(url) < 1024)
			strcpy(m_uploadAddress[resource.type], url);
		AddToUploadList(resource);
		return true;
	}
	return false;
}

void NetResourceManager::RequestGuildRSS(unsigned long id,
	void(__fastcall* callback)(int))
{
	m_rss.GetRSSFile(id, callback);
}

void NetResourceManager::RequestGuildRSS_JP(unsigned long id,
	void(__fastcall* callback)(int), const char* arg1, const char* arg2)
{
	m_rss.GetRSSFile_JP(id, callback, arg1, arg2);
}

void NetResourceManager::UploadBugReportData(const char* filename, int mainType,
	int subType, bool attach)
{
	sPostArgument arg;
	std::list<sPostArgument> args;
	char buffer[32];
	arg.bBinary = true;
	arg.name = "bugReportFile";
	arg.value = filename;
	args.push_back(arg);
	arg.bBinary = false;
	arg.name = "bugReportMainType";
	sprintf(buffer, "%d", mainType);
	arg.value = buffer;
	args.push_back(arg);
	arg.bBinary = false;
	arg.name = "bugReportSubType";
	sprintf(buffer, "%d", mainType);
	arg.value = buffer;
	args.push_back(arg);
	arg.bBinary = false;
	arg.name = "uid";
	unsigned long uid = Doc()->m_myInfo.info.dwUID;
	sprintf(buffer, "%d", uid);
	arg.value = buffer;
	args.push_back(arg);
	arg.bBinary = false;
	arg.name = "userID";
	arg.value = Doc()->m_myInfo.info.sID;
	args.push_back(arg);
	arg.bBinary = false;
	arg.name = "packetVer";
	sprintf(buffer, "%d", 2011100699);
	arg.value = buffer;
	args.push_back(arg);
	arg.bBinary = false;
	arg.name = "clientVer";
	arg.value = "645.00";
	args.push_back(arg);
	m_postArgument[filename] = args;
	sResource resource;
	resource.type = RESOURCE_BUG_REPORT;
	strcpy(resource.arg, filename);
	if (attach)
		resource.filename = filename;
	Upload(resource,
		"http://qagame.pangya.ntreev.com:50000/post/BugReport.asp");
}
