#pragma once

#include "wscene.h"

class CLensFlare;
class CPvsFxBox;

struct sCloud
{
	float windRand;
	float size;
	WVector pos;
	float alpha;
	bool bActive;
	int tile;
	bool bFlip;
};

class CSky
{
public:
	CSky();
	~CSky();

	void Clear();
	void LoadSky(const char* name, float turn);
	void ChangeWeatherTex(unsigned char weather);

	void ClearClouds();
	void RegisterCloud(char* name);
	void ShowClouds(bool bShow);
	void GenerateClouds(int num, float rate);
	void GenerateStars(int map);
	void GenerateMoons();
	void GenerateLensFlare();
	void ClearSky();

	void Process(float delta);
	void Render(WView* view);
	void UpdateBright();

	void SetColor(unsigned long color) { m_color = color; }

protected:
	void RenderUpAndBottom(WView* view);
	void RenderFarCylinder(WView* view);
	void RenderNearCylinder(WView* view);
	void RenderBillboardCloud(WView* view);
	void RenderFlatCloud(WView* view);

private:
	void LoadTexByWeather(const char* name, const char* part);
	int IsThereTexHandle(const char* part, int weather);
	void SetSkyTex(int tex, unsigned char unused, unsigned char index);
	float CalcScaleByView(WView* view) const;
	float CalcClipFarByView(WView* view) const;

	int m_farTex[5];
	int m_nearTex[5];
	int m_upTex[2];
	int m_dnTex[2];
	int m_bLoaded;
	WxBatchState m_batch[4];
	float m_cloudRange;
	float m_cloudFadeRange;
	float m_cloudHeight;
	int m_cloudTex;
	int m_cloudNum;
	sCloud m_cloud[50];
	int m_activeCloudNum;
	float m_cloudInterval;
	float m_cloudTime;
	float m_height;

public:
	unsigned long m_color;

private:
	float m_turn;
	int m_farDiv;
	int m_nearDiv;
	unsigned char m_reserved[192];
	WVector m_pos;
	CLensFlare* m_pLensFlare;
	CPvsFxBox* m_pStar;
	CPvsFxBox* m_pMoon;
	float m_upAngle;
	float m_nearAngle;
};
