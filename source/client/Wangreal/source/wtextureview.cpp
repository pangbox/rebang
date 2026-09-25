#include "wmemblock.inl"
#include "wtextureview.h"
#include "bitmap.h"
#include "gamath.h"

inline WRenderToTextureParam::WRenderToTextureParam()
{
}

WTextureView::WTextureView()
{
	SetCamera(WMatrix::IDENTITY);
	m_r2tBegun = false;
	m_bDisableFog = true;
}

__forceinline WView::~WView()
{
}

WTextureView::~WTextureView()
{
	for (int i = 0; i < 2; ++i)
	{
		if (GetResrcManager() && (&m_r2tParam.m_rtTexInfo0)[i].m_hTex)
		{
			GetResrcManager()->Release((&m_r2tParam.m_rtTexInfo0)[i].m_hTex);
			(&m_r2tParam.m_rtTexInfo0)[i].m_hTex = 0;
		}
	}
	if (GetResrcManager() && m_r2tParam.m_depthSurfInfo.m_hTex)
	{
		GetResrcManager()->Release(m_r2tParam.m_depthSurfInfo.m_hTex);
		m_r2tParam.m_depthSurfInfo.m_hTex = 0;
	}
}

void WTextureView::SetTextureParam(const TextureParam& param)
{
	m_texParam = param;
}

void WTextureView::Begin()
{
	WVideoDev* video = GetVideoDevice();
	if (!m_texParam.m_rtTexInfo[0].m_sizeInfo.m_isAbsolute)
	{
		float width =
			video->GetWidth() * m_texParam.m_rtTexInfo[0].m_sizeInfo.m_width +
			0.5f;
		width = floor(width);
		float height =
			video->GetHeight() * m_texParam.m_rtTexInfo[0].m_sizeInfo.m_width +
			0.5f;
		height = floor(height);
		if (GetWidth() != width || GetHeight() != height)
			SetViewport(width, height);
	}
	if ((&m_r2tParam.m_rtTexInfo0)[0].m_hTex == 0)
		CreateTexture();
	video->EndScene();
	m_r2tBegun = false;
	if ((&m_r2tParam.m_rtTexInfo0)[0].m_hTex > 0)
		m_r2tBegun = video->BeginRenderToTexture(&m_r2tParam);
	if (m_r2tBegun)
		m_r2tBegun = video->BeginScene();
}

void WTextureView::End()
{
	WVideoDev* video = GetVideoDevice();
	if (m_r2tBegun)
	{
		video->EndScene();
		video->EndRenderToTexture(&m_r2tParam);
	}
	video->BeginScene();
	m_r2tBegun = false;
}

void WTextureView::CreateTexture()
{
	BITMAPINFO bi;
	WVideoDev* video = GetVideoDevice();
	int width, height;

	if (video->GetDeviceState() == W_VDEVSTATE_LOST)
		return;

	Bitmap bitmap;
	unsigned i;
	for (i = 0; i < m_texParam.m_numRts; ++i)
	{
		if (m_texParam.m_rtTexInfo[i].m_sizeInfo.m_isAbsolute)
		{
			width = (int)GetWidth();
			height = (int)GetHeight();
		}
		else
		{
			width = (int)floor(video->GetWidth() *
					m_texParam.m_rtTexInfo[i].m_sizeInfo.m_width +
				0.5f);
			height = (int)floor(video->GetHeight() *
					m_texParam.m_rtTexInfo[i].m_sizeInfo.m_height +
				0.5f);
		}
		bi.bmiHeader.biWidth = width;
		bi.bmiHeader.biHeight = height;
		bi.bmiHeader.biBitCount = 24;
		bitmap.SetBITMAPINFO(&bi, 0);
		(&m_r2tParam.m_rtTexInfo0)[i].m_hTex = GetResrcManager()->UploadTexture(
			0, &bitmap, m_texParam.m_rtTexInfo[i].m_texStyle, 0);
		if ((&m_r2tParam.m_rtTexInfo0)[i].m_hTex > 0)
			video->SetRenderTargetSizeInfo((&m_r2tParam.m_rtTexInfo0)[i].m_hTex,
				m_texParam.m_rtTexInfo[i].m_sizeInfo);
		(&m_r2tParam.m_rtTexInfo0)[i].m_needToClear =
			m_texParam.m_rtTexInfo[i].m_needToClear;
		(&m_r2tParam.m_rtTexInfo0)[i].m_clearClr =
			m_texParam.m_rtTexInfo[i].m_clearClr;
	}

	if (m_texParam.m_depthSurfInfo.m_surfUsage ==
		WRenderToTextureParam::DepthSurfInfo::USE_EXCLUSIVE_SURFACE)
	{
		TextureParam::DepthSurfInfo& requested = m_texParam.m_depthSurfInfo;
		if (requested.m_sizeInfo.m_isAbsolute)
		{
			width = (int)GetWidth();
			height = (int)GetHeight();
		}
		else
		{
			width = (int)floor(
				video->GetWidth() * requested.m_sizeInfo.m_width + 0.5f);
			height = (int)floor(
				video->GetHeight() * requested.m_sizeInfo.m_height + 0.5f);
		}
		bi.bmiHeader.biWidth = width;
		bi.bmiHeader.biHeight = height;
		bi.bmiHeader.biBitCount = 24;
		bitmap.SetBITMAPINFO(&bi, 0);
		m_r2tParam.m_depthSurfInfo.m_hTex =
			GetResrcManager()->UploadTexture(0, &bitmap, 0x8000, 0);
		if (m_r2tParam.m_depthSurfInfo.m_hTex > 0)
			video->SetRenderTargetSizeInfo(m_r2tParam.m_depthSurfInfo.m_hTex,
				requested.m_sizeInfo);
	}
	m_r2tParam.m_depthSurfInfo.m_surfUsage =
		m_texParam.m_depthSurfInfo.m_surfUsage;
	m_r2tParam.m_depthSurfInfo.m_needToClear =
		m_texParam.m_depthSurfInfo.m_needToClear;
	m_r2tParam.m_depthSurfInfo.m_clearZ = m_texParam.m_depthSurfInfo.m_clearZ;
}

WTextureImposterView::WTextureImposterView(WPuppet* pet)
	: m_target(pet), m_tolerance(10.0f * g_DEGTORAD)
{
	float w = 16.0f;
	while (true)
	{
		if (w > m_target->GetBoundSphere().radius)
			break;
		w += w;
	}
	SetViewport(w, w);
}

bool WTextureImposterView::NeedToUpdate(const WMatrix& cam)
{
	WMatrix camMat = cam;
	const WSphere& sphere = m_target->GetBoundSphere();
	WVector toTarget = sphere.pos - camMat.pivot;
	camMat.za = toTarget;
	camMat.za.Normalize();

	if ((&m_r2tParam.m_rtTexInfo0)[0].m_hTex > 0 &&
		GetVideoDevice()->IsTextureFilled((&m_r2tParam.m_rtTexInfo0)[0].m_hTex))
	{
		float dist = camMat.za * GetLastCamera().za;
		dist = Abs(dist);
		if (dist > cos(m_tolerance))
			return false;
	}

	camMat.ya = WVector::UNIT_POS_Y;
	camMat.xa = WCrossProduct(camMat.ya, camMat.za);
	camMat.ya = WCrossProduct(camMat.za, camMat.xa);

	float dist = toTarget.Magnitude();
	SetFOV(asinf(sphere.radius / dist) * 1280.0f / 480.0f);
	float nearClip = dist - sphere.radius;
	SetClip(nearClip < 0.0f ? 0.0f : nearClip, dist + sphere.radius, false);
	SetCamera(camMat);
	UpdateCamera();
	return true;
}
