#pragma once
#ifndef AIFLEE_H
#define AIFLEE_H

#include "AIState.h"
#include "Entity.h"
#include "Agent.h"
#include "AIStateData.h"

namespace AILogic
{
    class AIFlee : public AIState
    {
    public:
		AIFlee(Agent* pAgent) : AIState(pAgent) {};
		virtual ~AIFlee() = default;

		virtual void Update(const float dt) override;

        virtual float CalculateActivation() override;
        inline virtual void SetTarget(Entity* pTarget) { m_pTarget = pTarget; }
        
        void SetParams(AIStateData::FleeParams params);
        
    private:
        Entity* m_pTarget;
        float m_fMinRange;
		float m_fMaxRange;
    };
}

#endif // !AIFLEE_H