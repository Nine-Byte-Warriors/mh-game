#include "stdafx.h"
#include "Agent.h"
#include "AIStateData.h"

using namespace AILogic;

Agent::Agent(const std::shared_ptr<Physics>& physics) : m_pPhysics(physics)
{
	m_fSpeed = 5.0f;

	m_pStateMachine = new AIStateMachine(this);

	m_sBehaviourFile = "";

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
		AIState* pState = m_pStateMachine->NewState(jState);

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
			//m_vTargetPos = *static_cast<Vector2f*>(event->GetData());
		}

		break;
	}
	case EVENTID::PlayerPosition:
		//if (!m_bTargetMouse)
		{
			std::pair < Sprite*, Vector2f*>* dPair = (std::pair<Sprite*, Vector2f*>*)(event->GetData());
			Vector2f vSpriteHalfSize = Vector2f(dPair->first->GetWidthHeight() / 2);
			//m_vTargetPos = *dPair->second + vSpriteHalfSize;
			Vector2f vSpriteSize = dPair->first->GetWidthHeight();
			//m_vTargetPos = *dPair->second + vSpriteSize;
			m_vTargetPos = *dPair->second;
		}
		break;
	default:
		break;
	}
}