#include "stdafx.h"
#include "AIStateMachine.h"
#include "AISeek.h"
#include "AIIdle.h"
#include "AIFlee.h"
#include "AIPatrol.h"
#include "AIFollow.h"
#include "AIWander.h"
#include "AIFire.h"

using namespace AILogic;

void AIStateMachine::UpdateMachine(float fDelta)
{
    if (m_vecStates.empty()) return;

    // Store active states from previous iteration
    m_vecLastActiveStates = m_vecActiveStates;

    // Clear list of active states
    m_vecActiveStates.clear();

	for (AIState* pState : m_vecStates)
	{
        if (pState->CalculateActivation() < pState->m_fActivationLevel)
            pState->Exit();
        else
			m_vecActiveStates.push_back(pState);
	}
	
    if (!m_vecActiveStates.empty()) 
    {
        for (AIState* pState : m_vecActiveStates)
            if(IsActive(pState))
				pState->Update(fDelta);
            else
				pState->Enter();
    }
}

void AIStateMachine::AddState(AIState* pStateToAdd)
{
    for (AIState* pState : m_vecStates)
		if (pState == pStateToAdd) return;
	
    m_vecStates.push_back(pStateToAdd);
}

bool AIStateMachine::IsActive(AIState* pStateToAdd)
{
	for (AIState* pState : m_vecLastActiveStates)
		if (pState == pStateToAdd) return true;

	return false;
}

AIState* AIStateMachine::NewState(AILogic::AIStateTypes eType)
{
	switch (eType)
	{
		case AILogic::AIStateTypes::Idle:	return new AIIdle(m_pAgent);
		case AILogic::AIStateTypes::Seek:	return new AISeek(m_pAgent);
		case AILogic::AIStateTypes::Flee:	return new AIFlee(m_pAgent);
		case AILogic::AIStateTypes::Patrol: return new AIPatrol(m_pAgent);
		case AILogic::AIStateTypes::Follow: return new AIFollow(m_pAgent);
		case AILogic::AIStateTypes::Wander: return new AIWander(m_pAgent);
		case AILogic::AIStateTypes::Fire:	return new AIFire(m_pAgent);
		default: return nullptr;
	}
}

AIState* AIStateMachine::NewState(AIStateData::AIStateJson jState)
{
	AIState* pState = nullptr;

	switch (jState.iStateType)
	{
	case AIStateTypes::Idle:
	{
		pState = (AIState*) new AIIdle(m_pAgent);
		break;
	}
	case AIStateTypes::Seek: 
	{
		AISeek* pSeek = new AISeek(m_pAgent);
		pSeek->SetParams(jState.oSeekParams);
		pState = (AIState*)pSeek;
		break;
	}
	case AIStateTypes::Flee:
	{
		AIFlee* pFlee = new AIFlee(m_pAgent);
		pFlee->SetParams(jState.oFleeParams);
		pState = (AIState*)pFlee;
		break;
	}
	case AIStateTypes::Patrol: 
	{
		AIPatrol* pPatrol = new AIPatrol(m_pAgent);
		pPatrol->SetParams(jState.oPatrolParams);
		pState = (AIState*)pPatrol;
		break;
	}
	case AIStateTypes::Follow: return new AIFollow(m_pAgent);
	{
		AIFollow* pFollow = new AIFollow(m_pAgent);
		pFollow->SetParams(jState.oFollowParams);
		pState = (AIState*)pFollow;
		break;
	}
	case AIStateTypes::Wander:
	{
		AIWander* pWander = new AIWander(m_pAgent);
		pWander->SetParams(jState.oWanderParams);
		pState = (AIState*)pWander;
	}
	case AIStateTypes::Fire: 
	{
		AIFire* pFire = new AIFire(m_pAgent);
		pFire->SetParams(jState.oFireParams);
		pState = (AIState*)pFire;
	}
	default: return nullptr;
	}

	pState->SetActivation(jState.fActivate);
	pState->SetBounds(jState.fMax, jState.fMin);
	return pState;
}