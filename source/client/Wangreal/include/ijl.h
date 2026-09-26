#pragma once

typedef enum
{
	IJL_JFILE_READPARAMS = 0,
	IJL_JBUFF_READPARAMS = 1,
	IJL_JFILE_READWHOLEIMAGE = 2,
	IJL_JBUFF_READWHOLEIMAGE = 3,
	IJL_JFILE_READHEADER = 4,
	IJL_JBUFF_READHEADER = 5,
	IJL_JFILE_READENTROPY = 6,
	IJL_JBUFF_READENTROPY = 7,
	IJL_JFILE_WRITEWHOLEIMAGE = 8,
	IJL_JBUFF_WRITEWHOLEIMAGE = 9,
	IJL_JFILE_WRITEHEADER = 10,
	IJL_JBUFF_WRITEHEADER = 11,
	IJL_JFILE_WRITEENTROPY = 12,
	IJL_JBUFF_WRITEENTROPY = 13,
	IJL_JFILE_READONEHALF = 14,
	IJL_JBUFF_READONEHALF = 15,
	IJL_JFILE_READONEQUARTER = 16,
	IJL_JBUFF_READONEQUARTER = 17,
	IJL_JFILE_READONEEIGHTH = 18,
	IJL_JBUFF_READONEEIGHTH = 19,
	IJL_JFILE_READTHUMBNAIL = 20,
	IJL_JBUFF_READTHUMBNAIL = 21
} IJLIOTYPE;

typedef enum
{
	IJL_RGB = 1,
	IJL_BGR = 2,
	IJL_YCBCR = 3,
	IJL_G = 4,
	IJL_RGBA_FPX = 5,
	IJL_YCBCRA_FPX = 6,
	IJL_OTHER = 255
} IJL_COLOR;

typedef enum
{
	IJL_OK = 0,
	IJL_INTERRUPT_OK = 1,
	IJL_ROI_OK = 2
} IJLERR;

typedef struct
{
	unsigned int UseJPEGPROPERTIES;
	unsigned char* DIBBytes;
	int DIBWidth;
	int DIBHeight;
	int DIBPadBytes;
	int DIBChannels;
	IJL_COLOR DIBColor;
	int DIBSubsampling;
	const char* JPGFile;
	unsigned char* JPGBytes;
	int JPGSizeBytes;
	int JPGWidth;
	int JPGHeight;
	int JPGChannels;
	IJL_COLOR JPGColor;
	int JPGSubsampling;
	int JPGThumbWidth;
	int JPGThumbHeight;
	int cconversion_reqd;
	int upsampling_reqd;
	int jquality;
	unsigned char jprops[19988];
} JPEG_CORE_PROPERTIES;

extern "C"
{
	IJLERR __stdcall ijlInit(JPEG_CORE_PROPERTIES* jcprops);
	IJLERR __stdcall ijlFree(JPEG_CORE_PROPERTIES* jcprops);
	IJLERR __stdcall ijlRead(JPEG_CORE_PROPERTIES* jcprops, IJLIOTYPE iotype);
}
