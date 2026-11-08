#pragma once
#include "rtti.h"
#include "frwnd.h"

class Bitmap;
class FrWndManager;
struct FrInputState;

class FrScrollBar : public FrWnd
{
	DECLARE_OBJECT(FrScrollBar)

public:
	FrScrollBar();
	virtual ~FrScrollBar();
	void Init(FrWndManager* pManager, FrWnd* parentWnd, int row,
		int rowCapacity, int colCapacity, bool atLeft);
	void Resize(int rowCapacity, int colCapacity, bool atLeft);
	bool IsBarVisible()
	{
		return m_rowCapacity && m_rowCapacity < GetCurRowCount();
	}
	int GetRowCapacity() { return m_rowCapacity; }
	int GetColCapacity() { return m_colCapacity; }
	int GetItemNum() { return m_itemNum; }
	float GetCurTopRow() { return m_curTopRow; }
	int GetCurTopRow_Int() { return (int)(m_curTopRow + 0.5f); }
	void SetCurTopRow(int row);
	void AddItem();
	void DelItem();
	void ClearItem();
	void ScrollToFirst();
	void ScrollToBottom();
	void ScrollUp(int delta);
	void ScrollDown(int delta);
	void FollowBottom(bool follow) { m_followBottom = follow; }
	void SetGuideVisible(bool visible) { m_showGuide = visible; }

protected:
	virtual void OnDraw();
	virtual void OnResize();
	virtual void OnMouseMove(const WPoint& mousePos);
	virtual bool OnLButtonUp(const WPoint& mousePos);
	virtual bool OnLButtonDown(const WPoint& mousePos);
	virtual void OnWheel(FrInputState& istate);
	int GetCurRowCount()
	{
		return m_colCapacity ? (m_itemNum + m_colCapacity - 1) / m_colCapacity
							 : 0;
	}
	void SetBarHnY();

	float m_barH;
	float m_barY;
	int m_itemNum;
	float m_curTopRow;
	int m_rowCapacity;
	int m_colCapacity;
	float m_dragPrevY;
	bool m_followBottom;
	bool m_showGuide;
	bool m_atLeft;
	const Bitmap* m_frames[3];
	bool m_mouseDown;
	struct Frag
	{
		int width;
		int height;
		const Bitmap* n_img;
		const Bitmap* o_img;
	};
	Frag m_frag[3];
};
