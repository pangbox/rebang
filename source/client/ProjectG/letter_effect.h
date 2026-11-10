#pragma once

#include "actor.h"
#include "wresrcmng.h"
#include "wfont.h"
#include "wlist.h"

extern WResourceManager* g_resrcmng;

class CLetter : public IActor
{
protected:
	CLetter();

public:
	virtual ~CLetter() { }

	DECLARE_OBJECT(CLetter)

	virtual void OnPreLoadInit();
	virtual void OnLoad();
	virtual void OnInit();
	virtual void OnProcess(float delta);
	virtual void OnDisplay();
	virtual void OnDestroy();
	virtual void HandleMsg(const MsgObject& msg);

protected:
	void Print(float x, float y, const char* str, float gap, int align,
		float scale, unsigned long color);
	void SetPos(const char* name, float x, float y);
	void AddOverlay(const char* name, const char* filename, WRect* rect,
		bool bSkip);
	void DisplayReplay();

	struct sNumber
	{
		bool bShow;
		int number;
		float scale;
		unsigned long color;
	};

	struct sText
	{
		sText() { overlay = NULL; }

		~sText()
		{
			if (overlay)
			{
				g_resrcmng->Release(overlay);
				overlay = NULL;
			}
		}

		WOverlay* overlay;
		WRect rect;
		WRect uv;
		float time;
		char name[16];
		bool bSkip;
		sNumber number;
	};

	sText* m_pText;
	WList<sText*> m_textList;
	float m_alpha;
	int m_state;
	float m_fadeTime;
	bool m_bShow;
	int m_align;
	unsigned long m_reserved;
	tagWTITLEFONT m_fontInfo;
	WTitleFont* m_pFont;
	sText* m_pFastForward;
	sText* m_pReplay;
	float m_replayTime;
};
