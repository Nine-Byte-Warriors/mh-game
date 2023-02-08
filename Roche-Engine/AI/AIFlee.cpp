#include "stdafx.h"
#include "AIFlee.h"

using namespace AILogic;

void AIFlee::Update(const float dt)
{
	// Get agent position from agent's GameObject
	Vector2f vAgentPos = m_pAgent->GetPhysics()->GetTransform()->GetPosition();
	
	// Get target position
	Vector2f vTargetPosition = m_pAgent->GetTargetPosition();

	// Get the direction from the target
	Vector2f vDirection = vTargetPosition
		.DirectionTo(vAgentPos)
		.Normalised();

	// Apply the directional force and speed
	m_pAgent->GetPhysics()->AddForce(vDirection * m_pAgent->GetSpeed() );
	
	// Update the physics model
	m_pAgent->GetPhysics()->Update( dt );
}

float AIFlee::CalculateActivation() 
{
	// Get agent position from agent's GameObject
	Vector2f vAgentPos = m_pAgent->GetPhysics()->GetTransform()->GetPosition();

	// Get target position
	Vector2f vTargetPosition = m_pAgent->GetTargetPosition();

	// Get the distance from the target
	float fDistance = vTargetPosition.Distance(vAgentPos);

	// Calculate the activation
	if (fDistance <= m_fMinRange || fDistance >= m_fMaxRange)
		return 0.0f;

	float fRange = m_fMaxRange - m_fMinRange;

	float fActivation = 1.0f - (fRange / fDistance - m_fMinRange);

	// Clamp the activation
	fActivation = fActivation < GetLower() ? GetLower() : fActivation;
	fActivation = fActivation > GetUpper() ? GetUpper() : fActivation;

	return fActivation;
}
void AIFlee::SetParams(AIStateData::FleeParams params)
{
	m_fMinRange = params.fMinRange;
	m_fMaxRange = params.fMaxRange;
}
