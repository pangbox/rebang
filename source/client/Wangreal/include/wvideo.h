const float g_PI = 3.14159265358979323846f;
const float g_2_PI = 6.28318530717958647692f;
const float g_PI_DIV_2 = 1.57079632679489661923f;
const float g_HUGE = 3.402823466e+38f;
const float g_EPSILON = 0.00001f;
const float g_CM_TO_WU = 0.32f;

namespace
{
	// This is a placeholder; there were probably inlines that used these
	// constants, but we have not recovered them yet.
	inline void InstantiateWangrealMathInlines()
	{
		(void)g_PI;
		(void)g_2_PI;
		(void)g_PI_DIV_2;
		(void)g_HUGE;
		(void)g_EPSILON;
		(void)g_CM_TO_WU;
	}
}

struct WRenderToTextureSizeInfo
{
	bool m_isAbsolute;
	float m_width;
	float m_height;

	static const WRenderToTextureSizeInfo SIZE_FULL;
	static const WRenderToTextureSizeInfo SIZE_HALF;
	static const WRenderToTextureSizeInfo SIZE_ABS;

	WRenderToTextureSizeInfo();
	WRenderToTextureSizeInfo(bool isAbsolute, float width, float height)
		: m_isAbsolute(isAbsolute), m_width(width), m_height(height)
	{
	}
	void Reset();
};

const WRenderToTextureSizeInfo WRenderToTextureSizeInfo::SIZE_FULL(false, 1.0f,
	1.0f);
const WRenderToTextureSizeInfo WRenderToTextureSizeInfo::SIZE_HALF(false, 0.5f,
	0.5f);
const WRenderToTextureSizeInfo WRenderToTextureSizeInfo::SIZE_ABS(true, 0.0f,
	0.0f);
