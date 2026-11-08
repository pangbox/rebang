#include "sequencestatemachine.h"

void CStateUnit::OnPrintLog(const char* log)
{
	// Compiled out
}

CSequenceStateMachine::CSequenceStateMachine(const void* pOwner,
	const char* name)
	: m_lastOrder(0), m_pOwner(pOwner), m_name(name)
{
	m_current = m_states.end();
	m_unknown34 = 0;
	m_unknown38 = 0;
}

CSequenceStateMachine::~CSequenceStateMachine()
{
	Clear();
}

void CSequenceStateMachine::PrintLog(const CStateUnit* pUnit, const char* log)
{
	// Compiled out
}

void CSequenceStateMachine::PrintLog(const char* format, ...)
{
	// Compiled out
}

bool CSequenceStateMachine::Process(float delta)
{
	CStateUnit* pUnit = GetCurrentState();
	if (pUnit)
	{
		pUnit->OnProcess(delta);
		if (pUnit->OnFinished())
		{
			++m_current;

			PrintLog("%s -> %s", pUnit->GetClassName(),
				GetCurrentState() ? GetCurrentState()->GetClassName() : "");
		}

		return true;
	}
	return false;
}

void CSequenceStateMachine::Render()
{
	CStateUnit* pUnit = GetCurrentState();
	if (pUnit && !pUnit->OnFinished())
		pUnit->OnRender();
}

void CSequenceStateMachine::Clear()
{
	std::list<CStateUnit*>::iterator it = m_states.begin();
	while (it != m_states.end())
	{
		delete *it;
		++it;
	}
	m_states.clear();
}

void CSequenceStateMachine::Reset()
{
	m_current = m_states.begin();
}

void CSequenceStateMachine::Insert(int order, CStateUnit* pUnit)
{
	pUnit->m_order = order;

	std::list<CStateUnit*>::iterator it =
		std::upper_bound(m_states.begin(), m_states.end(), *pUnit);

	pUnit->SetOwner(this);
	m_states.insert(it, pUnit);

	PrintLog("Insert %s", pUnit->GetClassName());

	if (m_states.size() == 1)
		m_current = m_states.begin();
}

void CSequenceStateMachine::BackInsert(CStateUnit* pUnit)
{
	pUnit->m_order = ++m_lastOrder;
	pUnit->SetOwner(this);
	m_states.push_back(pUnit);

	PrintLog("BackInsert %s", pUnit->GetClassName());

	if (m_states.size() == 1)
		m_current = m_states.begin();
}

CStateUnit* CSequenceStateMachine::GetCurrentState()
{
	if (m_current != m_states.end())
		return *m_current;

	return NULL;
}

CStateUnit* CSequenceStateMachine::GetPreviousState()
{
	CStateUnit* pUnit = NULL;
	if (m_current != m_states.begin())
	{
		pUnit = *--m_current;
		++m_current;
	}
	return pUnit;
}

CStateUnit* CSequenceStateMachine::GetNextState()
{
	CStateUnit* pUnit = NULL;
	if (m_current != m_states.end())
	{
		if (++m_current != m_states.end())
		{
			pUnit = *m_current;
			--m_current;
		}
	}
	return pUnit;
}

bool CSequenceStateMachine::SkipCurrentState()
{
	if (GetCurrentState())
	{
		CStateUnit* pUnit = GetCurrentState();
		++m_current;

		PrintLog("%s -> %s", pUnit->GetClassName(),
			GetCurrentState() ? GetCurrentState()->GetClassName() : "");
	}
	return m_current != m_states.end();
}
