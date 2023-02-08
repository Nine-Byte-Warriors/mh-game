#include "stdafx.h"
#include "AIFire.h"

using namespace AILogic;

void AIFire::Update(const float dt)
{
	m_fTimer -= dt;
	
	if (m_fTimer < 0.0f)
	{
		m_fTimer = m_fRate;
		
		m_pEmitter->SetSpawnPosition(m_pAgent->GetPhysics()->GetTransform()->GetPosition());
		m_pEmitter->SetTargetPosition(m_pAgent->GetTargetPosition());
		m_pEmitter->EmitProjectile();
	}
}

void AIFire::Enter()
{
	m_pEmitter = m_pAgent->GetEmitter();
}

float AIFire::CalculateActivation()
{
	// Get agent position from agent's GameObject
	Vector2f vAgentPos = m_pAgent->GetPhysics()->GetTransform()->GetPosition();

	// Get target position
	Vector2f vTargetPosition = m_pAgent->GetTargetPosition();

	// Get the distance from the target
	float fDistance = vTargetPosition.Distance(vAgentPos);

	// return the activation
	return fDistance <= m_fRange ? 1.0f : 0.0f;
}

void AIFire::SetParams(AIStateData::FireParams params)
{
	m_fRange = params.fRange;
	m_fRate = params.fRate;
}