#include "invitedlg.h"

IMPLEMENT_OBJECT(FrInviteDlg, FrForm)

void FrInviteDlg::Init()
{
	m_svrGUID = 0xffffffff;
	m_channelIdx = 0xff;
	m_roomIdx = 0xffff;

	m_fromUID = 0xffffffff;
	m_toUID = 0xffffffff;
}

void FrInviteDlg::SetRoomInfo(unsigned short roomIdx)
{
	m_roomIdx = roomIdx;
}

void FrInviteDlg::SetInvaiteInfo(unsigned long svrGUID,
	unsigned char channelIdx, unsigned short roomIdx, unsigned long fromUID,
	unsigned long toUID)
{
	m_svrGUID = svrGUID;
	m_channelIdx = channelIdx;
	m_roomIdx = roomIdx;
	m_fromUID = fromUID;
	m_toUID = toUID;
}
