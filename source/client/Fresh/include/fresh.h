#pragma once
#include "frelement.h"
#include "frwndmanager.h"

class Bitmap;
class FrElement;
class FrCmdTarget;
class FrWndManager;
class FrDesktop;

class Fresh
{
public:
	Fresh();
	virtual ~Fresh();
	bool Init(const char*, bool, const char*);
	void ShutDown();
	void OnProcess(float);
	void OnDisplay(bool);
	void CloseLayout();
	const FreshBitmapMap& ListBres(std::string&);
	void HidePrivacy(bool);
	bool IsCurrentLayout(const char*) const;
	const Bitmap* RegisterBitmap(const char*);
	const Bitmap* GetBitmap(const char*);
	bool OpenLayout(const char*, FrCmdTarget*, bool);
	bool IsDesktopFocused() const;
	FrDesktop* GetDesktop() const;
	FrWndManager* GetManager() const { return manager; }

	void Toggle(bool b) { show = b; }
	bool IsVisible() const { return show; }

private:
	FrElementDoc document;
	FrWndManager* manager;
	bool show;
	std::string filename;
};

inline FrDesktop* Fresh::GetDesktop() const
{
	return manager->GetDesktop();
}
