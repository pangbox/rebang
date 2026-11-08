#include "networkunit.h"

NetworkUnit::NetworkUnit()
{
	m_fsm.addStateTransition(0, 1, 2);
	m_fsm.addStateTransition(0, 2, 4);

	m_fsm.addStateTransition(1, 0, 2);
	m_fsm.addStateTransition(1, 2, 1);
	m_fsm.addStateTransition(1, 3, 1);

	m_fsm.addStateTransition(2, 0, 3);
	m_fsm.addStateTransition(2, 3, 5);

	m_fsm.addStateTransition(5, 0, 2);

	m_fsm.addStateTransition(3, 0, 0);

	m_fsm.addStateTransition(4, 2, 1);

	Init();
}

NetworkUnit::~NetworkUnit()
{
}

bool NetworkUnit::Init()
{
	m_fsm.setCurrentState(1);
	m_bConnectionComplete = FALSE;
	m_socket.Close();
	return true;
}

void NetworkUnit::setTransition(unsigned long state)
{
	if (state == 0 && m_fsm.getCurrentStateID() == 1)
		m_bConnectionComplete = FALSE;

	m_fsm.stateTransition(state);
}

void NetworkUnit::setForceTransition(unsigned long state)
{
	m_fsm.setCurrentState(state);
}

unsigned long NetworkUnit::getState()
{
	return m_fsm.getCurrentStateID();
}

bool NetworkUnit::Process()
{
	unsigned long state = getState();

	switch (state)
	{
	case 0:
	{
		return OnLine();
	}
	break;

	case 1:
	{
		return OffLine();
	}
	break;

	case 2:
	{
		return Connect();
	}
	break;

	case 3:
	{
		return Connecting();
	}
	break;

	case 4:
	{
		return Disconnecting();
	}
	break;

	case 5:
	{
		ResetConnect();
	}
	break;

	default:
		break;
	}

	return true;
}

int NetworkUnit::IsConnected()
{
	return m_socket.m_bConnected;
}

int NetworkUnit::IsConnectionComplete()
{
	return m_bConnectionComplete;
}

bool NetworkUnit::ResetConnect()
{
	return true;
}
