#pragma once
#include "wview.h"
#include "wvideo.h"
#include "wpuppet.h"

class WTextureView : public WView
{
public:
	struct TextureParam
	{
		struct RtTexInfo
		{
			ulong m_texStyle;
			WRenderToTextureSizeInfo m_sizeInfo;
			bool m_needToClear;
			ulong m_clearClr;

			RtTexInfo();
			void Reset();
			void Set(ulong style, const WRenderToTextureSizeInfo& sizeInfo,
				bool needToClear, ulong clearColor);
		};

		struct DepthSurfInfo
		{
			WRenderToTextureParam::DepthSurfInfo::SurfaceUsage m_surfUsage;
			WRenderToTextureSizeInfo m_sizeInfo;
			bool m_needToClear;
			float m_clearZ;

			DepthSurfInfo();
			void Reset();
			void Set(WRenderToTextureParam::DepthSurfInfo::SurfaceUsage usage,
				const WRenderToTextureSizeInfo& sizeInfo, bool needToClear,
				float clearZ);
		};

		unsigned m_numRts;
		RtTexInfo m_rtTexInfo[2];
		DepthSurfInfo m_depthSurfInfo;

		TextureParam();
		void Reset();
	};

	WTextureView();
	virtual ~WTextureView();

	void SetTextureParam(const TextureParam& param);
	int GetTexture(unsigned index) const;
	void Begin();
	void End();

protected:
	void CreateTexture();

	TextureParam m_texParam;
	WRenderToTextureParam m_r2tParam;
	bool m_r2tBegun;
};

class WTextureImposterView : public WTextureView
{
public:
	WTextureImposterView(WPuppet* pet);

	bool NeedToUpdate(const WMatrix& cam);

private:
	WPuppet* m_target;
	float m_tolerance;
};
