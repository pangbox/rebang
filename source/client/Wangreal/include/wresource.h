#pragma once
#include <windows.h>
#include "wtypes.h"
#include "cfile.h"
#include "wlist.h"
#include "wlock.h"
#include "baseobject.h"

class WAudioDev;
class WVideoDev;
class WResource;
class WTitleFont;
class WOverlay;
class WPuppet;
class WSoundFx;
class WFont;

enum rmlType_t
{
	PG0551_RML0 = 0,
	PG0551_RML1 = 1,
	PG0551_RML2 = 2
};

struct w_texlist
{
	int width;
	int height;
	int count;
	int texhandle;
	unsigned int size;
	char texname[1];
};

template <typename T>
class WResrcCache
{
	struct w_cache;
	struct w_origin;
	WList<w_cache*> resrcList;
	WList<w_origin*> originList;
};

class WResourceManager
{
public:
	struct w_match_filename
	{
		char fullname[1];
	};

	WPuppet* GetPuppet(const char*, bool, bool, bool);
	WOverlay* GetOverlay(const char*, unsigned long);
	int LoadTexture(const char*, unsigned long, int, const char*);
	int UploadTexture(const char* name, class Bitmap* bitmap,
		unsigned long style, RECT* rect);
	void Lock(rmlType_t type);
	void Unlock(rmlType_t type);
	void UnlockAllByThread(ulong thread);
	void SetUseLessVideoRam(int mode);
	void Release(int handle);
	void Release(WResource* resource);
	void Release(WTitleFont* font);
	w_texlist* FindTexture(int handle);
	int GetTextureWidth(int handle);
	int GetTextureHeight(int handle);
	cFile* GetCFile(const char* filename, int len);

	WVideoDev* video;
	WAudioDev* audio;
	unsigned int m_nByteUsedTexture;
	int m_blankTexture;
	bool m_savemem[3];
	bool m_matchDirectory;
	WList<char*> m_missingTextureList;
	WList<w_match_filename*> m_matchList;
	WList<w_texlist*> texList;
	WResrcCache<WPuppet*> puppetList;
	WResrcCache<WSoundFx*> soundList;
	WResrcCache<WFont*> fontList;
	WList<WSoundFx*> voiceList;
	WList<WResource*> originList;
	WLock m_lock[3];
};

extern WResourceManager* g_resrcmng;

class WResource : public BaseObject
{
public:
	__forceinline WResource()
		: m_resrcMng(0)
	{
	}

	__forceinline virtual ~WResource() { }

protected:
	__forceinline WResourceManager* GetResrcManager() const
	{
		return m_resrcMng;
	}

private:
	WResourceManager* m_resrcMng;
};
