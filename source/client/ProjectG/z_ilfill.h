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
			void g() { v += 1; v += 1; v += 1; } \
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
			void g() { ILFILL64 ILFILL16 ILFILL4 ILFILL4 v += 1; v += 1; { v += 1; } v += 200; v += 200; v += 200; v += 200; v += 200; } \
		}; \
	}

#define ILFILLTU_GOLFRULE2 \
	namespace ILFILLTU_GOLFRULE2 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL4 ILFILL4 v += 1; v += 1; } \
		}; \
	}

#define ILFILLTU_GOLFRULE3 \
	namespace ILFILLTU_GOLFRULE3 \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL64 ILFILL64 ILFILL64 } \
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
			void g() { ILFILL256 ILFILL16 ILFILL16 ILFILL16 v += 1; v += 200; v; } \
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
			void g() { v += 1; ILFILL16 ILFILL4 ILFILL256 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 ILFILL16 v += 1; } \
		}; \
	}

#define ILFILLTU_SHAREDDOC \
	namespace ILFILLTU_SHAREDDOC \
	{ \
		struct Pad \
		{ \
			int v; \
			ILFILLFN(0) \
			void g() { v += 1; v += 1; ILFILL16 ILFILL16 v += 1; } \
		}; \
	}

#define ILFILLTU_REALMYROOMMAINUI \
	namespace ILFILLTU_REALMYROOMMAINUI \
	{ \
		struct Pad \
		{ \
			int v; \
			void g() { ILFILL64 ILFILL64 ILFILL16 ILFILL16 ILFILL16 ILFILL4 ILFILL4 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL256 ILFILL16  v += 1; { } v += 200; v += 200; v += 200; v += 200; v += 200; } \
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

// HACK! pads for symbol numbering; global symbol immediates are hashed by c1 symbol number % 64
// ILNUM begin
#define ILNUM_NET namespace ILNUM_NET { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; } // n=20
#define ILNUM_GDH namespace ILNUM_GDH { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; typedef int t23; typedef int t24; typedef int t25; typedef int t26; typedef int t27; typedef int t28; typedef int t29; typedef int t30; typedef int t31; typedef int t32; typedef int t33; typedef int t34; } // n=35
#define ILNUM_PSS namespace ILNUM_PSS { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; } // n=23
#define ILNUM_FW namespace ILNUM_FW { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; } // n=22

#define ILNUM_G0 namespace ILNUM_G0 { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; } // n=12
#define ILNUM_G1 namespace ILNUM_G1 { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; typedef int t23; typedef int t24; typedef int t25; typedef int t26; typedef int t27; typedef int t28; typedef int t29; typedef int t30; typedef int t31; typedef int t32; typedef int t33; } // n=34
#define ILNUM_TB2 namespace ILNUM_TB2 { typedef int t0; } // n=1
#define ILNUM_UI namespace ILNUM_UI { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; typedef int t23; typedef int t24; typedef int t25; typedef int t26; typedef int t27; typedef int t28; typedef int t29; typedef int t30; typedef int t31; typedef int t32; typedef int t33; typedef int t34; typedef int t35; typedef int t36; typedef int t37; } // n=38
#define ILNUM_UT namespace ILNUM_UT { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; typedef int t23; typedef int t24; typedef int t25; typedef int t26; typedef int t27; typedef int t28; typedef int t29; typedef int t30; typedef int t31; typedef int t32; typedef int t33; typedef int t34; typedef int t35; typedef int t36; typedef int t37; typedef int t38; typedef int t39; typedef int t40; typedef int t41; typedef int t42; typedef int t43; typedef int t44; typedef int t45; typedef int t46; typedef int t47; typedef int t48; typedef int t49; typedef int t50; typedef int t51; typedef int t52; typedef int t53; typedef int t54; typedef int t55; typedef int t56; typedef int t57; } // n=58
#define ILNUM_UF namespace ILNUM_UF { typedef int t0; } // n=1
#define ILNUM_MASCOT namespace ILNUM_MASCOT { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; } // n=11
#define ILNUM_FXSPRAY namespace ILNUM_FXSPRAY { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; typedef int t23; typedef int t24; typedef int t25; typedef int t26; typedef int t27; typedef int t28; typedef int t29; typedef int t30; typedef int t31; } // n=32
#define ILNUM_SCENE namespace ILNUM_SCENE { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; } // n=12
#define ILNUM_SMI namespace ILNUM_SMI { typedef int t0; typedef int t1; typedef int t2; typedef int t3; } // n=4
#define ILNUM_GB namespace ILNUM_GB { typedef int t0; typedef int t1; } // n=2
#define ILNUM_GB2 namespace ILNUM_GB2 { typedef int t0; } // n=1
#define ILNUM_SD namespace ILNUM_SD { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; } // n=10
#define ILNUM_SDC namespace ILNUM_SDC { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; typedef int t23; typedef int t24; typedef int t25; typedef int t26; typedef int t27; typedef int t28; typedef int t29; typedef int t30; typedef int t31; typedef int t32; typedef int t33; typedef int t34; typedef int t35; typedef int t36; typedef int t37; typedef int t38; typedef int t39; typedef int t40; typedef int t41; typedef int t42; typedef int t43; typedef int t44; typedef int t45; typedef int t46; typedef int t47; typedef int t48; typedef int t49; typedef int t50; typedef int t51; typedef int t52; typedef int t53; } // n=54
#define ILNUM_FR namespace ILNUM_FR { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; typedef int t23; typedef int t24; typedef int t25; typedef int t26; typedef int t27; typedef int t28; typedef int t29; typedef int t30; typedef int t31; typedef int t32; typedef int t33; typedef int t34; typedef int t35; typedef int t36; typedef int t37; typedef int t38; typedef int t39; typedef int t40; typedef int t41; typedef int t42; typedef int t43; typedef int t44; typedef int t45; typedef int t46; typedef int t47; typedef int t48; } // n=49
// ILNUM end
// clang-format on
