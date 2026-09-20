#include "fresh.h"
#include "frwndmanager.h"
#include "frdesktop.h"

static __declspec(thread) int __rtti_obj;

Fresh::Fresh()
	: manager(0), show(true)
{
}

Fresh::~Fresh()
{
	ShutDown();
}

void Fresh::ShutDown()
{
	if (manager)
	{
		delete manager;
		manager = 0;
	}
}

bool Fresh::Init(const char* path, bool option, const char* name)
{
	if (!document.Load(path))
		return false;
	filename = path;
	manager = new FrWndManager(&document);
	manager->Init(name, option);
	return true;
}

void Fresh::OnProcess(float delta)
{
	if (manager && show)
		manager->Process(delta);
}

void Fresh::OnDisplay(bool option)
{
	if (manager && show)
		manager->Display(option);
}

void Fresh::CloseLayout()
{
	if (manager)
		manager->CloseLayout();
}

const FreshBitmapMap& Fresh::ListBres(std::string& output)
{
	output = filename;
	return document.GetBitmapList();
}

void Fresh::HidePrivacy(bool hidden)
{
	manager->HidePrivacy(hidden);
}

bool Fresh::IsCurrentLayout(const char* name) const
{
	const char* current = manager->GetLayoutID();
	bool matches = stricmp(current, name) == 0;
	return matches;
}

const Bitmap* Fresh::RegisterBitmap(const char* name)
{
	return manager->GetBitmap("", name);
}

const Bitmap* Fresh::GetBitmap(const char* name)
{
	return manager->GetBitmap("", name);
}

bool Fresh::OpenLayout(const char* name, FrCmdTarget* owner, bool option)
{
	// HACK: the exact reason why this is needed is unknown.
	std::allocator<FrElement*> allocator;
	if (!manager)
		return false;
	manager->CloseLayout();
	return manager->CreateLayout(name, owner, option);
}

bool Fresh::IsDesktopFocused() const
{
	// HACK: the exact reason why this is needed is unknown.
	std::allocator<FrElement*> allocator;
	if (!manager)
		return false;
	FrDesktop* desktop = manager->GetDesktop();
	if (!desktop)
		return false;
	return desktop->m_nFlags.GetFlag(8);
}
