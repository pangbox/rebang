#include "fortunedlg.h"
#include "frarea.h"
#include "frstatic.h"

IMPLEMENT_OBJECT(FrFortuneDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrFortuneDlg, FrForm)

ON_FRESH_VI("portrait", FRCMD_INIT, FrFortuneDlg::OnPortraitInit)
ON_FRESH_VI("desc_name", FRCMD_INIT, FrFortuneDlg::OnDescNameInit)
ON_FRESH_VI("desc_level", FRCMD_INIT, FrFortuneDlg::OnDescLevelInit)
ON_FRESH_VI("desc_desc", FRCMD_INIT, FrFortuneDlg::OnDescDescInit)

END_FRESH_MSGMAP()

void FrFortuneDlg::OnPortraitInit(int param)
{
	m_pPortrait = DYNAMIC_CAST(FrArea, param);
}

void FrFortuneDlg::OnDescNameInit(int param)
{
	m_pDescName = DYNAMIC_CAST(FrStatic, param);
}

void FrFortuneDlg::OnDescLevelInit(int param)
{
	m_pDescLevel = DYNAMIC_CAST(FrStatic, param);
}

void FrFortuneDlg::OnDescDescInit(int param)
{
	m_pDescDesc = DYNAMIC_CAST(FrStatic, param);
}

FrFortuneDlg::FrFortuneDlg()
{
	m_typeId = 0;

	m_pPortrait = NULL;
	m_pDescName = NULL;
	m_pDescLevel = NULL;
	m_pDescDesc = NULL;
}

FrFortuneDlg::~FrFortuneDlg()
{
}

void FrFortuneDlg::SetTypeId(unsigned long typeId, IFF_ITEM_COMMON* item)
{
	m_typeId = typeId;

	if (item == NULL)
		return;

	if (m_pPortrait)
	{
		m_pPortrait->SetBgImg(item->Icon);
	}

	if (m_pDescName)
	{
		m_pDescName->SetCaption(MakeStr("\xc0\xcc\xb8\xa7 : %s", item->Name));
	}

	if (m_pDescLevel)
	{
		m_pDescLevel->SetCaption(
			"\xb7\xb9\xba\xa7 : \xb4\xa9\xb1\xb8\xb3\xaa \xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9");
	}

	if (m_pDescDesc)
	{
		m_pDescDesc->SetCaption(MakeStr(
			"\xbc\xd2\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 %s\xc0\xbb \xbf\xad\xbe\xee\xba\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			item->Name));
	}
}
