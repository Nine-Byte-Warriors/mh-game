#include "stdafx.h"
#include "AIWander.h"
#include "AIStateData.h"

using namespace AILogic;

void AIWander::Update(const float dt)
{
	// Get agent position from agent's GameObject
	Vector2f vAgentPos = m_pAgent->GetPhysics()->GetTransform()->GetPosition();

	Vector2f vVelocity = m_pAgent
		->GetPhysics()
		->GetVelocity()
		.Normalised();

	// Get the direction from the target
	float fNonZeroAngle = m_fWanderAngle == 0 
		? RND::Getf() 
		: RND::Getf(-m_fWanderAngle, m_fWanderAngle);

	Vector2f vDirection = DelayCount(dt)
		? vVelocity.Rotate(fNonZeroAngle)
		: vVelocity;

	// Apply the directional force and speed
	m_pAgent->GetPhysics()->AddForce(vDirection * m_pAgent->GetSpeed());

	// Update the physics model
	m_pAgent->GetPhysics()->Update(dt);
}

float AIWander::CalculateActivation()
{
	// Get agent position from agent's GameObject
	Vector2f vAgentPos = m_pAgent->GetPhysics()->GetTransform()->GetPosition();

	// Get target position
	Vector2f vTargetPosition = m_pAgent->GetTargetPosition();

	// Get the distance from the target
	float fDistance = vTargetPosition.Distance(vAgentPos);

	// Calculate the activation
	if (fDistance <= m_fRange)
		return 0.0f;

	float fActivation = 1.0f - (m_fRange / fDistance);

	// Clamp the activation
	fActivation = fActivation < GetLower() ? GetLower() : fActivation;
	fActivation = fActivation > GetUpper() ? GetUpper() : fActivation;

	return fActivation;
}


void AIWander::SetParams(AIStateData::WanderParams params)
{
	m_fRange = params.fRange;
	m_fWanderAngle = params.fWanderAngle;
	m_fMaxDelay = params.fWanderDelay;
}

bool AIWander::DelayCount(const float dt)
{
	m_fDelayCount -= dt;

	if (m_fDelayCount <= 0.0f)
	{
		m_fDelayCount = m_fMaxDelay;
		return true;
	}

	return false;
}