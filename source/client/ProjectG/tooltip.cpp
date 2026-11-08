#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "inputmanager.h"
#include "golfdoc.h"
#include "tooltip.h"

extern Fresh* g_pFresh;

struct sToolTip
{
	eBarButton id;
	const char* name;
};

sToolTip ToolTips[] = {
	{ TOOLTIP_BACK,             "tooltip_back"             },
	{ TOOLTIP_FRONT,            "tooltip_front"            },
	{ TOOLTIP_SHOP,             "tooltip_shop"             },
	{ TOOLTIP_MYROOM,           "tooltip_myroom"           },
	{ TOOLTIP_DOL,              "tooltip_dol"              },
	{ RMR_RM_POSTBOX,           "rmr_rm_postbox"           },
	{ TOOLTIP_COOKIE,           "tooltip_cookie"           },
	{ TOOLTIP_GAME,             "tooltip_game"             },
	{ TOOLTIP_SERVER,           "tooltip_server"           },
	{ TOOLTIP_EXIT,             "tooltip_exit"             },
	{ TOOLTIP_SPECIAL_CARD,     "tooltip_special_card"     },
	{ TOOLTIP_MSN,              "tooltip_msn"              },
	{ TOOLTIP_MYINFO,           "tooltip_myinfo"           },
	{ TOOLTIP_OPTION,           "tooltip_option"           },
	{ TOOLTIP_MAGICBOX,         "tooltip_magicbox"         },
	{ TOOLTIP_BONG,             "tooltip_bong"             },
	{ TOOLTIP_RANKING,          "tooltip_ranking"          },
	{ TOOLTIP_GUILD,            "tooltip_guild"            },
	{ TOOLTIP_NOTICE,           "tooltip_notice"           },
	{ RMR_TOOLTIP_MARKET,       "rmr_tooltip_market"       },
	{ TOOLTIP_SCRATCH,          "tooltip_scratch"          },
	{ TOOLTIP_PLAY,             "tooltip_play"             },
	{ TOOLTIP_BUY,              "tooltip_buy"              },
	{ TOOLTIP_GIFT,             "tooltip_gift"             },
	{ TOOLTIP_BUYALL,           "tooltip_buyall"           },
	{ TOOLTIP_BUYRESET,         "tooltip_buyreset"         },
	{ TOOLTIP_EQUIP,            "tooltip_equip"            },
	{ TOOLTIP_MOVEGIFT,         "tooltip_movegift"         },
	{ TOOLTIP_DELETE,           "tooltip_delete"           },
	{ TOOLTIP_RESET,            "tooltip_reset"            },
	{ TOOLTIP_REEMPLOY,         "tooltip_reemploy"         },
	{ TOOLTIP_REPLY,            "tooltip_reply"            },
	{ TOOLTIP_HANDSEL,          "tooltip_handsel"          },
	{ TOOLTIP_OPENGIFT,         "tooltip_opengift"         },
	{ TOOLTIP_REPORT,           "tooltip_report"           },
	{ TOOLTIP_UPGRADE,          "tooltip_upgrade"          },
	{ TOOLTIP_DOWNGRADE,        "tooltip_downgrade"        },
	{ TOOLTIP_EQUIPCANCEL,      "tooltip_equipcancel"      },
	{ TOOLTIP_EQUIPRESET,       "tooltip_equipreset"       },
	{ TOOLTIP_CARD,             "tooltip_card"             },
	{ TOOLTIP_CHARACTER,        "tooltip_character"        },
	{ TOOLTIP_CADDIE,           "tooltip_caddie"           },
	{ TOOLTIP_CLUB,             "tooltip_club"             },
	{ TOOLTIP_AZTEC,            "tooltip_aztec"            },
	{ TOOLTIP_MASCOT,           "tooltip_mascot"           },
	{ UCC_TOOLTIP_DRAW,         "ucc_tooltip_draw"         },
	{ UCC_TOOLTIP_COPY,         "ucc_tooltip_copy"         },
	{ UCC_TOOLTIP_DRAG,         "ucc_tooltip_drag"         },
	{ UCC_TOOLTIP_GRID,         "ucc_tooltip_grid"         },
	{ UCC_TOOLTIP_PAINT,        "ucc_tooltip_paint"        },
	{ UCC_TOOLTIP_REDO,         "ucc_tooltip_redo"         },
	{ UCC_TOOLTIP_UNDO,         "ucc_tooltip_undo"         },
	{ UCC_TOOLTIP_RESET,        "ucc_tooltip_reset"        },
	{ UCC_TOOLTIP_ZOOMIN,       "ucc_tooltip_zoomin"       },
	{ UCC_TOOLTIP_ZOOMOUT,      "ucc_tooltip_zoomout"      },
	{ UCC_TOOLTIP_SPOID,        "ucc_tooltip_spoid"        },
	{ UCC_TOOLTIP_TAB_DRAW,     "ucc_tooltip_tab_draw"     },
	{ UCC_TOOLTIP_TAB_PATTERN,  "ucc_tooltip_tab_pattern"  },
	{ UCC_TOOLTIP_TAB_EMOTICON, "ucc_tooltip_tab_emoticon" },
	{ UCC_TOOLTIP_TAB_TEXT,     "ucc_tooltip_tab_text"     },
	{ UCC_TOOLTIP_REDRAW,       "ucc_tooltip_redraw"       },
	{ UCC_TOOLTIP_TEMPSAVE,     "ucc_tooltip_tempsave"     },
	{ RMR_RM_DECO,              "rmr_rm_deco"              },
	{ RMR_RM_LETTER,            "rmr_rm_letter"            },
	{ RMR_DELETE,               "rmr_delete"               },
	{ RMR_RM_WITHDRAWAL,        "rmr_rm_withdrawal"        },
	{ RMR_TOOLTIP_CANCEL,       "rmr_tooltip_cancel"       },
	{ RMR_TOOLTIP_EXTEND,       "rmr_tooltip_extend"       },
	{ RMR_ITEM_UPLOAD,          "rmr_item_upload"          },
	{ RMR_TOOLTIP_RETURN,       "rmr_tooltip_return"       },
	{ TOOLTIP_USE,              "tooltip_use"              },
	{ TOOLTIP_PACKAGE,          "tooltip_package"          },
};
CIconToolTip::CIconToolTip()
{
	m_curTipIndex = -1;
	m_pCurToolTip = NULL;

	for (int i = 0; i < MAX_BARBUTTON; ++i)
	{
		m_pToolTip[i] = g_pFresh->GetBitmap(ToolTips[i].name);
	}
}

CIconToolTip::~CIconToolTip()
{
	for (int i = 0; i < MAX_BARBUTTON; ++i)
	{
		m_pToolTip[i] = NULL;
	}
}

void CIconToolTip::Render()
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (m_pCurToolTip && pGDI)
	{
		if (GOLFDOC() && GOLFDOC()->m_gameMode == 0x200)
		{
			ClearToolTip();
			return;
		}
		float x = g_input->GetMousePoint().x;
		float y = g_input->GetMousePoint().y;
		float width = (float)m_pCurToolTip->Width();
		float height = (float)m_pCurToolTip->Height();

		WRect dest(x + 12.0f, y + 23.0f, width, height);

		if (dest.x >= g_view->GetWidth() - width)
			dest.x = g_view->GetWidth() - width;

		if (dest.y >= g_view->GetHeight() - height)
			dest.y = g_view->GetHeight() - height;

		pGDI->DrawTexture(m_pCurToolTip, dest, 0xffffffff, 0);
	}
}

void CIconToolTip::SetToolTip(eBarButton tip)
{
	if (!m_pToolTip[tip])
	{
		m_pToolTip[tip] = g_pFresh->GetBitmap(ToolTips[tip].name);
	}

	m_pCurToolTip = m_pToolTip[tip];
	m_curTipIndex = tip;
}

void CIconToolTip::ClearToolTip()
{
	m_pCurToolTip = NULL;
	m_curTipIndex = -1;
}

bool CIconToolTip::IsCurToolTip(eBarButton tip)
{
	if (m_pCurToolTip == m_pToolTip[tip])
		return true;

	return false;
}
