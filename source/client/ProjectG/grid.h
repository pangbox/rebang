#pragma once

#include "actor.h"
#include "scenemanager.h"

class CGrid : public IActor, public CRenderFuncPtr
{
protected:
	CGrid();

public:
	virtual ~CGrid() { }
	DECLARE_OBJECT(CGrid)

	virtual void OnPreLoadInit();
	virtual void OnLoad();
	virtual void OnReload();
	virtual void OnProcess(float delta);
	virtual void OnDestroy();

	virtual void Display();

protected:
	virtual void HandleMsg(const MsgObject& msg);

	void Reset();
	void SetGridType();
	void CheckGridPass();
	void MakeStaticGrid();
	void MakeDynamicGrid();
	int GetColor(float slope);
	void SetHoleGrid();
	void MakeSlopeIndex();
	float GetSlope(int row, int col, float interval, bool bRow);

private:
	void MoveLine(float delta);

	struct sGridBlock
	{
		WVector pos;
		WVector slope;
		bool bPass;
	};

	WVector m_center;
	sGridBlock m_block[21][21];
	float m_angle;
	int m_colNum;
	int m_rowNum;
	float m_interval;
	int m_colNumAdd;
	int m_rowNumAdd;
	float m_intervalAdd;
	int m_holeRow;
	int m_holeCol;
	bool m_bHoleGrid;
	WVector m_dir;
	WVector m_right;
	int m_slopeTex;
	int m_middleTex;
	bool m_bCheckPass;
	float m_gridValue;
	unsigned long m_color;
	WTVertex* m_vl[5];
	WTVertex m_v[4];
	bool m_bBlink;
	float m_blinkTime;
	unsigned long m_blinkColor;
	float m_linePos;
	unsigned long m_reserved;
};
