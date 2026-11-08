#include "frreadytoall.h"
#include "shareddoc.h"

IMPLEMENT_OBJECT(FrReadyToAll, FrForm)

BEGIN_FRESH_MSGMAP(FrReadyToAll, FrForm)

ON_FRESH_VV("ReadyStart", FRCMD_LBUTTONUP, FrReadyToAll::OnReadyBtnUp)
ON_FRESH_VV("WaitCancel", FRCMD_LBUTTONUP, FrReadyToAll::OnWaitBtnUp)

END_FRESH_MSGMAP()

FrReadyToAll::FrReadyToAll()
{
}

FrReadyToAll::~FrReadyToAll()
{
}

void FrReadyToAll::OnReadyBtnUp()
{
	if (Doc()->IsControlServerService(2))
		return;

	WSendPacket send((enumClientPacket)13);
	send.Encode1(0);
	send.Send(TO_GAME);

	Close(true);
}

void FrReadyToAll::OnWaitBtnUp()
{
	Close(true);
}

IMPLEMENT_OBJECT(FrReadyToMaster, FrForm)

BEGIN_FRESH_MSGMAP(FrReadyToMaster, FrForm)

ON_FRESH_VV("ReadyStart", FRCMD_LBUTTONUP, FrReadyToMaster::OnStartBtnUp)
ON_FRESH_VV("WaitCancel", FRCMD_LBUTTONUP, FrReadyToMaster::OnExitBtnUp)

END_FRESH_MSGMAP()

FrReadyToMaster::FrReadyToMaster()
{
}

FrReadyToMaster::~FrReadyToMaster()
{
}

void FrReadyToMaster::OnStartBtnUp()
{
	if (Doc()->IsControlServerService(2))
		return;

	WSendPacket send((enumClientPacket)14);
	send.Encode4(MyGuid(false));
	send.Send(TO_GAME);

	Close(true);
}

void FrReadyToMaster::OnExitBtnUp()
{
	Close(true);
}

void FrReadyToMaster::OnProc(const float dt)
{
}
