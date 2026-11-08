#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "itemstorage.h"
#include "../../shared/globalnetworkdefine.h"
#include "indexcontrol.h"
#include "packet.h"
#include "objectfactory.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "frlistbox.h"
#include "fresh.h"
extern Fresh* g_pFresh;
#include "frwndmanager.h"
#include "inputmanager.h"
#include "shareddoc.h"
#include "wresrcmng.h"
#include "wfont.h"
#include "wview.h"
#include "wutil.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "../../shared/localize.h"
#include "uccmanager.h"
#include "ucclibrary.h"
#include "taskmanager.h"
#include "binstr.h"

template <class T, class P>
bool TSafeNew(P& p, unsigned int count)
{
	bool result = false;
	if (!p && count > 0)
	{
		p = new T[count];
		if (p)
		{
			result = true;
		}
	}
	return result;
}

template <class P>
void TSafeDelete(P& p)
{
	if (p)
	{
		delete[] p;
		p = NULL;
	}
}

void FrItemStorageNotice::OnProc(float)
{
}

void FrItemStorageNotice::OnDraw(void)
{
}

IMPLEMENT_OBJECT(FrItemStorageNotice, FrForm)

BEGIN_FRESH_MSGMAP(FrItemStorageNotice, FrForm)
ON_FRESH_VV("confirm", FRCMD_LBUTTONDOWN, FrItemStorageNotice::OnLBDownConfirm)
ON_FRESH_VI("print_out", FRCMD_OWNERDRAW,
	FrItemStorageNotice::OnOwnerDrawPrintOut)
END_FRESH_MSGMAP()

void FrItemStorageNotice::OnLBDownConfirm(void)
{
	Close(true);
}

void FrItemStorageNotice::OnOwnerDrawPrintOut(intptr_t id)
{
	const char* lines[7] = {

		"\xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4, \xc6\xce\xbe\xdf \xc0\xaf\xc0\xfa \xbf\xa9\xb7\xaf\xba\xd0!",
		"",

		"\xc3\xd6\xb1\xd9 \xba\xf3\xb9\xf8\xc7\xcf\xb0\xd4 \xb9\xdf\xbb\xfd\xc7\xcf\xb4\xc2 \xb0\xe8\xc1\xa4\xb5\xb5\xbf\xeb \xc7\xd8\xc5\xb7 \xbb\xe7\xb7\xca\xb8\xa6 \xb9\xe6\xc1\xf6\xc7\xcf\xbf\xa9",

		"\xc0\xaf\xc0\xfa \xbf\xa9\xb7\xaf\xba\xd0\xb2\xb2\xbc\xad \xb4\xf5\xbf\xed \xbe\xc8\xbd\xc9\xc7\xcf\xb0\xed \xc6\xce\xbe\xdf\xb8\xa6 \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xc0\xd6\xb5\xb5\xb7\xcf",

		"'\xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xba\xf1\xb9\xd0\xb1\xdd\xb0\xed' \xbd\xc3\xbd\xba\xc5\xdb\xc0\xbb \xbf\xc0\xc7\xc2\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9!",
		"",

		"\xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xba\xf1\xb9\xd0\xb1\xdd\xb0\xed\xb8\xa6 \xbe\xc8\xc0\xfc\xc7\xcf\xb0\xd4 \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xc0\xd6\xb5\xb5\xb7\xcf,"
	};

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	WRect rc = GetRect();

	if (pGDI)
	{
		float x = rc.x + 10.0f;

		for (int i = 0; i < 7; i++)
		{
			pGDI->Print(WPoint(x, (float)i * 15.0f + rc.y + 45.0f), 0,
				lines[i]);
		}
	}

	pGDI->Print(WPoint(rc.x + 10.0f, rc.y + 150.0f), 0,
		"\xc3\xd6\xc3\xca \xbb\xe7\xbf\xeb\xbd\xc3");
	pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	pGDI->SetTextStyle(1);

	pGDI->Print(WPoint(rc.x + 80.0f, rc.y + 150.0f), 0,
		"4\xc0\xda\xb8\xae\xc0\xc7 \xbc\xfd\xc0\xda \xbe\xcf\xc8\xa3\xb8\xa6 \xbc\xb3\xc1\xa4");
	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(0);

	pGDI->Print(WPoint(rc.x + 220.0f, rc.y + 150.0f), 0,
		"\xc7\xd8 \xc1\xd6\xbc\xc5\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.");
}

FrItemStorageAccessPanel::FrItemStorageAccessPanel(void)
{
	m_formType = (eFormType)0;
	m_phase = 1;

	memset(m_oldPassword, 0, sizeof(m_oldPassword));
	memset(m_password, 0, sizeof(m_password));
	memset(m_confirmPassword, 0, sizeof(m_confirmPassword));

	memset(m_slotNumber, 0, sizeof(m_slotNumber));

	m_pPrintOut = 0;

	memset(m_pNumberSlot, 0, sizeof(m_pNumberSlot));

	m_pConfirm = 0;
	m_pCancel = 0;

	srand(rand());

	int i;
	for (i = 0; i < 10; i++)
	{
		m_slotNumber[i] = (unsigned char)i;
	}

	for (i = 0; i < 10; i++)
	{
		int other = rand() % 10;

		unsigned char digit = m_slotNumber[i];
		m_slotNumber[i] = m_slotNumber[other];
		m_slotNumber[other] = digit;
	}
}

FrItemStorageAccessPanel::~FrItemStorageAccessPanel(void)
{
}

bool FrItemStorageAccessPanel::InputPassword(unsigned char number)
{
	bool ret = false;
	char szNumber[0x20] = "\0";
	char szPrev[2];

	switch (m_formType)
	{
	case 0:
		switch (m_phase)
		{
		case 1:
		{
			int len = strlen(m_password);

			if (len < 4)
			{
				if (len > 0)
				{
					strcpy(szPrev, itoa(m_slotNumber[number], szNumber, 10));
				}

				strcat(m_password, itoa(m_slotNumber[number], szNumber, 10));
			}
			else
			{
				ret = true;
			}
			break;
		}
		case 2:
		{
			int len = strlen(m_confirmPassword);

			if (len < 4)
			{
				if (len > 0)
				{
					strcpy(szPrev, itoa(m_slotNumber[number], szNumber, 10));
				}

				strcat(m_confirmPassword,
					itoa(m_slotNumber[number], szNumber, 10));
			}
			else
			{
				ret = true;
			}
			break;
		}
		}
		break;
	case 1:
	case 3:
	case 4:
	{
		int len = strlen(m_password);

		if (len < 4)
		{
			strcat(m_password, itoa(m_slotNumber[number], szNumber, 10));
		}
		else
		{
			ret = true;
		}
		break;
	}
	case 2:
		switch (m_phase)
		{
		case 0:
		{
			int len = strlen(m_oldPassword);

			if (len < 4)
			{
				strcat(m_oldPassword, itoa(m_slotNumber[number], szNumber, 10));
			}
			else
			{
				ret = true;
			}
			break;
		}
		case 1:
		{
			int len = strlen(m_password);

			if (len < 4)
			{
				if (len > 0)
				{
					strcpy(szPrev, itoa(m_slotNumber[number], szNumber, 10));
				}

				strcat(m_password, itoa(m_slotNumber[number], szNumber, 10));
			}
			else
			{
				ret = true;
			}
			break;
		}
		case 2:
		{
			int len = strlen(m_confirmPassword);

			if (len < 4)
			{
				if (len > 0)
				{
					strcpy(szPrev, itoa(m_slotNumber[number], szNumber, 10));
				}

				strcat(m_confirmPassword,
					itoa(m_slotNumber[number], szNumber, 10));
			}
			else
			{
				ret = true;
			}
			break;
		}
		}
		break;
	}

	if (ret)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pForm->SetMessage(
			"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb4\xc2 4\xc0\xda\xb8\xae\xb8\xa6 \xb3\xd1\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open((FRESH_PFN_RESULT)0, 9);
	}

	return ret;
}

bool FrItemStorageAccessPanel::DiffPassword(void)
{
	bool ret = false;

	if (strcmp(m_password, m_confirmPassword) == 0)
	{
		ret = true;
	}

	return ret;
}

void FrItemStorageAccessPanel::SetButtonImage(FrButton* pButton,
	unsigned char number)
{
	if (pButton)
	{
		char normalImage[0x80] = "\0";
		char overImage[0x80] = "\0";

		unsigned char digit = m_slotNumber[number];
		sprintf(normalImage, "%d_n", digit);
		sprintf(overImage, "%d_o", digit);

		pButton->SetStyle(FrButton::BT_3TYPES);
		pButton->SetButtonImg(normalImage, FrButton::NORMAL);
		pButton->SetButtonImg(overImage, FrButton::OVER);
		pButton->SetButtonImg(overImage, FrButton::PRESSED);
	}
}

IMPLEMENT_OBJECT(FrItemStorageAccessPanel, FrForm)

void FrItemStorageAccessPanel::SendPasswordToServer(void)
{
	switch (m_formType)
	{
	case 0:
	{
		WSendPacket packet((enumClientPacket)0xd2);
		packet.EncodeStr(m_password);
		packet.Send(TO_GAME);
		break;
	}
	case 1:
	{
		WSendPacket packet((enumClientPacket)0xce);
		packet.EncodeStr(m_password);
		packet.Send(TO_GAME);
		break;
	}
	case 2:
	{
		WSendPacket packet((enumClientPacket)0xd3);
		packet.EncodeStr(m_oldPassword);
		packet.EncodeStr(m_password);
		packet.Send(TO_GAME);
		break;
	}
	case 3:
	{
		WSendPacket packet((enumClientPacket)0xd4);
		packet.Encode1(1);
		packet.EncodeStr(m_password);
		packet.Send(TO_GAME);
		break;
	}
	case 4:
	{
		WSendPacket packet((enumClientPacket)0xd4);
		packet.Encode1(0);
		packet.EncodeStr(m_password);
		packet.Send(TO_GAME);
		break;
	}
	}
}

BEGIN_FRESH_MSGMAP(FrItemStorageAccessPanel, FrForm)
ON_FRESH_VI("print_out", FRCMD_INIT, FrItemStorageAccessPanel::OnInitPrintOut)
ON_FRESH_VI("print_out", FRCMD_OWNERDRAW,
	FrItemStorageAccessPanel::OnOwnerDrawPrintOut)
ON_FRESH_VI("btn_slot_0", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot0)
ON_FRESH_VI("btn_slot_1", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot1)
ON_FRESH_VI("btn_slot_2", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot2)
ON_FRESH_VI("btn_slot_3", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot3)
ON_FRESH_VI("btn_slot_4", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot4)
ON_FRESH_VI("btn_slot_5", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot5)
ON_FRESH_VI("btn_slot_6", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot6)
ON_FRESH_VI("btn_slot_7", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot7)
ON_FRESH_VI("btn_slot_8", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot8)
ON_FRESH_VI("btn_slot_9", FRCMD_INIT,
	FrItemStorageAccessPanel::OnInitNumberSlot9)
ON_FRESH_VV("btn_slot_0", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot0)
ON_FRESH_VV("btn_slot_1", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot1)
ON_FRESH_VV("btn_slot_2", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot2)
ON_FRESH_VV("btn_slot_3", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot3)
ON_FRESH_VV("btn_slot_4", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot4)
ON_FRESH_VV("btn_slot_5", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot5)
ON_FRESH_VV("btn_slot_6", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot6)
ON_FRESH_VV("btn_slot_7", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot7)
ON_FRESH_VV("btn_slot_8", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot8)
ON_FRESH_VV("btn_slot_9", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownNumberSlot9)
ON_FRESH_VI("btn_confirm", FRCMD_INIT, FrItemStorageAccessPanel::OnInitConfirm)
ON_FRESH_VV("btn_confirm", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownConfirm)
ON_FRESH_VI("btn_cancel", FRCMD_INIT, FrItemStorageAccessPanel::OnInitCancel)
ON_FRESH_VV("btn_cancel", FRCMD_LBUTTONDOWN,
	FrItemStorageAccessPanel::OnLBDownCancel)
END_FRESH_MSGMAP()

void FrItemStorageAccessPanel::OnInitPrintOut(intptr_t id)
{
	TBindControl(m_pPrintOut, id);
}

void FrItemStorageAccessPanel::OnOwnerDrawPrintOut(intptr_t id)
{
	int len;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	WRect rc = GetRect();

	const Bitmap* pTextBox = g_pFresh->GetBitmap("text_box");
	const Bitmap* pTip = g_pFresh->GetBitmap("btn_tip_o");

	pGDI->SetTextStyle(1);

	switch (m_formType)
	{
	case 0:
	{
		len = strlen(m_password);

		if (m_phase == 1 && pTextBox)
		{
			pGDI->DrawTexture(pTextBox,
				WRect(rc.x + 79.0f, rc.y + 30.0f, 88.0f, 21.0f), 0xFFFFFFFF, 0);
		}

		pGDI->Print(WPoint(rc.x + 20.0f, rc.y + 35.0f), 0,
			"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3");

		if (len > 0 && pTip)
		{
			for (int i = 0; i < len; i++)
			{
				pGDI->DrawTexture(pTip,
					WRect((float)(i * 22.0f) + rc.x + 82.0f, rc.y + 32.0f,
						16.0f, 16.0f),
					0xFFFFFFFF, 0);
			}
		}

		len = strlen(m_confirmPassword);

		if (m_phase == 2 && pTextBox)
		{
			pGDI->DrawTexture(pTextBox,
				WRect(rc.x + 79.0f, rc.y + 55.0f, 88.0f, 21.0f), 0xFFFFFFFF, 0);
		}

		pGDI->Print(WPoint(rc.x + 20.0f, rc.y + 60.0f), 0,
			"\xc8\xae\xc0\xce\xc0\xd4\xb7\xc2");

		if (len > 0 && pTip)
		{
			for (int i = 0; i < len; i++)
			{
				pGDI->DrawTexture(pTip,
					WRect((float)(i * 22.0f) + rc.x + 82.0f, rc.y + 57.0f,
						16.0f, 16.0f),
					0xFFFFFFFF, 0);
			}
		}
		break;
	}
	case 1:
	case 3:
	case 4:
	{
		int len = strlen(m_password);

		pGDI->Print(WPoint(rc.x + 20.0f, rc.y + 55.0f), 0,
			"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3");

		if (pTextBox)
		{
			pGDI->DrawTexture(pTextBox,
				WRect(rc.x + 79.0f, rc.y + 50.0f, 88.0f, 21.0f), 0xFFFFFFFF, 0);
		}

		if (len > 0 && pTip)
		{
			for (int i = 0; i < len; i++)
			{
				pGDI->DrawTexture(pTip,
					WRect((float)(i * 22.0f) + rc.x + 82.0f, rc.y + 52.0f,
						16.0f, 16.0f),
					0xFFFFFFFF, 0);
			}
		}
		break;
	}
	case 2:
		switch (m_phase)
		{
		case 0:
			len = strlen(m_oldPassword);

			pGDI->Print(WPoint(rc.x + 20.0f, rc.y + 55.0f), 0,
				"\xb1\xb8\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3");

			if (pTextBox)
			{
				pGDI->DrawTexture(pTextBox,
					WRect(rc.x + 88.0f, rc.y + 50.0f, 88.0f, 21.0f), 0xFFFFFFFF,
					0);
			}

			if (len > 0 && pTip)
			{
				for (int i = 0; i < len; i++)
				{
					pGDI->DrawTexture(pTip,
						WRect((float)(i * 22.0f) + rc.x + 92.0f, rc.y + 52.0f,
							16.0f, 16.0f),
						0xFFFFFFFF, 0);
				}
			}
			break;
		default:
			pGDI->Print(WPoint(rc.x + 20.0f, rc.y + 35.0f), 0,
				"\xbb\xf5\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3");
			pGDI->Print(WPoint(rc.x + 30.0f, rc.y + 60.0f), 0,
				"\xc8\xae\xc0\xce\xc0\xd4\xb7\xc2");

			if (pTextBox)
			{
				if (m_phase == 1)
				{
					pGDI->DrawTexture(pTextBox,
						WRect(rc.x + 88.0f, rc.y + 30.0f, 88.0f, 21.0f),
						0xFFFFFFFF, 0);
				}
				else
				{
					pGDI->DrawTexture(pTextBox,
						WRect(rc.x + 88.0f, rc.y + 55.0f, 88.0f, 21.0f),
						0xFFFFFFFF, 0);
				}
			}

			len = strlen(m_password);

			if (len > 0 && pTip)
			{
				for (int i = 0; i < len; i++)
				{
					pGDI->DrawTexture(pTip,
						WRect((float)(i * 22.0f) + rc.x + 92.0f, rc.y + 32.0f,
							16.0f, 16.0f),
						0xFFFFFFFF, 0);
				}
			}

			len = strlen(m_confirmPassword);

			if (len > 0 && pTip)
			{
				for (int i = 0; i < len; i++)
				{
					pGDI->DrawTexture(pTip,
						WRect((float)(i * 22.0f) + rc.x + 92.0f, rc.y + 57.0f,
							16.0f, 16.0f),
						0xFFFFFFFF, 0);
				}
			}
			break;
		}
		break;
	}

	pGDI->SetTextStyle(0);
}

void FrItemStorageAccessPanel::OnInitNumberSlot0(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[0], id))
	{
		m_pNumberSlot[0]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[0], 0);
	}
}

void FrItemStorageAccessPanel::OnInitNumberSlot1(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[1], id))
	{
		m_pNumberSlot[1]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[1], 1);
	}
}

void FrItemStorageAccessPanel::OnInitNumberSlot2(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[2], id))
	{
		m_pNumberSlot[2]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[2], 2);
	}
}

void FrItemStorageAccessPanel::OnInitNumberSlot3(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[3], id))
	{
		m_pNumberSlot[3]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[3], 3);
	}
}

void FrItemStorageAccessPanel::OnInitNumberSlot4(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[4], id))
	{
		m_pNumberSlot[4]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[4], 4);
	}
}

void FrItemStorageAccessPanel::OnInitNumberSlot5(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[5], id))
	{
		m_pNumberSlot[5]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[5], 5);
	}
}

void FrItemStorageAccessPanel::OnInitNumberSlot6(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[6], id))
	{
		m_pNumberSlot[6]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[6], 6);
	}
}

void FrItemStorageAccessPanel::OnInitNumberSlot7(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[7], id))
	{
		m_pNumberSlot[7]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[7], 7);
	}
}

void FrItemStorageAccessPanel::OnInitNumberSlot8(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[8], id))
	{
		m_pNumberSlot[8]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[8], 8);
	}
}

void FrItemStorageAccessPanel::OnInitNumberSlot9(intptr_t id)
{
	if (TBindControl(m_pNumberSlot[9], id))
	{
		m_pNumberSlot[9]->SetPushDelay(0.1f);
		SetButtonImage(m_pNumberSlot[9], 9);
	}
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot0(void)
{
	InputPassword(0);
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot1(void)
{
	InputPassword(1);
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot2(void)
{
	InputPassword(2);
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot3(void)
{
	InputPassword(3);
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot4(void)
{
	InputPassword(4);
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot5(void)
{
	InputPassword(5);
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot6(void)
{
	InputPassword(6);
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot7(void)
{
	InputPassword(7);
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot8(void)
{
	InputPassword(8);
}

void FrItemStorageAccessPanel::OnLBDownNumberSlot9(void)
{
	InputPassword(9);
}

void FrItemStorageAccessPanel::OnInitConfirm(intptr_t id)
{
	if (TBindControl(m_pConfirm, id))
	{
		m_pConfirm->SetPushDelay(0.1f);
	}
}

void FrItemStorageAccessPanel::OnLBDownConfirm(void)
{
	switch (m_formType)
	{
	case 0:
		switch (m_phase)
		{
		case 1:
		{
			int len = strlen(m_password);

			if (len < 4)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb4\xc2 \xb9\xdd\xb5\xe5\xbd\xc3 4\xc0\xda\xb8\xae\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xd8\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open((FRESH_PFN_RESULT)0, 9);
			}
			else if (len == 4)
			{
				m_phase = 2;
			}
			break;
		}
		case 2:
		{
			int len = strlen(m_confirmPassword);

			if (len < 4)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb4\xc2 \xb9\xdd\xb5\xe5\xbd\xc3 4\xc0\xda\xb8\xae\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xd8\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open((FRESH_PFN_RESULT)0, 9);
			}
			else if (len == 4)
			{
				if (DiffPassword())
				{
					m_phase = 3;
					Close(FrOK, true);
				}
				else
				{
					FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "notify", 0);
					pForm->SetMessage(
						"\xc0\xd4\xb7\xc2 \xb9\xde\xc0\xba \xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xbc\xad\xb7\xce \xb4\xd9\xb8\xa8\xb4\xcf\xb4\xd9 \xb4\xd9\xbd\xc3 \xc0\xd4\xb7\xc2 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.",
						true);
					pForm->Open((FRESH_PFN_RESULT)0, 9);

					m_phase = 1;
					memset(m_password, 0, sizeof(m_password));
					memset(m_confirmPassword, 0, sizeof(m_confirmPassword));
				}
			}
			break;
		}
		}
		break;
	case 1:
	case 3:
	case 4:
		switch (m_phase)
		{
		case 1:
		{
			int len = strlen(m_password);

			if (len < 4)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb4\xc2 \xb9\xdd\xb5\xe5\xbd\xc3 4\xc0\xda\xb8\xae\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xd8\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
					true);
				pForm->Open((FRESH_PFN_RESULT)0, 9);
			}
			else if (len == 4)
			{
				m_phase = 3;
				Close(FrOK, true);
			}
			break;
		}
		}
		break;
	case 2:
		switch (m_phase)
		{
		case 0:
		{
			int len = strlen(m_oldPassword);

			if (len < 4)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb4\xc2 \xb9\xdd\xb5\xe5\xbd\xc3 4\xc0\xda\xb8\xae\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xd8\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open((FRESH_PFN_RESULT)0, 9);
			}
			else if (len == 4)
			{
				m_phase = 1;
			}
			break;
		}
		case 1:
		{
			int len = strlen(m_password);

			if (len < 4)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb4\xc2 \xb9\xdd\xb5\xe5\xbd\xc3 4\xc0\xda\xb8\xae\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xd8\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open((FRESH_PFN_RESULT)0, 9);
			}
			else if (len == 4)
			{
				m_phase = 2;
			}
			break;
		}
		case 2:
		{
			int len = strlen(m_confirmPassword);

			if (len < 4)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb4\xc2 \xb9\xdd\xb5\xe5\xbd\xc3 4\xc0\xda\xb8\xae\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xd8\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open((FRESH_PFN_RESULT)0, 9);
			}
			else if (len == 4)
			{
				if (DiffPassword())
				{
					m_phase = 3;
					Close(FrOK, true);
				}
				else
				{
					FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "notify", 0);
					pForm->SetMessage(
						"\xc0\xd4\xb7\xc2 \xb9\xde\xc0\xba \xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xbc\xad\xb7\xce \xb4\xd9\xb8\xa8\xb4\xcf\xb4\xd9 \xb4\xd9\xbd\xc3 \xc0\xd4\xb7\xc2 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.",
						true);
					pForm->Open((FRESH_PFN_RESULT)0, 9);

					m_phase = 1;
					memset(m_password, 0, sizeof(m_password));
					memset(m_confirmPassword, 0, sizeof(m_confirmPassword));
				}
			}
			break;
		}
		}
		break;
	}
}

void FrItemStorageAccessPanel::OnInitCancel(intptr_t id)
{
	if (TBindControl(m_pCancel, id))
	{
		m_pCancel->SetPushDelay(0.1f);
	}
}

void FrItemStorageAccessPanel::OnLBDownCancel(void)
{
	switch (m_formType)
	{
	case 0:
		switch (m_phase)
		{
		case 1:
			if (strlen(m_password) != 0)
			{
				memset(m_password, 0, sizeof(m_password));
			}
			else
			{
				Close(FrCANCEL, true);
			}
			break;
		case 2:
			if (strlen(m_confirmPassword) != 0)
			{
				memset(m_confirmPassword, 0, sizeof(m_confirmPassword));
			}
			else
			{
				memset(m_confirmPassword, 0, sizeof(m_confirmPassword));
				m_phase = 1;
			}
			break;
		case 3:
			memset(m_confirmPassword, 0, sizeof(m_confirmPassword));
			m_phase = 2;
			break;
		}
		break;
	case 1:
	case 3:
	case 4:
		m_phase = 1;
		if (strlen(m_password) != 0)
		{
			memset(m_password, 0, sizeof(m_password));
			memset(m_confirmPassword, 0, sizeof(m_confirmPassword));
		}
		else
		{
			Close(FrCANCEL, true);
		}
		break;
	case 2:
		switch (m_phase)
		{
		case 0:
			if (strlen(m_oldPassword) != 0)
			{
				memset(m_oldPassword, 0, sizeof(m_oldPassword));
			}
			else
			{
				Close(FrCANCEL, true);
			}
			break;
		case 1:
			if (strlen(m_password) != 0)
			{
				memset(m_password, 0, sizeof(m_password));
			}
			else
			{
				memset(m_password, 0, sizeof(m_password));
				m_phase = 0;
			}
			break;
		case 2:
			if (strlen(m_confirmPassword) != 0)
			{
				memset(m_confirmPassword, 0, sizeof(m_confirmPassword));
			}
			else
			{
				memset(m_confirmPassword, 0, sizeof(m_confirmPassword));
				m_phase = 1;
			}
			break;
		case 3:
			memset(m_confirmPassword, 0, sizeof(m_confirmPassword));
			m_phase = 2;
			break;
		}
		break;
	}
}

FrItemCounter::FrItemCounter(void)
{
	m_pDrawArea = 0;
	m_pInput = 0;
	m_pConfirm = 0;
	m_pCancel = 0;
	memset(&m_storedItemInfo, 0, sizeof(m_storedItemInfo));
}

FrItemCounter::~FrItemCounter(void)
{
}

IMPLEMENT_OBJECT(FrItemCounter, FrForm)

BEGIN_FRESH_MSGMAP(FrItemCounter, FrForm)
ON_FRESH_VI("draw_area", FRCMD_INIT, FrItemCounter::OnInitDrawArea)
ON_FRESH_VI("draw_area", FRCMD_OWNERDRAW, FrItemCounter::OnOwnerDrawDrawArea)
ON_FRESH_VI("edit_count", FRCMD_INIT, FrItemCounter::OnInitInput)
ON_FRESH_VI("btn_confirm", FRCMD_INIT, FrItemCounter::OnInitConfirm)
ON_FRESH_VV("btn_confirm", FRCMD_LBUTTONDOWN, FrItemCounter::OnLBDownConfirm)
ON_FRESH_VI("btn_cancel", FRCMD_INIT, FrItemCounter::OnInitCancel)
ON_FRESH_VV("btn_cancel", FRCMD_LBUTTONDOWN, FrItemCounter::OnLBDownCancel)
END_FRESH_MSGMAP()

void FrItemCounter::OnInitDrawArea(intptr_t param)
{
	m_pDrawArea = DYNAMIC_CAST(FrArea, param);
}

void FrItemCounter::OnOwnerDrawDrawArea(intptr_t id)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (!pGDI)
		return;

	WRect rc = GetRect();

	unsigned long typeId = m_storedItemInfo.itemInfo.dwTid;
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);

	if (pCommon)
	{
		const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->Icon);
		if (pIcon)
		{
			pGDI->DrawTexture(pIcon,
				WRect(rc.x + 35.0f, rc.y + 40.0f,
					100.0f * ((float)pIcon->Width() * 0.01f),
					100.0f * ((float)pIcon->Height() * 0.01f)),
				0xFFFFFFFF, 0);
		}

		pGDI->Print(WPoint(rc.x + 100.0f, rc.y + 55.0f), 0, pCommon->Name);
	}

	pGDI->Print(WPoint(rc.x + 15.0f, rc.y + 135.0f), 0,
		"\xbc\xf6\xb7\xae\xc0\xd4\xb7\xc2");
}

void FrItemCounter::OnInitInput(intptr_t param)
{
	m_pInput = DYNAMIC_CAST(FrEdit, param);
}

void FrItemCounter::OnInitConfirm(intptr_t param)
{
	m_pConfirm = DYNAMIC_CAST(FrButton, param);

	if (m_pConfirm)
	{
		m_pConfirm->SetPushDelay(0.5f);
	}
}

void FrItemCounter::OnLBDownConfirm(void)
{
	char text[0x20] = "\0";

	if (m_pInput)
	{
		m_pInput->GetWindowTextA(text, sizeof(text));
		if (strlen(text) >= 1)
		{
			unsigned long count = atoi(text);

			if (count > 0 && m_storedItemInfo.GetCount() >= count)
			{
				m_storedItemInfo.SetCount(count);
				Close(FrOK, true);
			}
		}
	}
}

void FrItemCounter::OnInitCancel(intptr_t param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, param);

	if (m_pCancel)
	{
		m_pCancel->SetPushDelay(0.5f);
	}
}

void FrItemCounter::OnLBDownCancel(void)
{
	Close(FrCANCEL, true);
}

FrItemStoragePangBank::FrItemStoragePangBank(void)
{
	m_pDrawArea = 0;
	m_pInput = 0;
	m_pConfirm = 0;
	m_pCancel = 0;
	m_storedPang = 0;
	m_myPang = 0;
}

FrItemStoragePangBank::~FrItemStoragePangBank(void)
{
}

IMPLEMENT_OBJECT(FrItemStoragePangBank, FrForm)

BEGIN_FRESH_MSGMAP(FrItemStoragePangBank, FrForm)
ON_FRESH_VI("draw_area", FRCMD_INIT, FrItemStoragePangBank::OnInitDrawArea)
ON_FRESH_VI("draw_area", FRCMD_OWNERDRAW,
	FrItemStoragePangBank::OnOwnerDrawDrawArea)
ON_FRESH_VI("edit_count", FRCMD_INIT, FrItemStoragePangBank::OnInitInput)
ON_FRESH_BI("edit_count", FRCMD_ENTERKEY,
	FrItemStoragePangBank::OnInputEnterKey)
ON_FRESH_VI("btn_confirm", FRCMD_INIT, FrItemStoragePangBank::OnInitConfirm)
ON_FRESH_VV("btn_confirm", FRCMD_LBUTTONDOWN,
	FrItemStoragePangBank::OnLBDownConfirm)
ON_FRESH_VI("btn_cancel", FRCMD_INIT, FrItemStoragePangBank::OnInitCancel)
ON_FRESH_VV("btn_cancel", FRCMD_LBUTTONDOWN,
	FrItemStoragePangBank::OnLBDownCancel)
END_FRESH_MSGMAP()

void FrItemStoragePangBank::OnInitDrawArea(intptr_t param)
{
	m_pDrawArea = DYNAMIC_CAST(FrArea, param);
}

void FrItemStoragePangBank::OnOwnerDrawDrawArea(intptr_t id)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (!pGDI)
		return;

	WRect rc = GetRect();

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(0x1A000010);

	if (pCommon)
	{
		const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->Icon);
		if (pIcon)
		{
			pGDI->DrawTexture(pIcon,
				WRect(rc.x + 35.0f, rc.y + 40.0f,
					100.0f * ((float)pIcon->Width() * 0.01f),
					100.0f * ((float)pIcon->Height() * 0.01f)),
				0xFFFFFFFF, 0);
		}

		unsigned long inputPang = 0;

		if (m_pInput)
		{
			char text[0x20];
			memset(text, 0, sizeof(text));

			m_pInput->GetWindowTextA(text, sizeof(text));

			if (strlen(text) != 0)
			{
				inputPang = atoi(text);
			}
		}

		__int64 myPang = GetMyPang();
		__int64 storedPang = m_storedPang;
		char myPangText[0x20] = "\0";
		char storedPangText[0x20] = "\0";

		if (m_type == eTYPE_STORE)
		{
			myPang -= inputPang;

			if (myPang < 0)
			{
				strcat(myPangText, "\xc6\xce \xba\xce\xc1\xb7");
				strcat(storedPangText, "\xb0\xe8\xbb\xea \xba\xd2\xb0\xa1");
			}
			else
			{
				MakePriceStrA(myPangText, myPang);
				MakePriceStrA(storedPangText, storedPang + inputPang);
			}
		}
		else if (m_type == eTYPE_TAKE)
		{
			myPang += inputPang;
			storedPang -= inputPang;

			if (storedPang < 0)
			{
				strcat(myPangText, "\xb0\xe8\xbb\xea \xba\xd2\xb0\xa1");
				strcat(storedPangText, "\xc6\xce \xba\xce\xc1\xb7");
			}
			else
			{
				MakePriceStrA(myPangText, myPang);
				MakePriceStrA(storedPangText, storedPang);
			}
		}

		pGDI->Print(WPoint(rc.x + 100.0f, rc.y + 55.0f), 0,
			"\xba\xb8\xc0\xaf \xc6\xce :");
		pGDI->Print(WPoint(rc.x + 100.0f, rc.y + 75.0f), 0,
			"\xba\xb8\xb0\xfc \xc6\xce :");

		if (m_type == eTYPE_STORE)
		{
			pGDI->Print(WPoint(rc.x + 240.0f, rc.y + 75.0f), 2, "%s P",
				storedPangText);

			if (inputPang > 0)
			{
				pGDI->SetTextColor(0xFF0000FF, 0xFFFFFFFF);
				pGDI->SetTextStyle(1);
			}

			pGDI->Print(WPoint(rc.x + 240.0f, rc.y + 55.0f), 2, "%s P",
				myPangText);
		}
		else if (m_type == eTYPE_TAKE)
		{
			pGDI->Print(WPoint(rc.x + 240.0f, rc.y + 55.0f), 2, "%s P",
				myPangText);

			if (inputPang > 0)
			{
				pGDI->SetTextColor(0xFF0000FF, 0xFFFFFFFF);
				pGDI->SetTextStyle(1);
			}

			pGDI->Print(WPoint(rc.x + 240.0f, rc.y + 75.0f), 2, "%s P",
				storedPangText);
		}

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);
	}

	pGDI->Print(WPoint(rc.x + 15.0f, rc.y + 135.0f), 0,
		"\xc6\xce \xc0\xd4\xb7\xc2");
}

void FrItemStoragePangBank::OnInitInput(intptr_t param)
{
	m_pInput = DYNAMIC_CAST(FrEdit, param);
}

bool FrItemStoragePangBank::OnInputEnterKey(int param)
{
	OnLBDownConfirm();

	return true;
}

void FrItemStoragePangBank::OnInitConfirm(intptr_t param)
{
	m_pConfirm = DYNAMIC_CAST(FrButton, param);

	if (m_pConfirm)
	{
		m_pConfirm->SetPushDelay(0.5f);
	}
}

void FrItemStoragePangBank::OnLBDownConfirm(void)
{
	char text[32] = "\0";
	int pang = 0;
	bool bValid = false;

	if (m_pInput)
	{
		m_pInput->GetWindowTextA(text, 32);
		if (strlen(text) >= 1)
		{
			pang = atoi(text);

			if (m_type == eTYPE_TAKE)
			{
				if (pang > 0 && m_storedPang >= pang)
				{
					bValid = true;
				}
			}
			else
			{
				if (pang > 0 && m_myPang >= pang)
				{
					bValid = true;
				}
			}

			if (bValid)
			{
				m_storedPang = pang;
				Close(FrOK, true);
				return;
			}
		}
	}

	__int64 limit = m_type == eTYPE_TAKE ? m_storedPang : m_myPang;
	if (pang > limit)
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23,
			(intptr_t)"\xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
	}
	else if (pang <= 0)
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23,
			(intptr_t)"\xc0\xdf\xb8\xf8\xb5\xc8 \xb1\xdd\xbe\xd7\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
	}

	m_pInput->ClearLine();
}

void FrItemStoragePangBank::OnInitCancel(intptr_t param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, param);

	if (m_pCancel)
	{
		m_pCancel->SetPushDelay(0.5f);
	}
}

void FrItemStoragePangBank::OnLBDownCancel(void)
{
	Close(FrCANCEL, true);
}

FrItemConfirm::FrItemConfirm(void)
{
	m_pDrawArea = 0;
	m_pConfirm = 0;
	m_pCancel = 0;
	memset(&m_storedItemInfo, 0, sizeof(m_storedItemInfo));
}

FrItemConfirm::~FrItemConfirm(void)
{
}

IMPLEMENT_OBJECT(FrItemConfirm, FrForm)

BEGIN_FRESH_MSGMAP(FrItemConfirm, FrForm)
ON_FRESH_VI("draw_area", FRCMD_INIT, FrItemConfirm::OnInitDrawArea)
ON_FRESH_VI("draw_area", FRCMD_OWNERDRAW, FrItemConfirm::OnOwnerDrawDrawArea)
ON_FRESH_VI("btn_confirm", FRCMD_INIT, FrItemConfirm::OnInitConfirm)
ON_FRESH_VV("btn_confirm", FRCMD_LBUTTONDOWN, FrItemConfirm::OnLBDownConfirm)
ON_FRESH_VI("btn_cancel", FRCMD_INIT, FrItemConfirm::OnInitCancel)
ON_FRESH_VV("btn_cancel", FRCMD_LBUTTONDOWN, FrItemConfirm::OnLBDownCancel)
END_FRESH_MSGMAP()

void FrItemConfirm::OnInitDrawArea(intptr_t param)
{
	m_pDrawArea = DYNAMIC_CAST(FrArea, param);
}

void FrItemConfirm::OnOwnerDrawDrawArea(intptr_t id)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (!pGDI)
		return;

	WRect rc = GetRect();

	if (IsLocalContent((localContentType_t)0x57) &&
		IsUccClothes(m_storedItemInfo.itemInfo.dwTid &&
			(m_storedItemInfo.itemInfo.status & 1)))
	{
		const Bitmap* pUccIcon = GetUccItemIcon(m_storedItemInfo.itemInfo.dwTid,
			m_storedItemInfo.itemInfo.UccIndex);

		if (pUccIcon)
		{
			pGDI->DrawTexture(pUccIcon,
				WRect(rc.x + 35.0f, rc.y + 40.0f,
					100.0f * ((float)pUccIcon->Width() * 0.01f),
					100.0f * ((float)pUccIcon->Height() * 0.01f)),
				0xFFFFFFFF, 0);
		}

		pGDI->Print(WPoint(rc.x + 110.0f, rc.y + 55.0f), 0,
			GetUccItemName(m_storedItemInfo.itemInfo.dwGuid));
	}
	else
	{
		unsigned long typeId = m_storedItemInfo.itemInfo.dwTid;
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);

		if (pCommon)
		{
			const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->Icon);
			if (pIcon)
			{
				pGDI->DrawTexture(pIcon,
					WRect(rc.x + 35.0f, rc.y + 40.0f,
						100.0f * ((float)pIcon->Width() * 0.01f),
						100.0f * ((float)pIcon->Height() * 0.01f)),
					0xFFFFFFFF, 0);
			}

			pGDI->Print(WPoint(rc.x + 110.0f, rc.y + 55.0f), 0, pCommon->Name);
		}
	}
}

void FrItemConfirm::OnInitConfirm(intptr_t param)
{
	m_pConfirm = DYNAMIC_CAST(FrButton, param);

	if (m_pConfirm)
	{
		m_pConfirm->SetPushDelay(0.5f);
	}
}

void FrItemConfirm::OnLBDownConfirm(void)
{
	if (m_storedItemInfo.GetCount() > 0)
	{
		Close(FrOK, true);
	}
}

void FrItemConfirm::OnInitCancel(intptr_t param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, param);

	if (m_pCancel)
	{
		m_pCancel->SetPushDelay(0.5f);
	}
}

void FrItemConfirm::OnLBDownCancel(void)
{
	Close(FrCANCEL, true);
}

CItemStorage::CItemStorage(void)
{
	m_bVisible = false;
	m_bLock = false;

	m_frontUI.clear();
	m_backEnableUI.clear();
	m_backVisibleUI.clear();

	m_pTitleFont = 0;
	m_storedPang = 0;
	m_totalPage = 0;
	m_curPage = 0;
	m_pIndexControl = 0;

	TSafeNew<CIndexControl>(m_pIndexControl, 1);
	m_storedItemList.clear();

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	m_pPangBank = 0;
	m_pItemCounter = 0;
	m_pItemConfirm = 0;

	static char* s_fontFile = "[font_wind.jpg";
	info.filename = &s_fontFile;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:,";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	memset(&m_dragDrop, 0, sizeof(m_dragDrop));
}

CItemStorage::~CItemStorage(void)
{
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = 0;
	}
	TSafeDelete(m_pIndexControl);
}

void CItemStorage::Lock(void)
{
	m_bLock = true;

	std::list<FrWnd*>::iterator it;
	for (it = m_frontUI.begin(); it != m_frontUI.end(); ++it)
	{
		if (*it)
		{
			(*it)->Enable(false);
		}
	}
}

void CItemStorage::UnLock(void)
{
	m_bLock = false;

	std::list<FrWnd*>::iterator it;
	for (it = m_frontUI.begin(); it != m_frontUI.end(); ++it)
	{
		if (*it)
		{
			(*it)->Enable(true);
		}
	}
}

void CItemStorage::AddFrontUI(FrWnd* pWnd)
{
	if (pWnd)
	{
		pWnd->SetVisible(false);
		m_frontUI.push_back(pWnd);
	}
}

void CItemStorage::AddBackGroundUI(FrWnd* pWnd, bool bVisibleGroup)
{
	if (pWnd)
	{
		if (!bVisibleGroup)
		{
			m_backEnableUI.push_back(pWnd);
		}
		else
		{
			m_backVisibleUI.push_back(pWnd);
		}
	}
}

void CItemStorage::SetVisibleFrontUI(bool bVisible)
{
	std::list<FrWnd*>::iterator it;
	for (it = m_frontUI.begin(); it != m_frontUI.end(); ++it)
	{
		m_bVisible = bVisible;

		FrWnd* pWnd = *it;
		if (pWnd)
		{
			pWnd->SetVisible(bVisible);
		}
	}
}

void CItemStorage::SetVisibleBackGroundUI(bool bVisible)
{
	std::list<FrWnd*>::iterator it;
	for (it = m_backVisibleUI.begin(); it != m_backVisibleUI.end(); ++it)
	{
		FrWnd* pWnd = *it;
		if (pWnd)
		{
			pWnd->SetVisible(bVisible);
		}
	}
}

void CItemStorage::SetEnableFrontUI(bool bEnable)
{
	std::list<FrWnd*>::iterator it;
	for (it = m_frontUI.begin(); it != m_frontUI.end(); ++it)
	{
		FrWnd* pWnd = *it;
		if (pWnd)
		{
			pWnd->Enable(bEnable);
		}
	}
}

void CItemStorage::SetEnableBackGroundUI(bool bEnable)
{
	std::list<FrWnd*>::iterator it;
	for (it = m_backEnableUI.begin(); it != m_backEnableUI.end(); ++it)
	{
		FrWnd* pWnd = *it;
		if (pWnd)
		{
			pWnd->Enable(bEnable);
		}
	}
}

void CItemStorage::Print(float x, float y, const char* text, float spacing,
	int align, float scale, unsigned long color)
{
	const char* p = text;
	if (!p)
	{
		return;
	}

	if (strlen(p) == 0)
	{
		return;
	}

	const char* charSet = m_pTitleFont->GetFontInfo()->pCharSet;
	float oldScale = m_pTitleFont->GetScale();

	m_pTitleFont->SetScale(scale);
	float width = 0.0f;

	switch (align)
	{
	case 1:
		while (*p)
		{
			int ch = *p++;

			const char* found = strchr(charSet, ch);

			width += m_pTitleFont->GetCharWidthA(g_view, found - charSet) +
				spacing * scale;
		}
		p = text;
		width = (width - spacing) * 0.5f;
		break;
	case 2:
		while (*p)
		{
			int ch = *p++;

			const char* found = strchr(charSet, ch);

			width += m_pTitleFont->GetCharWidthA(g_view, found - charSet) +
				spacing * scale;
		}
		p = text;
		width -= spacing;
		break;
	}

	x -= width;

	while (*p)
	{
		int ch = *p++;

		const char* found = strchr(charSet, ch);

		float advance =
			m_pTitleFont->PutChar(g_view, x, y, found - charSet, 0, color);
		advance += spacing;
		x += advance;
	}

	m_pTitleFont->SetScale(oldScale);
	m_pTitleFont->Flush(g_view);
}

void CItemStorage::ClearStoredItemInfo(void)
{
	std::vector<sStoredItemInfo*>::iterator it;
	for (it = m_storedItemList.begin(); it != m_storedItemList.end(); ++it)
	{
		TSafeDelete(*it);
	}

	std::vector<sItemInfo*>::iterator itItem;
	for (itItem = m_uccItemList.begin(); itItem != m_uccItemList.end();
		++itItem)
	{
		TSafeDelete(*itItem);
	}

	m_storedItemList.clear();
}

void CItemStorage::AddStoredItemInfo(const sStoredItemInfo& info)
{
	sStoredItemInfo* pStored = 0;
	TSafeNew<sStoredItemInfo>(pStored, 1);

	if (pStored)
	{
		memcpy(pStored, &info, sizeof(sStoredItemInfo));
		m_storedItemList.push_back(pStored);

		if (IsLocalContent((localContentType_t)0x57) &&
			IsUccClothes(pStored->itemInfo.dwTid))
		{
			sItemInfo* pItem = new sItemInfo;
			memset(pItem, 0, sizeof(sItemInfo));

			if (pItem)
			{
				pItem->tid = pStored->itemInfo.dwTid;
				pItem->guid = pStored->itemInfo.dwGuid;
				pItem->Seq = pStored->itemInfo.Seq;
				pItem->status = pStored->itemInfo.status;
				memcpy(pItem->ItemName, pStored->itemInfo.ItemName,
					sizeof(pItem->ItemName));
				memcpy(pItem->UccIndex, pStored->itemInfo.UccIndex,
					sizeof(pItem->UccIndex));

				m_uccItemList.push_back(pItem);

				sUccClothes clothes;
				clothes.id = pItem->guid;
				clothes.typeId = pItem->tid;
				strncpy(clothes.uccIndex, pItem->UccIndex, 9);
				strncpy(clothes.uccName, pItem->ItemName, 0x29);
				clothes.downloadCount = !(pStored->itemInfo.status & 1);

				UccManager()->AddClothes(0, clothes, true, false);
			}
		}
	}
}

bool CItemStorage::CheckEquiped(unsigned long id)
{
	bool ret = false;

	std::map<unsigned int, sCharacterInfo>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_charMap.begin();
		it != WSingleton<CSharedDoc>::Instance()->m_charMap.end(); ++it)
	{
		for (int i = 0; i < 24; i++)
		{
			if (it->second.ItemIdList[i] != 0)
			{
				if (it->second.ItemIdList[i] == id)
				{
					ret = true;
					break;
				}
			}
		}

		if (ret)
		{
			break;
		}
	}

	return ret;
}

bool CItemStorage::CheckStoredItemInfo(sStoredItemInfo& stored,
	const sItemInfo& info)
{
	bool ret = false;

	switch (info.tid >> 26)
	{
	case 2:
	case 4:
	case 5:
	case 6:
	case 28:
		break;
	default:
		return false;
	}

	memset(&stored, 0, sizeof(stored));

	stored.SetTypeID(info.tid);
	stored.SetGuid(info.guid);
	strcpy(stored.itemInfo.CopierNick, info.CopierNick);
	strcpy(stored.itemInfo.UccIndex, info.UccIndex);

	switch (stored.itemInfo.dwTid >> 26)
	{
	case 28:
		if (info.Common[0] > 1)
		{
			ret = false;
			break;
		}
		stored.SetCount(1);
		ret = true;
		break;
	case 2:
		stored.SetCount(1);
		ret = true;
		break;
	case 4:
		break;
	case 5:
	case 6:
		ret = false;
		break;
	default:
		ret = false;
		break;
	}

	if (ret == true)
	{
		ret = true;
	}
	else
	{
		memset(&stored, 0, sizeof(stored));
	}

	return ret;
}

void CItemStorage::ReqPopStoredItemInfo(const FrListItem& item)
{
	const sStoredItemInfo* pInfo = (const sStoredItemInfo*)item.pData;

	if (pInfo)
	{
		ReqPopStoredItemInfo(*pInfo);
	}
}

void CItemStorage::ReqPopStoredItemInfo(const sStoredItemInfo& info)
{
	if (info.GetCount() > 1)
	{
		if (m_pItemCounter == 0)
		{
			m_pItemCounter = CreateForm<FrItemCounter>(g_pFresh->GetManager(),
				this, "item_storage_item_counter", 0);
		}

		if (m_pItemCounter)
		{
			FrItemCounter* pForm = m_pItemCounter;

			pForm->SetType(1);
			m_pItemCounter->SetStoredItemInfo(info);
			m_pItemCounter->Open(
				(FRESH_PFN_RESULT)&CItemStorage::OnResultItemCounter, 9);
		}
	}
	else if (info.GetCount() == 1)
	{
		if (m_pItemConfirm == 0)
		{
			m_pItemConfirm = CreateForm<FrItemConfirm>(g_pFresh->GetManager(),
				this, "item_storage_item_confirm", 0);
		}

		if (m_pItemConfirm)
		{
			FrItemConfirm* pForm = m_pItemConfirm;

			pForm->SetType(1);
			m_pItemConfirm->SetStoredItemInfo(info);
			m_pItemConfirm->Open(
				(FRESH_PFN_RESULT)&CItemStorage::OnResultItemConfirm, 9);
		}
	}
}

void CItemStorage::ReqPushStoredItemInfo(const FrListItem& item)
{
	const sItemInfo* pInfo = (const sItemInfo*)item.pData;

	if (pInfo)
	{
		ReqPushStoredItemInfo(*pInfo);
	}
}

void CItemStorage::ReqPushStoredItemInfo(const sItemInfo& info)
{
	sStoredItemInfo stored;

	if (CheckStoredItemInfo(stored, info) == true)
	{
		if (!CheckEquiped(stored.itemInfo.dwGuid))
		{
			if ((stored.itemInfo.dwTid & 0xFC000000) == 0x08000000)
			{
				CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
				std::list<sItemInfo>::iterator it = std::find_if(
					pDoc->m_partsList.begin(), pDoc->m_partsList.end(),
					_FindByGuid(stored.itemInfo.dwGuid));
				if (it != pDoc->m_partsList.end())
				{
					if (it->ItemType == 6)
					{
						FrForm* pForm = CreateForm<FrForm>(
							g_pFresh->GetManager(), this, "notify", 0);
						pForm->SetMessage(
							"\xb4\xeb\xbf\xa9 \xc0\xc7\xbb\xf3\xc0\xba \xc3\xa2\xb0\xed\xbf\xa1 \xba\xb8\xb0\xfc \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
							true);
						pForm->Open((FRESH_PFN_RESULT)0, 9);
						return;
					}
				}
			}

			if (stored.GetCount() > 1)
			{
				if (m_pItemCounter == 0)
				{
					m_pItemCounter =
						CreateForm<FrItemCounter>(g_pFresh->GetManager(), this,
							"item_storage_item_counter", 0);
				}

				if (m_pItemCounter)
				{
					FrItemCounter* pForm = m_pItemCounter;

					pForm->SetType(0);
					m_pItemCounter->SetStoredItemInfo(stored);
					m_pItemCounter->Open(
						(FRESH_PFN_RESULT)&CItemStorage::OnResultItemCounter,
						9);
				}
			}
			else if (stored.GetCount() == 1)
			{
				if (m_pItemConfirm == 0)
				{
					m_pItemConfirm =
						CreateForm<FrItemConfirm>(g_pFresh->GetManager(), this,
							"item_storage_item_confirm", 0);
				}

				if (m_pItemConfirm)
				{
					FrItemConfirm* pForm = m_pItemConfirm;

					pForm->SetType(0);
					m_pItemConfirm->SetStoredItemInfo(stored);
					m_pItemConfirm->Open(
						(FRESH_PFN_RESULT)&CItemStorage::OnResultItemConfirm,
						9);
				}
			}
			else if (stored.GetCount() == 0)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", 0);
				pForm->SetMessage(
					"\xba\xb8\xb0\xfc \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb \xc0\xd4\xb4\xcf\xb4\xd9.",
					true);
				pForm->Open((FRESH_PFN_RESULT)0, 9);
			}
		}
		else
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage(
				"\xc0\xe5\xc2\xf8\xc1\xdf\xc0\xce \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xba\xb8\xb0\xfc \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				true);
			pForm->Open((FRESH_PFN_RESULT)0, 9);
		}
	}
	else
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pForm->SetMessage(
			"\xba\xb8\xb0\xfc \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb \xc0\xd4\xb4\xcf\xb4\xd9.",
			true);
		pForm->Open((FRESH_PFN_RESULT)0, 9);
	}
}

void CItemStorage::DrawBackgroundImage(void)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	const Bitmap* pBg1 = g_pFresh->GetBitmap("bg_01");
	const Bitmap* pBg2 = g_pFresh->GetBitmap("bg_02");
	const Bitmap* pBg3 = g_pFresh->GetBitmap("bg_03");
	const Bitmap* pBg4 = g_pFresh->GetBitmap("bg_04");

	if (pBg1 && pBg2 && pBg3 && pBg4)
	{
		pGDI->DrawTexture(pBg1,
			WRect(30.0f, 74.0f, (float)pBg1->Width(), (float)pBg1->Height()),
			0xFFFFFFFF, 0);
		pGDI->DrawTexture(pBg2,
			WRect(pBg1->Width() + 30.0f, 74.0f, (float)pBg2->Width(),
				(float)pBg2->Height()),
			0xFFFFFFFF, 0);
		pGDI->DrawTexture(pBg3,
			WRect(37.0f, pBg1->Height() + 74.0f, (float)pBg3->Width(),
				(float)pBg3->Height()),
			0xFFFFFFFF, 0);
		pGDI->DrawTexture(pBg4,
			WRect(pBg3->Width() + 37.0f, pBg2->Height() + 74.0f,
				(float)pBg4->Width(), (float)pBg4->Height()),
			0xFFFFFFFF, 0);
	}

	char price[32];
	memset(price, 0, sizeof(price));
	MakePriceStrA(price, GetStoredPang());

	Print(329.0f, 131.0f, price, 2.0f, 2, 1.0f, 0xFFFFD18E);
}

void CItemStorage::MakeStoredItemInfo(FrListBox*& pListBox)
{
	if (pListBox)
	{
		pListBox->ClearItem();

		for (std::vector<sStoredItemInfo*>::iterator it =
				 m_storedItemList.begin();
			it != m_storedItemList.end(); ++it)
		{
			sStoredItemInfo* pInfo = *it;
			if (pInfo)
			{
				pListBox->AddItem(pInfo);
			}
		}
	}
}

void CItemStorage::DrawStoredItemInfo(const FrListBox*& pListBox,
	const FrListItem& item)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI && pListBox)
	{
		const Bitmap* pBg;

		if (item.selected == false && item.underCursor == false)
		{
			pBg = g_pFresh->GetBitmap("button_n");
		}
		else
		{
			pBg = g_pFresh->GetBitmap("button_o");
		}

		if (pBg)
		{
			pGDI->DrawTexture(pBg,
				WRect(item.pos.x, item.pos.y, (float)pBg->Width(),
					(float)pBg->Height()),
				0xFFFFFFFF, 0);
		}

		const sStoredItemInfo* pInfo = (const sStoredItemInfo*)item.pData;

		if (pInfo)
		{
			unsigned long typeId = pInfo->itemInfo.dwTid;
			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);

			if (pCommon)
			{
				unsigned long charTypeId =
					((pInfo->itemInfo.dwTid >> 18) & 0xFF) | 0x04000000;
				IFF_ITEM_COMMON* pChar =
					ItemManager()->FindCommonItem(charTypeId);

				if (pChar)
				{
					const Bitmap* pCharIcon =
						g_pFresh->GetBitmap(MakeStr("s_%s", pChar->Icon));

					if (pCharIcon)
					{
						pGDI->DrawTexture(pCharIcon,
							WRect(item.pos.x + 5.0f, item.pos.y + 3.0f,
								90.0f * ((float)pCharIcon->Width() * 0.01f),
								90.0f * ((float)pCharIcon->Height() * 0.01f)),
							0xFFFFFFFF, 0);
					}
				}

				const Bitmap* pItemIcon = g_pFresh->GetBitmap(pCommon->Icon);

				const Bitmap* pIcon =
					(IsLocalContent((localContentType_t)0x57) &&
						IsUccClothes(pInfo->itemInfo.dwTid) &&
						(pInfo->itemInfo.status & 1))
					? GetUccItemIcon(pInfo->itemInfo.dwTid,
						  pInfo->itemInfo.UccIndex)
					: pItemIcon;

				unsigned long dragIndex = (unsigned long)-1;
				if (m_dragDrop.mode == sStoredItemDragDrop::MODE_STORAGE_LIST)
				{
					dragIndex = m_dragDrop.index;
				}

				if (dragIndex != (unsigned long)item.idx && pIcon)
				{
					pGDI->DrawTexture(pIcon,
						WRect(item.pos.x + 30.0f, item.pos.y + 1.0f,
							34.0f * ((float)pIcon->Width() * 0.01f),
							34.0f * ((float)pIcon->Height() * 0.01f)),
						0xFFFFFFFF, 0);
				}

				if (item.selected == true)
				{
					pGDI->SetTextStyle(1);
					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
				}
				else
				{
					pGDI->SetTextStyle(0);
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				}

				if (IsLocalContent((localContentType_t)0x57) &&
					IsUccClothes(pInfo->itemInfo.dwTid) &&
					(pInfo->itemInfo.status & 1))
				{
					pGDI->Print(WPoint(item.pos.x + 61.0f, item.pos.y + 10.0f),
						0,
						GetUccItemName(pInfo->itemInfo.dwTid,
							pInfo->itemInfo.UccIndex));
				}
				else
				{
					pGDI->Print(WPoint(item.pos.x + 61.0f, item.pos.y + 10.0f),
						0, pCommon->Name);
				}

				switch (pInfo->itemInfo.dwTid >> 26)
				{
				case 2:
					break;
				default:
					pGDI->Print(WPoint(item.pos.x + 220.0f, item.pos.y + 10.0f),
						0, "%d\xb0\xb3", pInfo->itemInfo.iNum);
					break;
				}

				pGDI->SetTextStyle(0);
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			}
		}
	}
}

void CItemStorage::InitIndexControl(unsigned long totalCount,
	unsigned long countPerPage)
{
	if (m_pIndexControl)
	{
		m_pIndexControl->Init(totalCount, countPerPage);
		m_pIndexControl->SetSelectedNumber(m_curPage);
	}
}

void CItemStorage::IndexControlClick(void)
{
	if (m_pIndexControl)
	{
		if (m_pIndexControl->Click() == true)
		{
			unsigned long selectedNumber = m_pIndexControl->GetSelectedNumber();

			WSendPacket packet((enumClientPacket)0xcf);
			packet.Encode4(0x63);
			packet.Encode2((unsigned char)selectedNumber);
			packet.Send(TO_GAME);
			Lock();

			m_curPage = selectedNumber;
		}
	}
}

void CItemStorage::IndexControlNext(void)
{
	if (m_pIndexControl)
	{
		if (m_pIndexControl->NextPage() == true)
		{
			unsigned long selectedNumber = m_pIndexControl->GetSelectedNumber();

			WSendPacket packet((enumClientPacket)0xcf);
			packet.Encode4(0x63);
			packet.Encode2((unsigned char)selectedNumber);
			packet.Send(TO_GAME);
			Lock();

			m_curPage = selectedNumber;
		}
	}
}

void CItemStorage::IndexControlPrev(void)
{
	if (m_pIndexControl)
	{
		if (m_pIndexControl->PrevPage() == true)
		{
			unsigned long selectedNumber = m_pIndexControl->GetSelectedNumber();

			WSendPacket packet((enumClientPacket)0xcf);
			packet.Encode4(0x63);
			packet.Encode2((unsigned char)selectedNumber);
			packet.Send(TO_GAME);
			Lock();

			m_curPage = selectedNumber;
		}
	}
}

void CItemStorage::IndexControlDraw(WPoint& pt)
{
	if (m_pIndexControl)
	{
		m_pIndexControl->Draw(pt);
	}
}

bool CItemStorage::NeedPageIndexer(void)
{
	bool ret = false;

	if (m_pIndexControl)
	{
		unsigned long totalPage = m_pIndexControl->GetTotalPage();

		if (totalPage > 1)
		{
			ret = true;
		}
	}

	return ret;
}

void CItemStorage::OpenPangBank(FrItemStoragePangBank::eType type)
{
	if (m_pPangBank == 0)
	{
		m_pPangBank = CreateForm<FrItemStoragePangBank>(g_pFresh->GetManager(),
			this, "item_storage_pang_bank", 0);
	}

	if (m_pPangBank)
	{
		m_pPangBank->SetStoredPang(m_storedPang);
		unsigned long myPang = (unsigned long)WSingleton<CSharedDoc>::Instance()
								   ->m_myInfo.stat.i64Pang;
		m_pPangBank->SetMyPang(myPang);
		m_pPangBank->SetType(type);
		m_pPangBank->Open((FRESH_PFN_RESULT)&CItemStorage::OnResultPangBank, 9);
	}
}

bool CItemStorage::OnResultPangBank(int ret, FrForm* pForm)
{
	FrItemStoragePangBank* pPangBank = (FrItemStoragePangBank*)pForm;

	if (ret == FrOK && pPangBank)
	{
		WSendPacket packet((enumClientPacket)0xd6);

		if (pPangBank->GetType() == FrItemStoragePangBank::eTYPE_TAKE)
		{
			packet.Encode1(0);
		}
		else
		{
			packet.Encode1(1);
		}

		packet.Encode8(pPangBank->GetStoredPang());
		packet.Send(TO_GAME);
		Lock();
	}

	m_pPangBank = 0;

	return true;
}

bool CItemStorage::OnResultItemCounter(int ret, FrForm* pForm)
{
	bool result = false;

	if (ret == FrOK)
	{
		FrItemCounter* pCounter = (FrItemCounter*)pForm;

		if (pCounter)
		{
			sStoredItemInfo info = pCounter->GetStoredItemInfo();

			switch (pCounter->GetType())
			{
			case 1:
			{
				WSendPacket packet((enumClientPacket)0xd1);
				packet.Encode1(1);
				packet.EncodeBuffer(&info, sizeof(info));
				packet.Send(TO_GAME);
				Lock();

				result = true;
				break;
			}
			case 0:
			{
				WSendPacket packet((enumClientPacket)0xd0);
				packet.Encode1(1);
				packet.EncodeBuffer(&info, sizeof(info));
				packet.Send(TO_GAME);
				Lock();

				result = true;
				break;
			}
			}
		}
	}
	else
	{
		result = true;
	}

	m_pItemCounter = 0;

	return result;
}

bool CItemStorage::OnResultItemConfirm(int ret, FrForm* pForm)
{
	bool result = false;

	if (ret == FrOK)
	{
		FrItemConfirm* pConfirm = (FrItemConfirm*)pForm;

		if (pConfirm)
		{
			sStoredItemInfo info = pConfirm->GetStoredItemInfo();

			switch (pConfirm->GetType())
			{
			case 1:
			{
				WSendPacket packet((enumClientPacket)0xd1);
				packet.Encode1(1);
				packet.EncodeBuffer(&info, sizeof(info));
				packet.Send(TO_GAME);
				Lock();

				result = true;
				break;
			}
			case 0:
			{
				WSendPacket packet((enumClientPacket)0xd0);
				packet.Encode1(1);
				packet.EncodeBuffer(&info, sizeof(info));
				packet.Send(TO_GAME);
				Lock();

				result = true;
				break;
			}
			}
		}
	}
	else
	{
		result = true;
	}

	m_pItemConfirm = 0;

	return result;
}

bool CItemStorage::CheckDrag(void)
{
	bool ret = false;

	if (g_input->GetButton(LEFT_BUTTON) == 3)
	{
		ret = true;

		const WPoint& pt = g_pFresh->GetManager()->GetMousePos();

		m_dragDrop.mouse.x = pt.x;
		m_dragDrop.mouse.y = pt.y;
	}

	return ret;
}

bool CItemStorage::CheckDrop(void)
{
	const WPoint& pt = g_pFresh->GetManager()->GetMousePos();

	if (g_input->GetButton(LEFT_BUTTON) == 2)
	{
		switch (m_dragDrop.mode)
		{
		case sStoredItemDragDrop::MODE_STORAGE_LIST:
			if (m_itemListArea.IsInRect(pt) == true)
			{
				ReqPopStoredItemInfo(m_dragDrop.storedItemInfo);
			}
			break;
		case sStoredItemDragDrop::MODE_ITEM_LIST:
			if (m_itemStorageArea.IsInRect(pt) == true)
			{
				if (m_dragDrop.pItemInfo)
				{
					ReqPushStoredItemInfo(*m_dragDrop.pItemInfo);
				}
			}
			break;
		}

		memset(&m_dragDrop, 0, sizeof(m_dragDrop));
	}

	return false;
}

void CItemStorage::SetDragDropItem(sStoredItemDragDrop& dragDrop)
{
	memcpy(&m_dragDrop, &dragDrop, sizeof(m_dragDrop));
}

void CItemStorage::SetDrag(sStoredItemDragDrop::MODE mode)
{
	if (m_dragDrop.storedItemInfo.GetTypeID())
	{
		if (m_dragDrop.mode == sStoredItemDragDrop::MODE_NONE)
		{
			m_dragDrop.mode = mode;
		}
	}
}

void CItemStorage::DragDropItemDraw(void)
{
	if (CheckDrag())
	{
		if (m_dragDrop.mode != sStoredItemDragDrop::MODE_NONE)
		{
			FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
			unsigned long typeId = m_dragDrop.storedItemInfo.itemInfo.dwTid;
			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);

			if (pGDI && pCommon)
			{
				const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->Icon);

				if (IsLocalContent((localContentType_t)0x57) &&
					IsUccClothes(pCommon->TypeId) &&
					(m_dragDrop.storedItemInfo.itemInfo.status & 1))
				{
					const Bitmap* pUccIcon =
						GetUccItemIcon(m_dragDrop.storedItemInfo.itemInfo.dwTid,
							m_dragDrop.storedItemInfo.itemInfo.UccIndex);

					if (pUccIcon)
					{
						pIcon = pUccIcon;
					}
				}

				if (pIcon)
				{
					float w = (float)pIcon->Width();
					float h = (float)pIcon->Height();

					pGDI->DrawTexture(pIcon,
						WRect(m_dragDrop.mouse.x - w * 0.5f,
							m_dragDrop.mouse.y - h * 0.5f, w, h),
						0xFFFFFFFF, 0);
				}
			}
		}
	}
	else
	{
		CheckDrop();
	}
}

unsigned long CItemStorage::GetDragingItemIndex(sStoredItemDragDrop::MODE mode)
{
	unsigned long index = (unsigned long)-1;

	if (m_dragDrop.mode == mode)
	{
		index = m_dragDrop.index;
	}

	return index;
}
