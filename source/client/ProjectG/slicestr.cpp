#include "slicestr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static char s_delimiter[] = "*";

bool WSliceStr::Get(const char* key, const char* format, ...)
{
	if (m_str == NULL)
		return false;

	va_list ap;
	char buf[256];
	float fValue;
	int i;
	const char* p;

	p = strstr(m_str, key);

	if (p != NULL)
	{
		p += strlen(key);

		va_start(ap, format);

		while (*format)
		{
			if (*format++ == '%')
			{
				while (*p == ' ' || *p == '\t')
					p++;

				for (i = 0; (unsigned char)(buf[i] = *p++) > ' '; i++)
					if (strchr(s_delimiter, buf[i]))
						break;
				buf[i] = '\0';

				switch (*format++)
				{
				case 'f':
				case 'F':
				case 'g':
				case 'G':
					fValue = (float)atof(buf);
					memcpy(va_arg(ap, float*), &fValue, sizeof(fValue));
					break;
				case 'd':
				case 'D':
				case 'u':
				case 'U':
					*va_arg(ap, int*) = atoi(buf);
					break;
				case 'x':
				case 'X':
					sscanf(buf, "%x", va_arg(ap, int*));
					break;
				case 's':
				case 'S':
					strcpy(va_arg(ap, char*), buf);
					break;
				}
			}
		}

		va_end(ap);
		return true;
	}

	return false;
}
