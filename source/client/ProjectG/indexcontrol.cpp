#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "indexcontrol.h"

extern Fresh* g_pFresh;

CIndexControl::CIndexControl()
	: m_indexRect(0.0f, 0.0f, 30.0f, 10.0f)
{
	ClearVariables();
}

CIndexControl::~CIndexControl()
{
	if (m_pIndexRect)
	{
		delete[] m_pIndexRect;
		m_pIndexRect = NULL;
	}
}

void CIndexControl::ClearVariables()
{
	m_bInit = false;
	m_align = 1;
	m_selColor = 0xffff0000;
	m_overColor = 0xffaa1111;
	m_sepColor = 0xffaaaaaa;
	m_totalIndex = 0;
	m_totalPage = 0;
	m_curIndexCount = 0;
	m_indexPerPage = 0;
	m_curPage = 0;
	m_selIndex = 0;
	m_cellRect = m_indexRect;
	m_pIndexRect = NULL;
	m_pos = WPoint(0.0f, 0.0f);
}

bool CIndexControl::Init(unsigned long totalIndex, unsigned long indexPerPage)
{
	bool ret = false;

	if (totalIndex > 0 && indexPerPage > 0)
	{
		m_totalIndex = totalIndex;
		m_indexPerPage = indexPerPage;

		m_totalPage = totalIndex / indexPerPage;

		if (totalIndex % indexPerPage)
		{
			m_totalPage++;
		}

		ret = BuildPage(0);

		if (ret == true)
		{
			m_bInit = true;
		}
	}
	return ret;
}

bool CIndexControl::BuildPage(unsigned long page)
{
	bool ret = false;

	if (page < m_totalPage)
	{
		m_curPage = page;

		if (m_totalIndex - (page * m_indexPerPage) >= m_indexPerPage)
		{
			m_curIndexCount = m_indexPerPage;
		}
		else
		{
			if (m_totalIndex % m_indexPerPage)
			{
				m_curIndexCount = m_totalIndex % m_indexPerPage;
			}
		}

		if (m_curIndexCount <= 0)
			goto done;

		ret = true;

		if (m_pIndexRect)
		{
			delete[] m_pIndexRect;
			m_pIndexRect = NULL;
		}

		m_pIndexRect = new WRect[m_curIndexCount];

		if (m_pIndexRect)
		{
			memset(m_pIndexRect, 0, sizeof(WRect) * m_curIndexCount);

			for (int i = 0; i < m_curIndexCount; i++)
			{
				WRect cell = m_cellRect;
				WRect& rect = m_pIndexRect[i];

				rect.x = i * cell.w;
				rect.y = cell.y;
				rect.w = rect.x + cell.w;
				rect.h = cell.h;
			}

			m_selIndex = 0;
		}
		else
			ret = false;
	}

done:
	return ret;
}

void CIndexControl::Draw(WPoint& pos)
{
	if (!m_bInit)
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();

	if (!gdi)
		return;

	m_pos = pos;

	float x = m_pos.x;
	float y = m_pos.y;

	float width = m_curIndexCount * m_indexRect.w;

	switch (m_align)
	{
	case 1:
	{
		x -= width * 0.5f;
	}
	break;

	case 2:
	{
		x -= width;
	}
	break;
	}

	for (unsigned long i = 0; i < m_curIndexCount; i++)
	{
		WRect rect = m_pIndexRect[i];
		rect.x += x;
		rect.y += y;
		rect.w += x;
		rect.h += y;

		if (i)
		{
			gdi->SetTextColor(m_sepColor, 0xffffffff);
			gdi->Print(rect.TopLeft(), 1, "-");
		}

		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

		if (mouse.x >= rect.x && mouse.x <= rect.w && mouse.y >= rect.y &&
			mouse.y <= rect.h)
		{
			m_overIndex = i;
		}
		else
		{
			m_overIndex = m_selIndex;
		}

		rect.x += m_indexRect.w * 0.5f;

		if (i == m_selIndex)
		{
			gdi->SetTextColor(m_selColor, 0xffffffff);
			gdi->SetTextStyle(1);
		}
		else if (i == m_overIndex)
		{
			gdi->SetTextColor(m_overColor, 0xffffffff);
			gdi->SetTextStyle(1);
		}
		else
		{
			gdi->SetTextColor(0xff000000, 0xffffffff);
			gdi->SetTextStyle(0);
		}

		gdi->Print(rect.TopLeft(), 1, "%d", m_indexPerPage * m_curPage + i + 1);

		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(0);
	}
}

bool CIndexControl::Click()
{
	bool ret = false;

	float x = m_pos.x;
	float y = m_pos.y;
	float width = m_curIndexCount * m_indexRect.w;

	switch (m_align)
	{
	case 1:
	{
		x -= width * 0.5f;
	}
	break;

	case 2:
	{
		x -= width;
	}
	break;
	}

	for (unsigned long i = 0; i < m_curIndexCount; i++)
	{
		WRect rect = m_pIndexRect[i];
		rect.x += x;
		rect.y += y;
		rect.w += x;
		rect.h += y;

		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

		if (mouse.x >= rect.x && mouse.x <= rect.w && mouse.y >= rect.y &&
			mouse.y <= rect.h)
		{
			if (m_selIndex != i)
			{
				m_selIndex = i;
				ret = true;
				break;
			}
		}
	}

	return ret;
}

bool CIndexControl::SelectIndex(unsigned long index)
{
	bool ret = false;

	if (m_selIndex != index)
	{
		m_selIndex = index;
		ret = true;
	}

	return ret;
}

bool CIndexControl::NextPage()
{
	bool ret = false;

	if (m_totalPage > m_curPage + 1)
	{
		m_curPage++;
		ret = true;

		BuildPage(m_curPage);
	}

	return ret;
}

bool CIndexControl::PrevPage()
{
	bool ret = false;

	if (m_curPage > 0)
	{
		m_curPage--;
		ret = true;

		BuildPage(m_curPage);
	}

	return ret;
}

void CIndexControl::SetSelectedNumber(unsigned long number)
{
	unsigned long page;
	unsigned long index;

	if (number > 0)
	{
		number--;

		if (number >= m_indexPerPage)
		{
			page = number / m_indexPerPage;
			index = number % m_indexPerPage;
		}
		else
		{
			page = 0;
			index = number;
		}

		BuildPage(page);
		SelectIndex(index);
	}
}

unsigned long CIndexControl::GetSelectedNumber() const
{
	return (m_indexPerPage * m_curPage) + m_selIndex + 1;
}

unsigned long CIndexControl::GetSelectedIndex() const
{
	return m_selIndex;
}

unsigned long CIndexControl::GetCurIndexCount() const
{
	return m_curIndexCount;
}

unsigned long CIndexControl::GetTotalPage() const
{
	return m_totalPage;
}
