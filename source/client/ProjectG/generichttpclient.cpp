#include "stdafx.h"
#include "generichttpclient.h"
#include <tchar.h>

void PrintLastError(unsigned long error)
{
	char* message;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&message,
		0, NULL);
	LocalFree(message);
}

GenericHTTPClient::GenericHTTPClient()
{
	_hHTTPRequest = NULL;
	_hHTTPConnection = NULL;
	_hHTTPOpen = NULL;
	_dwSecureFlag = 0;
}

GenericHTTPClient::~GenericHTTPClient()
{
	_hHTTPRequest = NULL;
	_hHTTPConnection = NULL;
	_hHTTPOpen = NULL;
}

GenericHTTPClient::enumRequestMethod GenericHTTPClient::GetMethod(int method)
{
	return method <= 0 ? RequestGetMethod : (enumRequestMethod)method;
}

GenericHTTPClient::enumTypePostArgument GenericHTTPClient::GetPostArgumentType(
	int type)
{
	return type <= 0 ? TypeNormal : (enumTypePostArgument)type;
}

int GenericHTTPClient::Connect(const char* address, const char* agent,
	unsigned short port, const char* user, const char* password)
{
	_hHTTPOpen = InternetOpen(agent, INTERNET_OPEN_TYPE_PRECONFIG, "", "", 0);
	if (!_hHTTPOpen)
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	_hHTTPConnection = InternetConnect(_hHTTPOpen, address, port, user,
		password, INTERNET_SERVICE_HTTP,
		INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if (!_hHTTPConnection)
	{
		_dwError = ::GetLastError();
		CloseHandle(_hHTTPOpen);
		return FALSE;
	}

	if (InternetAttemptConnect(0) != ERROR_SUCCESS)
	{
		_dwError = ::GetLastError();
		CloseHandle(_hHTTPConnection);
		CloseHandle(_hHTTPOpen);
		return FALSE;
	}

	return TRUE;
}

int GenericHTTPClient::Close()
{
	if (_hHTTPRequest)
		InternetCloseHandle(_hHTTPRequest);
	if (_hHTTPConnection)
		InternetCloseHandle(_hHTTPConnection);
	if (_hHTTPOpen)
		InternetCloseHandle(_hHTTPOpen);

	return TRUE;
}

int GenericHTTPClient::Request(const char* url, int method, const char* agent)
{
	char protocol[1024] = { 0 }, address[1024] = { 0 }, uri[1024] = { 0 };
	BOOL result = TRUE;
	DWORD port = 0, size = 0;

	ParseURL(url, protocol, address, port, uri);
	_dwSecureFlag =
		port == INTERNET_DEFAULT_HTTPS_PORT ? INTERNET_FLAG_SECURE : 0;

	if (Connect(address, agent, (unsigned short)port, NULL, NULL))
	{
		if (result && result)
		{
			if (RequestOfURI(uri, method))
			{
				if (!Response((unsigned char*)_szHTTPResponseHeader,
						sizeof(_szHTTPResponseHeader),
						(unsigned char*)_szHTTPResponseHTML,
						sizeof(_szHTTPResponseHTML), size))
					result = FALSE;
			}
			else
				result = FALSE;
		}
		Close();
		return static_cast<volatile BOOL&>(result);
	}

	return FALSE;
}

int GenericHTTPClient::RequestOfURI(const char* uri, int method)
{
	switch (method)
	{
	case RequestPostMethod:
		return RequestPost(uri);
	case RequestPostMethodMultiPartsFormData:
		return RequestPostMultiPartsFormData(uri);
	default:
		return RequestGet(uri);
	}
}

int GenericHTTPClient::RequestGet(const char* uri)
{
	const char* accept[] = { "*/*", NULL };
	_hHTTPRequest = HttpOpenRequest(_hHTTPConnection, "GET", uri, "HTTP/1.0",
		"", accept, _dwSecureFlag | 0x4400000, 0);
	if (!_hHTTPRequest)
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	if (!HttpAddRequestHeaders(_hHTTPRequest, "Accept: */*\r\n", 13,
			HTTP_ADDREQ_FLAG_REPLACE))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	if (!HttpSendRequest(_hHTTPRequest, NULL, 0, NULL, 0))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	return TRUE;
}

int GenericHTTPClient::RequestPost(const char* uri)
{
	char arguments[1024] = { 0 };
	const char* accept[] = { "*/*", NULL };
	GetPostArguments(arguments, sizeof(arguments));
	_hHTTPRequest = HttpOpenRequest(_hHTTPConnection, "POST", uri, "HTTP/1.0",
		"", accept, _dwSecureFlag | 0x4400040, 0);
	const char* contentType =
		"Content-Type: application/x-www-form-urlencoded\r\n";

	if (!_hHTTPRequest)
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	if (!HttpAddRequestHeaders(_hHTTPRequest, "Accept: */*\r\n", 13,
			HTTP_ADDREQ_FLAG_REPLACE))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	if (!HttpSendRequest(_hHTTPRequest, contentType, strlen(contentType),
			arguments, strlen(arguments)))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	return TRUE;
}

int GenericHTTPClient::RequestPostMultiPartsFormData(const char* uri)
{
	const char* contentType =
		"Content-Type: multipart/form-data; boundary=--MULTI-PARTS-FORM-DATA-BOUNDARY\r\n";
	const char* accept[] = { "*/*", NULL };
	unsigned char* buffer = NULL;
	DWORD size =
		AllocMultiPartsFormData(buffer, "--MULTI-PARTS-FORM-DATA-BOUNDARY");
	_hHTTPRequest = HttpOpenRequest(_hHTTPConnection, "POST", uri, "HTTP/1.0",
		"", accept, _dwSecureFlag | 0x4400040, 0);
	if (!_hHTTPRequest)
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	if (!HttpAddRequestHeaders(_hHTTPRequest, "Accept: */*\r\n", 13,
			HTTP_ADDREQ_FLAG_REPLACE))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	if (!HttpAddRequestHeaders(_hHTTPRequest, contentType, strlen(contentType),
			HTTP_ADDREQ_FLAG_ADD_IF_NEW))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	char length[1024];
	memset(length, 0, sizeof(length));
	sprintf(length, "Content-Length: %d\r\n", size);
	if (!HttpAddRequestHeaders(_hHTTPRequest, length, strlen(length),
			HTTP_ADDREQ_FLAG_ADD_IF_NEW))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	INTERNET_BUFFERS buffers = { 0 };
	buffers.dwStructSize = sizeof(INTERNET_BUFFERS);
	buffers.Next = NULL;
	if (!HttpSendRequestEx(_hHTTPRequest, &buffers, NULL, HSR_INITIATE, 0))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	DWORD written = 0;
	if (!InternetWriteFile(_hHTTPRequest, buffer, size, &written))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	if (!HttpEndRequest(_hHTTPRequest, NULL, HSR_INITIATE, 0))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	FreeMultiPartsFormData(buffer);
	return TRUE;
}

unsigned long GenericHTTPClient::ResponseOfBytes(unsigned char* buffer,
	unsigned long size)
{
	static DWORD bytesRead;
	if (!_hHTTPRequest)
	{
		_dwError = ::GetLastError();
		return 0;
	}

	memset(buffer, 0, size);
	if (!InternetReadFile(_hHTTPRequest, buffer, size, &bytesRead))
	{
		_dwError = ::GetLastError();
		return 0;
	}

	return bytesRead;
}

int GenericHTTPClient::Response(unsigned char* header,
	unsigned long headerLength, unsigned char* output, unsigned long length,
	unsigned long& resultSize)
{
	unsigned char buffer[1024] = { 0 };
	if (!_hHTTPRequest)
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	memset(output, 0, length);
	resultSize = 0;
	DWORD size;
	while ((size = ResponseOfBytes(buffer, sizeof(buffer))) != 0 &&
		resultSize < length)
	{
		memcpy(output + resultSize, buffer, size);
		resultSize += size;
	}

	memset(header, 0, headerLength);
	if (!HttpQueryInfo(_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, header,
			&headerLength, NULL))
	{
		_dwError = ::GetLastError();
		return FALSE;
	}

	return resultSize != 0;
}

void GenericHTTPClient::AddPostArguments(const char* name, const char* value,
	int binary)
{
	GenericHTTPArgument arg;
	memset(&arg, 0, sizeof(arg));
	strcpy(arg.szName, name);
	strcpy(arg.szValue, value);
	arg.dwType = binary ? TypeBinary : TypeNormal;
	_vArguments.push_back(arg);
}

void GenericHTTPClient::AddPostArguments(const char* name, unsigned long value)
{
	GenericHTTPArgument arg;
	memset(&arg, 0, sizeof(arg));
	strcpy(arg.szName, name);
	sprintf(arg.szValue, "%d", value);
	arg.dwType = TypeNormal;
	_vArguments.push_back(arg);
}

unsigned long GenericHTTPClient::GetPostArguments(char* arguments,
	unsigned long length)
{
	memset(arguments, 0, length);
	std::vector<GenericHTTPArgument>::iterator it;
	for (it = _vArguments.begin(); it < _vArguments.end();)
	{
		strcat(arguments, it->szName);
		strcat(arguments, "=");
		strcat(arguments, it->szValue);
		++it;
		if (it < _vArguments.end())
			strcat(arguments, "&");
	}

	arguments[length] = 0;
	return strlen(arguments);
}

void GenericHTTPClient::InitilizePostArguments()
{
	_vArguments.clear();
}

void GenericHTTPClient::FreeMultiPartsFormData(unsigned char*& buffer)
{
	if (buffer)
	{
		LocalFree(buffer);
		buffer = NULL;
	}
}

unsigned long GenericHTTPClient::AllocMultiPartsFormData(unsigned char*& buffer,
	const char* boundary)
{
	FreeMultiPartsFormData(buffer);
	buffer = (unsigned char*)LocalAlloc(LPTR, GetMultiPartsFormDataLength());
	unsigned long total = 0, size = 0;
	std::vector<GenericHTTPArgument>::iterator it;
	for (it = _vArguments.begin(); it < _vArguments.end(); ++it)
	{
		char* part = NULL;
		switch (it->dwType)
		{
		case TypeNormal:
		{
			part = (char*)LocalAlloc(LPTR, 8192);
			sprintf(part,
				"--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n",
				boundary, it->szName, it->szValue);
			size = strlen(part);
			break;
		}
		case TypeBinary:
		{
			DWORD bytesRead = 0, offset = 0;
			HANDLE file = CreateFile(it->szValue, GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (file != INVALID_HANDLE_VALUE)
			{
				DWORD fileSize = GetFileSize(file, NULL);
				part = (char*)LocalAlloc(LPTR, fileSize + 6145);
				char chunk[1025] = { 0 };
				sprintf(part,
					"--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n",
					boundary, it->szName, it->szValue,
					GetContentType(it->szValue));
				DWORD headerSize = strlen(part);
				while (ReadFile(file, chunk, 1024, &bytesRead, NULL) &&
					bytesRead > 0 && offset <= fileSize)
				{
					memcpy(part + (headerSize + offset), chunk, bytesRead);
					memset(chunk, 0, sizeof(chunk));
					offset += bytesRead;
				}
				memcpy(part + (headerSize + offset), "\r\n", 2);
				CloseHandle(file);
				size = headerSize + offset + 2;
			}
			break;
		}
		}
		memcpy(buffer + total, part, size);
		total += size;
		if (part)
			LocalFree(part);
	}

	memcpy(buffer + total, "--", 2);
	memcpy(buffer + (total + 2), boundary, strlen(boundary));
	memcpy(buffer + (total + strlen(boundary) + 2), "--\r\n", 4);
	return total + strlen(boundary) + 6;
}

unsigned long GenericHTTPClient::GetMultiPartsFormDataLength()
{
	unsigned long length = 0;
	std::vector<GenericHTTPArgument>::iterator it;
	for (it = _vArguments.begin(); it < _vArguments.end(); ++it)
	{
		switch (it->dwType)
		{
		case TypeNormal:
			length += 8192;
			break;
		case TypeBinary:
		{
			HANDLE file = CreateFile(it->szValue, GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file != INVALID_HANDLE_VALUE)
			{
				length += GetFileSize(file, NULL) + 6144;
				CloseHandle(file);
			}
			break;
		}
		}
	}

	return length;
}

const char* GenericHTTPClient::GetContentType(const char* name)
{
	static char contentType[1024];
	DWORD length = sizeof(contentType);
	unsigned int i;
	for (i = strlen(name); i > 0; --i)
		if (!_tcsncmp(name + i, ".", 1))
			break;
	HKEY key;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, name + i, 0, KEY_QUERY_VALUE, &key) ==
		ERROR_SUCCESS)
	{
		if (RegQueryValueEx(key, "Content Type", NULL, NULL, (BYTE*)contentType,
				&length) != ERROR_SUCCESS)
			_tcsncpy(contentType, "application/octet-stream", 24);
		RegCloseKey(key);
		return contentType;
	}
	_tcsncpy(contentType, "application/octet-stream", 24);
	return contentType;
}

unsigned long GenericHTTPClient::GetLastError()
{
	return _dwError;
}

void GenericHTTPClient::ParseURL(const char* url, char* protocol, char* address,
	unsigned long& port, char* uri)
{
	unsigned int i;
	for (i = 0; strlen(url) && i < strlen(url); ++i)
		if (!_tcsncmp(url + i, ":", 1))
			break;
	unsigned int start;
	if (!_tcsncmp(url + i + 1, "/", 1))
	{
		if (protocol)
		{
			_tcsncpy(protocol, url, i);
			protocol[i] = 0;
		}
		start = (i += 3);
	}
	else
	{
		if (protocol)
		{
			_tcsncpy(protocol, "http", 4);
			protocol[5] = 0;
		}
		start = i = 0;
	}
	for (; strlen(url) && i < strlen(url); ++i)
		if (!_tcsncmp(url + i, "/", 1))
			break;
	bool found = false;
	unsigned int colon;
	for (colon = start; colon <= i; ++colon)
	{
		if (!_tcsncmp(url + colon, ":", 1))
		{
			char number[1024] = { 0 };
			_tcsncpy(number, url + colon + 1, i - colon);
			port = atol(number);
			_tcsncpy(address, url + start, colon - start);
			found = true;
			break;
		}
	}

	if (!found)
	{
		if (!_tcscmp(protocol, "https"))
		{
			port = INTERNET_DEFAULT_HTTPS_PORT;
			_tcsncpy(address, url + start, i - start);
		}
		else
		{
			port = INTERNET_DEFAULT_HTTP_PORT;
			_tcsncpy(address, url + start, i - start);
		}
	}

	if (i < strlen(url))
		_tcsncpy(uri, url + i, strlen(url) - i);
	else
		uri[0] = 0;
}

namespace ILHTTP
{
	struct Pad
	{
		int v;
		void g()
		{
			ILFILL64 ILFILL4 ILFILL4 v += 1;
			v += 1;
		}
	};
}

const char* GenericHTTPClient::QueryHTTPResponseHeader()
{
	return _szHTTPResponseHeader;
}

const char* GenericHTTPClient::QueryHTTPResponse()
{
	return _szHTTPResponseHTML;
}
