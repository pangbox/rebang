#include "projectg.h"
#include "networksystem.h"
#include "packet.h"
#include "packetversion.h"
#include "dxdiaginfo.h"
#include "s5/utilities.h"
#include <stdio.h>
#include <dbghelp.h>

extern "C" void* _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)

typedef BOOL(__stdcall* PSYMCLEANUP)(HANDLE);
typedef PVOID(__stdcall* PSYMFUNCTIONTABLEACCESS)(HANDLE, DWORD);
typedef DWORD(__stdcall* PSYMGETMODULEBASE)(HANDLE, DWORD);
typedef BOOL(__stdcall* PSYMGETMODULEINFO)(HANDLE, DWORD, PIMAGEHLP_MODULE);
typedef DWORD(__stdcall* PSYMGETOPTIONS)();
typedef BOOL(
	__stdcall* PSYMGETSYMFROMADDR)(HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL);
typedef BOOL(__stdcall* PSYMINITIALIZE)(HANDLE, PSTR, BOOL);
typedef DWORD(__stdcall* PSYMSETOPTIONS)(DWORD);
typedef BOOL(__stdcall* PSTACKWALK)(DWORD, HANDLE, HANDLE, LPSTACKFRAME, PVOID,
	PREAD_PROCESS_MEMORY_ROUTINE, PFUNCTION_TABLE_ACCESS_ROUTINE,
	PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE);
typedef DWORD(__stdcall* PUNDECORATESYMBOLNAME)(PCSTR, PSTR, DWORD, DWORD);
typedef BOOL(
	__stdcall* PSYMLOADMODULE)(HANDLE, HANDLE, PSTR, PSTR, DWORD, DWORD);
typedef BOOL(
	__stdcall* PSYMGETLINEFROMADDR)(HANDLE, DWORD, PDWORD, PIMAGEHLP_LINE);

CExceptionReport g_exceptionReport;

_client::CCriticalSection g_criticalSection;

static PUNDECORATESYMBOLNAME pUnDecorateSymbolName;
static PSTACKWALK pStackWalk;
static PSYMSETOPTIONS pSymSetOptions;
static PSYMLOADMODULE pSymLoadModule;
static PSYMINITIALIZE pSymInitialize;
static PSYMGETSYMFROMADDR pSymGetSymFromAddr;
static PSYMGETOPTIONS pSymGetOptions;
static PSYMGETMODULEINFO pSymGetModuleInfo;
static PSYMGETMODULEBASE pSymGetModuleBase;
static PSYMGETLINEFROMADDR pSymGetLineFromAddr;
static PSYMFUNCTIONTABLEACCESS pSymFunctionTableAccess;
static PSYMCLEANUP pSymCleanup;
LPTOP_LEVEL_EXCEPTION_FILTER g_pPrevUnhandledExceptionFilter;
DWORD g_dwStack;
DWORD g_dwStackBottom;
HINSTANCE g_hDLL;

__declspec(thread) HANDLE CExceptionReport::m_curThread = INVALID_HANDLE_VALUE;
__declspec(thread) unsigned long CExceptionReport::m_curThreadId = 0;

static bool InitSymFunctions()
{
	HINSTANCE hDLL = LoadLibraryA("dbghelp.dll");
	if (!hDLL && !(hDLL = LoadLibraryA("imagehlp.dll")))
		return false;

	pSymCleanup = (PSYMCLEANUP)GetProcAddress(hDLL, "SymCleanup");
	pSymFunctionTableAccess =
		(PSYMFUNCTIONTABLEACCESS)GetProcAddress(hDLL, "SymFunctionTableAccess");
	pSymGetModuleBase =
		(PSYMGETMODULEBASE)GetProcAddress(hDLL, "SymGetModuleBase");
	pSymGetModuleInfo =
		(PSYMGETMODULEINFO)GetProcAddress(hDLL, "SymGetModuleInfo");
	pSymGetOptions = (PSYMGETOPTIONS)GetProcAddress(hDLL, "SymGetOptions");
	pSymGetSymFromAddr =
		(PSYMGETSYMFROMADDR)GetProcAddress(hDLL, "SymGetSymFromAddr");
	pSymInitialize = (PSYMINITIALIZE)GetProcAddress(hDLL, "SymInitialize");
	pSymSetOptions = (PSYMSETOPTIONS)GetProcAddress(hDLL, "SymSetOptions");
	pStackWalk = (PSTACKWALK)GetProcAddress(hDLL, "StackWalk");
	pUnDecorateSymbolName =
		(PUNDECORATESYMBOLNAME)GetProcAddress(hDLL, "UnDecorateSymbolName");
	pSymLoadModule = (PSYMLOADMODULE)GetProcAddress(hDLL, "SymLoadModule");
	pSymGetLineFromAddr =
		(PSYMGETLINEFROMADDR)GetProcAddress(hDLL, "SymGetLineFromAddr");

	if (pSymCleanup && pSymFunctionTableAccess && pSymGetModuleBase &&
		pSymGetModuleInfo && pSymGetOptions && pSymGetSymFromAddr &&
		pSymInitialize && pSymSetOptions && pStackWalk &&
		pUnDecorateSymbolName && pSymLoadModule)
	{
		g_hDLL = hDLL;
		return true;
	}

	FreeLibrary(hDLL);
	return false;
}

static void FreeSymFunctions()
{
	if (g_hDLL)
	{
		FreeLibrary(g_hDLL);
		g_hDLL = NULL;
	}
}

static BOOL GetModuleFileNameWithAddress(const void* addr, char* moduleName,
	DWORD size)
{
	MEMORY_BASIC_INFORMATION MemInfo;
	if (VirtualQuery(addr, &MemInfo, sizeof(MemInfo)) &&
		GetModuleFileNameA((HMODULE)MemInfo.AllocationBase, moduleName, size) >
			0)
		return TRUE;

	strncpy(moduleName, "Unknown", size);
	return FALSE;
}

static const char* GetExceptionDescription(
	const EXCEPTION_RECORD* exceptionRecord)
{
	switch (exceptionRecord->ExceptionCode)
	{
	case DBG_CONTROL_C:
		return "Control-C";
	case DBG_CONTROL_BREAK:
		return "Control-Break";
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return "Datatype Misalignment";
	case EXCEPTION_BREAKPOINT:
		return "Breakpoint";
	case EXCEPTION_ACCESS_VIOLATION:
		return "Access Violation";
	case EXCEPTION_IN_PAGE_ERROR:
		return "an In Page Error";
	case STATUS_NO_MEMORY:
		return "No Memory";
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return "Illegal Instruction";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return "Noncontinuable Exception";
	case EXCEPTION_INVALID_DISPOSITION:
		return "Invalid Disposition";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return "Array Bounds Exceeded";
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return "Float Denormal Operand";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return "Float Divide by Zero";
	case EXCEPTION_FLT_INEXACT_RESULT:
		return "Float Inexact Result";
	case EXCEPTION_FLT_INVALID_OPERATION:
		return "Float Invalid Operation";
	case EXCEPTION_FLT_OVERFLOW:
		return "Float Overflow";
	case EXCEPTION_FLT_STACK_CHECK:
		return "Float Stack Check";
	case EXCEPTION_FLT_UNDERFLOW:
		return "Float Underflow";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return "Integer Divide by Zero";
	case EXCEPTION_INT_OVERFLOW:
		return "Integer Overflow";
	case EXCEPTION_PRIV_INSTRUCTION:
		return "Privileged Instruction";
	case EXCEPTION_STACK_OVERFLOW:
		return "Stack Overflow";
	case 0xc0000142:
		return "DLL Initialization Failed";
	case 0xe06d7363:
		return "Microsoft C++ Exception";
	case 0x60000000:
		return "PacketDecodeException";
	case EXCEPTION_WITH_USER_INFO:
	{
		static __declspec(thread) char szMessage[8192];

		if (exceptionRecord->NumberParameters >= 3)
			sprintf(szMessage,
				"EXCEPTION_WITH_USER_INFO => msg: %s, arg1: %d, arg2: %d",
				exceptionRecord->ExceptionInformation[0],
				exceptionRecord->ExceptionInformation[1],
				exceptionRecord->ExceptionInformation[2]);
		else
			sprintf(szMessage,
				"EXCEPTION_WITH_USER_INFO => invalid parameter count(%d)",
				exceptionRecord->NumberParameters);
		return szMessage;
	}
	default:
		return "Unknown exception type";
	}
}

static void CreateMiniDump(const char* filename,
	EXCEPTION_POINTERS* pExceptionPointers)
{
	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
	PMINIDUMP_EXCEPTION_INFORMATION pInfo;

	HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	SetThreadPriority(CExceptionReport::GetCurrentThread(),
		THREAD_PRIORITY_HIGHEST);

	if (pExceptionPointers)
	{
		exceptionInfo.ThreadId = CExceptionReport::GetCurrentThreadId();
		exceptionInfo.ClientPointers = TRUE;
		exceptionInfo.ExceptionPointers = pExceptionPointers;
		pInfo = &exceptionInfo;
	}
	else
	{
		pInfo = NULL;
	}

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
		MiniDumpNormal, pInfo, NULL, NULL);

	SetThreadPriority(CExceptionReport::GetCurrentThread(),
		THREAD_PRIORITY_NORMAL);
	CloseHandle(hFile);
}

void __cdecl SecurityErrorHandler(int code, void* unused)
{
	if (g_exceptionReport.IsEnabled())
	{
		_client::_private::CLock<_client::CCriticalSection> lock(
			g_criticalSection);

		g_exceptionReport.DumpExceptionReport(NULL);
	}
	ExitProcess(1);
}

long __stdcall RecordExceptionInfo(EXCEPTION_POINTERS* data)
{
	static BOOL beenHere = FALSE;

	char text[1024] = { 0 };

	if (g_exceptionReport.IsEnabled())
	{
		_client::_private::CLock<_client::CCriticalSection> lock(
			g_criticalSection);

		if (beenHere)
			goto CallPreviousFilter;

		beenHere = TRUE;
		g_exceptionReport.DumpExceptionReport(data);

		wsprintfA(text,
			"%s Exception raised at 0x%08x. Program will be terminated.",
			GetExceptionDescription(data->ExceptionRecord),
			data->ExceptionRecord->ExceptionAddress);
	}
	else
	{
		MessageBoxA(NULL, text, "g_exceptionReport.IsEnabled() == false",
			MB_OK);
	}

	if (WNetworkSystem::Instance())
	{
		std::string strType("Exception");
		WSendPacket packet((enumClientPacket)0x33);
		packet.Encode1(0);
		packet.EncodeStr(strType);
		packet.Send(TO_GAME);
	}

CallPreviousFilter:
	if (g_pPrevUnhandledExceptionFilter)
		return g_pPrevUnhandledExceptionFilter(data);

	return EXCEPTION_CONTINUE_SEARCH;
}

void CExceptionReport::Init()
{
	InitSymFunctions();
	g_pPrevUnhandledExceptionFilter =
		SetUnhandledExceptionFilter(RecordExceptionInfo);
	_set_security_error_handler(SecurityErrorHandler);
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX |
		SEM_NOOPENFILEERRORBOX);
	m_enable = TRUE;
	m_initialized = true;
}

void CExceptionReport::Shutdown()
{
	if (g_pPrevUnhandledExceptionFilter)
		SetUnhandledExceptionFilter(g_pPrevUnhandledExceptionFilter);
	SetErrorMode(0);
	FreeSymFunctions();
	m_initialized = false;
}

int CExceptionReport::Enable()
{
	return InterlockedExchange(&m_enable, TRUE) == FALSE;
}

int CExceptionReport::Disable()
{
	return InterlockedExchange(&m_enable, FALSE);
}

int CExceptionReport::IsEnabled() const
{
	return m_enable;
}

void CExceptionReport::StartLog(const char* filename)
{
	m_logFileHandle =
		CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
}

void CExceptionReport::LogPrintf(const char* fmt, ...) const
{
	char buffer[4096];
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	DWORD numBytes;
	WriteFile(m_logFileHandle, buffer, strlen(buffer), &numBytes, NULL);
	va_end(args);
}

void CExceptionReport::EndLog()
{
	LogPrintf(
		"--------------------------------- END_OF_LOG ---------------------------------\r\n");
	if (m_logFileHandle != INVALID_HANDLE_VALUE)
		CloseHandle(m_logFileHandle);
	m_logFileHandle = INVALID_HANDLE_VALUE;
}

void CExceptionReport::CollectSystemInfo()
{
	OSVERSIONINFOA osvi;
	char* os;

	memset(&osvi, 0, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	if (GetVersionExA(&osvi))
	{
		switch (osvi.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_WINDOWS:
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
				os = "Windows 95";
			else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
				os = "Windows 98/98SE";
			else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				os = "Windows ME";
			else
				os = "Windows ME or later";
			break;
		case VER_PLATFORM_WIN32_NT:
			if (osvi.dwMajorVersion <= 4)
				os = "Windows NT";
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
				os = "Windows 2000";
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
				os = "Microsoft Windows XP";
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
				os = "Microsoft Windows Server 2003 family";
			else
				os = "Microsoft Vista or later";
			break;
		default:
			os = "Unknown Windows variant";
			break;
		}

		wsprintfA(m_osName, "%s %i.%02d.%i %s", os, osvi.dwMajorVersion,
			osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.szCSDVersion);
	}
	else
	{
		strcpy(m_osName, "Unknown");
	}

	GetSystemInfo(&m_systemInfo);

	m_memInfo.dwLength = sizeof(m_memInfo);
	GlobalMemoryStatus(&m_memInfo);

	DWORD size = 200;
	if (!GetUserNameA(m_userName, &size))
		strcpy(m_userName, "Unknown");

	size = 200;
	if (!GetComputerNameA(m_computerName, &size))
		strcpy(m_computerName, "Unknown");
}

static void GetBaseName(const char* pszSrc, char* pszDst)
{
	const char* p = pszSrc + strlen(pszSrc) - 1;
	while (p != pszSrc && p[-1] != '\\' && p[-1] != '/')
		--p;

	while (*p)
		*pszDst++ = *p++;
	*pszDst = '\0';
}

void CExceptionReport::DumpHeaderInfo(EXCEPTION_POINTERS* pExceptionInfo)
{
	char* pszNick = "NotDetected";
	int nUID = -1;
	char* pszID = "NotDetected";

	if (Doc() && strlen(MyId()))
	{
		pszID = MyId();
		nUID = MyUID();
		pszNick = MyNick();
	}

	char szEncodedNick[64] = { 0 };
	S5::UTIL::Base64Encode(pszNick, strlen(pszNick), szEncodedNick);

	LogPrintf("%d, %s, %d, %s, %s\n", -1, pszID, nUID, szEncodedNick,
		PY_CLIENT_VERSION);
}

void CExceptionReport::EndStackLog()
{
	if (m_logFileHandle != INVALID_HANDLE_VALUE)
		CloseHandle(m_logFileHandle);
	m_logFileHandle = INVALID_HANDLE_VALUE;
}

void CExceptionReport::DumpHeader(EXCEPTION_POINTERS* data) const
{
	char szPath[MAX_PATH];
	GetModuleFileNameWithAddress((void*)data->ContextRecord->Eip, szPath,
		MAX_PATH);
	GetBaseName(szPath, szPath);

	char* pszID = "NotDetected";
	if (Doc() && strlen(MyId()))
		pszID = MyId();

	LogPrintf("<%s %s %s %04x:%08x %s>\r\n", pszID, PY_PUBLIC_VERSION,
		PY_CLIENT_VERSION, data->ContextRecord->SegCs, data->ContextRecord->Eip,
		szPath);
	LogPrintf(
		"==============================================================================\r\n");
	LogPrintf("  Pangya (Version: %s, Packet Version: %d)\r\n",
		PY_PUBLIC_VERSION, PY_PACKET_VERSION);
	LogPrintf(
		"==============================================================================\r\n");
}

void CExceptionReport::DumpSystemInfo() const
{
	LogPrintf("Time:      %s\r\n", m_crashTime);
	LogPrintf("User:      %s\r\n", m_userName);
	LogPrintf("Computer:  %s\r\n", m_computerName);
	if (CProjectG::Instance() && CProjectG::Instance()->m_pDxDiagInfo)
		LogPrintf("OS:        %s\r\n",
			CProjectG::Instance()->m_pDxDiagInfo->GetOSName());
	else
		LogPrintf("OS:        %s\r\n", m_osName);
	LogPrintf("Processor: %d processor(s), type %d.\r\n",
		m_systemInfo.dwNumberOfProcessors, m_systemInfo.dwProcessorType);
	LogPrintf("Memory:    %d MBytes physical memory.\r\n",
		(m_memInfo.dwTotalPhys + 0xfffff) >> 20);
	if (CProjectG::Instance() && CProjectG::Instance()->m_pDxDiagInfo)
	{
		LogPrintf("Graphic Card: %s\r\n",
			CProjectG::Instance()->m_pDxDiagInfo->GetGraphicCardInfo());
		LogPrintf("CPU:       %s\r\n",
			CProjectG::Instance()->m_pDxDiagInfo->GetCPUInfo());
	}
	LogPrintf(
		"------------------------------------------------------------------------------\r\n");
}

void CExceptionReport::DumpErrorMessage(const CONTEXT* contextRecord,
	const EXCEPTION_RECORD* exceptionRecord) const
{
	char crashModuleFilename[MAX_PATH];
	GetModuleFileNameWithAddress((void*)contextRecord->Eip, crashModuleFilename,
		MAX_PATH);

	LogPrintf("\r\n");
	LogPrintf("Program:   %s\r\n", crashModuleFilename);
	LogPrintf("Exception: %08x (%s) at %04x:%08x.\r\n",
		exceptionRecord->ExceptionCode,
		GetExceptionDescription(exceptionRecord), contextRecord->SegCs,
		contextRecord->Eip);

	if (exceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION &&
		exceptionRecord->NumberParameters >= 2)
	{
		LogPrintf(
			"\r\nThe instruction at \"%08x\" referenced memory at \"%08x\"\r\nThe memory could not be %s\r\n",
			contextRecord->Eip, exceptionRecord->ExceptionInformation[1],
			exceptionRecord->ExceptionInformation[0] ? "\"write\""
													 : "\"read\"");
	}

	LogPrintf("\r\n");
}

void CExceptionReport::DumpRegisters(const CONTEXT* contextRecord) const
{
	LogPrintf("----------------------------------------\r\n");
	LogPrintf("  x86 Registers\r\n");
	LogPrintf("----------------------------------------\r\n");
	LogPrintf("EAX=%08x  CS=%04x  EIP=%08x  EFLGS=%08x\r\n", contextRecord->Eax,
		contextRecord->SegCs, contextRecord->Eip, contextRecord->EFlags);
	LogPrintf("EBX=%08x  SS=%04x  ESP=%08x  EBP=%08x\r\n", contextRecord->Ebx,
		contextRecord->SegSs, contextRecord->Esp, contextRecord->Ebp);
	LogPrintf("ECX=%08x  DS=%04x  ESI=%08x  FS=%04x\r\n", contextRecord->Ecx,
		contextRecord->SegDs, contextRecord->Esi, contextRecord->SegFs);
	LogPrintf("EDX=%08x  ES=%04x  EDI=%08x  GS=%04x\r\n", contextRecord->Edx,
		contextRecord->SegEs, contextRecord->Edi, contextRecord->SegGs);
	LogPrintf("\r\n");
}

void CExceptionReport::PrintStack(unsigned long begin, unsigned long end) const
{
	char buffer[92] = { 0 };

	begin &= ~31;
	if (begin < g_dwStack)
		begin = g_dwStack;

	if (end > g_dwStackBottom)
		end = g_dwStackBottom;
	else
		end = (end + 31) & ~31;

	__try
	{
		char* output = buffer;
		while (begin < end)
		{
			if (!(begin & 31))
				output += wsprintfA(output, "%08x:", begin);
			output += wsprintfA(output, " %08x", *(DWORD*)begin);
			begin += 4;
			if (!(begin & 31))
			{
				LogPrintf("%s\r\n", buffer);
				buffer[0] = '\0';
				output = buffer;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		LogPrintf("*** Exception encountered during stack dump\r\n", begin);
		g_dwStackBottom = begin;
	}
}

void CExceptionReport::IntelStackWalk(CONTEXT* ptrContext) const
{
	DWORD* pFrame = (DWORD*)ptrContext->Ebp;
	DWORD dwEip = ptrContext->Eip;
	DWORD* pPrevFrame;

	do
	{
		char crashModulePathname[MAX_PATH];
		GetModuleFileNameWithAddress((void*)dwEip, crashModulePathname,
			MAX_PATH);

		LogPrintf("Module=%s\r\n", crashModulePathname);
		LogPrintf("Frame=%08x\r\n", pFrame);
		PrintStack((DWORD)pFrame, (DWORD)(pFrame + 40));

		LogPrintf("Address=%08x: ", dwEip);
		BYTE* code = (BYTE*)dwEip;
		for (int codebyte = 0; codebyte < 16; codebyte++)
		{
			__try
			{
				LogPrintf("%02x ", code[codebyte]);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				LogPrintf("?? ");
			}
		}
		LogPrintf("\r\n\r\n");

		pPrevFrame = (DWORD*)pFrame[0];
		dwEip = pFrame[1];

		if ((DWORD)pPrevFrame & 3)
			break;

		if (pPrevFrame <= pFrame)
			break;

		if (IsBadWritePtr(pPrevFrame, sizeof(void*) * 2))
			break;

		pFrame = pPrevFrame;
	} while (1);
}

void CExceptionReport::ImageHelpStackWalk(CONTEXT* ptrContext) const
{
	STACKFRAME sf;
	memset(&sf, 0, sizeof(sf));
	sf.AddrPC.Offset = ptrContext->Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = ptrContext->Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = ptrContext->Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;

	while (1)
	{
		if (!pStackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(),
				GetCurrentThread(), &sf, ptrContext, NULL,
				pSymFunctionTableAccess, pSymGetModuleBase, NULL))
			break;

		if (sf.AddrFrame.Offset == 0)
			break;

		LogPrintf("Frame=%08x\r\n", sf.AddrFrame.Offset);
		PrintStack(sf.AddrFrame.Offset, sf.AddrFrame.Offset + 40);

		BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 1024];
		PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
		pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		pSymbol->MaxNameLength = 1024;

		char UnDName[512];
		UnDName[0] = '\0';

		DWORD symDisplacement = 0;

		LogPrintf("Address=%08x\r\n", sf.AddrPC.Offset);
		if (pSymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset,
				&symDisplacement, pSymbol))
		{
			LogPrintf("%s +0x%x\r\n", pSymbol->Name, symDisplacement);
			pUnDecorateSymbolName(pSymbol->Name, UnDName, sizeof(UnDName),
				UNDNAME_COMPLETE);

			IMAGEHLP_LINE64 line;
			DWORD lineDisplacement = 0;
			line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
			if (SymGetLineFromAddr64(GetCurrentProcess(), sf.AddrPC.Offset,
					&lineDisplacement, &line))
			{
				LogPrintf("%s(%d) : %s\r\n", line.FileName, line.LineNumber,
					UnDName);
				goto PrintParams;
			}
		}

		{
			char crashModulePathname[MAX_PATH];
			GetModuleFileNameWithAddress((void*)sf.AddrPC.Offset,
				crashModulePathname, MAX_PATH);
			LogPrintf("%s %s\r\n", crashModulePathname, UnDName);
		}

PrintParams:
		LogPrintf("Params: %08x %08x %08x %08x\r\n\r\n", sf.Params[0],
			sf.Params[1], sf.Params[2], sf.Params[3]);
	}
}

void CExceptionReport::DumpStackTrace(const CONTEXT* contextRecord) const
{
	g_dwStack = contextRecord->Esp & ~31;

	__asm
		{
		mov eax, fs:[4]
		mov g_dwStackBottom, eax
		}
	g_dwStackBottom &= ~31;

	LogPrintf("----------------------------------------\r\n");
	LogPrintf("  Stack Trace (Using DBGHELP.DLL)\r\n");
	LogPrintf("----------------------------------------\r\n");

	if (!pSymInitialize(GetCurrentProcess(), NULL, TRUE))
		return;

	DWORD dwOptions = pSymGetOptions();
	if ((dwOptions &
			(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES)) !=
		(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES))
		pSymSetOptions((dwOptions & ~SYMOPT_UNDNAME) |
			(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES));

	CONTEXT context = *contextRecord;
	ImageHelpStackWalk(&context);

	pSymCleanup(GetCurrentProcess());
}

void CExceptionReport::DumpMemory(const CONTEXT* contextRecord) const
{
	LogPrintf("----------------------------------------\r\n");
	LogPrintf("  Memory dump\r\n");
	LogPrintf("----------------------------------------\r\n");
	LogPrintf("Code: %d Bytes starting at CS:EIP = %08x:%08x\r\n\r\n", 16,
		contextRecord->SegCs, contextRecord->Eip);

	BYTE* ptr = (BYTE*)contextRecord->Eip;
	unsigned int i, j;
	for (i = 0; i < 16; i++)
	{
		if (((i + 1) & 15) == 1)
			LogPrintf("%08x: ", ptr + i);

		__try
		{
			LogPrintf("%02x ", ptr[i]);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			LogPrintf("?? ");
		}

		if (!((i + 1) & 3))
			LogPrintf(" ");

		if (!((i + 1) & 15))
		{
			for (j = i - 16; j < i; j++)
			{
				__try
				{
					LogPrintf("%c", isprint(ptr[j]) ? ptr[j] : '.');
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					LogPrintf("?");
				}
			}
			LogPrintf("\r\n");
		}
	}

	LogPrintf("\r\n");
	LogPrintf("Stack: %d Bytes starting at ESP = %08x\r\n", 1024,
		contextRecord->Esp);
	LogPrintf("\r\n* = addr\r\n");
	LogPrintf("          ");
	for (i = 0; i < (contextRecord->Esp & 15); i++)
	{
		LogPrintf("   ");
		if (!((i + 1) & 3))
			LogPrintf(" ");
	}
	LogPrintf("**\r\n");

	ptr = (BYTE*)(contextRecord->Esp - (contextRecord->Esp & 15));
	for (i = 0; i < 1024; i++)
	{
		if (((i + 1) & 15) == 1)
			LogPrintf("%08x: ", ptr + i);

		__try
		{
			LogPrintf("%02x ", ptr[i]);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			LogPrintf("?? ");
		}

		if (!((i + 1) & 3))
			LogPrintf(" ");

		if (!((i + 1) & 15))
		{
			for (j = i - 16; j < i; j++)
			{
				__try
				{
					LogPrintf("%c", isprint(ptr[j]) ? ptr[j] : '.');
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					LogPrintf("?");
				}
			}
			LogPrintf("\r\n");
		}
	}
	LogPrintf("\r\n");
}

__declspec(naked) DWORD SnapCurrentContext(CONTEXT* pContext)
{
	CONTEXT initialContext;
	DWORD eRetVal;

	__asm
		{
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		mov initialContext.Eax, eax
		mov initialContext.Ebx, ebx
		mov initialContext.Ecx, ecx
		mov initialContext.Edx, edx
		mov initialContext.Edi, edi
		mov initialContext.Esi, esi

		xor eax, eax
		mov ax, gs
		mov initialContext.SegGs, eax
		mov ax, fs
		mov initialContext.SegFs, eax
		mov ax, es
		mov initialContext.SegEs, eax
		mov ax, ds
		mov initialContext.SegDs, eax
		mov ax, cs
		mov initialContext.SegCs, eax
		mov ax, ss
		mov initialContext.SegSs, eax

		mov eax, [ebp]
		mov initialContext.Ebp, eax

		mov eax, ebp
		add eax, 8
		mov initialContext.Esp, eax

		push esi
		push edi
		push ebx
		push ecx
		push edx
		}

	memset(pContext, 0, sizeof(CONTEXT));
	pContext->ContextFlags = CONTEXT_FULL | CONTEXT_FLOATING_POINT |
		CONTEXT_DEBUG_REGISTERS | CONTEXT_EXTENDED_REGISTERS;

	if (GetThreadContext(CExceptionReport::GetCurrentThread(), pContext))
	{
		pContext->Eax = initialContext.Eax;
		pContext->Ebx = initialContext.Ebx;
		pContext->Ecx = initialContext.Ecx;
		pContext->Edx = initialContext.Edx;
		pContext->Edi = initialContext.Edi;
		pContext->Esi = initialContext.Esi;
		pContext->SegGs = initialContext.SegGs;
		pContext->SegFs = initialContext.SegFs;
		pContext->SegEs = initialContext.SegEs;
		pContext->SegDs = initialContext.SegDs;
		pContext->SegCs = initialContext.SegCs;
		pContext->SegSs = initialContext.SegSs;
		pContext->Ebp = initialContext.Ebp;
		pContext->Eip = (DWORD)_ReturnAddress();
		eRetVal = TRUE;
	}
	else
	{
		eRetVal = FALSE;
	}

	__asm
	{
		pop edx
		pop ecx
		pop ebx
		pop edi
		pop esi
		mov eax, eRetVal
		mov esp, ebp
		pop ebp
		ret
	}
}

void CExceptionReport::DumpExceptionReport(EXCEPTION_POINTERS* data)
{
	SYSTEMTIME time;
	char name[64];
	char filename[64];
	CONTEXT stContext;
	EXCEPTION_RECORD stExRec;
	EXCEPTION_POINTERS stExpPtrs;

	GetLocalTime(&time);
	wsprintfA(m_crashTime, "%04d/%02d/%02d %02d:%02d:%02d.%03d %s", time.wYear,
		time.wMonth, time.wDay, (time.wHour % 12) == 0 ? 12 : time.wHour % 12,
		time.wMinute, time.wSecond, time.wMilliseconds,
		time.wHour >= 12 ? "PM" : "AM");

	strcpy(name, "exception");

	if (!data)
	{
		SnapCurrentContext(&stContext);

		memset(&stExRec, 0, sizeof(stExRec));
		stExRec.ExceptionAddress = (PVOID)stContext.Eip;

		memset(&stExpPtrs, 0, sizeof(stExpPtrs));
		stExpPtrs.ContextRecord = &stContext;
		stExpPtrs.ExceptionRecord = &stExRec;

		data = &stExpPtrs;
	}

	wsprintfA(filename, "%s.dmp", name);
	CreateMiniDump(filename, data);

	CollectSystemInfo();
	wsprintfA(filename, "%s.log", name);
	StartLog(filename);
	DumpHeader(data);
	DumpSystemInfo();
	DumpErrorMessage(data->ContextRecord, data->ExceptionRecord);
	DumpRegisters(data->ContextRecord);
	DumpStackTrace(data->ContextRecord);
	DumpMemory(data->ContextRecord);
	EndLog();

	wsprintfA(filename, "%s.log", "stack");
	StartLog(filename);
	DumpHeaderInfo(data);
	EndStackLog();
}

void CExceptionReport::SetThreadInfo(HANDLE hThread, unsigned long dwThreadId)
{
	m_curThread = hThread;
	m_curThreadId = dwThreadId;
}

HANDLE CExceptionReport::GetCurrentThread()
{
	if (m_curThread == INVALID_HANDLE_VALUE)
		m_curThread = ::GetCurrentThread();
	return m_curThread;
}

unsigned long CExceptionReport::GetCurrentThreadId()
{
	if (!m_curThreadId)
		m_curThreadId = ::GetCurrentThreadId();
	return m_curThreadId;
}
