#pragma once
// clang-format off
namespace ILNUM_WVIEW { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; typedef int t23; typedef int t24; typedef int t25; typedef int t26; typedef int t27; typedef int t28; typedef int t29; typedef int t30; typedef int t31; typedef int t32; typedef int t33; typedef int t34; typedef int t35; typedef int t36; typedef int t37; typedef int t38; typedef int t39; typedef int t40; typedef int t41; typedef int t42; typedef int t43; typedef int t44; typedef int t45; typedef int t46; typedef int t47; typedef int t48; typedef int t49; typedef int t50; typedef int t51; typedef int t52; typedef int t53; typedef int t54; typedef int t55; } // n=56
// clang-format on
#include "wmath.h"
#include "wtypes.h"
#include "wresrcmng.h"
#include "wvideo.h"

class WView : public WResource
{
public:
	enum PROJECTION_MODE
	{
		PERSPECTIVE = 0,
		PARALLEL = 1
	};

	WView();

	virtual void BeginScene();
	virtual void Clear(ulong flags, int color);
	virtual void EndScene();
	void Update(unsigned long, int);
	virtual void Flush(ulong flags);
	virtual void Render();
	virtual void DrawPolygonFan(WTVertex** vertices, int count, int flags,
		bool transform);
	virtual void DrawIndexedTriangles(WTVertex* vertices, int vertexCount,
		unsigned short* indices, int indexCount, int flags, int transform);
	void DrawProjPolygonFan(WTVertex** v, int type, int flags)
	{
		DrawPolygonFan(v, type, flags, true);
	}
	virtual bool IsShadowView(void) { return false; }
	virtual void DrawIndexedTrianglesDirect(WTVertex*, int, unsigned short*,
		int, int, int);

	void SetClip(float, float, bool);
	void SetPrevCamera(const WMatrix&);
	void SetCamera(const WMatrix&);
	const WMatrix& GetCamera() const { return camera; }
	const WMatrix& GetInvCamera() const { return invcamera; }
	bool InFrustum(const WSphere& sphere);
	bool InFrustum(const WVector& pos);
	bool InFrustum(const Waabb& box);
	bool InFrustumSafe(const WSphere& sphere);
	bool InFrustumSafe(const Waabb& box);
	bool ProcessEffect() { return m_bProcessEffect; }
	bool SetFogEnable(bool);
	void SetFogState(float, float, unsigned long);
	void SetViewport(float, float);
	WVector2D GetScreenCenter() const
	{
		return WVector2D(m_center_x, m_center_y);
	}
	void SetScreenCenter(const WVector2D& center)
	{
		SetScreenCenter(center.x, center.y);
	}
	void SetFOV(float);
	void DrawLine2D(const _WPOINT&, const _WPOINT&, unsigned long, int);
	void DrawLine2D(const _WPOINT&, const _WPOINT&, unsigned long,
		unsigned long, int);
	void DrawLine2D(const WVector&, const WVector&, unsigned long, float);
	void Draw2DTexture(const WRect&, const WRect&, int, unsigned long,
		unsigned long);
	void DrawLine(const WVector&, unsigned long, const WVector&, unsigned long,
		int);
	void DrawAABB(const Waabb&, unsigned long, bool, int);
	void DrawAABB(WMatrix&, const Waabb&, unsigned long, bool, int);
	void DrawOBB(const WMatrix&, unsigned long);
	void DrawOBB(const Wobb&, unsigned long);
	void DrawSphere(const WVector&, float, unsigned long, int, int);
	void DrawSphere(const WSphere&, unsigned long, int, int);
	void SetScreenCenter(float, float);
	void ResetScreenCenter()
	{
		SetScreenCenter(SCREEN_XS * 0.5f, SCREEN_YS * 0.5f);
	}
	void UpdateCamera(void);
	void GetScreen2World(float, float, WVector&, WVector&);
	void xSetLight(unsigned long, const LightSet&);
	LightSet* xGetLight(void);
	void xScaleProjMat(int, int);
	void SetScale(float);
	float GetScale() const { return m_scale; }
	bool GetReflective() const { return m_isReflective; }
	float GetFOV_Unmodified() const { return FOV; }
	PROJECTION_MODE GetProjectionMode() const { return m_projMode; }
	void SetProjectionMode(PROJECTION_MODE);
	WVector Projection(const WVector&);
	void Projection2(WTVertex*, const WVector&);
	void SetClippingArea(const WRect&);
	void ResetClippingArea()
	{
		SetClippingArea(WRect(0.f, 0.f, SCREEN_XS, SCREEN_YS));
	}
	void xDrawIndexedTriangles(const WxBatchState&);

protected:
	void CheckReflectiveAndConvertCullFlag(int& flag) const
	{
		if (m_isReflective)
		{
			switch (flag & 0xc00)
			{
			case 0:
				flag |= 0x800;
				break;
			case 0x800:
				flag &= ~0x800;
				break;
			}
		}
	}

	void SetFOV_Unmodified(float);
	void CheckViewAndProjTransformUpdateToVideo(void);
	WVector Projection_Perspective(const WVector&);
	WVector Projection_Parallel(const WVector&);
	void Projection2_Perspective(WTVertex*, const WVector&);
	void Projection2_Parallel(WTVertex*, const WVector&);
	void GetScreen2World_Perspective(float, float, WVector&, WVector&);
	void GetScreen2World_Parallel(float, float, WVector&, WVector&);
	void UpdateProjectionTransform_Perspective(void);
	void UpdateProjectionTransform_Parallel(void);
	void UpdateViewTransform(void);
	void UpdateCamera_Perspective(void);
	void UpdateCamera_Parallel(void);
	void ClipPlane(WTVertex*, const WTVertex*, const WTVertex*, int,
		const WPlane&);

	static WVector (WView::* ms_fnProjection[2])(const WVector&);
	static void (WView::* ms_fnProjection2[2])(WTVertex*, const WVector&);
	static void (WView::* ms_fnUpdateProjectionTransform[2])(void);
	static void (WView::* ms_fnUpdateCamera[2])(void);
	static void (
		WView::* ms_fnGetScreen2World[2])(float, float, WVector&, WVector&);

public:
	static const WMatrix4 ms_uvConvMat;

	__forceinline float GetWidth() const { return SCREEN_XS; }
	__forceinline float GetHeight() const { return SCREEN_YS; }
	float GetRatio() const { return SCREEN_YS / SCREEN_XS; }
	float GetClipNearValue() const { return clip_near; }
	float GetClipFarValue() const { return clip_far; }
	float GetFOV() const { return FOV * (SCREEN_YS / SCREEN_XS) * (4.f / 3.f); }
	__forceinline WVideoDev* GetVideoDevice() const
	{
		WVideoDev* video = GetResrcManager()->video;
		return video;
	}
	void SetReflective(bool reflective) { m_isReflective = reflective; }
	float xGetProjScale() const { return proj_scale; }
	const WxViewState& xGetViewState() const { return m_xViewState; }

	void xConvScreenRectByProjScale(WRect& rc) const
	{
		if (proj_scale > 1.0f)
		{
			float xs = SCREEN_XS;
			rc.x = (rc.x - (left + 0.5f) * xs) * proj_scale;
			float ys = SCREEN_YS;
			rc.y = (rc.y - (top + 0.5f) * ys) * proj_scale;
			rc.w = rc.w * proj_scale;
			rc.h = rc.h * proj_scale;
		}
	}

protected:
	const WMatrix& GetLastCamera() { return lastcam; }
	float CalcSZ(float);

public:
	WMatrix camera;
	WMatrix invcamera;

protected:
	WMatrix matrix;
	WPlane frustum[6];
	WPlane frustumSafe[6];
	float scalex;
	float scaley;
	float m_center_x;
	float m_center_y;
	float clip_near;
	float clip_far;
	float clip_scaled_near;
	float clip_scaled_far;
	float SCREEN_XS;
	float SCREEN_YS;
	unsigned int m_cliptype;
	bool m_fastclip;
	WRect m_clipArea;
	float clip_scale_z;
	float clip_near_scale;
	float FOV;
	WVector m_temp[512];
	WMatrix lastcam;
	int update;
	float proj_scale;
	float left;
	float right;
	float top;
	float bottom;
	bool m_bProcessEffect;
	bool m_bDisableFog;
	bool m_isReflective;
	WxViewState m_xViewState;
	bool m_xNeedToUpdateViewTransfToVideo;
	bool m_xNeedToUpdateProjTransfToVideo;
	WView::PROJECTION_MODE m_projMode;
	float m_scale;
	WPlane frustumByCam[6];

public:
	__forceinline virtual ~WView();
};

class WViewOrth : public WView
{
public:
	virtual ~WViewOrth() { }
};

WMatrix __cdecl MakeCameraMatrix(const WVector& eye, const WVector& at);

inline unsigned long ULblend(unsigned long c1, unsigned long c2,
	unsigned char t)
{
	return ((((c1 & 0xff00ff) * (255 - t) + (c2 & 0xff00ff) * t) >> 8) &
			   0xff00ff) |
		((((c1 >> 8) & 0xff00ff) * (255 - t) + ((c2 >> 8) & 0xff00ff) * t) &
			0xff00ff00);
}

inline bool WView::InFrustum(const WSphere& sphere)
{
	for (int i = 0; i < 6; ++i)
		if (frustum[i] * sphere.pos > sphere.radius)
			return false;
	return true;
}

inline bool WView::InFrustum(const Waabb& box)
{
	WPlane* plane = frustum;
	for (int i = 0; i < 6; ++i, ++plane)
	{
		WVector v(plane->x < 0.0f ? box.max.x : box.min.x,
			plane->y < 0.0f ? box.max.y : box.min.y,
			plane->z < 0.0f ? box.max.z : box.min.z);
		if (*plane * v > 0.0f)
			return false;
	}
	return true;
}

inline bool WView::InFrustumSafe(const WSphere& sphere)
{
	for (int i = 0; i < 6; ++i)
		if (frustumSafe[i] * sphere.pos + sphere.radius >= 0.0f)
			return false;
	return true;
}

inline bool WView::InFrustumSafe(const Waabb& box)
{
	WPlane* plane = frustumSafe;
	for (int i = 0; i < 6; ++i, ++plane)
	{
		WVector v(plane->x > 0.0f ? box.max.x : box.min.x,
			plane->y > 0.0f ? box.max.y : box.min.y,
			plane->z > 0.0f ? box.max.z : box.min.z);
		if (*plane * v >= 0.0f)
			return false;
	}
	return true;
}

inline bool WView::InFrustum(const WVector& pos)
{
	for (int i = 0; i < 6; ++i)
		if (frustum[i] * pos > 0.f)
			return false;
	return true;
}
