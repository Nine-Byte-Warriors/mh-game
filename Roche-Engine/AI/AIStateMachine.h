#pragma once
#ifndef AISTATEMACHINE_H
#define AISTATEMACHINE_H

#include "AIState.h"
#include "AIStateData.h"

namespace AILogic
{
	class AIStateMachine : public AIState
	{
	public:
		AIStateMachine(Agent* fAgent) :	AIState(fAgent)
		{
			m_vecStates.clear();
			m_vecActiveStates.clear();
			m_vecLastActiveStates.clear();
		}
		virtual ~AIStateMachine() = default;

		virtual void UpdateMachine(float fDelta);
		virtual void AddState(AIState* pState);
		virtual bool IsActive(AIState* pState);
		virtual void Reset() { for (int i = 0; i < m_vecStates.size(); i++)	m_vecStates[i]->Exit();	}

		void Clear() { if(m_vecStates.size() > 0) m_vecStates.clear(); } // temporary. should be done differently

		AIState* NewState(AILogic::AIStateTypes eType);
		AIState* NewState(AIStateData::AIStateJson jState);

	protected:
		std::vector<AIState*> m_vecStates;
		std::vector<AIState*> m_vecActiveStates;
		std::vector<AIState*> m_vecLastActiveStates;
	};
}

#endif // AISTATEMACHINE_H