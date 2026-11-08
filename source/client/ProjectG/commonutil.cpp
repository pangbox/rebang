#include "stdafx.h"
#include "commonutil.h"
#include "registry.h"
#include "chttp.h"
#include "tinyxml.h"
#include "ijl.h"
#include "exception.h"
#include <stdarg.h>
#include <ctype.h>

extern HINSTANCE g_instance;

WResourceManager* WFile::m_pResMng;

static const unsigned int key[] = { 0x485b576, 0x5148e02, 0x5141d96,
	0x28fa9d6 };

struct PAKINFO
{
	char filename[260];
	unsigned long crc;
	unsigned long size;
};

void SaveJPG(const char* filename, const Bitmap* bitmap, int quality)
{
	JPEG_CORE_PROPERTIES properties;
	memset(&properties, 0, sizeof(properties));
	if (ijlInit(&properties) == IJL_OK)
	{
		properties.DIBBytes = bitmap->vram;
		properties.DIBWidth = bitmap->bi->bmiHeader.biWidth;
		properties.DIBHeight = bitmap->bi->bmiHeader.biHeight;
		properties.DIBColor = IJL_BGR;
		properties.DIBChannels = 3;
		properties.DIBPadBytes =
			bitmap->pitch - bitmap->bi->bmiHeader.biWidth * 3;
		properties.JPGFile = filename;
		properties.JPGWidth = properties.DIBWidth;
		properties.JPGHeight = properties.DIBHeight;
		properties.JPGColor = IJL_YCBCR;
		properties.JPGChannels = 3;
		properties.JPGSubsampling = 1;
		properties.jquality = quality;
		ijlWrite(&properties, IJL_JFILE_WRITEWHOLEIMAGE);
		ijlFree(&properties);
	}
}

#pragma pack(push, 2)
struct BitmapFileHeader
{
	char type[2];
	unsigned long size;
	unsigned short reserved1, reserved2;
	unsigned long offset;
};
#pragma pack(pop)

void SaveBMP(const char* filename, const Bitmap* bitmap)
{
	BITMAPINFOHEADER info = { 40, 0, 0, 1, 24, 0, 0, 0, 0, 0, 0 };
	BitmapFileHeader file = {
		{ 'B', 'M' },
        14, 0, 0, 54
	};
	FILE* out = fopen(filename, "wb");
	if (out)
	{
		info.biWidth = bitmap->Width();
		info.biHeight = bitmap->Height();
		fwrite(&file, 1, 14, out);
		fwrite(&info, 1, 40, out);
		int bytes = bitmap->BitsPerPixel() >> 3;
		for (int y = info.biHeight - 1; y >= 0; --y)
		{
			for (int x = 0; x < info.biWidth; ++x)
			{
				unsigned char* pixel =
					bitmap->vram + y * bitmap->pitch + x * bytes;
				fputc(pixel[0], out);
				fputc(pixel[1], out);
				fputc(pixel[2], out);
			}
			for (int pad = ((info.biWidth * 3 + 3) & ~3) - info.biWidth * 3;
				pad > 0; --pad)
				fputc(0, out);
		}
		fclose(out);
	}
}

const char* LoadStr(unsigned int id)
{
	static char buffer[512];
	LoadString(g_instance, id, buffer, 512);
	return buffer;
}

bool IsPakFile(const char* name)
{
	int len = strlen(name);
	if (len < 5)
		return false;
	return strcmpi(name + len - 4, ".pak") ? FALSE : TRUE;
}

namespace std
{
	template <>
	struct less<PAKINFO> : public binary_function<PAKINFO, PAKINFO, bool>
	{
		bool operator()(PAKINFO a, PAKINFO b)
		{
			if (!strcmpi("ProjectG_ReleaseQA.exe", a.filename))
				return false;
			if (!strcmpi("ProjectG_ReleaseQA.exe", b.filename))
				return true;
			return strcmpi(a.filename, b.filename) < 0;
		}
	};
}

bool IsIntegratedPakFile(const char* name)
{
	int len = strlen(name);
	if (len < 6)
		return false;
	return strcmpi(name + len - 5, "+.pak") ? FALSE : TRUE;
}

bool IsExeFile(const char* name)
{
	int len = strlen(name);
	if (len < 5)
		return false;
	return strcmpi(name + len - 4, ".exe") ? FALSE : TRUE;
}

void DeleteReadOnlyFile(const char* name)
{
	DWORD attributes = GetFileAttributes(name);
	if (attributes & FILE_ATTRIBUTE_READONLY)
		SetFileAttributes(name, attributes & ~FILE_ATTRIBUTE_READONLY);
	DeleteFile(name);
}

void TrimExt(const char* source, char* dest)
{
	strcpy(dest, source);
	char* ext = strrchr(dest, '.');
	if (ext)
		*ext = 0;
}

void GetModuleName(char* name)
{
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	char* slash = strrchr(path, '\\');
	if (slash)
		strcpy(name, slash + 1);
	else
		strcpy(name, path);
}

extern int DecodeTEA_NBYTE(char*, int, const char*, const unsigned int*);
int g_iLoadTotalNum = 100;
bool g_bExitDrawLoading;
int g_iLoadNextNum;
int g_iLoadCurNum;

bool DownloadUpdateList(std::list<PAKINFO>* files, char* base)
{
	cHttp http;
	unsigned long size = 0;
	char* data = http.DownLoad("/pangya/season4/patch/qa/updatelist",
		"http://patch.pangya.ntreev.com/", size);
	if (!data)
	{
		MessageBox(NULL,
			"\xbc\xad\xb9\xf6\xb7\xce\xba\xce\xc5\xcd \xc6\xc4\xc0\xcf \xb8\xf1\xb7\xcf\xc0\xbb \xb0\xa1\xc1\xae\xbf\xc0\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
			"\xbe\xcb\xb8\xb2", MB_ICONINFORMATION);
		return false;
	}
	if (IsLocalContent((localContentType_t)18))
	{
		unsigned int padded = size + ((size & 7) == 0 ? 0 : 8 - (size & 7));
		char* decoded = new char[padded];
		memset(decoded, 0, padded);

		DecodeTEA_NBYTE(decoded, padded, data, key);
		delete[] data;
		data = decoded;
	}
	TiXmlDocument doc;
	TiXmlString xml;
	xml.reserve(size);
	xml += data;
	doc.Parse(xml.c_str());
	if (data)
		delete[] data;
	if (doc.Error())
	{
		MessageBox(NULL,
			"XML \xc6\xc4\xbd\xcc\xc1\xdf \xbf\xa1\xb7\xaf\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
			"\xbe\xcb\xb8\xb2", MB_ICONINFORMATION);
		return false;
	}

	std::string tags[5];
	if (IsLocalContent((localContentType_t)18))
	{
		tags[0] = "updatefiles";
		tags[1] = "fileinfo";
		tags[2] = "fname";
		tags[3] = "fsize";
		tags[4] = "fcrc";
	}
	else
	{
		tags[0] = "filelist";
		tags[1] = "file";
		tags[2] = "filename";
		tags[3] = "size";
		tags[4] = "crc";
	}
	TiXmlNode* root = doc.FirstChild(tags[0].c_str());
	if (root)
	{
		TiXmlNode* node = root->FirstChild(tags[1].c_str());
		while (node)
		{
			PAKINFO info;
			int dummy;
			strcpy(info.filename,
				node->ToElement()->Attribute(tags[2].c_str(), &dummy));
			if (IsPakFile(info.filename))
			{
				char name[64] = { 0 };
				TrimExt(info.filename, name);
				if (!strcmpi(name, base) ||
					(strcmpi(name, base) > 0 &&
						!IsIntegratedPakFile(info.filename)))
				{
					info.size = atoi(
						node->ToElement()->Attribute(tags[3].c_str(), &dummy));
					info.crc = atoi(
						node->ToElement()->Attribute(tags[4].c_str(), &dummy));
					files->push_back(info);
				}
			}
			else if (!strcmpi("ProjectG_ReleaseQA.exe", info.filename))
			{
				info.size = atoi(node->ToElement()->Attribute("fsize", &dummy));
				info.crc = atoi(node->ToElement()->Attribute("fcrc", &dummy));
				files->push_back(info);
			}
			node = node->NextSibling();
		}
		return true;
	}
	else
	{
		MessageBox(NULL,
			"\xc6\xc4\xc0\xcf \xb8\xf1\xb7\xcf\xc0\xbb \xc0\xd0\xb4\xc2 \xc1\xdf \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
			"\xc6\xc4\xc0\xcf \xbf\xa1\xb7\xaf", MB_ICONINFORMATION);
	}
	return false;
}

WTL::CString __stdcall GetRegIntegratedPakName()
{
	CRegistry registry;
	registry.SetRootKey(HKEY_LOCAL_MACHINE);
	registry.SetKey("Software\\Ntreev\\Pangya_QA", TRUE);
	WTL::CString result = "";
	result = registry.ReadString("IntegratedPak", "");
	return result;
}

namespace std
{
	template <>
	struct greater<PAKINFO> : public binary_function<PAKINFO, PAKINFO, bool>
	{
		bool operator()(const PAKINFO& a, const PAKINFO& b) const
		{
			return a.size > b.size;
		}
	};
}

int __stdcall IntegrityPakCRC(std::list<PAKINFO>& files, const char* base,
	std::string& error)
{
	std::list<PAKINFO>::iterator it = files.begin();
	SYSTEM_INFO system;
	memset(&system, 0, sizeof(system));
	GetSystemInfo(&system);
	BOOL multi = FALSE;
	if (system.dwNumberOfProcessors > 1)
		multi = TRUE;
	CRC_32 crc;
	std::list<PAKINFO> check;
	for (; it != files.end(); ++it)
	{
		PAKINFO& info = *it;
		if (!strcmpi("ProjectG_ReleaseQA.exe", info.filename))
		{
			char name[260] = { 0 };
			GetModuleName(name);
			if (!strcmpi(name, info.filename))
			{
				unsigned long calculated = crc.CalcCRC(info.filename, NULL);
				if (calculated == info.crc)
					continue;
			}
			error = "ProjectG_ReleaseQA.exe";
			g_bExitDrawLoading = true;
			return 0;
		}
		else
		{
			char name[260] = { 0 };
			TrimExt(info.filename, name);
			if (strcmpi(base, name) <= 0)
			{
				if (multi)
				{
					if (strstr(info.filename, "_"))
						check.push_back(info);
				}
				else
				{
					check.push_back(info);
				}
				HookPAKFile(info.filename);
			}
		}
	}
	float total = (float)check.size();
	float count = 0;
	check.sort(std::greater<PAKINFO>());
	for (std::list<PAKINFO>::iterator jt = check.begin(); jt != check.end();
		++jt)
	{
		PAKINFO& info = *jt;
		if (info.crc != crc.CalcCRC(info.filename, NULL))
		{
			error = info.filename;
			return 0;
		}
		g_iLoadNextNum = (int)((++count / total) * g_iLoadTotalNum);
	}
	g_iLoadNextNum = g_iLoadTotalNum;
	return 1;
}

bool __stdcall GetCRCCheckList(
	std::list<std::pair<std::string, unsigned long> >& result)
{
	std::list<PAKINFO> files;
	WTL::CString pak = GetRegIntegratedPakName();
	char base[64] = { 0 };
	if (pak.GetLength() != 0)
	{
		TrimExt(pak, base);
		if (DownloadUpdateList(&files, base))
		{
			files.sort(std::less<PAKINFO>());
			for (std::list<PAKINFO>::iterator it = files.begin();
				it != files.end(); ++it)
			{
				if (strcmpi("ProjectG_ReleaseQA.exe", (*it).filename))
				{
					char name[260] = { 0 };
					TrimExt((*it).filename, name);
					if (strcmpi(base, name) <= 0 &&
						!strstr((*it).filename, "_"))
						result.push_back(std::pair<std::string, unsigned long>(
							std::string((*it).filename), (*it).crc));
				}
			}
			return true;
		}
	}
	else
	{
		MessageBox(NULL,
			"\xc0\xce\xbd\xba\xc5\xe7 \xb9\xf6\xc1\xaf\xc0\xbb \xbc\xb3\xc4\xa1, \xc8\xa4\xc0\xba \xb7\xb1\xc3\xc4\xb8\xa6 \xb8\xd5\xc0\xfa \xbd\xc7\xc7\xe0\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
			"\xc6\xc4\xc0\xcf \xbf\xa1\xb7\xaf", MB_ICONEXCLAMATION);
	}
	return false;
}

bool InitPakedFile2()
{
	std::list<PAKINFO> files;
	char base[64] = { 0 };
	WTL::CString pak = GetRegIntegratedPakName();
	if (pak.GetLength() != 0)
	{
		TrimExt(pak, base);
	}
	else
	{
		MessageBox(NULL,
			"\xc0\xce\xbd\xba\xc5\xe7 \xb9\xf6\xc1\xaf\xc0\xbb \xbc\xb3\xc4\xa1, \xc8\xa4\xc0\xba \xb7\xb1\xc3\xc4\xb8\xa6 \xb8\xd5\xc0\xfa \xbd\xc7\xc7\xe0\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
			"\xc6\xc4\xc0\xcf \xbf\xa1\xb7\xaf", MB_ICONEXCLAMATION);
		return false;
	}
	if (!DownloadUpdateList(&files, base))
		return false;
	files.sort(std::less<PAKINFO>());
	std::string error;
	if (IntegrityPakCRC(files, base, error))
	{
		MessageBox(NULL,
			MakeStr(
				"%s \xc6\xc4\xc0\xcf\xbf\xa1 \xbc\xd5\xbb\xf3\xc0\xcc \xb9\xdf\xb0\xdf\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc7\xc7\xe0\xc7\xd8\xc1\xd6 \xbc\xbc\xbf\xe4.",
				error.c_str()),
			"\xc6\xc4\xc0\xcf \xbf\xa1\xb7\xaf", MB_ICONEXCLAMATION);
		return false;
	}
	else
	{
		g_resrcmng->SetAutoMatchDirectory("data");
		return true;
	}
}

void InitPakedFile()
{
	try
	{
		char* name;
		WList<char*> files(16, 16);
		WIN32_FIND_DATA data;
		HANDLE find = FindFirstFile("projectg*.pak", &data);
		if (find != INVALID_HANDLE_VALUE)
		{
			do
			{
				name = new char[strlen(data.cFileName) + 1];
				strcpy(name, data.cFileName);
				files += name;
			} while (FindNextFile(find, &data));
		}
		FindClose(find);
		char* first;
		while ((first = files.Start()) != NULL)
		{
			for (name = first; name; name = files.Next())
				if (strcmp(name, first) < 0)
					first = name;
			files -= first;
			HookPAKFile(first);
			delete[] first;
		}
		g_resrcmng->SetAutoMatchDirectory("data");
	}
	catch (...)
	{
		MessageBox(NULL,
			"\xc6\xc4\xc0\xcf\xbf\xa1 \xb9\xae\xc1\xa6\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbc\xb3\xc4\xa1\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
			"\xc6\xc4\xc0\xcf \xbf\xa1\xb7\xaf", MB_ICONINFORMATION);
		throw;
	}
}

void LineParse(const char* line, std::vector<std::string>& tokens)
{
	while (*line)
	{
		const char* cursor = line;
		if (*cursor == '\r')
			break;
		std::string token;
		if (*cursor != '"')
		{
			while (!isspace(*cursor) && *cursor)
			{
				token += *cursor;
				++cursor;
			}
		}
		else
		{
			++cursor;
			while (*cursor != '"' && *cursor)
			{
				token += *cursor;
				++cursor;
			}
			if (*cursor == '"')
				++cursor;
		}
		tokens.push_back(token);
		while (isspace(*cursor))
			++cursor;
		line = cursor;
	}
}

char* GetStr(char* source, char* dest)
{
	while ((*source == ' ' || *source == '\t') && *source)
		++source;
	if (*source && *source++ == '"')
	{
		while (*source && *source != '"')
			*dest++ = *source++;
		if (*source)
			++source;
	}
	*dest = 0;
	return source;
}

const char* GetStrConst(const char* source, char* dest)
{
	while ((*source == ' ' || *source == '\t') && *source)
		++source;
	if (*source && *source++ == '"')
	{
		while (*source && *source != '"')
			*dest++ = *source++;
		if (*source)
			++source;
	}
	*dest = 0;
	return source;
}

const char* GetCharsTillSpace(const char* source, char* dest)
{
	while ((*source == ' ' || *source == '\t') && *source)
		++source;
	if (!*source)
	{
		*dest = 0;
		return NULL;
	}
	while (*source && *source != ' ')
		*dest++ = *source++;
	*dest = 0;
	return source;
}

char* FindTag(char* begin, char* end, char* tag)
{
	if (begin && tag)
	{
		char* found = strstr(begin, tag);
		if (found && found < end)
			return found + strlen(tag);
	}
	return NULL;
}

void LimitText(char* text, int size)
{
	if (text && strlen(text) > size)
	{
		int i;
		for (i = 0; i < size; ++i)
		{
			if (Custom_IsLeadByte(text[i]))
			{
				if (i + 1 >= size)
					break;
				++i;
			}
		}
		text[i] = 0;
	}
}

char* __cdecl MakeStr(const char* format, ...)
{
	static __declspec(thread) unsigned char formatIndex;
	static __declspec(thread) char formatBuffer[64][576];
	va_list args;
	va_start(args, format);
	_vsnprintf(formatBuffer[formatIndex & 63], 576, format, args);
	va_end(args);
	return formatBuffer[formatIndex++ & 63];
}

bool str_Replace(std::string& str, const std::string& from,
	const std::string& to)
{
	bool result = false;
	int pos = str.find(from);
	if (pos != std::string::npos)
	{
		do
		{
			str.replace(str.begin() + pos, str.begin() + pos + from.size(), to);
			pos = str.find(from, pos + to.size());
		} while (pos != std::string::npos);
		result = true;
	}
	return result;
}

std::string GetMyComputerName()
{
	char name[20];
	DWORD size = 17;
	GetComputerName(name, &size);
	return std::string(name);
}

float Random(float min, float max, int seed)
{
	if (seed < 0)
		seed = rand();
	return (max - min) * (seed % 1023) / 1024.0f + min;
}

unsigned long htoi(const char* str)
{
	if (str)
	{
		unsigned long value = strlen(str);
		if (value)
		{
			char text[256];
			sprintf(text, "0x%s", str);
			sscanf(text, "%X", &value);
			return value;
		}
	}
	return 0;
}

unsigned long ModifyColor(unsigned long color, float bright)
{
	unsigned char r =
		(unsigned char)Min(((color >> 16) & 255) * bright, 255.0f);
	unsigned char g = (unsigned char)Min(((color >> 8) & 255) * bright, 255.0f);
	unsigned char b = (unsigned char)Min((color & 255) * bright, 255.0f);
	return (r << 16) | (g << 8) | b;
}

void RemakeNumber(int value, char* text)
{
	char number[32];
	sprintf(number, "%d", value);
	int len = strlen(number);
	int j = 0;
	for (int i = 0; i < len;)
	{
		text[j++] = number[i++];
		if (i >= len)
			break;
		if ((len - i) % 3 == 0)
			text[j++] = ',';
	}
	text[j] = 0;
}

WFile::WFile()
	: m_filename(NULL), m_pFile(NULL)
{
}

WFile::WFile(const char* filename)
	: m_filename(filename), m_pFile(NULL)
{
	m_pFile = m_pResMng->GetCFile(filename, -1);
	if (!m_pFile)
		throw WAppException(
			"\xc6\xc4\xc0\xcf\xc0\xbb \xbf\xad \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
}

WFile::~WFile()
{
	Close();
}

cFile* WFile::Open(const char* filename, unsigned int option)
{
	Close();
	m_pFile = m_pResMng->GetCFile(filename, option);
	return m_pFile;
}

void WFile::Close()
{
	if (m_pFile)
		CloseCFile(m_pFile);
	m_pFile = NULL;
}

int WFile::GetByte()
{
	return m_pFile ? m_pFile->GetByte() : -1;
}

int WFile::Tell()
{
	return m_pFile ? m_pFile->Tell() : -1;
}

int WFile::Read(void* buffer, int size)
{
	return m_pFile ? m_pFile->Read(buffer, size) : -1;
}

void WFile::Seek(int offset, int origin)
{
	if (m_pFile)
		m_pFile->Seek(offset, origin);
}

int WFile::Length()
{
	return m_pFile ? m_pFile->Length() : -1;
}

void WFile::SetResourceManager(WResourceManager* manager)
{
	m_pResMng = manager;
}
