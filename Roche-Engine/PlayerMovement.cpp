#include "stdafx.h"
#include "PlayerMovement.h"

void PlayerMovement::Initilise(EntityController& entityController, float speed, int entityNum, std::shared_ptr<Physics> &physics, std::shared_ptr<Sprite> &sprite)
{
	m_iEntityNum = entityNum;
	m_entityController = &entityController;
	m_physics = physics;
	m_sprite = sprite;
	m_speed = speed;
	m_currentSpeed = m_speed;
	m_bDash = false;

#if _DEBUG
	m_bshouldMove = false;
#else
	m_bshouldMove = true;
#endif

	AddToEvent();
}

PlayerMovement::~PlayerMovement()
{
	EventSystem::Instance()->RemoveClient(EVENTID::MoveUp, this);
	EventSystem::Instance()->RemoveClient(EVENTID::MoveLeft, this);
	EventSystem::Instance()->RemoveClient(EVENTID::MoveDown, this);
	EventSystem::Instance()->RemoveClient(EVENTID::MoveRight, this);
	EventSystem::Instance()->RemoveClient(EVENTID::PlayerDash, this);
	EventSystem::Instance()->RemoveClient(EVENTID::TogglePlayerMovement, this);
}

void PlayerMovement::Update(const float dt)
{

	m_entityController->SetAnimationType("Hit", m_iEntityNum);

	Vector2f* pos = new Vector2f(m_physics->GetTransform()->GetPosition());
	std::pair<Sprite*, Vector2f*>* charSpriteandPos = new std::pair<Sprite*, Vector2f*>();

	charSpriteandPos->first = m_physics->GetTransform()->GetSprite().get();
	charSpriteandPos->second = pos;

	EventSystem::Instance()->AddEvent(EVENTID::PlayerPosition, charSpriteandPos);
}

void PlayerMovement::SetSpeed(float speed)
{
	m_speed = speed;
}

void PlayerMovement::Dash()
{
	m_bDash = true;
}

void PlayerMovement::HandleEvent(Event* event)
{
	switch (event->GetEventID())
	{
	case EVENTID::MoveUp: if(m_bshouldMove) m_physics->AddForce({ 0, -1 * m_speed });
		m_entityController->SetAnimationType("Up", m_iEntityNum);
		break;
	case EVENTID::MoveLeft: if (m_bshouldMove) m_physics->AddForce({ -1 * m_speed, 0 });
		//m_sprite->SetCurFrameY(PlayerDirection::Left);
		m_entityController->SetAnimationType("Left", m_iEntityNum);
		break;
	case EVENTID::MoveDown: if (m_bshouldMove) m_physics->AddForce({ 0, 1 * m_speed});
		//m_sprite->SetCurFrameY(PlayerDirection::Down);
		m_entityController->SetAnimationType("Down", m_iEntityNum);
		break;
	case EVENTID::MoveRight: if (m_bshouldMove) m_physics->AddForce({ 1 * m_speed, 0 });
		//m_sprite->SetCurFrameY(PlayerDirection::Right);
		m_entityController->SetAnimationType("Right", m_iEntityNum);
		break;
	case EVENTID::PlayerDash: Dash();
		break;
	case EVENTID::TogglePlayerMovement:
		m_bshouldMove = !m_bshouldMove;
		break;
	default: break;
	}
}

void PlayerMovement::AddToEvent() noexcept
{
	EventSystem::Instance()->AddClient(EVENTID::MoveUp, this);
	EventSystem::Instance()->AddClient(EVENTID::MoveLeft, this);
	EventSystem::Instance()->AddClient(EVENTID::MoveDown, this);
	EventSystem::Instance()->AddClient(EVENTID::MoveRight, this);
	EventSystem::Instance()->AddClient(EVENTID::PlayerDash, this);
	EventSystem::Instance()->AddClient(EVENTID::TogglePlayerMovement, this);
}
