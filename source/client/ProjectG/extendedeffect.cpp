#include "stdafx.h"
#include "fxspray.h"

#include "fx.h"
#include "fxsequence.h"
#include "partsinfo.h"
#include "wbone.h"
#include "extendedeffect.h"
#include "mathconsts.h"

CExtendedEffect::CExtendedEffect()
{
}

CExtendedEffect::~CExtendedEffect()
{
	for (std::list<sAvatarInfo>::iterator avatar = m_avatarList.begin();
		avatar != m_avatarList.end(); avatar++)
	{
		for (std::list<sEffectData>::iterator effect =
				 (*avatar).effects.begin();
			effect != (*avatar).effects.end(); effect++)
		{
			if ((*effect).sequence)
				CFx::Instance()->CloseSequence((*effect).sequence, false);
		}
		(*avatar).effects.clear();
	}
	m_avatarList.clear();
	m_effectMap.clear();
}

int CExtendedEffect::AddEffect(unsigned long tid, const char* bone,
	const char* filename, bool loop)
{
	if (!tid || !bone || !filename)
		return 0;
	std::map<unsigned long, sEffectData>::iterator it = m_effectMap.find(tid);
	if (it != m_effectMap.end())
		return 0;
	sEffectData data;
	data.tid = tid;
	data.bone = bone;
	data.filename = filename;
	data.loop = loop;
	data.sequence = NULL;
	m_effectMap[tid] = data;
	return 1;
}

CExtendedEffect::sEffectData* CExtendedEffect::FindEffect(unsigned long tid)
{
	sEffectData* effect = NULL;
	std::map<unsigned long, sEffectData>::iterator it = m_effectMap.find(tid);
	if (it != m_effectMap.end())
		effect = &it->second;
	return effect;
}

int CExtendedEffect::RegistSequence(unsigned long tid,
	std::list<sEffectData>* list)
{
	if (!tid || !list)
		return 0;
	sEffectData* effect = FindEffect(tid);
	if (!effect)
		return 0;
	sEffectData data;
	data.tid = (*effect).tid;
	data.bone = (*effect).bone;
	data.filename = (*effect).filename;
	data.loop = (*effect).loop;
	data.sequence = NULL;
	data.sequence = CFx::Instance()->OpenSequence(data.filename.c_str(), true);
	if (!data.sequence)
		return 0;
	list->push_back(data);
	return 1;
}

int CExtendedEffect::RegistExtenedEffect(CPetFrame* pet)
{
	if (!pet)
		return 0;
	for (std::list<sAvatarInfo>::iterator it = m_avatarList.begin();
		it != m_avatarList.end(); it++)
	{
		if (pet == (*it).pet)
			return 0;
	}
	sAvatarInfo info;
	info.pet = pet;
	CPartList& parts = pet->GetPartList();
	for (unsigned char i = 0; i < 24; i++)
	{
		const sPart* part = parts.GetPart(i);
		if (part)
		{
			unsigned long tid = part->tid;
			RegistSequence(tid, &info.effects);
		}
	}
	if (!info.effects.empty())
	{
		m_avatarList.push_back(info);
		info.effects.clear();
	}
	return 1;
}

void CExtendedEffect::Update(CPetFrame* pet)
{
	for (std::list<sAvatarInfo>::iterator avatar = m_avatarList.begin();
		avatar != m_avatarList.end(); avatar++)
	{
		bool active = true;
		if (pet && pet != (*avatar).pet)
			active = false;
		for (std::list<sEffectData>::iterator effect =
				 (*avatar).effects.begin();
			effect != (*avatar).effects.end(); effect++)
		{
			if (!CFx::Instance()->FindSequence((*effect).sequence))
			{
				if ((*effect).loop == true)
					(*effect).sequence = CFx::Instance()->OpenSequence(
						(*effect).filename.c_str(), true);
				else
					(*effect).sequence = NULL;
			}
			if ((*effect).sequence)
			{
				(*effect).sequence->SetActive(active, false);
				WBone* bone = (*avatar).pet->GetBonePet()->FindBone(
					(*effect).bone.c_str());
				(*effect).sequence->Pos() = bone->GetMatrix().pivot;
			}
		}
	}
}
