#pragma once
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
	virtual void Flush(ulong flags);
	virtual void Render();
	virtual void DrawPolygonFan(WTVertex** vertices, int count, int flags,
		bool transform);
	virtual void DrawIndexedTriangles(WTVertex* vertices, int vertexCount,
		unsigned short* indices, int indexCount, int flags, int transform);
	virtual bool IsShadowView();
	virtual void DrawIndexedTrianglesDirect(WTVertex*, int, unsigned short*,
		int, int, int);

	void SetClip(float, float, bool);
	void SetPrevCamera(const WMatrix&);
	void SetCamera(const WMatrix&);
	const WMatrix& GetCamera() const { return camera; }
	const WMatrix& GetInvCamera() const { return invcamera; }
	bool ProcessEffect() { return m_bProcessEffect; }
	bool SetFogEnable(bool);
	void SetFogState(float, float, unsigned long);
	void SetViewport(float, float);
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
	void UpdateCamera(void);
	void GetScreen2World(float, float, WVector&, WVector&);
	void xSetLight(unsigned long, const LightSet&);
	LightSet* xGetLight(void);
	void xScaleProjMat(int, int);
	void SetScale(float);
	void SetProjectionMode(PROJECTION_MODE);
	WVector Projection(const WVector&);
	void Projection2(WTVertex*, const WVector&);
	void SetClippingArea(const WRect&);
	void xDrawIndexedTriangles(const WxBatchState&);

protected:
	void CheckReflectiveAndConvertCullFlag(int&) const;
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
	__forceinline WVideoDev* GetVideoDevice() const
	{
		return GetResrcManager()->m_video;
	}
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
	const WMatrix& GetLastCamera() { return lastcam; }

	WMatrix camera;
	WMatrix invcamera;
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
