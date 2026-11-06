#pragma once

#include <string>
#include <vector>

#include "actor.h"
#include "scenemanager.h"
#include "tweaker.h"
#include "wmath.h"

class C3dFont;
class CGolfBall;
class FrButton;
class MsgObject;

class ShotPreview : public IActor,
					public CRenderFuncPtr,
					public tweaker_cmd_target
{
public:
	DECLARE_OBJECT(ShotPreview)

	class PangCoin;
	class UpdateSensor;

	struct PredictType
	{
		enum Enum
		{
		};
	};

private:
	ShotPreview(void);
	virtual ~ShotPreview(void);

public:
	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnReload(void);
	virtual void OnProcess(float delta);
	virtual void Display(void);

	std::string TwAddOption(const tweaker_param& param);
	std::string TwSubOption(const tweaker_param& param);

private:
	virtual void HandleMsg(const MsgObject& msg);

	bool IsSkipState(void);
	void DrawIndicator(const WVector& pos, unsigned long color, float size);
	bool Predict(std::vector<WVector>& path, std::vector<WVector>& bounces,
		int* count);
	unsigned short GetBallInitSpecialValueFromPredictType(
		PredictType::Enum type);

	unsigned int GetOptionFlag(void) const { return m_optionFlag; }

	void SetOptionFlag(unsigned int flag) { m_optionFlag = flag; }

	CGolfBall* m_pBall;
	std::vector<WVector> m_path;
	std::vector<WVector> m_path2;
	UpdateSensor* m_pSensor;
	float m_updateElapsed;
	int m_indicatorTexture;
	PredictType::Enum m_predictType;
	unsigned int m_optionFlag;
	int m_visible;
	C3dFont* m_pFont;
	std::vector<FrButton*> m_predictButtons;
	std::vector<PangCoin*> m_pangCoins;
};
