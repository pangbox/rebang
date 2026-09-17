template <class T>
struct _Coordinates
{
	T x;
	T y;
};

template <class T>
struct _Rectangle
{
	union
	{
		struct
		{
			_Coordinates<T> tl;
			_Coordinates<T> br;
		};
		struct
		{
			T left;
			T top;
			T right;
			T bottom;
		};
	};

	T Height() const { return bottom - top; }
	T Width() const { return right - left; }
};

struct _RectangleSHORT : public _Rectangle<short>
{
};

class RectCache
{
public:
	struct RectInfo
	{
		_RectangleSHORT rcPixel;
		_RectangleSHORT rcBlock;
	};
	typedef const RectInfo* LPRECTINFO;

	RectCache();
	virtual ~RectCache();
	void Init(int w, int h, int bw, int bh);
	void Clear();
	int Width() const { return m_Width; }
	int Height() const { return m_Height; }
	bool Add(RectInfo& info, int width, int height);

protected:
	enum eCRRes
	{
		eCRRes_Empty,
		eCRRes_RectOnX,
		eCRRes_RectOnY,
		eCRRes_Error
	};

	eCRRes CheckRect(int xInBlock, int yInBlock, int wInBlock, int hInBlock,
		LPRECTINFO& pResInfo, LPRECTINFO& pMinHeightInfo)
	{
		int line = m_BlockCountW * yInBlock;

		for (int x = xInBlock; x < xInBlock + wInBlock; ++x)
		{
			LPRECTINFO pInfo = m_ppTable[line + x];
			if (pInfo)
			{
				if (pInfo->rcBlock.left < 0 ||
					pInfo->rcBlock.left > m_BlockCountW ||
					pInfo->rcBlock.right < 0 ||
					pInfo->rcBlock.right > m_BlockCountW ||
					pInfo->rcBlock.top < 0 ||
					pInfo->rcBlock.top > m_BlockCountH ||
					pInfo->rcBlock.bottom < 0 ||
					pInfo->rcBlock.bottom > m_BlockCountH)
				{
					pResInfo = pInfo;
					return eCRRes_Error;
				}
				if (!pMinHeightInfo ||
					pInfo->rcBlock.bottom < pMinHeightInfo->rcBlock.bottom)
					pMinHeightInfo = pInfo;
				pResInfo = pInfo;
				return eCRRes_RectOnX;
			}
		}

		for (int y = 1; y < hInBlock; ++y)
		{
			line += m_BlockCountW;
			LPRECTINFO pInfo = m_ppTable[line + xInBlock];
			if (pInfo)
			{
				pResInfo = pInfo;
				return eCRRes_RectOnY;
			}
		}

		line = (yInBlock + 1) * m_BlockCountW;
		for (int y = 1; y < hInBlock; ++y)
		{
			LPRECTINFO pInfo = m_ppTable[line + xInBlock + wInBlock - 1];
			if (pInfo)
			{
				if (!pMinHeightInfo ||
					pInfo->rcBlock.bottom < pMinHeightInfo->rcBlock.bottom)
					pMinHeightInfo = pInfo;
				pResInfo = pInfo;
				return eCRRes_RectOnX;
			}
			line += m_BlockCountW;
		}

		pResInfo = 0;
		return eCRRes_Empty;
	}

	int SkipX(int xInBlock, int yInBlock, int xDest, LPRECTINFO& pMinHeightInfo)
	{
		const int line = m_BlockCountW * yInBlock;
		int x = xInBlock;
		while (x < xDest)
		{
			LPRECTINFO pInfo = m_ppTable[x + line];
			if (pInfo)
			{
				if (!pMinHeightInfo ||
					pInfo->rcBlock.bottom < pMinHeightInfo->rcBlock.bottom)
					pMinHeightInfo = pInfo;
				x = pInfo->rcBlock.right;
			}
			else
			{
				++x;
			}
		}
		return x;
	}

	void FillRect(const RectInfo& info)
	{
		int line = info.rcBlock.top * m_BlockCountW;
		for (int x = info.rcBlock.left; x < info.rcBlock.right; ++x)
			m_ppTable[line + x] = &info;

		line += m_BlockCountW;
		short height = info.rcBlock.Height();
		for (int y = 1; y < height; ++y)
		{
			m_ppTable[line + info.rcBlock.left] = &info;
			line += m_BlockCountW;
		}
	}

	int PixelToBlock_W(int w) const
	{
		return (w + m_BlockWidth - 1) / m_BlockWidth;
	}

	int PixelToBlock_H(int h) const
	{
		return (h + m_BlockHeight - 1) / m_BlockHeight;
	}

	int BlockToPixel_X(int x) const { return x * m_BlockWidth; }

	int BlockToPixel_Y(int y) const { return y * m_BlockHeight; }

private:
	int m_Width;
	int m_Height;
	int m_BlockWidth;
	int m_BlockHeight;
	int m_BlockCountW;
	int m_BlockCountH;
	int m_EmptyArea;
	LPRECTINFO* m_ppTable;
};
