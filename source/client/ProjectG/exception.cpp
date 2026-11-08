#include "exception.h"

void WException::Handle()
{
	FatalAppExitA(0, m_msg.c_str());
}
