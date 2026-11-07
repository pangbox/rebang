#include "fresh.h"
#include "frwndmanager.h"
#include "frdesktop.h"

Fresh::Fresh()
	: m_pWndManager(0), m_bShow(true)
{
}

Fresh::~Fresh()
{
	ShutDown();
}

void Fresh::ShutDown()
{
	if (m_pWndManager)
	{
		delete m_pWndManager;
		m_pWndManager = 0;
	}
}

bool Fresh::Init(const char* path, bool option, const char* name)
{
	if (!m_doc.Load(path))
		return false;
	m_xmlFileName = path;
	m_pWndManager = new FrWndManager(&m_doc);
	m_pWndManager->Init(name, option);
	return true;
}

void Fresh::OnProcess(const float delta)
{
	if (m_pWndManager && m_bShow)
		m_pWndManager->Process(delta);
}

void Fresh::OnDisplay(bool option)
{
	if (m_pWndManager && m_bShow)
		m_pWndManager->Display(option);
}

bool Fresh::OpenLayout(const char* layout, FrCmdTarget* owner, bool first)
{
	if (!m_pWndManager)
		return false;
	CloseLayout();
	return m_pWndManager->CreateLayout(layout, owner, first);
}

void Fresh::CloseLayout()
{
	if (m_pWndManager)
		m_pWndManager->CloseLayout();
}

bool Fresh::IsDesktopFocused() const
{
	if (!m_pWndManager)
		return false;
	FrDesktop* desktop = m_pWndManager->GetDesktop();
	if (!desktop)
		return false;
	return desktop->m_nFlags.GetFlag(FWF_VIEWFOCUS);
}

const FreshBitmapMap& Fresh::ListBres(std::string& output)
{
	output = m_xmlFileName;
	return m_doc.GetBitmapList();
}

void Fresh::HidePrivacy(bool hidden)
{
	m_pWndManager->HidePrivacy(hidden);
}

bool Fresh::IsCurrentLayout(const char* name) const
{
	const char* current = m_pWndManager->GetLayoutID();
	bool matches = stricmp(current, name) == 0;
	return matches;
}

const Bitmap* Fresh::RegisterBitmap(const char* name)
{
	return m_pWndManager->GetBitmap("", name);
}

const Bitmap* Fresh::GetBitmap(const char* name)
{
	return m_pWndManager->GetBitmap("", name);
}
