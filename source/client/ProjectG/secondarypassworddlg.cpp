#include "secondarypassworddlg.h"
#include "messagemanager.h"
#include "gatewayactor.h"
#include "s5/utilities.h"
#include <mbstring.h>

extern Fresh* g_pFresh;

namespace _gateway
{
	IMPLEMENT_OBJECT(FrPasswordInputDlg, FrForm)

	BEGIN_FRESH_MSGMAP(FrPasswordInputDlg, FrForm)
	ON_FRESH_VI("btn_slot_0", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot0)
	ON_FRESH_VI("btn_slot_1", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot1)
	ON_FRESH_VI("btn_slot_2", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot2)
	ON_FRESH_VI("btn_slot_3", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot3)
	ON_FRESH_VI("btn_slot_4", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot4)
	ON_FRESH_VI("btn_slot_5", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot5)
	ON_FRESH_VI("btn_slot_6", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot6)
	ON_FRESH_VI("btn_slot_7", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot7)
	ON_FRESH_VI("btn_slot_8", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot8)
	ON_FRESH_VI("btn_slot_9", FRCMD_INIT, FrPasswordInputDlg::OnInitNumberSlot9)
	ON_FRESH_VI("btn_back", FRCMD_INIT,
		FrPasswordInputDlg::OnBackSpace_ButtonInit)
	ON_FRESH_VI("btn_clear", FRCMD_INIT, FrPasswordInputDlg::OnClear_ButtonInit)
	ON_FRESH_VI("btn_create", FRCMD_INIT,
		FrPasswordInputDlg::OnCreatePassword_ButtonInit)
	ON_FRESH_VI("btn_confirm", FRCMD_INIT,
		FrPasswordInputDlg::OnConfirm_ButtonInit)
	ON_FRESH_VI("btn_change_pwd", FRCMD_INIT,
		FrPasswordInputDlg::OnChangePassword_ButtonInit)
	ON_FRESH_VI("btn_report", FRCMD_INIT,
		FrPasswordInputDlg::OnReport_ButtonInit)
	ON_FRESH_VV("btn_slot_0", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot0)
	ON_FRESH_VV("btn_slot_1", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot1)
	ON_FRESH_VV("btn_slot_2", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot2)
	ON_FRESH_VV("btn_slot_3", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot3)
	ON_FRESH_VV("btn_slot_4", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot4)
	ON_FRESH_VV("btn_slot_5", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot5)
	ON_FRESH_VV("btn_slot_6", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot6)
	ON_FRESH_VV("btn_slot_7", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot7)
	ON_FRESH_VV("btn_slot_8", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot8)
	ON_FRESH_VV("btn_slot_9", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnLButtonUpNumberSlot9)
	ON_FRESH_VV("btn_back", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnBackSpace_LButtonUp)
	ON_FRESH_VV("btn_clear", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnClear_LButtonUp)
	ON_FRESH_VV("btn_create", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnCreatePassword_LButtonUp)
	ON_FRESH_VV("btn_confirm", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnConfirm_LButtonUp)
	ON_FRESH_VV("btn_change_pwd", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnChangePassword_LButtonUp)
	ON_FRESH_VV("btn_report", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnReport_LButtonUp)
	ON_FRESH_VI("confirm_first_pwd", FRCMD_INIT,
		FrPasswordInputDlg::OnConfirmFirstPassword_ButtonInit)
	ON_FRESH_VV("confirm_first_pwd", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnConfirmFirstPassword_LButtonUp)
	ON_FRESH_VI("caption", FRCMD_INIT, FrPasswordInputDlg::OnCaption_AreaInit)
	ON_FRESH_VI("explane_block1", FRCMD_INIT,
		FrPasswordInputDlg::OnExplaneBlock_AreaInit)
	ON_FRESH_VI("explane_block1", FRCMD_OWNERDRAW,
		FrPasswordInputDlg::OnExplaneBlock_AreaOwnerdraw)
	ON_FRESH_VI("password", FRCMD_INIT,
		FrPasswordInputDlg::OnPasswordInput_EditInit)
	ON_FRESH_VI("password_re", FRCMD_INIT,
		FrPasswordInputDlg::OnPasswordConfirm_EditInit)
	ON_FRESH_VI("btn_close", FRCMD_INIT, FrPasswordInputDlg::OnClose_ButtonInit)
	ON_FRESH_VV("btn_close", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnClose_LButtonUp)
	ON_FRESH_VI("input_again", FRCMD_INIT,
		FrPasswordInputDlg::OnInputAgain_ButtonInit)
	ON_FRESH_VV("input_again", FRCMD_LBUTTONUP,
		FrPasswordInputDlg::OnInputAgain_LButtonUp)
	END_FRESH_MSGMAP()

	FrPasswordInputDlg::FrPasswordInputDlg()
		: m_dlgType(SECONDPWD_INPUT_CREATE), m_inputSequence(INPUT_NONE)
	{
		InitVariable();
	}

	FrPasswordInputDlg::~FrPasswordInputDlg()
	{
	}

	void FrPasswordInputDlg::InitVariable()
	{
		memset(m_pButton, 0, sizeof(m_pButton));
		m_pExplaneBlock = NULL;
		memset(m_pPasswordEdit, 0, sizeof(m_pPasswordEdit));
		memset(m_pBitmap, 0, sizeof(m_pBitmap));
		m_pCurrentEdit = NULL;
	}

	void FrPasswordInputDlg::LoadResource()
	{
		const char* bitmap[] = { "2ndPW_text_title_createpw",
			"2ndPW_text_title_insertpw", "2ndPW_text_top_Insertnew",
			"2ndPW_text_top_Insert", "2ndPW_text_top_declare",
			"2ndPW_text_newpassword_I", "2ndPW_text_newpassword_C",
			"2ndPW_Insertbox", "2ndPW_text_warning_01", "2ndPW_text_warning_02",
			"2ndPW_Icon_notice_s", "2ndPW_Insertbox_mask",
			"2ndPW_Insertbox_mask02" };
		for (int i = 0; i < 13; ++i)
			m_pBitmap[i] = g_pFresh->RegisterBitmap(bitmap[i]);
	}

	bool FrPasswordInputDlg::OnInit()
	{
		LoadResource();
		switch (m_dlgType)
		{
		case SECONDPWD_INPUT_CREATE:
			ConvertCreateInput();
			ChangeInputSequence(INPUT_FIRST);
			break;
		case SECONDPWD_INPUT_LOGIN:
			ConvertConfirmInput();
			ChangeInputSequence(INPUT_CONFIRM);
			break;
		}
		for (int i = 0; i < 10; ++i)
			m_numberList.push_back(i);
		ArrangeButtonImage();
		return FrForm::OnInit();
	}

	bool FrPasswordInputDlg::ConvertCreateInput()
	{
		S5::DisableFreshWindow(m_pButton[13]);
		S5::DisableFreshWindow(m_pButton[14]);
		S5::DisableFreshWindow(m_pButton[15]);
		S5::DisableFreshWindow(m_pButton[17]);
		m_pButton[12]->Enable(false);
		m_pCaption->SetBgImg(m_pBitmap[0]);
		return true;
	}

	bool FrPasswordInputDlg::ConvertConfirmInput()
	{
		S5::DisableFreshWindow(m_pButton[12]);
		S5::DisableFreshWindow(m_pButton[16]);
		S5::DisableFreshWindow(m_pButton[17]);
		m_pCaption->SetBgImg(m_pBitmap[1]);
		S5::DisableFreshWindow(m_pPasswordEdit[1]);
		return true;
	}

	bool FrPasswordInputDlg::SetButtonProperty(int index, int param)
	{
		m_pButton[index] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
		return m_pButton[index] ? true : false;
	}

	void FrPasswordInputDlg::PushNumButton(int slot)
	{
		if (m_password.length() >= 8)
			return;
		InsertPasswordChar(m_numberList[slot]);
		CheckEnableCreateButton();
	}

	void FillAsterisk(char* text, int count);

	void FrPasswordInputDlg::InsertPasswordChar(int number)
	{
		char digit[4] = { 0 };
		char text[9] = { 0 };
		_itoa(number, digit, 10);
		m_password += digit;
		int length = m_password.length();
		if (length <= 8)
		{
			_tcsncpy(text, m_password.c_str(), length);
			FillAsterisk(text, length - 1);
		}
		if (m_pCurrentEdit)
		{
			m_pCurrentEdit->ClearLine();
			m_pCurrentEdit->SetLine(1, text, 0, false, 0);
		}
	}

	void FrPasswordInputDlg::ErasePasswordChar()
	{
		if (m_password.empty())
			return;
		char text[9] = { 0 };
		m_password.erase(m_password.end() - 1);
		int length = m_password.length();
		if (length <= 8)
		{
			_tcsncpy(text, m_password.c_str(), length);
			FillAsterisk(text, length);
		}
		if (m_pCurrentEdit)
		{
			m_pCurrentEdit->ClearLine();
			m_pCurrentEdit->SetLine(1, text, 0, false, 0);
		}
	}

	void FillAsterisk(char* text, int count)
	{
		for (int i = 0; i < count; ++i)
			text[i] = '*';
	}

	void FrPasswordInputDlg::ClearPassword()
	{
		if (m_password.empty() == false && m_pCurrentEdit)
		{
			m_pCurrentEdit->ClearLine();
			m_password.clear();
		}
	}

	bool FrPasswordInputDlg::ChangeInputSequence(eInputSequence sequence)
	{
		if (sequence == m_inputSequence)
			return false;
		if (sequence == INPUT_FIRST)
		{
			m_pCurrentEdit = m_pPasswordEdit[0];
			m_pPasswordEdit[0]->ClearLine();
			m_pPasswordEdit[1]->ClearLine();
			m_password.clear();
			m_firstPassword.clear();
			S5::EnableFreshWindow(m_pButton[16]);
		}
		else if (sequence == INPUT_CONFIRM)
		{
			if (m_inputSequence == INPUT_FIRST)
			{
				char text[9] = { 0 };
				_tcsncpy(text, m_password.c_str(), m_password.length());
				FillAsterisk(text, m_password.length());
				m_pPasswordEdit[0]->ClearLine();
				m_pPasswordEdit[0]->SetLine(1, text, 0, false, 0);
				m_pButton[16]->Enable(false);
				m_pCurrentEdit = m_pPasswordEdit[1];
				S5::EnableFreshWindow(m_pButton[17]);
			}
			else
				m_pCurrentEdit = m_pPasswordEdit[0];
			m_pPasswordEdit[1]->ClearLine();
			m_firstPassword = m_password;
			m_password.clear();
		}
		else
			return false;
		m_inputSequence = sequence;
		return true;
	}

	bool FrPasswordInputDlg::CheckInputPassword()
	{
		return m_password == m_firstPassword ? true : false;
	}

	void FrPasswordInputDlg::ArrangeButtonImage()
	{
		std::random_shuffle(m_numberList.begin(), m_numberList.end());
		for (int i = 0; i < 10; ++i)
		{
			m_pButton[i]->SetButtonImg(MakeStr("2ndPW_no%d_n", m_numberList[i]),
				FrButton::NORMAL);
			m_pButton[i]->SetButtonImg(MakeStr("2ndPW_no%d_o", m_numberList[i]),
				FrButton::OVER);
			m_pButton[i]->SetButtonImg(MakeStr("2ndPW_no%d_d", m_numberList[i]),
				FrButton::PRESSED);
		}
	}

	void FrPasswordInputDlg::CheckEnableCreateButton()
	{
		if (m_dlgType == SECONDPWD_INPUT_CREATE &&
			m_inputSequence == INPUT_CONFIRM)
		{
			if (m_password.length() > 4 && m_firstPassword.length() > 4)
				m_pButton[12]->Enable(true);
			else
				m_pButton[12]->Enable(false);
		}
	}

	void FrPasswordInputDlg::DrawCreateDlgExplane(FrGraphicInterface* gdi)
	{
		{
			const Bitmap* bitmap = m_pBitmap[2];
			if (bitmap)
			{
				gdi->DrawTexture(bitmap,
					WRect(m_rect.x + 135.0f, m_rect.y + 35.0f, bitmap->Width(),
						bitmap->Height()),
					0xffffffff, 0);
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[5];
			if (bitmap)
			{
				gdi->DrawTexture(bitmap,
					WRect(m_rect.x + 93.0f, m_rect.y + 72.0f, bitmap->Width(),
						bitmap->Height()),
					0xffffffff, 0);
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[6];
			if (bitmap)
			{
				gdi->DrawTexture(bitmap,
					WRect(m_rect.x + 93.0f, m_rect.y + 103.0f, bitmap->Width(),
						bitmap->Height()),
					0xffffffff, 0);
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[7];
			if (bitmap)
			{
				gdi->DrawTexture(bitmap,
					WRect(m_rect.x + 199.0f, m_rect.y + 69.0f, bitmap->Width(),
						bitmap->Height()),
					0xffffffff, 0);
				gdi->DrawTexture(bitmap,
					WRect(m_rect.x + 199.0f, m_rect.y + 100.0f, bitmap->Width(),
						bitmap->Height()),
					0xffffffff, 0);
			}
		}
		switch (m_inputSequence)
		{
		case INPUT_FIRST:
		{
			const Bitmap* bitmap = m_pBitmap[12];
			gdi->DrawTexture(bitmap,
				WRect(m_rect.x + 199.0f, m_rect.y + 100.0f, bitmap->Width(),
					bitmap->Height()),
				0xffffffff, 0);
		}
		break;
		case INPUT_CONFIRM:
		{
			const Bitmap* bitmap = m_pBitmap[11];
			gdi->DrawTexture(bitmap,
				WRect(m_rect.x + 199.0f, m_rect.y + 69.0f, bitmap->Width(),
					bitmap->Height()),
				0xffffffff, 0);
		}
		break;
		}
	}

	void FrPasswordInputDlg::DrawLoginDlgExplane(FrGraphicInterface* gdi)
	{
		{
			const Bitmap* bitmap = m_pBitmap[3];
			if (bitmap)
			{
				WRect rect(m_rect.x + 151.0f, m_rect.y + 39.0f, bitmap->Width(),
					bitmap->Height());
				gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[7];
			if (bitmap)
			{
				WRect rect(m_rect.x + 199.0f, m_rect.y + 69.0f, bitmap->Width(),
					bitmap->Height());
				gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[10];
			if (bitmap)
			{
				WRect rect(m_rect.x + 87.0f, m_rect.y + 94.0f, bitmap->Width(),
					bitmap->Height());
				gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[4];
			if (bitmap)
			{
				WRect rect(m_rect.x + 115.0f, m_rect.y + 102.0f,
					bitmap->Width(), bitmap->Height());
				gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
		}
	}

	void FrPasswordInputDlg::OnInitNumberSlot0(int param)
	{
		SetButtonProperty(0, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot0()
	{
		PushNumButton(0);
	}

	void FrPasswordInputDlg::OnInitNumberSlot1(int param)
	{
		SetButtonProperty(1, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot1()
	{
		PushNumButton(1);
	}

	void FrPasswordInputDlg::OnInitNumberSlot2(int param)
	{
		SetButtonProperty(2, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot2()
	{
		PushNumButton(2);
	}

	void FrPasswordInputDlg::OnInitNumberSlot3(int param)
	{
		SetButtonProperty(3, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot3()
	{
		PushNumButton(3);
	}

	void FrPasswordInputDlg::OnInitNumberSlot4(int param)
	{
		SetButtonProperty(4, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot4()
	{
		PushNumButton(4);
	}

	void FrPasswordInputDlg::OnInitNumberSlot5(int param)
	{
		SetButtonProperty(5, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot5()
	{
		PushNumButton(5);
	}

	void FrPasswordInputDlg::OnInitNumberSlot6(int param)
	{
		SetButtonProperty(6, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot6()
	{
		PushNumButton(6);
	}

	void FrPasswordInputDlg::OnInitNumberSlot7(int param)
	{
		SetButtonProperty(7, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot7()
	{
		PushNumButton(7);
	}

	void FrPasswordInputDlg::OnInitNumberSlot8(int param)
	{
		SetButtonProperty(8, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot8()
	{
		PushNumButton(8);
	}

	void FrPasswordInputDlg::OnInitNumberSlot9(int param)
	{
		SetButtonProperty(9, param);
	}

	void FrPasswordInputDlg::OnLButtonUpNumberSlot9()
	{
		PushNumButton(9);
	}

	void FrPasswordInputDlg::OnBackSpace_ButtonInit(int param)
	{
		SetButtonProperty(10, param);
	}

	void FrPasswordInputDlg::OnClear_ButtonInit(int param)
	{
		SetButtonProperty(11, param);
	}

	void FrPasswordInputDlg::OnCreatePassword_ButtonInit(int param)
	{
		SetButtonProperty(12, param);
	}

	void FrPasswordInputDlg::OnConfirm_ButtonInit(int param)
	{
		SetButtonProperty(13, param);
	}

	void FrPasswordInputDlg::OnChangePassword_ButtonInit(int param)
	{
		SetButtonProperty(14, param);
	}

	void FrPasswordInputDlg::OnReport_ButtonInit(int param)
	{
		SetButtonProperty(15, param);
	}

	void FrPasswordInputDlg::OnConfirmFirstPassword_ButtonInit(int param)
	{
		SetButtonProperty(16, param);
	}

	void FrPasswordInputDlg::OnCaption_AreaInit(int param)
	{
		m_pCaption = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	}

	void FrPasswordInputDlg::OnExplaneBlock_AreaInit(int param)
	{
		m_pExplaneBlock = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	}

	void FrPasswordInputDlg::OnExplaneBlock_AreaOwnerdraw(int param)
	{
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		switch (m_dlgType)
		{
		case SECONDPWD_INPUT_CREATE:
			DrawCreateDlgExplane(gdi);
			break;
		case SECONDPWD_INPUT_LOGIN:
			DrawLoginDlgExplane(gdi);
			break;
		}
		{
			const Bitmap* bitmap = m_pBitmap[8];
			if (bitmap)
			{
				WRect rect(m_rect.x + 20.0f, m_rect.y + 384.0f, bitmap->Width(),
					bitmap->Height());
				gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[9];
			if (bitmap)
			{
				WRect rect(m_rect.x + 276.0f, m_rect.y + 384.0f,
					bitmap->Width(), bitmap->Height());
				gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
		}
	}

	void FrPasswordInputDlg::OnPasswordInput_EditInit(int param)
	{
		m_pPasswordEdit[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	}

	void FrPasswordInputDlg::OnPasswordConfirm_EditInit(int param)
	{
		m_pPasswordEdit[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	}

	void FrPasswordInputDlg::OnBackSpace_LButtonUp()
	{
		ErasePasswordChar();
		CheckEnableCreateButton();
	}

	void FrPasswordInputDlg::OnClear_LButtonUp()
	{
		ClearPassword();
		CheckEnableCreateButton();
	}

	void FrPasswordInputDlg::OnCreatePassword_LButtonUp()
	{
		if (CheckInputPassword() == false)
		{
			AfxGetTask()->GetActor(s_gatewayActor[1])
				<< MsgObject(NULL, 621, 22, 100, 0, 0, 0);
			if (m_inputSequence == INPUT_FIRST)
				ClearPassword();
			else
			{
				OnInputAgain_LButtonUp();
				m_pButton[12]->Enable(false);
			}
			return;
		}
		bool valid = true;
		if (m_dlgType == SECONDPWD_INPUT_CREATE)
		{
			AfxGetTask()->GetActor(s_gatewayActor[1]) << MsgObject(NULL, 621,
				21, (int)&m_firstPassword, (int)&valid, 0, 0);
		}
		if (valid == false)
		{
			OnInputAgain_LButtonUp();
			m_pButton[12]->Enable(false);
			return;
		}
		AfxGetTask()->GetActor(s_gatewayActor[1])
			<< MsgObject(NULL, 621, 2, (int)m_firstPassword.c_str(), 0, 0, 0);
	}

	void FrPasswordInputDlg::OnConfirm_LButtonUp()
	{
		bool valid = true;
		if (m_dlgType == SECONDPWD_INPUT_LOGIN)
			AfxGetTask()->GetActor(s_gatewayActor[1]) << MsgObject(NULL, 621,
				21, (int)&m_password, (int)&valid, 0, 0);
		else if (m_dlgType == SECONDPWD_INPUT_CREATE)
			AfxGetTask()->GetActor(s_gatewayActor[1]) << MsgObject(NULL, 621,
				21, (int)&m_firstPassword, (int)&valid, 0, 0);
		if (valid == false)
		{
			ClearPassword();
			return;
		}
		AfxGetTask()->GetActor(s_gatewayActor[1])
			<< MsgObject(NULL, 621, 12, (int)m_password.c_str(), 0, 0, 0);
		ClearPassword();
	}

	void FrPasswordInputDlg::OnChangePassword_LButtonUp()
	{
		IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
		actor << MsgObject(NULL, 621, 13, 0, 0, 0, 0);
	}

	void FrPasswordInputDlg::OnReport_LButtonUp()
	{
		AfxGetTask()->GetActor(s_gatewayActor[1])
			<< MsgObject(NULL, 621, 22, 117, 0, 0, 0);
	}

	void FrPasswordInputDlg::OnConfirmFirstPassword_LButtonUp()
	{
		bool valid = true;
		if (m_dlgType == SECONDPWD_INPUT_CREATE &&
			m_inputSequence == INPUT_FIRST)
			AfxGetTask()->GetActor(s_gatewayActor[1]) << MsgObject(NULL, 621,
				21, (int)&m_password, (int)&valid, 0, 0);
		if (valid == false)
		{
			ClearPassword();
			return;
		}
		ChangeInputSequence(INPUT_CONFIRM);
		S5::DisableFreshWindow(m_pButton[16]);
	}

	void FrPasswordInputDlg::OnInputAgain_LButtonUp()
	{
		S5::DisableFreshWindow(m_pButton[17]);
		ChangeInputSequence(INPUT_FIRST);
		m_pButton[12]->Enable(false);
	}

	void FrPasswordInputDlg::OnClose_LButtonUp()
	{
		PostQuitMessage(0);
	}
	void FrPasswordInputDlg::OnInputAgain_ButtonInit(int param)
	{
		SetButtonProperty(17, param);
	}

	void FrPasswordInputDlg::OnClose_ButtonInit(int param)
	{
		SetButtonProperty(18, param);
	}

	IMPLEMENT_OBJECT(FrSelectCertification, FrForm)

	BEGIN_FRESH_MSGMAP(FrSelectCertification, FrForm)
	ON_FRESH_VI("btn_confirm", FRCMD_INIT,
		FrSelectCertification::OnRequestCertify_ButtonInit)
	ON_FRESH_VV("btn_confirm", FRCMD_LBUTTONUP,
		FrSelectCertification::OnRequestCertify_LButtonDown)
	ON_FRESH_VI("btn_cancel", FRCMD_INIT,
		FrSelectCertification::OnPassCertify_ButtonInit)
	ON_FRESH_VV("btn_cancel", FRCMD_LBUTTONUP,
		FrSelectCertification::OnPassCertify_LButtonDown)
	ON_FRESH_VI("edit_left", FRCMD_INIT, FrSelectCertification::OnLeft_EditInit)
	ON_FRESH_VI("edit_right", FRCMD_INIT,
		FrSelectCertification::OnRight_EditInit)
	ON_FRESH_VI("inframe_bg", FRCMD_INIT,
		FrSelectCertification::OnInFrame_AreaInit)
	ON_FRESH_VI("inframe_bg", FRCMD_OWNERDRAW,
		FrSelectCertification::OnInFrame_AreaOwnerDraw)
	END_FRESH_MSGMAP()

	FrSelectCertification::FrSelectCertification()
	{
		memset(m_pButton, 0, sizeof(m_pButton));
		memset(m_pEdit, 0, sizeof(m_pEdit));
		m_pInFrame = NULL;
		memset(m_pBitmap, 0, sizeof(m_pBitmap));
	}

	FrSelectCertification::~FrSelectCertification()
	{
	}

	bool FrSelectCertification::OnInit()
	{
		const char* bitmap[] = { "2ndPW_inframe_01", "2ndPW_inframe_02",
			"2ndPW_Icon_certification", "2ndPW_Icon_notice_b" };
		for (int i = 0; i < 4; ++i)
			m_pBitmap[i] = g_pFresh->RegisterBitmap(bitmap[i]);
		return FrForm::OnInit();
	}

	void FrSelectCertification::OnRequestCertify_ButtonInit(int param)
	{
		m_pButton[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrSelectCertification::OnPassCertify_ButtonInit(int param)
	{
		m_pButton[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrSelectCertification::OnRequestCertify_LButtonDown()
	{
		Close(FrOK, true);
		IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
		actor << MsgObject(NULL, 621, 7, 0, 0, 0, 0);
		actor << MsgObject(NULL, 621, 13, 0, 0, 0, 0);
	}

	void FrSelectCertification::OnPassCertify_LButtonDown()
	{
		Close(FrOK, true);
		AfxGetTask()->GetActor(s_gatewayActor[1])
			<< MsgObject(NULL, 621, 7, 0, 0, 0, 0);
	}

	void FrSelectCertification::OnLeft_EditInit(int param)
	{
		m_pEdit[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
		m_pEdit[0]->AddText(_systemmsg::CMessageManager::Instance()->GetMsg(
								(_systemmsg::eMsgIdentifier)115),
			0, true);
	}

	void FrSelectCertification::OnRight_EditInit(int param)
	{
		m_pEdit[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
		m_pEdit[1]->AddText(_systemmsg::CMessageManager::Instance()->GetMsg(
								(_systemmsg::eMsgIdentifier)116),
			0, true);
	}

	void FrSelectCertification::OnInFrame_AreaInit(int param)
	{
		m_pInFrame = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	}

	void FrSelectCertification::OnInFrame_AreaOwnerDraw(int param)
	{
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		{
			const Bitmap* bitmap = m_pBitmap[0];
			if (bitmap)
			{
				{
					WRect rect(m_rect.x + 53.0f, m_rect.y + 95.0f,
						bitmap->Width(), bitmap->Height());
					gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
				}
				{
					WRect rect(m_rect.x + 260.0f, m_rect.y + 95.0f,
						bitmap->Width(), bitmap->Height());
					gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
				}
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[1];
			if (bitmap)
			{
				{
					WRect rect(m_rect.x + 53.0f, m_rect.y + 351.0f,
						bitmap->Width(), bitmap->Height());
					gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
				}
				{
					WRect rect(m_rect.x + 260.0f, m_rect.y + 351.0f,
						bitmap->Width(), bitmap->Height());
					gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
				}
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[2];
			if (bitmap)
			{
				{
					WRect rect(m_rect.x + 126.0f, m_rect.y + 185.0f,
						bitmap->Width(), bitmap->Height());
					gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
				}
			}
		}
		{
			const Bitmap* bitmap = m_pBitmap[3];
			if (bitmap)
			{
				{
					WRect rect(m_rect.x + 330.0f, m_rect.y + 185.0f,
						bitmap->Width(), bitmap->Height());
					gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
				}
			}
		}
	}

	IMPLEMENT_OBJECT(FrExplanePassword, FrForm)

	BEGIN_FRESH_MSGMAP(FrExplanePassword, FrForm)
	ON_FRESH_VI("btn_opencreate", FRCMD_INIT,
		FrExplanePassword::OnOpenPasswordCreate_ButtonInit)
	ON_FRESH_VV("btn_opencreate", FRCMD_LBUTTONUP,
		FrExplanePassword::OnOpenPasswordCreate_LButtonUp)
	ON_FRESH_VI("edit_explane", FRCMD_INIT,
		FrExplanePassword::OnExplane_EditInit)
	END_FRESH_MSGMAP()

	FrExplanePassword::FrExplanePassword()
		: m_pOpenCreateBtn(NULL), m_pExplaneEdit(NULL)
	{
	}

	FrExplanePassword::~FrExplanePassword()
	{
	}

	void FrExplanePassword::OnOpenPasswordCreate_ButtonInit(int param)
	{
		m_pOpenCreateBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrExplanePassword::OnOpenPasswordCreate_LButtonUp()
	{
		Close(FrOK, true);
		AfxGetTask()->GetActor(s_gatewayActor[1])
			<< MsgObject(NULL, 621, 6, 0, 0, 0, 0);
	}

	void FrExplanePassword::OnExplane_EditInit(int param)
	{
		m_pExplaneEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
		m_pExplaneEdit->AddText(_systemmsg::CMessageManager::Instance()->GetMsg(
									(_systemmsg::eMsgIdentifier)114),
			0, true);
	}
	IMPLEMENT_OBJECT(FrPasswordNotifyDlg, FrForm)

	BEGIN_FRESH_MSGMAP(FrPasswordNotifyDlg, FrForm)
	ON_FRESH_VI("notify_area", FRCMD_INIT, FrPasswordNotifyDlg::OnIcon_AreaInit)
	ON_FRESH_VI("notify_area", FRCMD_OWNERDRAW,
		FrPasswordNotifyDlg::OnIcon_AreaOwnerdraw)
	ON_FRESH_VI("edit_msg", FRCMD_INIT,
		FrPasswordNotifyDlg::OnSystemMsg_EditInit)
	ON_FRESH_VI("btn_close", FRCMD_INIT,
		FrPasswordNotifyDlg::OnClose_ButtonInit)
	ON_FRESH_VV("btn_close", FRCMD_LBUTTONUP,
		FrPasswordNotifyDlg::OnClose_LButtonUp)
	END_FRESH_MSGMAP()

	FrPasswordNotifyDlg::FrPasswordNotifyDlg()
	{
		m_notifyType = 0;
		m_pIcon = NULL;
		m_pSystemMsg = NULL;
		m_pCloseBtn = NULL;
		memset(m_pBitmap, 0, sizeof(m_pBitmap));
		m_bCustomMessage = false;
	}

	FrPasswordNotifyDlg::~FrPasswordNotifyDlg()
	{
	}

	bool FrPasswordNotifyDlg::OnInit()
	{
		const char* bitmap[] = { "2ndPW_Icon_Policy", "2ndPW_Icon_notice_b",
			"2ndPW_Icon_confirm" };
		for (int i = 0; i < 3; ++i)
			m_pBitmap[i] = g_pFresh->RegisterBitmap(bitmap[i]);
		switch (m_notifyType)
		{
		case 117:
			m_iconType = 0;
			break;
		case 100:
		case 101:
		case 102:
		case 105:
		case 106:
		case 108:
		case 110:
		case 111:
		case 112:
		case 113:
			m_iconType = 1;
			break;
		case 104:
			m_iconType = 2;
			break;
		}
		if (m_bCustomMessage == false)
		{
			const char* message =
				_systemmsg::CMessageManager::Instance()->GetMsg(
					(_systemmsg::eMsgIdentifier)m_notifyType);
			if (m_pSystemMsg && message)
				m_pSystemMsg->AddText(message, 0, true);
		}
		else
			m_pSystemMsg->AddText(m_message.c_str(), 0, true);
		return FrForm::OnInit();
	}

	void FrPasswordNotifyDlg::OnIcon_AreaInit(int param)
	{
		m_pIcon = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	}

	void FrPasswordNotifyDlg::OnIcon_AreaOwnerdraw(int param)
	{
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		const Bitmap* bitmap;
		switch (m_iconType)
		{
		case 0:
			bitmap = m_pBitmap[0];
			break;
		case 1:
			bitmap = m_pBitmap[1];
			break;
		case 2:
			bitmap = m_pBitmap[2];
			break;
		default:
			return;
		}
		if (bitmap)
		{
			WRect rect(m_rect.x + 150.0f, m_rect.y, bitmap->Width(),
				bitmap->Height());
			gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
		}
	}

	void FrPasswordNotifyDlg::OnSystemMsg_EditInit(int param)
	{
		m_pSystemMsg = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	}

	void FrPasswordNotifyDlg::OnClose_ButtonInit(int param)
	{
		m_pCloseBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	void FrPasswordNotifyDlg::SetMessage(const char* message)
	{
		m_bCustomMessage = true;
		m_message = message;
	}

	void FrPasswordNotifyDlg::OnClose_LButtonUp()
	{
		Close(FrOK, true);
	}
}
