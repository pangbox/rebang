#pragma once
#include <windows.h>
#include "wtypes.h"
#include "cfile.h"
#include "wlist.h"
#include "wlock.h"
#include "wresource.h"
#include "wresrccache.h"

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
class W3dSpr;
class W3dAniSpr;
class WxStaticPuppetGrp;
struct WTVertex;

enum rmlType_t
{
	RML_TEXTURE_LOADING = 0,
	RML_BITMAP_LOADING = 1,
	RML_PUPPET_LOADING = 2
};

struct w_texlist
{
	int width;
	int height;
	int count;
	int texhandle;
	unsigned long size;
	char texname[1];
};

void __cdecl EnumFileList(WList<char*>* list);

class WResourceManager
{
public:
	struct w_match_filename
	{
		char fullname[1];
	};

	WResourceManager();
	~WResourceManager();

	void SetVideoReference(WVideoDev* dev) { video = dev; }
	void SetAudioReference(WAudioDev* dev) { audio = dev; }
	WVideoDev* VideoReference() const { return video; }
	WAudioDev* AudioReference() const { return audio; }

	void Lock(rmlType_t type);
	void Unlock(rmlType_t type);
	void UnlockAllByThread(unsigned long threadID);
	void SetUseLessVideoRam(int level);
	WOverlay* GetOverlay(const char* filename, unsigned long flag);
	WOverlay* GetOverlay(const char* name, Bitmap* bitmap, unsigned long flag);
	WFont* GetTexFont(const char* filename);
	WFont* GetTrueFont(const char* filename, int fontsize, int fontindex);
	WFont* GetFntFont(const char* filename);
	WFont* GetFixedFont(const char* filename);
	WTitleFont* GetTitleFont();
	WView* GetView(const char* type);
	WPuppet* GetPuppet(const char* f, bool alphatest, bool bNeedTnLBuf,
		bool bForceLegacy);
	WxStaticPuppetGrp* xGetStaticPuppetGrp(int nSPets, const char* filename,
		WPuppet** ppaSPets);
	W3dSpr* Get3DSpr(const char* filename, int type);
	W3dAniSpr* Get3DAniSpr(const char* filename, int type, float fSprSizeX,
		float fSprSizeY);
	void Release(WResource* resrc);
	void Release(WPuppet* puppet, bool bForceDeleteOrigin);
	void Release(int texhandle);
	void Release(WFont* font);
	void Release(WTitleFont* font);
	void ReleaseAllPets();
	void ReleaseIdleResources(int cnt);
	const WList<w_texlist*>& GetTextureList() const { return texList; }
	w_texlist* FindTexture(int texHandle);
	w_texlist* FindTexture(const char* texName);
	int GetTextureWidth(int texHandle);
	int GetTextureHeight(int texHandle);
	unsigned long GetTextureUsedBytes() const { return m_nByteUsedTexture; }
	int UploadTexture(const char* texName, Bitmap* bitmap, unsigned long type,
		RECT* rect);
	void FixTexture(int texHandle, Bitmap* bitmap, unsigned long type,
		RECT* rect);
	bool CheckTexture(const char* name)
	{
		return FindTexture(name) ? true : false;
	}
	int LoadTexture(const char* filename, unsigned long type, int level,
		const char* texname);
	int LoadTexture(Bitmap* bitmap, const char* filename, int type, int level);
	int LoadTexture_DDS(unsigned char* dds, unsigned int size,
		const char* filename, int type, int level);
	bool ChangeTexturePart(char* dstName, char* srcName, Bitmap** ppBmp,
		RECT* pRc);
	void ChangeTexturePart(char* dstName, Bitmap* pBmp, const RECT& rc);
	bool ChangeTexturePart(int hDstTex, char* srcName, Bitmap** ppBmp,
		RECT* pRc);
	void ChangeTexturePart(int hDstTex, Bitmap* pBmp, const RECT& rc);
	int GetBlankTexture();
	void ReloadTextures();
	bool CheckMaskTexture(Bitmap** ppBitmap, const char* filename);
	WList<char*>& MissingTextureList() { return m_missingTextureList; }
	Bitmap* LoadBitmap(const char* filename, int level, bool bNet);
	Bitmap* LoadBitmapPart(char* dstName, char* srcName);
	Bitmap* LoadBMP(const char* filename);
	Bitmap* LoadJPG(const char* filename);
	Bitmap* LoadTGA(const char* filename);
	Bitmap* LoadPNG(const char* filename, bool bFromMem, bool bNet);
	Bitmap* LoadBRES(const char* zipname, const char* filename);
	Bitmap* MakeQuarterBitmap(Bitmap* bitmap);
	Bitmap* Combine4Alpha(Bitmap* bitmap, Bitmap* alpha);
	Bitmap* Combine(Bitmap* dest, Bitmap* src);
	bool IsOrigin(WResource* resrc);
	cFile* GetCFile(const char* filename, int len);
	bool FindFileInZip(const char* zipname, const char* filename);
	void SetAutoMatchDirectory(const char* directory);
	const char* FindMatchFile(const char* filename);

protected:
	WVideoDev* video;
	WAudioDev* audio;

private:
	w_texlist* AddTexture(const char* texturename, int texhandle, int width,
		int height);
	void AddMissingTexture(const char* filename);
	bool GetRectInSourceTexture(const BITMAPINFO* src, const void* data,
		RECT& rc) const;
	WFont* GetFntClone(const char* filename);
	void AddMatchList(const char* filename);
	void UpdateMatchList(const char* directory);
	void StrcpyLower(char* out, const char* src);
	void LoadMatchList(const char* directory);
	void SaveMatchCacheList(const char* filename);
	void LoadMatchCacheList(const char* filename);
	void ReleaseMatchList();
	void AddOriginList(WResource* resrc);

	unsigned long m_nByteUsedTexture;
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

	friend class WView;
	friend class WOverlay;
	friend class WxPuppet;
	friend class WxStaticPuppetGrp;
};

extern WResourceManager* g_resrcmng;
