#include "fremoticon.h"
#include "frwndmanager.h"
#include "rtti.h"
#include "frgraphicinterface.h"
#include "wresrcmng.h"
#include "tinyxml.h"
#include "../../../shared/token.h"
#include "wlocalize.h"
#include <mmsystem.h>

extern "C" __declspec(dllimport) int __cdecl strcmpi(const char*, const char*);

inline const char* cTokenV::GetBuf() const
{
	return m_pcBuf;
}

FrEmoticon::FrEmoticon(FrWndManager* pManager)
{
	m_pManager = pManager;
	m_overlay = 0;
	m_EnableAnim = false;
	m_AnimStart = 0;
	m_selWidth = 26;
	m_selHeight = 24;
	m_selXNum = 9;
	m_selYNum = 10;
	m_selNum = 90;
}

FrEmoticon::~FrEmoticon()
{
	EMOTICONLIST_itr e;
	for (e = m_Emoticons.begin(); e != m_Emoticons.end(); e++)
	{
		(*e)->Frames.clear();
		delete *e;
	}
	m_Emoticons.clear();
	RESMAP_itr i;
	for (i = m_ResMap.begin(); i != m_ResMap.end(); i++)
		if (g_resrcmng && (*i).second)
		{
			g_resrcmng->Release((*i).second);
			(*i).second = 0;
		}
	m_ResMap.clear();
	if (g_resrcmng && m_overlay)
	{
		g_resrcmng->Release(m_overlay);
		m_overlay = 0;
	}
}

bool FrEmoticon::Init()
{
	LoadXml("emoticon.xml");
	if (!m_overlay)
		m_overlay = g_resrcmng->GetOverlay("emoticon.tga", 0);
	return true;
}

bool FrEmoticon::Draw(int icon, float x, float y, unsigned long diffuse,
	bool flip)
{
	return Draw(icon, WRect(x, y, m_selWidth, m_selHeight), diffuse, flip);
}

void FrEmoticon::SetClippingArea(WRect* clip)
{
	m_pManager->GetGDI()->SetClippingArea(clip);
	m_overlay->SetClippingArea(clip);
	for (RESMAP_itr i = m_ResMap.begin(); i != m_ResMap.end(); i++)
		i->second->SetClippingArea(clip);
}

bool FrEmoticon::Draw(int icon, const WRect& dst, unsigned long diffuse,
	bool flip)
{
	if (icon < 0)
		return false;
	FrGraphicInterface* pGdi = m_pManager->GetGDI();
	if (!pGdi)
		return false;
	unsigned long frame = (timeGetTime() - m_AnimStart) / (1000 / m_Fps);
	sEmoticon* emo = m_Emoticons[icon];
	int selected;
	if (m_EnableAnim && emo->Frames.size())
		selected = emo->Frames[frame % emo->Frames.size()];
	else
		selected = icon;
	int selX = selected % m_selXNum;
	int selY = (selected % m_selNum) / m_selXNum;
	WOverlay* overlay =
		m_EnableAnim && emo->Frames.size() ? emo->Overlay : m_overlay;
	int width = overlay->GetWidth();
	int height = overlay->GetHeight();
	unsigned long color = (int(pGdi->GetAlpha() * 255) << 24) | 0xffffff;
	if (flip)
		overlay->Render(g_view,
			WRect(float((selX + 1) * m_selWidth) / width,
				float(selY * m_selHeight) / height, float(-m_selWidth) / width,
				float(m_selHeight) / height),
			dst, 0, color, 0, 0);
	else
		overlay->Render(g_view,
			WRect(float(selX * m_selWidth) / width,
				float(selY * m_selHeight) / height, float(m_selWidth) / width,
				float(m_selHeight) / height),
			dst, 0, color, 0, 0);
	return true;
}

float FrEmoticon::PrintText(const WPoint& pos, unsigned long align,
	const char* text, unsigned long emoDiffuse)
{
	FrGraphicInterface* pGdi = m_pManager->GetGDI();
	if (!pGdi)
		return 0.0f;
	pGdi->SetTextColor((pGdi->GetTextColor() & 0xffffff) |
			(emoDiffuse & 0xff000000),
		(pGdi->GetTextOutlineColor() & 0xffffff) | (emoDiffuse & 0xff000000));
	WPoint p;
	switch (align)
	{
	case 0:
		p.x = pos.x;
		p.y = pos.y;
		break;
	case 1:
		p.x = pos.x - GetTextWidth(text) * 0.5f;
		p.y = pos.y;
		break;
	case 2:
		p.x = pos.x - GetTextWidth(text);
		p.y = pos.y;
		break;
	}
	cTokenV token;
	token.Init(text, strlen(text));
	static const char* pLeft = K2L_Compatibility("(");
	static const char* pRite = K2L_Compatibility(")");
	static const char* pBoth = K2L_Compatibility("()");
	while (token.GetPos() < token.GetLen())
	{
		char* str;
		bool found = token.GetToken(&str, pLeft, 1);
		p.x += pGdi->PrintText(p, 0, str, 0);
		if (token.GetPos() >= token.GetLen())
		{
			if (found)
				p.x += pGdi->PrintText(p, 0, pLeft, 0);
			break;
		}
		while ((found = token.GetToken(&str, pBoth, 2)))
		{
			if (token.GetPos() == 0 ||
				token.GetBuf()[token.GetPos() - 1] == *pRite)
				break;
			p.x += pGdi->PrintText(p, 0, pLeft, 0);
			p.x += pGdi->PrintText(p, 0, str, 0);
		}
		int icon = GetIconIndex(str);
		if (found && icon != -1)
		{
			bool flip = strstr(str, "-") != 0;
			int height = m_selHeight;
			float y = p.y - (height - pGdi->GetFontHeight());
			float h = height;
			float w = m_selWidth;
			Draw(icon, WRect(p.x, y, w, h), emoDiffuse, flip);
			p.x += m_selWidth;
		}
		else
		{
			p.x += pGdi->PrintText(p, 0, pLeft, 0);
			p.x += pGdi->PrintText(p, 0, str, 0);
			if (found)
				p.x += pGdi->PrintText(p, 0, pRite, 0);
		}
	}
	return p.x - pos.x;
}

float FrEmoticon::PrintText11(const WPoint& pos, unsigned long align,
	const char* text, unsigned long emoDiffuse)
{
	FrGraphicInterface* pGdi = m_pManager->GetGDI();
	if (!pGdi)
		return 0.0f;
	pGdi->SetTextColor((pGdi->GetTextColor() & 0xffffff) |
			(emoDiffuse & 0xff000000),
		(pGdi->GetTextOutlineColor() & 0xffffff) | (emoDiffuse & 0xff000000));
	WPoint p;
	switch (align)
	{
	case 0:
		p.x = pos.x;
		p.y = pos.y;
		break;
	case 1:
		p.x = pos.x - GetTextWidth11(text) * 0.5f;
		p.y = pos.y;
		break;
	case 2:
		p.x = pos.x - GetTextWidth11(text);
		p.y = pos.y;
		break;
	}
	cTokenV token;
	token.Init(text, strlen(text));
	static const char* pLeft = K2L_Compatibility("(");
	static const char* pRite = K2L_Compatibility(")");
	static const char* pBoth = K2L_Compatibility("()");
	while (token.GetPos() < token.GetLen())
	{
		char* str;
		bool found = token.GetToken(&str, pLeft, 1);
		p.x += pGdi->PrintText11(p, 0, str, 0);
		if (token.GetPos() >= token.GetLen())
		{
			if (found)
				p.x += pGdi->PrintText11(p, 0, pLeft, 0);
			break;
		}
		while ((found = token.GetToken(&str, pBoth, 2)))
		{
			if (token.GetPos() == 0 ||
				token.GetBuf()[token.GetPos() - 1] == *pRite)
				break;
			p.x += pGdi->PrintText11(p, 0, pLeft, 0);
			p.x += pGdi->PrintText11(p, 0, str, 0);
		}
		int icon = GetIconIndex(str);
		if (found && icon != -1)
		{
			bool flip = strstr(str, "-") != 0;
			int height = m_selHeight;
			float y = p.y - (height - pGdi->GetFontHeight());
			float h = height;
			float w = m_selWidth;
			Draw(icon, WRect(p.x, y, w, h), emoDiffuse, flip);
			p.x += m_selWidth;
		}
		else
		{
			p.x += pGdi->PrintText11(p, 0, pLeft, 0);
			p.x += pGdi->PrintText11(p, 0, str, 0);
			if (found)
				p.x += pGdi->PrintText11(p, 0, pRite, 0);
		}
	}
	return p.x - pos.x;
}

float FrEmoticon::GetTextWidth(const char* text)
{
	FrGraphicInterface* pGdi = m_pManager->GetGDI();
	if (!pGdi)
		return 0.0f;
	float width = 0.0f;
	cTokenV token;
	token.Init(text, strlen(text));
	static const char* pLeft = K2L_Compatibility("(");
	static const char* pRite = K2L_Compatibility(")");
	static const char* pBoth = K2L_Compatibility("()");
	while (token.GetPos() < token.GetLen())
	{
		char* str;
		bool found = token.GetToken(&str, pLeft, 1);
		width += pGdi->GetTextExtend(str);
		if (token.GetPos() >= token.GetLen())
		{
			if (found)
				width += pGdi->GetTextExtend(pLeft);
			break;
		}
		while ((found = token.GetToken(&str, pBoth, 2)))
		{
			if (token.GetPos() == 0 ||
				token.GetBuf()[token.GetPos() - 1] == *pRite)
				break;
			width += pGdi->GetTextExtend(pLeft);
			width += pGdi->GetTextExtend(str);
		}
		if (found && GetIconIndex(str) != -1)
			width += m_selWidth;
		else
		{
			width += pGdi->GetTextExtend(pLeft);
			width += pGdi->GetTextExtend(str);
			if (found)
				width += pGdi->GetTextExtend(pRite);
		}
	}
	return width;
}

float FrEmoticon::GetTextWidth11(const char* text)
{
	FrGraphicInterface* pGdi = m_pManager->GetGDI();
	if (!pGdi)
		return 0.0f;
	float width = 0.0f;
	cTokenV token;
	token.Init(text, strlen(text));
	static const char* pLeft = K2L_Compatibility("(");
	static const char* pRite = K2L_Compatibility(")");
	static const char* pBoth = K2L_Compatibility("()");
	while (token.GetPos() < token.GetLen())
	{
		char* str;
		bool found = token.GetToken(&str, pLeft, 1);
		width += pGdi->GetTextExtend11(str);
		if (token.GetPos() >= token.GetLen())
		{
			if (found)
				width += pGdi->GetTextExtend11(pLeft);
			break;
		}
		while ((found = token.GetToken(&str, pBoth, 2)))
		{
			if (token.GetPos() == 0 ||
				token.GetBuf()[token.GetPos() - 1] == *pRite)
				break;
			width += pGdi->GetTextExtend11(pLeft);
			width += pGdi->GetTextExtend11(str);
		}
		if (found && GetIconIndex(str) != -1)
			width += m_selWidth;
		else
		{
			width += pGdi->GetTextExtend11(pLeft);
			width += pGdi->GetTextExtend11(str);
			if (found)
				width += pGdi->GetTextExtend11(pRite);
		}
	}
	return width;
}

int FrEmoticon::GetIconIndex(const char* key)
{
	if (!key || !*key)
		return -1;
	for (int i = 0; i < m_Emoticons.size(); i++)
	{
		sEmoticon* emo = m_Emoticons[i];
		if (!strcmpi(emo->Name.c_str(), key) ||
			(*key == '-' && !strcmpi(emo->Name.c_str(), key + 1)))
			return i;
		for (int j = 0; j < 2; j++)
		{
			if (!strcmpi(emo->Alias[j].c_str(), key))
				return i;
			if (*key == '-' && !strcmpi(emo->Alias[j].c_str(), key + 1))
				return i;
		}
	}
	return -1;
}

const char* FrEmoticon::GetIconName(int index)
{
	return m_Emoticons[index]->Name.c_str();
}

int FrEmoticon::GetIconNum()
{
	return m_Emoticons.size();
}

void FrEmoticon::GetAlias(std::string (*buffer)[2], int index)
{
	if (strlen(m_Emoticons[index]->Alias[0].c_str()))
		(*buffer)[0] = m_Emoticons[index]->Alias[0];
	if (strlen(m_Emoticons[index]->Alias[1].c_str()))
		(*buffer)[1] = m_Emoticons[index]->Alias[1];
}

void FrEmoticon::ParseAnimFrame(const std::string& Frames,
	std::vector<int>& Vector)
{
	char Seps[] = " ";
	char* token = strtok(const_cast<char*>(Frames.c_str()), Seps);
	while (token)
	{
		Vector.push_back(atoi(token));
		token = strtok(0, Seps);
	}
}

bool FrEmoticon::LoadXml(const char* FileName)
{
	TiXmlDocument Xml;
	if (m_Emoticons.size())
		return true;
	if (!Xml.LoadFileEx(FileName))
		return false;
	TiXmlNode* Root = Xml.FirstChild("emoticon");
	if (!Root)
		return false;
	Root->ToElement()->Attribute("fps", &m_Fps);
	TiXmlNode* DefNode = Root->FirstChild("def");
	while (DefNode)
	{
		TiXmlElement* Def = DefNode->ToElement();
		sEmoticon* Emoticon = new sEmoticon;
		Emoticon->Name = Def->Attribute("name");
		Emoticon->Alias[0] = Def->Attribute("alias");
		Emoticon->Alias[1] = Def->Attribute("alias2");
		TiXmlNode* AnimNode = DefNode->FirstChild("anim");
		if (AnimNode)
		{
			TiXmlElement* Anim = AnimNode->ToElement();
			std::string Res = Anim->Attribute("res");
			ParseAnimFrame(Anim->Attribute("frame"), Emoticon->Frames);
			RESMAP_itr i = m_ResMap.find(Res);
			if (i == m_ResMap.end())
			{
				WOverlay* overlay = g_resrcmng->GetOverlay(Res.c_str(), 0);
				m_ResMap[Res] = overlay;
				Emoticon->Overlay = overlay;
			}
			else
				Emoticon->Overlay = m_ResMap[Res];
		}
		m_Emoticons.push_back(Emoticon);
		DefNode = DefNode->NextSibling();
	}
	return true;
}
