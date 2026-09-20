#pragma once
// Header for Rebang-specific adaptations.
//
// Right now, this is just going to be used to try to help Clangd handle this
// crusty old C++03 era C++ code. It may expand to broader compatibility uses
// in the future.

#if defined(_MSC_VER) && (_MSC_VER < 1900) && !defined(_CLANGD)
#define REBANG_LEGACY_CPP
#endif

#ifdef REBANG_LEGACY_CPP
#define DEFAULT_IMPL \
	{ \
	}
#else
#define DEFAULT_IMPL = default
#endif
