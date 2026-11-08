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
#define ILFILLFN10(n) \
	ILFILLFN(n##0) ILFILLFN(n##1) ILFILLFN(n##2) ILFILLFN(n##3) \
	ILFILLFN(n##4) ILFILLFN(n##5) ILFILLFN(n##6) ILFILLFN(n##7) \
	ILFILLFN(n##8) ILFILLFN(n##9)

#define ILFILL1 \
	namespace ILFILLP1 \
	{ \
		struct Pad \
		{ \
			int v; \
			void f0() { ILFILL256 ILFILL256 v += 1; v += 1; v += 1; v += 1; ILFILL4 ILFILL4 v += 1; v += 1; v += 1; } \
			void g() { v += 1; v += 1; } \
			void f1(); \
			void f2(); \
		}; \
	}

#define ILFILLF1 \
	inline void ILFILLP1::Pad::f1() { ILFILL16 }

#define ILFILLF2 \
	inline void ILFILLP1::Pad::f2() {}

#define ILFILLX2 \
	namespace ILFILLX2 \
	{ \
		struct Pad \
		{ \
			int v; \
			ILFILLFN(0) \
			\
			void g() { ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL4 ILFILL4 v += 1; v += 1; { v += 1; } { v += 1; } { v += 1; } v += 200; v += 200; v += 200; v += 200; v += 200; v += 200; v += 200; } \
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
			void g() { ILFILL64 ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL16 ILFILL4 ILFILL4 ILFILL4 v += 1; v += 1; v += 1; } \
		}; \
	}

#define ILFILLTU_GOLFRULE \
	namespace ILFILLTU_GOLFRULE \
	{ \
		struct Pad \
		{ \
			int v; \
			ILFILLFN(0) ILFILLFN(1) ILFILLFN(2) \
			void g() { ILFILL64 ILFILL16 ILFILL4 ILFILL4 v += 1; v += 1; { v += 1; } v += 1; v += 1; v += 1; v += 200; v += 200; } \
		}; \
	}

#define ILFILLTU_GOLFRULE2 \
	namespace ILFILLTU_GOLFRULE2 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL64 ILFILL16 v += 1; } \
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
			ILFILLFN(0) ILFILLFN(1) \
			void h(); \
			void g() { ILFILL4 ILFILL4 ILFILL16 ILFILL16 v += 1; v += 1; v += 1; ILFILL64 ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL4 v += 1; ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL16 ILFILL16 ILFILL16 ILFILL64 ILFILL64 v += 1; { v += 1; } v += 1; v += 1; } \
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
			void g() { ILFILL64 ILFILL16 ILFILL16 ILFILL16 ILFILL4 ILFILL4 ILFILL4 ILFILL4 ILFILL4 v += 1; v += 1; v += 1; { v += 1; } { v += 1; } { v += 1; } { v += 1; } v += 200; v += 200; } \
		}; \
	}

#define ILFILL2 \
	namespace ILFILL2 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL256 ILFILL256 ILFILL64 ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL16 v += 1; v += 1; v += 1; { v += 1; } { v += 1; } v += 200; v += 200; v += 200; v += 200; v += 200; v += 200; } \
		}; \
	}

#define ILFILL_SHARED_ITEMMANAGER \
	namespace ILFILL_ITEMMANAGER \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { v += 1; v += 1; v += 1; ILFILL16 ILFILL16 ILFILL4 v += 1; } \
		}; \
	}

#define ILFILL3 \
	namespace ILFILL3 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL16 ILFILL4 v += 1; v += 1; } \
		}; \
	}

#define ILFILLB1 \
	template <int N> struct TuFill { int v; void g() { ILFILL256 ILFILL16 ILFILL4 v += 1; v += 1; v += 1; } }; \
	template <int N> struct TuMid { void g() { TuFill<N>().g(); } }; \
	template <int N> struct TuPad \
	{ \
		int v; \
		void g() { ILFILL256 ILFILL16 ILFILL16 ILFILL16 v += 1; v += 1; TuMid<N>().g(); } \
	}; \
	namespace TUPAD1 \
	{ \
		struct Pad \
		{ \
			int v; \
			ILFILLFN(0) ILFILLFN(1) ILFILLFN(2) ILFILLFN(3) \
			void g() { ILFILL256 ILFILL16 ILFILL16 ILFILL16 ILFILL16 v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; v += 1; } \
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
			void g() { v += 1; v += 1; ILFILL16 ILFILL4 ILFILL256 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 v += 1; } \
		}; \
	}

#define ILFILLTU_SHAREDDOC \
	namespace ILFILLTU_SHAREDDOC \
	{ \
		struct Pad \
		{ \
			int v; \
			ILFILLFN(0) \
			void g() { v += 1; v += 1; v += 1; v += 1; v += 1; ILFILL16 ILFILL16 v += 1; } \
		}; \
	}

#define ILFILLTU_REALMYROOMMAINUI \
	namespace ILFILLTU_REALMYROOMMAINUI \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { v += 1; v += 1; v += 1; v += 1; v += 1; ILFILL16 ILFILL16 ILFILL4 ILFILL4 v += 1; ILFILL64 ILFILL16 ILFILL16 ILFILL16 ILFILL4 ILFILL4 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL16 ILFILL4 v += 1; v += 1; } \
		}; \
	}

#define ILFILLTU_LOBBYMAIN \
	namespace ILFILLTU_LOBBYMAIN \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL16 ILFILL16 ILFILL16 ILFILL4 ILFILL4 v += 1; v += 1; } \
		}; \
	}

#define ILPAIRSS0 \
	namespace ILPAIRSS_NS \
	{ \
		typedef std::map<std::string, std::string, std::greater<std::string> > M; \
		struct H { M m; }; \
	}

#define ILPAIRSS \
	namespace ILPAIRSS_NS \
	{ \
		inline void f(const std::string& k) { M m; m[k]; } \
	}

// clang-format on
