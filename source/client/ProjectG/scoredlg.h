#pragma once

#include "frform.h"

#include "frgraphicinterface.h"
class FrArea;
struct sRivalData;
class FrScoreDlg : public FrForm
{
	DECLARE_OBJECT(FrScoreDlg)

	FrScoreDlg();

	void SetPlayer(unsigned long uid);
	void SetVisibleCloseBtn(bool bVisible);
	void SetVisibleCancelBtn(bool bVisible);
	void SetGuildPlayer(unsigned long uid);

protected:
	void Resize();
	void LoadBitmaps();

	void OnBar2Init(int param);
	void OnBar3Init(int param);
	void OnFront9BtnInit(int param);
	void OnFront9BtnUp();
	void OnBack9BtnUp();
	void OnShotBtnUp();
	void OnGraphBtnUp();
	void OnPangBtnUp();
	void OnVersusBtnUp();
	void OnPuttBtnInit(int param);
	void OnBack9BtnInit(int param);
	void OnShotBtnInit(int param);
	void OnGraphBtnInit(int param);
	void OnPangBtnInit(int param);
	void OnVersusBtnInit(int param);
	void OnHoleListInit(int param);
	void OnHoleListOwnerDraw(int param);
	void OnParListInit(int param);
	void OnParListOwnerDraw(int param);
	void OnGolferListInit(int param);
	void OnGolferListOwnerDraw(int param);
	void OnRecordListInit(int param);
	void OnRecordListOwnerDraw(int param);
	void OnTotalListInit(int param);
	void OnTotalListOwnerDraw(int param);
	void OnCancelBtnInit(int param);
	void OnCancelBtnUp();
	void OnCloseBtnInit(int param);
	void OnCloseBtnUp();
	void OnBlankPanel01Init(int param);
	void OnBlankPanel01OwnerDraw(int param);
	void OnBlankPanel02Init(int param);
	void OnBlankPanel02OwnerDraw(int param);
	void OnBlankPanel03Init(int param);
	void OnBlankPanel03OwnerDraw(int param);

	virtual void OnProc(const float delta);
	virtual void OnDraw();

	void SetFrontBackBtn();
	void SetTypeBtn();
	void DrawShot(FrListItem* item);
	void DrawGraph(FrListItem* item);
	void DrawPang(FrListItem* item);
	void DrawVersus(FrListItem* item);
	void DrawDigit(FrGraphicInterface* gdi, char* text, WPoint pos,
		unsigned long color, FrGDI::eTextAlign align, bool narrow);
	int GetGuildVersus(sRivalData* left, sRivalData* right, int hole);
	unsigned long GetScoreColor(int score);
	int GetMyCurHoleInGuildMatch();

	FrArea* m_pBar2;
	FrArea* m_pBar3;
	FrButton* m_pFront9Btn;
	FrButton* m_pBack9Btn;
	void* m_pUnknown120;
	void* m_pUnknown124;
	void* m_pUnknown128;
	void* m_pUnknown12c;
	FrButton* m_pTypeBtn[4];
	FrListBox* m_pList[5];
	FrButton* m_pCancelBtn;
	FrButton* m_pCloseBtn;
	bool m_bFront9;
	int m_type;
	float m_aniTime;
	const Bitmap* m_pDigit[10];
	const Bitmap* m_pPlus;
	const Bitmap* m_pMinus;
	const Bitmap* m_pPoint;
	const Bitmap* m_pWin;
	const Bitmap* m_pLose;
	const Bitmap* m_pDraw;
	const Bitmap* m_pNoResult;
	unsigned long m_versusOID[2];
	const Bitmap* m_pPCIcon;
	sRivalData* m_pMyData;
	bool m_bShowBlankPanel;
	FrArea* m_pBlankPanel[3];

	DECLARE_FRESH_MSGMAP()
};

void ResizeScoreBar23(FrArea* bar2, FrArea* bar3, float x, float w);
