#pragma once

#include <list>

class WPuppet;
class CWeatherType;
class CBeach;
class CWavelet;
class CPool;
class ntRiver;
class CSea;
class CMarker;
class WFont;

enum eObjType
{
	OBJ_BASE,
	OBJ_FUNC,
	OBJ_STATIC,
	OBJ_MODEL,
	OBJ_ACTOR,
	OBJ_CUSTOM,
	OBJ_EFFECT,
	OBJ_NONE
};

class CRenderFuncPtr
{
public:
	CRenderFuncPtr() { }
	~CRenderFuncPtr() { }

	void DisplayAll() { Display(); }

public:
	virtual void DisplayShadow() { }
	virtual void Display() = 0;

protected:
	virtual void DebugDisplay() { }
};

class WTextureImposterView;

struct SceneElement
{
	eObjType m_type;
	WPuppet* m_pPuppet;
	CRenderFuncPtr* m_pRender;
	char m_name[0x80];
	unsigned long m_flags;
	unsigned char m_alpha;
	bool m_visible;
	WTextureImposterView* m_pImposter;
	Waabb* m_pBox;

	SceneElement(void)
	{
		m_type = (eObjType)2;
		m_pPuppet = 0;
		m_pRender = 0;
		m_name[0] = 0;
		m_pImposter = 0;
		m_pBox = 0;
		m_flags = 0;
		m_alpha = 0xFF;
		m_visible = true;
	}

	~SceneElement(void) { }

	bool operator==(const char* name) const
	{
		if (m_name[0] == 0 || name == 0)
		{
			return false;
		}

		return strcmpi(m_name, name) == 0;
	}
};

struct CustomElement
{
	eObjType m_type;
	CRenderFuncPtr* m_pRender;

	CustomElement(eObjType type, CRenderFuncPtr* pRender)
	{
		m_type = type;
		m_pRender = pRender;
	}
};

struct TransparentElement
{
	TransparentElement(WPuppet* pPuppet, Waabb* pBox, int texture, int numPolys)
	{
		m_pPuppet = pPuppet;
		m_pBox = pBox;
		m_texture = texture;
		m_numPolys = numPolys;
	}

	WPuppet* m_pPuppet;
	Waabb* m_pBox;
	unsigned long m_texture;
	int m_numPolys;
};

struct SceneElemRegister
{
	eObjType m_type;
	WPuppet* m_pPuppet;
	CRenderFuncPtr* m_pRender;
	const char* m_name;
	unsigned long m_flags;
	Waabb* m_pBox;
};

typedef std::list<SceneElement>::iterator SceneElementIter;
typedef std::list<CustomElement>::iterator CustomElementIter;
typedef std::list<TransparentElement>::iterator TransparentElementIter;

class CSceneManager : public CStackableSingleton<CSceneManager>
{
	friend class CConsole;

public:
	struct sDrop
	{
		WVector m_pos;
		unsigned long m_color;
	};

	CSceneManager(void);
	virtual ~CSceneManager(void);
	void Load(void);
	void SetReflection(bool reflection);
	void Prerender(WView* pView);
	void UpdateSea(void);
	void UpdateWind(void);
	void RegisterElement(eObjType type, WPuppet* pPuppet,
		CRenderFuncPtr* pRender, const char* name, unsigned long flags,
		Waabb* pBox);
	void RegisterElement(SceneElemRegister& reg);
	void ReleaseElement(void);
	void UpdateBright(void);
	void UpdateFog(bool load);
	void UpdateFog(const char* name);
	void SetLobbyWeather(bool weather);
	WPuppet* GetPuppet(const char* name);
	eObjType GetType(const char* name);
	void SetAlpha(const char* name, unsigned char alpha);
	void SetVisible(const char* name, bool visible);
	void SetPuppet(const char* name, WPuppet* pPuppet);
	void SetType(const char* name, eObjType type);
	void DeleteElement(const char* name);
	void SetLOD(bool lod, bool apply);
	void DropTest(void);
	void Clear(void);
	void Process(float elapsed);
	void DrawDebugLine(WView* pView);
	void Render(WView* pView, unsigned long mask);
	void SetWeather(unsigned char weather, bool lobby);
	void SetFog(bool b) { m_useFog = b; }

	void ShowTag(void) { m_bDrawElementBoxes = !m_bDrawElementBoxes; }

	bool ToggleTransparent(void)
	{
		return m_bOverlapCamera = !m_bOverlapCamera;
	}

	bool ToogleNoOverlapCamera(void)
	{
		return m_bNoOverlapCamera = !m_bNoOverlapCamera;
	}

protected:
	unsigned char OverlapCamera(WView* pView, const SceneElement& elem);
	void Render_BuildImposter(WView* pView);
	void Render_RenderWaterStuff(WView* pView);
	void Render_RenderDropPosition(WView* pView);
	int Render_RenderElement(WView* pView, unsigned long mask);
	void Render_RenderBlendGroup(WView* pView,
		std::list<TransparentElement>& elements, int first, int last);
	void Render_RenderOBMarker(WView* pView);
	void Render_RenderSea(WView* pView);
	void Render_RenderSky(WView* pView);
	void Render_RenderWeather(WView* pView);
	void Render_RenderWeatherFullScreenOverlay(WView* pView);
	void Render_RenderEffect(WView* pView);
	void Render_RenderBillboardGroup(WView* pView);
	void Render_RenderCustom(WView* pView);
	void Render_RenderNearGroup(WView* pView);
	void DrawImposter(WView* pView, const TransparentElement& elem);
	void DrawPuppet(WView* pView, WPuppet* pPuppet, Waabb* pBox);
	void RenderShadow(WView* pView, unsigned long mask);
	void Render_BaseAndShadow(WView* pView, unsigned long mask);

private:
	static WTVertex vtr[4];
	static WTVertex* vtx[5];

	WList<CBeach*> m_beachList;
	WList<CWavelet*> m_waveletList;
	WList<CPool*> m_poolList;
	WList<ntRiver*> m_riverList;
	CSea* m_pSea;
	CMarker* m_pMarker;

public:
	CWeatherType* m_pWeather;

private:
	CWeatherType* m_pLobbyWeather;
	bool m_useFog;
	bool m_useImposter;
	bool m_skipTransparent;

	std::list<SceneElement> m_elements;
	std::list<CustomElement> m_customElements;
	std::list<CustomElement> m_effectElements;
	std::list<TransparentElement> m_transparentElements;
	std::list<TransparentElement> m_blendElements;
	std::list<TransparentElement> m_nearElements;
	bool m_dirty;

	std::list<sDrop> m_drops;
	unsigned char m_reserved114;
	bool m_bDrawElementBoxes;

	WFont* m_pFont;
	float m_fogStart;
	float m_fogEnd;
	unsigned long m_fogColor;
	bool m_bOverlapCamera;
	bool m_bNoOverlapCamera;

	struct sDebugDraw
	{
		unsigned long bits;
	};
	sDebugDraw m_debugDraw;
};
class CWavelet
{
public:
	struct sWavelet
	{
		WVector pos[2];
		WVector dir;
		float u[2];
	};

	struct sLayer
	{
		float pos;
		float delay;
		float alpha;
		float time;
		float speed;
		int state;
	};

	CWavelet();
	virtual ~CWavelet();

	virtual void Process(float dt);
	virtual void Render();

	void SetWavelet(const std::vector<WVector>& points, const char* name,
		int layerNum);

protected:
	virtual void ReadAttribute(const char* name);
	virtual void SetLayer(int num);

	std::vector<sWavelet> m_wavelet;
	int m_texHandle;
	float m_width;
	float m_frequency;
	float m_deceleration;
	float m_fadeInTime;
	float m_fadeOutTime;
	float m_delay;
	std::vector<sLayer> m_layer;

	static WTVertex vtr[4];
	static WTVertex* vtx[5];
};
