#include "finitestate.h"

FiniteState::FiniteState(unsigned long stateID)
	: m_stateID(stateID)
{
}

FiniteState::~FiniteState()
{
	std::map<unsigned long, unsigned long>::iterator it =
		m_transitionMap.begin();

	while (it != m_transitionMap.end())
	{
		m_transitionMap.erase(it++);
	}
}

void FiniteState::addTransition(unsigned long input, unsigned long outputID)
{
	m_transitionMap[input] = outputID;
}

void FiniteState::deleteTransition(unsigned long input)
{
	m_transitionMap.erase(input);
}

unsigned long FiniteState::outputState(unsigned long input)
{
	std::map<unsigned long, unsigned long>::iterator it;

	it = m_transitionMap.find(input);
	if (it == m_transitionMap.end())

		return m_stateID;

	return m_transitionMap[input];
}

unsigned long FiniteState::getCount()
{
	return m_transitionMap.size();
}
