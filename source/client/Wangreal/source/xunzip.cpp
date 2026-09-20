// XUnzip 1.3 by Lucian Wischik, with zlib 1.1.3 and minizip 0.15.
#define WIN32_LEAN_AND_MEAN
#include "xunzip.h"
#include <new>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define _T(x) x
#define _tcscpy strcpy
#define _tcscat strcat
#define _tcslen strlen
#define _tcsncpy(dst, src, n) \
	_mbsnbcpy((unsigned char*)(dst), (const unsigned char*)(src), (n))

extern "C"
{
	__declspec(dllimport) unsigned char* __cdecl _mbsstr(
		const unsigned char* s1, const unsigned char* s2);
	__declspec(dllimport) unsigned char* __cdecl _mbsnbcpy(unsigned char* dst,
		const unsigned char* src, unsigned int n);
	char* _tcsstr(char* s1, const char* s2);
}

typedef unsigned char Byte;
typedef unsigned int uInt;
typedef unsigned long uLong;
typedef Byte Bytef;
typedef char charf;
typedef int intf;
typedef uInt uIntf;
typedef uLong uLongf;
typedef void* voidpf;
typedef void* voidp;

#define Z_NULL 0

#define Z_OK 0
#define Z_STREAM_END 1
#define Z_NEED_DICT 2
#define Z_ERRNO (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR (-3)
#define Z_MEM_ERROR (-4)
#define Z_BUF_ERROR (-5)
#define Z_VERSION_ERROR (-6)

#define Z_NO_FLUSH 0
#define Z_PARTIAL_FLUSH 1
#define Z_SYNC_FLUSH 2
#define Z_FULL_FLUSH 3
#define Z_FINISH 4

#define MAX_WBITS 15
#define Z_DEFLATED 8
#define PRESET_DICT 0x20
#define UNZ_BUFSIZE (16384)
#define ZLIB_VERSION "1.1.3"
#define MANY 1440

struct internal_state;

typedef struct z_stream_s
{
	Bytef* next_in;
	uInt avail_in;
	uLong total_in;
	Bytef* next_out;
	uInt avail_out;
	uLong total_out;
	char* msg;
	struct internal_state* state;
	voidpf (*zalloc)(voidpf opaque, uInt items, uInt size);
	void (*zfree)(voidpf opaque, voidpf address);
	voidpf opaque;
	int data_type;
	uLong adler;
	uLong reserved;
} z_stream;

typedef z_stream* z_streamp;

typedef voidpf (*alloc_func)(voidpf opaque, uInt items, uInt size);
typedef void (*free_func)(voidpf opaque, voidpf address);

#define ZALLOC(strm, items, size) \
	(*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr) (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))

typedef struct inflate_huft_s inflate_huft;
struct inflate_huft_s
{
	union
	{
		struct
		{
			Byte Exop;
			Byte Bits;
		} what;
		Bytef* pad;
	} word;
	uInt base;
};

#define exop word.what.Exop
#define bits word.what.Bits

typedef uLong (*check_func)(uLong check, const Bytef* buf, uInt len);

struct inflate_codes_state;
typedef struct inflate_codes_state inflate_codes_statef;

enum inflate_block_mode
{
	IBM_TYPE,
	IBM_LENS,
	IBM_STORED,
	IBM_TABLE,
	IBM_BTREE,
	IBM_DTREE,
	IBM_CODES,
	IBM_DRY,
	IBM_DONE,
	IBM_BAD
};

struct inflate_blocks_state
{
	inflate_block_mode mode;
	union
	{
		uInt left;
		struct
		{
			uInt table;
			uInt index;
			uInt* blens;
			uInt bb;
			inflate_huft* tb;
		} trees;
		struct
		{
			inflate_codes_statef* codes;
		} decode;
	} sub;
	uInt last;
	uInt bitk;
	uLong bitb;
	inflate_huft* hufts;
	Bytef* window;
	Bytef* end;
	Bytef* read;
	Bytef* write;
	check_func checkfn;
	uLong check;
};
typedef struct inflate_blocks_state inflate_blocks_statef;

enum inflate_codes_mode
{
	ICM_START,
	ICM_LEN,
	ICM_LENEXT,
	ICM_DIST,
	ICM_DISTEXT,
	ICM_COPY,
	ICM_LIT,
	ICM_WASH,
	ICM_END,
	ICM_BADCODE
};

struct inflate_codes_state
{
	inflate_codes_mode mode;
	uInt len;
	union
	{
		struct
		{
			inflate_huft* tree;
			uInt need;
		} code;
		uInt lit;
		struct
		{
			uInt get;
			uInt dist;
		} copy;
	} sub;
	Byte lbits;
	Byte dbits;
	inflate_huft* ltree;
	inflate_huft* dtree;
};

enum inflate_mode
{
	IM_METHOD,
	IM_FLAG,
	IM_DICT4,
	IM_DICT3,
	IM_DICT2,
	IM_DICT1,
	IM_DICT0,
	IM_BLOCKS,
	IM_CHECK4,
	IM_CHECK3,
	IM_CHECK2,
	IM_CHECK1,
	IM_DONE,
	IM_BAD
};

struct internal_state
{
	inflate_mode mode;
	union
	{
		uInt method;
		struct
		{
			uLong was;
			uLong need;
		} check;
		uInt marker;
	} sub;
	int nowrap;
	uInt wbits;
	inflate_blocks_statef* blocks;
};

extern const char* const z_errmsg[10];
#define ERR_MSG(err) z_errmsg[Z_NEED_DICT - (err)]

int inflate_flush(inflate_blocks_statef*, z_streamp, int);
inflate_codes_statef* inflate_codes_new(uInt, uInt, const inflate_huft*,
	const inflate_huft*, z_streamp);
int inflate_codes(inflate_blocks_statef*, z_streamp, int);
void inflate_codes_free(inflate_codes_statef*, z_streamp);
void inflate_blocks_reset(inflate_blocks_statef*, z_streamp, uLong*);
inflate_blocks_statef* inflate_blocks_new(z_streamp, check_func, uInt);
int inflate_blocks(inflate_blocks_statef*, z_streamp, int);
int inflate_blocks_free(inflate_blocks_statef*, z_streamp);
int huft_build(uIntf*, uInt, uInt, const uIntf*, const uIntf*,
	inflate_huft * FAR*, uIntf*, inflate_huft*, uInt*, uIntf*);
int inflate_trees_bits(uIntf*, uIntf*, inflate_huft * FAR*, inflate_huft*,
	z_streamp);
int inflate_trees_dynamic(uInt, uInt, uIntf*, uIntf*, uIntf*,
	inflate_huft * FAR*, inflate_huft * FAR*, inflate_huft*, z_streamp);
int inflate_trees_fixed(uIntf*, uIntf*, const inflate_huft * FAR*,
	const inflate_huft * FAR*, z_streamp);
int inflate_fast(uInt, uInt, const inflate_huft*, const inflate_huft*,
	inflate_blocks_statef*, z_streamp);
const uLongf* get_crc_table(void);
uLong ucrc32(uLong, const Bytef*, uInt);
uLong adler32(uLong, const Bytef*, uInt);
const char* zlibVersion(void);
const char* zError(int);
voidpf zcalloc(voidpf, unsigned, unsigned);
void zcfree(voidpf, voidpf);
int inflateReset(z_streamp);
int inflateEnd(z_streamp);
int inflateInit2(z_streamp);
int inflate(z_streamp, int);

struct LUFILE
{
	bool is_handle;
	bool canseek;
	HANDLE h;
	bool herr;
	unsigned long initial_offset;
	void* buf;
	unsigned int len, pos;
};

LUFILE* lufopen(void* z, unsigned int len, DWORD flags, ZRESULT* err);
int lufclose(LUFILE* stream);
int luferror(LUFILE* stream);
long int luftell(LUFILE* stream);
int lufseek(LUFILE* stream, long offset, int whence);
unsigned int lufread(void* ptr, unsigned int size, unsigned int n,
	LUFILE* stream);

#define UNZ_OK (0)
#define UNZ_END_OF_LIST_OF_FILE (-100)
#define UNZ_ERRNO (Z_ERRNO)
#define UNZ_EOF (0)
#define UNZ_PARAMERROR (-102)
#define UNZ_BADZIPFILE (-103)
#define UNZ_INTERNALERROR (-104)
#define UNZ_CRCERROR (-105)

#define UNZ_MAXFILENAMEINZIP (256)
#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

#define CASE_SENSITIVE 1
#define CASE_INSENSITIVE 2

typedef struct tm_unz_s
{
	uInt tm_sec;
	uInt tm_min;
	uInt tm_hour;
	uInt tm_mday;
	uInt tm_mon;
	uInt tm_year;
} tm_unz;

typedef struct unz_global_info_s
{
	uLong number_entry;
	uLong size_comment;
} unz_global_info;

typedef struct unz_file_info_s
{
	uLong version;
	uLong version_needed;
	uLong flag;
	uLong compression_method;
	uLong dosDate;
	uLong crc;
	uLong compressed_size;
	uLong uncompressed_size;
	uLong size_filename;
	uLong size_file_extra;
	uLong size_file_comment;
	uLong disk_num_start;
	uLong internal_fa;
	uLong external_fa;
	tm_unz tmu_date;
} unz_file_info;

typedef struct unz_file_info_internal_s
{
	uLong offset_curfile;
} unz_file_info_internal;

typedef struct
{
	char* read_buffer;
	z_stream stream;
	uLong pos_in_zipfile;
	uLong stream_initialised;
	uLong offset_local_extrafield;
	uInt size_local_extrafield;
	uLong pos_local_extrafield;
	uLong crc32;
	uLong crc32_wait;
	uLong rest_read_compressed;
	uLong rest_read_uncompressed;
	LUFILE* file;
	uLong compression_method;
	uLong byte_before_the_zipfile;
} file_in_zip_read_info_s;

typedef struct unz_s
{
	LUFILE* file;
	unz_global_info gi;
	uLong byte_before_the_zipfile;
	uLong num_file;
	uLong pos_in_central_dir;
	uLong current_file_ok;
	uLong central_pos;
	uLong size_central_dir;
	uLong offset_central_dir;
	unz_file_info cur_file_info;
	unz_file_info_internal cur_file_info_internal;
	file_in_zip_read_info_s* pfile_in_zip_read;
} unz_s;

typedef unz_s* unzFile;

int unzlocal_getByte(LUFILE* fin, int* pi);
int unzlocal_getShort(LUFILE* fin, uLong* pX);
int unzlocal_getLong(LUFILE* fin, uLong* pX);
int strcmpcasenosensitive_internal(const char* fileName1,
	const char* fileName2);
int unzStringFileNameCompare(const char* fileName1, const char* fileName2,
	int iCaseSensitivity);
uLong unzlocal_SearchCentralDir(LUFILE* fin);
unzFile unzOpenInternal(LUFILE* fin);
int unzClose(unzFile file);
int unzGetGlobalInfo(unzFile file, unz_global_info* pglobal_info);
void unzlocal_DosDateToTmuDate(uLong ulDosDate, tm_unz* ptm);
int unzlocal_GetCurrentFileInfoInternal(unzFile file, unz_file_info* pfile_info,
	unz_file_info_internal* pfile_info_internal, char* szFileName,
	uLong fileNameBufferSize, void* extraField, uLong extraFieldBufferSize,
	char* szComment, uLong commentBufferSize);
int unzGetCurrentFileInfo(unzFile file, unz_file_info* pfile_info,
	char* szFileName, uLong fileNameBufferSize, void* extraField,
	uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize);
int unzGoToFirstFile(unzFile file);
int unzGoToNextFile(unzFile file);
int unzLocateFile(unzFile file, const char* szFileName, int iCaseSensitivity);
int unzlocal_CheckCurrentFileCoherencyHeader(unz_s* s, uInt* piSizeVar,
	uLong* poffset_local_extrafield, uInt* psize_local_extrafield);
int unzOpenCurrentFile(unzFile file);
int unzReadCurrentFile(unzFile file, voidp buf, unsigned len);
long unztell(unzFile file);
int unzeof(unzFile file);
int unzGetLocalExtrafield(unzFile file, voidp buf, unsigned len);
int unzCloseCurrentFile(unzFile file);
int unzGetGlobalComment(unzFile file, char* szComment, uLong uSizeBuf);

FILETIME timet2filetime(const time_t t);
void EnsureDirectory(const char* rootdir, const char* dir);

class TUnzip
{
public:
	TUnzip();

	ZRESULT Open(void* z, unsigned int len, DWORD flags);
	ZRESULT Get(int index, ZIPENTRY* ze);
	ZRESULT Find(const char* name, bool ic, int* index, ZIPENTRY* ze);
	ZRESULT Unzip(int index, void* dst, unsigned int len, DWORD flags);
	ZRESULT Close();

	unzFile uf;
	int currentfile;
	ZIPENTRY cze;
	int czei;
	char rootdir[MAX_PATH];
};

extern ZRESULT lasterrorU;

extern "C" char* _tcsstr(char* s1, const char* s2)
{
	return (char*)_mbsstr((const unsigned char*)s1, (const unsigned char*)s2);
}

extern const char* const z_errmsg[10] = { "need dictionary", "stream end", "",
	"file error", "stream error", "data error", "insufficient memory",
	"buffer error", "incompatible version", "" };

const uInt inflate_mask[17] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f,
	0x003f, 0x007f, 0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff,
	0x7fff, 0xffff };

#define zmemcpy memcpy

#define WAVAIL (uInt)(q < s->read ? s->read - q - 1 : s->end - q)

#define UPDBITS \
	{ \
		s->bitb = b; \
		s->bitk = k; \
	}
#define UPDIN \
	{ \
		z->avail_in = n; \
		z->total_in += p - z->next_in; \
		z->next_in = p; \
	}
#define UPDOUT \
	{ \
		s->write = q; \
	}
#define UPDATE { UPDBITS UPDIN UPDOUT }
#define LEAVE \
	{ \
		UPDATE return inflate_flush(s, z, r); \
	}
#define LOADIN \
	{ \
		p = z->next_in; \
		n = z->avail_in; \
		b = s->bitb; \
		k = s->bitk; \
	}
#define NEEDBYTE \
	{ \
		if (n) \
			r = Z_OK; \
		else \
			LEAVE \
	}
#define NEXTBYTE (n--, *p++)
#define NEEDBITS(j) \
	{ \
		while (k < (j)) \
		{ \
			NEEDBYTE; \
			b |= ((uLong)NEXTBYTE) << k; \
			k += 8; \
		} \
	}
#define DUMPBITS(j) \
	{ \
		b >>= (j); \
		k -= (j); \
	}
#define LOADOUT \
	{ \
		q = s->write; \
		m = (uInt)WAVAIL; \
	}
#define WRAP \
	{ \
		if (q == s->end && s->read != s->window) \
		{ \
			q = s->window; \
			m = (uInt)WAVAIL; \
		} \
	}
#define FLUSH \
	{ \
		UPDOUT r = inflate_flush(s, z, r); \
		LOADOUT \
	}
#define NEEDOUT \
	{ \
		if (m == 0) \
		{ \
			WRAP if (m == 0) \
			{ \
				FLUSH WRAP if (m == 0) LEAVE \
			} \
		} \
		r = Z_OK; \
	}
#define OUTBYTE(a) \
	{ \
		*q++ = (Byte)(a); \
		m--; \
	}
#define LOAD { LOADIN LOADOUT }

int inflate_flush(inflate_blocks_statef* s, z_streamp z, int r)
{
	uInt n;
	Bytef* p;
	Bytef* q;

	p = z->next_out;
	q = s->read;

	n = (uInt)((q <= s->write ? s->write : s->end) - q);
	if (n > z->avail_out)
		n = z->avail_out;
	if (n && r == Z_BUF_ERROR)
		r = Z_OK;

	z->avail_out -= n;
	z->total_out += n;

	if (s->checkfn != Z_NULL)
		z->adler = s->check = (*s->checkfn)(s->check, q, n);

	if (n)
	{
		zmemcpy(p, q, n);
		p += n;
		q += n;
	}

	if (q == s->end)
	{
		q = s->window;
		if (s->write == s->end)
			s->write = s->window;

		n = (uInt)(s->write - q);
		if (n > z->avail_out)
			n = z->avail_out;
		if (n && r == Z_BUF_ERROR)
			r = Z_OK;

		z->avail_out -= n;
		z->total_out += n;

		if (s->checkfn != Z_NULL)
			z->adler = s->check = (*s->checkfn)(s->check, q, n);

		zmemcpy(p, q, n);
		p += n;
		q += n;
	}

	z->next_out = p;
	s->read = q;

	return r;
}

static const uInt fixed_bl = 9;
static const uInt fixed_bd = 5;

static const inflate_huft fixed_tl[] = {
	{ { { 96, 7 } },  256 },
    { { { 0, 8 } },   80  },
    { { { 0, 8 } },   16  },
	{ { { 84, 8 } },  115 },
    { { { 82, 7 } },  31  },
    { { { 0, 8 } },   112 },
	{ { { 0, 8 } },   48  },
    { { { 0, 9 } },   192 },
    { { { 80, 7 } },  10  },
	{ { { 0, 8 } },   96  },
    { { { 0, 8 } },   32  },
    { { { 0, 9 } },   160 },
	{ { { 0, 8 } },   0   },
    { { { 0, 8 } },   128 },
    { { { 0, 8 } },   64  },
	{ { { 0, 9 } },   224 },
    { { { 80, 7 } },  6   },
    { { { 0, 8 } },   88  },
	{ { { 0, 8 } },   24  },
    { { { 0, 9 } },   144 },
    { { { 83, 7 } },  59  },
	{ { { 0, 8 } },   120 },
    { { { 0, 8 } },   56  },
    { { { 0, 9 } },   208 },
	{ { { 81, 7 } },  17  },
    { { { 0, 8 } },   104 },
    { { { 0, 8 } },   40  },
	{ { { 0, 9 } },   176 },
    { { { 0, 8 } },   8   },
    { { { 0, 8 } },   136 },
	{ { { 0, 8 } },   72  },
    { { { 0, 9 } },   240 },
    { { { 80, 7 } },  4   },
	{ { { 0, 8 } },   84  },
    { { { 0, 8 } },   20  },
    { { { 85, 8 } },  227 },
	{ { { 83, 7 } },  43  },
    { { { 0, 8 } },   116 },
    { { { 0, 8 } },   52  },
	{ { { 0, 9 } },   200 },
    { { { 81, 7 } },  13  },
    { { { 0, 8 } },   100 },
	{ { { 0, 8 } },   36  },
    { { { 0, 9 } },   168 },
    { { { 0, 8 } },   4   },
	{ { { 0, 8 } },   132 },
    { { { 0, 8 } },   68  },
    { { { 0, 9 } },   232 },
	{ { { 80, 7 } },  8   },
    { { { 0, 8 } },   92  },
    { { { 0, 8 } },   28  },
	{ { { 0, 9 } },   152 },
    { { { 84, 7 } },  83  },
    { { { 0, 8 } },   124 },
	{ { { 0, 8 } },   60  },
    { { { 0, 9 } },   216 },
    { { { 82, 7 } },  23  },
	{ { { 0, 8 } },   108 },
    { { { 0, 8 } },   44  },
    { { { 0, 9 } },   184 },
	{ { { 0, 8 } },   12  },
    { { { 0, 8 } },   140 },
    { { { 0, 8 } },   76  },
	{ { { 0, 9 } },   248 },
    { { { 80, 7 } },  3   },
    { { { 0, 8 } },   82  },
	{ { { 0, 8 } },   18  },
    { { { 85, 8 } },  163 },
    { { { 83, 7 } },  35  },
	{ { { 0, 8 } },   114 },
    { { { 0, 8 } },   50  },
    { { { 0, 9 } },   196 },
	{ { { 81, 7 } },  11  },
    { { { 0, 8 } },   98  },
    { { { 0, 8 } },   34  },
	{ { { 0, 9 } },   164 },
    { { { 0, 8 } },   2   },
    { { { 0, 8 } },   130 },
	{ { { 0, 8 } },   66  },
    { { { 0, 9 } },   228 },
    { { { 80, 7 } },  7   },
	{ { { 0, 8 } },   90  },
    { { { 0, 8 } },   26  },
    { { { 0, 9 } },   148 },
	{ { { 84, 7 } },  67  },
    { { { 0, 8 } },   122 },
    { { { 0, 8 } },   58  },
	{ { { 0, 9 } },   212 },
    { { { 82, 7 } },  19  },
    { { { 0, 8 } },   106 },
	{ { { 0, 8 } },   42  },
    { { { 0, 9 } },   180 },
    { { { 0, 8 } },   10  },
	{ { { 0, 8 } },   138 },
    { { { 0, 8 } },   74  },
    { { { 0, 9 } },   244 },
	{ { { 80, 7 } },  5   },
    { { { 0, 8 } },   86  },
    { { { 0, 8 } },   22  },
	{ { { 192, 8 } }, 0   },
    { { { 83, 7 } },  51  },
    { { { 0, 8 } },   118 },
	{ { { 0, 8 } },   54  },
    { { { 0, 9 } },   204 },
    { { { 81, 7 } },  15  },
	{ { { 0, 8 } },   102 },
    { { { 0, 8 } },   38  },
    { { { 0, 9 } },   172 },
	{ { { 0, 8 } },   6   },
    { { { 0, 8 } },   134 },
    { { { 0, 8 } },   70  },
	{ { { 0, 9 } },   236 },
    { { { 80, 7 } },  9   },
    { { { 0, 8 } },   94  },
	{ { { 0, 8 } },   30  },
    { { { 0, 9 } },   156 },
    { { { 84, 7 } },  99  },
	{ { { 0, 8 } },   126 },
    { { { 0, 8 } },   62  },
    { { { 0, 9 } },   220 },
	{ { { 82, 7 } },  27  },
    { { { 0, 8 } },   110 },
    { { { 0, 8 } },   46  },
	{ { { 0, 9 } },   188 },
    { { { 0, 8 } },   14  },
    { { { 0, 8 } },   142 },
	{ { { 0, 8 } },   78  },
    { { { 0, 9 } },   252 },
    { { { 96, 7 } },  256 },
	{ { { 0, 8 } },   81  },
    { { { 0, 8 } },   17  },
    { { { 85, 8 } },  131 },
	{ { { 82, 7 } },  31  },
    { { { 0, 8 } },   113 },
    { { { 0, 8 } },   49  },
	{ { { 0, 9 } },   194 },
    { { { 80, 7 } },  10  },
    { { { 0, 8 } },   97  },
	{ { { 0, 8 } },   33  },
    { { { 0, 9 } },   162 },
    { { { 0, 8 } },   1   },
	{ { { 0, 8 } },   129 },
    { { { 0, 8 } },   65  },
    { { { 0, 9 } },   226 },
	{ { { 80, 7 } },  6   },
    { { { 0, 8 } },   89  },
    { { { 0, 8 } },   25  },
	{ { { 0, 9 } },   146 },
    { { { 83, 7 } },  59  },
    { { { 0, 8 } },   121 },
	{ { { 0, 8 } },   57  },
    { { { 0, 9 } },   210 },
    { { { 81, 7 } },  17  },
	{ { { 0, 8 } },   105 },
    { { { 0, 8 } },   41  },
    { { { 0, 9 } },   178 },
	{ { { 0, 8 } },   9   },
    { { { 0, 8 } },   137 },
    { { { 0, 8 } },   73  },
	{ { { 0, 9 } },   242 },
    { { { 80, 7 } },  4   },
    { { { 0, 8 } },   85  },
	{ { { 0, 8 } },   21  },
    { { { 80, 8 } },  258 },
    { { { 83, 7 } },  43  },
	{ { { 0, 8 } },   117 },
    { { { 0, 8 } },   53  },
    { { { 0, 9 } },   202 },
	{ { { 81, 7 } },  13  },
    { { { 0, 8 } },   101 },
    { { { 0, 8 } },   37  },
	{ { { 0, 9 } },   170 },
    { { { 0, 8 } },   5   },
    { { { 0, 8 } },   133 },
	{ { { 0, 8 } },   69  },
    { { { 0, 9 } },   234 },
    { { { 80, 7 } },  8   },
	{ { { 0, 8 } },   93  },
    { { { 0, 8 } },   29  },
    { { { 0, 9 } },   154 },
	{ { { 84, 7 } },  83  },
    { { { 0, 8 } },   125 },
    { { { 0, 8 } },   61  },
	{ { { 0, 9 } },   218 },
    { { { 82, 7 } },  23  },
    { { { 0, 8 } },   109 },
	{ { { 0, 8 } },   45  },
    { { { 0, 9 } },   186 },
    { { { 0, 8 } },   13  },
	{ { { 0, 8 } },   141 },
    { { { 0, 8 } },   77  },
    { { { 0, 9 } },   250 },
	{ { { 80, 7 } },  3   },
    { { { 0, 8 } },   83  },
    { { { 0, 8 } },   19  },
	{ { { 85, 8 } },  195 },
    { { { 83, 7 } },  35  },
    { { { 0, 8 } },   115 },
	{ { { 0, 8 } },   51  },
    { { { 0, 9 } },   198 },
    { { { 81, 7 } },  11  },
	{ { { 0, 8 } },   99  },
    { { { 0, 8 } },   35  },
    { { { 0, 9 } },   166 },
	{ { { 0, 8 } },   3   },
    { { { 0, 8 } },   131 },
    { { { 0, 8 } },   67  },
	{ { { 0, 9 } },   230 },
    { { { 80, 7 } },  7   },
    { { { 0, 8 } },   91  },
	{ { { 0, 8 } },   27  },
    { { { 0, 9 } },   150 },
    { { { 84, 7 } },  67  },
	{ { { 0, 8 } },   123 },
    { { { 0, 8 } },   59  },
    { { { 0, 9 } },   214 },
	{ { { 82, 7 } },  19  },
    { { { 0, 8 } },   107 },
    { { { 0, 8 } },   43  },
	{ { { 0, 9 } },   182 },
    { { { 0, 8 } },   11  },
    { { { 0, 8 } },   139 },
	{ { { 0, 8 } },   75  },
    { { { 0, 9 } },   246 },
    { { { 80, 7 } },  5   },
	{ { { 0, 8 } },   87  },
    { { { 0, 8 } },   23  },
    { { { 192, 8 } }, 0   },
	{ { { 83, 7 } },  51  },
    { { { 0, 8 } },   119 },
    { { { 0, 8 } },   55  },
	{ { { 0, 9 } },   206 },
    { { { 81, 7 } },  15  },
    { { { 0, 8 } },   103 },
	{ { { 0, 8 } },   39  },
    { { { 0, 9 } },   174 },
    { { { 0, 8 } },   7   },
	{ { { 0, 8 } },   135 },
    { { { 0, 8 } },   71  },
    { { { 0, 9 } },   238 },
	{ { { 80, 7 } },  9   },
    { { { 0, 8 } },   95  },
    { { { 0, 8 } },   31  },
	{ { { 0, 9 } },   158 },
    { { { 84, 7 } },  99  },
    { { { 0, 8 } },   127 },
	{ { { 0, 8 } },   63  },
    { { { 0, 9 } },   222 },
    { { { 82, 7 } },  27  },
	{ { { 0, 8 } },   111 },
    { { { 0, 8 } },   47  },
    { { { 0, 9 } },   190 },
	{ { { 0, 8 } },   15  },
    { { { 0, 8 } },   143 },
    { { { 0, 8 } },   79  },
	{ { { 0, 9 } },   254 },
    { { { 96, 7 } },  256 },
    { { { 0, 8 } },   80  },
	{ { { 0, 8 } },   16  },
    { { { 84, 8 } },  115 },
    { { { 82, 7 } },  31  },
	{ { { 0, 8 } },   112 },
    { { { 0, 8 } },   48  },
    { { { 0, 9 } },   193 },
	{ { { 80, 7 } },  10  },
    { { { 0, 8 } },   96  },
    { { { 0, 8 } },   32  },
	{ { { 0, 9 } },   161 },
    { { { 0, 8 } },   0   },
    { { { 0, 8 } },   128 },
	{ { { 0, 8 } },   64  },
    { { { 0, 9 } },   225 },
    { { { 80, 7 } },  6   },
	{ { { 0, 8 } },   88  },
    { { { 0, 8 } },   24  },
    { { { 0, 9 } },   145 },
	{ { { 83, 7 } },  59  },
    { { { 0, 8 } },   120 },
    { { { 0, 8 } },   56  },
	{ { { 0, 9 } },   209 },
    { { { 81, 7 } },  17  },
    { { { 0, 8 } },   104 },
	{ { { 0, 8 } },   40  },
    { { { 0, 9 } },   177 },
    { { { 0, 8 } },   8   },
	{ { { 0, 8 } },   136 },
    { { { 0, 8 } },   72  },
    { { { 0, 9 } },   241 },
	{ { { 80, 7 } },  4   },
    { { { 0, 8 } },   84  },
    { { { 0, 8 } },   20  },
	{ { { 85, 8 } },  227 },
    { { { 83, 7 } },  43  },
    { { { 0, 8 } },   116 },
	{ { { 0, 8 } },   52  },
    { { { 0, 9 } },   201 },
    { { { 81, 7 } },  13  },
	{ { { 0, 8 } },   100 },
    { { { 0, 8 } },   36  },
    { { { 0, 9 } },   169 },
	{ { { 0, 8 } },   4   },
    { { { 0, 8 } },   132 },
    { { { 0, 8 } },   68  },
	{ { { 0, 9 } },   233 },
    { { { 80, 7 } },  8   },
    { { { 0, 8 } },   92  },
	{ { { 0, 8 } },   28  },
    { { { 0, 9 } },   153 },
    { { { 84, 7 } },  83  },
	{ { { 0, 8 } },   124 },
    { { { 0, 8 } },   60  },
    { { { 0, 9 } },   217 },
	{ { { 82, 7 } },  23  },
    { { { 0, 8 } },   108 },
    { { { 0, 8 } },   44  },
	{ { { 0, 9 } },   185 },
    { { { 0, 8 } },   12  },
    { { { 0, 8 } },   140 },
	{ { { 0, 8 } },   76  },
    { { { 0, 9 } },   249 },
    { { { 80, 7 } },  3   },
	{ { { 0, 8 } },   82  },
    { { { 0, 8 } },   18  },
    { { { 85, 8 } },  163 },
	{ { { 83, 7 } },  35  },
    { { { 0, 8 } },   114 },
    { { { 0, 8 } },   50  },
	{ { { 0, 9 } },   197 },
    { { { 81, 7 } },  11  },
    { { { 0, 8 } },   98  },
	{ { { 0, 8 } },   34  },
    { { { 0, 9 } },   165 },
    { { { 0, 8 } },   2   },
	{ { { 0, 8 } },   130 },
    { { { 0, 8 } },   66  },
    { { { 0, 9 } },   229 },
	{ { { 80, 7 } },  7   },
    { { { 0, 8 } },   90  },
    { { { 0, 8 } },   26  },
	{ { { 0, 9 } },   149 },
    { { { 84, 7 } },  67  },
    { { { 0, 8 } },   122 },
	{ { { 0, 8 } },   58  },
    { { { 0, 9 } },   213 },
    { { { 82, 7 } },  19  },
	{ { { 0, 8 } },   106 },
    { { { 0, 8 } },   42  },
    { { { 0, 9 } },   181 },
	{ { { 0, 8 } },   10  },
    { { { 0, 8 } },   138 },
    { { { 0, 8 } },   74  },
	{ { { 0, 9 } },   245 },
    { { { 80, 7 } },  5   },
    { { { 0, 8 } },   86  },
	{ { { 0, 8 } },   22  },
    { { { 192, 8 } }, 0   },
    { { { 83, 7 } },  51  },
	{ { { 0, 8 } },   118 },
    { { { 0, 8 } },   54  },
    { { { 0, 9 } },   205 },
	{ { { 81, 7 } },  15  },
    { { { 0, 8 } },   102 },
    { { { 0, 8 } },   38  },
	{ { { 0, 9 } },   173 },
    { { { 0, 8 } },   6   },
    { { { 0, 8 } },   134 },
	{ { { 0, 8 } },   70  },
    { { { 0, 9 } },   237 },
    { { { 80, 7 } },  9   },
	{ { { 0, 8 } },   94  },
    { { { 0, 8 } },   30  },
    { { { 0, 9 } },   157 },
	{ { { 84, 7 } },  99  },
    { { { 0, 8 } },   126 },
    { { { 0, 8 } },   62  },
	{ { { 0, 9 } },   221 },
    { { { 82, 7 } },  27  },
    { { { 0, 8 } },   110 },
	{ { { 0, 8 } },   46  },
    { { { 0, 9 } },   189 },
    { { { 0, 8 } },   14  },
	{ { { 0, 8 } },   142 },
    { { { 0, 8 } },   78  },
    { { { 0, 9 } },   253 },
	{ { { 96, 7 } },  256 },
    { { { 0, 8 } },   81  },
    { { { 0, 8 } },   17  },
	{ { { 85, 8 } },  131 },
    { { { 82, 7 } },  31  },
    { { { 0, 8 } },   113 },
	{ { { 0, 8 } },   49  },
    { { { 0, 9 } },   195 },
    { { { 80, 7 } },  10  },
	{ { { 0, 8 } },   97  },
    { { { 0, 8 } },   33  },
    { { { 0, 9 } },   163 },
	{ { { 0, 8 } },   1   },
    { { { 0, 8 } },   129 },
    { { { 0, 8 } },   65  },
	{ { { 0, 9 } },   227 },
    { { { 80, 7 } },  6   },
    { { { 0, 8 } },   89  },
	{ { { 0, 8 } },   25  },
    { { { 0, 9 } },   147 },
    { { { 83, 7 } },  59  },
	{ { { 0, 8 } },   121 },
    { { { 0, 8 } },   57  },
    { { { 0, 9 } },   211 },
	{ { { 81, 7 } },  17  },
    { { { 0, 8 } },   105 },
    { { { 0, 8 } },   41  },
	{ { { 0, 9 } },   179 },
    { { { 0, 8 } },   9   },
    { { { 0, 8 } },   137 },
	{ { { 0, 8 } },   73  },
    { { { 0, 9 } },   243 },
    { { { 80, 7 } },  4   },
	{ { { 0, 8 } },   85  },
    { { { 0, 8 } },   21  },
    { { { 80, 8 } },  258 },
	{ { { 83, 7 } },  43  },
    { { { 0, 8 } },   117 },
    { { { 0, 8 } },   53  },
	{ { { 0, 9 } },   203 },
    { { { 81, 7 } },  13  },
    { { { 0, 8 } },   101 },
	{ { { 0, 8 } },   37  },
    { { { 0, 9 } },   171 },
    { { { 0, 8 } },   5   },
	{ { { 0, 8 } },   133 },
    { { { 0, 8 } },   69  },
    { { { 0, 9 } },   235 },
	{ { { 80, 7 } },  8   },
    { { { 0, 8 } },   93  },
    { { { 0, 8 } },   29  },
	{ { { 0, 9 } },   155 },
    { { { 84, 7 } },  83  },
    { { { 0, 8 } },   125 },
	{ { { 0, 8 } },   61  },
    { { { 0, 9 } },   219 },
    { { { 82, 7 } },  23  },
	{ { { 0, 8 } },   109 },
    { { { 0, 8 } },   45  },
    { { { 0, 9 } },   187 },
	{ { { 0, 8 } },   13  },
    { { { 0, 8 } },   141 },
    { { { 0, 8 } },   77  },
	{ { { 0, 9 } },   251 },
    { { { 80, 7 } },  3   },
    { { { 0, 8 } },   83  },
	{ { { 0, 8 } },   19  },
    { { { 85, 8 } },  195 },
    { { { 83, 7 } },  35  },
	{ { { 0, 8 } },   115 },
    { { { 0, 8 } },   51  },
    { { { 0, 9 } },   199 },
	{ { { 81, 7 } },  11  },
    { { { 0, 8 } },   99  },
    { { { 0, 8 } },   35  },
	{ { { 0, 9 } },   167 },
    { { { 0, 8 } },   3   },
    { { { 0, 8 } },   131 },
	{ { { 0, 8 } },   67  },
    { { { 0, 9 } },   231 },
    { { { 80, 7 } },  7   },
	{ { { 0, 8 } },   91  },
    { { { 0, 8 } },   27  },
    { { { 0, 9 } },   151 },
	{ { { 84, 7 } },  67  },
    { { { 0, 8 } },   123 },
    { { { 0, 8 } },   59  },
	{ { { 0, 9 } },   215 },
    { { { 82, 7 } },  19  },
    { { { 0, 8 } },   107 },
	{ { { 0, 8 } },   43  },
    { { { 0, 9 } },   183 },
    { { { 0, 8 } },   11  },
	{ { { 0, 8 } },   139 },
    { { { 0, 8 } },   75  },
    { { { 0, 9 } },   247 },
	{ { { 80, 7 } },  5   },
    { { { 0, 8 } },   87  },
    { { { 0, 8 } },   23  },
	{ { { 192, 8 } }, 0   },
    { { { 83, 7 } },  51  },
    { { { 0, 8 } },   119 },
	{ { { 0, 8 } },   55  },
    { { { 0, 9 } },   207 },
    { { { 81, 7 } },  15  },
	{ { { 0, 8 } },   103 },
    { { { 0, 8 } },   39  },
    { { { 0, 9 } },   175 },
	{ { { 0, 8 } },   7   },
    { { { 0, 8 } },   135 },
    { { { 0, 8 } },   71  },
	{ { { 0, 9 } },   239 },
    { { { 80, 7 } },  9   },
    { { { 0, 8 } },   95  },
	{ { { 0, 8 } },   31  },
    { { { 0, 9 } },   159 },
    { { { 84, 7 } },  99  },
	{ { { 0, 8 } },   127 },
    { { { 0, 8 } },   63  },
    { { { 0, 9 } },   223 },
	{ { { 82, 7 } },  27  },
    { { { 0, 8 } },   111 },
    { { { 0, 8 } },   47  },
	{ { { 0, 9 } },   191 },
    { { { 0, 8 } },   15  },
    { { { 0, 8 } },   143 },
	{ { { 0, 8 } },   79  },
    { { { 0, 9 } },   255 }
};

static const inflate_huft fixed_td[] = {
	{ { { 80, 5 } },  1     },
    { { { 87, 5 } },  257   },
    { { { 83, 5 } },  17    },
	{ { { 91, 5 } },  4097  },
    { { { 81, 5 } },  5     },
    { { { 89, 5 } },  1025  },
	{ { { 85, 5 } },  65    },
    { { { 93, 5 } },  16385 },
    { { { 80, 5 } },  3     },
	{ { { 88, 5 } },  513   },
    { { { 84, 5 } },  33    },
    { { { 92, 5 } },  8193  },
	{ { { 82, 5 } },  9     },
    { { { 90, 5 } },  2049  },
    { { { 86, 5 } },  129   },
	{ { { 192, 5 } }, 24577 },
    { { { 80, 5 } },  2     },
    { { { 87, 5 } },  385   },
	{ { { 83, 5 } },  25    },
    { { { 91, 5 } },  6145  },
    { { { 81, 5 } },  7     },
	{ { { 89, 5 } },  1537  },
    { { { 85, 5 } },  97    },
    { { { 93, 5 } },  24577 },
	{ { { 80, 5 } },  4     },
    { { { 88, 5 } },  769   },
    { { { 84, 5 } },  49    },
	{ { { 92, 5 } },  12289 },
    { { { 82, 5 } },  13    },
    { { { 90, 5 } },  3073  },
	{ { { 86, 5 } },  193   },
    { { { 192, 5 } }, 24577 }
};

inflate_codes_statef* inflate_codes_new(uInt bl, uInt bd,
	const inflate_huft* tl, const inflate_huft* td, z_streamp z)
{
	inflate_codes_statef* c;

	if ((c = (inflate_codes_statef*)ZALLOC(z, 1,
			 sizeof(struct inflate_codes_state))) != Z_NULL)
	{
		c->mode = ICM_START;
		c->lbits = (Byte)bl;
		c->dbits = (Byte)bd;
		c->ltree = (inflate_huft*)tl;
		c->dtree = (inflate_huft*)td;
	}
	return c;
}

void inflate_codes_free(inflate_codes_statef* c, z_streamp z)
{
	ZFREE(z, c);
}

int inflate_codes(inflate_blocks_statef* s, z_streamp z, int r)
{
	uInt j;
	inflate_huft* t;
	uInt e;
	uLong b;
	uInt k;
	Bytef* p;
	uInt n;
	Bytef* q;
	uInt m;
	Bytef* f;
	inflate_codes_statef* c = s->sub.decode.codes;

	LOAD

		while (1) switch (c->mode)
	{
	case ICM_START:
		if (m >= 258 && n >= 10)
		{
			UPDATE
			r = inflate_fast(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
			LOAD if (r != Z_OK)
			{
				c->mode = r == Z_STREAM_END ? ICM_WASH : ICM_BADCODE;
				break;
			}
		}
		c->sub.code.need = c->lbits;
		c->sub.code.tree = c->ltree;
		c->mode = ICM_LEN;
	case ICM_LEN:
		j = c->sub.code.need;
		NEEDBITS(j)
		t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
		DUMPBITS(t->bits)
		e = (uInt)(t->exop);
		if (e == 0)
		{
			c->sub.lit = t->base;
			c->mode = ICM_LIT;
			break;
		}
		if (e & 16)
		{
			c->sub.copy.get = e & 15;
			c->len = t->base;
			c->mode = ICM_LENEXT;
			break;
		}
		if ((e & 64) == 0)
		{
			c->sub.code.need = e;
			c->sub.code.tree = t + t->base;
			break;
		}
		if (e & 32)
		{
			c->mode = ICM_WASH;
			break;
		}
		c->mode = ICM_BADCODE;
		z->msg = (char*)"invalid literal/length code";
		r = Z_DATA_ERROR;
		LEAVE
	case ICM_LENEXT:
		j = c->sub.copy.get;
		NEEDBITS(j)
		c->len += (uInt)b & inflate_mask[j];
		DUMPBITS(j)
		c->sub.code.need = c->dbits;
		c->sub.code.tree = c->dtree;
		c->mode = ICM_DIST;
	case ICM_DIST:
		j = c->sub.code.need;
		NEEDBITS(j)
		t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
		DUMPBITS(t->bits)
		e = (uInt)(t->exop);
		if (e & 16)
		{
			c->sub.copy.get = e & 15;
			c->sub.copy.dist = t->base;
			c->mode = ICM_DISTEXT;
			break;
		}
		if ((e & 64) == 0)
		{
			c->sub.code.need = e;
			c->sub.code.tree = t + t->base;
			break;
		}
		c->mode = ICM_BADCODE;
		z->msg = (char*)"invalid distance code";
		r = Z_DATA_ERROR;
		LEAVE
	case ICM_DISTEXT:
		j = c->sub.copy.get;
		NEEDBITS(j)
		c->sub.copy.dist += (uInt)b & inflate_mask[j];
		DUMPBITS(j)
		c->mode = ICM_COPY;
	case ICM_COPY:
		f = (uInt)(q - s->window) < c->sub.copy.dist
			? s->end - (c->sub.copy.dist - (q - s->window))
			: q - c->sub.copy.dist;
		while (c->len)
		{
			NEEDOUT
			OUTBYTE(*f++)
			if (f == s->end)
				f = s->window;
			c->len--;
		}
		c->mode = ICM_START;
		break;
	case ICM_LIT:
		NEEDOUT
		OUTBYTE(c->sub.lit)
		c->mode = ICM_START;
		break;
	case ICM_WASH:
		if (k > 7)
		{
			k -= 8;
			n++;
			p--;
		}
		FLUSH
		if (s->read != s->write)
			LEAVE
		c->mode = ICM_END;
	case ICM_END:
		r = Z_STREAM_END;
		LEAVE
	case ICM_BADCODE:
		r = Z_DATA_ERROR;
		LEAVE
	default:
		r = Z_STREAM_ERROR;
		LEAVE
	}
}

static const uInt border[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3,
	13, 2, 14, 1, 15 };

void inflate_blocks_reset(inflate_blocks_statef* s, z_streamp z, uLong* c)
{
	if (c != Z_NULL)
		*c = s->check;
	if (s->mode == IBM_BTREE || s->mode == IBM_DTREE)
		ZFREE(z, s->sub.trees.blens);
	if (s->mode == IBM_CODES)
		inflate_codes_free(s->sub.decode.codes, z);
	s->mode = IBM_TYPE;
	s->bitk = 0;
	s->bitb = 0;
	s->read = s->write = s->window;
	if (s->checkfn != Z_NULL)
		z->adler = s->check = (*s->checkfn)(0L, (const Bytef*)Z_NULL, 0);
}

inflate_blocks_statef* inflate_blocks_new(z_streamp z, check_func c, uInt w)
{
	inflate_blocks_statef* s;

	if ((s = (inflate_blocks_statef*)ZALLOC(z, 1,
			 sizeof(struct inflate_blocks_state))) == Z_NULL)
		return s;
	if ((s->hufts = (inflate_huft*)ZALLOC(z, sizeof(inflate_huft), MANY)) ==
		Z_NULL)
	{
		ZFREE(z, s);
		return Z_NULL;
	}
	if ((s->window = (Bytef*)ZALLOC(z, 1, w)) == Z_NULL)
	{
		ZFREE(z, s->hufts);
		ZFREE(z, s);
		return Z_NULL;
	}
	s->end = s->window + w;
	s->checkfn = c;
	s->mode = IBM_TYPE;
	inflate_blocks_reset(s, z, Z_NULL);
	return s;
}

int inflate_blocks(inflate_blocks_statef* s, z_streamp z, int r)
{
	uInt t;
	uLong b;
	uInt k;
	Bytef* p;
	uInt n;
	Bytef* q;
	uInt m;

	LOAD

		while (1) switch (s->mode)
	{
	case IBM_TYPE:
		NEEDBITS(3)
		t = (uInt)b & 7;
		s->last = t & 1;
		switch (t >> 1)
		{
		case 0:
			DUMPBITS(3)
			t = k & 7;
			DUMPBITS(t)
			s->mode = IBM_LENS;
			break;
		case 1:
		{
			uInt bl, bd;
			const inflate_huft* tl;
			const inflate_huft* td;

			inflate_trees_fixed(&bl, &bd, &tl, &td, z);
			s->sub.decode.codes = inflate_codes_new(bl, bd, tl, td, z);
			if (s->sub.decode.codes == Z_NULL)
			{
				r = Z_MEM_ERROR;
				LEAVE
			}
		}
			DUMPBITS(3)
			s->mode = IBM_CODES;
			break;
		case 2:
			DUMPBITS(3)
			s->mode = IBM_TABLE;
			break;
		case 3:
			DUMPBITS(3)
			s->mode = IBM_BAD;
			z->msg = (char*)"invalid block type";
			r = Z_DATA_ERROR;
			LEAVE
		}
		break;
	case IBM_LENS:
		NEEDBITS(32)
		if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
		{
			s->mode = IBM_BAD;
			z->msg = (char*)"invalid stored block lengths";
			r = Z_DATA_ERROR;
			LEAVE
		}
		s->sub.left = (uInt)b & 0xffff;
		b = k = 0;
		s->mode = s->sub.left ? IBM_STORED : (s->last ? IBM_DRY : IBM_TYPE);
		break;
	case IBM_STORED:
		if (n == 0)
			LEAVE
		NEEDOUT
		t = s->sub.left;
		if (t > n)
			t = n;
		if (t > m)
			t = m;
		zmemcpy(q, p, t);
		p += t;
		n -= t;
		q += t;
		m -= t;
		if ((s->sub.left -= t) != 0)
			break;
		s->mode = s->last ? IBM_DRY : IBM_TYPE;
		break;
	case IBM_TABLE:
		NEEDBITS(14)
		s->sub.trees.table = t = (uInt)b & 0x3fff;
		if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
		{
			s->mode = IBM_BAD;
			z->msg = (char*)"too many length or distance symbols";
			r = Z_DATA_ERROR;
			LEAVE
		}
		t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
		if ((s->sub.trees.blens = (uIntf*)ZALLOC(z, t, sizeof(uInt))) == Z_NULL)
		{
			r = Z_MEM_ERROR;
			LEAVE
		}
		DUMPBITS(14)
		s->sub.trees.index = 0;
		s->mode = IBM_BTREE;
	case IBM_BTREE:
		while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
		{
			NEEDBITS(3)
			s->sub.trees.blens[border[s->sub.trees.index++]] = (uInt)b & 7;
			DUMPBITS(3)
		}
		while (s->sub.trees.index < 19)
			s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
		s->sub.trees.bb = 7;
		t = inflate_trees_bits(s->sub.trees.blens, &s->sub.trees.bb,
			&s->sub.trees.tb, s->hufts, z);
		if (t != Z_OK)
		{
			ZFREE(z, s->sub.trees.blens);
			r = t;
			if (r == Z_DATA_ERROR)
				s->mode = IBM_BAD;
			LEAVE
		}
		s->sub.trees.index = 0;
		s->mode = IBM_DTREE;
	case IBM_DTREE:
		while (t = s->sub.trees.table,
			s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
		{
			inflate_huft* h;
			uInt i, j, c;

			t = s->sub.trees.bb;
			NEEDBITS(t)
			h = s->sub.trees.tb + ((uInt)b & inflate_mask[t]);
			t = h->bits;
			c = h->base;
			if (c < 16)
			{
				DUMPBITS(t)
				s->sub.trees.blens[s->sub.trees.index++] = c;
			}
			else
			{
				i = c == 18 ? 7 : c - 14;
				j = c == 18 ? 11 : 3;
				NEEDBITS(t + i)
				DUMPBITS(t)
				j += (uInt)b & inflate_mask[i];
				DUMPBITS(i)
				i = s->sub.trees.index;
				t = s->sub.trees.table;
				if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
					(c == 16 && i < 1))
				{
					ZFREE(z, s->sub.trees.blens);
					s->mode = IBM_BAD;
					z->msg = (char*)"invalid bit length repeat";
					r = Z_DATA_ERROR;
					LEAVE
				}
				c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
				do
				{
					s->sub.trees.blens[i++] = c;
				} while (--j);
				s->sub.trees.index = i;
			}
		}
		s->sub.trees.tb = Z_NULL;
		{
			uInt bl, bd;
			inflate_huft* tl;
			inflate_huft* td;
			inflate_codes_statef* c;

			bl = 9;
			bd = 6;
			t = s->sub.trees.table;
			t = inflate_trees_dynamic(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
				s->sub.trees.blens, &bl, &bd, &tl, &td, s->hufts, z);
			ZFREE(z, s->sub.trees.blens);
			if (t != Z_OK)
			{
				if (t == (uInt)Z_DATA_ERROR)
					s->mode = IBM_BAD;
				r = t;
				LEAVE
			}
			if ((c = inflate_codes_new(bl, bd, tl, td, z)) == Z_NULL)
			{
				r = Z_MEM_ERROR;
				LEAVE
			}
			s->sub.decode.codes = c;
		}
		s->mode = IBM_CODES;
	case IBM_CODES:
		UPDATE
		if ((r = inflate_codes(s, z, r)) != Z_STREAM_END)
			return inflate_flush(s, z, r);
		r = Z_OK;
		inflate_codes_free(s->sub.decode.codes, z);
		LOAD if (!s->last)
		{
			s->mode = IBM_TYPE;
			break;
		}
		s->mode = IBM_DRY;
	case IBM_DRY:
		FLUSH
		if (s->read != s->write)
			LEAVE
		s->mode = IBM_DONE;
	case IBM_DONE:
		r = Z_STREAM_END;
		LEAVE
	case IBM_BAD:
		r = Z_DATA_ERROR;
		LEAVE
	default:
		r = Z_STREAM_ERROR;
		LEAVE
	}
}

int inflate_blocks_free(inflate_blocks_statef* s, z_streamp z)
{
	inflate_blocks_reset(s, z, Z_NULL);
	ZFREE(z, s->window);
	ZFREE(z, s->hufts);
	ZFREE(z, s);
	return Z_OK;
}

extern const char inflate_copyright[] =
	" inflate 1.1.3 Copyright 1995-1998 Mark Adler ";

#define BMAX 15

static const uInt cplens[31] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19,
	23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,
	0 };

static const uInt cplext[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 112, 112 };
static const uInt cpdist[30] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65,
	97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193,
	12289, 16385, 24577 };
static const uInt cpdext[30] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
	7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };

int huft_build(uIntf* b, uInt n, uInt s, const uIntf* d, const uIntf* e,
	inflate_huft * FAR * t, uIntf* m, inflate_huft* hp, uInt* hn, uIntf* v)
{
	uInt a;
	uInt c[BMAX + 1];
	uInt f;
	int g;
	int h;
	register uInt i;
	register uInt j;
	register int k;
	int l;
	uInt mask;
	register uIntf* p;
	inflate_huft* q;
	struct inflate_huft_s r;
	inflate_huft* u[BMAX];
	register int w;
	uInt x[BMAX + 1];
	uIntf* xp;
	int y;
	uInt z;

	p = c;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	*p = 0;
	p = b;
	i = n;
	do
	{
		c[*p++]++;
	} while (--i);
	if (c[0] == n)
	{
		*t = (inflate_huft*)Z_NULL;
		*m = 0;
		return Z_OK;
	}

	l = *m;
	for (j = 1; j <= BMAX; j++)
		if (c[j])
			break;
	k = j;
	if ((uInt)l < j)
		l = j;
	for (i = BMAX; i; i--)
		if (c[i])
			break;
	g = i;
	if ((uInt)l > i)
		l = i;
	*m = l;

	for (y = 1 << j; j < i; j++, y <<= 1)
		if ((y -= c[j]) < 0)
			return Z_DATA_ERROR;
	if ((y -= c[i]) < 0)
		return Z_DATA_ERROR;
	c[i] += y;

	x[1] = j = 0;
	p = c + 1;
	xp = x + 2;
	while (--i)
	{
		*xp++ = (j += *p++);
	}

	p = b;
	i = 0;
	do
	{
		if ((j = *p++) != 0)
			v[x[j]++] = i;
	} while (++i < n);
	n = x[g];

	x[0] = i = 0;
	p = v;
	h = -1;
	w = -l;
	u[0] = (inflate_huft*)Z_NULL;
	q = (inflate_huft*)Z_NULL;
	z = 0;

	for (; k <= g; k++)
	{
		a = c[k];
		while (a--)
		{
			while (k > w + l)
			{
				h++;
				w += l;

				z = g - w;
				z = z > (uInt)l ? l : z;
				if ((f = 1 << (j = k - w)) > a + 1)
				{
					f -= a + 1;
					xp = c + k;
					if (j < z)
						while (++j < z)
						{
							if ((f <<= 1) <= *++xp)
								break;
							f -= *xp;
						}
				}
				z = 1 << j;

				if (*hn + z > MANY)
					return Z_MEM_ERROR;
				u[h] = q = hp + *hn;
				*hn += z;

				if (h)
				{
					x[h] = i;
					r.bits = (Byte)l;
					r.exop = (Byte)j;
					j = i >> (w - l);
					r.base = (uInt)(q - u[h - 1] - j);
					u[h - 1][j] = r;
				}
				else
					*t = q;
			}

			r.bits = (Byte)(k - w);
			if (p >= v + n)
				r.exop = 128 + 64;
			else if (*p < s)
			{
				r.exop = (Byte)(*p < 256 ? 0 : 32 + 64);
				r.base = *p++;
			}
			else
			{
				r.exop = (Byte)(e[*p - s] + 16 + 64);
				r.base = d[*p++ - s];
			}

			f = 1 << (k - w);
			for (j = i >> w; j < z; j += f)
				q[j] = r;

			for (j = 1 << (k - 1); i & j; j >>= 1)
				i ^= j;
			i ^= j;

			mask = (1 << w) - 1;
			while ((i & mask) != x[h])
			{
				h--;
				w -= l;
				mask = (1 << w) - 1;
			}
		}
	}

	return y != 0 && g != 1 ? Z_BUF_ERROR : Z_OK;
}

int inflate_trees_bits(uIntf* c, uIntf* bb, inflate_huft * FAR * tb,
	inflate_huft* hp, z_streamp z)
{
	int r;
	uInt hn = 0;
	uIntf* v;

	if ((v = (uIntf*)ZALLOC(z, 19, sizeof(uInt))) == Z_NULL)
		return Z_MEM_ERROR;
	r = huft_build(c, 19, 19, (uIntf*)Z_NULL, (uIntf*)Z_NULL, tb, bb, hp, &hn,
		v);
	if (r == Z_DATA_ERROR)
		z->msg = (char*)"oversubscribed dynamic bit lengths tree";
	else if (r == Z_BUF_ERROR || *bb == 0)
	{
		z->msg = (char*)"incomplete dynamic bit lengths tree";
		r = Z_DATA_ERROR;
	}
	ZFREE(z, v);
	return r;
}

int inflate_trees_dynamic(uInt nl, uInt nd, uIntf* c, uIntf* bl, uIntf* bd,
	inflate_huft * FAR * tl, inflate_huft * FAR * td, inflate_huft* hp,
	z_streamp z)
{
	int r;
	uInt hn = 0;
	uIntf* v;

	if ((v = (uIntf*)ZALLOC(z, 288, sizeof(uInt))) == Z_NULL)
		return Z_MEM_ERROR;

	r = huft_build(c, nl, 257, cplens, cplext, tl, bl, hp, &hn, v);
	if (r != Z_OK || *bl == 0)
	{
		if (r == Z_DATA_ERROR)
			z->msg = (char*)"oversubscribed literal/length tree";
		else if (r != Z_MEM_ERROR)
		{
			z->msg = (char*)"incomplete literal/length tree";
			r = Z_DATA_ERROR;
		}
		ZFREE(z, v);
		return r;
	}

	r = huft_build(c + nl, nd, 0, cpdist, cpdext, td, bd, hp, &hn, v);
	if (r != Z_OK || (*bd == 0 && nl > 257))
	{
		if (r == Z_DATA_ERROR)
			z->msg = (char*)"oversubscribed distance tree";
		else if (r == Z_BUF_ERROR)
		{
			z->msg = (char*)"incomplete distance tree";
			r = Z_DATA_ERROR;
		}
		else if (r != Z_MEM_ERROR)
		{
			z->msg = (char*)"empty distance tree with lengths";
			r = Z_DATA_ERROR;
		}
		ZFREE(z, v);
		return r;
	}

	ZFREE(z, v);
	return Z_OK;
}

int inflate_trees_fixed(uIntf* bl, uIntf* bd, const inflate_huft * FAR * tl,
	const inflate_huft * FAR * td, z_streamp z)
{
	*bl = fixed_bl;
	*bd = fixed_bd;
	*tl = fixed_tl;
	*td = fixed_td;
	return Z_OK;
}

#define GRABBITS(j) \
	{ \
		while (k < (j)) \
		{ \
			b |= ((uLong)NEXTBYTE) << k; \
			k += 8; \
		} \
	}
#define UNGRAB \
	{ \
		c = z->avail_in - n; \
		c = (k >> 3) < c ? k >> 3 : c; \
		n += c; \
		p -= c; \
		k -= c << 3; \
	}

int inflate_fast(uInt bl, uInt bd, const inflate_huft* tl,
	const inflate_huft* td, inflate_blocks_statef* s, z_streamp z)
{
	const inflate_huft* t;
	uInt e;
	uLong b;
	uInt k;
	Bytef* p;
	uInt n;
	Bytef* q;
	uInt m;
	uInt ml;
	uInt md;
	uInt c;
	uInt d;
	Bytef* r;

	LOAD

		ml = inflate_mask[bl];
	md = inflate_mask[bd];

	do
	{
		GRABBITS(20)
		if ((e = (t = tl + ((uInt)b & ml))->exop) == 0)
		{
			DUMPBITS(t->bits)
			*q++ = (Byte)t->base;
			m--;
			continue;
		}
		do
		{
			DUMPBITS(t->bits)
			if (e & 16)
			{
				e &= 15;
				c = t->base + ((uInt)b & inflate_mask[e]);
				DUMPBITS(e)

				GRABBITS(15);
				e = (t = td + ((uInt)b & md))->exop;
				do
				{
					DUMPBITS(t->bits)
					if (e & 16)
					{
						e &= 15;
						GRABBITS(e)
						d = t->base + ((uInt)b & inflate_mask[e]);
						DUMPBITS(e)

						m -= c;
						if ((uInt)(q - s->window) >= d)
						{
							r = q - d;
							*q++ = *r++;
							c--;
							*q++ = *r++;
							c--;
						}
						else
						{
							e = d - (uInt)(q - s->window);
							r = s->end - e;
							if (c > e)
							{
								c -= e;
								do
								{
									*q++ = *r++;
								} while (--e);
								r = s->window;
							}
						}
						do
						{
							*q++ = *r++;
						} while (--c);
						break;
					}
					else if ((e & 64) == 0)
					{
						t += t->base;
						e = (t += ((uInt)b & inflate_mask[e]))->exop;
					}
					else
					{
						z->msg = (char*)"invalid distance code";
						UNGRAB
						UPDATE
						return Z_DATA_ERROR;
					}
				} while (1);
				break;
			}
			if ((e & 64) == 0)
			{
				t += t->base;
				if ((e = (t += ((uInt)b & inflate_mask[e]))->exop) == 0)
				{
					DUMPBITS(t->bits)
					*q++ = (Byte)t->base;
					m--;
					break;
				}
			}
			else if (e & 32)
			{
				UNGRAB
				UPDATE
				return Z_STREAM_END;
			}
			else
			{
				z->msg = (char*)"invalid literal/length code";
				UNGRAB
				UPDATE
				return Z_DATA_ERROR;
			}
		} while (1);
	} while (m >= 258 && n >= 10);

	UNGRAB
	UPDATE
	return Z_OK;
}

static const uLong crc_table[256] = { 0x00000000L, 0x77073096L, 0xee0e612cL,
	0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
	0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL,
	0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L, 0x1db71064L, 0x6ab020f2L,
	0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L,
	0x83d385c7L, 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
	0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L,
	0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L,
	0xd20d85fdL, 0xa50ab56bL, 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L,
	0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
	0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L,
	0x56b3c423L, 0xcfba9599L, 0xb8bda50fL, 0x2802b89eL, 0x5f058808L,
	0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL,
	0xb6662d3dL, 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
	0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L,
	0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL,
	0x91646c97L, 0xe6635c01L, 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L,
	0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
	0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL,
	0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L, 0x4db26158L, 0x3ab551ceL,
	0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL,
	0xd3d6f4fbL, 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
	0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL,
	0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L,
	0xb966d409L, 0xce61e49fL, 0x5edef90eL, 0x29d9c998L, 0xb0d09822L,
	0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
	0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L,
	0x9dd277afL, 0x04db2615L, 0x73dc1683L, 0xe3630b12L, 0x94643b84L,
	0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L,
	0x7d079eb1L, 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
	0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L, 0xfed41b76L,
	0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L,
	0x17b7be43L, 0x60b08ed5L, 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L,
	0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
	0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L,
	0xa867df55L, 0x316e8eefL, 0x4669be79L, 0xcb61b38cL, 0xbc66831aL,
	0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L,
	0x5505262fL, 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
	0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L,
	0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL,
	0x72076785L, 0x05005713L, 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL,
	0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
	0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL,
	0xf6b9265bL, 0x6fb077e1L, 0x18b74777L, 0x88085ae6L, 0xff0f6a70L,
	0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L,
	0x166ccf45L, 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
	0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL,
	0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L,
	0x47b2cf7fL, 0x30b5ffe9L, 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L,
	0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
	0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L,
	0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL };

const uLongf* get_crc_table(void)
{
	return (const uLongf*)crc_table;
}

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

uLong ucrc32(uLong crc, const Bytef* buf, uInt len)
{
	if (buf == Z_NULL)
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

#define BASE 65521L
#define NMAX 5552

#define AD1(buf, i) \
	{ \
		s1 += buf[i]; \
		s2 += s1; \
	}
#define AD2(buf, i) \
	AD1(buf, i); \
	AD1(buf, i + 1);
#define AD4(buf, i) \
	AD2(buf, i); \
	AD2(buf, i + 2);
#define AD8(buf, i) \
	AD4(buf, i); \
	AD4(buf, i + 4);
#define AD16(buf) \
	AD8(buf, 0); \
	AD8(buf, 8);

uLong adler32(uLong adler, const Bytef* buf, uInt len)
{
	unsigned long s1 = adler & 0xffff;
	unsigned long s2 = (adler >> 16) & 0xffff;
	int k;

	if (buf == Z_NULL)
		return 1L;

	while (len > 0)
	{
		k = len < NMAX ? len : NMAX;
		len -= k;
		while (k >= 16)
		{
			AD16(buf);
			buf += 16;
			k -= 16;
		}
		if (k != 0)
			do
			{
				s1 += *buf++;
				s2 += s1;
			} while (--k);
		s1 %= BASE;
		s2 %= BASE;
	}
	return (s2 << 16) | s1;
}

const char* zlibVersion(void)
{
	return ZLIB_VERSION;
}

const char* zError(int err)
{
	return ERR_MSG(err);
}

voidpf zcalloc(voidpf opaque, unsigned items, unsigned size)
{
	if (opaque)
		items += size - size;
	return (voidpf)calloc(items, size);
}

void zcfree(voidpf opaque, voidpf ptr)
{
	free(ptr);
}

int inflateReset(z_streamp z)
{
	if (z == Z_NULL || z->state == Z_NULL)
		return Z_STREAM_ERROR;
	z->total_in = z->total_out = 0;
	z->msg = Z_NULL;
	z->state->mode = z->state->nowrap ? IM_BLOCKS : IM_METHOD;
	inflate_blocks_reset(z->state->blocks, z, Z_NULL);
	return Z_OK;
}

int inflateEnd(z_streamp z)
{
	if (z == Z_NULL || z->state == Z_NULL || z->zfree == Z_NULL)
		return Z_STREAM_ERROR;
	if (z->state->blocks != Z_NULL)
		inflate_blocks_free(z->state->blocks, z);
	ZFREE(z, z->state);
	z->state = Z_NULL;
	return Z_OK;
}

int inflateInit2(z_streamp z)
{
	int w = -MAX_WBITS;

	if (z == Z_NULL)
		return Z_STREAM_ERROR;
	z->msg = Z_NULL;
	if (z->zalloc == Z_NULL)
	{
		z->zalloc = zcalloc;
		z->opaque = (voidpf)0;
	}
	if (z->zfree == Z_NULL)
		z->zfree = zcfree;
	if ((z->state = (struct internal_state FAR*)ZALLOC(z, 1,
			 sizeof(struct internal_state))) == Z_NULL)
		return Z_MEM_ERROR;
	z->state->blocks = Z_NULL;

	z->state->nowrap = 0;
	if (w < 0)
	{
		w = -w;
		z->state->nowrap = 1;
	}

	if (w < 8 || w > 15)
	{
		inflateEnd(z);
		return Z_STREAM_ERROR;
	}
	z->state->wbits = (uInt)w;

	if ((z->state->blocks = inflate_blocks_new(z,
			 z->state->nowrap ? Z_NULL : adler32, (uInt)1 << w)) == Z_NULL)
	{
		inflateEnd(z);
		return Z_MEM_ERROR;
	}

	inflateReset(z);
	return Z_OK;
}

#define NEEDBYTE_ \
	{ \
		if (z->avail_in == 0) \
			return r; \
		r = f; \
	}
#define NEXTBYTE_ (z->avail_in--, z->total_in++, *z->next_in++)

int inflate(z_streamp z, int f)
{
	int r;
	uInt b;

	if (z == Z_NULL || z->state == Z_NULL || z->next_in == Z_NULL)
		return Z_STREAM_ERROR;
	f = f == Z_FINISH ? Z_BUF_ERROR : Z_OK;
	r = Z_BUF_ERROR;
	while (1)
		switch (z->state->mode)
		{
		case IM_METHOD:
			NEEDBYTE_
			if (((z->state->sub.method = NEXTBYTE_) & 0xf) != Z_DEFLATED)
			{
				z->state->mode = IM_BAD;
				z->msg = (char*)"unknown compression method";
				z->state->sub.marker = 5;
				break;
			}
			if ((z->state->sub.method >> 4) + 8 > z->state->wbits)
			{
				z->state->mode = IM_BAD;
				z->msg = (char*)"invalid window size";
				z->state->sub.marker = 5;
				break;
			}
			z->state->mode = IM_FLAG;
		case IM_FLAG:
			NEEDBYTE_
			b = NEXTBYTE_;
			if (((z->state->sub.method << 8) + b) % 31)
			{
				z->state->mode = IM_BAD;
				z->msg = (char*)"incorrect header check";
				z->state->sub.marker = 5;
				break;
			}
			if (!(b & PRESET_DICT))
			{
				z->state->mode = IM_BLOCKS;
				break;
			}
			z->state->mode = IM_DICT4;
		case IM_DICT4:
			NEEDBYTE_
			z->state->sub.check.need = (uLong)NEXTBYTE_ << 24;
			z->state->mode = IM_DICT3;
		case IM_DICT3:
			NEEDBYTE_
			z->state->sub.check.need += (uLong)NEXTBYTE_ << 16;
			z->state->mode = IM_DICT2;
		case IM_DICT2:
			NEEDBYTE_
			z->state->sub.check.need += (uLong)NEXTBYTE_ << 8;
			z->state->mode = IM_DICT1;
		case IM_DICT1:
			NEEDBYTE_
			z->state->sub.check.need += (uLong)NEXTBYTE_;
			z->adler = z->state->sub.check.need;
			z->state->mode = IM_DICT0;
			return Z_NEED_DICT;
		case IM_DICT0:
			z->state->mode = IM_BAD;
			z->msg = (char*)"need dictionary";
			z->state->sub.marker = 0;
			return Z_STREAM_ERROR;
		case IM_BLOCKS:
			r = inflate_blocks(z->state->blocks, z, r);
			if (r == Z_DATA_ERROR)
			{
				z->state->mode = IM_BAD;
				z->state->sub.marker = 0;
				break;
			}
			if (r == Z_OK)
				r = f;
			if (r != Z_STREAM_END)
				return r;
			r = f;
			inflate_blocks_reset(z->state->blocks, z, &z->state->sub.check.was);
			if (z->state->nowrap)
			{
				z->state->mode = IM_DONE;
				break;
			}
			z->state->mode = IM_CHECK4;
		case IM_CHECK4:
			NEEDBYTE_
			z->state->sub.check.need = (uLong)NEXTBYTE_ << 24;
			z->state->mode = IM_CHECK3;
		case IM_CHECK3:
			NEEDBYTE_
			z->state->sub.check.need += (uLong)NEXTBYTE_ << 16;
			z->state->mode = IM_CHECK2;
		case IM_CHECK2:
			NEEDBYTE_
			z->state->sub.check.need += (uLong)NEXTBYTE_ << 8;
			z->state->mode = IM_CHECK1;
		case IM_CHECK1:
			NEEDBYTE_
			z->state->sub.check.need += (uLong)NEXTBYTE_;

			if (z->state->sub.check.was != z->state->sub.check.need)
			{
				z->state->mode = IM_BAD;
				z->msg = (char*)"incorrect data check";
				z->state->sub.marker = 5;
				break;
			}
			z->state->mode = IM_DONE;
		case IM_DONE:
			return Z_STREAM_END;
		case IM_BAD:
			return Z_DATA_ERROR;
		default:
			return Z_STREAM_ERROR;
		}
}

extern const char unz_copyright[] =
	" unzip 0.15 Copyright 1998 Gilles Vollant ";

LUFILE* lufopen(void* z, unsigned int len, DWORD flags, ZRESULT* err)
{
	if (flags != ZIP_HANDLE && flags != ZIP_FILENAME && flags != ZIP_MEMORY)
	{
		*err = ZR_ARGS;
		return NULL;
	}

	HANDLE h = 0;
	bool canseek = false;
	*err = ZR_OK;
	if (flags == ZIP_HANDLE || flags == ZIP_FILENAME)
	{
		if (flags == ZIP_HANDLE)
		{
			HANDLE hf = z;
			BOOL res = DuplicateHandle(GetCurrentProcess(), hf,
				GetCurrentProcess(), &h, 0, FALSE, DUPLICATE_SAME_ACCESS);
			if (!res)
			{
				*err = ZR_NODUPH;
				return NULL;
			}
		}
		else
		{
			h = CreateFile((const char*)z, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h == INVALID_HANDLE_VALUE)
			{
				*err = ZR_NOFILE;
				return NULL;
			}
		}
		DWORD type = GetFileType(h);
		canseek = (type == FILE_TYPE_DISK);
	}
	LUFILE* lf = new LUFILE;
	if (flags == ZIP_HANDLE || flags == ZIP_FILENAME)
	{
		lf->is_handle = true;
		lf->canseek = canseek;
		lf->h = h;
		lf->herr = false;
		lf->initial_offset = 0;
		if (canseek)
			lf->initial_offset = SetFilePointer(h, 0, NULL, FILE_CURRENT);
	}
	else
	{
		lf->is_handle = false;
		lf->canseek = true;
		lf->buf = z;
		lf->len = len;
		lf->pos = 0;
		lf->initial_offset = 0;
	}
	*err = ZR_OK;
	return lf;
}

int lufclose(LUFILE* stream)
{
	if (stream == NULL)
		return EOF;
	if (stream->is_handle)
		CloseHandle(stream->h);
	delete stream;
	return 0;
}

int luferror(LUFILE* stream)
{
	if (stream->is_handle && stream->herr)
		return 1;
	else
		return 0;
}

long int luftell(LUFILE* stream)
{
	if (stream->is_handle && stream->canseek)
		return SetFilePointer(stream->h, 0, NULL, FILE_CURRENT) -
			stream->initial_offset;
	else if (stream->is_handle)
		return 0;
	else
		return stream->pos;
}

int lufseek(LUFILE* stream, long offset, int whence)
{
	if (stream->is_handle)
	{
		if (stream->canseek)
		{
			if (whence == SEEK_SET)
				SetFilePointer(stream->h, stream->initial_offset + offset, NULL,
					FILE_BEGIN);
			else if (whence == SEEK_CUR)
				SetFilePointer(stream->h, offset, NULL, FILE_CURRENT);
			else if (whence == SEEK_END)
				SetFilePointer(stream->h, offset, NULL, FILE_END);
			else
				return 19;
			return 0;
		}
		return 29;
	}
	else
	{
		if (whence == SEEK_SET)
			stream->pos = offset;
		else if (whence == SEEK_CUR)
			stream->pos += offset;
		else if (whence == SEEK_END)
			stream->pos = stream->len + offset;
		return 0;
	}
}

unsigned int lufread(void* ptr, unsigned int size, unsigned int n,
	LUFILE* stream)
{
	unsigned int red = size * n;
	if (stream->is_handle)
	{
		DWORD red2;
		BOOL res = ReadFile(stream->h, ptr, red, &red2, NULL);
		if (!res)
			stream->herr = true;
		return red2 / size;
	}
	if (stream->pos + red > stream->len)
		red = stream->len - stream->pos;
	memcpy(ptr, (char*)stream->buf + stream->pos, red);
	stream->pos += red;
	return red / size;
}

int unzlocal_getByte(LUFILE* fin, int* pi)
{
	unsigned char c;
	int err = (int)lufread(&c, 1, 1, fin);
	if (err == 1)
	{
		*pi = (int)c;
		return UNZ_OK;
	}
	else
	{
		if (luferror(fin))
			return UNZ_ERRNO;
		else
			return UNZ_EOF;
	}
}

int unzlocal_getShort(LUFILE* fin, uLong* pX)
{
	uLong x;
	int i;
	int err;

	err = unzlocal_getByte(fin, &i);
	x = (uLong)i;
	if (err == UNZ_OK)
		err = unzlocal_getByte(fin, &i);
	x += ((uLong)i) << 8;

	if (err == UNZ_OK)
		*pX = x;
	else
		*pX = 0;
	return err;
}

int unzlocal_getLong(LUFILE* fin, uLong* pX)
{
	uLong x;
	int i;
	int err;

	err = unzlocal_getByte(fin, &i);
	x = (uLong)i;

	if (err == UNZ_OK)
		err = unzlocal_getByte(fin, &i);
	x += ((uLong)i) << 8;

	if (err == UNZ_OK)
		err = unzlocal_getByte(fin, &i);
	x += ((uLong)i) << 16;

	if (err == UNZ_OK)
		err = unzlocal_getByte(fin, &i);
	x += ((uLong)i) << 24;

	if (err == UNZ_OK)
		*pX = x;
	else
		*pX = 0;
	return err;
}

int strcmpcasenosensitive_internal(const char* fileName1, const char* fileName2)
{
	for (;;)
	{
		char c1 = *(fileName1++);
		char c2 = *(fileName2++);
		if ((c1 >= 'a') && (c1 <= 'z'))
			c1 -= 0x20;
		if ((c2 >= 'a') && (c2 <= 'z'))
			c2 -= 0x20;
		if (c1 == '\0')
			return ((c2 == '\0') ? 0 : -1);
		if (c2 == '\0')
			return 1;
		if (c1 < c2)
			return -1;
		if (c1 > c2)
			return 1;
	}
}

int unzStringFileNameCompare(const char* fileName1, const char* fileName2,
	int iCaseSensitivity)
{
	if (iCaseSensitivity == 1)
		return strcmp(fileName1, fileName2);
	else
		return strcmpcasenosensitive_internal(fileName1, fileName2);
}

#define BUFREADCOMMENT (0x400)

uLong unzlocal_SearchCentralDir(LUFILE* fin)
{
	if (lufseek(fin, 0, SEEK_END) != 0)
		return 0;
	uLong uSizeFile = luftell(fin);

	uLong uMaxBack = 0xffff;
	if (uMaxBack > uSizeFile)
		uMaxBack = uSizeFile;

	unsigned char* buf = (unsigned char*)malloc(BUFREADCOMMENT + 4);
	if (buf == NULL)
		return 0;
	uLong uPosFound = 0;

	uLong uBackRead = 4;
	while (uBackRead < uMaxBack)
	{
		uLong uReadSize, uReadPos;
		int i;
		if (uBackRead + BUFREADCOMMENT > uMaxBack)
			uBackRead = uMaxBack;
		else
			uBackRead += BUFREADCOMMENT;
		uReadPos = uSizeFile - uBackRead;
		uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos))
			? (BUFREADCOMMENT + 4)
			: (uSizeFile - uReadPos);
		if (lufseek(fin, uReadPos, SEEK_SET) != 0)
			break;
		if (lufread(buf, (uInt)uReadSize, 1, fin) != 1)
			break;
		for (i = (int)uReadSize - 3; (i--) > 0;)
			if (((*(buf + i)) == 0x50) && ((*(buf + i + 1)) == 0x4b) &&
				((*(buf + i + 2)) == 0x05) && ((*(buf + i + 3)) == 0x06))
			{
				uPosFound = uReadPos + i;
				break;
			}
		if (uPosFound != 0)
			break;
	}
	free(buf);
	return uPosFound;
}

unzFile unzOpenInternal(LUFILE* fin)
{
	unz_s us;
	unz_s* s;
	uLong central_pos, uL;

	uLong number_disk;
	uLong number_disk_with_CD;
	uLong number_entry_CD;

	if (fin == NULL)
		return NULL;

	int err = UNZ_OK;

	central_pos = unzlocal_SearchCentralDir(fin);
	if (central_pos == 0)
		err = UNZ_ERRNO;
	if (lufseek(fin, central_pos, SEEK_SET) != 0)
		err = UNZ_ERRNO;

	if (unzlocal_getLong(fin, &uL) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(fin, &number_disk) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(fin, &number_disk_with_CD) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(fin, &us.gi.number_entry) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(fin, &number_entry_CD) != UNZ_OK)
		err = UNZ_ERRNO;

	if ((number_entry_CD != us.gi.number_entry) || (number_disk_with_CD != 0) ||
		(number_disk != 0))
		err = UNZ_BADZIPFILE;

	if (unzlocal_getLong(fin, &us.size_central_dir) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getLong(fin, &us.offset_central_dir) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(fin, &us.gi.size_comment) != UNZ_OK)
		err = UNZ_ERRNO;

	if ((central_pos + fin->initial_offset <
			us.offset_central_dir + us.size_central_dir) &&
		(err == UNZ_OK))
		err = UNZ_BADZIPFILE;

	if (err != UNZ_OK)
	{
#pragma inline_depth(0)
		lufclose(fin);
#pragma inline_depth()
		return NULL;
	}

	us.file = fin;
	us.byte_before_the_zipfile = central_pos + fin->initial_offset -
		(us.offset_central_dir + us.size_central_dir);
	us.central_pos = central_pos;
	us.pfile_in_zip_read = NULL;
	fin->initial_offset = 0;

	s = (unz_s*)malloc(sizeof(unz_s));
	*s = us;
	unzGoToFirstFile((unzFile)s);
	return (unzFile)s;
}

int unzClose(unzFile file)
{
	unz_s* s;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;

	if (s->pfile_in_zip_read != NULL)
		unzCloseCurrentFile(file);

	lufclose(s->file);
	free(s);
	return UNZ_OK;
}

int unzGetGlobalInfo(unzFile file, unz_global_info* pglobal_info)
{
	unz_s* s;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	*pglobal_info = s->gi;
	return UNZ_OK;
}

void unzlocal_DosDateToTmuDate(uLong ulDosDate, tm_unz* ptm)
{
	uLong uDate;
	uDate = (uLong)(ulDosDate >> 16);
	ptm->tm_mday = (uInt)(uDate & 0x1f);
	ptm->tm_mon = (uInt)((((uDate) & 0x1E0) / 0x20) - 1);
	ptm->tm_year = (uInt)(((uDate & 0x0FE00) / 0x0200) + 1980);

	ptm->tm_hour = (uInt)((ulDosDate & 0xF800) / 0x800);
	ptm->tm_min = (uInt)((ulDosDate & 0x7E0) / 0x20);
	ptm->tm_sec = (uInt)(2 * (ulDosDate & 0x1f));
}

int unzlocal_GetCurrentFileInfoInternal(unzFile file, unz_file_info* pfile_info,
	unz_file_info_internal* pfile_info_internal, char* szFileName,
	uLong fileNameBufferSize, void* extraField, uLong extraFieldBufferSize,
	char* szComment, uLong commentBufferSize)
{
	unz_s* s;
	unz_file_info file_info;
	unz_file_info_internal file_info_internal;
	int err = UNZ_OK;
	uLong uMagic;
	long lSeek = 0;

	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	if (lufseek(s->file, s->pos_in_central_dir + s->byte_before_the_zipfile,
			SEEK_SET) != 0)
		err = UNZ_ERRNO;

	if (err == UNZ_OK)
		if (unzlocal_getLong(s->file, &uMagic) != UNZ_OK)
			err = UNZ_ERRNO;
		else if (uMagic != 0x02014b50)
			err = UNZ_BADZIPFILE;

	if (unzlocal_getShort(s->file, &file_info.version) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(s->file, &file_info.version_needed) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(s->file, &file_info.flag) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(s->file, &file_info.compression_method) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getLong(s->file, &file_info.dosDate) != UNZ_OK)
		err = UNZ_ERRNO;

	unzlocal_DosDateToTmuDate(file_info.dosDate, &file_info.tmu_date);

	if (unzlocal_getLong(s->file, &file_info.crc) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getLong(s->file, &file_info.compressed_size) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getLong(s->file, &file_info.uncompressed_size) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(s->file, &file_info.size_filename) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(s->file, &file_info.size_file_extra) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(s->file, &file_info.size_file_comment) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(s->file, &file_info.disk_num_start) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getShort(s->file, &file_info.internal_fa) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getLong(s->file, &file_info.external_fa) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getLong(s->file, &file_info_internal.offset_curfile) != UNZ_OK)
		err = UNZ_ERRNO;

	lSeek += file_info.size_filename;
	if ((err == UNZ_OK) && (szFileName != NULL))
	{
		uLong uSizeRead;
		if (file_info.size_filename < fileNameBufferSize)
		{
			*(szFileName + file_info.size_filename) = '\0';
			uSizeRead = file_info.size_filename;
		}
		else
			uSizeRead = fileNameBufferSize;

		if ((file_info.size_filename > 0) && (fileNameBufferSize > 0))
			if (lufread(szFileName, (uInt)uSizeRead, 1, s->file) != 1)
				err = UNZ_ERRNO;
		lSeek -= uSizeRead;
	}

	if ((err == UNZ_OK) && (extraField != NULL))
	{
		uLong uSizeRead;
		if (file_info.size_file_extra < extraFieldBufferSize)
			uSizeRead = file_info.size_file_extra;
		else
			uSizeRead = extraFieldBufferSize;

		if (lSeek != 0)
			if (lufseek(s->file, lSeek, SEEK_CUR) == 0)
				lSeek = 0;
			else
				err = UNZ_ERRNO;
		if ((file_info.size_file_extra > 0) && (extraFieldBufferSize > 0))
			if (lufread(extraField, (uInt)uSizeRead, 1, s->file) != 1)
				err = UNZ_ERRNO;
		lSeek += file_info.size_file_extra - uSizeRead;
	}
	else
		lSeek += file_info.size_file_extra;

	if ((err == UNZ_OK) && (szComment != NULL))
	{
		uLong uSizeRead;
		if (file_info.size_file_comment < commentBufferSize)
		{
			*(szComment + file_info.size_file_comment) = '\0';
			uSizeRead = file_info.size_file_comment;
		}
		else
			uSizeRead = commentBufferSize;

		if (lSeek != 0)
			if (lufseek(s->file, lSeek, SEEK_CUR) == 0)
				lSeek = 0;
			else
				err = UNZ_ERRNO;
		if ((file_info.size_file_comment > 0) && (commentBufferSize > 0))
			if (lufread(szComment, (uInt)uSizeRead, 1, s->file) != 1)
				err = UNZ_ERRNO;
		lSeek += file_info.size_file_comment - uSizeRead;
	}
	else
		lSeek += file_info.size_file_comment;

	if ((err == UNZ_OK) && (pfile_info != NULL))
		*pfile_info = file_info;

	if ((err == UNZ_OK) && (pfile_info_internal != NULL))
		*pfile_info_internal = file_info_internal;

	return err;
}

int unzGetCurrentFileInfo(unzFile file, unz_file_info* pfile_info,
	char* szFileName, uLong fileNameBufferSize, void* extraField,
	uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize)
{
	return unzlocal_GetCurrentFileInfoInternal(file, pfile_info, NULL,
		szFileName, fileNameBufferSize, extraField, extraFieldBufferSize,
		szComment, commentBufferSize);
}

int unzGoToFirstFile(unzFile file)
{
	int err = UNZ_OK;
	unz_s* s;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	s->pos_in_central_dir = s->offset_central_dir;
	s->num_file = 0;
	err = unzlocal_GetCurrentFileInfoInternal(file, &s->cur_file_info,
		&s->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);
	s->current_file_ok = (err == UNZ_OK);
	return err;
}

int unzGoToNextFile(unzFile file)
{
	unz_s* s;
	int err;

	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	if (!s->current_file_ok)
		return UNZ_END_OF_LIST_OF_FILE;
	if (s->num_file + 1 == s->gi.number_entry)
		return UNZ_END_OF_LIST_OF_FILE;

	s->pos_in_central_dir += SIZECENTRALDIRITEM +
		s->cur_file_info.size_filename + s->cur_file_info.size_file_extra +
		s->cur_file_info.size_file_comment;
	s->num_file++;
	err = unzlocal_GetCurrentFileInfoInternal(file, &s->cur_file_info,
		&s->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);
	s->current_file_ok = (err == UNZ_OK);
	return err;
}

int unzLocateFile(unzFile file, const char* szFileName, int iCaseSensitivity)
{
	unz_s* s;
	int err;

	uLong num_fileSaved;
	uLong pos_in_central_dirSaved;

	if (file == NULL)
		return UNZ_PARAMERROR;
	if (strlen(szFileName) >= UNZ_MAXFILENAMEINZIP)
		return UNZ_PARAMERROR;
	char szFileNameToFind[MAX_PATH];
	strcpy(szFileNameToFind, szFileName);

	s = (unz_s*)file;
	if (!s->current_file_ok)
		return UNZ_END_OF_LIST_OF_FILE;

	num_fileSaved = s->num_file;
	pos_in_central_dirSaved = s->pos_in_central_dir;

	err = unzGoToFirstFile(file);

	while (err == UNZ_OK)
	{
		char szCurrentFileName[UNZ_MAXFILENAMEINZIP + 1];
		unzGetCurrentFileInfo(file, NULL, szCurrentFileName,
			sizeof(szCurrentFileName) - 1, NULL, 0, NULL, 0);
		if (unzStringFileNameCompare(szCurrentFileName, szFileNameToFind,
				iCaseSensitivity) == 0)
			return UNZ_OK;
		err = unzGoToNextFile(file);
	}

	s->num_file = num_fileSaved;
	s->pos_in_central_dir = pos_in_central_dirSaved;
	return err;
}

int unzlocal_CheckCurrentFileCoherencyHeader(unz_s* s, uInt* piSizeVar,
	uLong* poffset_local_extrafield, uInt* psize_local_extrafield)
{
	uLong uMagic, uData, uFlags;
	uLong size_filename;
	uLong size_extra_field;
	int err = UNZ_OK;

	*piSizeVar = 0;
	*poffset_local_extrafield = 0;
	*psize_local_extrafield = 0;

	if (lufseek(s->file,
			s->cur_file_info_internal.offset_curfile +
				s->byte_before_the_zipfile,
			SEEK_SET) != 0)
		return UNZ_ERRNO;

	if (err == UNZ_OK)
		if (unzlocal_getLong(s->file, &uMagic) != UNZ_OK)
			err = UNZ_ERRNO;
		else if (uMagic != 0x04034b50)
			err = UNZ_BADZIPFILE;

	if (unzlocal_getShort(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &uFlags) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.compression_method))
		err = UNZ_BADZIPFILE;

	if ((err == UNZ_OK) && (s->cur_file_info.compression_method != 0) &&
		(s->cur_file_info.compression_method != Z_DEFLATED))
		err = UNZ_BADZIPFILE;

	if (unzlocal_getLong(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzlocal_getLong(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.crc) &&
		((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;

	if (unzlocal_getLong(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.compressed_size) &&
		((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;

	if (unzlocal_getLong(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.uncompressed_size) &&
		((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;

	if (unzlocal_getShort(s->file, &size_filename) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) &&
		(size_filename != s->cur_file_info.size_filename))
		err = UNZ_BADZIPFILE;

	*piSizeVar += (uInt)size_filename;

	if (unzlocal_getShort(s->file, &size_extra_field) != UNZ_OK)
		err = UNZ_ERRNO;
	*poffset_local_extrafield = s->cur_file_info_internal.offset_curfile +
		SIZEZIPLOCALHEADER + size_filename;
	*psize_local_extrafield = (uInt)size_extra_field;

	*piSizeVar += (uInt)size_extra_field;

	return err;
}

int unzOpenCurrentFile(unzFile file)
{
	int err;
	int Store;
	uInt iSizeVar;
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	uLong offset_local_extrafield;
	uInt size_local_extrafield;

	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	if (!s->current_file_ok)
		return UNZ_PARAMERROR;

	if (s->pfile_in_zip_read != NULL)
		unzCloseCurrentFile(file);

	if (unzlocal_CheckCurrentFileCoherencyHeader(s, &iSizeVar,
			&offset_local_extrafield, &size_local_extrafield) != UNZ_OK)
		return UNZ_BADZIPFILE;

	pfile_in_zip_read_info =
		(file_in_zip_read_info_s*)malloc(sizeof(file_in_zip_read_info_s));
	if (pfile_in_zip_read_info == NULL)
		return UNZ_INTERNALERROR;

	pfile_in_zip_read_info->read_buffer = (char*)malloc(UNZ_BUFSIZE);
	pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
	pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
	pfile_in_zip_read_info->pos_local_extrafield = 0;

	if (pfile_in_zip_read_info->read_buffer == NULL)
	{
		free(pfile_in_zip_read_info);
		return UNZ_INTERNALERROR;
	}

	pfile_in_zip_read_info->stream_initialised = 0;

	Store = s->cur_file_info.compression_method == 0;

	pfile_in_zip_read_info->crc32_wait = s->cur_file_info.crc;
	pfile_in_zip_read_info->crc32 = 0;
	pfile_in_zip_read_info->compression_method =
		s->cur_file_info.compression_method;
	pfile_in_zip_read_info->file = s->file;
	pfile_in_zip_read_info->byte_before_the_zipfile =
		s->byte_before_the_zipfile;

	pfile_in_zip_read_info->stream.total_out = 0;

	if (!Store)
	{
		pfile_in_zip_read_info->stream.zalloc = (alloc_func)0;
		pfile_in_zip_read_info->stream.zfree = (free_func)0;
		pfile_in_zip_read_info->stream.opaque = (voidpf)0;

		err = inflateInit2(&pfile_in_zip_read_info->stream);
		if (err == Z_OK)
			pfile_in_zip_read_info->stream_initialised = 1;
	}
	pfile_in_zip_read_info->rest_read_compressed =
		s->cur_file_info.compressed_size;
	pfile_in_zip_read_info->rest_read_uncompressed =
		s->cur_file_info.uncompressed_size;
	pfile_in_zip_read_info->pos_in_zipfile =
		s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
		iSizeVar;
	pfile_in_zip_read_info->stream.avail_in = (uInt)0;

	s->pfile_in_zip_read = pfile_in_zip_read_info;
	return UNZ_OK;
}

int unzReadCurrentFile(unzFile file, voidp buf, unsigned len)
{
	int err = UNZ_OK;
	uInt iRead = 0;

	unz_s* s = (unz_s*)file;
	if (s == NULL)
		return UNZ_PARAMERROR;

	file_in_zip_read_info_s* pfile_in_zip_read_info = s->pfile_in_zip_read;
	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;
	if ((pfile_in_zip_read_info->read_buffer == NULL))
		return UNZ_END_OF_LIST_OF_FILE;
	if (len == 0)
		return 0;

	pfile_in_zip_read_info->stream.next_out = (Bytef*)buf;
	pfile_in_zip_read_info->stream.avail_out = (uInt)len;

	if (len > pfile_in_zip_read_info->rest_read_uncompressed)
		pfile_in_zip_read_info->stream.avail_out =
			(uInt)pfile_in_zip_read_info->rest_read_uncompressed;

	while (pfile_in_zip_read_info->stream.avail_out > 0)
	{
		if ((pfile_in_zip_read_info->stream.avail_in == 0) &&
			(pfile_in_zip_read_info->rest_read_compressed > 0))
		{
			uInt uReadThis = UNZ_BUFSIZE;
			if (pfile_in_zip_read_info->rest_read_compressed < uReadThis)
				uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
			if (uReadThis == 0)
				return UNZ_EOF;
			if (lufseek(pfile_in_zip_read_info->file,
					pfile_in_zip_read_info->pos_in_zipfile +
						pfile_in_zip_read_info->byte_before_the_zipfile,
					SEEK_SET) != 0)
				return UNZ_ERRNO;
			if (lufread(pfile_in_zip_read_info->read_buffer, uReadThis, 1,
					pfile_in_zip_read_info->file) != 1)
				return UNZ_ERRNO;
			pfile_in_zip_read_info->pos_in_zipfile += uReadThis;
			pfile_in_zip_read_info->rest_read_compressed -= uReadThis;
			pfile_in_zip_read_info->stream.next_in =
				(Bytef*)pfile_in_zip_read_info->read_buffer;
			pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
		}

		if (pfile_in_zip_read_info->compression_method == 0)
		{
			uInt uDoCopy, i;
			if (pfile_in_zip_read_info->stream.avail_out <
				pfile_in_zip_read_info->stream.avail_in)
				uDoCopy = pfile_in_zip_read_info->stream.avail_out;
			else
				uDoCopy = pfile_in_zip_read_info->stream.avail_in;
			for (i = 0; i < uDoCopy; i++)
				*(pfile_in_zip_read_info->stream.next_out + i) =
					*(pfile_in_zip_read_info->stream.next_in + i);
			pfile_in_zip_read_info->crc32 =
				ucrc32(pfile_in_zip_read_info->crc32,
					pfile_in_zip_read_info->stream.next_out, uDoCopy);
			pfile_in_zip_read_info->rest_read_uncompressed -= uDoCopy;
			pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
			pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
			pfile_in_zip_read_info->stream.next_out += uDoCopy;
			pfile_in_zip_read_info->stream.next_in += uDoCopy;
			pfile_in_zip_read_info->stream.total_out += uDoCopy;
			iRead += uDoCopy;
		}
		else
		{
			uLong uTotalOutBefore, uTotalOutAfter;
			const Bytef* bufBefore;
			uLong uOutThis;
			int flush = Z_SYNC_FLUSH;

			uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
			bufBefore = pfile_in_zip_read_info->stream.next_out;

			err = inflate(&pfile_in_zip_read_info->stream, flush);

			uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
			uOutThis = uTotalOutAfter - uTotalOutBefore;

			pfile_in_zip_read_info->crc32 = ucrc32(
				pfile_in_zip_read_info->crc32, bufBefore, (uInt)(uOutThis));

			pfile_in_zip_read_info->rest_read_uncompressed -= uOutThis;

			iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

			if (err == Z_STREAM_END)
				return (iRead == 0) ? UNZ_EOF : iRead;
			if (err != Z_OK)
				break;
		}
	}

	if (err == Z_OK)
		return iRead;
	return err;
}

long unztell(unzFile file)
{
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	pfile_in_zip_read_info = s->pfile_in_zip_read;

	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;

	return (long)pfile_in_zip_read_info->stream.total_out;
}

int unzeof(unzFile file)
{
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	pfile_in_zip_read_info = s->pfile_in_zip_read;

	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;

	if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
		return 1;
	else
		return 0;
}

int unzGetLocalExtrafield(unzFile file, voidp buf, unsigned len)
{
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	uInt read_now;
	uLong size_to_read;

	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	pfile_in_zip_read_info = s->pfile_in_zip_read;

	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;

	size_to_read = (pfile_in_zip_read_info->size_local_extrafield -
		pfile_in_zip_read_info->pos_local_extrafield);

	if (buf == NULL)
		return (int)size_to_read;

	if (len > size_to_read)
		read_now = (uInt)size_to_read;
	else
		read_now = (uInt)len;

	if (read_now == 0)
		return 0;

	if (lufseek(pfile_in_zip_read_info->file,
			pfile_in_zip_read_info->offset_local_extrafield +
				pfile_in_zip_read_info->pos_local_extrafield,
			SEEK_SET) != 0)
		return UNZ_ERRNO;

	if (lufread(buf, (uInt)size_to_read, 1, pfile_in_zip_read_info->file) != 1)
		return UNZ_ERRNO;

	return (int)read_now;
}

int unzCloseCurrentFile(unzFile file)
{
	int err = UNZ_OK;

	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	pfile_in_zip_read_info = s->pfile_in_zip_read;

	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;

	if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
	{
		if (pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
			err = UNZ_CRCERROR;
	}

	if (pfile_in_zip_read_info->read_buffer != 0)
	{
		free(pfile_in_zip_read_info->read_buffer);
		pfile_in_zip_read_info->read_buffer = 0;
	}
	pfile_in_zip_read_info->read_buffer = NULL;
	if (pfile_in_zip_read_info->stream_initialised)
		inflateEnd(&pfile_in_zip_read_info->stream);

	pfile_in_zip_read_info->stream_initialised = 0;
	free(pfile_in_zip_read_info);

	s->pfile_in_zip_read = NULL;

	return err;
}

int unzGetGlobalComment(unzFile file, char* szComment, uLong uSizeBuf)
{
	unz_s* s;
	uLong uReadThis;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;

	uReadThis = uSizeBuf;
	if (uReadThis > s->gi.size_comment)
		uReadThis = s->gi.size_comment;

	if (lufseek(s->file, s->central_pos + 22, SEEK_SET) != 0)
		return UNZ_ERRNO;

	if (uReadThis > 0)
	{
		*szComment = '\0';
		if (lufread(szComment, (uInt)uReadThis, 1, s->file) != 1)
			return UNZ_ERRNO;
	}

	if ((szComment != NULL) && (uSizeBuf > s->gi.size_comment))
		*(szComment + s->gi.size_comment) = '\0';
	return (int)uReadThis;
}

FILETIME timet2filetime(const time_t t)
{
	FILETIME ft;
	ZeroMemory(&ft, sizeof(ft));
	return ft;
}

TUnzip::TUnzip()
{
	uf = 0;
	currentfile = -1;
	czei = -1;
}

ZRESULT TUnzip::Open(void* z, unsigned int len, DWORD flags)
{
	if (uf != 0 || currentfile != -1)
		return ZR_NOTINITED;
	GetCurrentDirectory(MAX_PATH, rootdir);
	_tcscat(rootdir, _T("\\"));
	if (flags == ZIP_HANDLE)
	{
		DWORD type = GetFileType(z);
		if (type != FILE_TYPE_DISK)
			return ZR_SEEK;
	}
	ZRESULT e;
	LUFILE* f = lufopen(z, len, flags, &e);
	if (f == NULL)
		return e;
	uf = unzOpenInternal(f);
	return ZR_OK;
}

ZRESULT TUnzip::Get(int index, ZIPENTRY* ze)
{
	if (index < -1 || index >= (int)uf->gi.number_entry)
		return ZR_ARGS;
	if (currentfile != -1)
		unzCloseCurrentFile(uf);
	currentfile = -1;
	if (index == czei && index != -1)
	{
		memcpy(ze, &cze, sizeof(ZIPENTRY));
		return ZR_OK;
	}
	if (index == -1)
	{
		ze->index = uf->gi.number_entry;
		ze->name[0] = 0;
		ze->attr = 0;
		ze->atime.dwLowDateTime = 0;
		ze->atime.dwHighDateTime = 0;
		ze->ctime.dwLowDateTime = 0;
		ze->ctime.dwHighDateTime = 0;
		ze->mtime.dwLowDateTime = 0;
		ze->mtime.dwHighDateTime = 0;
		ze->comp_size = 0;
		ze->unc_size = 0;
		return ZR_OK;
	}
	if (index < (int)uf->num_file)
		unzGoToFirstFile(uf);
	while ((int)uf->num_file < index)
		unzGoToNextFile(uf);
	unz_file_info ufi;
	char fn[MAX_PATH];
	unzGetCurrentFileInfo(uf, &ufi, fn, MAX_PATH, NULL, 0, NULL, 0);

	unsigned int extralen, iSizeVar;
	unsigned long offset;
	int res = unzlocal_CheckCurrentFileCoherencyHeader(uf, &iSizeVar, &offset,
		&extralen);
	if (res != UNZ_OK)
		return ZR_CORRUPT;
	if (lufseek(uf->file, offset, SEEK_SET) != 0)
		return ZR_READ;
	unsigned char* extra = new unsigned char[extralen];
	if (lufread(extra, 1, (uInt)extralen, uf->file) != extralen)
	{
		delete[] extra;
		return ZR_READ;
	}

	ze->index = uf->num_file;
	char* name = fn;
	char c;
	do
	{
		c = *name;
		ze->name[name - fn] = c;
		name++;
	} while (c != 0);

	unsigned long a = ufi.external_fa;
	bool isdir = (a & 0x40000000) != 0;
	bool writable = (a & 0x00800000) != 0;
	bool hidden = false, system = false, archive = true;

	bool dosreadonly = (a & 0x00000001) != 0;
	bool doshidden = (a & 0x00000002) != 0;
	bool dossystem = (a & 0x00000004) != 0;
	bool dosdir = (a & 0x00000010) != 0;
	bool dosarchive = (a & 0x00000020) != 0;

	ze->attr = FILE_ATTRIBUTE_NORMAL;
	if (isdir || dosdir)
		ze->attr |= FILE_ATTRIBUTE_DIRECTORY;
	if (archive && dosarchive)
		ze->attr |= FILE_ATTRIBUTE_ARCHIVE;
	if (hidden || doshidden)
		ze->attr |= FILE_ATTRIBUTE_HIDDEN;
	if (!writable || dosreadonly)
		ze->attr |= FILE_ATTRIBUTE_READONLY;
	if (system || dossystem)
		ze->attr |= FILE_ATTRIBUTE_SYSTEM;
	ze->comp_size = ufi.compressed_size;
	ze->unc_size = ufi.uncompressed_size;

	WORD dostime = (WORD)(ufi.dosDate & 0xFFFF);
	WORD dosdate = (WORD)((ufi.dosDate >> 16) & 0xFFFF);
	FILETIME ft;
	DosDateTimeToFileTime(dosdate, dostime, &ft);
	ze->atime = ft;
	ze->ctime = ft;
	ze->mtime = ft;

	{
		unsigned int epos = 0;
		while (epos + 4 < extralen)
		{
			char etype[3];
			etype[0] = extra[epos];
			etype[1] = extra[epos + 1];
			etype[2] = 0;
			int size = (char)extra[epos + 2];
			if (strcmp(etype, "UT") != 0)
			{
				epos += 4 + size;
				continue;
			}
			int flags = (char)extra[epos + 4];
			bool hasmtime = (flags & 1) != 0;
			bool hasatime = (flags & 2) != 0;
			bool hasctime = (flags & 4) != 0;
			epos += 5;
			if (hasmtime)
			{
				time_t mtime = *(time_t*)(extra + epos);
				epos += 4;
				ze->mtime = timet2filetime(mtime);
			}
			if (hasatime)
			{
				time_t atime = *(time_t*)(extra + epos);
				epos += 4;
				ze->atime = timet2filetime(atime);
			}
			if (hasctime)
			{
				time_t ctime = *(time_t*)(extra + epos);
				epos += 4;
				ze->ctime = timet2filetime(ctime);
			}
			break;
		}
	}

	if (extra != 0)
		delete[] extra;
	memcpy(&cze, ze, sizeof(ZIPENTRY));
	czei = index;
	return ZR_OK;
}

ZRESULT TUnzip::Find(const char* tname, bool ic, int* index, ZIPENTRY* ze)
{
	int res = unzLocateFile(uf, tname, ic ? CASE_INSENSITIVE : CASE_SENSITIVE);
	if (res != UNZ_OK)
	{
		if (index != 0)
			*index = -1;
		if (ze != 0)
		{
			memset(ze, 0, sizeof(ZIPENTRY));
			ze->index = -1;
		}
		return ZR_NOTFOUND;
	}
	if (currentfile != -1)
		unzCloseCurrentFile(uf);
	currentfile = -1;
	int i = (int)uf->num_file;
	if (index != 0)
		*index = i;
	if (ze != 0)
	{
		ZRESULT zres = Get(i, ze);
		if (zres != ZR_OK)
			return zres;
	}
	return ZR_OK;
}

void EnsureDirectory(const char* rootdir, const char* dir)
{
	if (dir == 0 || *dir == 0)
		return;
	const char *lastslash = dir, *c = lastslash;
	while (*c != 0)
	{
		if (*c == '/' || *c == '\\')
			lastslash = c;
		c++;
	}
	const char* name = lastslash;
	if (lastslash != dir)
	{
		char tmp[MAX_PATH];
		_tcsncpy(tmp, dir, lastslash - dir);
		tmp[lastslash - dir] = 0;
		EnsureDirectory(rootdir, tmp);
		name++;
	}
	char cd[MAX_PATH];
	_tcscpy(cd, rootdir);
	_tcscat(cd, name);
	CreateDirectory(cd, NULL);
}

ZRESULT TUnzip::Unzip(int index, void* dst, unsigned int len, DWORD flags)
{
	if (flags != ZIP_MEMORY && flags != ZIP_FILENAME && flags != ZIP_HANDLE)
		return ZR_ARGS;
	if (flags == ZIP_MEMORY)
	{
		if (index != currentfile)
		{
			if (currentfile != -1)
				unzCloseCurrentFile(uf);
			currentfile = -1;
			if (index >= (int)uf->gi.number_entry)
				return ZR_ARGS;
			if (index < (int)uf->num_file)
				unzGoToFirstFile(uf);
			while ((int)uf->num_file < index)
				unzGoToNextFile(uf);
			unzOpenCurrentFile(uf);
			currentfile = index;
		}
		int res = unzReadCurrentFile(uf, dst, len);
		if (res > 0)
			return ZR_MORE;
		unzCloseCurrentFile(uf);
		currentfile = -1;
		if (res == 0)
			return ZR_OK;
		else
			return ZR_FLATE;
	}

	if (currentfile != -1)
		unzCloseCurrentFile(uf);
	currentfile = -1;
	if (index >= (int)uf->gi.number_entry)
		return ZR_ARGS;
	if (index < (int)uf->num_file)
		unzGoToFirstFile(uf);
	while ((int)uf->num_file < index)
		unzGoToNextFile(uf);
	ZIPENTRY ze;
	Get(index, &ze);

	if ((ze.attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		if (flags != ZIP_HANDLE)
			EnsureDirectory(rootdir, ze.name);
		return ZR_OK;
	}

	HANDLE h;
	if (flags == ZIP_HANDLE)
		h = dst;
	else
	{
		const char* name = (const char*)dst;
		const char* c = name;
		while (*c)
		{
			if (*c == '/' || *c == '\\')
				name = c + 1;
			c++;
		}

		if (name != (const char*)dst)
		{
			char dir[MAX_PATH];
			_tcscpy(dir, (const char*)dst);
			dir[name - (const char*)dst - 1] = 0;
			bool isabsolute =
				(dir[0] == '/' || dir[0] == '\\' || dir[1] == ':');
			isabsolute |= (_tcsstr(dir, _T("../")) != 0) |
				(_tcsstr(dir, _T("..\\")) != 0);
			if (!isabsolute)
				EnsureDirectory(rootdir, dir);
		}
		h = ::CreateFile((const char*)dst, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, ze.attr, NULL);
	}

	if (h == INVALID_HANDLE_VALUE)
		return ZR_NOFILE;
	unzOpenCurrentFile(uf);
	BYTE buf[16384];
	bool haderr = false;
	for (;;)
	{
		int res = unzReadCurrentFile(uf, buf, 16384);
		if (res < 0)
		{
			haderr = true;
			break;
		}
		if (res == 0)
			break;
		DWORD writ;
		BOOL bres = WriteFile(h, buf, res, &writ, NULL);
		if (!bres)
		{
			haderr = true;
			break;
		}
	}
	bool settime = false;
	DWORD type = GetFileType(h);
	if (type == FILE_TYPE_DISK && !haderr)
		settime = true;
	if (settime)
		SetFileTime(h, &ze.ctime, &ze.atime, &ze.mtime);
	if (flags != ZIP_HANDLE)
		CloseHandle(h);
	unzCloseCurrentFile(uf);
	if (haderr)
		return ZR_WRITE;
	return ZR_OK;
}

ZRESULT TUnzip::Close()
{
	if (currentfile != -1)
		unzCloseCurrentFile(uf);
	currentfile = -1;
	if (uf != 0)
		unzClose(uf);
	uf = 0;
	return ZR_OK;
}

ZRESULT lasterrorU = ZR_OK;

unsigned int FormatZipMessageU(ZRESULT code, char* buf, unsigned int len)
{
	if (code == ZR_RECENT)
		code = lasterrorU;
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

typedef struct
{
	DWORD flag;
	TUnzip* unz;
} TUnzipHandleData;

HZIP OpenZipU(void* z, unsigned int len, DWORD flags)
{
	TUnzip* unz = new TUnzip();
	lasterrorU = unz->Open(z, len, flags);
	if (lasterrorU != ZR_OK)
	{
		delete unz;
		return 0;
	}
	TUnzipHandleData* han = new TUnzipHandleData;
	han->flag = 1;
	han->unz = unz;
	return (HZIP)han;
}

ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY* ze)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	lasterrorU = unz->Get(index, ze);
	return lasterrorU;
}

ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW* zew)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	ZIPENTRY ze;
	lasterrorU = unz->Get(index, &ze);
	if (lasterrorU == ZR_OK)
	{
		zew->index = ze.index;
		zew->attr = ze.attr;
		zew->atime = ze.atime;
		zew->ctime = ze.ctime;
		zew->mtime = ze.mtime;
		zew->comp_size = ze.comp_size;
		zew->unc_size = ze.unc_size;
		_tcscpy(zew->name, ze.name);
	}
	return lasterrorU;
}

ZRESULT FindZipItemA(HZIP hz, const char* name, bool ic, int* index,
	ZIPENTRY* ze)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	lasterrorU = unz->Find(name, ic, index, ze);
	return lasterrorU;
}

ZRESULT FindZipItemW(HZIP hz, const char* name, bool ic, int* index,
	ZIPENTRYW* zew)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	ZIPENTRY ze;
	lasterrorU = unz->Find(name, ic, index, &ze);
	if (lasterrorU == ZR_OK)
	{
		zew->index = ze.index;
		zew->attr = ze.attr;
		zew->atime = ze.atime;
		zew->ctime = ze.ctime;
		zew->mtime = ze.mtime;
		zew->comp_size = ze.comp_size;
		zew->unc_size = ze.unc_size;
		_tcscpy(zew->name, ze.name);
	}
	return lasterrorU;
}

ZRESULT UnzipItem(HZIP hz, int index, void* dst, unsigned int len, DWORD flags)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	lasterrorU = unz->Unzip(index, dst, len, flags);
	return lasterrorU;
}

ZRESULT CloseZipU(HZIP hz)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	lasterrorU = unz->Close();
	delete unz;
	delete han;
	return lasterrorU;
}

bool IsZipHandleU(HZIP hz)
{
	if (hz == 0)
		return true;
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	return (han->flag == 1);
}
