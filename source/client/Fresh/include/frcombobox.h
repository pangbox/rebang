#pragma once
#include <list>
#include <string>
#include <vector>
#include "rtti.h"
#include "frelement.h"
#include "fredit.h"

class Bitmap;
class CChatMsg;
class FrButton;
class FrListBox;
class FrWndManager;

class FrComboBox : public FrEdit
{
	DECLARE_OBJECT(FrComboBox)

public:
	FrComboBox();
	virtual ~FrComboBox();

	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	void ResizeListRect();
	void AddString(const char* string);
	void DelString(int index);
	int FindString(const char* string);
	void ClearList();
	bool IsSelectString(const std::string& compStr);
	void IncreaseMaxListNum(int maxListNum);
	int GetMaxComboListNum() { return m_maxListNum; }
	int GetComboListNum() { return m_comboList.size(); }
	virtual void Enable(bool enable);
	const FrButton* GetButton() const { return m_pButton; }
	const FrListBox* GetListBox() const { return m_pListBox; }

protected:
	virtual void OnKeyFocus(CChatMsg* im);

	int m_maxListNum;
	std::list<FrLine*> m_comboList;
	std::list<FrLine*>::iterator m_selected;
	FrButton* m_pButton;
	FrGuiItem m_buttonInfo;
	FrListBox* m_pListBox;
	FrGuiItem m_listInfo;
	unsigned long m_selectColor;

	void OnUnfold();
	void OnListOwnerDraw(int var1);
	virtual void OnListBtnDown();
	void OnListLostKeyFocus(int var1);

	DECLARE_FRESH_MSGMAP()
};

class FrComboCtlEx : public FrComboBox
{
	DECLARE_OBJECT(FrComboCtlEx)

public:
	enum
	{
		NORMAL,
		OVER,
		MAX
	};

	enum eItemDrawStyle
	{
		IDS_TEXT,
		IDS_IMG
	};

	FrComboCtlEx();
	virtual ~FrComboCtlEx();

	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	void AddString(const char* string);
	void AddTexItem(const char* fileName);
	void SelectItem(int idx);
	void ClearAllListItem();
	unsigned char GetCurrentIdx() { return m_byCurrentIdx; }

private:
	void AlignListItem();

	const Bitmap* m_BitmapSkin[MAX];
	const Bitmap* m_pSelectItem;
	std::vector<const Bitmap*> m_TexItemList;
	eItemDrawStyle m_eDrawStyle;
	WPoint m_ItemOffset;
	bool m_bFold;
	unsigned char m_byCurrentIdx;

protected:
	virtual void OnKeyFocus(CChatMsg* im);
	void OnUnfold();
	void OnUnfoldOwnerDraw(int var1);
	void OnListOwnerDraw(int var1);
	virtual void OnListBtnDown();
	void OnListLostKeyFocus(int var1);

	DECLARE_FRESH_MSGMAP()
};
