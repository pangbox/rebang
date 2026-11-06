#include "stdafx.h"
#include "../../shared/sharedtables.h"
#include "mathconsts.h"
#include "guildactor.h"
#include "hatmanager.h"
#include "guilddefine.h"
#include "XUnzip.h"
#include "avatar_task.h"
#include "messengerdlg.h"
#include "gmtoolkit.h"
#include <io.h>
#include <wchar.h>
extern "C" __declspec(dllimport) int __cdecl mkdir(const char*);
#include "fresh.h"
#include "frwndmanager.h"
#include "fremoticon.h"

extern Fresh* g_pFresh;

extern unsigned long g_CurrentTime;

const char* GetCharsTillSpace(const char* pSrc, char* pDest);

unsigned short FakeTable[28] = { ' ', '`', '~', '!', '@', '#', '$', '%', '^',
	'&', '*', '(', ')', '-', '_', '=', '+', '|', ';', ':', '\'', '"', ',', '<',
	'.', '>', '/', '?' };

sChatCmd ChatCmd[3] = {
	{ "identity" },
	{ "notice" },
	{ "itemdrop" },
};

#pragma init_seg(lib)

// clang-format off
const char* FacialTextTable[4][12] = {
	{ K2L_Compatibility("^^"), K2L_Compatibility("\xa4\xbb\xa4\xbb"), K2L_Compatibility("\xa4\xbe\xa4\xbe"), K2L_Compatibility("\xc7\xcf\xc7\xcf"), K2L_Compatibility("\xc8\xa3\xc8\xa3"), K2L_Compatibility("\xbb\xe7\xb6\xfb"),
		K2L_Compatibility("\xa4\xa3"), K2L_Compatibility("\xb0\xed\xb8\xb6\xbf\xf6"), K2L_Compatibility("\xb1\xe8\xc4\xa1"), K2L_Compatibility("\xb0\xa8\xbb\xe7"), K2L_Compatibility("\xc7\xcf\xc0\xcc"), K2L_Compatibility("\xa4\xbe\xa4\xb7") },
	{ K2L_Compatibility("\xa4\xd0.\xa4\xd0"), K2L_Compatibility("\xa4\xd0\xa4\xd0"), K2L_Compatibility("\xc8\xc7\xc2\xbd"), K2L_Compatibility("OTL"), K2L_Compatibility("T_T"), K2L_Compatibility("\xc0\xe1\xbc\xf6"),
		K2L_Compatibility("\xbf\xec\xbf\xef"), K2L_Compatibility("\xc2\xa1\xc2\xa1"), K2L_Compatibility("orz"), K2L_Compatibility("\xc8\xe6\xc8\xe6"), K2L_Compatibility("\xa4\xcc\xa4\xcc"), K2L_Compatibility("\xa4\xcc.\xa4\xcc") },
	{ K2L_Compatibility("\x94\xee"), K2L_Compatibility("\x94\xe6"), K2L_Compatibility("-_-+"), K2L_Compatibility("\xb9\xf6\xb7\xb0"), K2L_Compatibility("\xc7\xe6"), K2L_Compatibility("\xb0\xad\xc5\xf0"),
		K2L_Compatibility("\xb9\xcc\xbf\xf6"), K2L_Compatibility("\xc2\xaf\xb3\xaa"), K2L_Compatibility("\xc2\xa5\xc1\xf5"), K2L_Compatibility("\xa4\xd1\xa4\xd1+"), K2L_Compatibility("\xc8\xad\xb3\xb2"), K2L_Compatibility("\xa4\xd1\xa4\xd1^") },
};

struct sMotionText
{
	const char* m_name;
	const char* m_command[7];
};

static sMotionText s_motionText[10][15] = {
	{
		{ K2L_Compatibility("\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc1\xc2\xc0\xfd"), K2L_Compatibility("\xc0\xfd\xb8\xc1") } },
		{ K2L_Compatibility("1\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xa4\xbb\xa4\xbb"), K2L_Compatibility("\xa4\xbe\xa4\xbe"), K2L_Compatibility("\xc7\xcf\xc7\xcf"), K2L_Compatibility("\xc0\xdf\xb3\xad\xc3\xbc") } },
		{ K2L_Compatibility("3\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("OTL"), K2L_Compatibility("orz") } },
		{ K2L_Compatibility("\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xbd\xc7\xb8\xc1") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8"), { K2L_Compatibility("\xb3\xaa\xc0\xcc\xbd\xba"), K2L_Compatibility("nice") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0"), { K2L_Compatibility("\xbe\xee\xc0\xcc\xc4\xed"), K2L_Compatibility("\xb2\xca\xb4\xe7"), K2L_Compatibility("\xc0\xb8\xbe\xc7") } },
		{ K2L_Compatibility("\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xb8\xb8\xbc\xbc") } },
		{ K2L_Compatibility("chat_\xb9\xda\xbc\xf6"), { K2L_Compatibility("\xb9\xda\xbc\xf6") } },
		{ K2L_Compatibility("chat_\xc0\xce\xbb\xe7"), { K2L_Compatibility("\xc0\xce\xbb\xe7") } },
		{ K2L_Compatibility("chat_\xb4\xed\xbd\xba"), { K2L_Compatibility("\xb4\xed\xbd\xba") } },
	},
	{
		{ K2L_Compatibility("\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc2\xa5\xc1\xf5") } },
		{ K2L_Compatibility("1\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xb8\xb8\xbc\xbc") } },
		{ K2L_Compatibility("2\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xc8\xaf\xc8\xa3") } },
		{ K2L_Compatibility("3\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xbe\xfb\xbe\xfb") } },
		{ K2L_Compatibility("chat_\xbb\xdf\xc1\xfc"), { K2L_Compatibility("\xc8\xef"), K2L_Compatibility("\xbb\xdf\xc1\xfc") } },
		{ K2L_Compatibility("\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xbe\xee\xb8\xd3") } },
		{ K2L_Compatibility("\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc8\xe6\xc8\xe6"), K2L_Compatibility("\xbf\xef\xc0\xbd") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8"), { K2L_Compatibility("\xb3\xaa\xc0\xcc\xbd\xba"), K2L_Compatibility("nice") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0"), { K2L_Compatibility("OTL"), K2L_Compatibility("orz"), K2L_Compatibility("\xc1\xc2\xc0\xfd"), K2L_Compatibility("\xc0\xfd\xb8\xc1"), K2L_Compatibility("\xc7\xae\xbd\xe2"), K2L_Compatibility("\xc5\xd0\xbd\xe2") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xba\xf1\xc6\xb2") } },
		{ K2L_Compatibility("\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc1\xc1\xbe\xc6"), K2L_Compatibility("\xc3\xe3") } },
		{ K2L_Compatibility("chat_\xb4\xed\xbd\xba"), { K2L_Compatibility("\xb4\xed\xbd\xba") } },
	},
	{
		{ K2L_Compatibility("\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xbf\xc0\xba\xfc") } },
		{ K2L_Compatibility("1\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xb8\xb8\xbc\xbc") } },
		{ K2L_Compatibility("2\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xbf\xb9\xbd\xba"), K2L_Compatibility("Yes") } },
		{ K2L_Compatibility("\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc8\xaf\xc8\xa3"), K2L_Compatibility("\xbd\xc2\xb8\xae") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8"), { K2L_Compatibility("\xb3\xaa\xc0\xcc\xbd\xba"), K2L_Compatibility("nice") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0"), { K2L_Compatibility("\xc4\xe2\xb4\xe7"), K2L_Compatibility("\xbe\xee\xc0\xcc\xc4\xed"), K2L_Compatibility("\xc7\xae\xbd\xe2"), K2L_Compatibility("\xc5\xd0\xbd\xe2") } },
		{ K2L_Compatibility("\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc1\xa8\xc0\xe5"), K2L_Compatibility("\xc8\xad\xb3\xb2"), K2L_Compatibility("\xba\xd0\xb3\xeb") } },
		{ K2L_Compatibility("chat_\xb8\xd3\xbe\xb3"), { K2L_Compatibility("\xb8\xd3\xbe\xb3"), K2L_Compatibility("\xc0\xb8\xc0\xbd"), K2L_Compatibility("...") } },
		{ K2L_Compatibility("chat_\xb4\xed\xbd\xba"), { K2L_Compatibility("\xb4\xed\xbd\xba") } },
	},
	{
		{ K2L_Compatibility("\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xb8\xf4\xb6\xf3") } },
		{ K2L_Compatibility("\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xbe\xc8\xb3\xe7"), K2L_Compatibility("\xa4\xbe\xa4\xb7"), K2L_Compatibility("\xc7\xcf\xc0\xcc") } },
		{ K2L_Compatibility("\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xbf\xb9\xbd\xba"), K2L_Compatibility("Yes") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8"), { K2L_Compatibility("\xb3\xaa\xc0\xcc\xbd\xba"), K2L_Compatibility("nice") } },
		{ K2L_Compatibility("\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xb8\xb8\xbc\xbc") } },
		{ K2L_Compatibility("3\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xc5\xd0\xbd\xe2"), K2L_Compatibility("\xc7\xae\xbd\xe2") } },
		{ K2L_Compatibility("\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xbd\xc7\xb8\xc1") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0"), { K2L_Compatibility("\xbb\xdf\xc1\xfc"), K2L_Compatibility("\xc8\xef") } },
		{ K2L_Compatibility("chat_\xc8\xa3\xc8\xa3"), { K2L_Compatibility("\xc8\xa3\xc8\xa3") } },
		{ K2L_Compatibility("chat_\xba\xea\xc0\xcc"), { K2L_Compatibility("\xba\xea\xc0\xcc"), K2L_Compatibility("V") } },
		{ K2L_Compatibility("chat_\xb4\xed\xbd\xba"), { K2L_Compatibility("\xb4\xed\xbd\xba") } },
	},
	{
		{ K2L_Compatibility("1\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("Yes"), K2L_Compatibility("\xbf\xb9\xbd\xba") } },
		{ K2L_Compatibility("2\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xc0\xce\xbb\xe7"), K2L_Compatibility("\xb0\xa8\xbb\xe7") } },
		{ K2L_Compatibility("\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc1\xc2\xc0\xfd") } },
		{ K2L_Compatibility("\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc8\xca") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc7\xe3\xb9\xab"), K2L_Compatibility("\xc7\xe6") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8"), { K2L_Compatibility("\xb3\xaa\xc0\xcc\xbd\xba"), K2L_Compatibility("nice") } },
		{ K2L_Compatibility("\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xbd\xc7\xb8\xc1") } },
		{ K2L_Compatibility("\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc5\xd7\xb4\xcf\xbd\xba") } },
		{ K2L_Compatibility("chat_\xc2\xf7\xc0\xce"), { K2L_Compatibility("\xc2\xf7\xc0\xce") } },
		{ K2L_Compatibility("chat_\xb4\xed\xbd\xba"), { K2L_Compatibility("\xb4\xed\xbd\xba") } },
	},
	{
		{ K2L_Compatibility("2\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xb8\xd3\xb8\xae") } },
		{ K2L_Compatibility("2\xb5\xee\xb8\xf0\xbc\xc7" "2"), { K2L_Compatibility("\xb8\xb8\xbc\xbc") } },
		{ K2L_Compatibility("3\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xbf\xd5\xb5\xfb") } },
		{ K2L_Compatibility("\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc3\xe3") } },
		{ K2L_Compatibility("\xbf\xec\xb5\xe5\xbc\xa6\xc8\xc4\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xb8\xc1\xbf\xf8\xb0\xe6"), K2L_Compatibility("\xbd\xba\xc5\xe4\xc4\xbf") } },
		{ K2L_Compatibility("\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xc1\xc2\xc0\xfd"), K2L_Compatibility("OTL"), K2L_Compatibility("orz") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xc8\xc4\xb4\xeb\xb1\xe2"), { K2L_Compatibility("\xb3\xaa\xc0\xcc\xbd\xba"), K2L_Compatibility("nice") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8"), { K2L_Compatibility("\xbf\xb9\xbd\xba"), K2L_Compatibility("Yes") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0"), { K2L_Compatibility("\xc5\xd0\xbd\xe2"), K2L_Compatibility("\xc7\xae\xbd\xe2") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xbd\xc7\xb8\xc1") } },
		{ K2L_Compatibility("chat_\xb4\xed\xbd\xba"), { K2L_Compatibility("\xb4\xed\xbd\xba") } },
	},
	{
		{ K2L_Compatibility("\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xb8\xb8\xbc\xbc") } },
		{ K2L_Compatibility("\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xb4\xe4\xb4\xe4"), K2L_Compatibility("\xc7\xd1\xbc\xfb") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xb8\xf4\xb6\xf3"), K2L_Compatibility("\xb9\xce\xb8\xc1") } },
		{ K2L_Compatibility("2\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xbd\xc2\xb8\xae"), K2L_Compatibility("\xbf\xb9\xbd\xba"), K2L_Compatibility("Yes") } },
		{ K2L_Compatibility("3\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xc5\xd0\xbd\xe2"), K2L_Compatibility("\xc7\xae\xbd\xe2") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xc8\xc4\xb4\xeb\xb1\xe2"), { K2L_Compatibility("\xc7\xcf\xc0\xcc"), K2L_Compatibility("\xa4\xbe\xa4\xb7") } },
		{ K2L_Compatibility("\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xba\xf9\xb1\xdb") } },
		{ K2L_Compatibility("chat_\xc5\xb0\xbd\xba"), { K2L_Compatibility("\xc5\xb0\xbd\xba"), K2L_Compatibility("\xc2\xca") } },
		{ K2L_Compatibility("chat_\xbd\xc7\xb8\xc1"), { K2L_Compatibility("\xbd\xc7\xb8\xc1"), K2L_Compatibility("\xbb\xdf\xc1\xfc") } },
		{ K2L_Compatibility("chat_\xbd\xc8\xbe\xee"), { K2L_Compatibility("\xbd\xc8\xbe\xee"), K2L_Compatibility("\xb8\xf4\xb6\xf3") } },
		{ K2L_Compatibility("chat_\xb4\xed\xbd\xba"), { K2L_Compatibility("\xb4\xed\xbd\xba") } },
	},
	{
		{ K2L_Compatibility("3\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xc5\xd0\xbd\xe2"), K2L_Compatibility("\xc7\xae\xbd\xe2") } },
		{ K2L_Compatibility("\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"), { K2L_Compatibility("\xba\xd0\xb3\xeb"), K2L_Compatibility("\xbd\xc7\xb8\xc1") } },
		{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0"), { K2L_Compatibility("\xbe\xc8\xb5\xc5"), K2L_Compatibility("\xc1\xc2\xc0\xfd") } },
		{ K2L_Compatibility("2\xb5\xee\xb8\xf0\xbc\xc7"), { K2L_Compatibility("\xbf\xb9\xbd\xba"), K2L_Compatibility("Yes") } },
		{ K2L_Compatibility("chat_\xb8\xb8\xbc\xbc"), { K2L_Compatibility("\xb8\xb8\xbc\xbc") } },
		{ K2L_Compatibility("chat_\xb4\xed\xbd\xba"), { K2L_Compatibility("\xb4\xed\xbd\xba") } },
	},
	{
		{ K2L_Compatibility("chat_\xbd\xc7\xb8\xc1"), { K2L_Compatibility("\xbd\xc7\xb8\xc1") } },
		{ K2L_Compatibility("chat_\xc8\xad\xb3\xb2"), { K2L_Compatibility("\xc8\xad\xb3\xb2") } },
		{ K2L_Compatibility("chat_\xc7\xcf\xc6\xae"), { K2L_Compatibility("\xc7\xcf\xc6\xae") } },
		{ K2L_Compatibility("chat_\xc3\xe3"), { K2L_Compatibility("\xc3\xe3") } },
		{ K2L_Compatibility("chat_\xc7\xcf\xc0\xcc"), { K2L_Compatibility("\xc7\xcf\xc0\xcc") } },
		{ K2L_Compatibility("chat_\xb8\xb8\xbc\xbc"), { K2L_Compatibility("\xb8\xb8\xbc\xbc") } },
		{ K2L_Compatibility("chat_\xb3\xaa\xc0\xcc\xbd\xba"), { K2L_Compatibility("\xb3\xaa\xc0\xcc\xbd\xba") } },
		{ K2L_Compatibility("chat_\xbe\xc8\xb3\xe7"), { K2L_Compatibility("\xbe\xc8\xb3\xe7") } },
		{ K2L_Compatibility("chat_\xbe\xfb\xbe\xfb"), { K2L_Compatibility("\xbe\xfb\xbe\xfb") } },
		{ K2L_Compatibility("chat_\xc5\xb0\xbd\xba"), { K2L_Compatibility("\xc5\xb0\xbd\xba") } },
		{ K2L_Compatibility("chat_\xba\xce\xb2\xf4"), { K2L_Compatibility("\xba\xce\xb2\xf4") } },
		{ K2L_Compatibility("chat_\xb4\xed\xbd\xba"), { K2L_Compatibility("\xb4\xed\xbd\xba") } },
	},
};
// clang-format on

namespace BABO_STR
{
	unsigned short TryToLower(unsigned short ch)
	{
		return (iswascii(ch) && iswupper(ch)) ? towlower(ch) : ch;
	}

	bool WStrStr(char* out, unsigned short* text, int textLen,
		unsigned short* word, int wordLen, bool bFindOnly)
	{
		bool found = false;

		for (int i = 0; i < textLen - 1; i++)
		{
			if (TryToLower(text[i]) == TryToLower(word[0]))
			{
				unsigned short* textPos = &text[i + 1];
				unsigned short* wordPos = &word[1];

				while (*textPos != 0 && *wordPos != 0)
				{
					if (TryToLower(*textPos) != TryToLower(*wordPos))
					{
						int k;
						for (k = 0; k < 28; k++)
						{
							if (*textPos == FakeTable[k])
							{
								break;
							}
						}

						if (k == 28)
						{
							break;
						}
					}
					else
					{
						wordPos++;
					}
					textPos++;
				}

				if (wordPos - word == wordLen - 1)
				{
					if (bFindOnly)
					{
						return true;
					}

					found = true;
					int matchLen = textPos - &text[i];
					for (int j = 0; j < matchLen; j++)
					{
						int k;
						for (k = 0; k < 28; k++)
						{
							if (text[i + j] == FakeTable[k])
							{
								break;
							}
						}

						if (k == 28)
						{
							text[i + j] = 0x2665;
						}
					}
				}
			}
		}

		if (found)
		{
			WideCharToMultiByte(0, 0, text, -1, out, 0x400, 0, 0);
		}

		return found;
	}

	bool WStrCmp(unsigned short* word, int wordLen, unsigned short* text,
		int textLen)
	{
		if (wordLen != textLen)
		{
			return true;
		}

		for (int i = 0; i < wordLen - 1; i++)
		{
			if (tolower(word[i]) != tolower(text[i]))
			{
				return true;
			}
		}

		return false;
	}

	bool WStrStr_JP(char* out, unsigned short* text, int textLen,
		unsigned short* word, int wordLen, bool bFindOnly)
	{
		unsigned short* textPos = NULL;
		unsigned short* wordPos = NULL;
		bool found = false;

		for (int i = 0; i < textLen - 1; i++)
		{
			if (text[i] < 0x80 && word[0] < 0x80)
			{
				if (tolower(text[i]) != tolower(word[0]))
				{
					continue;
				}
			}
			else if (text[i] != word[0])
			{
				continue;
			}

			textPos = &text[i + 1];
			wordPos = &word[1];

			while (*textPos != 0 && *wordPos != 0)
			{
				bool bSame = false;

				if (*textPos < 0x80 && *wordPos < 0x80)
				{
					if (tolower(*textPos) == tolower(*wordPos))
					{
						bSame = true;
					}
				}
				else if (*textPos == *wordPos)
				{
					bSame = true;
				}

				if (!bSame)
				{
					int k;
					for (k = 0; k < 28; k++)
					{
						if (*textPos == FakeTable[k])
						{
							break;
						}
					}

					if (k == 28)
					{
						break;
					}
				}
				else
				{
					wordPos++;
				}

				textPos++;
			}

			if (wordPos - word != wordLen - 1)
			{
				continue;
			}

			if (bFindOnly)
			{
				return true;
			}

			found = true;

			static const char* mark = K2L_Compatibility("\xa2\xbe");
			unsigned short wideMark[32];
			int converted =
				MultiByteToWideChar(0, 0, mark, strlen(mark) + 1, wideMark, 32);

			if (mark != 0)
			{
				if (converted != 0)
				{
					for (int j = 0; j < textPos - &text[i]; j++)
					{
						int k;
						for (k = 0; k < 28; k++)
						{
							if (text[i + j] == FakeTable[k])
							{
								break;
							}
						}

						if (k == 28)
						{
							text[i + j] = wideMark[0];
						}
					}
				}
				else
				{
					for (int j = 0; j < textPos - &text[i]; j++)
					{
						int k;
						for (k = 0; k < 28; k++)
						{
							if (text[i + j] == FakeTable[k])
							{
								break;
							}
						}

						if (k == 28)
						{
							text[i + j] = 0x2665;
						}
					}
				}
			}
			else
			{
				for (int j = 0; j < textPos - &text[i]; j++)
				{
					int k;
					for (k = 0; k < 28; k++)
					{
						if (text[i + j] == FakeTable[k])
						{
							break;
						}
					}

					if (k == 28)
					{
						text[i + j] = 0x2665;
					}
				}
			}
		}

		if (found)
		{
			WideCharToMultiByte(0, 0, text, -1, out, 0x400, 0, 0);
		}

		return found;
	}

	bool WStrCmp_JP(unsigned short* word, int wordLen, unsigned short* text,
		int textLen)
	{
		if (wordLen != textLen)
		{
			return true;
		}

		for (int i = 0; i < wordLen - 1; i++)
		{
			if (word[i] < 0x80 && *text < 0x80)
			{
				if (tolower(word[i]) != tolower(text[i]))
				{
					return true;
				}
			}
			else
			{
				if (word[i] != text[i])
				{
					return true;
				}
			}
		}

		return false;
	}

}

ChatManager::ChatManager(void)
{
	m_swearCount = 0;
	m_floodCount = 0;
	m_lastChatTime = g_CurrentTime;
	m_bSwearPenalty = false;
	m_bFloodPenalty = false;
	m_chatFilter = 0;
	m_nickFilter = 0;
	m_chatFilterNum = 0;
	m_nickFilterNum = 0;
	m_bUnused = false;
}

ChatManager::~ChatManager(void)
{
	if (m_chatFilter)
	{
		delete[] m_chatFilter;
		m_chatFilter = 0;
	}

	if (m_nickFilter)
	{
		delete[] m_nickFilter;
		m_nickFilter = 0;
	}
}

bool ChatManager::Load(void)
{
	bool loaded = false;

	loaded |= LoadChatFilter();
	loaded |= LoadNickFilter();

	return loaded;
}

bool ChatManager::LoadChatFilter(void)
{
	WFile file("chat.bin");

	int length = file.Length();
	char* data = new char[length];

	file.Read(data, length);
	file.Close();

	HZIP zip = OpenZipU(data, length, 3);
	if (zip == 0)
	{
		delete[] data;
		return false;
	}

	ZIPENTRY entry;
	GetZipItemA(zip, -1, &entry);
	int count = entry.index;

	for (int i = 0; i < count; i++)
	{
		GetZipItemA(zip, i, &entry);
		char* item = new char[entry.unc_size];
		UnzipItem(zip, i, item, entry.unc_size, 3);

		for (int k = 0; k < entry.unc_size; k++)
		{
			item[k] = ~item[k];
		}

		m_chatFilterNum = 0;
		int wordCount = *(int*)item;

		m_chatFilterNum = wordCount;
		if (m_chatFilterNum > 0)
		{
			m_chatFilter = new char[m_chatFilterNum][30];
			memcpy(m_chatFilter, item + 4, m_chatFilterNum * 30);
		}

		delete[] item;
	}

	CloseZipU(zip);
	delete[] data;

	return true;
}

bool ChatManager::LoadNickFilter(void)
{
	WFile file("nick.bin");

	int length = file.Length();
	char* data = new char[length];

	file.Read(data, length);
	file.Close();

	HZIP zip = OpenZipU(data, length, 3);
	if (zip == 0)
	{
		delete[] data;
		return false;
	}

	ZIPENTRY entry;
	GetZipItemA(zip, -1, &entry);
	int count = entry.index;

	for (int i = 0; i < count; i++)
	{
		GetZipItemA(zip, i, &entry);
		char* item = new char[entry.unc_size];
		UnzipItem(zip, i, item, entry.unc_size, 3);

		for (int k = 0; k < entry.unc_size; k++)
		{
			item[k] = ~item[k];
		}

		m_nickFilterNum = 0;
		int wordCount = *(int*)item;

		m_nickFilterNum = wordCount;
		if (m_nickFilterNum > 0)
		{
			m_nickFilter = new sNickFilter[m_nickFilterNum];
			memcpy(m_nickFilter, item + 4,
				m_nickFilterNum * sizeof(sNickFilter));
		}

		delete[] item;
	}

	CloseZipU(zip);
	delete[] data;

	return true;
}

void ChatManager::ParseString(const char*)
{
}

void ChatManager::CheckPenaltyTime(void)
{
	if (Doc()->m_playingMode == 2)
	{
		return;
	}

	sPlayerData* player = 0;
	if (!((__rtti_obj = AfxGetTask()) != 0
				? ((IObject*)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI)
				: false))
	{
		player = PLAYER(MyIndex());
	}

	if (!OnlinePlay() || !player || !player->chatBlock ||
		g_CurrentTime - m_lastChatTime <= 60000)
	{
		return;
	}

	player->chatBlock = false;
	WSendPacket packet((enumClientPacket)0x4f);
	packet.Encode1(0);
	packet.Send(TO_GAME);
}

void ChatManager::SendChatMessage(const char* text, const char* target,
	bool broadcast)
{
	if (IsLocalContent((localContentType_t)0x4F) && Doc()->m_playingMode == 2)
	{
		return;
	}

	int len = strlen(text);

	if (len == 0)
	{
		return;
	}

	int i;
	for (i = 0; i < len; i++)
	{
		if (text[i] != ' ')
		{
			break;
		}
	}

	if (i == len)
	{
		return;
	}

	char lastMessage[0x400];

	if (Doc()->GetFlagBlock() & 8)
	{
		CChatMsg::Instance()->AddChatMsg(
			"(\xbe\xcb\xb8\xb2)\xc3\xa4\xc6\xc3 \xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xc3\xa4\xc6\xc3\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0xFFFF7878, true, false);
		return;
	}

	if (Doc()->m_chatHistory.size() != 0)
	{
		strcpy(lastMessage, Doc()->m_chatHistory.back().c_str());
	}
	else
	{
		lastMessage[0] = 0;
	}

	RecordMyChatHistory(text);

	int command = CheckChatCommand(text, 1);

	if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
	{
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(0, 0x60, 0, (intptr_t)text, 0, 0, 0);
	}

	if (command == 1)
	{
		return;
	}

	unsigned long elapsed = g_CurrentTime - m_lastChatTime;

	if (m_bSwearPenalty)
	{
		if (elapsed > 60000)
		{
			m_bSwearPenalty = false;
			m_swearCount = 0;
		}
		else
		{
			CChatMsg::Instance()->AddChatMsg(
				"(\xbe\xcb\xb8\xb2)1\xba\xd0\xb0\xa3 \xc3\xa4\xc6\xc3 \xb1\xdd\xc1\xf6\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9",
				0xFFFF7878, true, false);
			return;
		}
	}

	if (m_bFloodPenalty)
	{
		if (elapsed > 60000)
		{
			m_bFloodPenalty = false;
			m_floodCount = 0;
		}
		else
		{
			CChatMsg::Instance()->AddChatMsg(
				"(\xbe\xcb\xb8\xb2)1\xba\xd0\xb0\xa3 \xc3\xa4\xc6\xc3 \xb1\xdd\xc1\xf6\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9",
				0xFFFF7878, true, false);
			return;
		}
	}

	if (!Doc()->m_myInfo.info.IsIdentity(4) &&
		(elapsed < 1000 ||
			(elapsed < 10000 && stricmp(text, lastMessage) == 0)))
	{
		m_floodCount++;
		if (m_floodCount == 3)
		{
			CChatMsg::Instance()->AddChatMsg(
				"(\xbe\xcb\xb8\xb2)\xc3\xa4\xc6\xc3\xb5\xb5\xb9\xe8 \xb9\xfa\xc4\xa2\xbf\xa1 \xb0\xc9\xb8\xae\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xFFFF7878, true, false);
			CChatMsg::Instance()->AddChatMsg(
				"(\xbe\xcb\xb8\xb2)1\xba\xd0\xb0\xa3 \xc3\xa4\xc6\xc3\xb1\xdd\xc1\xf6\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xFFFF7878, true, false);
			m_lastChatTime = g_CurrentTime;
			m_bFloodPenalty = true;

			sPlayerData* player = 0;
			if (!IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
			{
				LogOut(0, "ChatBlock : flood\n");
				player = PLAYER(MyIndex());
			}

			if (OnlinePlay() && player && !player->chatBlock)
			{
				player->chatBlock = true;
				WSendPacket packet((enumClientPacket)0x4f);
				packet.Encode1(1);
				packet.Send(TO_GAME);
			}

			return;
		}
	}
	else
	{
		m_floodCount = 0;
	}

	char message[0x400] = { 0 };
	strcpy(message, text);

	if (!Doc()->m_myInfo.info.IsIdentity(4) && Filtering(message))
	{
		m_swearCount++;
		if (m_swearCount == 5)
		{
			CChatMsg::Instance()->AddChatMsg(
				"(\xbe\xcb\xb8\xb2)5\xc8\xb8\xc0\xcc\xbb\xf3 \xbf\xe5\xbc\xb3\xb4\xdc\xbe\xee\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xFFFF7878, true, false);
			CChatMsg::Instance()->AddChatMsg(
				"(\xbe\xcb\xb8\xb2)1\xba\xd0\xb0\xa3 \xc3\xa4\xc6\xc3\xb1\xdd\xc1\xf6\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xFFFF7878, true, false);
			m_lastChatTime = g_CurrentTime;
			m_bSwearPenalty = true;

			sPlayerData* player = 0;
			if (!IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
			{
				LogOut(0, "ChatBlock : swear\n");
				player = PLAYER(MyIndex());
			}

			if (OnlinePlay() && player && !player->chatBlock)
			{
				player->chatBlock = true;
				WSendPacket packet((enumClientPacket)0x4f);
				packet.Encode1(1);
				packet.Send(TO_GAME);
			}

			return;
		}
	}

	if (broadcast)
	{
		WSendPacket packet((enumClientPacket)0x54);
		packet.EncodeStr(std::string(message));
		packet.Send(TO_GAME);
	}
	else
	{
		if (target)
		{
			if (strcmp(target, "\xb1\xe6\xb5\xe5\xbf\xa1\xb0\xd4") == 0)
			{
				if (WNetworkSystem::Instance()->IsConnected(
						WNetworkSystem::NET_MSN) &&
					CMessengerInfo::Instance()->GetComplete() == 4)
				{
					std::string line(message);

					WSendPacket packet(0x25);
					LogOut(0, "GuildChat : %s\n", message);
					packet.EncodeStr(line.c_str());
					packet.Send(TO_MSN);
				}
				else
				{
					AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3,
						(intptr_t)"\xc1\xf6\xb1\xdd\xc0\xba \xb1\xe6\xb5\xe5 \xb4\xeb\xc8\xad\xb8\xa6 \xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0xFFFFCC00, 0, 0, 0);
				}
			}
			else
			{
				std::string to(target);
				std::string line(message);

				LogOut(0, "Whisper %s : %s\n", target, message);
				WSendPacket packet((enumClientPacket)0x2a);
				packet.EncodeStr(to);
				packet.EncodeStr(line);
				packet.Send(TO_GAME);
			}
		}
		else
		{
			if (Doc()->m_gameMode != 5)
			{
				std::string nickname(Doc()->GetPangyaUserInfo().sNick);
				std::string line(message);

				LogOut(0, "Chat : %s\n", message);
				WSendPacket packet((enumClientPacket)0x03);
				packet.EncodeStr(nickname);
				packet.EncodeStr(line);
				packet.Send(TO_GAME);
			}
		}
	}

	m_lastChatTime = g_CurrentTime;
}

bool ChatManager::Filtering(char* text)
{
	if (text == 0)
	{
		return false;
	}

	int i;
	bool found;
	unsigned short wideWord[0x400];
	unsigned short wideText[0x400];

	found = false;
	int textLen =
		MultiByteToWideChar(0, 0, text, strlen(text) + 1, wideText, 0x400);

	for (i = 0; i < m_chatFilterNum; i++)
	{
		int wordLen = MultiByteToWideChar(0, 0, (char*)m_chatFilter[i],
			strlen((char*)m_chatFilter[i]) + 1, wideWord, 0x400);

		if (BABO_STR::WStrStr(text, wideText, textLen, wideWord, wordLen,
				false))
		{
			found = true;
		}
	}

	FilteringHack(text, true);

	return found;
}

int ChatManager::GetFaceIndex(const char* text)
{
	for (int face = 0; face < 3; face++)
	{
		for (int i = 0; i < 12; i++)
		{
			if (strstr(text, FacialTextTable[face][i]))
			{
				return face;
			}
		}
	}

	return 5;
}

const char* ChatManager::GetMotionName(const char* text, unsigned long type)
{
	char command[64];

	for (int pass = 0; pass < 9; pass++)
	{
		sMotionText* motions = s_motionText[type & 0x3FFFFFF];
		for (int i = 0; i < 14; i++)
		{
			sMotionText* motion = &motions[i];
			if (motion == 0)
			{
				break;
			}

			for (int k = 0; k < 7; k++)
			{
				if (motion->m_command[k] == 0)
				{
					break;
				}

				sprintf(command, "/%s", motion->m_command[k]);
				if (strstr(text, command))
				{
					return motions[i].m_name;
				}
			}
		}
	}

	return 0;
}

void ChatManager::FilteringHack(const char* text, bool encode)
{
	if (text == 0)
	{
		return;
	}

	char hack[2][4] = {
		{ ';',  '"',  '\'', '-'  },
        { 0x11, 0x12, 0x13, 0x14 }
	};
	char* str = (char*)text;
	int len = strlen(str);

	for (int i = 0; i < len; i++)
	{
		if (encode)
		{
			for (int k = 0; k < 4; k++)
			{
				if (str[i] == hack[0][k])
				{
					if (k == 3)
					{
						if (i < len - 1)
						{
							if (str[i + 1] == '-')
							{
								str[i] = hack[1][k];
								str[i + 1] = hack[1][k];
							}
						}
					}
					else
					{
						str[i] = hack[1][k];
					}
				}
			}
		}
		else
		{
			for (int k = 0; k < 4; k++)
			{
				if (str[i] == hack[1][k])
				{
					str[i] = hack[0][k];
				}
			}
		}
	}
}

const char* ChatManager::FilteringNick(char* nick)
{
	if (nick == 0)
	{
		return NICKNAME_ERR_EMPTY;
	}

	if (strlen(nick) < 4 || strlen(nick) > 16)
	{
		return NICKNAME_ERR_LENGTH_LIMIT;
	}

	unsigned short wideNick[0x400];
	int nickLen =
		MultiByteToWideChar(0, 0, nick, strlen(nick) + 1, wideNick, 0x400);

	for (int i = 0; i < nickLen - 1; i++)
	{
		if (wideNick[i] == ' ')
		{
			return NICKNAME_ERR_SPACE;
		}

		if (wideNick[i] == '(')
		{
			for (int j = i + 1; j < nickLen - 1; j++)
			{
				if (wideNick[j] == ')')
				{
					unsigned short emoticon[256] = { 0 };
					unsigned short* start = &wideNick[i + 1];
					std::copy(start, &wideNick[j], emoticon);

					const char* reason = FilteringEmoticon(emoticon);
					if (reason)
					{
						return reason;
					}
				}
			}
		}

		if ((wideNick[i] < 0x21 || wideNick[i] > 0x7E) &&
			(wideNick[i] < 0xAC00 || wideNick[i] > 0xD7A3))
		{
			return NICKNAME_ERR_INVALID_CHAR;
		}
	}

	if (Doc()->GetPangyaUserInfo().dwIdentity & 0x14)
	{
		return 0;
	}

	unsigned short wideWord[0x400];
	for (int i = 0; i < m_nickFilterNum; i++)
	{
		char* word = m_nickFilter[i].word;
		int wordLen =
			MultiByteToWideChar(0, 0, word, strlen(word) + 1, wideWord, 0x400);

		if (m_nickFilter[i].type == 1)
		{
			if (!BABO_STR::WStrCmp(wideNick, nickLen, wideWord, wordLen))
			{
				return NICKNAME_ERR_BAD_WORD;
			}
		}
		else
		{
			if (BABO_STR::WStrStr(nick, wideNick, nickLen, wideWord, wordLen,
					false))
			{
				return NICKNAME_ERR_CONTAINS_BAD_WORD;
			}
		}
	}

	return 0;
}

int ChatManager::FilteringGuildString(const char* text, int minLen, int maxLen,
	bool noSpace)
{
	if (text == 0)
	{
		return 1000;
	}

	int len = strlen(text);
	if (len < minLen || len > maxLen)
	{
		CTask* task = AfxGetTask();
		task->SendMsgToMainActor(MsgObject(0, 0x23,
			(intptr_t)MakeStr(
				"\\c0xffff0000\\c\xbf\xb5\xb9\xae %d - %d \xb1\xdb\xc0\xda\\c0xff000000\\c \xb6\xc7\xb4\xc2 \\c0xffff0000\\c\xc7\xd1\xb1\xdb %d - %d\xb1\xdb\xc0\xda\\c0xff000000\\c\xc0\xc7 \xb1\xe6\xc0\xcc\xb8\xa6 \xb0\xa1\xc1\xae\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
				minLen, maxLen, minLen >> 1, maxLen >> 1),
			0, 0, 0, 0));
		return 0;
	}

	unsigned short wideText[0x400];
	int textLen =
		MultiByteToWideChar(0, 0, text, strlen(text) + 1, wideText, 0x400);

	for (int i = 0; i < textLen - 1; i++)
	{
		if (noSpace)
		{
			if (wideText[i] == ' ')
			{
				return 0x413;
			}
		}

		if (wideText[i] == '\'')
		{
			return 0x414;
		}
	}

	unsigned short wideWord[0x400] = { 0 };
	int wordLen = 0;
	for (int i = 0; i < m_nickFilterNum; i++)
	{
		wordLen = MultiByteToWideChar(0, 0, m_nickFilter[i].word,
			strlen(m_nickFilter[i].word) + 1, wideWord, 0x400);

		if (m_nickFilter[i].type == 1)
		{
			if (!BABO_STR::WStrCmp(wideText, textLen, wideWord, wordLen))
			{
				return 0x415;
			}
		}
		else
		{
			if (BABO_STR::WStrStr((char*)text, wideText, textLen, wideWord,
					wordLen, false))
			{
				return 0x415;
			}
		}
	}

	return 1;
}

const char* ChatManager::FilteringEmoticon(unsigned short* text)
{
	if (!IsLocalContent((localContentType_t)0x81))
	{
		return 0;
	}

	FrEmoticon* emoticon = g_pFresh->GetManager()->GetEmoticon();
	int count = emoticon->GetIconNum();

	std::string alias[2];
	char name[256];
	WideCharToMultiByte(0, 0, text, -1, name, 256, 0, 0);

	if (strstr(name, "-"))
	{
		return "\xbf\xaa\xc0\xa7 \xc0\xcc\xb8\xf0\xc6\xbc\xc4\xdc\xc0\xba \xb4\xd0\xb3\xd7\xc0\xd3\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
	}

	for (int i = 0; i < count; i++)
	{
		if (strcmp(name, emoticon->GetIconName(i)) == 0)
		{
			return 0;
		}

		emoticon->GetAlias(&alias, i);
		if (alias[0] == name)
		{
			return "\xc0\xd4\xb7\xc2\xc7\xcf\xbd\xc5 \xc0\xcc\xb8\xf0\xc6\xbc\xc4\xdc \xc1\xf6\xbd\xc3\xbe\xee\xb4\xc2 \xb4\xd0\xb3\xd7\xc0\xd3\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
		}

		if (alias[1] == name)
		{
			return "\xc0\xd4\xb7\xc2\xc7\xcf\xbd\xc5 \xc0\xcc\xb8\xf0\xc6\xbc\xc4\xdc \xc1\xf6\xbd\xc3\xbe\xee\xb4\xc2 \xb4\xd0\xb3\xd7\xc0\xd3\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
		}
	}

	return 0;
}

void ChatManager::SendChatMessage2(const char* text, const char* target,
	bool broadcast)
{
	int len = strlen(text);

	if (len == 0)
	{
		return;
	}

	int i;
	for (i = 0; i < len; i++)
	{
		if (text[i] != ' ')
		{
			break;
		}
	}

	if (i == len)
	{
		return;
	}

	if (Doc()->GetFlagBlock() & 8)
	{
		WSingleton<CChatMsg>::Instance()->AddChatMsg(
			"(\xbe\xcb\xb8\xb2)\xc3\xa4\xc6\xc3 \xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xc3\xa4\xc6\xc3\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0xFFFF7878, true, false);
		return;
	}

	char lastMessage[0x400];

	if (Doc()->m_chatHistory.size() != 0)
	{
		strcpy(lastMessage, Doc()->m_chatHistory.back().c_str());
	}
	else
	{
		lastMessage[0] = 0;
	}

	RecordMyChatHistory(text);

	int command = CheckChatCommand(text, 0);

	if (command == 1)
	{
		return;
	}

	unsigned long elapsed = g_CurrentTime - m_lastChatTime;

	if (m_bSwearPenalty)
	{
		if (elapsed > 60000)
		{
			m_bSwearPenalty = false;
			m_swearCount = 0;
		}
		else
		{
			std::string notice(
				"\xbe\xcb\xb8\xb2 : 1\xba\xd0\xb0\xa3 \xc3\xa4\xc6\xc3 \xb1\xdd\xc1\xf6\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9");
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3,
				(intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);
			return;
		}
	}

	if (m_bFloodPenalty)
	{
		if (elapsed > 60000)
		{
			m_bFloodPenalty = false;
			m_floodCount = 0;
		}
		else
		{
			std::string notice(
				"\xbe\xcb\xb8\xb2 : 1\xba\xd0\xb0\xa3 \xc3\xa4\xc6\xc3 \xb1\xdd\xc1\xf6\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9");
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3,
				(intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);
			return;
		}
	}

	if (!Doc()->m_myInfo.info.IsIdentity(4) &&
		(elapsed < 1000 ||
			(elapsed < 10000 && stricmp(text, lastMessage) == 0)))
	{
		m_floodCount++;
		if (m_floodCount == 3)
		{
			std::string notice(
				"\xbe\xcb\xb8\xb2 : \xc3\xa4\xc6\xc3\xb5\xb5\xb9\xe8 \xb9\xfa\xc4\xa2\xbf\xa1 \xb0\xc9\xb8\xae\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.");
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3,
				(intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);

			notice =
				"\xbe\xcb\xb8\xb2 : 1\xba\xd0\xb0\xa3 \xc3\xa4\xc6\xc3\xb1\xdd\xc1\xf6\xc0\xd4\xb4\xcf\xb4\xd9.";
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3,
				(intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);

			m_lastChatTime = g_CurrentTime;
			m_bFloodPenalty = true;
			return;
		}
	}
	else
	{
		m_floodCount = 0;
	}

	char message[0x400] = { 0 };
	strcpy(message, text);

	if (!Doc()->m_myInfo.info.IsIdentity(4) && Filtering(message))
	{
		m_swearCount++;
		if (m_swearCount == 5)
		{
			std::string notice(
				"\xbe\xcb\xb8\xb2 : 5\xc8\xb8\xc0\xcc\xbb\xf3 \xbf\xe5\xbc\xb3\xb4\xdc\xbe\xee\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.");
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3,
				(intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);

			notice =
				"\xbe\xcb\xb8\xb2 : 1\xba\xd0\xb0\xa3 \xc3\xa4\xc6\xc3\xb1\xdd\xc1\xf6\xc0\xd4\xb4\xcf\xb4\xd9.";
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3,
				(intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);

			m_lastChatTime = g_CurrentTime;
			m_bSwearPenalty = true;
			return;
		}
	}

	if (broadcast)
	{
		WSendPacket packet((enumClientPacket)0x54);
		packet.EncodeStr(std::string(message));
		packet.Send(TO_GAME);
	}
	else if (target)
	{
		if (strcmp(target, "\xb1\xe6\xb5\xe5\xbf\xa1\xb0\xd4") == 0)
		{
			if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
					WNetworkSystem::NET_MSN) &&
				WSingleton<CMessengerInfo>::Instance()->GetComplete() == 4)
			{
				std::string line(message);

				WSendPacket packet(0x25);
				packet.EncodeStr(line.c_str());
				packet.Send(TO_MSN);
			}
			else
			{
				MsgObject msg(0, 3,
					(intptr_t)"\xc1\xf6\xb1\xdd\xc0\xba \xb1\xe6\xb5\xe5 \xb4\xeb\xc8\xad\xb8\xa6 \xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xFFFFCC00, 0, 0, 0);
				AfxGetTask()->GetActor("Lobby") << msg;
			}
		}
		else
		{
			std::string to(target);
			std::string line(message);

			WSendPacket packet((enumClientPacket)0x2a);
			packet.EncodeStr(std::string(target));
			packet.EncodeStr(std::string(message));
			packet.Send(TO_GAME);
		}
	}
	else
	{
		std::string nickname(MyNick());
		std::string line(message);

		WSendPacket packet((enumClientPacket)0x03);
		packet.EncodeStr(nickname);
		packet.EncodeStr(line);
		packet.Send(TO_GAME);
	}

	m_lastChatTime = g_CurrentTime;
}

int ChatManager::CheckChatCommand(const char* text, int type)
{
	if (text[0] == '/')
	{
		CGMToolkit* toolkit = WSingleton<CGMToolkit>::Instance();

		if (toolkit->IsAdministrator())
		{
			if (toolkit->Command(text))
			{
				return 1;
			}
		}
	}

	if (text[0] == '/')
	{
		char command[0x200];
		const char* args = text + 1;

		GetCharsTillSpace(args, command);

		if (command[0] == 0)
		{
			return 1;
		}

		int i;
		for (i = 0; i < sizeof(ChatCmd) / sizeof(ChatCmd[0]); i++)
		{
			if (stricmp(command, ChatCmd[i].name) == 0)
			{
				break;
			}
		}

		if (i < sizeof(ChatCmd) / sizeof(ChatCmd[0]))
		{
			switch (i)
			{
			case 0:
			{
				int identity = 0;
				char name[22];

				int count = sscanf(args, "%s %d %s", command, &identity, name);

				switch (count)
				{
				case 1:
				{
					WSendPacket packet((enumClientPacket)0x41);
					packet.Encode4(0xFFFFFFFF);
					packet.EncodeStr(
						std::string(Doc()->GetPangyaUserInfo().sNick));
					packet.Send(TO_GAME);
					break;
				}
				case 2:
				{
					WSendPacket packet((enumClientPacket)0x41);
					packet.Encode4(identity);
					packet.EncodeStr(
						std::string(Doc()->GetPangyaUserInfo().sNick));
					packet.Send(TO_GAME);
					break;
				}
				case 3:
				{
					WSendPacket packet((enumClientPacket)0x41);
					packet.Encode4(identity);
					packet.EncodeStr(std::string(name));
					packet.Send(TO_GAME);
					break;
				}
				}
				break;
			}
			case 2:
				if (Doc()->m_myInfo.info.IsIdentity(0x40))
				{
					int rate = 0;

					int count = sscanf(args, "%s %d", command, &rate);

					switch (count)
					{
					case 1:
					{
						WSendPacket packet((enumClientPacket)0x4e);
						packet.Encode4(3);
						packet.Encode1(0);
						packet.Send(TO_GAME);
						break;
					}
					case 2:
						if (rate < 100)
						{
							Display(
								"\xbe\xc6\xc0\xcc\xc5\xdb\xb5\xe5\xb7\xd3 \xc8\xae\xb7\xfc\xc0\xcc \xb1\xe2\xba\xbb(100%)\xba\xb8\xb4\xd9 \xc0\xdb\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbc\xb3\xc1\xa4\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
								type);
						}
						else
						{
							WSendPacket packet((enumClientPacket)0x4e);
							packet.Encode4(3);
							packet.Encode1(1);
							packet.Encode4(rate);
							packet.Send(TO_GAME);
						}
						break;
					default:
						Display(
							MakeStr(
								"\xb4\xd9\xbd\xc3 \xc0\xd4\xb7\xc2\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4. \xbf\xb9)/itemdrop \xb9\xe8\xc0\xb2"),
							type);
						break;
					}
				}
				break;
			}
		}

		return 1;
	}

	return 0;
}

int ChatManager::SaveChatHistory(int index, int type)
{
	if (Doc()->m_chatLineList.size() == 0)
	{
		return 0;
	}

	mkdir("history");
	mkdir("history/chatting");

	char fileName[MAX_PATH];

	if (index < 0)
	{
		index = 1;
		do
		{
			sprintf(fileName, "history/chatting/chat%03d.txt", index++);
		} while (_access(fileName, 0) != -1);
		index--;
	}
	else
	{
		index++;
		sprintf(fileName, "history/chatting/chat%03d.txt", index);
	}

	FILE* fp = fopen(fileName, "wt");
	if (fp == 0)
	{
		return 0;
	}

	for (std::list<sChatLine>::iterator it = Doc()->m_chatLineList.begin();
		it != Doc()->m_chatLineList.end(); ++it)
	{
		fwrite(it->text.c_str(), strlen(it->text.c_str()), 1, fp);
		fwrite("\n", 1, 1, fp);
	}

	fclose(fp);

	Display(
		MakeStr(
			"\xc3\xa4\xc6\xc3 \xb3\xbb\xbf\xeb\xc0\xcc %s\xbf\xa1 \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			fileName),
		type);

	return index;
}

void ChatManager::Display(const char* text, int type)
{
	if (type == 0)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(0, 3, (intptr_t)text, 0xFFFF7878, 0, 0, 0);
	}
	else if (type == 1)
	{
		WSingleton<CChatMsg>::Instance()->AddChatMsg(text, 0xFFFF7878, true,
			false);
	}
}

void ChatManager::RecordTournament(void)
{
	char path[MAX_PATH];

	std::vector<sRivalData>::iterator it;
	for (it = Doc()->m_rivalList.begin(); it != Doc()->m_rivalList.end(); ++it)
	{
		if (it->state == 0)
		{
			return;
		}
	}

	int count = Doc()->m_rivalList.size();
	if (count == 0)
	{
		return;
	}

	mkdir("history");
	mkdir("history/tournament");

	unsigned char index = 1;
	do
	{
		sprintf(path, "history/tournament/result%03d.txt", index++);
	} while (_access(path, 0) != -1);

	FILE* fp = fopen(path, "wt");
	if (fp == 0)
	{
		return;
	}

	fprintf(fp, "\xb9\xe6\xc0\xe5 : %s\n\n", Doc()->m_rivalList[0].nickname);
	fprintf(fp,
		"\xbc\xf8\xc0\xa7     \xb7\xb9  \xba\xa7           \xb4\xeb \xc8\xad \xb8\xed       \xbd\xba\xc4\xda\xbe\xee   \xc6\xce\n");
	fprintf(fp, "--------------------------------------------------------\n");

	for (it = Doc()->m_rivalList.begin(); it != Doc()->m_rivalList.end(); ++it)
	{
		fprintf(fp, " %2d    %s    %s    %3d    %I64d\n",
			it->state == 3 ? 0 : it->rank,
			GetAlignedString(g_LevelTable[it->level].name, 11, 0),
			GetAlignedString(it->nickname, 16, 0), it->totalScore,
			it->totalPang);
	}

	fclose(fp);

	Display(
		MakeStr(
			"30\xc0\xce\xb4\xeb\xc8\xb8 \xb0\xe1\xb0\xfa\xb0\xa1 %s\xbf\xa1 \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			path),
		0);
}

const char* ChatManager::GetAlignedString(const char* text, int width,
	int align)
{
	static char s_alignedString[8][128];
	static int s_alignedIndex = -1;

	if (width > 128)
	{
		return text;
	}

	s_alignedIndex++;

	memset(s_alignedString[s_alignedIndex], ' ', width);

	int offset;
	switch (align)
	{
	case 0:
		offset = 0;
		break;
	case 1:
		offset = (width - strlen(text)) / 2;
		break;
	case 2:
		offset = width - strlen(text);
		break;
	}

	strncpy(s_alignedString[s_alignedIndex] + offset, text, strlen(text));
	s_alignedString[s_alignedIndex][width] = 0;

	return s_alignedString[s_alignedIndex];
}

void ChatManager::RecordMyChatHistory(const char* text)
{
	std::list<std::string>& history = Doc()->m_chatHistory;
	std::list<std::string>::iterator it =
		std::find(history.begin(), history.end(), text);

	if (it != history.end())
	{
		history.erase(it);
	}
	else if (Doc()->m_chatHistory.size() == 20)
	{
		Doc()->m_chatHistory.erase(history.begin());
	}

	history.push_back(text);
}
