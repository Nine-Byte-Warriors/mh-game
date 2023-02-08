#include "stdafx.h"
#include "PlayerMovement.h"

PlayerMovement::PlayerMovement(const std::shared_ptr<Physics>& physics, const std::shared_ptr<Sprite>& sprite, float speed)
{
	m_pPhysics = physics;
	m_pSprite = sprite;
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
	Vector2f* pos = new Vector2f(m_pPhysics->GetTransform()->GetPosition());
	std::pair<Sprite*, Vector2f*>* charSpriteandPos = new std::pair<Sprite*, Vector2f*>();
	charSpriteandPos->first = m_pSprite.get();
	charSpriteandPos->second = pos;
	EventSystem::Instance()->AddEvent(EVENTID::PlayerPosition, charSpriteandPos);

	// Set idle animation if not moving
	switch ( m_eMoveDirection )
	{
	case Direction::Up:
	case Direction::Left:
	case Direction::Down:
	case Direction::Right:
		m_pSprite->SetShouldUpdate( false );
		break;
	}
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
	case EVENTID::MoveUp:
		if(m_bshouldMove)
			m_pPhysics->AddForce({ 0, -10 });
		m_eMoveDirection = Direction::Up;
		m_pSprite->SetShouldUpdate( true );
		m_pSprite->SetCurFrameY(3);
		break;
	case EVENTID::MoveLeft:
		if (m_bshouldMove)
			m_pPhysics->AddForce({ -10,0 });
		m_eMoveDirection = Direction::Left;
		m_pSprite->SetShouldUpdate( true );
		m_pSprite->SetCurFrameY(1);
		break;
	case EVENTID::MoveDown:
		if (m_bshouldMove)
			m_pPhysics->AddForce({ 0, 10 });
		m_eMoveDirection = Direction::Down;
		m_pSprite->SetShouldUpdate( true );
		m_pSprite->SetCurFrameY(0);
		break;
	case EVENTID::MoveRight:
		if (m_bshouldMove)
			m_pPhysics->AddForce({ 10,0 });
		m_eMoveDirection = Direction::Right;
		m_pSprite->SetShouldUpdate( true );
		m_pSprite->SetCurFrameY(2);
		break;
	case EVENTID::PlayerDash:
		Dash();
		break;
	case EVENTID::TogglePlayerMovement:
		m_bshouldMove = !m_bshouldMove;
		break;
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
