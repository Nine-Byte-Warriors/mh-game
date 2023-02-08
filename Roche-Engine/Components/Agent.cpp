#include "stdafx.h"
#include "Agent.h"
#include "AIStateData.h"

#if _DEBUG
#include <imgui/imgui.h>
#endif

using namespace AILogic;

Agent::Agent(const std::shared_ptr<Physics>& physics) : m_pPhysics(physics)
{
	m_fSpeed = 5.0f;

	m_pStateMachine = new AIStateMachine(this);

	m_sBehaviourFile = "";

	AIState* pSeekState = m_pStateMachine->NewState(AIStateTypes::Seek);
	pSeekState->SetBounds(1.0f, 0.0f);
	pSeekState->SetActivation(0.0f);
	m_mapStates.emplace(AIStateTypes::Seek, pSeekState);

	AIState* pIdleState = m_pStateMachine->NewState(AIStateTypes::Idle);
	pIdleState->SetActivation(1.0f);
	m_mapStates.emplace(AIStateTypes::Idle, pIdleState);

	AIState* pFleeState = m_pStateMachine->NewState(AIStateTypes::Flee);
	pFleeState->SetBounds(1.0f, 0.0f);
	pFleeState->SetActivation(0.0f);
	m_mapStates.emplace(AIStateTypes::Flee, pFleeState);

	AIStateData::PatrolParams* patrolParams = new AIStateData::PatrolParams();
	patrolParams->fDistanceToWaypoint = 200.0f;
	patrolParams->fSensingRange = 10.0f;
	patrolParams->iWaypointCount = 6;
	patrolParams->ePatrolType = PatrolType::Loop;

	AIState* pPatrolState = m_pStateMachine->NewState(AIStateTypes::Patrol);
	pPatrolState->SetBounds(1.0f, 0.0f);
	pPatrolState->SetActivation(0.0f);
	pPatrolState->SetParams((void*)patrolParams);
	m_mapStates.emplace(AIStateTypes::Patrol, pPatrolState);

	AIStateData::FollowParams* followParams = new AIStateData::FollowParams();
	followParams->bKeepRange = true;
	followParams->fFollowRange = 200.0f;
	followParams->fRepulseRange = 100.0f;

	AIState* pFollowState = m_pStateMachine->NewState(AIStateTypes::Follow);
	pFollowState->SetBounds(1.0f, 0.0f);
	pFollowState->SetActivation(0.0f);
	pFollowState->SetParams(followParams);
	m_mapStates.emplace(AIStateTypes::Follow, pFollowState);

	AIStateData::WanderParams* pWanderParams = new AIStateData::WanderParams();
	pWanderParams->fWanderAngle = 5.0f;
	pWanderParams->fWanderDelay = 0.5f;

	AIState* pWanderState = m_pStateMachine->NewState(AIStateTypes::Wander);
	pWanderState->SetBounds(1.0f, 0.0f);
	pWanderState->SetActivation(0.0f);
	pWanderState->SetParams(pWanderParams);
	m_mapStates.emplace(AIStateTypes::Wander, pWanderState);

	AIStateData::FireParams* pFireParams = new AIStateData::FireParams();
	pFireParams->fRange = 200.0f;
	pFireParams->fRate = 0.5f;

	AIState* pFireState = m_pStateMachine->NewState(AIStateTypes::Fire);
	pFireState->SetBounds(1.0f, 0.0f);
	pFireState->SetActivation(0.0f);
	pFireState->SetParams(pFireParams);
	m_mapStates.emplace(AIStateTypes::Fire, pFireState);

	AddToEvent();
}

void Agent::Update(const float dt)
{
	if (!HasBehaviourFile())
		FillStateMachine();
		
	m_pStateMachine->UpdateMachine(dt);
}

void Agent::SetBehaviour(AILogic::AIStateTypes behaviour)
{
	m_bTargetMouse = false;
	m_fSpeed = 5.0f;
	ResetBehaviour();

	m_mapStates.find(behaviour)->second->SetActivation(1.0f);
}

void Agent::LoadBehaviourFile(const std::string sFilePath)
{
	if (sFilePath.empty())
		return;

	m_sBehaviourFile = sFilePath;

	std::vector<AIStateData::AIStateJson> vecJsonStates;

	JsonLoading::LoadJson(vecJsonStates, m_sBehaviourFile);

	if (vecJsonStates.empty()) { m_sBehaviourFile = ""; return; }

	ClearStates();
	
	for (AIStateData::AIStateJson jState : vecJsonStates)
	{
		AIState* pState = m_pStateMachine->NewState(jState.iStateType);
		pState->SetActivation(jState.fActivate);
		pState->SetBounds(jState.fMax, jState.fMin);

		switch (jState.iStateType)
		{
			case AIStateTypes::Patrol:	pState->SetParams((void*)&jState.oPatrolParams);	break;
			case AIStateTypes::Follow:	pState->SetParams((void*)&jState.oFollowParams);	break;
			case AIStateTypes::Wander:	pState->SetParams((void*)&jState.oWanderParams);	break;
			case AIStateTypes::Fire:	pState->SetParams((void*)&jState.oFireParams);		break;
		}

		m_mapStates.emplace(jState.iStateType, pState);
	}
}

void Agent::ClearStates()
{
	for (auto state : m_mapStates)
		delete state.second;
	
	m_mapStates.clear();
}

void Agent::ResetBehaviour()
{
	for (std::pair<AIStateTypes, AILogic::AIState*> state : m_mapStates)
		if(state.second != nullptr) state.second->SetActivation(0.0f);
}

void Agent::FillStateMachine()
{
	m_pStateMachine->Clear();
	for (auto const& [key, value] : m_mapStates)
		m_pStateMachine->AddState(value);
}

void Agent::AddToEvent() noexcept
{
	EventSystem::Instance()->AddClient(EVENTID::MouseCameraPosition, this);
	EventSystem::Instance()->AddClient(EVENTID::PlayerPosition, this);
}

void Agent::RemoveFromEvent() noexcept
{
	EventSystem::Instance()->RemoveClient(EVENTID::MouseCameraPosition, this);
	EventSystem::Instance()->RemoveClient(EVENTID::PlayerPosition, this);
}

void Agent::HandleEvent(Event* event)
{
	switch (event->GetEventID())
	{
	case EVENTID::MouseCameraPosition:
	{
		if (m_bTargetMouse)
		{
			m_vTargetPos = *static_cast<Vector2f*>(event->GetData());
		}

		break;
	}
	case EVENTID::PlayerPosition:
		if (!m_bTargetMouse)
		{
			std::pair < Sprite*, Vector2f*>* dPair = (std::pair<Sprite*, Vector2f*>*)(event->GetData());
			m_vTargetPos = *dPair->second + Vector2f(dPair->first->GetWidthHeight() / 2);
		}
		break;
	default:
		break;
	}
}