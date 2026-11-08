#include "finitestatemachine.h"

FiniteStateMachine::FiniteStateMachine()
	: m_pCurrentState(NULL)
{
}

FiniteStateMachine::~FiniteStateMachine()
{
	std::map<unsigned long, FiniteState*>::iterator it = m_stateMap.begin();

	while (it != m_stateMap.end())
	{
		delete it->second;
		m_stateMap.erase(it++);
	}
}

void FiniteStateMachine::addStateTransition(unsigned long stateID,
	unsigned long input, unsigned long outputID)
{
	FiniteState* pState = NULL;

	std::map<unsigned long, FiniteState*>::iterator it = m_stateMap.begin();
	while (it != m_stateMap.end())
	{
		pState = it->second;

		if (pState->getStateID() == stateID)
			break;

		it++;
	}

	if (it == m_stateMap.end())
	{
		pState = new FiniteState(stateID);
		m_stateMap[pState->getStateID()] = pState;
	}

	pState->addTransition(input, outputID);
}

void FiniteStateMachine::deleteTransition(unsigned long stateID,
	unsigned long input)
{
	std::map<unsigned long, FiniteState*>::iterator itDel;
	FiniteState* pState = NULL;

	std::map<unsigned long, FiniteState*>::iterator it = m_stateMap.begin();
	while (it != m_stateMap.end())
	{
		pState = it->second;
		itDel = it;
		if (pState->getStateID() == stateID)
			break;

		it++;
	}

	if (it == m_stateMap.end())
		return;

	pState->deleteTransition(input);
	if (pState->getCount() == 0)
	{
		delete pState;

		m_stateMap.erase(itDel);
	}
}

unsigned long FiniteStateMachine::getOutputState(unsigned long input)
{
	FiniteState* pState = m_stateMap[m_pCurrentState->getStateID()];

	return pState->outputState(input);
}

void FiniteStateMachine::setCurrentState(unsigned long stateID)
{
	std::map<unsigned long, FiniteState*>::iterator it;

	it = m_stateMap.find(stateID);

	m_pCurrentState = it->second;
}

unsigned long FiniteStateMachine::getCurrentStateID()
{
	return m_pCurrentState->getStateID();
}

void FiniteStateMachine::stateTransition(int input)
{
	unsigned long outputID = m_pCurrentState->outputState(input);

	m_pCurrentState = m_stateMap[outputID];
}
