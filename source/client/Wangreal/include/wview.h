#pragma once
#include "wresource.h"
#include "wmath.h"
#include "wvideo.h"

class WView : public WResource
{
public:
	WView();
	void SetFOV(float fov);
	void SetClip(float nearClip, float farClip, bool updateDevice);
	void SetCamera(const WMatrix& camera);

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

	__forceinline float GetWidth() const { return SCREEN_XS; }
	__forceinline float GetHeight() const { return SCREEN_YS; }
	void SetViewport(float width, float height);
	void UpdateCamera();
	__forceinline WVideoDev* GetVideoDevice() const
	{
		return GetResrcManager()->video;
	}
	virtual void DrawIndexedTrianglesDirect(WTVertex* vertices, int vertexCount,
		unsigned short* indices, int indexCount, int flags, int transform);

protected:
	const WMatrix& GetLastCamera() { return lastcam; }

	char unknown_02c[0x170];
	float SCREEN_XS;
	float SCREEN_YS;
	char unknown_01a4[0x1824];
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
	char unknown_1a13[0x109];

public:
	__forceinline virtual ~WView();
};
