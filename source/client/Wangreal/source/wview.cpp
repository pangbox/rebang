#include "wview.h"

// TODO: another workaround for undetermined optimizer issues
#pragma function(sin, cos, tan)

unsigned long __fastcall ULblend(unsigned long, unsigned long, unsigned char);

static int g_foffset = 0;

WView::WView(void)
{
	update = 3;
	FOV = 1.1780972f;
	clip_near = 6.3999996f;
	clip_far = 1600.0f;
	clip_scale_z = 1.004016f;
	clip_near_scale = 6.425702f;
	clip_scaled_near = 0.99600005f;
	clip_scaled_far = 249.00003f;
	m_fastclip = true;
	m_cliptype = 0;
	camera.Reset();
	lastcam.Reset();
	SetViewport(1.0f, 1.0f);
	m_bProcessEffect = true;
	proj_scale = 1.0f;
	left = -0.5f;
	right = 0.5f;
	top = -0.5f;
	bottom = 0.5f;
	m_bDisableFog = false;
	m_isReflective = false;
	m_xViewState.xLight.type = 2;
	m_xViewState.xLight.nearOne = WVector::UNIT_NEG_Z;
	m_xViewState.xmView = WMatrix4::IDENTITY;
	m_xViewState.xmProj = WMatrix4::ZERO;
	update |= 2;
	m_xNeedToUpdateViewTransfToVideo = true;
	m_xNeedToUpdateProjTransfToVideo = true;
	m_projMode = PERSPECTIVE;
	m_scale = 1.0f;
	UpdateCamera();
}

__forceinline WView::~WView()
{
}

void WView::SetClip(float n, float f, bool toVideo)
{
	clip_near = n;
	clip_far = f;
	clip_scale_z = f / (f - n);
	clip_near_scale = clip_scale_z * n;
	clip_scaled_near = n / clip_near_scale;
	clip_scaled_far = f / clip_near_scale;
	if (toVideo)
	{
		WVideoDev* video = GetVideoDevice();
		if (video)
		{
			video->m_clip_scale_z = clip_scale_z;
			video->m_clip_near_scale = clip_near_scale;
		}
	}
	update |= 2;
}

void WView::Render(void)
{
	WVideoDev* video = GetVideoDevice();
	video->EndScene();
	video->Paint();
}

void WView::SetPrevCamera(const WMatrix& cam)
{
	WMatrix4 m4;
	WMatrix inv = ~cam;
	SetWMatrix4FromWMatrix(m4, inv);
	GetVideoDevice()->xSetPrevViewTransform(m4);
}

void WView::SetCamera(const WMatrix& cam)
{
	update |= 1;
	lastcam = cam;
}

bool WView::SetFogEnable(bool enable)
{
	WVideoDev* video = GetVideoDevice();
	if (video == 0)
	{
		return false;
	}
	return video->SetFogEnable(enable);
}

void WView::SetFogState(float start, float end, unsigned long color)
{
	WVideoDev* video = GetVideoDevice();
	if (video)
	{
		video->SetFogState(start, end, color);
	}
}

void WView::SetViewport(float w, float h)
{
	m_center_x = w * 0.5f;
	m_center_y = h * 0.5f;
	SCREEN_XS = w;
	SCREEN_YS = h;
	m_clipArea = WRect(0.0f, 0.0f, w, h);
	update |= 2;
}

void WView::SetFOV(float fov)
{
	update |= 2;
	FOV = SCREEN_XS / SCREEN_YS * fov * 0.75f;
}

void WView::DrawLine2D(const _WPOINT& a, const _WPOINT& b, unsigned long color,
	int flag)
{
	WTVertex v[2];
	WTVertex* vl[3];

	vl[0] = &v[0];
	v[0].sx = a.x;
	v[0].sy = a.y;
	vl[1] = &v[1];
	v[0].sz = 0.001f;
	v[0].rhw = 1.0f;
	v[0].diffuse = color;
	v[1].sx = b.x;
	v[1].sy = b.y;
	v[1].sz = 0.001f;
	v[1].rhw = 1.0f;
	v[1].diffuse = color;
	vl[2] = 0;
	DrawPolygonFan(vl, flag | 0x4000000, 0, true);
}

void WView::DrawLine2D(const _WPOINT& a, const _WPOINT& b, unsigned long ca,
	unsigned long cb, int flag)
{
	WTVertex v[2];
	WTVertex* vl[3];

	vl[0] = &v[0];
	v[0].sx = a.x;
	v[0].sy = a.y;
	v[0].sz = 0.001f;
	v[0].rhw = 1.0f;
	v[0].diffuse = ca;
	vl[1] = &v[1];
	v[1].sx = b.x;
	v[1].sy = b.y;
	v[1].sz = 0.001f;
	v[1].rhw = 1.0f;
	v[1].diffuse = cb;
	vl[2] = 0;
	DrawPolygonFan(vl, flag | 0x4000000, 0, true);
}

void WView::Draw2DTexture(const WRect& src, const WRect& dst, int tex,
	unsigned long color, unsigned long flag)
{
	static WTVertex m_vtx[4] = { 0 };
	static WTVertex* m_vl[5] = { &m_vtx[0], &m_vtx[1], &m_vtx[3], &m_vtx[2],
		0 };

	for (int i = 0; i < 4; ++i)
	{
		m_vtx[i].sx = ((i & 1) ? dst.x + dst.w : dst.x) - 0.5f;
		m_vtx[i].sy = ((i & 2) ? dst.y + dst.h : dst.y) - 0.5f;
		m_vtx[i].sz = 0.001f;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = color;
		m_vtx[i].tu = (i & 1) ? src.x + src.w : src.x;
		m_vtx[i].tv = (i & 2) ? src.y + src.h : src.y;
	}
	DrawPolygonFan(m_vl, ((tex & 0x7ff) | flag) | 0x20380000, 0, true);
}

void WView::DrawLine2D(const WVector& a, const WVector& b, unsigned long color,
	float width)
{
	WTVertex vStart, vEnd;
	Projection2(&vStart, a);
	Projection2(&vEnd, b);
	WVector p1(vStart.sx, vStart.sy, 1.0f);
	WVector p2(vEnd.sx, vEnd.sy, 1.0f);
	WVector vRight = p1;
	vRight.z = 0.0f;
	WVector vUp = WCrossProduct(p2 - p1, vRight - p1);
	vUp.Normalize();
	vUp *= width;
	WVector vLT = p1 - vUp;
	WVector vLB = p2 - vUp;
	WVector vRB = p2 + vUp;
	WVector vRT = p1 + vUp;
	WTVertex v[4];
	WTVertex* pV[5] = { 0 };
	for (int i = 0; i < 4; ++i)
	{
		pV[i] = &v[i];
		v[i].diffuse = color;
		v[i].sz = 0.001f;
		v[i].rhw = 1.0f;
	}
	v[0].sx = vLT.x;
	v[0].sy = vLT.y;
	v[1].sx = vLB.x;
	v[1].sy = vLB.y;
	v[2].sx = vRB.x;
	v[2].sy = vRB.y;
	v[3].sx = vRT.x;
	v[3].sy = vRT.y;
	DrawPolygonFan(pV, 0x700000, 0, true);
}

void WView::DrawLine(const WVector& a, unsigned long ca, const WVector& b,
	unsigned long cb, int flag)
{
	WTVertex v[2];
	WTVertex* vl[3];

	vl[0] = &v[0];
	vl[1] = &v[1];
	vl[2] = 0;
	v[0].diffuse = ca;
	v[1].diffuse = cb;
	v[0].sx = a.x;
	v[0].sy = a.y;
	v[0].sz = a.z;
	v[1].sx = b.x;
	v[1].sy = b.y;
	v[1].sz = b.z;
	DrawPolygonFan(vl, flag | 0x4000000, 0, false);
}

void WView::DrawAABB(const Waabb&, unsigned long, bool, int)
{
}

void WView::DrawAABB(WMatrix& mat, const Waabb& box, unsigned long color,
	bool solid, int flag)
{
	static WTVertex vtr[4] = { 0 };
	static WTVertex* vtx[5] = { &vtr[0], &vtr[1], &vtr[2], &vtr[3], 0 };
	static int box_p[6][4] = {
		{ 0, 2, 3, 1 },
        { 4, 5, 7, 6 },
        { 0, 1, 5, 4 },
        { 3, 2, 6, 7 },
		{ 1, 3, 7, 5 },
        { 2, 0, 4, 6 }
	};
	static int box_v[8][3] = {
		{ 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 0, 0, 1 },
		{ 1, 0, 1 },
        { 0, 1, 1 },
        { 1, 1, 1 }
	};

	WVector pos[8];
	int i;
	int j;

	vtr[0].diffuse = color;
	vtr[1].diffuse = color;
	vtr[2].diffuse = color;
	vtr[3].diffuse = color;
	for (i = 0; i < 8; ++i)
	{
		pos[i] = WVector(box_v[i][0] ? box.max.x : box.min.x,
					 box_v[i][1] ? box.max.y : box.min.y,
					 box_v[i][2] ? box.max.z : box.min.z) *
			mat;
	}
	for (j = 0; j < 6; ++j)
	{
		for (i = 0; i < 4; ++i)
		{
			if (solid)
			{
				vtr[i].SetPosition(pos[box_p[j][i]]);
				DrawPolygonFan(vtx, flag | 0x400, 0, false);
			}
			else
			{
				DrawLine(pos[box_p[j][(i - 1) & 3]], color, pos[box_p[j][i]],
					color, flag);
			}
		}
	}
}

void WView::DrawOBB(const WMatrix& mat, unsigned long color)
{
	static int box_p[6][4] = {
		{ 0, 2, 3, 1 },
        { 4, 5, 7, 6 },
        { 0, 1, 5, 4 },
        { 3, 2, 6, 7 },
		{ 1, 3, 7, 5 },
        { 2, 0, 4, 6 }
	};
	static int box_v[8][3] = {
		{ 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 0, 0, 1 },
		{ 1, 0, 1 },
        { 0, 1, 1 },
        { 1, 1, 1 }
	};

	WVector pos[8];
	int i;
	int j;

	for (i = 0; i < 8; ++i)
	{
		pos[i] = WVector(box_v[i][0] ? 1.0f : -1.0f, box_v[i][1] ? 1.0f : -1.0f,
					 box_v[i][2] ? 1.0f : -1.0f) *
			mat;
	}
	for (j = 0; j < 6; ++j)
	{
		for (i = 0; i < 4; ++i)
		{
			DrawLine(pos[box_p[j][(i - 1) & 3]], color, pos[box_p[j][i]], color,
				0);
		}
	}
}

void WView::DrawOBB(const Wobb& obb, unsigned long color)
{
	WMatrix mat;

	mat.pivot = obb.center;
	mat.xa = obb.extend[0];
	mat.ya = obb.extend[1];
	mat.za = obb.extend[2];
	DrawOBB(mat, color);
}

void WView::DrawSphere(const WVector& center, float radius, unsigned long color,
	int nSeg, int nStep)
{
	WVector pos[25];
	int i;

	if (nSeg > 24)
	{
		nSeg = 24;
	}
	for (i = 0; i < nSeg; ++i)
	{
		float ang = (float)i * 6.28318548f / (float)nSeg;
		pos[i] = center + camera.xa * radius * cosf(ang) +
			camera.ya * radius * sinf(ang);
	}
	int prev = nSeg - 1;
	for (i = 0; i < nSeg; ++i)
	{
		if (nStep == 0)
			DrawLine(pos[i], color, pos[prev], color, 0);
		else if ((i + 1) % (nStep + 1) == 0)
			DrawLine(pos[i], 0xff00ff00, pos[prev], 0xff00ff00, 0);
		else
			DrawLine(pos[i], color, pos[prev], color, 0);
		prev = i;
	}
}

void WView::DrawSphere(const WSphere& sphere, unsigned long color, int nSeg,
	int nStep)
{
	DrawSphere(sphere.pos, sphere.radius, color, nSeg, nStep);
}

void WView::Clear(unsigned long color, int flag)
{
	if (GetResrcManager() == 0)
	{
		return;
	}
	WVideoDev* video = GetVideoDevice();
	if (video == 0)
	{
		return;
	}
	switch (flag & 6)
	{
	case 2:
		video->Clear(0, 2, 1.0f);
		break;
	case 4:
		video->Clear(color, 1, 1.0f);
		break;
	case 6:
		video->Clear(color, 3, 1.0f);
		break;
	}
}

void WView::BeginScene(void)
{
	if (GetResrcManager() != 0)
	{
		WVideoDev* video = GetVideoDevice();
		video->BeginScene();
		float sx;
		float sy;
		if ((sx = SCREEN_XS) != (float)video->GetWidth() ||
			(sy = SCREEN_YS) != (float)video->GetHeight())
		{
			SetViewport((float)video->GetWidth(), (float)video->GetHeight());
		}
	}
	update |= 3;
	UpdateCamera();
}

void WView::EndScene(void)
{
}

void WView::SetClippingArea(const WRect& rc)
{
	m_clipArea.x = Max(0.0f, rc.x);
	m_clipArea.y = Max(0.0f, rc.y);
	m_clipArea.w = Min(rc.w, SCREEN_XS - m_clipArea.x);
	m_clipArea.h = Min(rc.h, SCREEN_YS - m_clipArea.y);
	WVideoDev* video = GetVideoDevice();
	if (video)
		video->SetViewPort((int)m_clipArea.x, (int)m_clipArea.y,
			(int)m_clipArea.w, (int)m_clipArea.h);
	update |= 2;
}

void WView::SetScreenCenter(float cx, float cy)
{
	if (cx != m_center_x || cy != m_center_y)
	{
		update |= 2;
		m_center_x = cx;
		m_center_y = cy;
	}
}

void WView::UpdateCamera(void)
{
	(this->*ms_fnUpdateCamera[m_projMode])();
}

void WView::DrawIndexedTriangles(WTVertex* pv, int nv, unsigned short* pi,
	int ni, int flag0, int flag1)
{
	if (proj_scale > 1.0f)
		return;
	int f = flag0;
	if (m_bDisableFog)
	{
		f = (int)((unsigned int)f & 0xf7ffffff);
	}
	DrawIndexedTrianglesDirect(pv, nv, pi, ni, f, flag1);
	if ((flag0 & 0x400000) != 0)
	{
		g_foffset += ni;
	}
}

void WView::DrawIndexedTrianglesDirect(WTVertex* pv, int nv, unsigned short* pi,
	int ni, int flag0, int flag1)
{
	WVideoDev* video = GetVideoDevice();
	if (video == 0)
	{
		return;
	}
	if ((flag0 & 0x400000) != 0)
	{
		for (int i = 0; i < nv; ++i)
		{
			pv[i].vz = TransformZ(pv[i].sx, pv[i].sy, pv[i].sz, invcamera);
		}
	}
	CheckViewAndProjTransformUpdateToVideo();
	CheckReflectiveAndConvertCullFlag(flag1);
	video->DrawIndexedTriangles(pv, nv, pi, ni, flag0, flag1);
}

void WView::SetFOV_Unmodified(float fov)
{
	update |= 2;
	FOV = fov;
}

void WView::GetScreen2World(float sx, float sy, WVector& pos, WVector& dir)
{
	(this->*ms_fnGetScreen2World[m_projMode])(sx, sy, pos, dir);
}

void WView::DrawPolygonFan(WTVertex** ppv, int flag0, int flag1, bool bEffect)
{
	if (proj_scale > 1.0f)
		return;
	{
		WVideoDev* video = GetVideoDevice();
		if ((flag0 & 0x4000000) != 0)
		{
			if (ppv[2] != 0)
			{
				float dx = ppv[0]->sx - ppv[1]->sx;
				float dy = ppv[0]->sy - ppv[1]->sy;
				if (dx <= 0.001f && dx >= -0.001f && dy <= 0.001f &&
					dy >= -0.001f)
				{
					ppv[1] = ppv[2];
				}
			}
			if ((flag0 & 0x400000) != 0)
			{
				int i = 0;
				if (bEffect)
				{
					for (; ppv[i]; ++i)
					{
						ppv[i]->vz = -g_HUGE;
					}
				}
				else
				{
					for (; ppv[i]; ++i)
					{
						ppv[i]->vz = TransformZ(ppv[i]->sx, ppv[i]->sy,
							ppv[i]->sz, invcamera);
					}
				}
			}
			CheckViewAndProjTransformUpdateToVideo();
			CheckReflectiveAndConvertCullFlag(flag1);
			video->DrawPolygonFan(ppv,
				m_bDisableFog ? flag0 & 0xf7ffffff : flag0, 2, flag1,
				bEffect ? 4 : 2);
			return;
		}
		int n = 3;
		while (ppv[n] != 0)
		{
			++n;
		}
		if ((flag0 & 0x400000) != 0)
		{
			int i = 0;
			if (bEffect)
			{
				for (; ppv[i]; ++i)
				{
					ppv[i]->vz = -g_HUGE;
				}
			}
			else
			{
				for (; ppv[i]; ++i)
				{
					ppv[i]->vz = TransformZ(ppv[i]->sx, ppv[i]->sy, ppv[i]->sz,
						invcamera);
				}
			}
		}

		if (m_xNeedToUpdateViewTransfToVideo)
		{
			m_xNeedToUpdateViewTransfToVideo = false;
			GetVideoDevice()->xSetTransform(WVDTS_VIEW, m_xViewState.xmView);
		}
		if (m_xNeedToUpdateProjTransfToVideo)
		{
			m_xNeedToUpdateProjTransfToVideo = false;
			GetVideoDevice()->xSetTransform(WVDTS_PROJECTION,
				m_xViewState.xmProj);
		}

		CheckReflectiveAndConvertCullFlag(flag1);
		video->DrawPolygonFan(ppv, m_bDisableFog ? flag0 & 0xf7ffffff : flag0,
			n, flag1, bEffect ? 4 : 2);
	}
}

void WView::Flush(unsigned long flag)
{
	GetVideoDevice()->Command(W_VDEV_FLUSH, flag, 0);
	g_foffset = 0;
}

void WView::xSetLight(unsigned long, const LightSet& light)
{
	memcpy(&m_xViewState.xLight, &light, sizeof(LightSet));
}

LightSet* WView::xGetLight(void)
{
	return &m_xViewState.xLight;
}

void WView::CheckViewAndProjTransformUpdateToVideo(void)
{
	if (m_xNeedToUpdateViewTransfToVideo)
	{
		m_xNeedToUpdateViewTransfToVideo = false;
		GetVideoDevice()->xSetTransform(WVDTS_VIEW, m_xViewState.xmView);
	}
	if (m_xNeedToUpdateProjTransfToVideo)
	{
		m_xNeedToUpdateProjTransfToVideo = false;
		GetVideoDevice()->xSetTransform(WVDTS_PROJECTION, m_xViewState.xmProj);
	}
}

void WView::xScaleProjMat(int index, int div)
{
	float d = (float)div;
	proj_scale = d;
	float step = 1.0f / d;
	float l = (float)(index % div) * step - 0.5f;
	left = l;
	right = l + step;
	float t = (float)(index / div) * step - 0.5f;
	top = t;
	bottom = t + step;
	update |= 2;
}

void WView::xDrawIndexedTriangles(const WxBatchState& state)
{
	WxBatchState s;
	s = state;
	if (m_bDisableFog)
	{
		s.xiFlag0 = (int)((unsigned int)s.xiFlag0 & 0xf7ffffff);
	}
	CheckReflectiveAndConvertCullFlag(s.xiFlag1);
	GetVideoDevice()->xDrawIndexedTriangles(m_xViewState, s);
}

void WView::SetScale(float scale)
{
	update |= 2;
	m_scale = scale;
}

void WView::ClipPlane(WTVertex* out, const WTVertex* a, const WTVertex* b, int,
	const WPlane& plane)
{
	WVector va;
	va = WVector(a->sx, a->sy, a->sz);
	WVector vb;
	vb = WVector(b->sx, b->sy, b->sz);
	float t1 = va * plane.normal + plane.dis;
	float t2 = vb * plane.normal + plane.dis;
	float t = t1 / (t1 - t2);
	Projection2_Parallel(out, (1.0f - t) * va + vb * t);
	out->tu = (b->tu - a->tu) * t + a->tu;
	out->tv = (b->tv - a->tv) * t + a->tv;
	out->lu = (b->lu - a->lu) * a->lu * t;
	out->lv = (b->lv - a->lv) * a->lv * t;
	out->diffuse =
		ULblend(a->diffuse, b->diffuse, (unsigned char)(int)(t * 256.0f));
}

void WView::SetProjectionMode(PROJECTION_MODE mode)
{
	if (mode != m_projMode)
	{
		m_projMode = mode;
		m_xViewState.xmProj = WMatrix4::ZERO;
		update |= 2;
	}
}

WVector WView::Projection(const WVector& v)
{
	return (this->*ms_fnProjection[m_projMode])(v);
}

void WView::Projection2(WTVertex* out, const WVector& v)
{
	(this->*ms_fnProjection2[m_projMode])(out, v);
}

WMatrix __cdecl MakeCameraMatrix(const WVector& eye, const WVector& at)
{
	WMatrix mat;
	mat.Reset();
	mat.za = (at - eye);
	mat.za.Normalize();

	if (WisEqual(Abs(mat.za.y), 1.0f, g_EPSILON))
	{
		mat.ya = WCrossProduct(mat.za, WVector::UNIT_POS_X);
		mat.xa = WCrossProduct(mat.ya, mat.za);
	}
	else
	{
		mat.ya = WVector::UNIT_POS_Y;
		mat.xa = WCrossProduct(mat.ya, mat.za);
		mat.xa.Normalize();
		mat.ya = WCrossProduct(mat.za, mat.xa);
		mat.ya.Normalize();
	}
	mat.pivot = eye;
	return mat;
}

WVector WView::Projection_Perspective(const WVector& v)
{
	WVector t = v * matrix;
	float w = 1.0f / t.z;
	WVector r;
	r.x = t.x * w + m_center_x;
	r.y = t.y * w + m_center_y;
	r.z = clip_scale_z - w;
	return r;
}

WVector WView::Projection_Parallel(const WVector& v)
{
	WVector t = v * matrix;
	WVector r;
	r.x = t.x + m_center_x;
	r.y = t.y + m_center_y;
	r.z = (v.z - clip_near) / (clip_far - clip_near);
	return r;
}

void WView::Projection2_Perspective(WTVertex* out, const WVector& v)
{
	WVector t = v * matrix;
	float w = 1.0f / t.z;
	out->sx = t.x * w + m_center_x;
	out->sy = t.y * w + m_center_y;
	out->sz = clip_scale_z - w;
	out->rhw = w;
}

void WView::Projection2_Parallel(WTVertex* out, const WVector& v)
{
	WVector t = v * matrix;
	out->sx = t.x + m_center_x;
	out->sy = t.y + m_center_y;
	out->sz = (v.z - clip_near) / (clip_far - clip_near);
	out->rhw = 1.0f;
}

void WView::UpdateCamera_Perspective(void)
{
	if (update != 0)
	{
		camera = lastcam;
		scalex = SCREEN_XS * 0.5f;
		scaley = SCREEN_YS * 0.5f;
		float tan[2];
		tan[0] = tanf(FOV * 0.5f);
		tan[1] = GetRatio() * tan[0];
		float safetan[2];
		safetan[0] = (scalex - 4.0f) / (scalex / tan[0]);
		safetan[1] = (scaley - 4.0f) / (scaley / tan[1]);
		invcamera = ~camera;
		matrix = invcamera;
		WVector scale(scalex / tan[0], -(scaley / tan[1]), 1.0f);
		scale /= clip_near_scale;
		matrix.xx = scale.x * matrix.xx;
		matrix.xy = scale.x * matrix.xy;
		matrix.xz = scale.x * matrix.xz;
		matrix.xm = scale.x * matrix.xm;
		matrix.yx = scale.y * matrix.yx;
		matrix.yy = scale.y * matrix.yy;
		matrix.yz = scale.y * matrix.yz;
		matrix.ym = scale.y * matrix.ym;
		matrix.zx = scale.z * matrix.zx;
		matrix.zy = scale.z * matrix.zy;
		matrix.zz = scale.z * matrix.zz;
		matrix.zm = scale.z * matrix.zm;

		frustumSafe[0] = frustum[0] =
			WPlane(RotVec(WVector::UNIT_NEG_Z, camera),
				camera.pivot + camera.za * GetClipNearValue());
		frustumSafe[1] = frustum[1] =
			WPlane(RotVec(WVector::UNIT_POS_Z, camera),
				camera.pivot + camera.za * GetClipFarValue());

		frustum[2] =
			WPlane(RotVec(WVector(-1.0f, 0.0f, -tan[0]), camera).Normalize(),
				camera.pivot);
		frustumSafe[2] = WPlane(
			RotVec(WVector(-1.0f, 0.0f, -safetan[0]), camera).Normalize(),
			camera.pivot);
		frustum[3] =
			WPlane(RotVec(WVector(1.0f, 0.0f, -tan[0]), camera).Normalize(),
				camera.pivot);
		frustumSafe[3] =
			WPlane(RotVec(WVector(1.0f, 0.0f, -safetan[0]), camera).Normalize(),
				camera.pivot);
		frustum[4] =
			WPlane(RotVec(WVector(0.0f, 1.0f, -tan[1]), camera).Normalize(),
				camera.pivot);
		frustumSafe[4] =
			WPlane(RotVec(WVector(0.0f, 1.0f, -safetan[1]), camera).Normalize(),
				camera.pivot);
		frustum[5] =
			WPlane(RotVec(WVector(0.0f, -1.0f, -tan[1]), camera).Normalize(),
				camera.pivot);
		frustumSafe[5] = WPlane(
			RotVec(WVector(0.0f, -1.0f, -safetan[1]), camera).Normalize(),
			camera.pivot);

		UpdateViewTransform();
		UpdateProjectionTransform_Perspective();
	}
}

void WView::UpdateCamera_Parallel(void)
{
	if (update != 0)
	{
		camera = lastcam;
		invcamera = ~camera;
		scalex = SCREEN_XS * 0.5f;
		scaley = SCREEN_YS * 0.5f;
		matrix = invcamera;
		float scale = scalex / GetScale() * 2.0f;
		matrix.xx = scale * matrix.xx;
		matrix.xy = scale * matrix.xy;
		matrix.xz = scale * matrix.xz;
		matrix.xm = scale * matrix.xm;
		matrix.yx *= -scale;
		matrix.yy *= -scale;
		matrix.yz *= -scale;
		matrix.ym *= -scale;
		WVector2D s(GetScale() * 0.5f, GetRatio() * GetScale() * 0.5f);
		frustumSafe[0] = frustum[0] =
			WPlane(RotVec(WVector::UNIT_NEG_Z, camera),
				camera.pivot + camera.za * clip_near);
		frustumSafe[1] = frustum[1] =
			WPlane(RotVec(WVector::UNIT_POS_Z, camera),
				camera.pivot + camera.za * clip_far);
		frustumSafe[2] = frustum[2] =
			WPlane(RotVec(WVector::UNIT_NEG_X, camera),
				camera.pivot - camera.xa * s.x);
		frustumSafe[3] = frustum[3] =
			WPlane(RotVec(WVector::UNIT_POS_X, camera),
				camera.pivot + camera.xa * s.x);
		frustumSafe[4] = frustum[4] =
			WPlane(RotVec(WVector::UNIT_POS_Y, camera),
				camera.pivot + camera.ya * s.y);
		frustumSafe[5] = frustum[5] =
			WPlane(RotVec(WVector::UNIT_NEG_Y, camera),
				camera.pivot - camera.ya * s.y);

		frustumByCam[0] = WPlane(WVector::UNIT_NEG_Z, 0.0f);
		frustumByCam[1] = WPlane(WVector::UNIT_POS_Z,
			WVector(0.0f, 0.0f, clip_far - clip_near));
		frustumByCam[2] =
			WPlane(WVector::UNIT_NEG_X, WVector(-scalex, 0.0f, 0.0f));
		frustumByCam[3] =
			WPlane(WVector::UNIT_POS_X, WVector(scalex, 0.0f, 0.0f));
		frustumByCam[4] =
			WPlane(WVector::UNIT_POS_Y, WVector(0.0f, scaley, 0.0f));
		frustumByCam[5] =
			WPlane(WVector::UNIT_NEG_Y, WVector(0.0f, -scaley, 0.0f));

		UpdateViewTransform();
		UpdateProjectionTransform_Parallel();
	}
}

void WView::GetScreen2World_Perspective(float sx, float sy, WVector& pos,
	WVector& dir)
{
	dir.z = clip_far;
	float t = tanf(FOV * 0.5f);
	float hx = SCREEN_XS * 0.5f;
	float k = (t * dir.z) / hx;
	dir.x = (sx - hx) * k;
	dir.y = (sy - SCREEN_YS * 0.5f) * k * -1.0f;
	dir = RotVec(dir, camera);
	pos = camera.pivot;
}

void WView::GetScreen2World_Parallel(float sx, float sy, WVector& pos,
	WVector& dir)
{
	WVector v;
	v.x = (sx - SCREEN_XS * 0.5f) / SCREEN_XS * GetScale();
	v.y = (SCREEN_YS * 0.5f - sy) / SCREEN_XS * GetScale();
	v.z = clip_far;
	v = v * camera;
	pos = v - camera.za * clip_far;
	dir = v - pos;
}

void WView::UpdateProjectionTransform_Perspective(void)
{
	if (!(update & 2))
		return;
	update &= ~2;
	float halfw = GetWidth() * 0.5f;
	float halfh = GetHeight() * 0.5f;
	float clipl = (m_clipArea.x - halfw) / GetWidth();
	float clipr = (m_clipArea.x + m_clipArea.w - halfw) / GetWidth();
	float clipt = (m_clipArea.y - halfh) / GetHeight();
	float clipb = (m_clipArea.y + m_clipArea.h - halfh) / GetHeight();
	float fw = (proj_scale / tanf(FOV * 0.5f)) * (GetWidth() / m_clipArea.w);
	float fh = fw / GetRatio() * (GetHeight() / m_clipArea.h) /
		(GetWidth() / m_clipArea.w);
	m_xViewState.xmProj.xx = fw;
	m_xViewState.xmProj.yy = fh;
	m_xViewState.xmProj.zz = clip_scale_z;
	m_xViewState.xmProj.zm = -clip_near_scale;
	m_xViewState.xmProj.wz = 1.0f;
	m_xViewState.xmProj.wm = 0.0f;
	m_xViewState.xmProj.xz = (left + right) / (left - right) +
		((m_center_x - halfw) / halfw - (clipr + clipl)) / (clipr - clipl);
	m_xViewState.xmProj.yz = (top + bottom) / (bottom - top) -
		((m_center_y - halfh) / halfh - (clipb + clipt)) / (clipb - clipt);
	m_xNeedToUpdateProjTransfToVideo = true;
}

void WView::UpdateProjectionTransform_Parallel(void)
{
	if (!(update & 2))
		return;
	update &= ~2;
	float halfw = GetWidth() * 0.5f;
	float halfh = GetHeight() * 0.5f;
	float clipl = (m_clipArea.x - halfw) / GetWidth();
	float clipr = (m_clipArea.x + m_clipArea.w - halfw) / GetWidth();
	float clipt = (m_clipArea.y - halfh) / GetHeight();
	float clipb = (m_clipArea.y + m_clipArea.h - halfh) / GetHeight();
	float fw = 2.0f * proj_scale / GetScale() * (GetWidth() / m_clipArea.w);
	float fh = fw / GetRatio() * (GetHeight() / m_clipArea.h) /
		(GetWidth() / m_clipArea.w);
	m_xViewState.xmProj.xx = fw;
	m_xViewState.xmProj.yy = fh;
	m_xViewState.xmProj.zz = 1.0f / (clip_far - clip_near);
	m_xViewState.xmProj.zm = -clip_near * m_xViewState.xmProj.zz;
	m_xViewState.xmProj.wz = 0.0f;
	m_xViewState.xmProj.wm = 1.0f;
	m_xViewState.xmProj.xz = (left + right) / (left - right) +
		((m_center_x - halfw) / halfw - (clipr + clipl)) / (clipr - clipl);
	m_xViewState.xmProj.yz = (top + bottom) / (bottom - top) -
		((m_center_y - halfh) / halfh - (clipb + clipt)) / (clipb - clipt);
	m_xNeedToUpdateProjTransfToVideo = true;
}

void WView::UpdateViewTransform(void)
{
	if ((update & 1) != 0)
	{
		update &= ~1;
		SetWMatrix4FromWMatrix(m_xViewState.xmView, invcamera);
		m_xNeedToUpdateViewTransfToVideo = true;
	}
}

WVector (WView::* WView::ms_fnProjection[2])(
	const WVector&) = { &WView::Projection_Perspective,
	&WView::Projection_Parallel };
void (WView::* WView::ms_fnProjection2[2])(WTVertex*,
	const WVector&) = { &WView::Projection2_Perspective,
	&WView::Projection2_Parallel };
void (WView::* WView::ms_fnUpdateProjectionTransform[2])(
	void) = { &WView::UpdateProjectionTransform_Perspective,
	&WView::UpdateProjectionTransform_Parallel };
void (WView::* WView::ms_fnUpdateCamera[2])(
	void) = { &WView::UpdateCamera_Perspective, &WView::UpdateCamera_Parallel };
void (WView::* WView::ms_fnGetScreen2World[2])(float, float, WVector&,
	WVector&) = { &WView::GetScreen2World_Perspective,
	&WView::GetScreen2World_Parallel };
const WMatrix4 WView::ms_uvConvMat(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f);
