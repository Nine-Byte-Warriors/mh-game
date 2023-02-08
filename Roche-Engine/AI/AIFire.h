#pragma once
#ifndef AIFIRE_H
#define AIFIRE_H

#include "AIState.h"
#include "Agent.h"
#include "Emitter.h"
#include "AIStateData.h"

namespace AILogic
{
	class AIFire : public AIState
	{
	public:
		AIFire(Agent* pAgent) :
			AIState(pAgent), 
			m_pEmitter(nullptr),
			m_fRange(100.0f),
			m_fRate(1.0f),
			m_fTimer(0.0f)
		{};
		virtual ~AIFire() = default;

		virtual void Update(const float dt) override;
		virtual void Enter() override;

		virtual float CalculateActivation() override;

		void SetParams(AIStateData::FireParams params);

	private:
		float m_fRange;
		float m_fRate;
		float m_fTimer;
		
		std::shared_ptr<Emitter> m_pEmitter;
	};
}

#endif // !AIFIRE_H
