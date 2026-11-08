#pragma once

#include <string>
#include <vector>
#include <list>

#include "actor.h"
#include "scenemanager.h"

class CBarFont;

class IReviewHistory
{
public:
	virtual ~IReviewHistory() { }

	virtual const char* ClassName() const = 0;
	virtual void AddItem(unsigned int type, unsigned int param) = 0;
	virtual void Clear() = 0;
	virtual void Display() = 0;
	virtual void PopBack() = 0;
};

class ShotReview : public IActor, public CRenderFuncPtr
{
	DECLARE_OBJECT(ShotReview)

protected:
	ShotReview();
	virtual ~ShotReview();

	virtual void OnInit();
	virtual void OnProcess(float delta);

public:
	virtual void HandleMsg(const MsgObject& msg);
	virtual void Display();

	struct ReviewHistoryFinder
	{
		std::string m_name;

		ReviewHistoryFinder(const std::string& name)
			: m_name(name)
		{
		}
		bool operator()(const IReviewHistory* pHistory) const
		{
			return m_name == pHistory->ClassName();
		}
	};

private:
	std::vector<IReviewHistory*> m_historyList;
	bool m_bVisible;
	bool m_bRecordFirstBound;
};

class SpotHistory : public IReviewHistory
{
public:
	SpotHistory();
	virtual ~SpotHistory();

	virtual void AddItem(unsigned int type, unsigned int param);
	virtual void Clear();
	virtual void Display();
	virtual void PopBack();

private:
	std::vector<std::pair<WVector, WVector> > m_spotList;
	CBarFont* m_pFont;
	WOverlay* m_pIndicator1;
	WOverlay* m_pIndicator2;

public:
	virtual const char* ClassName() const { return "SpotHistory"; }
};

class BallTraceHistory : public IReviewHistory
{
public:
	BallTraceHistory();
	virtual ~BallTraceHistory();

	virtual void AddItem(unsigned int type, unsigned int param);
	virtual void Clear();
	virtual void Display();
	virtual void PopBack();

private:
	std::list<std::vector<WVector> > m_traceList;

public:
	virtual const char* ClassName() const { return "BallTraceHistory"; }
};

class FirstBoundHistory : public IReviewHistory
{
public:
	FirstBoundHistory();
	virtual ~FirstBoundHistory();

	virtual void AddItem(unsigned int type, unsigned int param);
	virtual void Clear();
	virtual void Display();
	virtual void PopBack();

private:
	void DrawIndicator(const WVector& pos, unsigned long color);

	std::vector<WVector> m_realList;
	std::vector<WVector> m_predictedList;
	WOverlay* m_pIndicator;
	CBarFont* m_pFont;

public:
	virtual const char* ClassName() const { return "FirstBoundHistory"; }
};
