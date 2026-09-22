#pragma once
#include <windows.h>
#include "wtypes.h"
#include "cfile.h"
#include "wlist.h"
#include "wlock.h"
#include "wresource.h"

class Bitmap;
class WVideoDev;
class WAudioDev;
class WFont;
class WOverlay;
class WProc;
class WPuppet;
class WResource;
class WResourceManager;
class WSoundFx;
class WTitleFont;
class WView;
struct WTVertex;

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

class WInstanceLock
{
public:
	explicit WInstanceLock(WLock* lock);
	~WInstanceLock(void);

private:
	WLock* m_lock;
};

void __cdecl EnumFileList(WList<char*>* list);

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
	WResourceManager(void);
	~WResourceManager(void);
	struct w_match_filename
	{
		char fullname[1];
	};

	void Lock(rmlType_t type);
	void Unlock(rmlType_t type);
	void UnlockAllByThread(ulong thread);
	void SetUseLessVideoRam(int mode);
	void Release(WResource* resource);
	void Release(WTitleFont* font);
	w_texlist* FindTexture(int handle);
	w_texlist* FindTexture(const char* name);
	bool CheckTexture(const char* name)
	{
		return FindTexture(name) ? true : false;
	}
	const char* FindMatchFile(const char* name);
	cFile* GetCFile(const char* name, int mode);
	void SetAutoMatchDirectory(const char* directory);
	void ChangeTexturePart(int handle, Bitmap* bitmap, const tagRECT& rect);
	void ChangeTexturePart(char* name, Bitmap* bitmap, const tagRECT& rect);
	bool ChangeTexturePart(int handle, char* name, Bitmap** bitmap,
		tagRECT* rect);
	bool ChangeTexturePart(char* name, char* part, Bitmap** bitmap,
		tagRECT* rect);
	void Release(WFont* font);
	void Release(int handle);
	bool IsOrigin(WResource* resource);
	void FixTexture(int handle, Bitmap* bitmap, unsigned long flag,
		tagRECT* rect);
	int GetBlankTexture(void);
	int GetTextureWidth(int handle);
	int GetTextureHeight(int handle);
	WPuppet* GetPuppet(const char*, bool, bool, bool);
	WOverlay* GetOverlay(const char*, unsigned long);
	int LoadTexture(const char*, unsigned long, int, const char*);
	Bitmap* LoadBitmap(const char* name, int mode, bool flag);
	int UploadTexture(const char* name, class Bitmap* bitmap,
		unsigned long style, RECT* rect);

private:
	WFont* GetFntClone(const char* name);
	void StrcpyLower(char* destination, const char* source);
	void ReleaseMatchList(void);
	w_texlist* AddTexture(const char* name, int handle, int width, int height);
	void LoadMatchList(const char* directory);
	void AddMatchList(const char* path);
	void AddMissingTexture(const char* name);
	void AddOriginList(WResource* resource);

public:
	WVideoDev* m_video;
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
