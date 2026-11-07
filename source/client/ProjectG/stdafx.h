#pragma once

#define WINVER 0x0400
#define _WIN32_WINNT 0x0400
#define _ATL_STATIC_REGISTRY
#define WLIST_ALLOC(size) malloc(size)
#define WLIST_FREE(ptr) free(ptr)
#include <winsock2.h>
#include <windows.h>
#include "../Wangreal/include/wtypes.h"
namespace ILNUM_WT
{
	typedef int t0;
	typedef int t1;
	typedef int t2;
	typedef int t3;
	typedef int t4;
	typedef int t5;
	typedef int t6;
	typedef int t7;
	typedef int t8;
	typedef int t9;
	typedef int t10;
	typedef int t11;
	typedef int t12;
	typedef int t13;
	typedef int t14;
	typedef int t15;
	typedef int t16;
	typedef int t17;
	typedef int t18;
	typedef int t19;
}
#include <atlbase.h>
#include <atlapp.h>
extern WTL::CAppModule _Module;
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include "z_ilfill.h"
ILFILLX1
#include <atlmisc.h>
#include <atldlgs.h>
ILFILLX1B
#include <math.h>
#include <stdexcept>
ILFILLX2
#include "wmath.h"
#include "gamath.h"
#include "wutil.h"
#include "wminmax.h"
#include "wlist.h"
#include "wmemblock.h"
#include "wlock.h"
#include "wmempak.h"
#include "rectmng.h"
#include "wscene.h"
#include "wvideo.h"
#include "wmesh.h"
#include "wresrcmng.h"
#include "wdevmng.h"
#include "wxtnlbuffer.h"
#include "xzip.h"
#include "cfile.h"
#include "wreg.h"
#include "bitmap.h"
#include "wavi.h"
#include "wpuppet.h"
#include "wview.h"
#include "wfont.h"
#include "wflag.h"
#include "pool.h"
#include "wangrealapp.h"
#include "westpak.h"
#include "wblockmodel.h"
#include "wboneset.h"
#include "wmathutil.inl"
#include "wpetfile.h"
#include "wlocalize.h"
#include "commonutil.h"
ILNUM_CU1
ILFILL0
#include "exception.h"
#include "objectfactory.h"
#include "background.h"
#include "w3danispr.h"
ILNUM_WOV
#include "woverlay.h"
#include "singleton.h"
#include <string>
#include <list>
#include <map>
#include <vector>
#include <algorithm>

#include "basicdebug.h"
ILNUM_CU

#include "tinyxml.h"

#include "../../shared/exceptionreport.h"
#include "lock.hpp"
ILNUM_PK
#include "titles_client.h"
ILFILLC
#include "../../shared/classdefine.h"
#include "chatmsg.h"
#include "../../shared/errorcode.h"
ILFILLEXC
ILNUM_EXC2
#include "tweaker.h"
ILNUM_TWK
#include "frelement.h"
ILNUM_FGI
#include "ucclibrary.h"
#include "hackingmanager.h"
#include "z_ilfill.h"
ILFILL1
#include "frwndinl.h"
ILNUM_FW
ILFILLF1
#include "frframe.h"
#include "frbutton.h"
#include "frstatic.h"
#include "frscrollbar.h"
#include "fredit.h"
#include "frform.h"
#include "frdesktop.h"
ILFILLF2
#include "frlistbox.h"
#include "frcombobox.h"
#include "frarea.h"
ILFILLTB
#include "frtabbutton.h"
#include "frgaugebar.h"
ILNUM_GB
#include "frwndmanager.h"
ILNUM_G0
#include "frgraphicinterface.h"
#include "fremoticon.h"
#include "fresh.h"
#include "packet.h"
#include "clientsocket.h"
#include "soundmanager.h"
#include "inputmanager.h"
ILNUM_G1
#include "networksystem.h"
ILNUM_NET
#include "networkmonitor.h"
#include "ccrc32.h"
// HACK
template unsigned int std::vector<CRC_MT::sCRC_CHKTASK*>::size() const;
#include "mainframe.h"
#include "projectg.h"
ILNUM_TB2
ILNUM_GB2
#include "frviewer.h"
#include "../../shared/globalgamedefine.h"
#include "../../shared/globalnetworkdefine.h"
#include "gdtextbox.h"
#include "gdbitmapbox.h"
#include "nodecontainer.h"
#include "taskmanager.h"
#include "actor.h"
#include "taskmain.h"
ILNUM_SCENE

// HACK: workaround for unsolved symbol ordering problems
template void std::vector<WVector>::reserve(unsigned int);
template float Min<float>(float, float);
#include "shareddoc.h"
ILNUM_SD
#include "uccmanager.h"
#include "frtext.h"
#include "wtemplate.h"

// HACK: the PCH seems to instantiate these somewhere - it hasn't been recovered yet
template const char& std::string::const_iterator::operator*() const;
template char& std::string::iterator::operator*() const;
template std::string::iterator std::string::iterator::operator+(int) const;
