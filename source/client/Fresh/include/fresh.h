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
	void OnProcess(const float);
	void OnDisplay(bool);
	bool OpenLayout(const char*, FrCmdTarget*, bool);
	void CloseLayout();
	bool IsCurrentLayout(const char*) const;
	const Bitmap* RegisterBitmap(const char*);
	const Bitmap* GetBitmap(const char*);
	FrDesktop* GetDesktop() const;
	FrWndManager* GetManager() const { return m_pWndManager; }
	bool IsDesktopFocused() const;
	void Toggle(bool b) { m_bShow = b; }
	bool IsVisible() const { return m_bShow; }
	const FreshBitmapMap& ListBres(std::string&);
	void HidePrivacy(bool);

protected:
	FrElementDoc m_doc;
	FrWndManager* m_pWndManager;
	bool m_bShow;
	std::string m_xmlFileName;
};

inline FrDesktop* Fresh::GetDesktop() const
{
	return m_pWndManager->GetDesktop();
}
