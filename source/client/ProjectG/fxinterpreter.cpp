#include "projectg.h"
#include "wpuppet.h"
#include "wbone.h"
#include "fx.h"
#include "fxinterpreter.h"
#include "mathconsts.h"

const char FxInterpreter::TOKEN[][8] = { "*fx", "*playif" };

FxInterpreter* GetFxInterpreter()
{
	static FxInterpreter fxInterpreter;

	return &fxInterpreter;
}

FxInterpreter::sFxInfo::sFxInfo(int _id, eEffectType _type, void* _pFx)
{
	id = _id;
	type = _type;
	pFx = _pFx;
}

void FxInterpreter::sFxInfo::DetachSingleFx()
{
	if (CFx::Instance())
	{
		if (type == eET_SPRAY)
			CFx::Instance()->CloseSpray((CFxSpray*)pFx);
		else if (type == eET_SEQUENCE)
			CFx::Instance()->CloseSequence((CFxSequence*)pFx, true);
	}
}

FxInterpreter::FxInterpreter()
{
	m_nextID = 0;
}

FxInterpreter::~FxInterpreter()
{
	if (!m_fxList.empty())
		DetachFxAll();
}

bool FxInterpreter::ParseToken(const char** str, eScripFormat format) const
{
	*str = strstr(*str, TOKEN[format]);
	return *str != NULL;
}

bool FxInterpreter::ParseFxName(const char** str, char* name,
	eScripFormat format, eEffectType& type) const
{
	switch (format)
	{
	case eSF_FX:

		*str = GetCharsTillSpace(*str + 3, name);
		break;
	case eSF_PLAYIF:
		*str = GetStrConst(*str + 3, name);
		break;
	default:

		return false;
	}

	int len = strlen(name);

	if (len == 0 || len >= 64)
		return false;

	char* ext = strrchr(name, '.');

	if (ext == NULL || strlen(ext) < 4)
		return false;
	const char* szExt = ext + 1;

	if (!stricmp(szExt, "seq"))
	{
		type = eET_SEQUENCE;
		return true;
	}

	if (!stricmp(szExt, "spr"))
	{
		type = eET_SPRAY;
		return true;
	}
	type = eET_NONE;
	return false;
}

int FxInterpreter::ParseBoundingBoxScript_fx(WPuppet* pet, eScripFormat format)
{
	bool bAttached = false;

	for (w_bound_box* box = pet->m_bbList.Start(); box;
		box = pet->m_bbList.Next())
	{
		if (!box->info->option || !box->bone || box->bone->IsInvisible())
			continue;

		const char* str = box->info->option;
		while (ParseToken(&str, format))
		{
			char name[64];
			eEffectType type;

			if (ParseFxName(&str, name, format, type))
			{
				Attacher attacher;
				attacher.mat[0] = &box->bone->GetMatrix();
				attacher.pos[0] = &box->info->spherePivot;
				bAttached = AttachFx(name, &attacher, type);
			}
		}
	}
	if (bAttached)
		return m_nextID++;

	return -1;
}

int FxInterpreter::ParseCharBoundingBoxScript_fx(WPuppet* pet,
	eScripFormat format)
{
	return -1;
}

int FxInterpreter::ParseCharFrameDataScrip_fx(WPuppet* pet, eScripFormat format)
{
	return -1;
}

bool FxInterpreter::AttachFx(const char* name, const Attacher* attacher,
	eEffectType type)
{
	if (name && attacher)
	{
		if (type == eET_SPRAY)
		{
			void* pFx = CFx::Instance()->Attach(*attacher, name, false, NULL);
			m_fxList.push_back(sFxInfo(m_nextID, eET_SPRAY, pFx));
			return true;
		}
		else if (type == eET_SEQUENCE)
		{
			void* pFx = CFx::Instance()->Attach(*attacher, name, false, NULL);
			m_fxList.push_back(sFxInfo(m_nextID, eET_SEQUENCE, pFx));
			return true;
		}
	}
	return false;
}

void FxInterpreter::DetachFx(int id)
{
	if (id < 0)
		return;
	std::list<sFxInfo>::iterator it = m_fxList.begin();
	while (it != m_fxList.end())
	{
		if ((*it).id == id)
		{
			std::list<sFxInfo>::iterator del = it++;
			(*del).DetachSingleFx();
			m_fxList.erase(del);
		}
		else
		{
			if ((*it).id > id)
				return;
			it++;
		}
	}
}

void FxInterpreter::DetachFxAll()
{
	for (std::list<sFxInfo>::iterator it = m_fxList.begin();
		it != m_fxList.end(); it++)
		(*it).DetachSingleFx();
	m_fxList.clear();
}
