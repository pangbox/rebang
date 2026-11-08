#pragma once

// HACK! This file contains filler to balance IL offsets between the PCH and TU.
// Ideally this file will disappear as we zero in on the actual contents of the
// PCH and TUs, but it will take a while.

// clang-format off
#define ILFILL4 v += 1; v += 1; v += 1; v += 1;
#define ILFILL16 ILFILL4 ILFILL4 ILFILL4 ILFILL4
#define ILFILL64 ILFILL16 ILFILL16 ILFILL16 ILFILL16
#define ILFILL256 ILFILL64 ILFILL64 ILFILL64 ILFILL64

#define ILFILLFN(n) void f##n() { ILFILL256 }

#define ILFILL1 \
	namespace ILFILLP1 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL256 ILFILL256 ILFILL16 v += 1; } \
			void f1(); \
		}; \
	}

#define ILFILLF1 \
	inline void ILFILLP1::Pad::f1() { ILFILL16 }

#define ILFILLX2 \
	namespace ILFILLX2 \
	{ \
		struct Pad \
		{ \
			int v; \
			\
			void g() { ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL16 ILFILL4 } \
		}; \
	}

#define ILFILLX1 \
	namespace ILFILLX1 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL16 ILFILL4 ILFILL4 ILFILL64 ILFILL64 v += 1; v += 1; v += 1; } \
		}; \
	}

#define ILFILLX1B \
	namespace ILFILLX1B \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL4 v += 1; v += 1; v += 1; } \
		}; \
	}

#define ILFILLTU_SHAREDDOC0 \
	namespace ILFILLTU_SHAREDDOC0 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL16 ILFILL4 ILFILL4 v += 1; v += 1; } \
		}; \
	}

#define ILFILLTU_GOLFRULE \
	namespace ILFILLTU_GOLFRULE \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL256 ILFILL256 ILFILL256 ILFILL64 ILFILL16 } \
		}; \
	}

#define ILFILLTU_GOLFRULE2 \
	namespace ILFILLTU_GOLFRULE2 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL64 ILFILL4 ILFILL4 ILFILL4 v += 1; } \
		}; \
	}

#define ILFILLTU_GOLFRULE3 \
	namespace ILFILLTU_GOLFRULE3 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL16 ILFILL16 ILFILL4 ILFILL4 v += 1; ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL16 v += 1; v += 1; v += 1; } \
		}; \
	}

#define ILFILLTB \
	namespace ILFILLTB \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL16 ILFILL16 ILFILL4 ILFILL4 v += 1; } \
		}; \
	}

#define ILFILL0 \
	namespace ILFILLP0 \
	{ \
		struct Pad \
		{ \
			int v; \
			void h(); \
			void g() { ILFILL256 ILFILL256 ILFILL64 ILFILL4 ILFILL4 ILFILL4 } \
		}; \
	}

#define ILFILLEXC \
	inline void ILFILLP0::Pad::h() { ILFILL64 ILFILL4 ILFILL4 ILFILL4 }

#define ILFILLC \
	namespace ILFILLC \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL16 ILFILL16 ILFILL16 ILFILL4 ILFILL4 ILFILL4 ILFILL4 ILFILL4 v += 1; v += 1; v += 1; { v += 1; } { v += 1; } { v += 1; } { v += 1; } v += 1; v += 1; } \
		}; \
	}

#define ILFILL2 \
	namespace ILFILL2 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL16 ILFILL256 ILFILL256 ILFILL64 ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL16 v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; } \
		}; \
	}

#define ILFILL_SHARED_ITEMMANAGER \
	namespace ILFILL_ITEMMANAGER \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL16 ILFILL16 ILFILL16 ILFILL4 ILFILL4 v += 1; v += 1; v += 1; } \
		}; \
	}

#define ILFILL3 \
	namespace ILFILL3 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL4 ILFILL4 v += 1; } \
		}; \
	}

#define ILFILLB1 \
	template <int N> struct TuFill { int v; void g() { ILFILL256 ILFILL16 ILFILL4 } }; \
	template <int N> struct TuMid { void g() { TuFill<N>().g(); } }; \
	template <int N> struct TuPad \
	{ \
		int v; \
		void g() { ILFILL256 ILFILL4 ILFILL4 ILFILL4 TuMid<N>().g(); } \
	}; \
	namespace TUPAD1 \
	{ \
		struct Pad \
		{ \
			int v; \
			ILFILLFN(0) ILFILLFN(1) ILFILLFN(2) ILFILLFN(3) \
			void g() { ILFILL256 ILFILL64 ILFILL4 v += 1; } \
		}; \
	}

#define ILFILLB2 \
	namespace TUPAD2 \
	{ \
		struct Pad \
		{ \
			void g() { TuPad<2>().g(); } \
		}; \
	}
#define ILFILLW1 \
	namespace TUPADW1 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL256 ILFILL64 ILFILL64 ILFILL64 ILFILL16 ILFILL4 ILFILL4 v += 1; } \
		}; \
	}


#define ILFILLTU_REALMYROOMMAINUI \
	namespace ILFILLTU_REALMYROOMMAINUI \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL16 ILFILL16 ILFILL16 ILFILL4 ILFILL4 ILFILL4 v += 1; } \
		}; \
	}

#define ILFILLTU_LOBBYMAIN \
	namespace ILFILLTU_LOBBYMAIN \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL64 ILFILL64 ILFILL64 ILFILL4 ILFILL4 ILFILL4 v += 1; } \
		}; \
	}

// clang-format on
