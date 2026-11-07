#pragma once
#include <vector>
#include <map>
#include <string>
#include "wtypes.h"

class WOverlay;
class FrWndManager;

class FrEmoticon
{
public:
	FrEmoticon(FrWndManager* pManager);
	virtual ~FrEmoticon();
	bool Init();
	bool Draw(int icon, const WRect& dst, unsigned long diffuse, bool flip);
	bool Draw(int icon, float x, float y, unsigned long diffuse, bool flip);
	float PrintText(const WPoint& point, unsigned long style, const char* text,
		unsigned long color);
	float PrintText11(const WPoint& pos, unsigned long align, const char* text,
		unsigned long emoDiffuse);
	float GetTextWidth(const char* text);
	float GetTextWidth11(const char* text);
	int GetIconIndex(const char* key);
	const char* GetIconName(int index);
	int GetIconNum();
	void GetAlias(std::string (*buffer)[2], int index);
	int GetWidth() { return m_selWidth; }
	int GetHeight() { return m_selHeight; }
	void SetClippingArea(WRect* clip);
	void SetAnim(bool enable) { m_EnableAnim = enable; }
	void SetAnimTime(unsigned long time) { m_AnimStart = time; }

protected:
	struct sEmoticon
	{
		std::string Name;
		std::string Alias[2];
		WOverlay* Overlay;
		std::vector<int> Frames;
	};
	typedef std::vector<sEmoticon*> EMOTICONLIST;
	typedef EMOTICONLIST::iterator EMOTICONLIST_itr;
	typedef std::map<std::string, WOverlay*> RESMAP;
	typedef RESMAP::iterator RESMAP_itr;
	int m_Fps;
	EMOTICONLIST m_Emoticons;
	RESMAP m_ResMap;
	void ParseAnimFrame(const std::string& Frames, std::vector<int>& Vector);
	bool LoadXml(const char* FileName);
	bool m_EnableAnim;
	int m_AnimStart;
	WOverlay* m_overlay;
	int m_selWidth;
	int m_selHeight;
	int m_selXNum;
	int m_selYNum;
	int m_selNum;
	FrWndManager* m_pManager;
};
