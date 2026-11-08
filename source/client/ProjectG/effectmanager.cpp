#include "projectg.h"
#include "golfball.h"
#include "fx.h"
#include "effectmanager.h"
#include "mathconsts.h"

CAztecHoleinEffect::CAztecHoleinEffect()
{
	Clear();
}

CAztecHoleinEffect::~CAztecHoleinEffect()
{
	Clear();
}

void CAztecHoleinEffect::Clear()
{
	m_unknown28 = 0;
	m_unknown2c = 0;

	for (std::map<unsigned long, const char*>::iterator it =
			 m_iffEffectMap.begin();
		it != m_iffEffectMap.end(); ++it)
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}

	m_iffEffectMap.clear();

	for (std::map<unsigned long, const char*>::iterator it2 =
			 m_effectMap.begin();
		it2 != m_effectMap.end(); ++it2)
		if (it2->second)
		{
			delete it2->second;
			it2->second = NULL;
		}

	m_effectMap.clear();
}

bool CAztecHoleinEffect::LoadEffectFromIFF()
{
	return true;
}

bool CAztecHoleinEffect::Initialize()
{
	return false;
}

bool CAztecHoleinEffect::RegisterEffect(unsigned long effect, const char* name)
{
	bool ret = m_effectMap.insert(std::make_pair(effect, name)).second;

	return ret;
}

const char* CAztecHoleinEffect::GetEffectName(unsigned long effect)
{
	const char* name = NULL;
	std::map<unsigned long, const char*>::iterator it;

	it = m_iffEffectMap.find(effect);

	if (it != m_iffEffectMap.end() && it->second)
	{
		name = it->second;
	}
	else
	{
		it = m_effectMap.find(effect);

		if (it != m_effectMap.end() && it->second)
			name = it->second;
	}

	return name;
}

bool CAztecHoleinEffect::PlayEffect(unsigned long effect)
{
	bool bRet = false;

	const char* name = GetEffectName(effect);

	if (name)
	{
		CFxSpray* pSpray = CFx::Instance()->OpenSpray(name);

		if (pSpray)
		{
			WVector pos = GolfBall().m_pos;
			pSpray->Pos() = pos;
			bRet = true;
		}
	}

	return bRet;
}
