#include "stdafx.h"
#include "Shooting.h"

Shooting::Shooting()
{
	AddToEvent();
}

Shooting::~Shooting()
{
	EventSystem::Instance()->RemoveClient(EVENTID::PlayerPosition, this);
	EventSystem::Instance()->RemoveClient(EVENTID::MousePosition, this);
	EventSystem::Instance()->RemoveClient(EVENTID::PlayerFire, this);
}

void Shooting::Shoot(Vector2f Targetloc)
{
	std::shared_ptr<Projectile> pProjectile;

	if (pProjectile != nullptr)
		pProjectile->SpawnProjectile(Targetloc, mousePos,1);
}


void Shooting::AddToEvent() noexcept
{
	EventSystem::Instance()->AddClient(EVENTID::PlayerPosition, this);
	EventSystem::Instance()->AddClient(EVENTID::MousePosition, this);
	EventSystem::Instance()->AddClient(EVENTID::PlayerFire, this);
}

void Shooting::HandleEvent(Event* event)
{
	switch (event->GetEventID())
	{
	case EVENTID::PlayerPosition:
		charSpriteandPos = static_cast<std::pair<Sprite*, Vector2f*>*>(event->GetData());
		charSprite = charSpriteandPos->first;
		m_vPosition = XMFLOAT2(charSpriteandPos->second->x + charSprite->GetWidth(), charSpriteandPos->second->y + charSprite->GetHeight());

		pos = Vector2f(m_vPosition.x, m_vPosition.y);
		break;
	case EVENTID::MousePosition:
		mousePos = *static_cast<Vector2f*>(event->GetData());
		targetpos = Vector2f(mousePos.x + 200, mousePos.y - 450);
		break;
	case EVENTID::PlayerFire: Shoot(pos);
		
		break;
	default:
		break;
	}
}


