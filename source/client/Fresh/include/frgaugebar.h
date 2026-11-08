#pragma once
#include "rtti.h"
#include "frcmdtarget.h"
#include "frelement.h"
#include "frwnd.h"
#include "../../Wangreal/include/wtypes.h"

class Bitmap;
class FrButton;
class FrWndManager;

class FrGaugeBar : public FrWnd
{
	DECLARE_OBJECT(FrGaugeBar)

public:
	FrGaugeBar();
	virtual ~FrGaugeBar();
	virtual void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	void SetBorder(int thickness) { m_thickness = thickness; }
	void SetRange(int lower, int upper, int origin);
	virtual void SetPos(int pos);
	void SetExpand(int expand);
	void SetExpandColor1(unsigned long color);
	void SetExpandColor2(unsigned long color);
	void SetDestPos(int pos, bool b);
	float GetPos() { return m_pos; }
	float GetDestPos();
	void SetColor(unsigned long color) { m_color = color; }
	void SetBarSpeed(float speed) { m_speed = speed; }
	int GetRangeLower() const;
	int GetRangeUpper() const;
	virtual void MoveWindow(const WPoint& point);
	virtual void Enable(bool enable) { m_dwStyle.Turn(FWS_DISABLED, !enable); }

protected:
	virtual void CalcBarRect(const WRect* rect);
	virtual void OnProc(const float deltaTime);
	virtual void OnDraw();

	FrGuiItem* m_pItem;
	WRect m_barRect;
	unsigned long m_color;
	unsigned long m_bgColor;
	unsigned long m_exColor[2];
	int m_thickness;
	int m_lower;
	int m_upper;
	int m_origin;
	float m_pos;
	float m_expand;
	float m_destPos;
	float m_srcPos;
	float m_speed;
	bool m_achillesRun;
};

class FrGaugeBarEx : public FrGaugeBar
{
	DECLARE_OBJECT(FrGaugeBarEx)

public:
	FrGaugeBarEx();
	virtual ~FrGaugeBarEx();
	virtual void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	virtual void SetPos(int pos);
	void SetSteps(int steps);
	int GetSteps();
	virtual void Enable(bool enable);
	const FrButton* GetLButton() const;
	const FrButton* GetRButton() const;
	virtual void MoveWindow(const WPoint& point);

protected:
	virtual void OnDraw();
	virtual void CalcBarRect(const WRect* rect);

	unsigned long m_borderColor;
	unsigned long m_color2;
	unsigned long m_colorP;
	WRect m_gaugeRect;
	float m_fPos;
	int m_steps;
	FrGuiItem m_LButtonInfo;
	FrButton* m_pLButton;
	FrGuiItem m_RButtonInfo;
	FrButton* m_pRButton;

	void OnLLButtonDown();
	void OnRLButtonDown();

	DECLARE_FRESH_MSGMAP()
};

class FrGaugeBarImage : public FrGaugeBar
{
	DECLARE_OBJECT(FrGaugeBarImage)

public:
	enum eGauge
	{
		GAUGE_NORMAL,
		GAUGE_MASK,
		MAX_COUNT
	};

	FrGaugeBarImage();
	virtual ~FrGaugeBarImage();
	virtual void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	virtual void MoveWindow(const WPoint& point);
	const Bitmap* GetGaugeSkin(int index);
	void ResetGauge();
	int GetPixelPerHeight();
	float GetComplete();

protected:
	virtual void OnDraw();
	virtual void OnProc(const float deltaTime);
	virtual void CalcBarRect(const WRect* rect);

	const Bitmap* m_pBitImage[MAX_COUNT];
	Bitmap* m_BitMask;
	WRect m_gaugeRect;
	float m_start;
	unsigned int m_iPixelCounter;
	bool m_bOwnerDraw;
	float m_complete;
};
