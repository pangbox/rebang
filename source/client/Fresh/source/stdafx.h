#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>

#include <list>
#include <math.h>
// HACK!
namespace
{
	struct Pad
	{
		int v;
		void g()
		{
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
			v += 1;
		}
	};
}
#include "../../Wangreal/include/wtypes.h"
#include <string>
#include "../../Wangreal/include/bitmap.h"
#include "../../Wangreal/include/wflag.h"
#include "../../Wangreal/include/woverlay.h"
#include "../../Wangreal/include/wfont.h"
#include "inputmanager.h"
// HACK: This just exists to push IL offsets. It is unused.
#include <algorithm>

// HACK! TU IL filler for frform.cpp and frwnd.cpp
// clang-format off
#define FRESH_ILFILL10 v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1;
#define FRESH_TU_ILFILL \
	namespace \
	{ \
		struct TuPad \
		{ \
			int v; \
			void g() { FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 } \
		}; \
	}
#define FRESH_TU_ILFILL_FRWND \
	namespace \
	{ \
		struct TuPad \
		{ \
			int v; \
			void g() { FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 FRESH_ILFILL10 v += 1; v += 1; v += 1; v += 200; { v += 1; } { v += 1; } } \
		}; \
	}
// clang-format on
