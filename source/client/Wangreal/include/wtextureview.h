#pragma once
#include "wview.h"
#include "wvideo.h"
#include "wpuppet.h"

class WTextureView : public WView
{
	friend class WShadowViewHw;

public:
	struct TextureParam
	{
		struct RtTexInfo
		{
			ulong m_texStyle;
			WRenderToTextureSizeInfo m_sizeInfo;
			bool m_needToClear;
			ulong m_clearClr;

			RtTexInfo() { Reset(); }
			void Reset()
			{
				m_texStyle = 0;
				m_sizeInfo.Reset();
				m_needToClear = false;
				m_clearClr = 0;
			}
			void Set(ulong style, const WRenderToTextureSizeInfo& sizeInfo,
				bool needToClear, ulong clearColor)
			{
				m_texStyle = style;
				m_sizeInfo = sizeInfo;
				m_needToClear = needToClear;
				m_clearClr = clearColor;
			}
		};

		struct DepthSurfInfo
		{
			WRenderToTextureParam::DepthSurfInfo::SurfaceUsage m_surfUsage;
			WRenderToTextureSizeInfo m_sizeInfo;
			bool m_needToClear;
			float m_clearZ;

			DepthSurfInfo() { Reset(); }
			void Reset()
			{
				m_surfUsage =
					WRenderToTextureParam::DepthSurfInfo::USE_NOT_NEEDED;
				m_sizeInfo.Reset();
				m_needToClear = false;
				m_clearZ = 1.0f;
			}
			void Set(WRenderToTextureParam::DepthSurfInfo::SurfaceUsage usage,
				const WRenderToTextureSizeInfo& sizeInfo, bool needToClear,
				float clearZ)
			{
				m_surfUsage = usage;
				m_sizeInfo = sizeInfo;
				m_needToClear = needToClear;
				m_clearZ = clearZ;
			}
		};

		unsigned m_numRts;
		RtTexInfo m_rtTexInfo[2];
		DepthSurfInfo m_depthSurfInfo;

		TextureParam() { Reset(); }
		void Reset()
		{
			m_numRts = 0;
			for (unsigned i = 0; i < 2; ++i)
				m_rtTexInfo[i].Reset();
			m_depthSurfInfo.Reset();
		}
	};

	WTextureView();
	virtual ~WTextureView();

	void SetTextureParam(const TextureParam& param);
	int GetTexture(unsigned index) const
	{
		return m_r2tParam.GetTexture(index);
	}
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
