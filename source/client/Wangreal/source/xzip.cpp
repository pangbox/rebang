// XZip 1.3 by Lucian Wischik, with the Info-ZIP deflate compressor.
#define WIN32_LEAN_AND_MEAN
#include "xzip.h"
#include <string.h>
#include <time.h>
#include <new>
extern "C" __declspec(dllimport) int __cdecl stricmp(const char*, const char*);

#define LENGTH_CODES 29
#define LITERALS 256
#define L_CODES (LITERALS + 1 + LENGTH_CODES)
#define D_CODES 30
#define BL_CODES 19
#define HEAP_SIZE (2 * L_CODES + 1)
#define MAX_BITS 15
#define MAX_BL_BITS 7
#define END_BLOCK 256
#define REP_3_6 16
#define REPZ_3_10 17
#define REPZ_11_138 18
#define WSIZE 0x8000
#define LIT_BUFSIZE 0x8000
#define DIST_BUFSIZE LIT_BUFSIZE
#define MIN_MATCH 3
#define MAX_MATCH 258
#define MIN_LOOKAHEAD (MAX_MATCH + MIN_MATCH + 1)
#define MAX_DIST (WSIZE - MIN_LOOKAHEAD)
#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES 2
#define BINARY 0
#define ASCII 1
#define Buf_size 16
#define UNKNOWN 0xffff
#define HASH_BITS 15
#define HASH_SIZE (unsigned int)(1 << HASH_BITS)
#define HASH_MASK (HASH_SIZE - 1)
#define WMASK (WSIZE - 1)
#define NIL 0
#define FAST 4
#define SLOW 2
#define TOO_FAR 4096
#define H_SHIFT ((HASH_BITS + MIN_MATCH - 1) / MIN_MATCH)

typedef unsigned int IPos;

typedef unsigned char uch;
typedef unsigned short ush;
typedef unsigned long ulg;

struct ct_data
{
	union
	{
		ush freq;
		ush code;
	} fc;
	union
	{
		ush dad;
		ush len;
	} dl;
};

struct tree_desc
{
	ct_data* dyn_tree;
	ct_data* static_tree;
	const int* extra_bits;
	int extra_base;
	int elems;
	int max_length;
	int max_code;
};

struct config
{
	ush good_length;
	ush max_lazy;
	ush nice_length;
	ush max_chain;
};

struct iztimes
{
	int atime;
	int mtime;
	int ctime;
};

struct zlist
{
	ush vem, ver, flg, how;
	unsigned int tim, crc, siz, len, nam, ext, cext, com;
	ush dsk, att, lflg;
	unsigned int atx, off;
	char name[260];
	char* extra;
	char* cextra;
	char* comment;
	char iname[260];
	char zname[260];
	int mark;
	int trash;
	int dosflag;
	zlist* nxt;
};

class TState;

class TTreeState
{
public:
	TTreeState(void);

	ct_data dyn_ltree[HEAP_SIZE];
	ct_data dyn_dtree[2 * D_CODES + 1];
	ct_data static_ltree[L_CODES + 2];
	ct_data static_dtree[D_CODES];
	ct_data bl_tree[2 * BL_CODES + 1];
	tree_desc l_desc;
	tree_desc d_desc;
	tree_desc bl_desc;
	ush bl_count[MAX_BITS + 1];
	int heap[HEAP_SIZE];
	int heap_len;
	int heap_max;
	uch depth[HEAP_SIZE];
	uch length_code[MAX_MATCH - MIN_MATCH + 1];
	uch dist_code[512];
	int base_length[LENGTH_CODES];
	int base_dist[D_CODES];
	uch l_buf[LIT_BUFSIZE];
	ush d_buf[DIST_BUFSIZE];
	uch flag_buf[LIT_BUFSIZE / 8];
	unsigned int last_lit;
	unsigned int last_dist;
	unsigned int last_flags;
	uch flags;
	uch flag_bit;
	ulg opt_len;
	ulg static_len;
	ulg cmpr_bytelen;
	ulg cmpr_len_bits;
	ulg input_len;
	ush* file_type;
};

struct TBitState
{
	int flush_flg;
	unsigned int bi_buf;
	int bi_valid;
	char* out_buf;
	unsigned int out_offset;
	unsigned int out_size;
	ulg bits_sent;
};

class TDeflateState
{
public:
	TDeflateState(void);

	uch window[2 * WSIZE];
	unsigned int prev[WSIZE];
	unsigned int head[WSIZE];
	unsigned int window_size;
	int block_start;
	int sliding;
	unsigned int ins_h;
	unsigned int prev_length;
	unsigned int strstart;
	unsigned int match_start;
	int eofile;
	unsigned int lookahead;
	unsigned int max_chain_length;
	unsigned int max_lazy_match;
	unsigned int good_match;
	int nice_match;
};

class TState
{
public:
	TState(void);

	void* param;
	int level;
	bool seekable;
	unsigned int (*readfunc)(TState&, char*, unsigned int);
	unsigned int (*flush_outbuf)(void*, const char*, unsigned int*);
	TTreeState ts;
	TBitState bs;
	TDeflateState ds;
	const char* err;
};

typedef unsigned int (
	*WRITEFUNC)(void* param, const char* buf, unsigned int size);

#define LOCSIG 0x04034b50L
#define EXTLOCSIG 0x08074b50L
#define CENSIG 0x02014b50L
#define ENDSIG 0x06054b50L
#define ZE_OK 0
#define ZE_TEMP 10
#define STORE 0
#define DEFLATE 8
#define LOCHEAD 26
#define EB_L_UT_SIZE 17
#define EB_C_UT_SIZE 9
#define CRCVAL_INITIAL 0L
#define MAX_PATH_Z 260

class TZip
{
public:
	TZip(void);
	~TZip(void);

	void* hfout;
	void* hmapout;
	unsigned int ooffset;
	ZRESULT oerr;
	unsigned int writ;
	bool ocanseek;
	char* obuf;
	unsigned int opos;
	unsigned int mapsize;
	bool hasputcen;
	zlist* zfis;

	ZRESULT Create(void* z, unsigned int len, unsigned long flags,
		unsigned long timestamp);
	static unsigned int sflush(void* param, const char* buf,
		unsigned int* size);
	static unsigned int swrite(void* param, const char* buf, unsigned int size);
	unsigned int write(const char* buf, unsigned int size);
	bool oseek(unsigned int pos);
	ZRESULT GetMemory(void** pbuf, unsigned long* plen);
	ZRESULT Close(void);

	unsigned long attr;
	iztimes times;
	unsigned long timestamp;
	bool iseekable;
	long isize;
	int ired;
	unsigned long crc;
	void* hfin;
	bool selfclosehf;
	const char* bufin;
	unsigned int lenin;
	unsigned int posin;
	unsigned int csize;

	ZRESULT open_file(const char* fn);
	ZRESULT open_handle(void* hf, unsigned int len);
	ZRESULT open_mem(void* src, unsigned int len);
	ZRESULT open_dir(void);
	static unsigned int sread(TState& s, char* buf, unsigned int size);
	unsigned int read(char* buf, unsigned int size);
	ZRESULT iclose(void);

	ZRESULT ideflate(zlist* zfi);
	ZRESULT istore(void);

	char buf[16384];

	ZRESULT Add(const char* odstzn, void* src, unsigned int len,
		unsigned long flags);
	ZRESULT AddCentral(void);
};

struct TZipHandleData
{
	unsigned int flag;
	TZip* zip;
};

void Assert(TState& state, bool cond, const char* msg);
void init_block(TState& state);
void set_file_type(TState& state);
void bi_init(TState& state, char* tgt_buf, unsigned int tgt_size,
	int flsh_allowed);
unsigned int bi_reverse(unsigned int code, int len);
bool IsZipHandleZ(HZIP__* h);
bool HasZipSuffix(const char* fn);
HZIP__* CreateZipZ(void* z, unsigned int len, unsigned long flags,
	unsigned long timestamp);
ZRESULT ZipAdd(HZIP__* hz, const char* dstzn, void* src, unsigned int len,
	unsigned long flags);
ZRESULT ZipGetMemory(HZIP__* hz, void** buf, unsigned long* len);
ZRESULT CloseZipZ(HZIP__* hz);
long filetime2timet(FILETIME ft);
unsigned long GetFileInfo(void* hf, unsigned long* attr, long* size,
	iztimes* times, unsigned long* timestamp);
int putlocal(zlist* z, WRITEFUNC wfunc, void* param);
int putextended(zlist* z, WRITEFUNC wfunc, void* param);
int putcentral(zlist* z, WRITEFUNC wfunc, void* param);
int putend(int n, ulg s, ulg c, unsigned int m, char* z, WRITEFUNC wfunc,
	void* param);
void gen_codes(TState& state, ct_data* tree, int max_code);
void pqdownheap(TState& state, ct_data* tree, int k);
void scan_tree(TState& state, ct_data* tree, int max_code);
void lm_init(TState& state, int pack_level, ush* flags);
void fill_window(TState& state);
int longest_match(TState& state, IPos cur_match);
unsigned long deflate_fast(TState& state);
unsigned long deflate(TState& state);
void ct_init(TState& state, ush* attr);
int ct_tally(TState& state, int dist, int lc);
unsigned long flush_block(TState& state, char* buf, ulg stored_len, int eof);
void gen_bitlen(TState& state, tree_desc* desc);
void build_tree(TState& state, tree_desc* desc);
void send_tree(TState& state, ct_data* tree, int max_code);
int build_bl_tree(TState& state);
void send_all_trees(TState& state, int lcodes, int dcodes, int blcodes);
void compress_block(TState& state, ct_data* ltree, ct_data* dtree);
void send_bits(TState& state, int value, int length);
void bi_windup(TState& state);
void copy_block(TState& state, char* block, unsigned int len, int header);
unsigned long crc32(unsigned long crc, const uch* buf, unsigned int len);

ZRESULT lasterrorZ = ZR_OK;

#define smaller(tree, n, m) \
	(tree[n].fc.freq < tree[m].fc.freq || \
		(tree[n].fc.freq == tree[m].fc.freq && \
			state.ts.depth[n] <= state.ts.depth[m]))

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf) \
	DO1(buf); \
	DO1(buf);
#define DO4(buf) \
	DO2(buf); \
	DO2(buf);
#define DO8(buf) \
	DO4(buf); \
	DO4(buf);

#define put_byte(s, c) \
	{ \
		if (s.bs.out_offset >= s.bs.out_size) \
		{ \
			s.flush_outbuf(s.param, s.bs.out_buf, &s.bs.out_offset); \
		} \
		s.bs.out_buf[s.bs.out_offset++] = (char)(c); \
	}

#define put_short(s, w) \
	{ \
		if (s.bs.out_offset >= s.bs.out_size - 1) \
		{ \
			s.flush_outbuf(s.param, s.bs.out_buf, &s.bs.out_offset); \
		} \
		s.bs.out_buf[s.bs.out_offset++] = (char)((w) & 0xff); \
		s.bs.out_buf[s.bs.out_offset++] = (char)((ush)(w) >> 8); \
	}

const int extra_lbits[30] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3,
	3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0 };
const int extra_dbits[30] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
	7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
const int extra_blbits[19] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 3, 7 };
const unsigned char bl_order[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4,
	12, 3, 13, 2, 14, 1, 15 };
const config configuration_table[10] = {
	{ 0,  0,   0,   0    },
    { 4,  4,   8,   4    },
    { 4,  5,   16,  8    },
    { 4,  6,   32,  32   },
	{ 4,  4,   16,  16   },
    { 8,  16,  32,  32   },
    { 8,  16,  128, 128  },
	{ 8,  32,  128, 256  },
    { 32, 128, 258, 1024 },
    { 32, 258, 258, 4096 }
};
const unsigned long crc_table[256] = { 0x00000000UL, 0x77073096UL, 0xee0e612cUL,
	0x990951baUL, 0x076dc419UL, 0x706af48fUL, 0xe963a535UL, 0x9e6495a3UL,
	0x0edb8832UL, 0x79dcb8a4UL, 0xe0d5e91eUL, 0x97d2d988UL, 0x09b64c2bUL,
	0x7eb17cbdUL, 0xe7b82d07UL, 0x90bf1d91UL, 0x1db71064UL, 0x6ab020f2UL,
	0xf3b97148UL, 0x84be41deUL, 0x1adad47dUL, 0x6ddde4ebUL, 0xf4d4b551UL,
	0x83d385c7UL, 0x136c9856UL, 0x646ba8c0UL, 0xfd62f97aUL, 0x8a65c9ecUL,
	0x14015c4fUL, 0x63066cd9UL, 0xfa0f3d63UL, 0x8d080df5UL, 0x3b6e20c8UL,
	0x4c69105eUL, 0xd56041e4UL, 0xa2677172UL, 0x3c03e4d1UL, 0x4b04d447UL,
	0xd20d85fdUL, 0xa50ab56bUL, 0x35b5a8faUL, 0x42b2986cUL, 0xdbbbc9d6UL,
	0xacbcf940UL, 0x32d86ce3UL, 0x45df5c75UL, 0xdcd60dcfUL, 0xabd13d59UL,
	0x26d930acUL, 0x51de003aUL, 0xc8d75180UL, 0xbfd06116UL, 0x21b4f4b5UL,
	0x56b3c423UL, 0xcfba9599UL, 0xb8bda50fUL, 0x2802b89eUL, 0x5f058808UL,
	0xc60cd9b2UL, 0xb10be924UL, 0x2f6f7c87UL, 0x58684c11UL, 0xc1611dabUL,
	0xb6662d3dUL, 0x76dc4190UL, 0x01db7106UL, 0x98d220bcUL, 0xefd5102aUL,
	0x71b18589UL, 0x06b6b51fUL, 0x9fbfe4a5UL, 0xe8b8d433UL, 0x7807c9a2UL,
	0x0f00f934UL, 0x9609a88eUL, 0xe10e9818UL, 0x7f6a0dbbUL, 0x086d3d2dUL,
	0x91646c97UL, 0xe6635c01UL, 0x6b6b51f4UL, 0x1c6c6162UL, 0x856530d8UL,
	0xf262004eUL, 0x6c0695edUL, 0x1b01a57bUL, 0x8208f4c1UL, 0xf50fc457UL,
	0x65b0d9c6UL, 0x12b7e950UL, 0x8bbeb8eaUL, 0xfcb9887cUL, 0x62dd1ddfUL,
	0x15da2d49UL, 0x8cd37cf3UL, 0xfbd44c65UL, 0x4db26158UL, 0x3ab551ceUL,
	0xa3bc0074UL, 0xd4bb30e2UL, 0x4adfa541UL, 0x3dd895d7UL, 0xa4d1c46dUL,
	0xd3d6f4fbUL, 0x4369e96aUL, 0x346ed9fcUL, 0xad678846UL, 0xda60b8d0UL,
	0x44042d73UL, 0x33031de5UL, 0xaa0a4c5fUL, 0xdd0d7cc9UL, 0x5005713cUL,
	0x270241aaUL, 0xbe0b1010UL, 0xc90c2086UL, 0x5768b525UL, 0x206f85b3UL,
	0xb966d409UL, 0xce61e49fUL, 0x5edef90eUL, 0x29d9c998UL, 0xb0d09822UL,
	0xc7d7a8b4UL, 0x59b33d17UL, 0x2eb40d81UL, 0xb7bd5c3bUL, 0xc0ba6cadUL,
	0xedb88320UL, 0x9abfb3b6UL, 0x03b6e20cUL, 0x74b1d29aUL, 0xead54739UL,
	0x9dd277afUL, 0x04db2615UL, 0x73dc1683UL, 0xe3630b12UL, 0x94643b84UL,
	0x0d6d6a3eUL, 0x7a6a5aa8UL, 0xe40ecf0bUL, 0x9309ff9dUL, 0x0a00ae27UL,
	0x7d079eb1UL, 0xf00f9344UL, 0x8708a3d2UL, 0x1e01f268UL, 0x6906c2feUL,
	0xf762575dUL, 0x806567cbUL, 0x196c3671UL, 0x6e6b06e7UL, 0xfed41b76UL,
	0x89d32be0UL, 0x10da7a5aUL, 0x67dd4accUL, 0xf9b9df6fUL, 0x8ebeeff9UL,
	0x17b7be43UL, 0x60b08ed5UL, 0xd6d6a3e8UL, 0xa1d1937eUL, 0x38d8c2c4UL,
	0x4fdff252UL, 0xd1bb67f1UL, 0xa6bc5767UL, 0x3fb506ddUL, 0x48b2364bUL,
	0xd80d2bdaUL, 0xaf0a1b4cUL, 0x36034af6UL, 0x41047a60UL, 0xdf60efc3UL,
	0xa867df55UL, 0x316e8eefUL, 0x4669be79UL, 0xcb61b38cUL, 0xbc66831aUL,
	0x256fd2a0UL, 0x5268e236UL, 0xcc0c7795UL, 0xbb0b4703UL, 0x220216b9UL,
	0x5505262fUL, 0xc5ba3bbeUL, 0xb2bd0b28UL, 0x2bb45a92UL, 0x5cb36a04UL,
	0xc2d7ffa7UL, 0xb5d0cf31UL, 0x2cd99e8bUL, 0x5bdeae1dUL, 0x9b64c2b0UL,
	0xec63f226UL, 0x756aa39cUL, 0x026d930aUL, 0x9c0906a9UL, 0xeb0e363fUL,
	0x72076785UL, 0x05005713UL, 0x95bf4a82UL, 0xe2b87a14UL, 0x7bb12baeUL,
	0x0cb61b38UL, 0x92d28e9bUL, 0xe5d5be0dUL, 0x7cdcefb7UL, 0x0bdbdf21UL,
	0x86d3d2d4UL, 0xf1d4e242UL, 0x68ddb3f8UL, 0x1fda836eUL, 0x81be16cdUL,
	0xf6b9265bUL, 0x6fb077e1UL, 0x18b74777UL, 0x88085ae6UL, 0xff0f6a70UL,
	0x66063bcaUL, 0x11010b5cUL, 0x8f659effUL, 0xf862ae69UL, 0x616bffd3UL,
	0x166ccf45UL, 0xa00ae278UL, 0xd70dd2eeUL, 0x4e048354UL, 0x3903b3c2UL,
	0xa7672661UL, 0xd06016f7UL, 0x4969474dUL, 0x3e6e77dbUL, 0xaed16a4aUL,
	0xd9d65adcUL, 0x40df0b66UL, 0x37d83bf0UL, 0xa9bcae53UL, 0xdebb9ec5UL,
	0x47b2cf7fUL, 0x30b5ffe9UL, 0xbdbdf21cUL, 0xcabac28aUL, 0x53b39330UL,
	0x24b4a3a6UL, 0xbad03605UL, 0xcdd70693UL, 0x54de5729UL, 0x23d967bfUL,
	0xb3667a2eUL, 0xc4614ab8UL, 0x5d681b02UL, 0x2a6f2b94UL, 0xb40bbe37UL,
	0xc30c8ea1UL, 0x5a05df1bUL, 0x2d02ef8dUL };

#define SMALLEST 1
#define MAX(a, b) (a >= b ? a : b)
#define send_code(s, c, tree) send_bits(s, tree[c].fc.code, tree[c].dl.len)
#define d_code(dist) \
	((dist) < 256 ? state.ts.dist_code[dist] \
				  : state.ts.dist_code[256 + ((dist) >> 7)])
#define pqremove(s, tree, top) \
	{ \
		top = s.ts.heap[SMALLEST]; \
		s.ts.heap[SMALLEST] = s.ts.heap[s.ts.heap_len--]; \
		pqdownheap(s, tree, SMALLEST); \
	}

#define UPDATE_HASH(h, c) (h = (((h) << H_SHIFT) ^ (c)) & HASH_MASK)
#define INSERT_STRING(s, str, match_head) \
	(UPDATE_HASH(s.ds.ins_h, s.ds.window[(str) + (MIN_MATCH - 1)]), \
		s.ds.prev[(str) & WMASK] = match_head = s.ds.head[s.ds.ins_h], \
		s.ds.head[s.ds.ins_h] = (str))
#define FLUSH_BLOCK(s, eof) \
	flush_block(s, \
		s.ds.block_start >= 0L \
			? (char*)&s.ds.window[(unsigned int)s.ds.block_start] \
			: (char*)0, \
		(long)s.ds.strstart - s.ds.block_start, (eof))

#define PUTSH(a, f) \
	{ \
		char _putsh_c = (char)((a) & 0xff); \
		wfunc(param, &_putsh_c, 1); \
		_putsh_c = (char)((a) >> 8); \
		wfunc(param, &_putsh_c, 1); \
	}
#define PUTLG(a, f) { PUTSH((a) & 0xffff, f) PUTSH((a) >> 16, f) }

TTreeState::TTreeState(void)
{
	tree_desc a = { dyn_ltree, static_ltree, extra_lbits, LITERALS + 1, L_CODES,
		MAX_BITS, 0 };
	l_desc = a;
	tree_desc b = { dyn_dtree, static_dtree, extra_dbits, 0, D_CODES, MAX_BITS,
		0 };
	d_desc = b;
	tree_desc c = { bl_tree, 0, extra_blbits, 0, BL_CODES, MAX_BL_BITS, 0 };
	bl_desc = c;
	last_lit = 0;
	last_dist = 0;
	last_flags = 0;
}

TDeflateState::TDeflateState(void)
{
	window_size = 0;
}

TState::TState(void)
{
	err = 0;
}

void Assert(TState& state, bool cond, const char* msg)
{
	if (cond)
		return;
	state.err = msg;
}

void __cdecl Trace(const char*, ...)
{
}

void ct_init(TState& state, ush* attr)
{
	int n;
	int bits;
	int length;
	int code;
	int dist;

	state.ts.file_type = attr;
	state.ts.cmpr_bytelen = state.ts.cmpr_len_bits = 0L;
	state.ts.input_len = 0L;

	if (state.ts.static_dtree[0].dl.len != 0)
		return;

	length = 0;
	for (code = 0; code < LENGTH_CODES - 1; code++)
	{
		state.ts.base_length[code] = length;
		for (n = 0; n < (1 << extra_lbits[code]); n++)
		{
			state.ts.length_code[length++] = (uch)code;
		}
	}
	Assert(state, length == 256, "ct_init: length != 256");
	state.ts.length_code[length - 1] = (uch)code;

	dist = 0;
	for (code = 0; code < 16; code++)
	{
		state.ts.base_dist[code] = dist;
		for (n = 0; n < (1 << extra_dbits[code]); n++)
		{
			state.ts.dist_code[dist++] = (uch)code;
		}
	}
	Assert(state, dist == 256, "ct_init: dist != 256");
	dist >>= 7;
	for (; code < D_CODES; code++)
	{
		state.ts.base_dist[code] = dist << 7;
		for (n = 0; n < (1 << (extra_dbits[code] - 7)); n++)
		{
			state.ts.dist_code[256 + dist++] = (uch)code;
		}
	}
	Assert(state, dist == 256, "ct_init: 256+dist != 512");

	for (bits = 0; bits <= MAX_BITS; bits++)
		state.ts.bl_count[bits] = 0;
	n = 0;
	while (n <= 143)
		state.ts.static_ltree[n++].dl.len = 8, state.ts.bl_count[8]++;
	while (n <= 255)
		state.ts.static_ltree[n++].dl.len = 9, state.ts.bl_count[9]++;
	while (n <= 279)
		state.ts.static_ltree[n++].dl.len = 7, state.ts.bl_count[7]++;
	while (n <= 287)
		state.ts.static_ltree[n++].dl.len = 8, state.ts.bl_count[8]++;
	gen_codes(state, (ct_data*)state.ts.static_ltree, L_CODES + 1);

	for (n = 0; n < D_CODES; n++)
	{
		state.ts.static_dtree[n].dl.len = 5;
		state.ts.static_dtree[n].fc.code = (ush)bi_reverse(n, 5);
	}

	init_block(state);
}

void init_block(TState& state)
{
	int n;
	for (n = 0; n < L_CODES; n++)
		state.ts.dyn_ltree[n].fc.freq = 0;
	for (n = 0; n < D_CODES; n++)
		state.ts.dyn_dtree[n].fc.freq = 0;
	for (n = 0; n < BL_CODES; n++)
		state.ts.bl_tree[n].fc.freq = 0;

	state.ts.dyn_ltree[END_BLOCK].fc.freq = 1;
	state.ts.opt_len = state.ts.static_len = 0L;
	state.ts.last_lit = state.ts.last_dist = state.ts.last_flags = 0;
	state.ts.flags = 0;
	state.ts.flag_bit = 1;
}

void pqdownheap(TState& state, ct_data* tree, int k)
{
	int v = state.ts.heap[k];
	int j = k << 1;
	while (j <= state.ts.heap_len)
	{
		if (j < state.ts.heap_len &&
			smaller(tree, state.ts.heap[j + 1], state.ts.heap[j]))
			j++;
		if (smaller(tree, v, state.ts.heap[j]))
			break;

		state.ts.heap[k] = state.ts.heap[j];
		k = j;
		j <<= 1;
	}
	state.ts.heap[k] = v;
}

void gen_bitlen(TState& state, tree_desc* desc)
{
	ct_data* tree = desc->dyn_tree;
	const int* extra = desc->extra_bits;
	int base = desc->extra_base;
	int max_code = desc->max_code;
	int max_length = desc->max_length;
	ct_data* stree = desc->static_tree;
	int h;
	int n, m;
	int bits;
	int xbits;
	ush f;
	int overflow = 0;

	for (bits = 0; bits <= MAX_BITS; bits++)
		state.ts.bl_count[bits] = 0;

	tree[state.ts.heap[state.ts.heap_max]].dl.len = 0;

	for (h = state.ts.heap_max + 1; h < HEAP_SIZE; h++)
	{
		n = state.ts.heap[h];
		bits = tree[tree[n].dl.dad].dl.len + 1;
		if (bits > max_length)
			bits = max_length, overflow++;
		tree[n].dl.len = (ush)bits;

		if (n > max_code)
			continue;

		state.ts.bl_count[bits]++;
		xbits = 0;
		if (n >= base)
			xbits = extra[n - base];
		f = tree[n].fc.freq;
		state.ts.opt_len += (ulg)f * (bits + xbits);
		if (stree)
			state.ts.static_len += (ulg)f * (stree[n].dl.len + xbits);
	}
	if (overflow == 0)
		return;

	do
	{
		bits = max_length - 1;
		while (state.ts.bl_count[bits] == 0)
			bits--;
		state.ts.bl_count[bits]--;
		state.ts.bl_count[bits + 1] += (ush)2;
		state.ts.bl_count[max_length]--;
		overflow -= 2;
	} while (overflow > 0);

	for (bits = max_length; bits != 0; bits--)
	{
		n = state.ts.bl_count[bits];
		while (n != 0)
		{
			m = state.ts.heap[--h];
			if (m > max_code)
				continue;
			if (tree[m].dl.len != (ush)bits)
			{
				state.ts.opt_len +=
					((long)bits - (long)tree[m].dl.len) * (long)tree[m].fc.freq;
				tree[m].dl.len = (ush)bits;
			}
			n--;
		}
	}
}

void gen_codes(TState& state, ct_data* tree, int max_code)
{
	ush next_code[MAX_BITS + 1];
	ush code = 0;
	int bits;
	int n;

	for (bits = 1; bits <= MAX_BITS; bits++)
	{
		next_code[bits] = code =
			(ush)((code + state.ts.bl_count[bits - 1]) << 1);
	}
	Assert(state, code + state.ts.bl_count[MAX_BITS] - 1 == (1 << MAX_BITS) - 1,
		"inconsistent bit counts");

	for (n = 0; n <= max_code; n++)
	{
		int len = tree[n].dl.len;
		if (len == 0)
			continue;
		tree[n].fc.code = (ush)bi_reverse(next_code[len]++, len);
	}
}

void build_tree(TState& state, tree_desc* desc)
{
	ct_data* tree = desc->dyn_tree;
	ct_data* stree = desc->static_tree;
	int elems = desc->elems;
	int n, m;
	int max_code = -1;
	int node = elems;

	state.ts.heap_len = 0, state.ts.heap_max = HEAP_SIZE;

	for (n = 0; n < elems; n++)
	{
		if (tree[n].fc.freq != 0)
		{
			state.ts.heap[++state.ts.heap_len] = max_code = n;
			state.ts.depth[n] = 0;
		}
		else
		{
			tree[n].dl.len = 0;
		}
	}

	while (state.ts.heap_len < 2)
	{
		int newp = state.ts.heap[++state.ts.heap_len] =
			(max_code < 2 ? ++max_code : 0);
		tree[newp].fc.freq = 1;
		state.ts.depth[newp] = 0;
		state.ts.opt_len--;
		if (stree)
			state.ts.static_len -= stree[newp].dl.len;
	}
	desc->max_code = max_code;

	for (n = state.ts.heap_len / 2; n >= 1; n--)
		pqdownheap(state, tree, n);

	do
	{
		pqremove(state, tree, n);
		m = state.ts.heap[SMALLEST];

		state.ts.heap[--state.ts.heap_max] = n;
		state.ts.heap[--state.ts.heap_max] = m;

		tree[node].fc.freq = (ush)(tree[n].fc.freq + tree[m].fc.freq);
		state.ts.depth[node] =
			(uch)(MAX(state.ts.depth[n], state.ts.depth[m]) + 1);
		tree[n].dl.dad = tree[m].dl.dad = (ush)node;
		state.ts.heap[SMALLEST] = node++;
		pqdownheap(state, tree, SMALLEST);

	} while (state.ts.heap_len >= 2);

	state.ts.heap[--state.ts.heap_max] = state.ts.heap[SMALLEST];

	gen_bitlen(state, (tree_desc*)desc);

	gen_codes(state, (ct_data*)tree, max_code);
}

void scan_tree(TState& state, ct_data* tree, int max_code)
{
	int n;
	int prevlen = -1;
	int curlen;
	int nextlen = tree[0].dl.len;
	int count = 0;
	int max_count = 7;
	int min_count = 4;

	if (nextlen == 0)
		max_count = 138, min_count = 3;
	tree[max_code + 1].dl.len = (ush)-1;

	for (n = 0; n <= max_code; n++)
	{
		curlen = nextlen;
		nextlen = tree[n + 1].dl.len;
		if (++count < max_count && curlen == nextlen)
		{
			continue;
		}
		else if (count < min_count)
		{
			state.ts.bl_tree[curlen].fc.freq += (ush)count;
		}
		else if (curlen != 0)
		{
			if (curlen != prevlen)
				state.ts.bl_tree[curlen].fc.freq++;
			state.ts.bl_tree[REP_3_6].fc.freq++;
		}
		else if (count <= 10)
		{
			state.ts.bl_tree[REPZ_3_10].fc.freq++;
		}
		else
		{
			state.ts.bl_tree[REPZ_11_138].fc.freq++;
		}
		count = 0;
		prevlen = curlen;
		if (nextlen == 0)
		{
			max_count = 138, min_count = 3;
		}
		else if (curlen == nextlen)
		{
			max_count = 6, min_count = 3;
		}
		else
		{
			max_count = 7, min_count = 4;
		}
	}
}

void send_tree(TState& state, ct_data* tree, int max_code)
{
	int n;
	int prevlen = -1;
	int curlen;
	int nextlen = tree[0].dl.len;
	int count = 0;
	int max_count = 7;
	int min_count = 4;

	if (nextlen == 0)
		max_count = 138, min_count = 3;

	for (n = 0; n <= max_code; n++)
	{
		curlen = nextlen;
		nextlen = tree[n + 1].dl.len;
		if (++count < max_count && curlen == nextlen)
		{
			continue;
		}
		else if (count < min_count)
		{
			do
			{
				send_code(state, curlen, state.ts.bl_tree);
			} while (--count != 0);
		}
		else if (curlen != 0)
		{
			if (curlen != prevlen)
			{
				send_code(state, curlen, state.ts.bl_tree);
				count--;
			}
			Assert(state, count >= 3 && count <= 6, " 3_6?");
			send_code(state, REP_3_6, state.ts.bl_tree);
			send_bits(state, count - 3, 2);
		}
		else if (count <= 10)
		{
			send_code(state, REPZ_3_10, state.ts.bl_tree);
			send_bits(state, count - 3, 3);
		}
		else
		{
			send_code(state, REPZ_11_138, state.ts.bl_tree);
			send_bits(state, count - 11, 7);
		}
		count = 0;
		prevlen = curlen;
		if (nextlen == 0)
		{
			max_count = 138, min_count = 3;
		}
		else if (curlen == nextlen)
		{
			max_count = 6, min_count = 3;
		}
		else
		{
			max_count = 7, min_count = 4;
		}
	}
}

int build_bl_tree(TState& state)
{
	int max_blindex;

	scan_tree(state, (ct_data*)state.ts.dyn_ltree, state.ts.l_desc.max_code);
	scan_tree(state, (ct_data*)state.ts.dyn_dtree, state.ts.d_desc.max_code);

	build_tree(state, (tree_desc*)(&state.ts.bl_desc));

	for (max_blindex = BL_CODES - 1; max_blindex >= 3; max_blindex--)
	{
		if (state.ts.bl_tree[bl_order[max_blindex]].dl.len != 0)
			break;
	}
	state.ts.opt_len += 3 * (max_blindex + 1) + 5 + 5 + 4;

	return max_blindex;
}

void send_all_trees(TState& state, int lcodes, int dcodes, int blcodes)
{
	int rank;

	Assert(state, lcodes >= 257 && dcodes >= 1 && blcodes >= 4,
		"not enough codes");
	Assert(state, lcodes <= L_CODES && dcodes <= D_CODES && blcodes <= BL_CODES,
		"too many codes");
	send_bits(state, lcodes - 257, 5);
	send_bits(state, dcodes - 1, 5);
	send_bits(state, blcodes - 4, 4);
	for (rank = 0; rank < blcodes; rank++)
	{
		send_bits(state, state.ts.bl_tree[bl_order[rank]].dl.len, 3);
	}

	send_tree(state, (ct_data*)state.ts.dyn_ltree, lcodes - 1);

	send_tree(state, (ct_data*)state.ts.dyn_dtree, dcodes - 1);
}

unsigned long flush_block(TState& state, char* buf, ulg stored_len, int eof)
{
	ulg opt_lenb, static_lenb;
	int max_blindex;

	state.ts.flag_buf[state.ts.last_flags] = state.ts.flags;

	if (*state.ts.file_type == (ush)UNKNOWN)
		set_file_type(state);

	build_tree(state, (tree_desc*)(&state.ts.l_desc));
	build_tree(state, (tree_desc*)(&state.ts.d_desc));

	max_blindex = build_bl_tree(state);

	opt_lenb = (state.ts.opt_len + 3 + 7) >> 3;
	static_lenb = (state.ts.static_len + 3 + 7) >> 3;
	state.ts.input_len += stored_len;

	if (static_lenb <= opt_lenb)
		opt_lenb = static_lenb;

	if (stored_len + 4 <= opt_lenb && buf != (char*)0)
	{
		send_bits(state, (STORED_BLOCK << 1) + eof, 3);
		state.ts.cmpr_bytelen +=
			((state.ts.cmpr_len_bits + 3 + 7) >> 3) + stored_len + 4;
		state.ts.cmpr_len_bits = 0L;

		copy_block(state, buf, (unsigned int)stored_len, 1);
	}
	else if (static_lenb == opt_lenb)
	{
		send_bits(state, (STATIC_TREES << 1) + eof, 3);
		compress_block(state, (ct_data*)state.ts.static_ltree,
			(ct_data*)state.ts.static_dtree);
		state.ts.cmpr_len_bits += 3 + state.ts.static_len;
		state.ts.cmpr_bytelen += state.ts.cmpr_len_bits >> 3;
		state.ts.cmpr_len_bits &= 7L;
	}
	else
	{
		send_bits(state, (DYN_TREES << 1) + eof, 3);
		send_all_trees(state, state.ts.l_desc.max_code + 1,
			state.ts.d_desc.max_code + 1, max_blindex + 1);
		compress_block(state, (ct_data*)state.ts.dyn_ltree,
			(ct_data*)state.ts.dyn_dtree);
		state.ts.cmpr_len_bits += 3 + state.ts.opt_len;
		state.ts.cmpr_bytelen += state.ts.cmpr_len_bits >> 3;
		state.ts.cmpr_len_bits &= 7L;
	}

	Assert(state,
		((state.ts.cmpr_bytelen << 3) + state.ts.cmpr_len_bits) ==
			state.bs.bits_sent,
		"bad compressed size");
	init_block(state);

	if (eof)
	{
		bi_windup(state);
		state.ts.cmpr_len_bits += 7;
	}
	return state.ts.cmpr_bytelen + (state.ts.cmpr_len_bits >> 3);
}

int ct_tally(TState& state, int dist, int lc)
{
	state.ts.l_buf[state.ts.last_lit++] = (uch)lc;
	if (dist == 0)
	{
		state.ts.dyn_ltree[lc].fc.freq++;
	}
	else
	{
		dist--;
		Assert(state,
			(ush)dist < (ush)MAX_DIST &&
				(ush)lc <= (ush)(MAX_MATCH - MIN_MATCH) &&
				(ush)d_code(dist) < (ush)D_CODES,
			"ct_tally: bad match");

		state.ts.dyn_ltree[state.ts.length_code[lc] + LITERALS + 1].fc.freq++;
		state.ts.dyn_dtree[d_code(dist)].fc.freq++;

		state.ts.d_buf[state.ts.last_dist++] = (ush)dist;
		state.ts.flags |= state.ts.flag_bit;
	}
	state.ts.flag_bit <<= 1;

	if ((state.ts.last_lit & 7) == 0)
	{
		state.ts.flag_buf[state.ts.last_flags++] = state.ts.flags;
		state.ts.flags = 0, state.ts.flag_bit = 1;
	}
	if (state.level > 2 && (state.ts.last_lit & 0xfff) == 0)
	{
		ulg out_length = (ulg)state.ts.last_lit * 8L;
		ulg in_length = (ulg)state.ds.strstart - state.ds.block_start;
		int dcode;
		for (dcode = 0; dcode < D_CODES; dcode++)
		{
			out_length += (ulg)state.ts.dyn_dtree[dcode].fc.freq *
				(5L + extra_dbits[dcode]);
		}
		out_length >>= 3;
		if (state.ts.last_dist < state.ts.last_lit / 2 &&
			out_length < in_length / 2)
			return 1;
	}
	return (state.ts.last_lit == LIT_BUFSIZE - 1 ||
		state.ts.last_dist == DIST_BUFSIZE);
}

void compress_block(TState& state, ct_data* ltree, ct_data* dtree)
{
	unsigned int dist;
	int lc;
	unsigned int lx = 0;
	unsigned int dx = 0;
	unsigned int fx = 0;
	uch flag = 0;
	unsigned int code;
	int extra;

	if (state.ts.last_lit != 0)
		do
		{
			if ((lx & 7) == 0)
				flag = state.ts.flag_buf[fx++];
			lc = state.ts.l_buf[lx++];
			if ((flag & 1) == 0)
			{
				send_code(state, lc, ltree);
			}
			else
			{
				code = state.ts.length_code[lc];
				send_code(state, code + LITERALS + 1, ltree);
				extra = extra_lbits[code];
				if (extra != 0)
				{
					lc -= state.ts.base_length[code];
					send_bits(state, lc, extra);
				}
				dist = state.ts.d_buf[dx++];
				code = d_code(dist);
				Assert(state, code < D_CODES, "bad d_code");

				send_code(state, code, dtree);
				extra = extra_dbits[code];
				if (extra != 0)
				{
					dist -= state.ts.base_dist[code];
					send_bits(state, dist, extra);
				}
			}
			flag >>= 1;
		} while (lx < state.ts.last_lit);

	send_code(state, END_BLOCK, ltree);
}

void set_file_type(TState& state)
{
	int n = 0;
	unsigned int ascii_freq = 0;
	unsigned int bin_freq = 0;
	while (n < 7)
		bin_freq += state.ts.dyn_ltree[n++].fc.freq;
	while (n < 128)
		ascii_freq += state.ts.dyn_ltree[n++].fc.freq;
	while (n < LITERALS)
		bin_freq += state.ts.dyn_ltree[n++].fc.freq;
	*state.ts.file_type = (ush)(bin_freq > (ascii_freq >> 2) ? BINARY : ASCII);
}

void bi_init(TState& state, char* tgt_buf, unsigned int tgt_size,
	int flsh_allowed)
{
	state.bs.out_buf = tgt_buf;
	state.bs.out_size = tgt_size;
	state.bs.out_offset = 0;
	state.bs.flush_flg = flsh_allowed;

	state.bs.bi_buf = 0;
	state.bs.bi_valid = 0;
	state.bs.bits_sent = 0L;
}

void send_bits(TState& state, int value, int length)
{
	Assert(state, length > 0 && length <= 15, "invalid length");
	state.bs.bits_sent += (ulg)length;

	state.bs.bi_buf |= value << state.bs.bi_valid;
	state.bs.bi_valid += length;
	if (state.bs.bi_valid > (int)Buf_size)
	{
		put_short(state, state.bs.bi_buf);
		state.bs.bi_valid -= Buf_size;
		state.bs.bi_buf = (unsigned int)value >> (length - state.bs.bi_valid);
	}
}

unsigned int bi_reverse(unsigned int code, int len)
{
	register unsigned int res = 0;
	do
	{
		res |= code & 1;
		code >>= 1, res <<= 1;
	} while (--len > 0);
	return res >> 1;
}

void bi_windup(TState& state)
{
	if (state.bs.bi_valid > 8)
	{
		put_short(state, state.bs.bi_buf);
	}
	else if (state.bs.bi_valid > 0)
	{
		put_byte(state, (uch)state.bs.bi_buf);
	}
	if (state.bs.flush_flg)
	{
		state.flush_outbuf(state.param, state.bs.out_buf, &state.bs.out_offset);
	}
	state.bs.bi_buf = 0;
	state.bs.bi_valid = 0;
	state.bs.bits_sent = (state.bs.bits_sent + 7) & ~7;
}

void copy_block(TState& state, char* block, unsigned int len, int header)
{
	bi_windup(state);

	if (header)
	{
		put_short(state, (ush)len);
		put_short(state, (ush)~len);
		state.bs.bits_sent += 2 * 16;
	}
	if (state.bs.flush_flg)
	{
		state.flush_outbuf(state.param, state.bs.out_buf, &state.bs.out_offset);
		state.bs.out_offset = len;
		state.flush_outbuf(state.param, block, &state.bs.out_offset);
	}
	else if (state.bs.out_offset + len > state.bs.out_size)
	{
		state.err = "output buffer too small for in-memory compression";
	}
	else
	{
		memcpy(state.bs.out_buf + state.bs.out_offset, block, len);
		state.bs.out_offset += len;
	}
	state.bs.bits_sent += (ulg)len << 3;
}

void lm_init(TState& state, int pack_level, ush* flags)
{
	unsigned int j;

	Assert(state, pack_level >= 1 && pack_level <= 8, "bad pack level");

	state.ds.sliding = 0;
	if (state.ds.window_size == 0L)
	{
		state.ds.sliding = 1;
		state.ds.window_size = (ulg)2L * WSIZE;
	}

	state.ds.head[HASH_SIZE - 1] = NIL;
	memset((char*)state.ds.head, 0,
		(unsigned int)(HASH_SIZE - 1) * sizeof(*state.ds.head));

	state.ds.max_lazy_match = configuration_table[pack_level].max_lazy;
	state.ds.good_match = configuration_table[pack_level].good_length;
	state.ds.nice_match = configuration_table[pack_level].nice_length;
	state.ds.max_chain_length = configuration_table[pack_level].max_chain;
	if (pack_level <= 2)
	{
		*flags |= FAST;
	}
	else if (pack_level >= 8)
	{
		*flags |= SLOW;
	}

	state.ds.strstart = 0;
	state.ds.block_start = 0L;

	state.ds.lookahead =
		state.readfunc(state, (char*)state.ds.window, 2 * WSIZE);

	if (state.ds.lookahead == 0 || state.ds.lookahead == (unsigned int)-1)
	{
		state.ds.eofile = 1, state.ds.lookahead = 0;
		return;
	}
	state.ds.eofile = 0;
	if (state.ds.lookahead < MIN_LOOKAHEAD)
		fill_window(state);

	state.ds.ins_h = 0;
	for (j = 0; j < MIN_MATCH - 1; j++)
		UPDATE_HASH(state.ds.ins_h, state.ds.window[j]);
}

int longest_match(TState& state, IPos cur_match)
{
	unsigned int chain_length = state.ds.max_chain_length;
	register uch* scan = state.ds.window + state.ds.strstart;
	register uch* match;
	register int len;
	int best_len = state.ds.prev_length;
	IPos limit = state.ds.strstart > (IPos)MAX_DIST
		? state.ds.strstart - (IPos)MAX_DIST
		: NIL;

	register uch* strend = state.ds.window + state.ds.strstart + MAX_MATCH;
	register uch scan_end1 = scan[best_len - 1];
	register uch scan_end = scan[best_len];

	if (state.ds.prev_length >= state.ds.good_match)
	{
		chain_length >>= 2;
	}
	Assert(state, state.ds.strstart <= state.ds.window_size - MIN_LOOKAHEAD,
		"insufficient lookahead");

	do
	{
		Assert(state, cur_match < state.ds.strstart, "no future");
		match = state.ds.window + cur_match;

		if (match[best_len] != scan_end || match[best_len - 1] != scan_end1 ||
			*match != *scan || *++match != scan[1])
			continue;

		scan += 2, match++;

		do
		{
		} while (*++scan == *++match && *++scan == *++match &&
			*++scan == *++match && *++scan == *++match && *++scan == *++match &&
			*++scan == *++match && *++scan == *++match && *++scan == *++match &&
			scan < strend);

		Assert(state,
			scan <= state.ds.window + (unsigned int)(state.ds.window_size - 1),
			"wild scan");

		len = MAX_MATCH - (int)(strend - scan);
		scan = strend - MAX_MATCH;

		if (len > best_len)
		{
			state.ds.match_start = cur_match;
			best_len = len;
			if (len >= state.ds.nice_match)
				break;
			scan_end1 = scan[best_len - 1];
			scan_end = scan[best_len];
		}
	} while ((cur_match = state.ds.prev[cur_match & WMASK]) > limit &&
		--chain_length != 0);

	return best_len;
}

void fill_window(TState& state)
{
	unsigned int n, m;
	unsigned int more;

	do
	{
		more = (unsigned int)(state.ds.window_size - (ulg)state.ds.lookahead -
			(ulg)state.ds.strstart);

		if (more == (unsigned int)-1)
		{
			more--;
		}
		else if (state.ds.strstart >= WSIZE + MAX_DIST && state.ds.sliding)
		{
			memcpy((char*)state.ds.window, (char*)state.ds.window + WSIZE,
				(unsigned int)WSIZE);
			state.ds.match_start -= WSIZE;
			state.ds.strstart -= WSIZE;

			state.ds.block_start -= (long)WSIZE;

			for (n = 0; n < HASH_SIZE; n++)
			{
				m = state.ds.head[n];
				state.ds.head[n] = (m >= WSIZE ? m - WSIZE : NIL);
			}
			for (n = 0; n < WSIZE; n++)
			{
				m = state.ds.prev[n];
				state.ds.prev[n] = (m >= WSIZE ? m - WSIZE : NIL);
			}
			more += WSIZE;
		}
		if (state.ds.eofile)
			return;

		Assert(state, more >= 2, "more < 2");

		n = state.readfunc(state,
			(char*)state.ds.window + state.ds.strstart + state.ds.lookahead,
			more);
		if (n == 0 || n == (unsigned int)-1)
		{
			state.ds.eofile = 1;
		}
		else
		{
			state.ds.lookahead += n;
		}
	} while (state.ds.lookahead < MIN_LOOKAHEAD && !state.ds.eofile);
}

unsigned long deflate_fast(TState& state)
{
	IPos hash_head = NIL;
	int flush;
	unsigned int match_length = 0;

	state.ds.prev_length = MIN_MATCH - 1;
	while (state.ds.lookahead != 0)
	{
		if (state.ds.lookahead >= MIN_MATCH)
			INSERT_STRING(state, state.ds.strstart, hash_head);

		if (hash_head != NIL && state.ds.strstart - hash_head <= MAX_DIST)
		{
			if (state.ds.nice_match > state.ds.lookahead)
				state.ds.nice_match = state.ds.lookahead;
			match_length = longest_match(state, hash_head);
			if (match_length > state.ds.lookahead)
				match_length = state.ds.lookahead;
		}
		if (match_length >= MIN_MATCH)
		{
			flush = ct_tally(state, state.ds.strstart - state.ds.match_start,
				match_length - MIN_MATCH);

			state.ds.lookahead -= match_length;

			if (match_length <= state.ds.max_lazy_match &&
				state.ds.lookahead >= MIN_MATCH)
			{
				match_length--;
				do
				{
					state.ds.strstart++;
					INSERT_STRING(state, state.ds.strstart, hash_head);
				} while (--match_length != 0);
				state.ds.strstart++;
			}
			else
			{
				state.ds.strstart += match_length;
				match_length = 0;
				state.ds.ins_h = state.ds.window[state.ds.strstart];
				UPDATE_HASH(state.ds.ins_h,
					state.ds.window[state.ds.strstart + 1]);
			}
		}
		else
		{
			flush = ct_tally(state, 0, state.ds.window[state.ds.strstart]);
			state.ds.lookahead--;
			state.ds.strstart++;
		}
		if (flush)
			FLUSH_BLOCK(state, 0), state.ds.block_start = state.ds.strstart;

		if (state.ds.lookahead < MIN_LOOKAHEAD)
			fill_window(state);
	}
	return FLUSH_BLOCK(state, 1);
}

unsigned long deflate(TState& state)
{
	IPos hash_head = NIL;
	IPos prev_match;
	int flush;
	int match_available = 0;
	register unsigned int match_length = MIN_MATCH - 1;

	if (state.level <= 3)
		return deflate_fast(state);

	while (state.ds.lookahead != 0)
	{
		if (state.ds.lookahead >= MIN_MATCH)
			INSERT_STRING(state, state.ds.strstart, hash_head);

		state.ds.prev_length = match_length, prev_match = state.ds.match_start;
		match_length = MIN_MATCH - 1;

		if (hash_head != NIL &&
			state.ds.prev_length < state.ds.max_lazy_match &&
			state.ds.strstart - hash_head <= MAX_DIST)
		{
			if (state.ds.nice_match > state.ds.lookahead)
				state.ds.nice_match = state.ds.lookahead;
			match_length = longest_match(state, hash_head);
			if (match_length > state.ds.lookahead)
				match_length = state.ds.lookahead;

			if (match_length == MIN_MATCH &&
				state.ds.strstart - state.ds.match_start > TOO_FAR)
			{
				match_length--;
			}
		}
		if (state.ds.prev_length >= MIN_MATCH &&
			match_length <= state.ds.prev_length)
		{
			unsigned int max_insert =
				state.ds.strstart + state.ds.lookahead - MIN_MATCH;

			flush = ct_tally(state, state.ds.strstart - 1 - prev_match,
				state.ds.prev_length - MIN_MATCH);

			state.ds.lookahead -= state.ds.prev_length - 1;
			state.ds.prev_length -= 2;
			do
			{
				state.ds.strstart++;
				if (state.ds.strstart <= max_insert)
					INSERT_STRING(state, state.ds.strstart, hash_head);
			} while (--state.ds.prev_length != 0);
			match_available = 0;
			match_length = MIN_MATCH - 1;
			state.ds.strstart++;
			if (flush)
				FLUSH_BLOCK(state, 0), state.ds.block_start = state.ds.strstart;
		}
		else if (match_available)
		{
			if (ct_tally(state, 0, state.ds.window[state.ds.strstart - 1]))
			{
				FLUSH_BLOCK(state, 0), state.ds.block_start = state.ds.strstart;
			}
			state.ds.strstart++;
			state.ds.lookahead--;
		}
		else
		{
			match_available = 1;
			state.ds.strstart++;
			state.ds.lookahead--;
		}

		if (state.ds.lookahead < MIN_LOOKAHEAD)
			fill_window(state);
	}
	if (match_available)
		ct_tally(state, 0, state.ds.window[state.ds.strstart - 1]);

	return FLUSH_BLOCK(state, 1);
}

int putlocal(zlist* z, WRITEFUNC wfunc, void* param)
{
	PUTLG(LOCSIG, f);
	PUTSH(z->ver, f);
	PUTSH(z->lflg, f);
	PUTSH(z->how, f);
	PUTLG(z->tim, f);
	PUTLG(z->crc, f);
	PUTLG(z->siz, f);
	PUTLG(z->len, f);
	PUTSH(z->nam, f);
	PUTSH(z->ext, f);
	if (wfunc(param, z->iname, (unsigned int)z->nam) != z->nam)
		return ZE_TEMP;
	if (z->ext != 0 && wfunc(param, z->extra, (unsigned int)z->ext) != z->ext)
		return ZE_TEMP;
	return ZE_OK;
}

int putextended(zlist* z, WRITEFUNC wfunc, void* param)
{
	PUTLG(EXTLOCSIG, f);
	PUTLG(z->crc, f);
	PUTLG(z->siz, f);
	PUTLG(z->len, f);
	return ZE_OK;
}

int putcentral(zlist* z, WRITEFUNC wfunc, void* param)
{
	PUTLG(CENSIG, f);
	PUTSH(z->vem, f);
	PUTSH(z->ver, f);
	PUTSH(z->flg, f);
	PUTSH(z->how, f);
	PUTLG(z->tim, f);
	PUTLG(z->crc, f);
	PUTLG(z->siz, f);
	PUTLG(z->len, f);
	PUTSH(z->nam, f);
	PUTSH(z->cext, f);
	PUTSH(z->com, f);
	PUTSH(z->dsk, f);
	PUTSH(z->att, f);
	PUTLG(z->atx, f);
	PUTLG(z->off, f);
	if (wfunc(param, z->iname, (unsigned int)z->nam) != z->nam ||
		(z->cext &&
			wfunc(param, z->cextra, (unsigned int)z->cext) != z->cext) ||
		(z->com && wfunc(param, z->comment, (unsigned int)z->com) != z->com))
		return ZE_TEMP;
	return ZE_OK;
}

int putend(int n, ulg s, ulg c, unsigned int m, char* z, WRITEFUNC wfunc,
	void* param)
{
	PUTLG(ENDSIG, f);
	PUTSH(0, f);
	PUTSH(0, f);
	PUTSH(n, f);
	PUTSH(n, f);
	PUTLG(s, f);
	PUTLG(c, f);
	PUTSH(m, f);
	if (m && wfunc(param, z, (unsigned int)m) != m)
		return ZE_TEMP;
	return ZE_OK;
}

unsigned long crc32(unsigned long crc, const uch* buf, unsigned int len)
{
	if (buf == 0)
		return 0L;
	crc = crc ^ 0xffffffffL;
	while (len >= 8)
	{
		DO8(buf);
		len -= 8;
	}
	if (len)
		do
		{
			DO1(buf);
		} while (--len);
	return crc ^ 0xffffffffL;
}

bool HasZipSuffix(const char* fn)
{
	const char* ext = fn + strlen(fn);
	while (ext > fn && *ext != '.')
		ext--;
	if (ext == fn && *ext != '.')
		return false;
	if (stricmp(ext, ".Z") == 0)
		return true;
	if (stricmp(ext, ".zip") == 0)
		return true;
	if (stricmp(ext, ".zoo") == 0)
		return true;
	if (stricmp(ext, ".arc") == 0)
		return true;
	if (stricmp(ext, ".lzh") == 0)
		return true;
	if (stricmp(ext, ".arj") == 0)
		return true;
	if (stricmp(ext, ".gz") == 0)
		return true;
	if (stricmp(ext, ".tgz") == 0)
		return true;
	return false;
}

long filetime2timet(FILETIME ft)
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&ft, &st);
	if (st.wYear < 1970)
	{
		st.wYear = 1970;
		st.wMonth = 1;
		st.wDay = 1;
	}
	else if (st.wYear >= 2038)
	{
		st.wYear = 2037;
		st.wMonth = 12;
		st.wDay = 31;
	}
	struct tm tm;
	tm.tm_sec = st.wSecond;
	tm.tm_min = st.wMinute;
	tm.tm_hour = st.wHour;
	tm.tm_mday = st.wDay;
	tm.tm_mon = st.wMonth - 1;
	tm.tm_year = st.wYear - 1900;
	tm.tm_isdst = 0;
	return (long)mktime(&tm);
}

unsigned long GetFileInfo(void* hf, unsigned long* attr, long* size,
	iztimes* times, unsigned long* timestamp)
{
	DWORD type = GetFileType(hf);
	if (type != FILE_TYPE_DISK)
		return ZR_NOTINITED;
	BY_HANDLE_FILE_INFORMATION bhi;
	BOOL res = GetFileInformationByHandle(hf, &bhi);
	if (!res)
		return ZR_NOFILE;
	DWORD fa = bhi.dwFileAttributes;
	ulg a = 0;
	if (fa & FILE_ATTRIBUTE_READONLY)
		a |= 0x01;
	if (fa & FILE_ATTRIBUTE_HIDDEN)
		a |= 0x02;
	if (fa & FILE_ATTRIBUTE_SYSTEM)
		a |= 0x04;
	if (fa & FILE_ATTRIBUTE_DIRECTORY)
		a |= 0x10;
	if (fa & FILE_ATTRIBUTE_ARCHIVE)
		a |= 0x20;
	if (fa & FILE_ATTRIBUTE_DIRECTORY)
		a |= 0x40000000;
	else
		a |= 0x80000000;
	a |= 0x01000000;
	if (!(fa & FILE_ATTRIBUTE_READONLY))
		a |= 0x00800000;
	DWORD fsize = GetFileSize(hf, NULL);
	if (fsize > 40)
	{
		SetFilePointer(hf, 0, NULL, FILE_BEGIN);
		WORD magic;
		DWORD red;
		ReadFile(hf, &magic, 2, &red, NULL);
		SetFilePointer(hf, 0x24, NULL, FILE_BEGIN);
		DWORD off;
		ReadFile(hf, &off, 4, &red, NULL);
		if (magic == 0x54AD && fsize > off + 0x34)
		{
			SetFilePointer(hf, off, NULL, FILE_BEGIN);
			DWORD sig;
			ReadFile(hf, &sig, 4, &red, NULL);
			if (sig == 0x5a4d || sig == 0x454e || sig == 0x454c ||
				sig == 0x4550)
				a |= 0x00400000;
		}
	}
	if (attr != NULL)
		*attr = a;
	if (size != NULL)
		*size = fsize;
	if (times != NULL)
	{
		times->atime = filetime2timet(bhi.ftLastAccessTime);
		times->mtime = filetime2timet(bhi.ftLastWriteTime);
		times->ctime = filetime2timet(bhi.ftCreationTime);
	}
	if (timestamp != NULL)
	{
		WORD dosdate, dostime;
		FileTimeToDosDateTime(&bhi.ftLastWriteTime, &dosdate, &dostime);
		*timestamp = (WORD)dostime | (((DWORD)dosdate) << 16);
	}
	return ZR_OK;
}

TZip::TZip(void)
	: hfout(0),
	  hmapout(0),
	  ooffset(0),
	  oerr(ZR_OK),
	  writ(0),
	  obuf(0),
	  hasputcen(false),
	  zfis(0),
	  hfin(0)
{
}

TZip::~TZip(void)
{
}

ZRESULT TZip::Create(void* z, unsigned int len, unsigned long flags,
	unsigned long attributes)
{
	if (hfout != 0 || hmapout != 0 || obuf != 0 || writ != 0 || oerr != ZR_OK ||
		hasputcen)
		return ZR_NOTINITED;

	if (flags == ZIP_HANDLE)
	{
		HANDLE hf = (HANDLE)z;
		BOOL res = DuplicateHandle(GetCurrentProcess(), hf, GetCurrentProcess(),
			&hfout, 0, FALSE, DUPLICATE_SAME_ACCESS);
		if (!res)
			return ZR_NODUPH;
		DWORD type = GetFileType(hfout);
		ocanseek = (type == FILE_TYPE_DISK);
		if (type == FILE_TYPE_DISK)
			ooffset = SetFilePointer(hfout, 0, NULL, FILE_CURRENT);
		else
			ooffset = 0;
		return ZR_OK;
	}
	else if (flags == ZIP_FILENAME)
	{
		const char* fn = (const char*)z;
		DWORD attr = attributes;
		if (attr == 0)
			attr = FILE_ATTRIBUTE_NORMAL;
		hfout =
			CreateFile(fn, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, attr, NULL);
		if (hfout == INVALID_HANDLE_VALUE)
		{
			hfout = 0;
			return ZR_NOFILE;
		}
		ocanseek = true;
		ooffset = 0;
		return ZR_OK;
	}
	else if (flags == ZIP_MEMORY)
	{
		unsigned int size = len;
		if (size == 0)
			return ZR_MEMSIZE;
		if (z != 0)
			obuf = (char*)z;
		else
		{
			hmapout = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
				PAGE_READWRITE, 0, size, NULL);
			if (hmapout == NULL)
				return ZR_NOALLOC;
			obuf =
				(char*)MapViewOfFile(hmapout, FILE_MAP_ALL_ACCESS, 0, 0, size);
			if (obuf == 0)
			{
				CloseHandle(hmapout);
				hmapout = 0;
				return ZR_NOALLOC;
			}
		}
		mapsize = size;
		opos = 0;
		ocanseek = true;
		return ZR_OK;
	}
	else
		return ZR_ARGS;
}

unsigned int TZip::sflush(void* param, const char* buf, unsigned int* size)
{
	if (*size == 0)
		return 0;
	TZip* zip = (TZip*)param;
	unsigned int writ = zip->write(buf, *size);
	if (writ != 0)
		*size = 0;
	return writ;
}

unsigned int TZip::swrite(void* param, const char* buf, unsigned int size)
{
	if (size == 0)
		return 0;
	TZip* zip = (TZip*)param;
	return zip->write(buf, size);
}

unsigned int TZip::write(const char* buf, unsigned int size)
{
	const char* srcbuf = buf;
	if (obuf != 0)
	{
		if (opos + size >= mapsize)
		{
			oerr = ZR_MEMSIZE;
			return 0;
		}
		memcpy(obuf + opos, srcbuf, size);
		opos += size;
		return size;
	}
	else if (hfout != 0)
	{
		DWORD writ;
		WriteFile(hfout, (const void*)srcbuf, size, &writ, NULL);
		return writ;
	}
	oerr = ZR_NOTINITED;
	return 0;
}

bool TZip::oseek(unsigned int pos)
{
	if (!ocanseek)
	{
		oerr = ZR_SEEK;
		return false;
	}
	if (obuf != 0)
	{
		if (pos >= mapsize)
		{
			oerr = ZR_MEMSIZE;
			return false;
		}
		opos = pos;
		return true;
	}
	else if (hfout != 0)
	{
		SetFilePointer(hfout, pos + ooffset, NULL, FILE_BEGIN);
		return true;
	}
	oerr = ZR_NOTINITED;
	return false;
}

ZRESULT TZip::GetMemory(void** pbuf, unsigned long* plen)
{
	if (!hasputcen)
		AddCentral();
	hasputcen = true;
	if (pbuf != 0)
		*pbuf = (void*)obuf;
	if (plen != 0)
		*plen = writ;
	if (obuf == 0)
		return ZR_NOTMMAP;
	return ZR_OK;
}

ZRESULT TZip::Close(void)
{
	ZRESULT res = ZR_OK;
	if (!hasputcen)
		res = AddCentral();
	hasputcen = true;
	if (obuf != 0 && hmapout != 0)
		UnmapViewOfFile(obuf);
	obuf = 0;
	if (hmapout != 0)
		CloseHandle(hmapout);
	hmapout = 0;
	if (hfout != 0)
		CloseHandle(hfout);
	hfout = 0;
	return res;
}

ZRESULT TZip::open_file(const char* fn)
{
	hfin = 0;
	bufin = 0;
	selfclosehf = false;
	crc = CRCVAL_INITIAL;
	isize = 0;
	csize = 0;
	ired = 0;
	if (fn == 0)
		return ZR_ARGS;
	HANDLE hf = CreateFile(fn, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if (hf == INVALID_HANDLE_VALUE)
		return ZR_NOFILE;
	ZRESULT res = open_handle(hf, 0);
	if (res != ZR_OK)
	{
		CloseHandle(hf);
		return res;
	}
	selfclosehf = true;
	return ZR_OK;
}

ZRESULT TZip::open_handle(void* hf, unsigned int len)
{
	hfin = 0;
	bufin = 0;
	selfclosehf = false;
	crc = CRCVAL_INITIAL;
	isize = 0;
	csize = 0;
	ired = 0;
	if (hf == 0 || hf == INVALID_HANDLE_VALUE)
		return ZR_ARGS;
	DWORD type = GetFileType(hf);
	if (type == FILE_TYPE_DISK)
	{
		ZRESULT res = GetFileInfo(hf, &attr, &isize, &times, &timestamp);
		if (res != ZR_OK)
			return res;
		SetFilePointer(hf, 0, NULL, FILE_BEGIN);
		hfin = hf;
		iseekable = true;
		return ZR_OK;
	}
	else
	{
		attr = 0x80000000;
		isize = -1;
		if (len != 0)
			isize = len;
		iseekable = false;
		SYSTEMTIME st;
		GetLocalTime(&st);
		FILETIME ft;
		SystemTimeToFileTime(&st, &ft);
		WORD dosdate, dostime;
		FileTimeToDosDateTime(&ft, &dosdate, &dostime);
		times.atime = filetime2timet(ft);
		times.mtime = times.atime;
		times.ctime = times.atime;
		timestamp = (WORD)dostime | (((DWORD)dosdate) << 16);
		hfin = hf;
		return ZR_OK;
	}
}

ZRESULT TZip::open_mem(void* src, unsigned int len)
{
	hfin = 0;
	bufin = (const char*)src;
	selfclosehf = false;
	crc = CRCVAL_INITIAL;
	csize = 0;
	ired = 0;
	lenin = len;
	posin = 0;
	if (src == 0 || len == 0)
		return ZR_ARGS;
	attr = 0x80000000;
	isize = len;
	iseekable = true;
	SYSTEMTIME st;
	GetLocalTime(&st);
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	WORD dosdate, dostime;
	FileTimeToDosDateTime(&ft, &dosdate, &dostime);
	times.atime = filetime2timet(ft);
	times.mtime = times.atime;
	times.ctime = times.atime;
	timestamp = (WORD)dostime | (((DWORD)dosdate) << 16);
	return ZR_OK;
}

ZRESULT TZip::open_dir(void)
{
	hfin = 0;
	bufin = 0;
	selfclosehf = false;
	crc = CRCVAL_INITIAL;
	csize = 0;
	ired = 0;
	isize = 0;
	iseekable = false;
	attr = 0x41C00010;
	SYSTEMTIME st;
	GetLocalTime(&st);
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	WORD dosdate, dostime;
	FileTimeToDosDateTime(&ft, &dosdate, &dostime);
	times.atime = filetime2timet(ft);
	times.mtime = times.atime;
	times.ctime = times.atime;
	timestamp = (WORD)dostime | (((DWORD)dosdate) << 16);
	return ZR_OK;
}

unsigned int TZip::sread(TState& s, char* buf, unsigned int size)
{
	TZip* zip = (TZip*)s.param;
	return zip->read(buf, size);
}

unsigned int TZip::read(char* buf, unsigned int size)
{
	if (bufin != 0)
	{
		if (posin >= lenin)
			return 0;
		ulg red = lenin - posin;
		if (red > size)
			red = size;
		memcpy(buf, bufin + posin, red);
		posin += red;
		ired += red;
		crc = crc32(crc, (uch*)buf, red);
		return red;
	}
	else if (hfin != 0)
	{
		DWORD red;
		BOOL ok = ReadFile(hfin, buf, size, &red, NULL);
		if (!ok)
			return 0;
		ired += red;
		crc = crc32(crc, (uch*)buf, red);
		return red;
	}
	oerr = ZR_NOTINITED;
	return 0;
}

ZRESULT TZip::iclose(void)
{
	if (selfclosehf && hfin != 0)
		CloseHandle(hfin);
	hfin = 0;
	bool mismatch = (isize != -1 && isize != ired);
	isize = ired;
	if (mismatch)
		return ZR_MISSIZE;
	else
		return ZR_OK;
}

ZRESULT TZip::ideflate(zlist* zfi)
{
	TState state;
	state.readfunc = sread;
	state.flush_outbuf = sflush;
	state.param = this;
	state.level = 8;
	state.seekable = iseekable;
	state.err = NULL;
	state.ts.static_dtree[0].dl.len = 0;
	bi_init(state, buf, sizeof(buf), 1);
	ct_init(state, &zfi->att);
	lm_init(state, state.level, &zfi->flg);
	ulg sz = deflate(state);
	csize = sz;
	if (state.err != NULL)
		return ZR_FLATE;
	return ZR_OK;
}

ZRESULT TZip::istore(void)
{
	ulg size = 0;
	for (;;)
	{
		unsigned int cin = read(buf, 16384);
		if (cin <= 0 || cin == (unsigned int)-1)
			break;
		unsigned int cout = write(buf, cin);
		if (cout != cin)
			return ZR_MISSIZE;
		size += cin;
	}
	csize = size;
	return ZR_OK;
}

ZRESULT TZip::Add(const char* odstzn, void* src, unsigned int len,
	unsigned long flags)
{
	if (oerr != ZR_OK)
		return ZR_FAILED;
	if (hasputcen)
		return ZR_ENDED;

	char dstzn[MAX_PATH_Z];
	strcpy(dstzn, odstzn);
	if (*dstzn == 0)
		return ZR_ARGS;
	char* d = dstzn;
	while (*d != 0)
	{
		if (*d == '\\')
			*d = '/';
		d++;
	}
	bool isdir = (flags == ZIP_FOLDER);
	bool needs_trailing_slash = (isdir && dstzn[strlen(dstzn) - 1] != '/');
	int method = DEFLATE;
	if (isdir || HasZipSuffix(dstzn))
		method = STORE;

	ZRESULT openres;
	if (flags == ZIP_FILENAME)
		openres = open_file((const char*)src);
	else if (flags == ZIP_HANDLE)
		openres = open_handle(src, len);
	else if (flags == ZIP_MEMORY)
		openres = open_mem(src, len);
	else if (flags == ZIP_FOLDER)
		openres = open_dir();
	else
		return ZR_ARGS;
	if (openres != ZR_OK)
		return openres;

	zlist zfi;
	zfi.nxt = NULL;
	zfi.name[0] = 0;
	strcpy(zfi.iname, dstzn);
	zfi.nam = strlen(zfi.iname);
	if (needs_trailing_slash)
	{
		strcat(zfi.iname, "/");
		zfi.nam++;
	}
	zfi.flg = 8;
	zfi.lflg = 8;
	zfi.zname[0] = 0;
	zfi.comment = 0;
	zfi.com = 0;
	zfi.mark = 1;
	zfi.dosflag = 0;
	zfi.att = 0;
	zfi.vem = 0xB17;
	zfi.ver = 20;
	zfi.tim = timestamp;
	zfi.crc = 0;
	zfi.how = (ush)method;
	zfi.siz = method == STORE && isize >= 0 ? isize : 0;
	zfi.len = isize;
	zfi.dsk = 0;
	zfi.atx = attr;
	zfi.off = writ + ooffset;

	char xloc[EB_L_UT_SIZE];
	zfi.extra = xloc;
	char xcen[EB_C_UT_SIZE];
	zfi.cextra = xcen;
	zfi.ext = EB_L_UT_SIZE;
	zfi.cext = EB_C_UT_SIZE;
	xloc[0] = 'U';
	xloc[1] = 'T';
	xloc[2] = 13;
	xloc[3] = 0;
	xloc[4] = 7;
	xloc[5] = (char)(times.mtime);
	xloc[6] = (char)(times.mtime >> 8);
	xloc[7] = (char)(times.mtime >> 16);
	xloc[8] = (char)(times.mtime >> 24);
	xloc[9] = (char)(times.atime);
	xloc[10] = (char)(times.atime >> 8);
	xloc[11] = (char)(times.atime >> 16);
	xloc[12] = (char)(times.atime >> 24);
	xloc[13] = (char)(times.ctime);
	xloc[14] = (char)(times.ctime >> 8);
	xloc[15] = (char)(times.ctime >> 16);
	xloc[16] = (char)(times.ctime >> 24);
	memcpy(xcen, xloc, EB_C_UT_SIZE);
	xcen[2] = 5;

	int r = putlocal(&zfi, swrite, this);
	if (r != ZE_OK)
	{
		iclose();
		return ZR_WRITE;
	}
	writ += 4 + LOCHEAD + (unsigned int)zfi.nam + (unsigned int)zfi.ext;
	if (oerr != ZR_OK)
	{
		iclose();
		return oerr;
	}

	ZRESULT writeres = ZR_OK;
	if (!isdir)
	{
		if (method == DEFLATE)
			writeres = ideflate(&zfi);
		else if (method == STORE)
			writeres = istore();
	}
	else
		csize = 0;
	iclose();
	writ += csize;
	if (oerr != ZR_OK)
		return oerr;
	if (writeres != ZR_OK)
		return ZR_WRITE;

	bool sizok = zfi.siz == csize;
	zfi.crc = crc;
	zfi.siz = csize;
	zfi.len = isize;
	if (ocanseek)
	{
		zfi.how = (ush)method;
		if (!(zfi.flg & 1))
			zfi.flg &= ~8;
		zfi.lflg = zfi.flg;
		if (!oseek(zfi.off - ooffset))
			return ZR_SEEK;
		r = putlocal(&zfi, swrite, this);
		if (r != ZE_OK)
		{
			return ZR_WRITE;
		}
		if (!oseek(writ))
			return ZR_SEEK;
	}
	else
	{
		if (zfi.how != (ush)method)
			return ZR_NOCHANGE;
		if (method == STORE && !sizok)
			return ZR_NOCHANGE;
		r = putextended(&zfi, swrite, this);
		if (r != ZE_OK)
			return ZR_WRITE;
		writ += 16L;
		zfi.flg = zfi.lflg;
	}

	if (oerr != ZR_OK)
		return oerr;
	char* extra = new char[zfi.cext];
	memcpy(extra, zfi.cextra, zfi.cext);
	zfi.cextra = extra;
	zlist* z = new zlist;
	memcpy(z, &zfi, sizeof(zlist));
	if (zfis == NULL)
	{
		zfis = z;
	}
	else
	{
		zlist* zz = zfis;
		while (zz->nxt != NULL)
			zz = zz->nxt;
		zz->nxt = z;
	}
	return ZR_OK;
}

ZRESULT TZip::AddCentral(void)
{
	unsigned int ocount = 0;
	unsigned int pos_at_start_of_central = writ;
	bool okay = true;
	for (zlist* zfi = zfis; zfi != NULL;)
	{
		if (okay)
		{
			int res = putcentral(zfi, swrite, this);
			if (res != ZE_OK)
				okay = false;
		}
		ocount++;
		writ += 46 + (unsigned int)zfi->nam + (unsigned int)zfi->cext +
			(unsigned int)zfi->com;
		zlist* zfinext = zfi->nxt;
		if (zfi->cextra != 0)
			delete[] zfi->cextra;
		delete zfi;
		zfi = zfinext;
	}
	unsigned int size_of_central = writ - pos_at_start_of_central;
	if (okay)
	{
		int res = putend(ocount, size_of_central,
			pos_at_start_of_central + ooffset, 0, NULL, swrite, this);
		if (res != ZE_OK)
			okay = false;
		writ += 22;
	}
	if (!okay)
		return ZR_WRITE;
	return ZR_OK;
}

unsigned int FormatZipMessageZ(ZRESULT code, char* buf, unsigned int len)
{
	if (code == ZR_RECENT)
		code = lasterrorZ;
	const char* msg = "unknown zip result code";
	switch (code)
	{
	case ZR_OK:
		msg = "Success";
		break;
	case ZR_NODUPH:
		msg = "Culdn't duplicate handle";
		break;
	case ZR_NOFILE:
		msg = "Couldn't create/open file";
		break;
	case ZR_NOALLOC:
		msg = "Failed to allocate memory";
		break;
	case ZR_WRITE:
		msg = "Error writing to file";
		break;
	case ZR_NOTFOUND:
		msg = "File not found in the zipfile";
		break;
	case ZR_MORE:
		msg = "Still more data to unzip";
		break;
	case ZR_CORRUPT:
		msg = "Zipfile is corrupt or not a zipfile";
		break;
	case ZR_READ:
		msg = "Error reading file";
		break;
	case ZR_ARGS:
		msg = "Caller: faulty arguments";
		break;
	case ZR_PARTIALUNZ:
		msg = "Caller: the file had already been partially unzipped";
		break;
	case ZR_NOTMMAP:
		msg = "Caller: can only get memory of a memory zipfile";
		break;
	case ZR_MEMSIZE:
		msg = "Caller: not enough space allocated for memory zipfile";
		break;
	case ZR_FAILED:
		msg = "Caller: there was a previous error";
		break;
	case ZR_ENDED:
		msg = "Caller: additions to the zip have already been ended";
		break;
	case ZR_MISSIZE:
		msg = "Zip-bug: the anticipated size turned out wrong";
		break;
	case ZR_ZMODE:
		msg = "Caller: mixing creation and opening of zip";
		break;
	case ZR_NOTINITED:
		msg = "Zip-bug: internal initialisation not completed";
		break;
	case ZR_SEEK:
		msg = "Zip-bug: trying to seek the unseekable";
		break;
	case ZR_NOCHANGE:
		msg = "Zip-bug: tried to change mind, but not allowed";
		break;
	case ZR_FLATE:
		msg = "Zip-bug: an internal error during flation";
		break;
	}
	unsigned int mlen = (unsigned int)strlen(msg);
	if (buf == 0 || len == 0)
		return mlen;
	unsigned int n = mlen;
	if (n + 1 > len)
		n = len - 1;
	strncpy(buf, msg, n);
	buf[n] = 0;
	return mlen;
}

HZIP__* CreateZipZ(void* z, unsigned int len, unsigned long flags,
	unsigned long attributes)
{
	tzset();
	TZip* zip = new TZip();
	lasterrorZ = zip->Create(z, len, flags, attributes);
	if (lasterrorZ != ZR_OK)
	{
		delete zip;
		return 0;
	}
	TZipHandleData* han = new TZipHandleData;
	han->flag = 2;
	han->zip = zip;
	return (HZIP__*)han;
}

ZRESULT ZipAdd(HZIP__* hz, const char* dstzn, void* src, unsigned int len,
	unsigned long flags)
{
	ZRESULT res;
	char dstz[MAX_PATH_Z * 2];
	TZipHandleData* han = (TZipHandleData*)hz;
	if (hz == 0)
		res = ZR_ARGS;
	else if (han->flag != 2)
		res = ZR_ZMODE;
	else
	{
		TZip* zip = han->zip;
		if (flags == ZIP_FILENAME)
		{
			memset(dstz, 0, sizeof(dstz));
			strcpy(dstz, dstzn);
			res = zip->Add(dstz, src, len, flags);
		}
		else
			res = zip->Add(dstzn, src, len, flags);
	}
	lasterrorZ = res;
	return res;
}

ZRESULT ZipGetMemory(HZIP__* hz, void** buf, unsigned long* len)
{
	if (hz == 0)
	{
		if (buf != 0)
			*buf = 0;
		if (len != 0)
			*len = 0;
		lasterrorZ = ZR_ARGS;
		return ZR_ARGS;
	}
	TZipHandleData* han = (TZipHandleData*)hz;
	if (han->flag != 2)
	{
		lasterrorZ = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TZip* zip = han->zip;
	ZRESULT res = zip->GetMemory(buf, len);
	lasterrorZ = res;
	return res;
}

ZRESULT CloseZipZ(HZIP__* hz)
{
	if (hz == 0)
	{
		lasterrorZ = ZR_ARGS;
		return ZR_ARGS;
	}
	TZipHandleData* han = (TZipHandleData*)hz;
	if (han->flag != 2)
	{
		lasterrorZ = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TZip* zip = han->zip;
	ZRESULT res = zip->Close();
	lasterrorZ = res;
	delete zip;
	delete han;
	return lasterrorZ;
}

bool IsZipHandleZ(HZIP__* hz)
{
	if (hz == 0)
		return true;
	TZipHandleData* han = (TZipHandleData*)hz;
	return (han->flag == 2);
}
