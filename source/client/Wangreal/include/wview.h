#pragma once
#include "wmath.h"
#include "wtypes.h"
#include "wresrcmng.h"
#include "wvideo.h"

class WView : public WResource
{
public:
	WView();
	void SetFOV(float);
	void SetClip(float, float, bool);
	bool SetFogEnable(bool);
	void SetFogState(float, float, unsigned long);
	void SetCamera(const WMatrix&);
	void SetPrevCamera(const WMatrix&);
	enum UPDATE_MODE
	{
		BEGINESCENE = 1,
		CLEARZBUFF = 2,
		CLEARFRAME = 4
	};
	enum UPDATE_TRANSFORM
	{
		UPDATE_NONE = 0,
		UPDATE_VIEW = 1,
		UPDATE_PROJ = 2
	};
	virtual void BeginScene();
	virtual void Clear(ulong flags, int color);
	virtual void EndScene();
	void Update(unsigned long, int);
	virtual void Flush(ulong flags);
	virtual void Render();
	WVector Projection(const WVector&);
	void Projection2(WTVertex*, const WVector&);
	virtual void DrawPolygonFan(WTVertex** vertices, int count, int flags,
		bool transform);
	virtual void DrawIndexedTriangles(WTVertex* vertices, int vertexCount,
		unsigned short* indices, int indexCount, int flags, int transform);
	void DrawProjPolygonFan(WTVertex** v, int type, int flags)
	{
		DrawPolygonFan(v, type, flags, true);
	}
	virtual bool IsShadowView(void) { return false; }
	void DrawLine2D(const _WPOINT&, const _WPOINT&, unsigned long, int);
	void DrawLine2D(const _WPOINT&, const _WPOINT&, unsigned long,
		unsigned long, int);
	void DrawLine2D(const WVector&, const WVector&, unsigned long, float);
	void DrawLine(const WVector&, unsigned long, const WVector&, unsigned long,
		int);
	void DrawAABB(WMatrix&, const Waabb&, unsigned long, bool, int);
	void DrawAABB(const Waabb&, unsigned long, bool, int);
	void DrawOBB(const WMatrix&, unsigned long);
	void DrawOBB(const Wobb&, unsigned long);
	void DrawSphere(const WVector&, float, unsigned long, int, int);
	void DrawSphere(const WSphere&, unsigned long, int, int);
	void Draw2DTexture(const WRect&, const WRect&, int, unsigned long,
		unsigned long);
	bool InFrustum(const WVector& pos);
	bool InFrustum(const Waabb& box);
	bool InFrustum(const WSphere& sphere);
	bool InFrustum(WTVertex** vertices);
	bool InFrustumSafe(const WSphere& sphere);
	bool InFrustumSafe(const Waabb& box);
	const WMatrix& GetCamera() const { return camera; }
	const WMatrix& GetInvCamera() const { return invcamera; }
	float GetClipNearValue() const { return clip_near; }
	float GetClipFarValue() const { return clip_far; }
	float GetFOV() const { return FOV * (SCREEN_YS / SCREEN_XS) * (4.f / 3.f); }
	float GetWidth() const { return SCREEN_XS; }
	float GetHeight() const { return SCREEN_YS; }
	float GetRatio() const { return SCREEN_YS / SCREEN_XS; }
	void SetViewport(float, float);
	WVector2D GetScreenCenter() const
	{
		return WVector2D(m_center_x, m_center_y);
	}
	void SetScreenCenter(float, float);
	void SetScreenCenter(const WVector2D& center)
	{
		SetScreenCenter(center.x, center.y);
	}
	void ResetScreenCenter()
	{
		SetScreenCenter(SCREEN_XS * 0.5f, SCREEN_YS * 0.5f);
	}
	void GetScreen2World(float, float, WVector&, WVector&);
	WMatrix camera;
	WMatrix invcamera;
	void SetClippingArea(const WRect&);
	void ResetClippingArea()
	{
		SetClippingArea(WRect(0.f, 0.f, SCREEN_XS, SCREEN_YS));
	}
	void UpdateCamera(void);
	float GetScale() const { return m_scale; }
	void SetScale(float);

protected:
	WMatrix matrix;

public:
	virtual void DrawIndexedTrianglesDirect(WTVertex*, int, unsigned short*,
		int, int, int);
	WVideoDev* GetVideoDevice() const
	{
		WVideoDev* video = GetResrcManager()->video;
		return video;
	}
	float GetFOV_Unmodified() const { return FOV; }

protected:
	void SetFOV_Unmodified(float);
	const WMatrix& GetLastCamera() { return lastcam; }
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
	float CalcSZ(float);
	unsigned long m_cliptype;
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

public:
	bool ProcessEffect() { return m_bProcessEffect; }

protected:
	bool m_bProcessEffect;
	bool m_bDisableFog;
	bool m_isReflective;

public:
	bool GetReflective() const { return m_isReflective; }
	void SetReflective(bool reflective) { m_isReflective = reflective; }

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
	WxViewState m_xViewState;
	bool m_xNeedToUpdateViewTransfToVideo;
	bool m_xNeedToUpdateProjTransfToVideo;

public:
	const WxViewState& xGetViewState() const { return m_xViewState; }
	void xSetLight(unsigned long, const LightSet&);
	LightSet* xGetLight(void);
	void xDrawIndexedTriangles(const WxBatchState&);
	void xScaleProjMat(int, int);
	float xGetProjScale() const { return proj_scale; }
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
	void CheckViewAndProjTransformUpdateToVideo(void);

public:
	static const WMatrix4 ms_uvConvMat;
	enum PROJECTION_MODE
	{
		PERSPECTIVE = 0,
		PARALLEL = 1,
		NUM_PROJECTION_MODE = 2
	};
	typedef WVector (WView::*FnProjection)(const WVector&);

protected:
	WVector Projection_Perspective(const WVector&);
	WVector Projection_Parallel(const WVector&);
	static FnProjection ms_fnProjection[NUM_PROJECTION_MODE];
	typedef void (WView::*FnProjection2)(WTVertex*, const WVector&);
	void Projection2_Perspective(WTVertex*, const WVector&);
	void Projection2_Parallel(WTVertex*, const WVector&);
	static FnProjection2 ms_fnProjection2[NUM_PROJECTION_MODE];
	typedef void (WView::*FnUpdateProjectionTransform)();
	void UpdateProjectionTransform_Perspective(void);
	void UpdateProjectionTransform_Parallel(void);
	static FnUpdateProjectionTransform
		ms_fnUpdateProjectionTransform[NUM_PROJECTION_MODE];
	typedef void (WView::*FnUpdateCamera)();
	void UpdateCamera_Perspective(void);
	void UpdateCamera_Parallel(void);
	static FnUpdateCamera ms_fnUpdateCamera[NUM_PROJECTION_MODE];
	typedef void (WView::*FnGetScreen2World)(float, float, WVector&, WVector&);
	void GetScreen2World_Perspective(float, float, WVector&, WVector&);
	void GetScreen2World_Parallel(float, float, WVector&, WVector&);
	static FnGetScreen2World ms_fnGetScreen2World[NUM_PROJECTION_MODE];
	WView::PROJECTION_MODE m_projMode;
	float m_scale;
	WPlane frustumByCam[6];
	void UpdateViewTransform(void);
	void ClipPlane(WTVertex*, const WTVertex*, const WTVertex*, int,
		const WPlane&);

public:
	PROJECTION_MODE GetProjectionMode() const { return m_projMode; }
	void SetProjectionMode(PROJECTION_MODE);
};

class WViewOrth : public WView
{
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
