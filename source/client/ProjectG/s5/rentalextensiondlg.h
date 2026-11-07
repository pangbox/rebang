#pragma once

#include "frform.h"

#include "tweaker.h"

struct sItemInfo;

namespace S5
{
	class FrRentalExtensionDlg : public FrForm, public tweaker_cmd_target
	{
		DECLARE_OBJECT(FrRentalExtensionDlg)
		FrRentalExtensionDlg();
		virtual ~FrRentalExtensionDlg();

		void Initialize(const sItemInfo* pItemInfo);

		const sItemInfo* GetItemInfo() { return m_pItemInfo; }

	protected:
		void OnExtensionBtnUp();
		void OnDestroyBtnUp();
		void OnCancelBtnUp();
		void OnDialogAreaDraw();

		std::string SetChildWindowPos(const tweaker_param& param);
		std::string GetChildWindowPos(const tweaker_param& param);

		sItemInfo* m_pItemInfo;

		DECLARE_FRESH_MSGMAP()
	};
}
