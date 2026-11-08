#include "ggc_dlg.h"
#include "ggc_helper.h"
#include "fredit.h"

IMPLEMENT_OBJECT(GGC_Dlg, FrForm)

BEGIN_FRESH_MSGMAP(GGC_Dlg, FrForm)

ON_FRESH_VI("message", FRCMD_INIT, GGC_Dlg::OnMessage_Init)

END_FRESH_MSGMAP()

void GGC_Dlg::OnMessage_Init(int param)
{
	m_pEdit = DYNAMIC_CAST(FrEdit, param);
	if (m_pEdit == NULL)
		return;

	unsigned long error;
	std::string message;
	GGC_GetHelper()->GetFirstCriticalErrorMessage(error, message);

	m_pEdit->AddLine(message.c_str(), 0, false);
	m_pEdit->AddLine("", 0, false);

	m_pEdit->AddLine("", 0, false);
	m_pEdit->AddLine(
		"* [\xb0\xd4\xc0\xd3\xb0\xa1\xb5\xe5]\xb4\xc2 [INCA]\xc0\xc7 \xc7\xd8\xc5\xb7 \xc2\xf7\xb4\xdc \xbc\xd6\xb7\xe7\xbc\xc7\xc0\xd4\xb4\xcf\xb4\xd9.",
		0, false);

	m_pEdit->AddLine("", 0, false);
	m_pEdit->AddLine(
		"\xba\xce\xb5\xe6\xc0\xcc \xc6\xce\xbe\xdf\xb8\xa6 \xc1\xbe\xb7\xe1\xc7\xd5\xb4\xcf\xb4\xd9!",
		0, false);
	m_pEdit->AddLine("", 0, false);
	m_pEdit->AddLine("\xb9\xae\xc0\xc7 : http://pangya.ntreev.com", 0, false);
}
