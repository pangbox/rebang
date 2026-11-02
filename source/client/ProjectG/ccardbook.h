#pragma once

#include <string.h>
#include <vector>

#include "../../shared/classdefine.h"

struct sCards;
class FrOpenCardPackDlg;
class FrCardUseDlg;
class FrItemAttachCardDlg;
class FrSpCardUseDlg;

extern WResourceManager* g_resrcmng;

class CCardSlot : public FrCmdTarget
{
protected:
	unsigned char m_pushed;
	WRect m_rect;
	FrButton* m_pButton;
	IFF_STRUCT::sCard* m_pCard;
	IFF_ITEM_COMMON* m_pItem;
	sCards* m_pCards;
	WOverlay* m_pOverlay;
	FrOpenCardPackDlg* m_pCardPackDlg;
	FrCardUseDlg* m_pCardDlg;
	FrItemAttachCardDlg* m_pItemAttachCardDlg;
	FrSpCardUseDlg* m_pSpCardDlg;

public:
	CCardSlot();
	virtual ~CCardSlot();
	unsigned char IsPushed() const { return m_pushed; }
	bool BoundControl(FrButton* pButton);
	void SetCard(unsigned long typeId);
	void UseCard();
	void Select(unsigned char state);
	void SetPosition(WRect& rect);
	void Draw(FrGraphicInterface* pGI);

	void Show(bool bShow)
	{
		if (m_pButton)
		{
			if (m_pCard && m_pItem)
			{
				m_pButton->SetVisible(bShow);
			}
		}
	}

	WRect GetPosition() const { return m_rect; }

	sCards* GetCards() { return m_pCards; }

	IFF_STRUCT::sCard* GetCardInfo()
	{
		// NOFORMAT
		return m_pCard;
	}

protected:
	bool OnCardPackDlgResult(int ret, FrForm* pForm);
	bool OnSpCardDlgResult(int ret, FrForm* pForm);
	bool OnItemAttachCardDlgResult(int ret, FrForm* pForm);
	bool OnCardDlgResult(int ret, FrForm* pForm);
};

class ICardBookPage
{
public:
	ICardBookPage()
	{
		m_pSlot = NULL;
		m_pFont = NULL;

		// HACK! Not sure what was here in the original code.
		{
		}
		{
		}
		{
		}
		static char* strBonusNumber = "[font_wind.jpg";

		tagWTITLEFONT tf;
		memset(&tf, 0, sizeof(tf));

		tf.filename = &strBonusNumber;
		tf.fontw = 16;
		tf.fonth = 16;
		tf.numPages = 1;
		tf.texw = 128;
		tf.texh = 64;
		tf.pCharSet = "1234567890ym%-.?/:";
		m_pFont = g_resrcmng->GetTitleFont();
		m_pFont->Create(&tf);
	}

	virtual ~ICardBookPage()
	{
		if (m_pSlot)
		{
			delete[] m_pSlot;
			m_pSlot = NULL;
		}
		m_pFont->Erase();
		if (g_resrcmng && m_pFont)
		{
			g_resrcmng->Release(m_pFont);
			m_pFont = NULL;
		}
	}

	bool BoundSlotControl(unsigned char index, FrButton* pButton)
	{
		bool bRet = false;
		if (m_pSlot)
		{
			bRet = m_pSlot[index].BoundControl(pButton);
		}
		return bRet;
	}

	CCardSlot* GetCardSlot()
	{
		// NOFORMAT
		return m_pSlot;
	}

	virtual bool Init() = 0;
	virtual void Draw(FrGraphicInterface* pGI) = 0;

	virtual void SetCard(unsigned char slot, unsigned long typeId) { }
	virtual void ShowSlotControl(bool bShow) = 0;
	virtual void SelectSlot(unsigned char action, unsigned char slot) = 0;
	virtual bool IsSelectedSlot() = 0;

protected:
	CCardSlot* m_pSlot;
	WTitleFont* m_pFont;
};

class CCardPackPage : public ICardBookPage
{
public:
	CCardPackPage()
	{
		// NOFORMAT
		m_pCardPackList = NULL;
	}

	virtual ~CCardPackPage()
	{
		if (m_pCardPackList)
		{
			delete m_pCardPackList;
			m_pCardPackList = NULL;
		}
	}

	virtual bool Init();
	virtual void Draw(FrGraphicInterface* pGI);
	virtual void SetCard(unsigned char slot, unsigned long typeId);

	std::vector<unsigned long>* GetCardPackList() { return m_pCardPackList; }

	virtual void ShowSlotControl(bool bShow);
	virtual void SelectSlot(unsigned char action, unsigned char slot);

	virtual bool IsSelectedSlot()
	{
		bool bSelected = false;

		for (int i = 0; i < 8; i++)
		{
			if (m_pSlot)
			{
				if (m_pSlot[i].IsPushed() == 1)
				{
					bSelected = true;
					break;
				}
			}
		}

		return bSelected;
	}

protected:
	std::vector<unsigned long>* m_pCardPackList;
};

class CCardPage : public ICardBookPage
{
public:
	CCardPage();
	virtual ~CCardPage();

	virtual bool Init();
	virtual void Draw(FrGraphicInterface* pGI);
	virtual void SetCard(unsigned char slot, unsigned long typeId);
	virtual void ShowSlotControl(bool bShow);
	virtual void SelectSlot(unsigned char action, unsigned char slot);

	std::vector<unsigned long>* GetCardList(unsigned char type);
	unsigned long GetPageCount(unsigned char type);

protected:
	void BuildCardList();
	void PageSeparate(std::vector<unsigned long>*& pList, unsigned char type);

public:
	virtual bool IsSelectedSlot()
	{
		bool bSelected = false;

		for (int i = 0; i < 12; i++)
		{
			if (m_pSlot)
			{
				if (m_pSlot[i].IsPushed() == 1)
				{
					bSelected = true;
					break;
				}
			}
		}

		return bSelected;
	}

	unsigned char m_type;
	unsigned char m_page;
	std::vector<unsigned long>* m_pCardList1;
	std::vector<unsigned long>* m_pCardList2;
	std::vector<unsigned long>* m_pCardList3;
	std::vector<unsigned long>* m_pCardList4;
	const Bitmap* m_pSlotBg;
};

class CCardBook
{
public:
	CCardBook();
	virtual ~CCardBook();

	void Show(bool bShow);
	void ShowSlotControl(bool bShow);
	void Draw();
	void UseCard();
	void CardSlotControl(unsigned char page, unsigned char action,
		unsigned char slot);
	void PageIndexBehavior(const WPoint& pt, unsigned char action);
	void ButtonTabControl(unsigned char tab);
	void BuildCardPackTab();
	void BuildCardTab(unsigned long type);
	void BuildCardPage(unsigned long page);
	void ReBuildCurrentPage();
	void DrawCardPackHelp(unsigned long typeId);

protected:
	void ClearVariables();
	void LoadImages();
	void ShowCardProperty(bool bShow);
	void DrawBackGround(FrGraphicInterface* pGI);
	void DrawCardPack(FrGraphicInterface* pGI);
	void DrawCard(FrGraphicInterface* pGI);
	void DrawPageIndex(FrGraphicInterface* pGI);
	void DrawProperty(FrGraphicInterface* pGI);

public:
	template <class T>
	bool BoundControl(T& pDst, T pSrc)
	{
		pDst = pSrc;

		pDst->SetVisible(false);

		return pDst ? true : false;
	}

	CCardPackPage* GetCardPackPage()
	{
		// NOFORMAT
		return (CCardPackPage*)m_pPage[0];
	}

	CCardPage* GetCardPage()
	{
		// NOFORMAT
		return (CCardPage*)m_pPage[1];
	}

	void SetSelectedSlot(unsigned char slot)
	{
		// NOFORMAT
		m_selectedSlot = slot;
	}

	bool IsShow() const
	{
		// NOFORMAT
		return m_bShow;
	}

	bool m_bShow;
	bool m_bShowProperty;
	unsigned char m_tab;
	unsigned char m_pageNum;
	unsigned char m_curPage;
	unsigned char m_overPage;
	unsigned char m_selectedSlot;
	std::vector<unsigned long> m_cardPackList;
	std::vector<unsigned long>* m_pCurCardList;
	std::vector<WRect> m_pageIndexRect;
	ICardBookPage* m_pPage[2];
	const Bitmap* m_pCardBookBg[4];
	const Bitmap* m_pCardPackBg[4];
	const Bitmap* m_pCollectionBg[4];
	FrArea* m_pCardBookArea;
	FrArea* m_pPropertyArea;
	FrButton* m_pTab[5];
	FrButton* m_pUseBtn;
	FrListBox* m_pIndexList;
};
