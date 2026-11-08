#pragma once
#include <list>
#include <map>
#include "rtti.h"
#include "frwnd.h"
#include "../../Wangreal/include/wtypes.h"

class Bitmap;
class CChatMsg;
class FrGuiItem;
class FrWndManager;
struct FrInputState;

struct FrListItem
{
	void* pData;
	WPoint pos;
	bool underCursor;
	bool selected;
	int no;
	int idx;
	std::list<int> noRBtnIdxList;

	FrListItem()
		: pData(NULL), underCursor(false), selected(false)
	{
		noRBtnIdxList.clear();
	}
	~FrListItem() { noRBtnIdxList.clear(); }
	void AddNoRBtnIdx(int idx) { noRBtnIdxList.push_back(idx); }
};

class FrListBoxGreater
{
public:
	FrListBoxGreater(bool (*compare)(const void*, const void*))
		: m_compare(compare)
	{
	}
	bool operator()(FrListItem* const& left, FrListItem* const& right) const
	{
		return m_compare(left->pData, right->pData);
	}

protected:
	bool (*m_compare)(const void*, const void*);
};

class FrListBox : public FrWnd
{
	DECLARE_OBJECT(FrListBox)

public:
	typedef std::list<FrListItem*> ITEM_LIST;
	ITEM_LIST m_itemList;

	FrListBox();
	virtual ~FrListBox();

	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	void RecalcColCapacity(bool b);
	void SetMultiSelect(bool multi) { m_multiSelect = multi; }
	void UseRightButton(bool use) { m_useRightButton = use; }
	const Bitmap* GetBitmap(const char* name);
	FrListItem* GetItemUnderCursor() { return m_pItemUnderCursor; }
	FrListItem* GetSelected() { return m_pSelected; }
	FrListItem* GetPreviousSelected() { return m_pPreviousSelected; }
	void SelectItem(FrListItem* item, bool b);
	void UnselectItem(FrListItem* item);
	void UnselectAllItem();
	void ToggleItem(FrListItem* item);
	int GetCurrentItemSize(bool b);
	int GetItemWidth() { return m_itemSize.w; }
	int GetItemHeight() { return m_itemSize.h; }
	void SetItemSize(int w, int h)
	{
		m_itemSize.w = w;
		m_itemSize.h = h;
	}
	FrListItem* AddItem(void* data);
	FrListItem* FindItem(FrListItem* item);
	FrListItem* FindItem(void* data);
	FrListItem* FindItem(void* data, int no);
	FrListItem* GetItem(int no);
	void DelItem(FrListItem* item);
	void DelItem(void* data);
	void DelItem(void* data, int no);
	ITEM_LIST::iterator _DelItem(ITEM_LIST::iterator it);
	void ClearItem();
	void SortItem(bool (*compare)(const void*, const void*));
	void RecalcTopBottom();
	void EnableScrollBar(bool enable);
	virtual void SetClientRect(const WRect& rect);
	void EnableRollOver(bool enable);
	bool MoveTopItem(int n);
	void Resize(int w, int h);
	void UseDummy(bool use) { m_useDummy = use; }
	void AddNoRButtonRect(const WRect& rect, int idx);
	void ResetTopRow() { m_prevTopRow = -1; }
	void SetMouseEvent(bool active);
	int GetColCapacity() { return m_colCapacity; }
	template <class T>
	void OrderItem(T function)
	{
		for (ITEM_LIST::iterator it = m_itemList.begin();
			it != m_itemList.end(); ++it)
			function((*it)->pData);
	}

protected:
	virtual void OnDraw();
	virtual void OnProc(const float deltaTime);
	virtual void OnResize();
	virtual void OnMouseMove(const WPoint& point);
	virtual void OnSetCursor(bool active, const WPoint& point);
	virtual bool OnLButtonUp(const WPoint& point);
	virtual bool OnLButtonDown(const WPoint& point);
	virtual bool OnRButtonUp(const WPoint& point);
	virtual bool OnRButtonDown(const WPoint& point);
	virtual void OnDblClick(const WPoint& point);
	virtual void EnableKeyFocus(FrInputState& input);
	virtual void OnKeyFocus(CChatMsg* im);
	void SetCursorToFirstItem();
	void SetCursorToLastItem();

	FrGuiItem* m_pItem;

	struct sSize
	{
		int w;
		int h;
	};

	sSize m_itemSize;
	int m_colCapacity;
	int m_prevTopRow;
	ITEM_LIST::iterator m_itemTop;
	ITEM_LIST::iterator m_itemBottom;
	bool m_multiSelect;
	bool m_useRightButton;
	bool m_rollOver;
	FrListItem* m_pItemUnderCursor;
	FrListItem* m_pSelected;
	FrListItem* m_pPreviousSelected;
	int m_dummyCount;
	bool m_useDummy;
	std::map<int, WRect> m_noRButtonMap;
	const Bitmap* m_listBgImg;

	DECLARE_FRESH_MSGMAP()
};
