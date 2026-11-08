#include "codetinker.h"

CCodeTinker::CCodeTinker()
{
}

CCodeTinker::~CCodeTinker()
{
}

const char* CCodeTinker::GetHoleDropEffectName(unsigned long typeId)
{
	const char* name = NULL;

	static struct
	{
		unsigned long typeId;
		char name[128];
	} s_effects[] = {
		{ 0x1A00003B, "newyear_money.spr" },
		{ 0x1A000044, "aeboat-01.spr"     },
		{ 0x1A000045, "aeboat-02.spr"     },
		{ 0x1A000046, "aeboat-03.spr"     },
		{ 0x1A000047, "aeboat-04.spr"     },
		{ 0x1A000048, "event_box.spr"     },
		{ 0x1A000049, "box_candy_01.spr"  },
		{ 0x1A00004A, "box_candy_02.spr"  },
		{ 0x1A00004B, "box_candy_03.spr"  },
		{ 0x1A00004C, "box_candy_04.spr"  },
		{ 0x1A00004E, "exticket.spr"      },
		{ 0x1A000054, "event_box_02.spr"  },
		{ 0x1A0000AA, "coin_medal01.spr"  },
		{ 0x1A0000AB, "coin_medal02.spr"  },
		{ 0x1A0000AC, "coin_medal03.spr"  },
		{ 0x1A0000BC, "box_ring.spr"      },
		{ 0x1A000150, "spell_piece01.spr" },
		{ 0x1A000151, "spell_piece02.spr" },
		{ 0x1A000152, "spell_piece03.spr" },
		{ 0x1A000153, "spell_piece04.spr" },
	};

	const int count = sizeof(s_effects) / sizeof(s_effects[0]);

	for (int i = 0; i < count; ++i)
	{
		if (typeId == s_effects[i].typeId)
		{
			name = s_effects[i].name;
			break;
		}
	}
	return name;
}

bool CCodeTinker::IsNeedOpenMiniButton(unsigned long typeId)
{
	switch (typeId)
	{
	case 0x1A00003B:
	case 0x1A000048:
	case 0x1A000052:
	case 0x1A0000BC:

		return true;
	default:
		break;
	}
	return false;
}
