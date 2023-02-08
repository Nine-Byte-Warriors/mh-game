#include "stdafx.h"
#include "AISeek.h"

using namespace AILogic;

void AISeek::Update(const float dt)
{
	// Get agent position from agent's GameObject
	Vector2f vAgentPos = m_pAgent->GetPhysics()->GetTransform()->GetPosition();

	// Get target position
	Vector2f vTargetPosition = m_pAgent->GetTargetPosition();

	// Get the direction to the target
	Vector2f vDirection = vAgentPos.DirectionTo(vTargetPosition);

	// Normalize the direction
	Vector2f vNormDir = vDirection.Normalised();
	
	// Apply the directional force and speed
	m_pAgent->GetPhysics()->AddForce( vNormDir * m_pAgent->GetSpeed() );
	
	// Update the physics model
	m_pAgent->GetPhysics()->Update( dt );
}

float AISeek::CalculateActivation()
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

	float fActivation = -1.0f*(fRange / fDistance - m_fMinRange);

	// Clamp the activation
	fActivation = fActivation < GetLower() ? GetLower() : fActivation;
	fActivation = fActivation > GetUpper() ? GetUpper() : fActivation;

	return fActivation;
}

void AISeek::SetParams(AIStateData::SeekParams params)
{
	m_fMinRange = params.fMinRange;
	m_fMaxRange = params.fMaxRange;
}