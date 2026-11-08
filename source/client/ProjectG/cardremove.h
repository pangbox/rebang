#pragma once

#include <list>

class FrCardRemove : public FrForm
{
	DECLARE_OBJECT(FrCardRemove)

public:
	FrCardRemove();
	virtual ~FrCardRemove();

	static bool IsCardRemoveItem(unsigned long typeId);
	void SetCardRemoveItem(unsigned long typeId, unsigned long uid);

protected:
	bool CheckEquiped(unsigned long uid);
	bool SetCurPartUid(unsigned long uid);
	void BuildCurPartCardList();

	void DrawInDlg(const char* name, float x, float y);
	void PrintInDlg(const char* text, float x, float y);
	void DrawTexture(const char* name, float x, float y, unsigned long color);
	void PrintText(const char* text, float x, float y, unsigned long color);

	void OnDrawAreaInit(int param);
	void OnDrawAreaOwnerDraw(int param);
	void OnItemListInit(int param);
	void OnItemListOwnerDraw(int param);
	void OnItemListDBClick();
	void OnItemListRBDown();
	void OnConfirmInit(int param);
	void OnConfirmLBDown();
	void OnCancelInit(int param);
	void OnCancelLBDown();
	bool OnUseCardRemoveConfirm(int ret, FrForm* pForm);

	DECLARE_FRESH_MSGMAP()

protected:
	FrArea* m_pDrawArea;
	FrListBox* m_pItemList;
	unsigned long m_itemTypeId;
	unsigned long m_itemUid;
	unsigned long m_partTypeId;
	unsigned long m_partUid;
	std::list<unsigned long> m_cardList;
};

class FrCardRemoveConfirm : public FrForm
{
	DECLARE_OBJECT(FrCardRemoveConfirm)

public:
	FrCardRemoveConfirm();
	virtual ~FrCardRemoveConfirm();

protected:
	void OnDrawAreaOwnerDraw(int param);
	void OnConfirmInit(int param);
	void OnConfirmLBDown();
	void OnCancelInit(int param);
	void OnCancelLBDown();

	DECLARE_FRESH_MSGMAP()

protected:
	unsigned long m_partTypeId;
	unsigned long m_partUid;

public:
	void SetPart(unsigned long typeId, unsigned long uid)
	{
		m_partTypeId = typeId;
		m_partUid = uid;
	}
};

class FrCardRemoveResult : public FrForm
{
	DECLARE_OBJECT(FrCardRemoveResult)

public:
	enum eRESULT
	{
		RESULT_ERROR,
		RESULT_SUCCESS,
		RESULT_FAIL,
		RESULT_ABNORMAL,
	};

	FrCardRemoveResult();
	virtual ~FrCardRemoveResult();

protected:
	virtual bool OnInit();
	virtual void OnProc(const float delta);

	void OnDrawAreaOwnerDraw(int param);
	void OnConfirmInit(int param);
	void OnConfirmLBDown();

	DECLARE_FRESH_MSGMAP()

protected:
	eRESULT m_result;
	std::list<unsigned long> m_cardList;
	const Bitmap* m_pCover;
	const Bitmap* m_pCoverMask;
	WRect m_coverRect;
	WRect m_maskRect;
	int m_state;
	unsigned long m_remover;

public:
	void SetRemover(unsigned long typeId) { m_remover = typeId; }
	void SetResult(eRESULT result)
	{
		// NOFORMAT
		m_result = result;
	}

	void PushList(unsigned long typeId)
	{
		// NOFORMAT
		m_cardList.push_back(typeId);
	}
};
