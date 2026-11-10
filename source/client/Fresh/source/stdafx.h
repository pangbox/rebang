#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>

#include <algorithm>
#include <list>
#include <stack>
#include <map>
#include <vector>
#include <math.h>
#include "../../Wangreal/include/wlocalize.h"
// HACK!
// clang-format off
#define FRESH_ILFILL10 v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1;
inline void FreshPchPad(int v) { FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 }
// clang-format on
#include "../../Wangreal/include/wtypes.h"
#include <string>
#include "../../Wangreal/include/bitmap.h"
#include "../../Wangreal/include/wflag.h"
#include "../../Wangreal/include/woverlay.h"
#include "../../Wangreal/include/wfont.h"
#include "inputmanager.h"

// HACK! TU IL filler for frform.cpp and frwnd.cpp
// clang-format off
#define FRESH_TU_ILFILL \
	inline void FreshFormPad(int v) { v += 1; FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; }
#define FRESH_TU_ILFILL_FRWND \
	inline void FreshWndPad(int v) { v += 1; FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; }
// clang-format on
